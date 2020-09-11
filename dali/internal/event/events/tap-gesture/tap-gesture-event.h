#ifndef DALI_INTERNAL_EVENT_TAP_GESTURE_H
#define DALI_INTERNAL_EVENT_TAP_GESTURE_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/vector2.h>
#include <dali/internal/event/events/gesture-event.h>

namespace Dali
{

namespace Internal
{

/**
 * If the adaptor detects a tap gesture, then it should create an instance of this structure and
 * send it to the Core.
 *
 * A Tap Gesture event should be in one of three states:
 * - POSSIBLE:  When the user first puts their finger down - Core needs to hit test the down point so
 *              that a tap (down and up quickly) is also on the same actor.
 * - STARTED:   If a tap is detected (No FINISHED state is expected).
 * - CANCELLED: If, after a down event, no tap is detected, or a system interruption.
 *
 * A STARTED state will be ignored if a POSSIBLE state does not precede it.
 */
struct TapGestureEvent : public GestureEvent
{
  // Construction & Destruction

  /**
   * Default Constructor
   * @param[in]  state  POSSIBLE, denotes down press;
   *                    STARTED, of a tap occurs; and
   *                    CANCELLED, when tap does not occur.
   */
  TapGestureEvent( GestureState state );

  /**
   * Virtual destructor
   */
  ~TapGestureEvent() override;

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

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_EVENT_TAP_GESTURE_H
