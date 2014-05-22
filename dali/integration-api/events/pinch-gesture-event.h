#ifndef __DALI_INTEGRATION_PINCH_GESTURE_EVENT_H__
#define __DALI_INTEGRATION_PINCH_GESTURE_EVENT_H__

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
#include <dali/integration-api/events/gesture-event.h>
#include <dali/public-api/math/vector2.h>

namespace Dali DALI_IMPORT_API
{

namespace Integration
{

/**
 * If the adaptor detects a pinch gesture, then it should create an instance of this structure and
 * send it to the Core.
 *
 * A Pinch Gesture event should be in one of four states:
 * - Started:    If a pinch is detected.
 * - Continuing: If after a pinch is detected, it continues.
 * - Finished:   If after a pinch, the user lifts their finger(s).
 * - Cancelled:  If there is a system interruption.
 */
struct PinchGestureEvent : public GestureEvent
{
  // Construction & Destruction

  /**
   * Default Constructor
   * @param[in]  state  The state of the gesture
   */
  PinchGestureEvent(Gesture::State state);

  /**
   * Virtual destructor
   */
  virtual ~PinchGestureEvent();

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

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_PINCH_GESTURE_EVENT_H__
