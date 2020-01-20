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

// // EXTERNAL INCLUDES
// #include <dali/public-api/events/key-event.h>
// #include <dali/public-api/events/touch-event.h>
// #include <dali/public-api/events/wheel-event.h>

// INTERNAL INCLUDES
#include <dali/devel-api/events/long-press-gesture-detector-devel.h>
#include <dali/internal/event/events/long-press-gesture/long-press-gesture-detector-impl.h>

namespace Dali
{

namespace DevelLongPressGestureDetector
{

uint32_t GetMinimumHoldingTime( LongPressGestureDetector longPressGestureDetector )
{
  return GetImplementation( longPressGestureDetector ).GetMinimumHoldingTime();
}


} // namespace DevelLongPressGestureDetector

} // namespace Dali

