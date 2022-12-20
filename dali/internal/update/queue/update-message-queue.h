#ifndef DALI_INTERNAL_UPDATE_MESSAGE_QUEUE_H
#define DALI_INTERNAL_UPDATE_MESSAGE_QUEUE_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/update/common/scene-graph-buffers.h>

// EXTERNAL INCLUDES
#include <cstddef>

namespace Dali
{
namespace Integration
{
class RenderController;
}

namespace Internal
{
class MessageBase;

namespace SceneGraph
{
class SceneGraphBuffers;
}

namespace Update
{
/**
 * Used by UpdateManager to receive messages from the event-thread.
 */
class MessageQueue
{
public:
  /**
   * Create a new UpdateMessageQueue.
   * @param[in] renderController After messages are flushed, we request a render from the RenderController.
   * @param[in] sceneGraphBuffers Used to keep track of which buffers are being written or read.
   */
  MessageQueue(Integration::RenderController&       renderController,
               const SceneGraph::SceneGraphBuffers& sceneGraphBuffers);

  /**
   * Destructor
   */
  ~MessageQueue();

  /**
   * Inform the queue that event processing has started
   */
  void EventProcessingStarted();

  /**
   * Reserve space for a message
   * @param[in] size the message size with respect to the size of type 'char'
   * @param[in] updateScene If set to true, denotes that the message will cause the scene graph node tree to require an update
   * @return A pointer to the first char allocated for the message
   */
  uint32_t* ReserveMessageSlot(uint32_t size, bool updateScene);

  /**
   * Flushes the message queue
   * @return true if there are messages to process
   */
  bool FlushQueue();

  // Exclusive to UpdateManager

  /**
   * Called once per update; process the previously flushed messages.
   * @param updateBufferIndex to use
   * @return true if the scene graph node tree is updated
   */
  bool ProcessMessages(BufferIndex updateBufferIndex);

  /**
   * Query whether the queue was empty this frame.
   */
  bool WasEmpty() const;

  /**
   * Query whether the queue contains at least one message that requires that the scene-graph
   * node tree be updated.
   * @return A flag, true if the scene graph needs an update
   */
  bool IsSceneUpdateRequired() const;

  /**
   * Query for the capacity of the current message queue
   */
  std::size_t GetCapacity() const;

private:
  /**
   * Helper to call Process and destructor on each queued message
   * @param[in] minorQueue The queue to process.
   */
  void ProcessMinorQueue(char* minorQueue);

private:
  // Not copyable:
  MessageQueue()                                   = delete;
  MessageQueue(const MessageQueue& rhs)            = delete;
  MessageQueue& operator=(const MessageQueue& rhs) = delete;

private:
  struct Impl;
  Impl* mImpl;
};

} // namespace Update

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_UPDATE_MESSAGE_QUEUE_H
