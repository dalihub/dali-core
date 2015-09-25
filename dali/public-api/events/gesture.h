#ifndef __DALI_GESTURE_H__
#define __DALI_GESTURE_H__

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
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
/**
 * @addtogroup dali_core_events
 * @{
 */

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
 * @see GestureDetector
 *
 * @note An instance of this class cannot be created.
 */
struct DALI_IMPORT_API Gesture
{
  /**
   * @brief Copy constructor.
   */
  Gesture( const Gesture& rhs );

  /**
   * @brief Assignment operator.
   */
  Gesture& operator=( const Gesture& rhs );

  // Destruction

  /**
   * @brief Virtual destructor.
   */
  virtual ~Gesture();

  /**
   * @brief Type of gesture.
   */
  enum Type
  {
    Pinch      = 1 << 0, ///< When two touch points move away or towards each other.
    Pan        = 1 << 1, ///< When the user drags their finger(s) in a particular direction.
    Tap        = 1 << 2, ///< When the user taps the screen.
    LongPress  = 1 << 3  ///< When the user continues to touch the same area on the screen for the device configured time.
  };

  /**
   * @brief State of the gesture.
   */
  enum State
  {
    Clear,      ///< There is no state associated with this gesture.
    Started,    ///< The touched points on the screen have moved enough to be considered a gesture.
    Continuing, ///< The gesture is continuing.
    Finished,   ///< The user has lifted a finger or touched an additional point on the screen.
    Cancelled,  ///< The gesture has been cancelled.
    Possible    ///< A gesture is possible.
  };

  // Data

  /**
   * @brief The gesture type.
   */
  Type type;

  /**
   * @brief The gesture state.
   *
   * Please see the description in the sub-classes to see the states
   * used by the actual gesture.
   */
  State state;

  /**
   * @brief The time the gesture took place.
   */
  unsigned int time;

protected: // Creation

  /**
   * @brief This constructor is only used by derived classes.
   *
   * @param[in] gestureType   The type of gesture event.
   * @param[in] gestureState  The state of the gesture event.
   */
  Gesture(Type gestureType, State gestureState);

};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_GESTURE_H__
