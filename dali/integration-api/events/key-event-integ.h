#ifndef DALI_INTEGRATION_KEY_EVENT_H
#define DALI_INTEGRATION_KEY_EVENT_H

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/integration-api/events/event.h>
#include <dali/public-api/events/key-event.h>

namespace Dali
{

namespace Integration
{

/**
 * An instance of this class should be used by the adaptor to send a key event to
 * the Dali core.
 *
 */
struct DALI_CORE_API KeyEvent : public Event
{
  // Enumerations
  // Specifies the state of the key event.
  enum State
  {
    DOWN,        /**< Key down */
    UP,          /**< Key up */
  };

  /**
   * Default Constructor
   */
  KeyEvent();

  /**
   * Constructor
   * @param[in]  keyName         The name of the key pressed or command from the IMF, if later then the some following parameters will be needed.
   * @param[in]  logicalKey      The logical key symbol (eg. shift + 1 == "exclamation")
   * @param[in]  keyString       The string of input characters or key pressed
   * @param[in]  keyCode         The unique key code for the key pressed.
   * @param[in]  keyModifier     The key modifier for special keys like shift and alt
   * @param[in]  timeStamp       The time (in ms) that the key event occurred.
   * @param[in]  keyState        The state of the key event.
   * @param[in]  compose         The key compose
   * @param[in]  deviceName      The name of device KeyEvent originated from
   * @param[in]  deviceClass     The class of device KeyEvent originated from
   * @param[in]  deviceSubclass  The subclass of device KeyEvent originated from
   */
  KeyEvent(const std::string& keyName,
           const std::string& logicalKey,
           const std::string& keyString,
           int keyCode,
           int keyModifier,
           unsigned long timeStamp,
           const State& keyState,
           const std::string& compose,
           const std::string& deviceName,
           const Device::Class::Type deviceClass,
           const Device::Subclass::Type deviceSubclass );

  /**
   * Virtual destructor
   */
  ~KeyEvent() override;

  // Data

  /**
   * The name of the key pressed or command from the IMF
   */
  std::string keyName;

  /**
   * The logical key symbol
   */
  std::string logicalKey;

  /**
   * The string of input characters or key pressed
   */
  std::string keyString;

  /**
   * The unique key code for the key pressed.
   */
  int  keyCode;

  /**
   * The key modifier for special keys like shift and alt
   */
  int  keyModifier;

  /**
   * The time (in ms) that the key event occurred.
   */
  unsigned long time;

  /**
   * State of the key event.
   * @see State
   */
  State state;

  /**
   * If this keystroke has modified a string in the middle of being composed - this string replaces the previous one.
   */
  std::string compose;

  /**
   * The name of device KeyEvent originated from
   */
  std::string deviceName;

  /**
   * The class of device KeyEvent originated from
   */
  Device::Class::Type deviceClass;

  /**
   * The subclass of device KeyEvent originated from
   */
  Device::Subclass::Type deviceSubclass;
};

} // namespace Integration

} // namespace Dali

#endif // DALI_INTEGRATION_KEY_EVENT_H
