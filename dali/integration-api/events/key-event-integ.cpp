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

// CLASS HEADER
#include <dali/integration-api/events/key-event-integ.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/key-event-impl.h>

namespace Dali
{
namespace Integration
{
KeyEvent::KeyEvent()
: Event(Key),
  keyName(),
  logicalKey(),
  keyString(),
  keyCode(-1),
  keyModifier(0),
  time(0),
  state(KeyEvent::DOWN),
  compose(""),
  deviceName(""),
  deviceClass(Device::Class::NONE),
  deviceSubclass(Device::Subclass::NONE)
{
}

KeyEvent::KeyEvent(const std::string&           keyName,
                   const std::string&           logicalKey,
                   const std::string&           keyString,
                   int                          keyCode,
                   int                          keyModifier,
                   unsigned long                timeStamp,
                   const State&                 keyState,
                   const std::string&           compose,
                   const std::string&           deviceName,
                   const Device::Class::Type    deviceClass,
                   const Device::Subclass::Type deviceSubclass)
: Event(Key),
  keyName(keyName),
  logicalKey(logicalKey),
  keyString(keyString),
  keyCode(keyCode),
  keyModifier(keyModifier),
  time(timeStamp),
  state(keyState),
  compose(compose),
  deviceName(deviceName),
  deviceClass(deviceClass),
  deviceSubclass(deviceSubclass)
{
}

KeyEvent::~KeyEvent() = default;

} // namespace Integration

} // namespace Dali
