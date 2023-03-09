#ifndef DALI_PAN_GESTURE_H
#define DALI_PAN_GESTURE_H

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
#include <dali/public-api/events/gesture.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{
namespace Internal DALI_INTERNAL
{
class PanGesture;
}

/**
 * @addtogroup dali_core_events
 * @{
 */

/**
 * @brief A PanGesture is emitted when the user moves one or more fingers in a particular direction.
 *
 * This gesture can be in one of three states, when the pan gesture is first detected: GestureState::STARTED
 * when the pan gesture is continuing: GestureState::CONTINUING and finally, when the pan gesture ends:
 * GestureState::FINISHED.
 *
 * A pan gesture will end in the following ways:
 * - User releases the primary finger (the first touch).
 * - User has more fingers on the screen than the maximum specified.
 * - User has less fingers on the screen than the minimum specified.
 * - Cancelled by the system.
 *
 * A pan gesture will continue to be sent to the actor under than initial pan until it ends.
 * @SINCE_1_9.28
 */
class DALI_CORE_API PanGesture : public Gesture
{
public:
  /**
   * @brief Creates an uninitialized PanGesture handle.
   *
   * Calling member functions with an uninitialized Actor handle is not allowed.
   * @SINCE_1_9.28
   */
  PanGesture();

  /**
   * @brief Copy constructor.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the copied handle
   */
  PanGesture(const PanGesture& rhs);

  /**
   * @brief Move constructor.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the moved handle
   */
  PanGesture(PanGesture&& rhs) noexcept;

  /**
   * @brief Assignment operator.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  PanGesture& operator=(const PanGesture& rhs);

  /**
   * @brief Move assignment operator.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  PanGesture& operator=(PanGesture&& rhs) noexcept;

  /**
   * @brief Non virtual destructor.
   * @SINCE_1_9.28
   */
  ~PanGesture();

  /**
   * @brief The velocity at which the user is moving their fingers.
   *
   * This is represented as a Vector2 and is the pixel movement per millisecond.
   * A positive x value shows that the user is panning to the right, a negative x value means the opposite.
   * A positive y value shows that the user is panning downwards, a negative y values means upwards.
   * This value represents the local coordinates of the actor attached to the PanGestureDetector.
   * @SINCE_1_9.28
   * @return The velocity of the pan
   */
  const Vector2& GetVelocity() const;

  /**
   * @brief This is a Vector2 showing how much the user has panned (dragged) since the last pan gesture or,
   * if the gesture has just started, then the amount panned since the user touched the screen.
   *
   * A positive x value shows that the user is panning to the right, a negative x value means the opposite.
   * A positive y value shows that the user is panning downwards, a negative y value means upwards.
   * This value is in local actor coordinates, the actor being the one attached to the
   * PanGestureDetector.
   * @SINCE_1_9.28
   * @return The displacement of the pan
   */
  const Vector2& GetDisplacement() const;

  /**
   * @brief This current touch position of the primary touch point in local actor coordinates.
   * @SINCE_1_9.28
   * @return The current position of the pan (in local actor coordinates)
   */
  const Vector2& GetPosition() const;

  /**
   * @brief The velocity at which the user is moving their fingers.
   *
   * This is represented as a Vector2 and is the pixel movement per millisecond.
   * A positive x value shows that the user is panning to the right, a negative x value means the opposite.
   * A positive y value shows that the user is panning downwards, a negative y values means upwards.
   * This value represents the screen coordinates.
   * @SINCE_1_9.28
   * @return The velocity of the pan (in screen coordinates)
   */
  const Vector2& GetScreenVelocity() const;

  /**
   * @brief This is a Vector2 showing how much the user has panned (dragged) since the last pan gesture or,
   * if the gesture has just started, then the amount panned since the user touched the screen.
   *
   * A positive x value shows that the user is panning to the right, a negative x value means the opposite.
   * A positive y value shows that the user is panning downwards, a negative y value means upwards.
   * This value is in screen coordinates.
   * @SINCE_1_9.28
   * @return The displacement of the pan (in screen coordinates)
   */
  const Vector2& GetScreenDisplacement() const;

  /**
   * @brief This current touch position of the primary touch point in screen coordinates.
   * @SINCE_1_9.28
   * @return The current position of the pan (in screen coordinates)
   */
  const Vector2& GetScreenPosition() const;

  /**
   * @brief The total number of fingers touching the screen in a pan gesture.
   * @SINCE_1_9.28
   * @return The number of touches in the pan
   */
  uint32_t GetNumberOfTouches() const;

  // Convenience Methods

  /**
   * @brief Returns the speed at which the user is moving their fingers.
   *
   * This is the pixel movement per millisecond.
   * @SINCE_1_9.28
   * @return The speed of the pan (in pixels per millisecond)
   */
  float GetSpeed() const;

  /**
   * @brief This returns the distance the user has panned (dragged) since the last pan gesture or,
   * if the gesture has just started, then the distance moved since the user touched the screen.
   *
   * This is always a positive value.
   * @SINCE_1_9.28
   * @return The distance, as a float, a user's finger has panned
   */
  float GetDistance() const;

  /**
   * @brief Returns the speed at which the user is moving their fingers relative to screen coordinates.
   *
   * This is the pixel movement per millisecond.
   * @SINCE_1_9.28
   * @return The speed of the pan (in pixels per millisecond)
   */
  float GetScreenSpeed() const;

  /**
   * @brief This returns the distance the user has panned (dragged) since the last pan gesture in screen
   * coordinates or, if the gesture has just started, then the distance in screen coordinates moved
   * since the user touched the screen.
   *
   * This is always a positive value.
   * @SINCE_1_9.28
   * @return The distance, as a float, a user's finger has panned
   */
  float GetScreenDistance() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used internally to Create an initialized PanGesture handle.
   *
   * @param[in] panGesture A pointer to a newly allocated Dali resource
   * @SINCE_1_9.28
   */
  explicit DALI_INTERNAL PanGesture(Internal::PanGesture* panGesture);
  /// @endcond
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_PAN_GESTURE_H
