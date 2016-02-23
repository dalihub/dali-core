#ifndef __DALI_TAP_GESTURE_H__
#define __DALI_TAP_GESTURE_H__

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
 * @brief A TapGesture is emitted when the user taps the screen with the stated number of fingers a stated number of times.
 *
 * This is a discrete gesture so does not have any state information.
 * @SINCE_1_0.0
 * @see TapGestureDetector
 */
struct DALI_IMPORT_API TapGesture : public Gesture
{
  // Construction & Destruction

  /**
   * @brief Default Constructor
   * @SINCE_1_0.0
   */
  TapGesture();

  /**
   * @brief Copy constructor
   * @SINCE_1_0.0
   * @param rhs A reference to the copied handle
   */
  TapGesture( const TapGesture& rhs );

  /**
   * @brief Assignment operator
   * @SINCE_1_0.0
   * @param rhs A reference to the copied handle
   * @return A reference to this
   */
  TapGesture& operator=( const TapGesture& rhs );

  /**
   * @brief Virtual destructor
   * @SINCE_1_0.0
   */
  virtual ~TapGesture();

  // Data

  /**
   * @brief The number of taps in this tap gesture.
   */
  unsigned int numberOfTaps;

  /**
   * @brief The number of touch points in this tap gesture, i.e. the number of fingers the user had on the
   * screen to generate the tap gesture.
   */
  unsigned int numberOfTouches;

  /**
   * @brief This is the point, in screen coordinates, where the tap occurred.
   *
   * If a multi-touch tap, then this is the centroid of all the touch points.
   */
  Vector2 screenPoint;

  /**
   * @brief This is the point, in local actor coordinates, where the tap occurred.
   *
   * If a multi-touch tap, then this is the centroid of all the touch points.
   * @return The point where tap has occurred (in local actor coordinates).
   */
  Vector2 localPoint;
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_TAP_GESTURE_H__
