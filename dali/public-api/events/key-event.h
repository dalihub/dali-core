#ifndef __DALI_KEY_EVENT_H__
#define __DALI_KEY_EVENT_H__

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

// EXTERNAL INCLUDES
#include <string>
#include <cstdint> // int32_t

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/events/device.h>

namespace Dali
{
/**
 * @addtogroup dali_core_events
 * @{
 */

/**
 * @brief The key event structure is used to store a key press.
 *
 * It facilitates processing of these key presses and passing to other
 * libraries like Toolkit. The keyString is the actual character you
 * might want to display while the key name is just a descriptive
 * name.  There is a key modifier which relates to keys like alt,
 * shift and control functions are supplied to check if they have been
 * pressed.
 *
 * Currently KeyEvent is also being used to relay messages from the
 * IMF(Input Method Framework) keyboard to the internal core. In future IMF may communicate via its own
 * module.
 * @SINCE_1_0.0
 */
struct DALI_CORE_API KeyEvent
{
  // Enumerations

  /**
   * @brief Enumeration for specifying the state of the key event.
   * @SINCE_1_0.0
   */
  enum State
  {
    Down,        ///< Key down @SINCE_1_0.0
    Up,          ///< Key up @SINCE_1_0.0
    Last
  };

  /**
   * @brief Default constructor.
   * @SINCE_1_0.0
   */
  KeyEvent();

  /**
   * @brief Constructor.
   *
   * @SINCE_1_0.0
   * @param[in] keyName The name of the key pressed or command from the IMF, if later then the some following parameters will be needed
   * @param[in] keyString A string of input characters or key pressed
   * @param[in] keyCode The unique key code for the key pressed
   * @param[in] keyModifier The key modifier for special keys like shift and alt
   * @param[in] timeStamp The time (in ms) that the key event occurred
   * @param[in] keyState The state of the key event
   */
  KeyEvent(const std::string& keyName, const std::string& keyString, int32_t keyCode, int32_t keyModifier, unsigned long timeStamp, const State& keyState);

  /**
   * @brief Copy constructor.
   * @SINCE_1_2.36
   * @param[in] rhs A reference to the copied handle
   */
  KeyEvent( const KeyEvent& rhs );

  /**
   * @brief Assignment operator.
   * @SINCE_1_2.36
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  KeyEvent& operator=( const KeyEvent& rhs );

  /**
   * @brief Destructor.
   * @SINCE_1_0.0
   */
  ~KeyEvent();

  /**
   * @brief Checks to see if Shift key modifier has been supplied.
   *
   * @SINCE_1_0.0
   * @return True if shift modifier
   */
  bool IsShiftModifier() const;

  /**
   * @brief Checks to see if Ctrl (control) key modifier has been supplied.
   *
   * @SINCE_1_0.0
   * @return True if ctrl modifier
   */
  bool IsCtrlModifier() const;

  /**
   * @brief Checks to see if Alt key modifier has been supplied.
   *
   * @SINCE_1_0.0
   * @return True if alt modifier
   */
  bool IsAltModifier() const;

  /**
   * @brief Get the key compose string.
   *
   * @SINCE_1_3.22
   * @return The compose string
   */
  std::string GetCompose() const;

  /**
   * @brief Get the device name the key event originated from.
   *
   * @SINCE_1_2.60
   * @return The device name
   */
  std::string GetDeviceName() const;

  /**
   * @brief Get the device class the key event originated from.
   *
   * The device class type is classification type of the input device of event received
   * @SINCE_1_2.60
   * @return The type of the device class
   */
  Device::Class::Type GetDeviceClass() const;

  /**
   * @brief Get the device subclass the key event originated from.
   *
   * The device subclass type is subclassification type of the input device of event received.
   * @SINCE_1_2.60
   * @return The type of the device subclass
   */
  Device::Subclass::Type GetDeviceSubclass() const;

  // Data

  /**
   * @brief Name given to the key pressed.
   */
  std::string keyPressedName;

  /**
   * @brief The actual string returned that should be used for input editors.
   */
  std::string keyPressed;

  /**
   * @brief Keycode for the key pressed.
   *
   * @remarks We recommend not to use this key code value
   * directly because its meaning might be changed in the future. Currently, it means a
   * platform-specific key code. You need to use IsKey() to know what a key event means
   * instead of direct comparison of key code value.
   */
  int32_t keyCode;

  /**
   * @brief special keys like shift, alt and control which modify the next key pressed.
   */
  int32_t keyModifier;

  /**
   * @brief The time (in ms) that the key event occurred.
   */
  unsigned long time;

  /**
   * @brief State of the key event.
   *
   * @see State
   */
  State state;

};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_KEY_EVENT_H__
