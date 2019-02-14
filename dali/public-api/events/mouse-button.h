#ifndef DALI_MOUSE_BUTTON_H
#define DALI_MOUSE_BUTTON_H

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

namespace Dali
{
/**
 * @addtogroup dali_core_events
 * @{
 */

/**
 * @brief Mouse Button Type
 * @SINCE_1_3_31
 */
namespace MouseButton
{

/**
 * @brief Enumeration for mouse button type.
 * @SINCE_1_3_31
 */
enum Type
{
  INVALID = -1, /**< No mouse button event or invalid data */
  PRIMARY = 1, /**< Primary(Left) mouse button */
  SECONDARY = 3, /**< Secondary(Right) mouse button */
  TERTIARY = 2, /**< Center(Wheel) mouse button */
};
}

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_MOUSE_BUTTON_H__
