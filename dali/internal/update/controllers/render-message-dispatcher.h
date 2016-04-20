#ifndef __DALI_INTERNAL_SCENE_GRAPH_RENDER_MESSAGE_DISPATCHER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_RENDER_MESSAGE_DISPATCHER_H__

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
 *
 */

// INTERNAL INCLUDES
#include <dali/internal/update/common/scene-graph-buffers.h>
#include <dali/internal/render/gl-resources/gpu-buffer.h>
#include <dali/internal/render/renderers/render-property-buffer.h>

namespace Dali
{

namespace Internal
{
class MessageBase;

namespace Render
{
class Renderer;
class RenderTracker;
}
namespace SceneGraph
{

class RenderManager;
class RenderQueue;
class PropertyBufferDataProvider;
/**
 * A utility class for sending messages to the render-thread.
 */
class RenderMessageDispatcher
{
public:

  /**
   * Constructor
   */
  RenderMessageDispatcher( RenderManager& renderManager, RenderQueue& renderQueue, const SceneGraphBuffers& buffers );

  /**
   * Destructor
   */
  virtual ~RenderMessageDispatcher();

  /**
   * Add a Renderer.
   * @param[in] renderer The renderer to add.
   * @post renderer ownership is transferred.
   */
  void AddRenderer( Render::Renderer& renderer );

  /**
   * Remove a Renderer.
   * @param[in] renderer The renderer to remove.
   * @post renderer will be destroyed in the next Render.
   */
  void RemoveRenderer( Render::Renderer& renderer );

  /**
   * Add a Render tracker.
   * @param[in] renderTracker The render tracker to add.
   * @post ownership is transferred
   */
  void AddRenderTracker( Render::RenderTracker& renderTracker );

  /**
   * Remove a Render tracker.
   * @param[in] renderTracker The render tracker to add.
   * @post render tracker will be destroyed in the next Render pass.
   */
  void RemoveRenderTracker( Render::RenderTracker& renderTracker );

private:

  RenderManager& mRenderManager;
  RenderQueue& mRenderQueue;

  const SceneGraphBuffers& mBuffers;
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_RENDERER_DISPATCHER_H__
