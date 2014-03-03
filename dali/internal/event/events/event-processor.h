#ifndef __DALI_INTERNAL_EVENT_PROCESSOR_H__
#define __DALI_INTERNAL_EVENT_PROCESSOR_H__

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
#include <dali/internal/event/events/touch-event-processor.h>
#include <dali/internal/event/events/key-event-processor.h>
#include <dali/internal/event/events/mouse-wheel-event-processor.h>
#include <dali/internal/common/message-buffer.h>

namespace Dali
{

namespace Integration
{
struct Event;
struct GestureEvent;
}

namespace Internal
{

class Stage;
class GestureEventProcessor;
class NotificationManager;

/**
 * The EventProcessor processes any events that are received by Dali.  Such events include
 * touch events, key events, mouse wheel events, and notification events.
 *
 * When the EventProcessor receives an event, it determines its type and passes it on to the
 * appropriate processor.
 */
class EventProcessor
{
public:

  /**
   * Constructor
   * @param[in] stage                  The stage.
   * @param[in] notificationManager    The Notification Manager.
   * @param[in] gestureEventProcessor  The gesture event processor.
   */
  EventProcessor(Stage& stage, NotificationManager& notificationManager, GestureEventProcessor& gestureEventProcessor);

  /**
   * Destructor
   */
  virtual ~EventProcessor();

public:

  /**
   * This function is called by Core when an event is queued.
   * @param[in] event A event to queue.
   */
  void QueueEvent( const Integration::Event& event );

  /**
   * This function is called by Core when events are processed.
   */
  void ProcessEvents();

private:

  /**
   * Helper for QueueEvent()
   */
  void QueueGestureEvent(const Integration::GestureEvent& event);

private:

  NotificationManager&     mNotificationManager;        ///< Processes notification events.
  TouchEventProcessor      mTouchEventProcessor;        ///< Processes touch events.
  GestureEventProcessor&   mGestureEventProcessor;      ///< Processes gesture events.
  KeyEventProcessor        mKeyEventProcessor;          ///< Processes key events.
  MouseWheelEventProcessor mMouseWheelEventProcessor;   ///< Processes mouse wheel events.

  // Allow messages to be added safely to one queue, while processing (iterating through) the second queue.
  MessageBuffer mEventQueue0;        ///< An event queue.
  MessageBuffer mEventQueue1;        ///< Another event queue.
  MessageBuffer* mCurrentEventQueue; ///< QueueEvent() will queue here.
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_EVENT_PROCESSOR_H__

