#ifndef __DALI_INTEGRATION_TAP_GESTURE_H__
#define __DALI_INTEGRATION_TAP_GESTURE_H__

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
 * If the adaptor detects a tap gesture, then it should create an instance of this structure and
 * send it to the Core.
 *
 * A Tap Gesture event should be in one of three states:
 * - Possible:  When the user first puts their finger down - Core needs to hit test the down point so
 *              that a tap (down and up quickly) is also on the same actor.
 * - Started:   If a tap is detected (No Finished state is expected).
 * - Cancelled: If, after a down event, no tap is detected, or a system interruption.
 *
 * A Started state will be ignored if a Possible state does not precede it.
 */
struct TapGestureEvent : public GestureEvent
{
  // Construction & Destruction

  /**
   * Default Constructor
   * @param[in]  state  Possible, denotes down press;
   *                    Started, of a tap occurs; and
   *                    Cancelled, when tap does not occur.
   */
  TapGestureEvent( Gesture::State state );

  /**
   * Virtual destructor
   */
  virtual ~TapGestureEvent();

  // Data

  /**
   * @copydoc Dali::TapGesture::numberOfTaps
   */
  unsigned int numberOfTaps;

  /**
   * @copydoc Dali::TapGesture::numberOfTouches
   */
  unsigned int numberOfTouches;

  /**
   * This is the point, in screen coordinates, where the tap occurred.
   * If a multi-touch tap, then this should be the centroid of all the touch points.
   */
  Vector2 point;
};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_TAP_GESTURE_H__
