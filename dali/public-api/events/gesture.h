#ifndef DALI_GESTURE_H
#define DALI_GESTURE_H

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

// EXTERNAL INCLUDES
#include <cstdint> // uint32_t

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/base-handle.h>

namespace Dali
{
/**
 * @addtogroup dali_core_events
 * @{
 */

namespace Internal DALI_INTERNAL
{
class Gesture;
}

/**
 * @brief Base structure for different gestures that an application can receive.
 *
 * A gesture is an event that is produced from a combination of
 * several touch events in a particular order or within a certain time
 * frame (e.g pinch).
 *
 * To receive a particular gesture, the application has to create and connect to the appropriate
 * GestureDetector.
 *
 * @SINCE_1_9.28
 * @note An instance of this class cannot be created.
 * @see GestureDetector
 *
 */
class DALI_CORE_API Gesture : public BaseHandle
{
public:

  /**
   * @brief Creates an uninitialized Gesture handle.
   *
   * Calling member functions with an uninitialized Actor handle is not allowed.
   * @SINCE_1_9.28
   */
  Gesture();

  /**
   * @brief Copy constructor.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the copied handle
   */
  Gesture( const Gesture& rhs );

  /**
   * @brief Move constructor.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the handle to move
   */
  Gesture( Gesture&& rhs );

  /**
   * @brief Assignment operator.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  Gesture& operator=( const Gesture& rhs );

  /**
   * @brief Move assignment operator.
   * @SINCE_1_9.28
   * @param[in] rhs A reference to the handle to move
   * @return A reference to this
   */
  Gesture& operator=( Gesture&& rhs );

  /**
   * @brief Non virtual destructor.
   * @SINCE_1_9.28
   */
  ~Gesture();

  /**
   * @brief Enumeration for type of gesture.
   * @SINCE_1_0.0
   */
  enum Type
  {
    Pinch      = 1 << 0, ///< When two touch points move away or towards each other. @SINCE_1_0.0
    Pan        = 1 << 1, ///< When the user drags their finger(s) in a particular direction. @SINCE_1_0.0
    Tap        = 1 << 2, ///< When the user taps the screen. @SINCE_1_0.0
    LongPress  = 1 << 3, ///< When the user continues to touch the same area on the screen for the device configured time. @SINCE_1_0.0
    Rotation   = 1 << 4, ///< When the user rotates two fingers around a particular ares of the screen. @SINCE_1_9.27
  };

  /**
   * @brief Enumeration for state of the gesture.
   * @SINCE_1_0.0
   */
  enum State
  {
    Clear,      ///< There is no state associated with this gesture. @SINCE_1_0.0
    Started,    ///< The touched points on the screen have moved enough to be considered a gesture. @SINCE_1_0.0
    Continuing, ///< The gesture is continuing. @SINCE_1_0.0
    Finished,   ///< The user has lifted a finger or touched an additional point on the screen. @SINCE_1_0.0
    Cancelled,  ///< The gesture has been cancelled. @SINCE_1_0.0
    Possible    ///< A gesture is possible. @SINCE_1_0.0
  };

  /**
   * @brief The gesture type.
   * @SINCE_1_9.28
   * @return The gesture type
   */
  Type GetType() const;

  /**
   * @brief The gesture state.
   *
   * Please see the description in the sub-classes to see the states
   * used by the actual gesture.
   * @SINCE_1_9.28
   * @return The state of the gesture
   */
  State GetState() const;

  /**
   * @brief The time the gesture took place.
   * @SINCE_1_9.28
   * @return The time the gesture took place
   */
  uint32_t GetTime() const;

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief Internal constructor.
   * @SINCE_1_9.28
   * @param[in] gesture Pointer to internal gesture
   */
  explicit DALI_INTERNAL Gesture(Internal::Gesture* gesture);
  /// @endcond
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_GESTURE_H
