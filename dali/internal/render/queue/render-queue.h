#ifndef __DALI_INTERNAL_SCENE_GRAPH_RENDER_QUEUE_H__
#define __DALI_INTERNAL_SCENE_GRAPH_RENDER_QUEUE_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/message-buffer.h>

namespace Dali
{

namespace Internal
{

class MessageBase;

namespace SceneGraph
{

/**
 * Allows messages to be queued for RenderManager, during the scene-graph Update.
 */
class RenderQueue
{
public:

  /**
   * Create a new RenderQueue.
   */
  RenderQueue();

  /**
   * Non-virtual destructor; RenderQueue is not suitable as a base class.
   */
  ~RenderQueue();

  /**
   * Reserve space for another message in the queue; this must then be initialized by the caller.
   * The message will be read from the next render-thread tick.
   * @post Calling this method may invalidate any previously returned slots.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] size The message size with respect to the size of type "char".
   * @return A pointer to the first char allocated for the message.
   */
  unsigned int* ReserveMessageSlot( BufferIndex updateBufferIndex, std::size_t size );

  /**
   * Process the batch of messages, which were queued in the previous update.
   * @pre This message should only be called by RenderManager from within the render-thread.
   * @param[in] bufferIndex The previous update buffer index.
   */
  void ProcessMessages( BufferIndex bufferIndex );

private:

  /**
   * Helper to retrieve the current container.
   * The update-thread queues messages with one container, whilst the render-thread is processing the other.
   * @param[in] bufferIndex The current buffer index.
   * @return The container.
   */
  MessageBuffer* GetCurrentContainer( BufferIndex bufferIndex );

  /**
   * Helper to limit the buffer capacity i.e. after a frame when an extreme number of messages have been sent.
   * @param[in] bufferIndex The current buffer index.
   */
  void LimitBufferCapacity( BufferIndex bufferIndex );

  // Undefined
  RenderQueue( const RenderQueue& );

  // Undefined
  RenderQueue& operator=( const RenderQueue& rhs );

private:

  MessageBuffer* container0; ///< Messages are queued here when the update buffer index == 0
  MessageBuffer* container1; ///< Messages are queued here when the update buffer index == 1
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_RENDER_QUEUE_H__
