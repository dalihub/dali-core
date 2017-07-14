#ifndef DALI_TOUCH_DATA_DEVEL_H
#define DALI_TOUCH_DATA_DEVEL_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

// INTERNAL_INCLUDES
#include <dali/public-api/events/touch-data.h>
#include <dali/devel-api/events/device.h>

namespace Dali
{

namespace DevelTouchData
{

/**
 * @brief Get the device class type the mouse/touch event originated from
 *
 * The device class type is classification type of the input device of event received.
 * @param[in] touch The TouchData to retrieve the device class from
 * @param[in] point The point required
 * @return The type of the device class
 */
DALI_IMPORT_API DevelDevice::Class::Type GetDeviceClass( const TouchData& touch, std::size_t point );

/**
 * @brief Get the device subclass type the mouse/touch event originated from
 *
 * The device subclass type is subclassification type of the input device of event received.
 * @param[in] touch The TouchData to retrieve the device subclass from
 * @param[in] point The point required
 * @return The type of the device subclass
 */
DALI_IMPORT_API DevelDevice::Subclass::Type GetDeviceSubclass( const TouchData& touch, std::size_t point );

} // namespace DevelTouchData

} // namespace Dali

#endif //DALI_TOUCH_DATA_DEVEL_H
