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
#include <dali/internal/event/rendering/sampler-impl.h> // Dali::Internal::Sampler

// INTERNAL INCLUDES
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/rendering/scene-graph-sampler.h>

namespace Dali
{
namespace Internal
{

SamplerPtr Sampler::New( )
{
  SamplerPtr sampler( new Sampler() );
  sampler->Initialize();
  return sampler;
}

void Sampler::SetFilterMode( Dali::FilterMode::Type minFilter, Dali::FilterMode::Type magFilter )
{
  if( NULL != mRenderObject )
  {
    SetFilterModeMessage( mEventThreadServices.GetUpdateManager(), *mRenderObject, static_cast< unsigned int >( minFilter ), static_cast< unsigned int >( magFilter ) );
  }
}

void Sampler::SetWrapMode( Dali::WrapMode::Type rWrap, Dali::WrapMode::Type sWrap, Dali::WrapMode::Type tWrap )
{
  if( NULL != mRenderObject )
  {
    SetWrapModeMessage( mEventThreadServices.GetUpdateManager(), *mRenderObject, static_cast< unsigned int >( rWrap ), static_cast< unsigned int >( sWrap ), static_cast< unsigned int >( tWrap ) );
  }
}

SceneGraph::Sampler* Sampler::GetSamplerRenderObject()
{
  return mRenderObject;
}


Sampler::Sampler()
:mEventThreadServices( *Stage::GetCurrent() ),
 mRenderObject( NULL )
{
}

void Sampler::Initialize()
{
  SceneGraph::UpdateManager& updateManager = mEventThreadServices.GetUpdateManager();

  mRenderObject = new SceneGraph::Sampler();
  OwnerPointer< SceneGraph::Sampler > transferOwnership( mRenderObject );
  AddSamplerMessage( updateManager, transferOwnership );
}

Sampler::~Sampler()
{
  if( EventThreadServices::IsCoreRunning() && mRenderObject )
  {
    SceneGraph::UpdateManager& updateManager = mEventThreadServices.GetUpdateManager();
    RemoveSamplerMessage( updateManager, *mRenderObject );
  }
}

} // namespace Internal
} // namespace Dali
