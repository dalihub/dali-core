#ifndef DALI_KEY_EVENT_H
#define DALI_KEY_EVENT_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/dali-string.h>
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
   * @brief Enumeration for the modifier bits held by the key modifier.
   *
   * The values can be combined with a bitwise OR and passed to SetKeyModifier(),
   * and the result of GetKeyModifier() can be tested against them.
   * @SINCE_2_5.34
   */
  enum Modifier
  {
    SHIFT = 0x1, ///< The Shift key modifier @SINCE_2_5.34
    CTRL  = 0x2, ///< The Ctrl (control) key modifier @SINCE_2_5.34
    ALT   = 0x4, ///< The Alt key modifier @SINCE_2_5.34
  };

  /**
   * @brief Creates an initialized KeyEvent with default values.
   *
   * The details of the key event are filled in with the setters.
   *
   * @SINCE_2_5.34
   * @return A handle to a newly allocated Dali resource
   */
  static KeyEvent New();

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
   * @brief Get the key compose string.
   *
   * @SINCE_1_9.27
   * @return The compose string
   */
  const Dali::String& GetCompose() const;

  /**
   * @brief Get the device name the key event originated from.
   *
   * @SINCE_1_9.27
   * @return The device name
   */
  const Dali::String& GetDeviceName() const;

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
  const Dali::String& GetKeyName() const;

  /**
   * @brief Get the actual string of input characters that should be used for input editors.
   *
   * @SINCE_1_9.27
   * @return The actual string of input characters
   */
  const Dali::String& GetKeyString() const;

  /**
   * @brief Gets the logical key string.
   *
   * For example, when the user presses 'shift' key and '1' key together, the logical key is "exclamation".
   * Plus, the key name is "1", and the key string is "!".
   *
   * @SINCE_1_9.27
   * @return The logical key symbol
   */
  const Dali::String& GetLogicalKey() const;

  /**
   * @brief Get the unique key code for the key pressed.
   *
   * @SINCE_1_9.27
   * @return The unique key code for the key pressed
   *
   * @remarks Named keys (e.g. Return, Back, arrow and media keys) resolve to the same
   * DALI_KEY_* value on every platform, but other keys remain platform-specific and
   * may change. Prefer IsKey() over comparing this value directly.
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
  uint32_t GetTime() const;

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

  /**
   * @brief Set the key compose string.
   *
   * @SINCE_2_5.34
   * @param[in] compose The compose string
   */
  void SetCompose(const Dali::String& compose);

  /**
   * @brief Set the device name the key event originated from.
   *
   * @SINCE_2_5.34
   * @param[in] deviceName The device name
   */
  void SetDeviceName(const Dali::String& deviceName);

  /**
   * @brief Set the device class the key event originated from.
   *
   * @SINCE_2_5.34
   * @param[in] deviceClass The type of the device class
   */
  void SetDeviceClass(Device::Class::Type deviceClass);

  /**
   * @brief Set the device subclass the key event originated from.
   *
   * @SINCE_2_5.34
   * @param[in] deviceSubclass The type of the device subclass
   */
  void SetDeviceSubclass(Device::Subclass::Type deviceSubclass);

  /**
   * @brief Set the name given to the key pressed.
   *
   * @SINCE_2_5.34
   * @param[in] keyName The name given to the key pressed
   */
  void SetKeyName(const Dali::String& keyName);

  /**
   * @brief Set the actual string of input characters that should be used for input editors.
   *
   * @SINCE_2_5.34
   * @param[in] keyString The actual string of input characters
   */
  void SetKeyString(const Dali::String& keyString);

  /**
   * @brief Set the logical key string.
   *
   * @see GetLogicalKey()
   *
   * @SINCE_2_5.34
   * @param[in] logicalKey The logical key symbol
   */
  void SetLogicalKey(const Dali::String& logicalKey);

  /**
   * @brief Set the unique key code for the key pressed.
   *
   * @SINCE_2_5.34
   * @param[in] keyCode The unique key code for the key pressed
   */
  void SetKeyCode(int32_t keyCode);

  /**
   * @brief Set the key modifier for special keys like Shift, Alt and Ctrl which modify the next key pressed.
   *
   * @see Modifier
   *
   * @SINCE_2_5.34
   * @param[in] keyModifier The key modifier, a bitwise OR of Modifier values
   */
  void SetKeyModifier(int32_t keyModifier);

  /**
   * @brief Set the time (in ms) that the key event occurred.
   *
   * @SINCE_2_5.34
   * @param[in] time The time (in ms)
   */
  void SetTime(uint32_t time);

  /**
   * @brief Set the state of the key event.
   *
   * @see State
   *
   * @SINCE_2_5.34
   * @param[in] state The state of the key event
   */
  void SetState(State state);

  /**
   * @brief Set whether the key event is a repeating key.
   *
   * @SINCE_2_5.34
   * @param[in] repeat Whether the key event is a repeating key
   */
  void SetRepeat(bool repeat);

  /**
   * @brief Sets the Window Id where the key event occurred.
   *
   * @SINCE_2_5.34
   * @param[in] windowId The window id where the key event occurred
   */
  void SetWindowId(uint32_t windowId);

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
