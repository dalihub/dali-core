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
#include <dali/devel-api/events/key-event-devel.h>
#include <dali/internal/event/events/key-event-impl.h>

namespace Dali
{
namespace DevelKeyEvent
{
KeyEvent New()
{
  Internal::KeyEventPtr internal = Internal::KeyEvent::New();

  return KeyEvent(internal.Get());
}

KeyEvent New(const std::string&           keyName,
             const std::string&           logicalKey,
             const std::string&           keyString,
             int                          keyCode,
             int                          keyModifier,
             unsigned long                timeStamp,
             const Dali::KeyEvent::State& keyState,
             const std::string&           compose,
             const std::string&           deviceName,
             const Device::Class::Type    deviceClass,
             const Device::Subclass::Type deviceSubclass)
{
  Internal::KeyEventPtr internal = Internal::KeyEvent::New(keyName, logicalKey, keyString, keyCode, keyModifier, timeStamp, keyState, compose, deviceName, deviceClass, deviceSubclass);

  return KeyEvent(internal.Get());
}

void SetKeyName(KeyEvent keyEvent, const std::string& keyName)
{
  GetImplementation(keyEvent).SetKeyName(keyName);
}

void SetKeyString(KeyEvent keyEvent, const std::string& keyString)
{
  GetImplementation(keyEvent).SetKeyString(keyString);
}

void SetKeyCode(KeyEvent keyEvent, int32_t keyCode)
{
  GetImplementation(keyEvent).SetKeyCode(keyCode);
}

void SetKeyModifier(KeyEvent keyEvent, int32_t keyModifier)
{
  GetImplementation(keyEvent).SetKeyModifier(keyModifier);
}

void SetTime(KeyEvent keyEvent, unsigned long time)
{
  GetImplementation(keyEvent).SetTime(time);
}

void SetState(KeyEvent keyEvent, const KeyEvent::State& state)
{
  GetImplementation(keyEvent).SetState(state);
}

void SetRepeat(KeyEvent keyEvent, const bool repeat)
{
  GetImplementation(keyEvent).SetRepeat(repeat);
}

} // namespace DevelKeyEvent

} // namespace Dali
