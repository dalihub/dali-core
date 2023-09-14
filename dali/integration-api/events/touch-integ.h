#ifndef DALI_INTEGRATION_TOUCH_INTEG_H
#define DALI_INTEGRATION_TOUCH_INTEG_H

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
 */

// INTERNAL INCLUDES
#include <dali/devel-api/events/touch-point.h>
#include <dali/integration-api/events/point.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/events/hover-event.h>

namespace Dali
{
namespace Integration
{
/**
 * Create a new touch data handle from timestamp and touch point.
 *
 * @param[in] timeStamp The time stamp of the touch event.
 * @param[in] point The point on screen where the touch occurred.
 * @return A new touch data handle.
 */
DALI_CORE_API Dali::TouchEvent NewTouchEvent(uint32_t timeStamp, const TouchPoint& point);

/**
 * Create a new touch data handle from timestamp and point.
 *
 * @param[in] timeStamp The time stamp of the touch event.
 * @param[in] point The point on screen where the touch occurred.
 * @return A new touch data handle.
 */
DALI_CORE_API Dali::TouchEvent NewTouchEvent(uint32_t timeStamp, const Dali::Integration::Point& point);

/**
 * Create a new hover data handle from timestamp and point.
 *
 * @param[in] timeStamp The time stamp of the hover event.
 * @param[in] point The point on screen where the hover occurred.
 * @return A new hover data handle.
 */
DALI_CORE_API Dali::HoverEvent NewHoverEvent(uint32_t timeStamp, const TouchPoint& point);

/**
 * Create a new hover data handle from timestamp and point.
 *
 * @param[in] timeStamp The time stamp of the hpver event.
 * @param[in] point The point on screen where the hover occurred.
 * @return A new hover data handle.
 */
DALI_CORE_API Dali::HoverEvent NewHoverEvent(uint32_t timeStamp, const Dali::Integration::Point& point);


} // namespace Integration

} // namespace Dali

#endif //DALI_INTEGRATION_TOUCH_INTEG_H
