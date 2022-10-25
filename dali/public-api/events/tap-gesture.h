#ifndef DALI_TAP_GESTURE_H
#define DALI_TAP_GESTURE_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
class TapGesture;
}

/**
 * @addtogroup dali_core_events
 * @{
 */

/**
 * @brief A TapGesture is emitted when the user taps the screen with the stated number of fingers a stated number of times.
 *
 * This is a discrete gesture so does not have any state information.
 * @SINCE_1_9.28
 * @see TapGestureDetector
 */
class DALI_CORE_API TapGesture : public Gesture
{
public:
  /**
   * @brief Creates an uninitialized TapGesture handle.
   *
   * Calling member functions with an uninitialized Actor handle is not allowed.
   * @SINCE_1_9.28
   */
  TapGesture();

  /**
   * @brief Copy constructor.
   * @SINCE_1_9.28
   * @param rhs A reference to the copied handle
   */
  TapGesture(const TapGesture& rhs);

  /**
   * @brief Move constructor.
   * @SINCE_1_9.28
   * @param rhs The handle to move.
   */
  TapGesture(TapGesture&& rhs);

  /**
   * @brief Assignment operator.
   * @SINCE_1_9.28
   * @param rhs A reference to the copied handle
   * @return A reference to this
   */
  TapGesture& operator=(const TapGesture& rhs);

  /**
   * @brief Move assignment operator.
   * @SINCE_1_9.28
   * @param rhs The handle to move.
   */
  TapGesture& operator=(TapGesture&& rhs);

  /**
   * @brief Non virtual destructor.
   * @SINCE_1_9.28
   */
  ~TapGesture();

  /**
   * @brief The number of taps in this tap gesture.
   * @SINCE_1_9.28
   * @return The number of taps
   */
  uint32_t GetNumberOfTaps() const;

  /**
   * @brief The number of touch points in this tap gesture, i.e. the number of fingers the user had on the
   * screen to generate the tap gesture.
   * @SINCE_1_9.28
   * @return The number of touchs
   */
  uint32_t GetNumberOfTouches() const;

  /**
   * @brief This is the point, in screen coordinates, where the tap occurred.
   *
   * If a multi-touch tap, then this is the centroid of all the touch points.
   * @SINCE_1_9.28
   * @return The point where tap has occurred( in screen coordinates)
   */
  const Vector2& GetScreenPoint() const;

  /**
   * @brief This is the point, in local actor coordinates, where the tap occurred.
   *
   * If a multi-touch tap, then this is the centroid of all the touch points.
   * @SINCE_1_9.28
   * @return The point where tap has occurred (in local actor coordinates)
   */
  const Vector2& GetLocalPoint() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used internally to Create an initialized TapGesture handle.
   *
   * @param[in] tapGesture A pointer to a newly allocated Dali resource
   * @SINCE_1_9.28
   */
  explicit DALI_INTERNAL TapGesture(Internal::TapGesture* tapGesture);
  /// @endcond
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_TAP_GESTURE_H
