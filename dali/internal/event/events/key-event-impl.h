#ifndef __DALI_INTERNAL_KEY_EVENT_H__
#define __DALI_INTERNAL_KEY_EVENT_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/events/key-event.h>

namespace Dali
{

namespace Internal
{

struct KeyEventImpl
{
public:

  /**
   * @brief Constructor.
   */
  KeyEventImpl( Dali::KeyEvent* keyEvent );

  /**
   * @brief Destructor.
   */
  ~KeyEventImpl();

  /**
   * @brief Assignment operator.
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  KeyEventImpl& operator=( const KeyEventImpl& rhs );

  /**
   * @brief Get the compose string.
   *
   * @return The compose string.
   */
   std::string GetCompose() const;

  /**
   * @brief Set the compose string to the KeyEvent
   * A string if this keystroke has modified a string in the middle of being composed,
   * this string replaces the previous one.
   *
   * @param[in] compose The compose string to set
   */
  void SetCompose( const std::string& compose );

  /**
   * @brief Get the device name the key event originated from
   *
   * @return The device name
   */
   std::string GetDeviceName() const;

  /**
   * @brief Set the device name to the KeyEvent
   *
   * @param[in] deviceName Device name string to set
   */
  void SetDeviceName( const std::string& deviceName );

  /**
   * @brief Get the device class the key event originated from
   *
   * @return The device class
   */
  Device::Class::Type GetDeviceClass() const;

  /**
   * @brief Set the device class to the KeyEvent
   *
   * @param[in] deviceClass Device class to set
   */
  void SetDeviceClass( Device::Class::Type deviceClass );

  /**
   * @brief Get the device subclass the key event originated from
   *
   * @return The device subclass
   */
  Device::Subclass::Type GetDeviceSubclass() const;

  /**
   * @brief Set the device subclass to the KeyEvent
   *
   * @param[in] deviceClass Device subclass to set
   */
  void SetDeviceSubclass( Device::Subclass::Type deviceSubclass );

private:

  // Undefined
  KeyEventImpl();

  // Undefined
  KeyEventImpl( const KeyEventImpl& rhs );

private:

  std::string mCompose;
  std::string mDeviceName;
  Device::Class::Type mDeviceClass;
  Device::Subclass::Type mDeviceSubclass;
};

} // namespace Internal

// Helpers for public-api forwarding methods

Internal::KeyEventImpl* GetImplementation( KeyEvent* keyEvent );

const Internal::KeyEventImpl* GetImplementation( const KeyEvent* keyEvent );

} // namespace Dali

#endif // __DALI_INTERNAL_KEY_EVENT_H__
