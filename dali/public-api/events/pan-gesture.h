#ifndef __DALI_PAN_GESTURE_H__
#define __DALI_PAN_GESTURE_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/vector2.h>

namespace Dali
{
/**
 * @addtogroup dali_core_events
 * @{
 */

/**
 * @brief A PanGesture is emitted when the user moves one or more fingers in a particular direction.
 *
 * This gesture can be in one of three states, when the pan gesture is first detected: "Started";
 * when the pan gesture is continuing: "Continuing"; and finally, when the pan gesture ends:
 * "Finished".
 *
 * A pan gesture will end in the following ways:
 * - User releases the primary finger (the first touch).
 * - User has more fingers on the screen than the maximum specified.
 * - User has less fingers on the screen than the minimum specified.
 * - Cancelled by the system.
 *
 * A pan gesture will continue to be sent to the actor under than initial pan until it ends.
 */
struct DALI_IMPORT_API PanGesture: public Gesture
{
  // Construction & Destruction

  /**
   * @brief Default Constructor.
   */
  PanGesture();

  /**
   * @brief Constructor.
   *
   * @param[in]  state  The state of the gesture
   */
  PanGesture(Gesture::State state);

  /**
   * @brief Copy constructor.
   */
  PanGesture( const PanGesture& rhs );

  /**
   * @brief Assignment operator.
   */
  PanGesture& operator=( const PanGesture& rhs );

  /**
   * @brief Virtual destructor.
   */
  virtual ~PanGesture();

  // Data

  /**
   * @brief The velocity at which the user is moving their fingers.
   *
   * This is represented as a Vector2 and is the pixel movement per millisecond.
   * A positive x value shows that the user is panning to the right, a negative x value means the opposite.
   * A positive y value shows that the user is panning downwards, a negative y values means upwards.
   * This value represents the local coordinates of the actor attached to the PanGestureDetector.
   */
  Vector2 velocity;

  /**
   * @brief This is a Vector2 showing how much the user has panned (dragged) since the last pan gesture or,
   * if the gesture has just started, then the amount panned since the user touched the screen.
   *
   * A positive x value shows that the user is panning to the right, a negative x value means the opposite.
   * A positive y value shows that the user is panning downwards, a negative y value means upwards.
   * This value is in local actor coordinates, the actor being the one attached to the
   * PanGestureDetector.
   */
  Vector2 displacement;

  /**
   * @brief This current touch position of the primary touch point in local actor coordinates.
   */
  Vector2 position;

  /**
   * @brief The velocity at which the user is moving their fingers.
   *
   * This is represented as a Vector2 and is the pixel movement per millisecond.
   * A positive x value shows that the user is panning to the right, a negative x value means the opposite.
   * A positive y value shows that the user is panning downwards, a negative y values means upwards.
   * This value represents the screen coordinates.
   */
  Vector2 screenVelocity;

  /**
   * @brief This is a Vector2 showing how much the user has panned (dragged) since the last pan gesture or,
   * if the gesture has just started, then the amount panned since the user touched the screen.
   *
   * A positive x value shows that the user is panning to the right, a negative x value means the opposite.
   * A positive y value shows that the user is panning downwards, a negative y value means upwards.
   * This value is in screen coordinates.
   */
  Vector2 screenDisplacement;

  /**
   * @brief This current touch position of the primary touch point in screen coordinates.
   */
  Vector2 screenPosition;

  /**
   * @brief The total number of fingers touching the screen in a pan gesture.
   */
  unsigned int numberOfTouches;

  // Convenience Methods

  /**
   * @brief Returns the speed at which the user is moving their fingers.
   *
   * This is the pixel movement per millisecond.
   * @return The speed of the pan (in pixels per millisecond).
   */
  float GetSpeed() const;

  /**
   * @brief This returns the distance the user has panned (dragged) since the last pan gesture or,
   * if the gesture has just started, then the distance moved since the user touched the screen.
   *
   * This is always a positive value.
   * @return The distance, as a float, a user's finger has panned.
   */
  float GetDistance() const;

  /**
   * @brief Returns the speed at which the user is moving their fingers relative to screen coordinates.
   *
   * This is the pixel movement per millisecond.
   * @return The speed of the pan (in pixels per millisecond).
   */
  float GetScreenSpeed() const;

  /**
   * @brief This returns the distance the user has panned (dragged) since the last pan gesture in screen
   * coordinates or, if the gesture has just started, then the distance in screen coordinates moved
   * since the user touched the screen.
   *
   * This is always a positive value.
   * @return The distance, as a float, a user's finger has panned.
   */
  float GetScreenDistance() const;
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_PAN_GESTURE_H__
