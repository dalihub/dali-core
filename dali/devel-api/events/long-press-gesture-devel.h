#ifndef DALI_LONG_PRESS_GESTURE_DEVEL_H
#define DALI_LONG_PRESS_GESTURE_DEVEL_H

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
#include <dali/public-api/events/long-press-gesture.h>

namespace Dali
{
namespace DevelLongPressGesture
{
/**
 * @brief Creates an initialized LongPressGesture.
 *
 * @param[in] state The state of the LongPressGesture
 * @return A handle to a newly allocated Dali resource
 */
DALI_CORE_API LongPressGesture New(GestureState state);

} // namespace DevelLongPressGesture

} // namespace Dali

#endif // DALI_LONG_PRESS_GESTURE_DEVEL_H
