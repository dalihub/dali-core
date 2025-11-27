/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/controllers/render-message-dispatcher.h>

// INTERNAL INCLUDES
#include <dali/internal/common/message.h>
#include <dali/internal/render/common/render-manager.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/renderers/render-renderer.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
RenderMessageDispatcher::RenderMessageDispatcher(RenderManager& renderManager, RenderQueue& renderQueue, const SceneGraphBuffers& buffers)
: mRenderManager(renderManager),
  mRenderQueue(renderQueue),
  mBuffers(buffers)
{
}

RenderMessageDispatcher::~RenderMessageDispatcher() = default;

void RenderMessageDispatcher::AddRenderer(const Render::RendererKey& renderer)
{
  // Message has ownership of renderer while in transit from update -> render
  typedef MessageValue1<RenderManager, Render::RendererKey> DerivedType;

  // Reserve some memory inside the render queue
  uint32_t* slot = mRenderQueue.ReserveMessageSlot(mBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mRenderManager, &RenderManager::AddRenderer, renderer);
}

void RenderMessageDispatcher::RemoveRenderer(const Render::RendererKey& renderer)
{
  using DerivedType = MessageValue1<RenderManager, Render::RendererKey>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mRenderQueue.ReserveMessageSlot(mBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mRenderManager, &RenderManager::RemoveRenderer, renderer);
}

void RenderMessageDispatcher::AddRenderTracker(Render::RenderTracker& renderTracker)
{
  using DerivedType = MessageValue1<RenderManager, Render::RenderTracker*>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mRenderQueue.ReserveMessageSlot(mBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mRenderManager, &RenderManager::AddRenderTracker, &renderTracker);
}

void RenderMessageDispatcher::RemoveRenderTracker(Render::RenderTracker& renderTracker)
{
  using DerivedType = MessageValue1<RenderManager, Render::RenderTracker*>;

  // Reserve some memory inside the render queue
  uint32_t* slot = mRenderQueue.ReserveMessageSlot(mBuffers.GetUpdateBufferIndex(), sizeof(DerivedType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) DerivedType(&mRenderManager, &RenderManager::RemoveRenderTracker, &renderTracker);
}

RenderManager& RenderMessageDispatcher::GetRenderManager()
{
  return mRenderManager;
}

uint32_t* RenderMessageDispatcher::ReserveMessageSlot(std::size_t size)
{
  return mRenderQueue.ReserveMessageSlot(mBuffers.GetUpdateBufferIndex(), size);
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
