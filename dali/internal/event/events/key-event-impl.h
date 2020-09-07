#ifndef DALI_INTERNAL_KEY_EVENT_H
#define DALI_INTERNAL_KEY_EVENT_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/base-object.h>

namespace Dali
{

namespace Internal
{

class KeyEvent;
typedef IntrusivePtr< KeyEvent > KeyEventPtr;

/**
 * @copydoc Dali::KeyEvent
 */
class KeyEvent : public BaseObject
{
public:

  /**
   * @brief Default constructor.
   */
  KeyEvent();

  /**
   * @brief Constructor.
   *
   * @param[in]  keyName         The name of the key pressed or command from the IMF, if later then the some following parameters will be needed.
   * @param[in]  logicalKey      The logical key symbol (eg. shift + 1 == "exclamation")
   * @param[in]  keyString       The string of input characters or key pressed
   * @param[in]  keyCode         The unique key code for the key pressed.
   * @param[in]  keyModifier     The key modifier for special keys like shift and alt
   * @param[in]  timeStamp       The time (in ms) that the key event occurred.
   * @param[in]  keyState        The state of the key event.
   * @param[in]  compose         The key compose
   * @param[in]  deviceName      The name of device the key event originated from
   * @param[in]  deviceClass     The class of device the key event originated from
   * @param[in]  deviceSubclass  The subclass of device the key event originated from
   */
  KeyEvent( const std::string& keyName,
            const std::string& logicalKey,
            const std::string& keyString,
            int keyCode,
            int keyModifier,
            unsigned long timeStamp,
            const Dali::KeyEvent::State& keyState,
            const std::string& compose,
            const std::string& deviceName,
            const Device::Class::Type deviceClass,
            const Device::Subclass::Type deviceSubclass );

  /**
   * Create a new KeyEvent.
   *
   * @param[in]  keyName         The name of the key pressed or command from the IMF, if later then the some following parameters will be needed.
   * @param[in]  logicalKey      The logical key symbol (eg. shift + 1 == "exclamation")
   * @param[in]  keyString       The string of input characters or key pressed
   * @param[in]  keyCode         The unique key code for the key pressed.
   * @param[in]  keyModifier     The key modifier for special keys like shift and alt
   * @param[in]  timeStamp       The time (in ms) that the key event occurred.
   * @param[in]  keyState        The state of the key event.
   * @param[in]  compose         The key compose
   * @param[in]  deviceName      The name of device the key event originated from
   * @param[in]  deviceClass     The class of device the key event originated from
   * @param[in]  deviceSubclass  The subclass of device the key event originated from
   * @return A smart-pointer to the newly allocated KeyEvent.
   */
  static KeyEventPtr New( const std::string& keyName,
                          const std::string& logicalKey,
                          const std::string& keyString,
                          int keyCode,
                          int keyModifier,
                          unsigned long timeStamp,
                          const Dali::KeyEvent::State& keyState,
                          const std::string& compose,
                          const std::string& deviceName,
                          const Device::Class::Type deviceClass,
                          const Device::Subclass::Type deviceSubclass );

  /**
   * @copydoc Dali::KeyEvent::IsShiftModifier()
   */
  bool IsShiftModifier() const;

  /**
   * @copydoc Dali::KeyEvent::IsCtrlModifier()
   */
  bool IsCtrlModifier() const;

  /**
   * @copydoc Dali::KeyEvent::IsAltModifier()
   */
  bool IsAltModifier() const;

  /**
   * @copydoc Dali::KeyEvent::GetCompose()
   */
  const std::string& GetCompose() const;

  /**
   * @copydoc Dali::KeyEvent::GetDeviceName()
   */
  const std::string& GetDeviceName() const;

  /**
   * @copydoc Dali::KeyEvent::GetDeviceClass()
   */
  Device::Class::Type GetDeviceClass() const;

  /**
   * @copydoc Dali::KeyEvent::GetDeviceSubclass()
   */
  Device::Subclass::Type GetDeviceSubclass() const;

