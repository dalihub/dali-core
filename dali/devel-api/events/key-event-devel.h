#ifndef DALI_KEY_EVENT_DEVEL_H
#define DALI_KEY_EVENT_DEVEL_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
namespace DevelKeyEvent
{
/**
 * @brief Creates an initialized KeyEvent as default.
 *
 * @SINCE_2_2.45
 * @return A handle to a newly allocated Dali resource
 */
DALI_CORE_API KeyEvent New();

/**
 * @brief Creates an initialized KeyEvent.
 *
 * @SINCE_1_9.27
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
 * @return A handle to a newly allocated Dali resource
 */
DALI_CORE_API KeyEvent New(const std::string&           keyName,
                           const std::string&           logicalKey,
                           const std::string&           keyString,
                           int                          keyCode,
                           int                          keyModifier,
                           unsigned long                timeStamp,
                           const Dali::KeyEvent::State& keyState,
                           const std::string&           compose,
                           const std::string&           deviceName,
                           const Device::Class::Type    deviceClass,
                           const Device::Subclass::Type deviceSubclass);

/**
 * @brief Set the name given to the key pressed
 *
 * @SINCE_1_9.27
 * @param[in] keyEvent The instance of KeyEvent.
 * @param[in] keyName The name given to the key pressed.
 */
DALI_CORE_API void SetKeyName(KeyEvent keyEvent, const std::string& keyName);

/**
 * @brief Set the actual string of input characters that should be used for input editors.
 *
 * @SINCE_1_9.27
 * @param[in] keyEvent The instance of KeyEvent.
 * @param[in] keyString The actual string of input characters
 */
DALI_CORE_API void SetKeyString(KeyEvent keyEvent, const std::string& keyString);

/**
 * @brief Set the unique key code for the key pressed.
 *
 * @SINCE_1_9.27
 * @param[in] keyEvent The instance of KeyEvent.
 * @param[in] keyCode The unique key code for the key pressed
 */
DALI_CORE_API void SetKeyCode(KeyEvent keyEvent, int32_t keyCode);

/**
 * @brief Set the key modifier for special keys like Shift, Alt and Ctrl which modify the next key pressed.
 *
 * @SINCE_1_9.27
 * @param[in] keyEvent The instance of KeyEvent.
 * @param[in] keyModifier The key modifier
 */
DALI_CORE_API void SetKeyModifier(KeyEvent keyEvent, int32_t keyModifier);

/**
 * @brief Set the time (in ms) that the key event occurred.
 *
 * @SINCE_1_9.27
 * @param[in] keyEvent The instance of KeyEvent.
 * @param[in] time The time (in ms)
 */
DALI_CORE_API void SetTime(KeyEvent keyEvent, unsigned long time);

/**
 * @brief Set the state of the key event.
 *
 * @SINCE_1_9.27
 * @param[in] keyEvent The instance of KeyEvent.
 * @param[in] state The state of the key event
 */
DALI_CORE_API void SetState(KeyEvent keyEvent, const KeyEvent::State& state);

/**
 * @brief Set whether to repeat key event.
 *
 * @SINCE_2_2.44
 * @param[in] keyEvent The instance of KeyEvent.
 * @param[in] repeat Whether the key event is a repeating key.
 */
DALI_CORE_API void SetRepeat(KeyEvent keyEvent, const bool repeat);

} // namespace DevelKeyEvent

} // namespace Dali

#endif // DALI_KEY_EVENT_DEVEL_H
