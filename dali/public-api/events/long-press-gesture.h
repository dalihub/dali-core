#ifndef DALI_LONG_PRESS_GESTURE_H
#define DALI_LONG_PRESS_GESTURE_H

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
class LongPressGesture;
}

/**
 * @addtogroup dali_core_events
 * @{
 */

/**
 * @brief A LongPressGesture is emitted when the user touches and holds the screen with the stated number of fingers.
 *
 * This gesture can be in one of two states, when the long-press gesture is first detected: GestureState::STARTED
 * and when the long-press gesture ends: GestureState::FINISHED.
 *
 * Long press gesture finishes when all touches have been released.
 *
 * @SINCE_1_9.28
 * @see LongPressGestureDetector
 */
class DALI_CORE_API LongPressGesture : public Gesture
{
public:
  /**
   * @brief Creates an uninitialized LongPressGesture handle.
   *
   * Calling member functions with an uninitialized Actor handle is not allowed.
   * @SINCE_1_9.28
   */
  LongPressGesture();

  /**
   * @brief Copy constructor.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the copied handle
   */
  LongPressGesture(const LongPressGesture& rhs);

  /**
   * @brief Move constructor.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the handle to move
   */
  LongPressGesture(LongPressGesture&& rhs) noexcept;

  /**
   * @brief Assignment operator.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  LongPressGesture& operator=(const LongPressGesture& rhs);

  /**
   * @brief Move assignment operator.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  LongPressGesture& operator=(LongPressGesture&& rhs) noexcept;

  /**
   * @brief Non virtual destructor.
   * @SINCE_1_9.28
   */
  ~LongPressGesture();

  /**
   * @brief The number of touch points in this long press gesture.
   *
   * In other words, the number of fingers the user had on the screen to generate the long press gesture.
   * @SINCE_1_9.28
   * @return The number of touch points
   */
  uint32_t GetNumberOfTouches() const;

  /**
   * @brief This is the point, in screen coordinates, where the long press occurred.
   *
   * If a multi-touch long press, then this is the centroid of all the touch points.
   * @SINCE_1_9.28
   * @return The point where the long press occurred (in screen coordinates)
   */
  const Vector2& GetScreenPoint() const;

  /**
   * @brief This is the point, in local actor coordinates, where the long press occurred.
   *
   * If a multi-touch long press, then this is the centroid of all the touch points.
   * @SINCE_1_9.28
   * @return The point where tap has occurred (in local actor coordinates)
   */
  const Vector2& GetLocalPoint() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used internally to Create an initialized LongPressGesture handle.
   *
   * @param[in] longPressGesture A pointer to a newly allocated Dali resource
   * @SINCE_1_9.28
   */
  explicit DALI_INTERNAL LongPressGesture(Internal::LongPressGesture* longPressGesture);
  /// @endcond
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_LONG_PRESS_GESTURE_H
