#ifndef DALI_TOUCH_DATA_DEVEL_H
#define DALI_TOUCH_DATA_DEVEL_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/events/touch-data.h>
#include <dali/devel-api/events/mouse-button.h>

namespace Dali
{

namespace DevelTouchData
{

/**
 * @brief Get mouse's button value (ex: right/left button)
 *
 * @param[in] touchData The instance of TouchData.
 * @param[in] point The point required
 * @return The mouse button value
 */
DALI_CORE_API MouseButton::Type GetMouseButton( TouchData touchData, std::size_t point );

} // namespace DevelTouchData

} // namespace Dali

#endif // DALI_INTERNAL_TOUCH_DATA_DEVEL_H
