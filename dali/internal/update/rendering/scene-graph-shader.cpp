/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/graphics-api/graphics-api-shader-details.h>

#include <tuple>
#include <map>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

Shader::Shader( Dali::Shader::Hint::Value& hints )
: mGraphics( nullptr ),
  mGraphicsShader( nullptr ),
  mShaderCache( nullptr ),
  mHints( hints ),
  mConnectionObservers()
{
  AddUniformMapObserver( *this );
}

Shader::~Shader()
{
  mConnectionObservers.Destroy( *this );
}

void Shader::Initialize( Integration::Graphics::Graphics& graphics, ShaderCache& shaderCache )
{
  mGraphics = &graphics;
  mShaderCache = &shaderCache;
}

const Graphics::API::Shader* Shader::GetGfxObject() const
{
  return mGraphicsShader;
}

Graphics::API::Shader* Shader::GetGfxObject()
{
  return mGraphicsShader;
}

void Shader::AddConnectionObserver( ConnectionChangePropagator::Observer& observer )
{
  mConnectionObservers.Add(observer);
}

void Shader::RemoveConnectionObserver( ConnectionChangePropagator::Observer& observer )
{
  mConnectionObservers.Remove(observer);
}

void Shader::UniformMappingsChanged( const UniformMap& mappings )
{
  // Our uniform map, or that of one of the watched children has changed.
  // Inform connected observers.
  mConnectionObservers.ConnectedUniformMapChanged();
}

void Shader::SetShaderProgram( Internal::ShaderDataPtr shaderData, bool modifiesGeometry )
{
  // @todo: we should handle non-binary shaders as well in the future
  if (shaderData->GetType() == ShaderData::Type::BINARY)
  {
    mGraphicsShader = &mShaderCache->GetShader(
      Graphics::API::ShaderDetails::ShaderSource(shaderData->GetShaderForStage(ShaderData::ShaderStage::VERTEX)),
      Graphics::API::ShaderDetails::ShaderSource(shaderData->GetShaderForStage(ShaderData::ShaderStage::FRAGMENT)));
  }

  if( mGraphicsShader )
  {
    BuildReflection();
  }
}

void Shader::DestroyGraphicsObjects()
{
  mGraphicsShader = nullptr;
}

void Shader::BuildReflection()
{
  if( mGraphicsShader )
  {
    auto uniformBlockCount = mGraphicsShader->GetUniformBlockCount();

    // add uniform block fields
    for( auto i = 0u; i < uniformBlockCount; ++i )
    {
      Graphics::API::ShaderDetails::UniformBlockInfo uboInfo;
      mGraphicsShader->GetUniformBlock( i, uboInfo );

      // for each member store data
      for( const auto& item : uboInfo.members )
      {
        mReflection.emplace_back( ReflectionUniformInfo{ CalculateHash( item.name ), false, mGraphicsShader, item } );

        // update buffer index
        mReflection.back().uniformInfo.bufferIndex = i;
      }
    }

    // add samplers
    auto samplers = mGraphicsShader->GetSamplers();
    for( const auto& sampler : samplers )
    {
      mReflection.emplace_back( ReflectionUniformInfo{ CalculateHash( sampler.name ), false, mGraphicsShader, sampler } );
    }

    // check for potential collisions
    std::map<size_t, bool> hashTest;
    bool hasCollisions( false );
    for( auto&& item : mReflection )
    {
      if( hashTest.find( item.hashValue ) == hashTest.end() )
      {
        hashTest[ item.hashValue ] = false;
      }
      else
      {
        hashTest[ item.hashValue ] = true;
        hasCollisions = true;
      }
    }

    // update collision flag for further use
    if( hasCollisions )
    {
      for( auto&& item : mReflection )
      {
        item.hasCollision = hashTest[ item.hashValue ];
      }
    }
  }
}

bool Shader::GetUniform( const std::string& name, size_t hashedName, Graphics::API::ShaderDetails::UniformInfo& out ) const
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

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
