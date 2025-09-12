#ifndef DALI_WHEEL_EVENT_DEVEL_H
#define DALI_WHEEL_EVENT_DEVEL_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/events/wheel-event.h>

namespace Dali
{
namespace DevelWheelEvent
{
/**
 * @brief Creates an initialized WheelEvent.
 *
 * @SINCE_1_9.26
 * @param[in] type      The type of the wheel event
 * @param[in] direction The direction of wheel rolling (0 = default vertical wheel, 1 = horizontal wheel)
 * @param[in] modifiers Modifier keys pressed during the event (such as shift, alt and control)
 * @param[in] point     The co-ordinates of the cursor relative to the top-left of the screen
 * @param[in] delta     The offset of rolling (positive value means roll down or clockwise, and negative value means roll up or counter-clockwise)
 * @param[in] timeStamp The time the wheel is being rolled
 * @return A handle to a newly allocated Dali resource
 */
DALI_CORE_API WheelEvent New(WheelEvent::Type type, int32_t direction, uint32_t modifiers, Vector2 point, int32_t delta, uint32_t timeStamp);

} // namespace DevelWheelEvent

} // namespace Dali

#endif // DALI_WHEEL_EVENT_DEVEL_H
