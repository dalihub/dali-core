#ifndef DALI_PINCH_GESTURE_H
#define DALI_PINCH_GESTURE_H

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
class PinchGesture;
}

/**
 * @addtogroup dali_core_events
 * @{
 */

/**
 * @brief A PinchGesture is emitted when the user moves two fingers towards or away from each other.
 *
 * This gesture can be in one of three states; when the pinch gesture is first detected, its
 * state is set to GestureState::STARTED.  After this, if there is change in the gesture, the state will
 * be GestureState::CONTINUING. Finally, when the gesture ends, the state of the gesture changes to
 * GestureState::FINISHED.
 *
 * A pinch gesture will continue to be sent to the actor under the center point of the pinch
 * until the pinch ends.
 * @SINCE_1_9.28
 */
class DALI_CORE_API PinchGesture : public Gesture
{
public:
  /**
   * @brief Creates an uninitialized PinchGesture handle.
   *
   * Calling member functions with an uninitialized Actor handle is not allowed.
   * @SINCE_1_9.28
   */
  PinchGesture();

  /**
   * @brief Copy constructor.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the copied handle
   */
  PinchGesture(const PinchGesture& rhs);

  /**
   * @brief Move constructor.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the moved handle
   */
  PinchGesture(PinchGesture&& rhs) noexcept;

  /**
   * @brief Assignment operator.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  PinchGesture& operator=(const PinchGesture& rhs);

  /**
   * @brief Assignment operator.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  PinchGesture& operator=(PinchGesture&& rhs) noexcept;

  /**
   * @brief Non virtual destructor.
   * @SINCE_1_9.28
   */
  ~PinchGesture();

  /**
   * @brief The scale factor from the start of the pinch gesture till the latest pinch gesture.
   *
   * If the user is moving their fingers away from each other, then this value increases.
   * Conversely, if the user is moving their fingers towards each other, this value will
   * decrease.
   * @SINCE_1_9.28
   * @return The scale of the pinch (from the pinch start)
   */
  float GetScale() const;

  /**
   * @brief The speed at which the user is moving their fingers.
   *
   * This is the pixel movement per second.
   * @SINCE_1_9.28
   * @return The speed of the pinch (pixels per second)
   */
  float GetSpeed() const;

  /**
   * @brief The center point of the two points that caused the pinch gesture in screen coordinates.
   * @SINCE_1_9.28
   * @return The current center point of the pinch (in screen coordinates)
   */
  const Vector2& GetScreenCenterPoint() const;

  /**
   * @brief The center point of the two points that caused the pinch gesture in local actor coordinates.
   * @SINCE_1_9.28
   * @return The current center point of the pinch (in local actor coordinates)
   */
  const Vector2& GetLocalCenterPoint() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used internally to Create an initialized PinchGesture handle.
   *
   * @param[in] PinchGesture A pointer to a newly allocated Dali resource
   * @SINCE_1_9.28
   */
  explicit DALI_INTERNAL PinchGesture(Internal::PinchGesture* pinchGesture);
  /// @endcond
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_PINCH_GESTURE_H
