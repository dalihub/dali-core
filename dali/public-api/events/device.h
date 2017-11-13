#ifndef DALI_DEVICE_H
#define DALI_DEVICE_H

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

namespace Dali
{
/**
 * @addtogroup dali_core_events
 * @{
 */

/**
 * @brief Information about device types
 *
 * These types are for widget viewer applications.
 * A widget viewer application by Dali have to deliver events to widget applications
 * Server for handling events actually delivers any events to widget applications
 * The server needs to know input device information such as type to deliver events of proper type to widget applications
 * @SINCE_1_2.60
 */
namespace Device
{

/**
 * @brief Categorization type about input Device
 * @SINCE_1_2.60
 */
namespace Class
{

/**
 * @brief An enum of Device Class types.
 * @SINCE_1_2.60
 */
enum Type
{
  NONE,      ///< Not a device. @SINCE_1_2.60
  USER,      ///< The user/seat (the user themselves). @SINCE_1_2.60
  KEYBOARD,  ///< A regular keyboard, numberpad or attached buttons. @SINCE_1_2.60
  MOUSE,     ///< A mouse, trackball or touchpad relative motion device. @SINCE_1_2.60
  TOUCH,     ///< A touchscreen with fingers or stylus. @SINCE_1_2.60
  PEN,       ///< A special pen device. @SINCE_1_2.60
  POINTER,   ///< A pointing device based on laser, infrared or similar technology. @SINCE_1_2.60
  GAMEPAD    ///< A gamepad controller or joystick. @SINCE_1_2.60
};

} // namespace Class

/**
 * @brief Subcategorization type about input device.
 * @SINCE_1_2.60
 */
namespace Subclass
{

/**
 * @brief An enum of Device Subclass types.
 * @SINCE_1_2.60
 */
enum Type
{
  NONE,              ///< Not a device. @SINCE_1_2.60
  FINGER,            ///< The normal flat of your finger. @SINCE_1_2.60
  FINGERNAIL,        ///< A fingernail. @SINCE_1_2.60
  KNUCKLE,           ///< A Knuckle. @SINCE_1_2.60
  PALM,              ///< The palm of a users hand. @SINCE_1_2.60
  HAND_SIDE,         ///< The side of your hand. @SINCE_1_2.60
  HAND_FLAT,         ///< The flat of your hand. @SINCE_1_2.60
  PEN_TIP,           ///< The tip of a pen. @SINCE_1_2.60
  TRACKPAD,          ///< A trackpad style mouse. @SINCE_1_2.60
  TRACKPOINT,        ///< A trackpoint style mouse. @SINCE_1_2.60
  TRACKBALL,         ///< A trackball style mouse. @SINCE_1_2.60
  REMOCON,           ///< A remote controller. @SINCE_1_2.61
  VIRTUAL_KEYBOARD,  ///< A virtual keyboard. @SINCE_1_2.61
};

} // namespace Subclass

} // namespace Device

/**
 * @}
 */
} // namespace Dali

#endif  //DALI_KEY_EVENT_DEVEL_H
