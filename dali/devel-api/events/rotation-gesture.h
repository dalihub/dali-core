#ifndef DALI_ROTATION_GESTURE_H
#define DALI_ROTATION_GESTURE_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

/**
 * @brief A RotationGesture is emitted when the user moves two fingers that are opposite each other
 * in a rotational/circular gesture.
 *
 * This gesture can be in one of three states; when the rotation gesture is first detected, its
 * state is set to Gesture::Started.  After this, if there is change in the gesture, the state will
 * be Gesture::Continuing. Finally, when the gesture ends, the state of the gesture changes to
 * Gesture::Finished.
 *
 * A rotation gesture will continue to be sent to the actor under the center point of the rotation
 * until the rotation ends.
 */
struct DALI_CORE_API RotationGesture: public Gesture
{
  // Construction & Destruction

  /**
   * @brief Default Constructor.
   *
   * @param[in] state The state of the gesture
   */
  RotationGesture( Gesture::State state );

  /**
   * @brief Copy constructor.
   * @param[in] rhs A reference to the copied handle
   */
  RotationGesture( const RotationGesture& rhs );

  /**
   * @brief Assignment operator.
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  RotationGesture& operator=( const RotationGesture& rhs );

  /**
   * @brief Virtual destructor.
   */
  virtual ~RotationGesture();

  // Data

  /**
   * @brief The overall rotation from the start of the rotation gesture till the latest rotation gesture.
   */
  Radian rotation;

  /**
   * @brief The center point of the two points that caused the rotation gesture in screen coordinates.
   */
  Vector2 screenCenterPoint;

  /**
   * @brief The center point of the two points that caused the rotation gesture in local actor coordinates.
   */
  Vector2 localCenterPoint;
};

} // namespace Dali

#endif // DALI_ROTATION_GESTURE_H
