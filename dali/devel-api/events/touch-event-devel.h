#ifndef DALI_TOUCH_EVENT_DEVEL_H
#define DALI_TOUCH_EVENT_DEVEL_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/events/touch-event.h>

namespace Dali
{
namespace DevelTouchEvent
{

/**
 * @brief Set the time (in ms) that the touch event occurred.
 *
 * @param[in] touchEvent The instance of TouchEvent.
 * @param[in] time The time (in ms)
 */
DALI_CORE_API void SetTime(TouchEvent touchEvent, uint64_t time);

} // namespace DevelTouchEvent

} // namespace Dali

#endif // DALI_TOUCH_EVENT_DEVEL_H
