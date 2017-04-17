#ifndef DALI_KEY_EVENT_DEVEL_H
#define DALI_KEY_EVENT_DEVEL_H

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
#include <dali/public-api/events/key-event.h>

namespace Dali
{

namespace DevelKeyEvent
{

namespace DeviceClass
{

/**
 * @brief An enum of Device Classe types.
 */
enum Type
{
  NONE,      ///< Not a device
  SEAT,      ///< The user/seat (the user themselves)
  KEYBOARD,  ///< A regular keyboard, numberpad or attached buttons
  MOUSE,     ///< A mouse, trackball or touchpad relative motion device
  TOUCH,     ///< A touchscreen with fingers or stylus
  PEN,       ///< A special pen device
  POINTER,   ///< A laser pointer, wii-style or 7"minority report" pointing device
  GAMEPAD    ///< A gamepad controller or joystick
};

} // namespace DeviceClass

/**
 * @brief Get the device name the key event originated from
 *
 * @param[in] keyEvent The KeyEvent to retrieve the device name from
 * @return The device name
 */
DALI_IMPORT_API std::string GetDeviceName( const KeyEvent& keyEvent );

/**
 * @brief Set the device name to the KeyEvent
 *
 * @param[in] keyEvent The KeyEvent to set the device name on
 * @param[in] deviceName Device name string to set
 */
DALI_IMPORT_API void SetDeviceName( KeyEvent& keyEvent, const std::string& deviceName );

/**
 * @brief Get the device class the key event originated from
 *
 * @param[in] keyEvent The KeyEvent to retrieve the device class from
 * @return The device class
 */
DALI_IMPORT_API DeviceClass::Type GetDeviceClass( const KeyEvent& keyEvent );

/**
 * @brief Set the device class to the KeyEvent
 *
 * @param[in] keyEvent The KeyEvent to set the device class on
 * @param[in] deviceClass Device class to set
 */
DALI_IMPORT_API void SetDeviceClass( KeyEvent& keyEvent, const DeviceClass::Type& deviceClass );

} // namespace DevelKeyEvent

} // namespace Dali

#endif  //DALI_KEY_EVENT_DEVEL_H
