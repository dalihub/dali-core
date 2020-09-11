#ifndef DALI_PAN_GESTURE_DEVEL_H
#define DALI_PAN_GESTURE_DEVEL_H

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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/events/pan-gesture.h>

namespace Dali
{
namespace DevelPanGesture
{
/**
 * @brief Create a PanGeture for internal.
 * @param[in] state The state of the PanGesture
 */
DALI_CORE_API Dali::PanGesture New(GestureState state);

/**
 * @brief Set the time the gesture took place.
 * @param[in] gesture The PanGesture to set
 * @param[in] time The time the gesture took place
 */
DALI_CORE_API void SetTime(Dali::PanGesture& gesture, uint32_t time);

/**
 * @brief Set the velocity at which the user is moving their fingers.
 * @param[in] gesture The PanGesture to set
 * @param[in] velocity The Vector2 in local coordinates to set
 */
DALI_CORE_API void SetVelocity(Dali::PanGesture& gesture, const Vector2& velocity);

/**
 * @brief Set the displacement.
 * @param[in] gesture The PanGesture to set
 * @param[in] displacement The Vector2 in local coordinates to set
 */
DALI_CORE_API void SetDisplacement(Dali::PanGesture& gesture, const Vector2& displacement);

/**
 * @brief Set the current touch position of the primary touch point in local actor coordinates.
 * @param[in] gesture The PanGesture to set.
 * @param[in] position The current touch position to set.
 */
DALI_CORE_API void SetPosition(Dali::PanGesture& gesture, const Vector2& position);

/**
 * @brief Set the velocity at which the user is moving their fingers.
 * @param[in] gesture The PanGesture to set
 * @param[in] screenVelocity The Vector2 in screen coordinates to set
 */
DALI_CORE_API void SetScreenVelocity(Dali::PanGesture& gesture, const Vector2& screenVelocity);

/**
 * @brief Set the screen displacement.
 * @param[in] gesture The PanGesture to set
 * @param[in] screenDisplacement The Vector2 in screen coordinates to set
 */
DALI_CORE_API void SetScreenDisplacement(Dali::PanGesture& gesture, const Vector2& screenDisplacement);

/**
 * @brief Set the current touch position of the primary touch point in screen coordinates.
 * @param[in] gesture The PanGesture to set.
 * @param[in] screenPosition The Vector2 in screen coordinates to set.
 */
DALI_CORE_API void SetScreenPosition(Dali::PanGesture& gesture, const Vector2& screenPosition);

/**
 * @brief Set the total number of fingers touching the screen in a pan gesture.
 * @param[in] gesture The PanGesture to set
 * @param[in] numberOfTouches The total number of fingers touching the screen to set
 */
DALI_CORE_API void SetNumberOfTouches(Dali::PanGesture& gesture, uint32_t numberOfTouches);

} // namespace DevelPanGesture

} // namespace Dali

#endif // DALI_PAN_GESTURE_DEVEL_H
