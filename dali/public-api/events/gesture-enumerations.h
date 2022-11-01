#ifndef DALI_GESTURE_ENUMERATIONS_H
#define DALI_GESTURE_ENUMERATIONS_H

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

// EXTERNAL INCLUDES
#include <cstdint>

namespace Dali
{
/**
 * @brief Enumeration for type of gesture.
 * @SINCE_1_9.28
 */
namespace GestureType
{
enum Value : uint8_t
{
  PINCH      = 1 << 0, ///< When two touch points move away or towards each other. @SINCE_1_9.28
  PAN        = 1 << 1, ///< When the user drags their finger(s) in a particular direction. @SINCE_1_9.28
  TAP        = 1 << 2, ///< When the user taps the screen. @SINCE_1_9.28
  LONG_PRESS = 1 << 3, ///< When the user continues to touch the same area on the screen for the device configured time. @SINCE_1_9.28
  ROTATION   = 1 << 4, ///< When the user rotates two fingers around a particular ares of the screen. @SINCE_1_9.28
};
} // namespace GestureType

/**
 * @brief Enumeration for state of the gesture.
 * @SINCE_1_9.28
 */
enum class GestureState : uint8_t
{
  CLEAR,      ///< There is no state associated with this gesture. @SINCE_1_9.28
  STARTED,    ///< The touched points on the screen have moved enough to be considered a gesture. @SINCE_1_9.28
  CONTINUING, ///< The gesture is continuing. @SINCE_1_9.28
  FINISHED,   ///< The user has lifted a finger or touched an additional point on the screen. @SINCE_1_9.28
  CANCELLED,  ///< The gesture has been cancelled. @SINCE_1_9.28
  POSSIBLE    ///< A gesture is possible. @SINCE_1_9.28
};

/**
 * @brief Enumeration for gesture input source type.
 * @SINCE_2_2.0
 */
enum class GestureSourceType : int8_t
{
  INVALID, ///< invalid data
  MOUSE,   ///< mouse
};

/**
 * @brief Enumeration for data of gesture input source type.
 * @SINCE_2_2.0
 */
enum class GestureSourceData : int8_t
{
  INVALID         = -1, ///< invalid data
  MOUSE_PRIMARY   = 1,  /**< Primary(Left) mouse button */
  MOUSE_SECONDARY = 3,  /**< Secondary(Right) mouse button */
  MOUSE_TERTIARY  = 2,  /**< Center(Wheel) mouse button */
};

} // namespace Dali

#endif // DALI_GESTURE_ENUMERATIONS_H
