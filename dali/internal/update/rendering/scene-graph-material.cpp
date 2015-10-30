/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
 */

// CLASS HEADER
#include "scene-graph-material.h"

// INTERNAL HEADERS
#include <dali/public-api/actors/blending.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/devel-api/rendering/material.h>
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

Material::Material()
: mShader( NULL ),
  mBlendColor( NULL ),
  mSamplers(),
  mTextureId(),
  mUniformName(),
  mIsFullyOpaque(),
  mConnectionObservers(),
  mFaceCullingMode( Dali::Material::NONE ),
  mBlendingMode( Dali::BlendingMode::AUTO ),
  mBlendingOptions(), // initializes to defaults
  mBlendPolicy( OPAQUE )
{
  // Observe own property-owner's uniform map
  AddUniformMapObserver( *this );
}

Material::~Material()
{
  mConnectionObservers.Destroy( *this );
}

void Material::SetShader( Shader* shader )
{
  mShader = shader;
  mShader->AddUniformMapObserver( *this );

  // Inform NewRenderer about this shader: (Will force a re-load of the
  // shader from the data providers)
  mConnectionObservers.ConnectionsChanged(*this);
}

Shader* Material::GetShader() const
{
  return mShader;
}

void Material::SetFaceCullingMode( unsigned int faceCullingMode )
{
  mFaceCullingMode = static_cast< Dali::Material::FaceCullingMode >( faceCullingMode );
}

void Material::SetBlendingMode( unsigned int blendingMode )
{
  mBlendingMode = static_cast< BlendingMode::Type >( blendingMode );
}

void Material::PrepareRender( BufferIndex bufferIndex )
{
  mBlendPolicy = OPAQUE;

  switch( mBlendingMode )
  {
    case BlendingMode::OFF:
    {
      mBlendPolicy = OPAQUE;
      break;
    }
    case BlendingMode::ON:
    {
      mBlendPolicy = TRANSPARENT;
      break;
    }
    case BlendingMode::AUTO:
    {
      bool opaque = true;

      // @todo: Change hints for new SceneGraphShader:
      // If shader hint OUTPUT_IS_OPAQUE is enabled, set policy to ALWAYS_OPAQUE
      // If shader hint OUTPUT_IS_TRANSPARENT is enabled, set policy to ALWAYS_TRANSPARENT
      // else test remainder, and set policy to either ALWAYS_TRANSPARENT or USE_ACTOR_COLOR

      if( mShader->GeometryHintEnabled( Dali::ShaderEffect::HINT_BLENDING ) )
      {
        opaque = false;
      }

      if( opaque )
      {
        size_t textureCount( GetTextureCount() );
        for( unsigned int i(0); i<textureCount; ++i )
        {
          if( !mIsFullyOpaque[i] )
          {
            opaque = false;
          }
        }
      }

      mBlendPolicy = opaque ? Material::USE_ACTOR_COLOR : Material::TRANSPARENT;
    }
  }
}

Material::BlendPolicy Material::GetBlendPolicy() const
{
  return mBlendPolicy;
}

void Material::SetBlendingOptions( unsigned int options )
{
  mBlendingOptions.SetBitmask( options );
}

void Material::SetBlendColor( const Vector4& blendColor )
{
  if( mBlendColor )
  {
    *mBlendColor = blendColor;
  }
  else
  {
    mBlendColor = new Vector4( blendColor );
  }
}

Vector4* Material::GetBlendColor() const
{
  return mBlendColor;
}

const BlendingOptions& Material::GetBlendingOptions() const
{
  return mBlendingOptions;
}

void Material::AddTexture( const std::string& name, ResourceId id, Render::Sampler* sampler )
{
  mTextureId.PushBack( id );
  mUniformName.push_back( name );
  mSamplers.PushBack( sampler );
  mIsFullyOpaque.PushBack( false );

  mConnectionObservers.ConnectionsChanged(*this);
}

void Material::RemoveTexture( size_t index )
{
  mTextureId.Erase( mTextureId.Begin()+index );
  mUniformName.erase( mUniformName.begin() + index );
  mSamplers.Erase( mSamplers.Begin()+index );
  mIsFullyOpaque.Erase( mIsFullyOpaque.Begin()+index );
  mConnectionObservers.ConnectionsChanged( *this );
}

void Material::SetTextureImage( size_t index, ResourceId id )
{
  mTextureId[index] = id;
  mConnectionObservers.ConnectionsChanged(*this);
}

void Material::SetTextureSampler( size_t index, Render::Sampler* sampler)
{
  mSamplers[index] = sampler;
  mConnectionObservers.ConnectionsChanged(*this);
}

void Material::SetTextureUniformName( size_t index, const std::string& uniformName)
{
  mUniformName[index] = uniformName;
  mConnectionObservers.ConnectionsChanged(*this);
}

void Material::ConnectToSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
}

void Material::DisconnectFromSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
}

void Material::AddConnectionObserver( ConnectionChangePropagator::Observer& observer )
{
  mConnectionObservers.Add(observer);
}

void Material::RemoveConnectionObserver( ConnectionChangePropagator::Observer& observer )
{
  mConnectionObservers.Remove(observer);
}

void Material::UniformMappingsChanged( const UniformMap& mappings )
{
  // Our uniform map, or that of one of the watched children has changed.
  // Inform connected observers.
  mConnectionObservers.ConnectedUniformMapChanged();
}

void Material::ConnectionsChanged( PropertyOwner& owner )
{
  mConnectionObservers.ConnectionsChanged(*this);
}

void Material::ConnectedUniformMapChanged( )
{
  mConnectionObservers.ConnectedUniformMapChanged();
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
