#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDER_MESSAGE_DISPATCHER_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDER_MESSAGE_DISPATCHER_H

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

// INTERNAL INCLUDES
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/update/common/scene-graph-buffers.h>

namespace Dali
{
namespace Internal
{
class MessageBase;

namespace Render
{
class RenderTracker;
}

namespace SceneGraph
{
class RenderManager;
class RenderQueue;

/**
 * A utility class for sending messages to the render-thread.
 */
class RenderMessageDispatcher
{
public:
  /**
   * Constructor
   */
  RenderMessageDispatcher(RenderManager& renderManager, RenderQueue& renderQueue, const SceneGraphBuffers& buffers);

  /**
   * Destructor
   */
  virtual ~RenderMessageDispatcher();

  /**
   * Add a Renderer.
   * @param[in] renderer The renderer to add.
   * @post renderer ownership is transferred.
   */
  void AddRenderer(const Render::RendererKey& renderer);

  /**
   * Remove a Renderer.
   * @param[in] renderer The renderer to remove.
   * @post renderer will be destroyed in the next Render.
   */
  void RemoveRenderer(const Render::RendererKey& renderer);

  /**
   * Add a Render tracker.
   * @param[in] renderTracker The render tracker to add.
   * @post ownership is transferred
   */
  void AddRenderTracker(Render::RenderTracker& renderTracker);

  /**
   * Remove a Render tracker.
   * @param[in] renderTracker The render tracker to add.
   * @post render tracker will be destroyed in the next Render pass.
   */
  void RemoveRenderTracker(Render::RenderTracker& renderTracker);

  /**
   * Return the render manager.
   * @return A reference to the render manager.
   */
  RenderManager& GetRenderManager();

  /**
   * Reserve space for another message in the queue.
   * @param[in] size The message size with respect to the size of type "char".
   * @return A pointer to the first char allocated for the message.
   */
  uint32_t* ReserveMessageSlot(std::size_t size);

private:
  RenderManager& mRenderManager;
  RenderQueue&   mRenderQueue;

  const SceneGraphBuffers& mBuffers;
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_RENDER_MESSAGE_DISPATCHER_H
