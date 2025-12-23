/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/controllers/render-manager-dispatcher.h>

// INTERNAL INCLUDES
#include <dali/internal/render/common/render-manager.h>
#include <dali/internal/render/common/render-tracker.h>
#include <dali/internal/render/renderers/render-renderer.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
RenderManagerDispatcher::RenderManagerDispatcher(RenderManager& renderManager)
: mRenderManager(renderManager)
{
}

RenderManagerDispatcher::~RenderManagerDispatcher() = default;

void RenderManagerDispatcher::AddRenderer(OwnerKeyType<Render::Renderer>& rendererKeyPointer)
{
  mRenderManager.AddRenderer(rendererKeyPointer);
}

void RenderManagerDispatcher::RemoveRenderer(const Render::RendererKey& renderer)
{
  mRenderManager.RemoveRenderer(renderer);
}

void RenderManagerDispatcher::AddRenderTracker(Render::RenderTracker& renderTracker)
{
  mRenderManager.AddRenderTracker(&renderTracker);
}

void RenderManagerDispatcher::RemoveRenderTracker(Render::RenderTracker& renderTracker)
{
  mRenderManager.RemoveRenderTracker(&renderTracker);
}

void RenderManagerDispatcher::SetTextureUpdated(const Render::TextureKey& textureKey)
{
  mRenderManager.SetTextureUpdated(textureKey);
}

RenderManager& RenderManagerDispatcher::GetRenderManager()
{
  return mRenderManager;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
