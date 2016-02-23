#ifndef __DALI_LONG_PRESS_GESTURE_H__
#define __DALI_LONG_PRESS_GESTURE_H__

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
 * @brief A LongPressGesture is emitted when the user touches and holds the screen with the stated number of fingers.
 *
 * This gesture can be in one of two states, when the long-press gesture is first detected: Gesture::Started
 * and when the long-press gesture ends: Gesture::Finished.
 *
 * Long press gesture finishes when all touches have been released.
 *
 * @SINCE_1_0.0
 * @see LongPressGestureDetector
 */
struct DALI_IMPORT_API LongPressGesture : public Gesture
{
  // Construction & Destruction

  /**
   * @brief Constructor
   *
   * @SINCE_1_0.0
   * @param[in] state The state of the gesture
   */
  LongPressGesture(Gesture::State state);

  /**
   * @brief Copy constructor
   * @SINCE_1_0.0
   * @param[in] rhs A reference to the copied handle
   */
  LongPressGesture( const LongPressGesture& rhs );

  /**
   * @brief Assignment operator
   * @SINCE_1_0.0
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  LongPressGesture& operator=( const LongPressGesture& rhs );

  /**
   * @brief Virtual destructor
   * @SINCE_1_0.0
   */
  virtual ~LongPressGesture();

  // Data

  /**
   * @brief The number of touch points in this long press gesture, i.e. the number of fingers the user had
   * on the screen to generate the long press gesture.
   */
  unsigned int numberOfTouches;

  /**
   * @brief This is the point, in screen coordinates, where the long press occurred.
   *
   * If a multi-touch long press, then this is the centroid of all the touch points.
   */
  Vector2 screenPoint;

  /**
   * @brief This is the point, in local actor coordinates, where the long press occurred.
   *
   * If a multi-touch long press, then this is the centroid of all the touch points.
   * @return The point where tap has occurred (in local actor coordinates).
   */
  Vector2 localPoint;
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_LONG_PRESS_GESTURE_H__