  /**
   * @copydoc Dali::KeyEvent::GetKeyName()
   */
  const std::string& GetKeyName() const;

  /**
   * @copydoc Dali::KeyEvent::GetKeyString()
   */
  const std::string& GetKeyString() const;

  /**
   * @copydoc Dali::KeyEvent::GetLogicalKey()
   */
  const std::string& GetLogicalKey() const;

  /**
   * @copydoc Dali::KeyEvent::GetKeyCode()
   */
  int32_t GetKeyCode() const;

  /**
   * @copydoc Dali::KeyEvent::GetKeyModifier()
   */
  int32_t GetKeyModifier() const;

  /**
   * @copydoc Dali::KeyEvent::GetTime()
   */
  unsigned long GetTime() const;

  /**
   * @copydoc Dali::KeyEvent::GetState()
   */
  Dali::KeyEvent::State GetState() const;

  /**
   * @brief Set the name given to the key pressed
   *
   * @param[in] keyName The name given to the key pressed.
   */
  void SetKeyName( const std::string& keyName );

  /**
   * @brief Set the actual string of input characters that should be used for input editors.
   *
   * @param[in] The actual string of input characters
   */
  void SetKeyString( const std::string& keyString );

  /**
   * @brief Set the unique key code for the key pressed.
   *
   * @param[in] keyCode The unique key code for the key pressed
   */
  void SetKeyCode( int32_t keyCode );

  /**
   * @brief Set the key modifier for special keys like Shift, Alt and Ctrl which modify the next key pressed.
   *
   * @param[in] keyModifier The key modifier
   */
  void SetKeyModifier( int32_t keyModifier );

  /**
   * @brief Set the time (in ms) that the key event occurred.
   *
   * @param[in] time The time (in ms)
   */
  void SetTime( unsigned long time );

  /**
   * @brief Set the state of the key event.
   *
   * @param[in] state The state of the key event
   */
  void SetState( const Dali::KeyEvent::State& state );

private:

  /**
   * @brief Destructor.
   *
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~KeyEvent() override = default;

  // Not copyable or movable

  KeyEvent( const KeyEvent& rhs ) = delete;             ///< Deleted copy constructor
  KeyEvent( KeyEvent&& rhs ) = delete;                  ///< Deleted move constructor
  KeyEvent& operator=( const KeyEvent& rhs ) = delete;  ///< Deleted copy assignment operator
  KeyEvent& operator=( KeyEvent&& rhs ) = delete;       ///< Deleted move assignment operator

private:

  std::string mKeyName;                      ///< The name of the key pressed
  std::string mLogicalKey;                   ///< The logical key symbol
  std::string mKeyString;                    ///< The string of input characters
  int  mKeyCode;                             ///< TThe unique key code
  int  mKeyModifier;                         ///< The key modifier
  unsigned long mTime;                       ///< The time that the key event occurred.
  Dali::KeyEvent::State mState;              ///< The state of the key event.
  std::string mCompose;                      ///< The key compose
  std::string mDeviceName;                   ///< The name of device the key event originated from
  Device::Class::Type mDeviceClass;          ///< The class of device the key event originated from
  Device::Subclass::Type mDeviceSubclass;    ///< The subclass of device the key event originated from
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::KeyEvent& GetImplementation( Dali::KeyEvent& keyEvent )
{
  DALI_ASSERT_ALWAYS( keyEvent && "Key Event handle is empty" );

  BaseObject& object = keyEvent.GetBaseObject();

  return static_cast< Internal::KeyEvent& >( object );
}

inline const Internal::KeyEvent& GetImplementation( const Dali::KeyEvent& keyEvent )
{
  DALI_ASSERT_ALWAYS( keyEvent && "Key Event handle is empty" );

  const BaseObject& object = keyEvent.GetBaseObject();

  return static_cast< const Internal::KeyEvent& >( object );
}

} // namespace Dali

#endif // DALI_INTERNAL_KEY_EVENT_H
