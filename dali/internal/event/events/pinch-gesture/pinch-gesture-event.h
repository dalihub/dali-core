#ifndef DALI_INTERNAL_EVENT_PINCH_GESTURE_EVENT_H
#define DALI_INTERNAL_EVENT_PINCH_GESTURE_EVENT_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/events/gesture-event.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{
namespace Internal
{
/**
 * If the adaptor detects a pinch gesture, then it should create an instance of this structure and
 * send it to the Core.
 *
 * A Pinch Gesture event should be in one of four states:
 * - STARTED:    If a pinch is detected.
 * - CONTINUING: If after a pinch is detected, it continues.
 * - FINISHED:   If after a pinch, the user lifts their finger(s).
 * - CANCELLED:  If there is a system interruption.
 */
struct PinchGestureEvent : public GestureEvent
{
  // Construction & Destruction

  /**
   * Default Constructor
   * @param[in]  state  The state of the gesture
   */
  PinchGestureEvent(GestureState state);

  /**
   * Virtual destructor
   */
  ~PinchGestureEvent() override;

  // Data

  /**
   * @copydoc Dali::PinchGesture::scale
   */
  float scale;

  /**
   * @copydoc Dali::PinchGesture::speed
   */
  float speed;

  /**
   * The center point between the two touch points of the last touch event in the series of touch motion
   * event producing this gesture.
   */
  Vector2 centerPoint;
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_EVENT_PINCH_GESTURE_EVENT_H
