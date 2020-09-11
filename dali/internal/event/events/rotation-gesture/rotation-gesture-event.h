#ifndef DALI_INTERNAL_EVENT_ROTATION_GESTURE_EVENT_H
#define DALI_INTERNAL_EVENT_ROTATION_GESTURE_EVENT_H

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
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/vector2.h>
#include <dali/internal/event/events/gesture-event.h>

namespace Dali
{

namespace Internal
{

/**
 * When a rotation gesture is detected, this structure holds information regarding the gesture.
 *
 * A ROTATION Gesture event should be in one of four states:
 * - STARTED:    If a rotation is detected.
 * - CONTINUING: If after a rotation is detected, it continues.
 * - FINISHED:   If after a rotation, the user lifts their finger(s).
 * - CANCELLED:  If there is a system interruption.
 */
struct RotationGestureEvent : public GestureEvent
{
  // Construction & Destruction

  /**
   * Default Constructor
   * @param[in]  state  The state of the gesture
   */
  RotationGestureEvent( GestureState state )
  : GestureEvent( GestureType::ROTATION, state )
  {
  }

  /**
   * Virtual destructor
   */
  ~RotationGestureEvent() override = default;

  // Data

  /**
   * @copydoc Dali::RotationGesture::rotation
   */
  Radian rotation;

  /**
   * @copydoc Dali::RotationGesture::screenCenterPoint
   */
  Vector2 centerPoint;
};

} // namespace Integration

} // namespace Dali

#endif // DALI_INTERNAL_EVENT_ROTATION_GESTURE_EVENT_H
