#ifndef __DALI_INTERNAL_DISCARD_QUEUE_H__
#define __DALI_INTERNAL_DISCARD_QUEUE_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/ref-object.h>
#include <dali/devel-api/common/owner-container.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/update/nodes/node-declarations.h>
#include <dali/internal/update/rendering/scene-graph-texture-set.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

class RenderQueue;
class Shader;
class Camera;


/**
 * DiscardQueue is used to cleanup nodes & resources when no longer in use.
 * Unwanted objects are added here during UpdateManager::Update().
 * When added, messages will be sent to clean-up GL resources in the next Render.
 * The Update for frame N+1 may occur in parallel with the rendering of frame N.
 * Therefore objects queued for destruction in frame N, are destroyed frame N+2.
 */
class DiscardQueue
{
public:

  typedef OwnerContainer< Shader* > ShaderQueue;
  typedef OwnerContainer< Renderer* > RendererQueue;
  typedef OwnerContainer< Camera* > CameraQueue;

  /**
   * Create a new DiscardQueue.
   * @param[in] renderQueue Used to send GL clean-up messages for the next Render.
   */
  DiscardQueue( RenderQueue& renderQueue );

  /**
   * Non-virtual destructor; DiscardQueue is not suitable as a base class.
   */
  ~DiscardQueue();

  /**
   * Adds an unwanted Node and its children to the discard queue.
   * If necessary, a message will be sent to clean-up GL resources in the next Render.
   * @pre This method is not thread-safe, and should only be called from the update-thread.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] node The discarded node; DiscardQueue takes ownership.
   */
  void Add( BufferIndex updateBufferIndex, Node* node );

  /**
   * Adds an unwanted shader to the discard queue.
   * A message will be sent to clean-up GL resources in the next Render.
   * @pre This method is not thread-safe, and should only be called from the update-thread.
   * @param[in] bufferIndex The current update buffer index.
   * @param[in] shader The shader to queue; DiscardQueue takes ownership.
   */
  void Add( BufferIndex bufferIndex, Shader* shader );

  /**
   * Adds an unwanted Renderer to the discard queue.
   * A message will be sent to clean up GL resources in the next Render.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] renderer The discarded renderer; DiscardQueue takes ownership.
   */
  void Add( BufferIndex updateBufferIndex, Renderer* renderer );

  /**
   * Adds an unwanted Camera to the discard queue.
   * This is done because Render thread may use Matrices from the camera
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] camera The discarded renderer; DiscardQueue takes ownership.
   */
  void Add( BufferIndex updateBufferIndex, Camera* camera );

  /**
   * Release the nodes which were queued in the frame N-2.
   * @pre This method should be called (once) at the beginning of every Update.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void Clear( BufferIndex updateBufferIndex );

private:

  // Undefined
  DiscardQueue( const DiscardQueue& );

  // Undefined
  DiscardQueue& operator=( const DiscardQueue& rhs );

private:

  RenderQueue& mRenderQueue; ///< Used to send GL clean-up messages for the next Render.

  // Messages are queued here following the current update buffer number
  NodeOwnerContainer           mNodeQueue[2];
  ShaderQueue                  mShaderQueue[2];
  RendererQueue                mRendererQueue[2];
  CameraQueue                  mCameraQueue[2];
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_RESOURCE_DISCARD_QUEUE_H__
