#ifndef __DALI_INTERNAL_DISCARD_QUEUE_H__
#define __DALI_INTERNAL_DISCARD_QUEUE_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/owner-container.h>
#include <dali/internal/update/nodes/node-declarations.h>
#include <dali/internal/update/node-attachments/node-attachment-declarations.h>
#include <dali/internal/update/modeling/scene-graph-mesh-declarations.h>

namespace Dali
{

namespace Internal
{

class Texture;

namespace SceneGraph
{

class Node;
class RenderQueue;
class Shader;

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

  typedef IntrusivePtr<RefObject> ResourcePointer;
  typedef std::vector<ResourcePointer> ResourceQueue;

  typedef OwnerContainer< Shader* > ShaderQueue;

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
   * Adds an unwanted NodeAttachment to the discard queue.
   * If necessary, a message will be sent to clean-up GL resources in the next Render.
   * @pre This method is not thread-safe, and should only be called from the update-thread.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] attachment The discarded attachment; DiscardQueue takes ownership.
   */
  void Add( BufferIndex updateBufferIndex, NodeAttachment* attachment );

  /**
   * Adds an unwanted mesh resource to the discard queue.
   * A message will be sent to clean-up GL resources in the next Render.
   * @pre This method is not thread-safe, and should only be called from the update-thread.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] mesh The mesh to queue; DiscardQueue takes a reference.
   */
  void Add( BufferIndex updateBufferIndex, Mesh* mesh );

  /**
   * Adds an unwanted shader to the discard queue.
   * A message will be sent to clean-up GL resources in the next Render.
   * @pre This method is not thread-safe, and should only be called from the update-thread.
   * @param[in] bufferIndex The current update buffer index.
   * @param[in] shader The shader to queue; DiscardQueue takes ownership.
   */
  void Add( BufferIndex bufferIndex, Shader* shader );

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

  // Messages are queued here when the update buffer index == 0
  NodeOwnerContainer           mNodeQueue0;
  NodeAttachmentOwnerContainer mAttachmentQueue0;
  ResourceQueue                mResourceQueue0;
  MeshOwnerContainer           mMeshQueue0;
  ShaderQueue                  mShaderQueue0;

  // Messages are queued here when the update buffer index == 1
  NodeOwnerContainer           mNodeQueue1;
  NodeAttachmentOwnerContainer mAttachmentQueue1;
  ResourceQueue                mResourceQueue1;
  MeshOwnerContainer           mMeshQueue1;
  ShaderQueue                  mShaderQueue1;
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_RESOURCE_DISCARD_QUEUE_H__
