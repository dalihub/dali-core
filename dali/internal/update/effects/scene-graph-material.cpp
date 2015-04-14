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
#include <dali/internal/render/data-providers/sampler-data-provider.h>
#include <dali/internal/update/effects/scene-graph-sampler.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

Material::Material()
: mColor( Color::WHITE ),
  mBlendColor( Color::WHITE ),
  mFaceCullingMode(Dali::Material::NONE),
  mShader(NULL)
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

Vector<Sampler*>& Material::GetSamplers()
{
  return mSamplers;
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
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
