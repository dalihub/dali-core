#ifndef DALI_INTEGRATION_KEY_EVENT_H
#define DALI_INTEGRATION_KEY_EVENT_H

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
    DOWN, /**< Key down */
    UP,   /**< Key up */
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
  KeyEvent(const Dali::String&          keyName,
           const Dali::String&          logicalKey,
           const Dali::String&          keyString,
           int32_t                      keyCode,
           int32_t                      keyModifier,
           uint32_t                     timeStamp,
           const State&                 keyState,
           const Dali::String&          compose,
           const Dali::String&          deviceName,
           const Device::Class::Type    deviceClass,
           const Device::Subclass::Type deviceSubclass);

  /**
   * @brief Constructor which creates a KeyEvent instance from a Dali::KeyEvent.
   *
   * Every field is carried over, so an event that is fed back into the core keeps the
   * state it was given. @see Dali::Integration::SceneHolder::FeedKeyEvent()
   *
   * @param[in] keyEvent The key event to copy from.
   */
  explicit KeyEvent(const Dali::KeyEvent& keyEvent);

  /**
   * Copy constructor.
   */
  KeyEvent(const KeyEvent& rhs);

  /**
   * Virtual destructor
   */
  ~KeyEvent() override;

  // Data

  /**
   * The name of the key pressed or command from the IMF
   */
  Dali::String keyName;

  /**
   * The logical key symbol
   */
  Dali::String logicalKey;

  /**
   * The string of input characters or key pressed
   */
  Dali::String keyString;

  /**
   * The unique key code for the key pressed.
   */
  int32_t keyCode;

  /**
   * The key modifier for special keys like shift and alt
   */
  int32_t keyModifier;

  /**
   * The time (in ms) that the key event occurred.
   */
  uint32_t time;

  /**
   * State of the key event.
   * @see State
   */
  State state;

  /**
   * If this keystroke has modified a string in the middle of being composed - this string replaces the previous one.
   */
  Dali::String compose;

  /**
   * The name of device KeyEvent originated from
   */
  Dali::String deviceName;

  /**
   * The class of device KeyEvent originated from
   */
  Device::Class::Type deviceClass;

  /**
   * The subclass of device KeyEvent originated from
   */
  Device::Subclass::Type deviceSubclass;

  /**
   * Whether the key referenced by the event is a repeating key.
   */
  bool isRepeat;

  /**
   * Whether the key event has already been through the intercept stage.
   *
   * Set when accessibility feeds an unconsumed key event back to the window; it stops the
   * intercept stage from running a second time and looping forever. Anything that rebuilds
   * a KeyEvent on the way back into the core must carry this over.
   */
  bool interceptProcessed;

  /**
   * Window id where key event occurred.
   */
  uint32_t windowId;

  /**
   * The time (in ms)  when key event was received.
   */
  uint32_t receiveTime;
};

} // namespace Integration

} // namespace Dali

#endif // DALI_INTEGRATION_KEY_EVENT_H
