#ifndef DALI_INTERNAL_EVENT_PROCESSOR_H
#define DALI_INTERNAL_EVENT_PROCESSOR_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/events/touch-event-processor.h>
#include <dali/internal/event/events/hover-event-processor.h>
#include <dali/internal/event/events/key-event-processor.h>
#include <dali/internal/event/events/wheel-event-processor.h>
#include <dali/internal/common/message-buffer.h>

namespace Dali
{

namespace Integration
{
struct Event;
}

namespace Internal
{

class Scene;
class GestureEventProcessor;
class NotificationManager;

/**
 * The EventProcessor processes any events that are received by Dali.  Such events include
 * touch events, key events, wheel events, and notification events.
 *
 * When the EventProcessor receives an event, it determines its type and passes it on to the
 * appropriate processor.
 */
class EventProcessor
{
public:

  /**
   * Constructor
   * @param[in] scene                  The scene.
   * @param[in] gestureEventProcessor  The gesture event processor.
   */
  EventProcessor( Scene& scene, GestureEventProcessor& gestureEventProcessor );

  /**
   * Destructor
   */
  virtual ~EventProcessor();

public:

  /**
   * This function is called when an event is queued.
   * @param[in] event A event to queue.
   */
  void QueueEvent( const Integration::Event& event );

  /**
   * This function is called when events are processed.
   */
  void ProcessEvents();

private:

  Scene& mScene;                                        ///< The Scene events are processed for.
  TouchEventProcessor      mTouchEventProcessor;        ///< Processes touch events.
  HoverEventProcessor      mHoverEventProcessor;        ///< Processes hover events.
  GestureEventProcessor&   mGestureEventProcessor;      ///< Processes gesture events.
  KeyEventProcessor        mKeyEventProcessor;          ///< Processes key events.
  WheelEventProcessor      mWheelEventProcessor;        ///< Processes wheel events.

  // Allow messages to be added safely to one queue, while processing (iterating through) the second queue.
  MessageBuffer mEventQueue0;        ///< An event queue.
  MessageBuffer mEventQueue1;        ///< Another event queue.
  MessageBuffer* mCurrentEventQueue; ///< QueueEvent() will queue here.
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_EVENT_PROCESSOR_H

