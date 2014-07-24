#ifndef __DALI_PINCH_GESTURE_H__
#define __DALI_PINCH_GESTURE_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

namespace Dali DALI_IMPORT_API
{

/**
 * @brief A PinchGesture is emitted when the user moves two fingers towards or away from each other.
 *
 * This gesture can be in one of three states; when the pinch gesture is first detected, its
 * state is set to "Started".  After this, if there is change in the gesture, the state will
 * be "Continuing".  Finally, when the gesture ends, the state of the gesture changes to
 * "Finished".
 *
 * A pinch gesture will continue to be sent to the actor under the center point of the pinch
 * until the pinch ends.
 */
struct PinchGesture: public Gesture
{
  // Construction & Destruction

  /**
   * @brief Default Constructor.
   *
   * @param[in]  state  The state of the gesture
   */
  PinchGesture(Gesture::State state);

  /**
   * @brief Copy constructor.
   */
  PinchGesture( const PinchGesture& rhs );

  /**
   * @brief Assignment operator.
   */
  PinchGesture& operator=( const PinchGesture& rhs );

  /**
   * @brief Virtual destructor.
   */
  virtual ~PinchGesture();

  // Data

  /**
   * @brief The scale factor from the start of the pinch gesture till the latest pinch gesture.
   *
   * If the user is moving their fingers away from each other, then
   * this value increases.  Conversely, if the user is moving their
   * fingers towards each other, this value will decrease.
   */
  float scale;

  /**
   * @brief The speed at which the user is moving their fingers.
   *
   * This is the pixel movement per second.
   */
  float speed;

  /**
   * @brief The center point of the two points that caused the pinch gesture in screen coordinates.
   */
  Vector2 screenCenterPoint;

  /**
   * @brief The center point of the two points that caused the pinch gesture in local actor coordinates.
   */
  Vector2 localCenterPoint;
};

} // namespace Dali

#endif // __DALI_PINCH_GESTURE_H__
