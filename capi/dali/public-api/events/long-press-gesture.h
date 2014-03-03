#ifndef __DALI_LONG_PRESS_GESTURE_H__
#define __DALI_LONG_PRESS_GESTURE_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/events/gesture.h>
#include <dali/public-api/math/vector2.h>

namespace Dali DALI_IMPORT_API
{

/**
 * A LongPressGesture is emitted when the user touches and holds the screen with the stated number of
 * fingers.
 * This gesture can be in one of two states, when the long-press gesture is first detected: "Started";
 * and when the long-press gesture ends: "Finished".
 *
 * Long press gesture finishes when all touches have been released.
 *
 * @see LongPressGestureDetector
 */
struct LongPressGesture : public Gesture
{
  // Construction & Destruction

  /**
   * Constructor
   * @param[in] state  The state of the gesture
   */
  LongPressGesture(Gesture::State state);

  /**
   * Copy constructor
   */
  LongPressGesture( const LongPressGesture& rhs );

  /**
   * Assignment operator
   */
  LongPressGesture& operator=( const LongPressGesture& rhs );

  /**
   * Virtual destructor
   */
  virtual ~LongPressGesture();

  // Data

  /**
   * The number of touch points in this long press gesture, i.e. the number of fingers the user had
   * on the screen to generate the long press gesture.
   */
  unsigned int numberOfTouches;

  /**
   * This is the point, in screen coordinates, where the long press occurred.
   * If a multi-touch long press, then this is the centroid of all the touch points.
   */
  Vector2 screenPoint;

  /**
   * This is the point, in local actor coordinates, where the long press occurred.
   * If a multi-touch long press, then this is the centroid of all the touch points.
   * @return The point where tap has occurred (in local actor coordinates).
   */
  Vector2 localPoint;
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_LONG_PRESS_GESTURE_H__
