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
 */
#include <dali/internal/event/actors/actor-renderer-container.h>
#include <dali/internal/event/rendering/renderer-impl.h>
#include <dali/internal/update/manager/update-manager.h>

namespace Dali::Internal
{
RendererContainer::RendererContainer(EventThreadServices& eventThreadServices)
: EventThreadServicesHolder(eventThreadServices)
{
}

uint32_t RendererContainer::Add(const SceneGraph::Node& node, Renderer& renderer, bool blending, DevelBlendEquation::Type blendEquation)
{
  if(blending)
  {
    renderer.SetBlendEquation(blendEquation);
  }

  uint32_t index = 0u;

  // Duplication check
  auto end = mRenderers.end();
  for(auto iter = mRenderers.begin(); iter != end; ++iter, ++index)
  {
    if((*iter).Get() == &renderer)
    {
      return index;
    }
  }
  RendererPtr rendererPtr = RendererPtr(&renderer);
  mRenderers.push_back(rendererPtr);
  AttachRendererMessage(GetEventThreadServices().GetUpdateManager(), node, renderer.GetRendererSceneObject());
  return index;
}

void RendererContainer::Remove(const SceneGraph::Node& node, Renderer& renderer)
{
  auto end = mRenderers.end();
  for(auto iter = mRenderers.begin(); iter != end; ++iter)
  {
    if((*iter).Get() == &renderer)
    {
      mRenderers.erase(iter);
      DetachRendererMessage(GetEventThreadServices(), node, renderer.GetRendererSceneObject());
      break;
    }
  }
}

void RendererContainer::Remove(const SceneGraph::Node& node, uint32_t index)
{
  if(index < mRenderers.size())
  {
    RendererPtr renderer = mRenderers[index];
    DetachRendererMessage(GetEventThreadServices(), node, renderer->GetRendererSceneObject());
    mRenderers.erase(mRenderers.begin() + index);
  }
}

void RendererContainer::RemoveAll(const SceneGraph::Node& node)
{
  for(auto&& renderer : mRenderers)
  {
    DetachRendererMessage(GetEventThreadServices(), node, renderer->GetRendererSceneObject());
  }
  mRenderers.clear();
}

uint32_t RendererContainer::GetCount() const
{
  return static_cast<uint32_t>(mRenderers.size());
}

RendererPtr RendererContainer::GetRendererAt(uint32_t index) const
{
  return index < mRenderers.size() ? mRenderers[index] : nullptr;
}

void RendererContainer::SetBlending(DevelBlendEquation::Type blendEquation)
{
  for(const auto& renderer : mRenderers)
  {
    renderer->SetBlendEquation(static_cast<DevelBlendEquation::Type>(blendEquation));
  }
}

} // namespace Dali::Internal
