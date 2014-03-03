#ifndef __DALI_INTERNAL_UPDATE_MESSAGE_QUEUE_H__
#define __DALI_INTERNAL_UPDATE_MESSAGE_QUEUE_H__

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
#include <dali/internal/common/event-to-update.h>
#include <dali/internal/update/common/scene-graph-buffers.h>

namespace Dali
{

namespace Integration
{
class RenderController;
}

namespace Internal
{

class MessageBase;

namespace Update
{

/**
 * Used by UpdateManager to receive messages from the event-thread.
 * The event-thread uses the EventToUpdate interface to queue messages for the next update.
 */
class MessageQueue: public EventToUpdate
{
public:

  /**
   * Create a new UpdateMessageQueue.
   * @param[in] renderController After messages are flushed, we request a render from the RenderController.
   * @param[in] sceneGraphBuffers Used to keep track of which buffers are being written or read.
   */
  MessageQueue( Integration::RenderController& renderController,
                const SceneGraph::SceneGraphBuffers& sceneGraphBuffers );

  /**
   * Virtual destructor
   */
  virtual ~MessageQueue();

  // From EventToUpdate interface

  /**
   * @copydoc Dali::Internal::EventToUpdate::EventProcessingStarted()
   */
  virtual void EventProcessingStarted();

  /**
   * @copydoc Dali::Internal::EventToUpdate::ReserveMessageSlot()
   */
  virtual unsigned int* ReserveMessageSlot( std::size_t size, bool updateScene );

  /**
   * @copydoc Dali::Internal::EventToUpdate::GetEventBufferIndex()
   */
  virtual BufferIndex GetEventBufferIndex() const;

  /**
   * @copydoc Dali::Internal::EventToUpdate::FlushQueue()
   */
  virtual bool FlushQueue();

  // Exclusive to UpdateManager

  /**
   * Called once per update; process the previously flushed messages.
   */
  void ProcessMessages();

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

private:

  /**
   * Helper to call Process and destructor on each queued message
   * @param[in] minorQueue The queue to process.
   */
  void ProcessMinorQueue( char* minorQueue );

private:

  struct Impl;
  Impl* mImpl;
};

} // namespace Update

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_UPDATE_MESSAGE_QUEUE_H__
