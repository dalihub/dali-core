#ifndef DALI_ROTATION_GESTURE_H
#define DALI_ROTATION_GESTURE_H

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
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{
namespace Internal DALI_INTERNAL
{
class RotationGesture;
}

/**
 * @brief A RotationGesture is emitted when the user moves two fingers that are opposite each other
 * in a rotational/circular gesture.
 *
 * This gesture can be in one of three states; when the rotation gesture is first detected, its
 * state is set to GestureState::STARTED.  After this, if there is change in the gesture, the state will
 * be GestureState::CONTINUING. Finally, when the gesture ends, the state of the gesture changes to
 * GestureState::FINISHED.
 *
 * A rotation gesture will continue to be sent to the actor under the center point of the rotation
 * until the rotation ends.
 * @SINCE_1_9.28
 */
class DALI_CORE_API RotationGesture : public Gesture
{
public:
  /**
   * @brief Creates an uninitialized RotationGesture handle.
   *
   * Calling member functions with an uninitialized Actor handle is not allowed.
   * @SINCE_1_9.28
   */
  RotationGesture();

  /**
   * @brief Copy constructor.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the copied handle
   */
  RotationGesture(const RotationGesture& rhs);

  /**
   * @brief Move constructor.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the moved handle
   */
  RotationGesture(RotationGesture&& rhs) noexcept;

  /**
   * @brief Assignment operator.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  RotationGesture& operator=(const RotationGesture& rhs);

  /**
   * @brief Move assignment operator.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  RotationGesture& operator=(RotationGesture&& rhs) noexcept;

  /**
   * @brief Non virtual destructor.
   * @SINCE_1_9.28
   */
  ~RotationGesture();

  /**
   * @brief The overall rotation from the start of the rotation gesture till the latest rotation gesture.
   * @SINCE_1_9.28
   * @return The rotation from the start of the rotation gesture till the latest rotation gesture.
   */
  const Radian& GetRotation() const;

  /**
   * @brief The center point of the two points that caused the rotation gesture in screen coordinates.
   * @SINCE_1_9.28
   * @return The center point in screen coordinates
   */
  const Vector2& GetScreenCenterPoint() const;

  /**
   * @brief The center point of the two points that caused the rotation gesture in local actor coordinates.
   * @SINCE_1_9.28
   * @return The center point in local actor coordinates
   */
  const Vector2& GetLocalCenterPoint() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used internally to Create an initialized RotationGesture handle.
   *
   * @param[in] rotationGesture A pointer to a newly allocated Dali resource
   * @SINCE_1_9.28
   */
  explicit DALI_INTERNAL RotationGesture(Internal::RotationGesture* rotationGesture);
  /// @endcond
};

} // namespace Dali

#endif // DALI_ROTATION_GESTURE_H
