#ifndef DALI_KEY_EVENT_DEVEL_H
#define DALI_KEY_EVENT_DEVEL_H

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

// INTERNAL INCLUDES
#include <dali/public-api/events/key-event.h>

namespace Dali
{
namespace DevelKeyEvent
{
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
DALI_CORE_API KeyEvent New(const Dali::String&          keyName,
                           const Dali::String&          logicalKey,
                           const Dali::String&          keyString,
                           int32_t                      keyCode,
                           int32_t                      keyModifier,
                           uint32_t                     timeStamp,
                           const Dali::KeyEvent::State& keyState,
                           const Dali::String&          compose,
                           const Dali::String&          deviceName,
                           const Device::Class::Type    deviceClass,
                           const Device::Subclass::Type deviceSubclass);

/**
 * @brief Queries whether the key event has already been through the intercept stage.
 *
 * @param[in] keyEvent The instance of KeyEvent
 * @return True if the key event has already been through the intercept stage
 */
DALI_CORE_API bool IsInterceptProcessed(const KeyEvent& keyEvent);

/**
 * @brief Marks the key event as having been through the intercept stage.
 *
 * Accessibility hands a key event to its remote client and consumes it. If the client
 * does not consume it, the event is marked and fed back to the window; the mark stops
 * the intercept stage from running a second time and looping forever.
 *
 * @param[in] keyEvent The instance of KeyEvent
 * @param[in] interceptProcessed Whether the key event has been through the intercept stage
 */
DALI_CORE_API void SetInterceptProcessed(KeyEvent keyEvent, bool interceptProcessed);

} // namespace DevelKeyEvent

} // namespace Dali

#endif // DALI_KEY_EVENT_DEVEL_H
