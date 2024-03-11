/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

// CLASS HEADER
#include <dali/internal/update/rendering/scene-graph-shader.h>

// INTERNAL HEADERS
#include <dali/internal/update/graphics/uniform-buffer-manager.h>
#include <dali/graphics-api/graphics-types.h>
#include <dali/devel-api/common/hash.h>

// EXTERNAL HEADERS
#include <map>
#include <tuple>

#include <dali/integration-api/debug.h>

namespace Dali::Internal::SceneGraph
{

/**
 * List of all default uniforms used for quicker lookup
 */
namespace
{

std::array<size_t, 8> DEFAULT_UNIFORM_HASHTABLE = {
  CalculateHash( std::string("uModelMatrix") ),
  CalculateHash( std::string("uMvpMatrix") ),
  CalculateHash( std::string("uViewMatrix") ),
  CalculateHash( std::string("uModelView") ),
  CalculateHash( std::string("uNormalMatrix") ),
  CalculateHash( std::string("uProjection") ),
  CalculateHash( std::string("uSize") ),
  CalculateHash( std::string("uColor") )
};

/**
 * Helper function to calculate the correct alignment of data for uniform buffers
 * @param dataSize size of uniform buffer
 * @return size of data aligned to given size
 */
inline uint32_t AlignSize(uint32_t dataSize, uint32_t alignSize)
{
  return ((dataSize / alignSize) + ((dataSize % alignSize) ? 1u : 0u)) * alignSize;
}

}

Shader::Shader( Dali::Shader::Hint::Value& hints )
: mController( nullptr ),
  mGraphicsProgram( nullptr ),
  mShaderCache( nullptr ),
  mHints( hints ),
  mConnectionObservers()
{
}

Shader::~Shader()
{
  mConnectionObservers.Destroy( *this );
}

void Shader::Initialize( Graphics::Controller& graphicsController,
                         ShaderCache& shaderCache,
                         UniformBufferManager& uboManager )
{
  mController  = &graphicsController;
  mShaderCache = &shaderCache;
  mUboManager = &uboManager;
}

const Graphics::Program* Shader::GetGraphicsObject() const
{
  return mGraphicsProgram;
}

Graphics::Program* Shader::GetGraphicsObject()
{
  return mGraphicsProgram;
}

void Shader::AddConnectionObserver( ConnectionChangePropagator::Observer& observer )
{
  mConnectionObservers.Add(observer);
}

void Shader::RemoveConnectionObserver( ConnectionChangePropagator::Observer& observer )
{
  mConnectionObservers.Remove(observer);
}

void Shader::SetShaderProgram( Internal::ShaderDataPtr shaderData, bool modifiesGeometry )
{
  // @todo: we should handle non-binary shaders as well in the future
  if (shaderData->GetType() == ShaderData::Type::BINARY)
  {
    mGraphicsProgram = &mShaderCache->GetShader(
      shaderData->GetShaderForStage(ShaderData::ShaderStage::VERTEX),
      shaderData->GetShaderForStage(ShaderData::ShaderStage::FRAGMENT));
  }

  if( mGraphicsProgram )
  {
    BuildReflection();
  }
}

void Shader::DestroyGraphicsObjects()
{
  mGraphicsProgram = nullptr;
}

void Shader::BuildReflection()
{
  if( mGraphicsProgram )
  {
    auto& reflection = mController->GetProgramReflection( *mGraphicsProgram );
    mReflectionDefaultUniforms.clear();
    mReflectionDefaultUniforms.resize( DEFAULT_UNIFORM_HASHTABLE.size() );

    auto uniformBlockCount = reflection.GetUniformBlockCount();

    // add uniform block fields
    for( auto i = 0u; i < uniformBlockCount; ++i )
    {
      Graphics::UniformBlockInfo uboInfo;
      reflection.GetUniformBlock( i, uboInfo );

      // for each member store data
      for( const auto& item : uboInfo.members )
      {
        auto hashValue = CalculateHash( item.name );
        mReflection.emplace_back( ReflectionUniformInfo{ item, hashValue, false } );

        // update buffer index
        mReflection.back().uniformInfo.bufferIndex = i;

        // Update default uniforms
        for( auto j = 0u; j < DEFAULT_UNIFORM_HASHTABLE.size(); ++j )
        {
          if( hashValue == DEFAULT_UNIFORM_HASHTABLE[j] )
          {
            mReflectionDefaultUniforms[j] = mReflection.back();
            break;
          }
        }
      }
    }

    // add samplers
    auto samplers = reflection.GetSamplers();
    for( const auto& sampler : samplers )
    {
      mReflection.emplace_back( ReflectionUniformInfo{ sampler, CalculateHash( sampler.name ), false } );
    }

    // check for potential collisions
    std::map<size_t, bool> hashTest;
    bool                   hasCollisions( false );
    for( auto&& item : mReflection )
    {
      if( hashTest.find( item.hashValue ) == hashTest.end() )
      {
        hashTest[item.hashValue] = false;
      }
      else
      {
        hashTest[item.hashValue] = true;
        hasCollisions            = true;
      }
    }

    // update collision flag for further use
    if( hasCollisions )
    {
      for( auto&& item : mReflection )
      {
        item.hasCollision = hashTest[item.hashValue];
      }
    }

    mUniformBlockMemoryRequirements.blockSize.resize( uniformBlockCount );
    mUniformBlockMemoryRequirements.blockSizeAligned.resize( uniformBlockCount );
    mUniformBlockMemoryRequirements.blockCount           = uniformBlockCount;
    mUniformBlockMemoryRequirements.totalSizeRequired    = 0u;
    mUniformBlockMemoryRequirements.totalCpuSizeRequired = 0u;
    mUniformBlockMemoryRequirements.totalGpuSizeRequired = 0u;

    for( auto i = 0u; i < uniformBlockCount; ++i )
    {
      Graphics::UniformBlockInfo uboInfo;
      reflection.GetUniformBlock( i, uboInfo );
      bool standaloneUniformBlock = ( i == 0 );

      auto     blockSize        = reflection.GetUniformBlockSize( i );
      uint32_t blockAlignment   = mUboManager->GetUniformBlockAlignment( standaloneUniformBlock );
      auto     alignedBlockSize = AlignSize( blockSize, blockAlignment );

      mUniformBlockMemoryRequirements.blockSize[i]        = blockSize;
      mUniformBlockMemoryRequirements.blockSizeAligned[i] = alignedBlockSize;

      mUniformBlockMemoryRequirements.totalSizeRequired += alignedBlockSize;
      mUniformBlockMemoryRequirements.totalCpuSizeRequired += ( standaloneUniformBlock ) ? alignedBlockSize : 0;
      mUniformBlockMemoryRequirements.totalGpuSizeRequired += ( standaloneUniformBlock ) ? 0 : alignedBlockSize;
    }
  }
}

bool Shader::GetUniform( const std::string& name, size_t hashedName, Graphics::UniformInfo& out ) const
{
  if( mReflection.empty() )
  {
    return false;
  }

  hashedName = !hashedName ? CalculateHash( name ) : hashedName;

  for( const ReflectionUniformInfo& item : mReflection )
  {
    if( item.hashValue == hashedName )
    {
      if( !item.hasCollision || item.uniformInfo.name == name )
      {
        out = item.uniformInfo;
        return true;
      }
      else
      {
        return false;
      }
    }
  }
  return false;
}

bool Shader::GetUniform( const std::string& name, size_t hashedName, size_t hashNoArray, Graphics::UniformInfo& out ) const
{
  if( mReflection.empty() )
  {
    return false;
  }
  size_t hash = hashedName;
  std::string_view match=name;

  int arrayIndex=0;
  if(!name.empty() && name.back() == ']')
  {
    auto pos = name.rfind('[');
    if(pos != std::string::npos)
    {
      hash = hashNoArray;
      match=name.substr(0, pos); // Remove subscript
      arrayIndex = atoi(&name[pos+1]);
    }
  }
  for(const ReflectionUniformInfo& item : mReflection)
  {
    if(item.hashValue == hash)
    {
      if(!item.hasCollision || item.uniformInfo.name == match)
      {
        out = item.uniformInfo;
        if(item.uniformInfo.elementCount > 0 &&
            arrayIndex >= int(item.uniformInfo.elementCount))
        {
          DALI_LOG_ERROR("Uniform %s, array index out of bound [%d >= %d]!\n",
                          item.uniformInfo.name.c_str(),
                          int(arrayIndex),
                          int(item.uniformInfo.elementCount));
          return false;
        }
        return true;
      }
      else
      {
        return false;
      }
    }
  }
  return false;
}

const Graphics::UniformInfo* Shader::GetDefaultUniform( DefaultUniformIndex defaultUniformIndex ) const
{
  if(mReflectionDefaultUniforms.empty())
  {
    return nullptr;
  }
  const auto value = &mReflectionDefaultUniforms[static_cast<uint32_t>(defaultUniformIndex)];
  return &value->uniformInfo;
}



} // namespace Dali
