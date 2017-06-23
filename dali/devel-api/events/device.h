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
 * @brief Information about device types
 *
 * These types are for widget viewer applications.
 * A widget viewer application by Dali have to deliver events to widget applications
 * Server for handling events actually delivers any events to widget applications
 * The server needs to know input device information such as type to deliver events of proper type to widget applications
 */
namespace DevelDevice
{

/**
 * @brief Categorization type about input Device
 */
namespace Class
{

/**
 * @brief An enum of Device Class types.
 */
enum Type
{
  NONE,      ///< Not a device
  USER,      ///< The user/seat (the user themselves)
  KEYBOARD,  ///< A regular keyboard, numberpad or attached buttons
  MOUSE,     ///< A mouse, trackball or touchpad relative motion device
  TOUCH,     ///< A touchscreen with fingers or stylus
  PEN,       ///< A special pen device
  POINTER,   ///< A pointing device based on laser, infrared or similar technology
  GAMEPAD    ///< A gamepad controller or joystick
};

} // namespace DeviceClass

/**
 * @brief Subcategorization type about input device
 */
namespace Subclass
{

/**
 * @brief An enum of Device Subclass types.
 */
enum Type
{
  NONE, ///< Not a device
  FINGER, ///< The normal flat of your finger
  FINGERNAIL, ///< A fingernail
  KNUCKLE, ///< A Knuckle
  PALM, ///< The palm of a users hand
  HAND_SIDE, ///< The side of your hand
  HAND_FLAT, ///< The flat of your hand
  PEN_TIP, ///< The tip of a pen
  TRACKPAD, ///< A trackpad style mouse
  TRACKPOINT, //< A trackpoint style mouse
  TRACKBALL, ///< A trackball style mouse
};

} // namespace DeviceSubclass

} // namespace DevelDevice

} // namespace Dali

#endif  //DALI_KEY_EVENT_DEVEL_H
