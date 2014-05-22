#ifndef __DALI_INTEGRATION_GESTURE_EVENT_H__
#define __DALI_INTEGRATION_GESTURE_EVENT_H__

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
#include <dali/public-api/events/gesture.h>
#include <dali/integration-api/events/event.h>

namespace Dali DALI_IMPORT_API
{

namespace Integration
{

/**
 * This is the abstract base structure for any gestures that the adaptor detects and wishes to send
 * to the Core.
 */
struct GestureEvent : public Event
{
  // Destruction

  /**
   * Virtual destructor.
   */
  virtual ~GestureEvent();

  // Data

  /**
   * Gesture Type.
   */
  Gesture::Type gestureType;

  /**
   * The state of the gesture.
   */
  Gesture::State state;

  /**
   * The time the gesture took place.
   */
  unsigned int time;

protected:  // Constructors only to be used by derived structures.

  /**
   * This constructor is only used by derived classes.
   * @param[in] gesture       The type of gesture event.
   * @param[in] gestureState  The state of the gesture event.
   */
  GestureEvent(Gesture::Type gesture, Gesture::State gestureState);
};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_GESTURE_EVENT_H__
