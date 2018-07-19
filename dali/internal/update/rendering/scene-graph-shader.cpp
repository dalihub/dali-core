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
  // TODO: for now we will use hardcoded binary SPIRV shaders which will replace anything
  // that is passed by the caller
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
        mReflection.emplace_back( ReflectionUniformInfo( CalculateHash( item.name ), false, mGraphicsShader, item ) );

        // update buffer index
        std::get<3>( mReflection.back() ).bufferIndex = i;
      }
    }

    // add samplers
    auto samplers = mGraphicsShader->GetSamplers();
    for( const auto& sampler : samplers )
    {
      mReflection.emplace_back( ReflectionUniformInfo( CalculateHash( sampler.name ), false, mGraphicsShader, sampler ) );
    }

    // check for potential collisions
    std::map<size_t, bool> hashTest;
    bool hasCollisions( false );
    for( auto&& item : mReflection )
    {
      auto hashValue = std::get<0>(item);
      if( hashTest.find( hashValue ) == hashTest.end() )
      {
        hashTest[ hashValue ] = false;
      }
      else
      {
        hashTest[ hashValue ] = true;
        hasCollisions = true;
      }
    }

    // update collision flag for further use
    if( hasCollisions )
    {
      for( auto&& item : mReflection )
      {
        std::get<1>( item ) = hashTest[ std::get<0>( item ) ];
      }
    }
  }
}

bool Shader::GetUniform( const std::string& name, size_t hashedName, Graphics::API::ShaderDetails::UniformInfo& out )
{
  if( mReflection.empty() )
  {
    return false;
  }

  hashedName = !hashedName ? CalculateHash( name ) : hashedName;

  for( ReflectionUniformInfo& item : mReflection )
  {
    if( std::get<0>( item ) == hashedName )
    {
      if( !std::get<1>( item ) || std::get<3>( item ).name == name )
      {
        out = std::get<3>( item );
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
