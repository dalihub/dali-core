#ifndef DALI_LONG_PRESS_GESTURE_DETECTOR_DEVEL_H
#define DALI_LONG_PRESS_GESTURE_DETECTOR_DEVEL_H

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
#include <dali/public-api/events/long-press-gesture-detector.h>

namespace Dali
{

class LongPressGestureDetector;

namespace DevelLongPressGestureDetector
{

/**
 * @brief Get the minimum holding time required to be recognized as a long press gesture in milliseconds
 *
 * @param[in] longPressGestureDetector The target LongPressGestureDetector instance
 * @return The time value in milliseconds
 */
DALI_CORE_API uint32_t GetMinimumHoldingTime( LongPressGestureDetector longPressGestureDetector );

} // namespace DevelLongPressGestureDetector

} // namespace Dali

#endif // DALI_LONG_PRESS_GESTURE_DETECTOR_DEVEL_H
