/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/internal/render/renderers/render-sampler.h>
#include <dali/internal/update/manager/update-manager.h>

namespace Dali
{
namespace Internal
{
SamplerPtr Sampler::New()
{
  SamplerPtr sampler(new Sampler());
  sampler->Initialize();
  return sampler;
}

void Sampler::SetFilterMode(Dali::FilterMode::Type minFilter, Dali::FilterMode::Type magFilter)
{
  if(nullptr != mRenderObject)
  {
    SetFilterModeMessage(GetEventThreadServices().GetUpdateManager(), *mRenderObject, static_cast<unsigned int>(minFilter), static_cast<unsigned int>(magFilter));
  }
}

void Sampler::SetWrapMode(Dali::WrapMode::Type rWrap, Dali::WrapMode::Type sWrap, Dali::WrapMode::Type tWrap)
{
  if(nullptr != mRenderObject)
  {
    SetWrapModeMessage(GetEventThreadServices().GetUpdateManager(), *mRenderObject, static_cast<unsigned int>(rWrap), static_cast<unsigned int>(sWrap), static_cast<unsigned int>(tWrap));
  }
}

Render::Sampler* Sampler::GetSamplerRenderObject()
{
  return mRenderObject;
}

Sampler::Sampler()
: EventThreadServicesHolder(EventThreadServices::Get()),
  mRenderObject(nullptr)
{
}

void Sampler::Initialize()
{
  SceneGraph::UpdateManager& updateManager = GetEventThreadServices().GetUpdateManager();

  mRenderObject = new Render::Sampler();
  OwnerPointer<Render::Sampler> transferOwnership(mRenderObject);
  AddSamplerMessage(updateManager, transferOwnership);
}

Sampler::~Sampler()
{
  if(DALI_UNLIKELY(!Dali::Stage::IsCoreThread()))
  {
    DALI_LOG_ERROR("~Sampler[%p] called from non-UI thread! something unknown issue will be happened!\n", this);
  }

  if(DALI_LIKELY(EventThreadServices::IsCoreRunning() && mRenderObject))
  {
    SceneGraph::UpdateManager& updateManager = GetEventThreadServices().GetUpdateManager();
    RemoveSamplerMessage(updateManager, *mRenderObject);
  }
}

} // namespace Internal
} // namespace Dali
