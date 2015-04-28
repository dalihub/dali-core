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
#include <dali/public-api/shader-effects/material.h>
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/update/effects/scene-graph-sampler.h>
#include <dali/internal/render/data-providers/sampler-data-provider.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/public-api/actors/blending.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

namespace
{
const unsigned int DEFAULT_BLENDING_OPTIONS( BlendingOptions().GetBitmask() );
}

Material::Material()
: mColor( Color::WHITE ),
  mBlendColor( Color::WHITE ),
  mFaceCullingMode(Dali::Material::NONE),
  mBlendingMode(Dali::BlendingMode::AUTO),
  mBlendingOptions( DEFAULT_BLENDING_OPTIONS ),
  mShader(NULL),
  mBlendPolicy(OPAQUE)
{
  // Observe own property-owner's uniform map
  AddUniformMapObserver( *this );
}

Material::~Material()
{
}

void Material::SetShader( Shader* shader )
{
  mShader = shader;
  // Inform NewRenderer about this shader: (Will force a re-load of the
  // shader from the data providers)
  mConnectionObservers.ConnectionsChanged(*this);
}

Shader* Material::GetShader() const
{
  // @todo - Fix this - move shader setup to the Renderer connect to stage...
  return mShader;
}

void Material::AddSampler( Sampler* sampler )
{
  mSamplers.PushBack( sampler );

  sampler->AddConnectionObserver( *this );
  sampler->AddUniformMapObserver( *this );

  mConnectionObservers.ConnectionsChanged(*this);
}

void Material::RemoveSampler( Sampler* sampler )
{
  Vector<Sampler*>::Iterator match = std::find( mSamplers.Begin(), mSamplers.End(), sampler );

  DALI_ASSERT_DEBUG( mSamplers.End() != match );
  if( mSamplers.End() != match )
  {
    sampler->RemoveConnectionObserver( *this );
    sampler->RemoveUniformMapObserver( *this );
    mSamplers.Erase( match );
    mConnectionObservers.ConnectionsChanged(*this);
  }
  else
  {
    DALI_ASSERT_DEBUG( 0 && "Sampler not found" );
  }
}

void Material::PrepareRender( BufferIndex bufferIndex )
{
  mBlendPolicy = OPAQUE;

  // @todo MESH_REWORK Add dirty flags to reduce processing.

  switch(mBlendingMode[bufferIndex])
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

      //  @todo: MESH_REWORK - Change hints for new SceneGraphShader:
      // If shader hint OUTPUT_IS_OPAQUE is enabled, set policy to ALWAYS_OPAQUE
      // If shader hint OUTPUT_IS_TRANSPARENT is enabled, set policy to ALWAYS_TRANSPARENT
      // else test remainder, and set policy to either ALWAYS_TRANSPARENT or USE_ACTOR_COLOR

      if( mShader->GeometryHintEnabled( Dali::ShaderEffect::HINT_BLENDING ) )
      {
        opaque = false;
      }

      if( opaque )
      {
        // Check the material color:
        opaque = ( mColor[ bufferIndex ].a >= FULLY_OPAQUE );
      }

      if( opaque )
      {
        // Require that all affecting samplers are opaque
        unsigned int opaqueCount=0;
        unsigned int affectingCount=0;

        for( Vector<Sampler*>::ConstIterator iter = mSamplers.Begin();
             iter != mSamplers.End(); ++iter )
        {
          const Sampler* sampler = *iter;
          if( sampler != NULL )
          {
            if( sampler->AffectsTransparency( bufferIndex ) )
            {
              affectingCount++;
              if( sampler->IsFullyOpaque( bufferIndex ) )
              {
                opaqueCount++;
              }
            }
          }
        }
        opaque = (opaqueCount == affectingCount);
      }

      mBlendPolicy = opaque ? Material::USE_ACTOR_COLOR : Material::TRANSPARENT;
    }
  }
}

Vector<Sampler*>& Material::GetSamplers()
{
  return mSamplers;
}

Material::BlendPolicy Material::GetBlendPolicy() const
{
  return mBlendPolicy;
}

void Material::SetBlendingOptions( BufferIndex updateBufferIndex, unsigned int options )
{
  mBlendingOptions.Set( updateBufferIndex, options );
}

const Vector4& Material::GetBlendColor(BufferIndex bufferIndex) const
{
  return mBlendColor[bufferIndex];
}

BlendingFactor::Type Material::GetBlendSrcFactorRgb( BufferIndex bufferIndex ) const
{
  BlendingOptions blendingOptions;
  blendingOptions.SetBitmask( mBlendingOptions[ bufferIndex ] );
  return blendingOptions.GetBlendSrcFactorRgb();
}

BlendingFactor::Type Material::GetBlendSrcFactorAlpha( BufferIndex bufferIndex ) const
{
  BlendingOptions blendingOptions;
  blendingOptions.SetBitmask( mBlendingOptions[ bufferIndex ] );
  return blendingOptions.GetBlendSrcFactorAlpha();
}

BlendingFactor::Type Material::GetBlendDestFactorRgb( BufferIndex bufferIndex ) const
{
  BlendingOptions blendingOptions;
  blendingOptions.SetBitmask( mBlendingOptions[ bufferIndex ] );
  return blendingOptions.GetBlendDestFactorRgb();
}

BlendingFactor::Type Material::GetBlendDestFactorAlpha( BufferIndex bufferIndex ) const
{
  BlendingOptions blendingOptions;
  blendingOptions.SetBitmask( mBlendingOptions[ bufferIndex ] );
  return blendingOptions.GetBlendDestFactorAlpha();
}

BlendingEquation::Type Material::GetBlendEquationRgb( BufferIndex bufferIndex ) const
{
  BlendingOptions blendingOptions;
  blendingOptions.SetBitmask( mBlendingOptions[ bufferIndex ] );
  return blendingOptions.GetBlendEquationRgb();
}

BlendingEquation::Type Material::GetBlendEquationAlpha( BufferIndex bufferIndex ) const
{
  BlendingOptions blendingOptions;
  blendingOptions.SetBitmask( mBlendingOptions[ bufferIndex ] );
  return blendingOptions.GetBlendEquationAlpha();
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

void Material::ResetDefaultProperties( BufferIndex updateBufferIndex )
{
  mColor.ResetToBaseValue( updateBufferIndex );
  mBlendColor.ResetToBaseValue( updateBufferIndex );
  mFaceCullingMode.CopyPrevious( updateBufferIndex );

  mBlendingMode.CopyPrevious( updateBufferIndex );
  mBlendingOptions.CopyPrevious( updateBufferIndex );
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
