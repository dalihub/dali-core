#ifndef __DALI_INTERNAL_EVENT_TO_UPDATE_H__
#define __DALI_INTERNAL_EVENT_TO_UPDATE_H__

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
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/message.h>
#include <dali/internal/update/common/scene-graph-buffers.h>

namespace Dali
{

namespace Internal
{

/**
 * Abstract interface for accessing update-thread data from the event-thread.
 * Used for queueing messages during the event-thread for the next update.
 * Allows the event-thread to read double-buffered property values.
 */
class EventToUpdate
{
public:

  /**
   * Virtual destructor
   */
  virtual ~EventToUpdate()
  {
  }

  /**
   * Called by the event-thread to signal that FlushQueue will be called
   * e.g. when it has finished event processing.
   */
  virtual void EventProcessingStarted() = 0;

  /**
   * Reserve space for another message in the queue; this must then be initialized by the caller.
   * The message will be read from the update-thread after the next FlushMessages is called.
   * @post Calling this method may invalidate any previously returned slots.
   * @param[in] size The message size with respect to the size of type "char".
   * @param[in] updateScene A flag, when true denotes that the message will cause the scene-graph node tree to require an update.
   * @return A pointer to the first char allocated for the message.
   */
  virtual unsigned int* ReserveMessageSlot( unsigned int size, bool updateScene = true ) = 0;

  /**
   * Retrieve the current event-buffer index.
   */
  virtual BufferIndex GetEventBufferIndex() const  = 0;

  /**
   * Flush the set of messages, which were previously stored with QueueMessage().
   * Calls to this thread-safe method should be minimized, to avoid thread blocking.
   *
   * @return True if there are messages to process.
   */
  virtual bool FlushQueue() = 0;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_EVENT_TO_UPDATE_H__
