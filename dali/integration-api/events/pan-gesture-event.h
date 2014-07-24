#ifndef __DALI_INTEGRATION_PAN_GESTURE_EVENT_H__
#define __DALI_INTEGRATION_PAN_GESTURE_EVENT_H__

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
#include <dali/integration-api/events/gesture-event.h>
#include <dali/public-api/math/vector2.h>

namespace Dali DALI_IMPORT_API
{

namespace Integration
{

/**
 * If the adaptor detects a pan gesture, then it should create an instance of this structure and
 * send it to the Core.
 *
 * A Pan Gesture event should be in one of five states:
 * - Possible:   When the user first puts their finger down - Core needs to hit test the down point.
 * - Started:    If a pan is detected.
 * - Continuing: If after a pan is detected, it continues.
 * - Finished:   If after a pan, the user lifts their finger(s).
 * - Cancelled:  If, after a down event, no pan is detected or a system interruption.
 *
 * A Started state will be ignored if a Possible state does not precede it.
 * Likewise, a Continuing or Finished state will be ignored if a Started state does not precede it.
 */
struct PanGestureEvent: public GestureEvent
{
  // Construction & Destruction

  /**
   * Default Constructor
   * @param[in]  state  The state of the gesture
   */
  PanGestureEvent(Gesture::State state);

  /**
   * Virtual destructor
   */
  virtual ~PanGestureEvent();

  // Data

  /**
   * The previous touch position of the primary touch point in screen coordinates.
   */
  Vector2 previousPosition;

  /**
   * This current touch position of the primary touch point in screen coordinates.
   */
  Vector2 currentPosition;

  /**
   * The time difference between the previous and latest touch motion events (in ms).
   */
  unsigned long timeDelta;

  /**
   * The total number of fingers touching the screen in a pan gesture.
   */
  unsigned int numberOfTouches;
};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_PAN_GESTURE_EVENT_H__
