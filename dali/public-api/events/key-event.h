#ifndef DALI_KEY_EVENT_H
#define DALI_KEY_EVENT_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <cstdint> // int32_t
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/events/device.h>
#include <dali/public-api/object/base-handle.h>

namespace Dali
{
namespace Internal DALI_INTERNAL
{
class KeyEvent;
}

/**
 * @addtogroup dali_core_events
 * @{
 */

/**
 * @brief The key event class is used to store a key press.
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
class DALI_CORE_API KeyEvent : public BaseHandle
{
public:
  // Enumerations

  /**
   * @brief Enumeration for specifying the state of the key event.
   * @SINCE_1_9.27
   */
  enum State
  {
    DOWN, ///< Key down @SINCE_1_9.27
    UP,   ///< Key up @SINCE_1_9.27
  };

  /**
   * @brief An uninitialized KeyEvent instance.
   *
   * Calling member functions with an uninitialized KeyEvent handle is not allowed.
   * @SINCE_1_0.0
   */
  KeyEvent();

  /**
   * @brief Copy constructor.
   * @SINCE_1_2.36
   * @param[in] rhs A reference to the copied handle
   */
  KeyEvent(const KeyEvent& rhs);

  /**
   * @brief Move constructor.
   *
   * @SINCE_1_9.27
   * @param[in] rhs A reference to the moved handle
   */
  KeyEvent(KeyEvent&& rhs) noexcept;

  /**
   * @brief Copy assignment operator.
   * @SINCE_1_2.36
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  KeyEvent& operator=(const KeyEvent& rhs);

  /**
   * @brief Move assignment operator.
   *
   * @SINCE_1_9.27
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this
   */
  KeyEvent& operator=(KeyEvent&& rhs) noexcept;

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
   * @brief Checks if the no-intercept key modifier has been supplied.
   *
   * @SINCE_2_3.13
   * @return True if no-intercept modifier is present
   */
  bool IsNoInterceptModifier() const;

  /**
   * @brief Get the key compose string.
   *
   * @SINCE_1_9.27
   * @return The compose string
   */
  const std::string& GetCompose() const;

  /**
   * @brief Get the device name the key event originated from.
   *
   * @SINCE_1_9.27
   * @return The device name
   */
  const std::string& GetDeviceName() const;

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

  /**
   * @brief Get the name given to the key pressed or command from the IMF
   *
   * @SINCE_1_9.27
   * @return The name given to the key pressed.
   */
  const std::string& GetKeyName() const;

  /**
   * @brief Get the actual string of input characters that should be used for input editors.
   *
   * @SINCE_1_9.27
   * @return The actual string of input characters
   */
  const std::string& GetKeyString() const;

  /**
   * @brief Gets the logical key string.
   *
   * For example, when the user presses 'shift' key and '1' key together, the logical key is "exclamation".
   * Plus, the key name is "1", and the key string is "!".
   *
   * @SINCE_1_9.27
   * @return The logical key symbol
   */
  const std::string& GetLogicalKey() const;

  /**
   * @brief Get the unique key code for the key pressed.
   *
   * @SINCE_1_9.27
   * @return The unique key code for the key pressed
   *
   * @remarks We recommend not to use this key code value directly because its meaning
   * might be changed in the future. Currently, it means a platform-specific key code.
   * You need to use IsKey() to know what a key event means instead of direct comparison
   * of key code value.
   */
  int32_t GetKeyCode() const;

  /**
   * @brief Return the key modifier for special keys like Shift, Alt and Ctrl which modify the next key pressed.
   *
   * @SINCE_1_9.27
   * @return The key modifier
   */
  int32_t GetKeyModifier() const;

  /**
   * @brief Get the time (in ms) that the key event occurred.
   *
   * @SINCE_1_9.27
   * @return The time (in ms)
   */
  unsigned long GetTime() const;

  /**
   * @brief Get the state of the key event.
   *
   * @see State
   *
   * @SINCE_1_9.27
   * @return The state of the key event
   */
  State GetState() const;

  /**
   * @brief Checks to see if key event is a repeating key.
   *
   * @SINCE_2_2.44
   * @return Whether the key event is a repeating key.
   */
  bool IsRepeat() const;

  /**
   * @brief Gets the Window Id where key event occurred.
   *
   * @SINCE_2_3.5
   * @return The window id
   */
  uint32_t GetWindowId() const;

  /**
   * @brief Gets the time when the key event was received.
   * @SINCE_2_3.52
   */
  uint32_t GetReceiveTime() const;

public: // Not intended for application developers
  /// @cond internal
  /**
   * @brief This constructor is used internally to Create an initialized KeyEvent handle.
   *
   * @SINCE_1_9.27
   * @param[in] keyEvent A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL KeyEvent(Internal::KeyEvent* keyEvent);
  /// @endcond
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_KEY_EVENT_H
