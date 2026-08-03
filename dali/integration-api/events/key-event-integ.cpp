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
  deviceSubclass(Device::Subclass::NONE),
  isRepeat(false),
  interceptProcessed(false),
  windowId(0),
  receiveTime(0)
{
}

KeyEvent::KeyEvent(const Dali::String&          keyName,
                   const Dali::String&          logicalKey,
                   const Dali::String&          keyString,
                   int32_t                      keyCode,
                   int32_t                      keyModifier,
                   uint32_t                     timeStamp,
                   const State&                 keyState,
                   const Dali::String&          compose,
                   const Dali::String&          deviceName,
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
  deviceSubclass(deviceSubclass),
  isRepeat(false),
  interceptProcessed(false),
  windowId(0),
  receiveTime(0)
{
}

KeyEvent::KeyEvent(const Dali::KeyEvent& keyEvent)
: Event(Key),
  keyName(keyEvent.GetKeyName()),
  logicalKey(keyEvent.GetLogicalKey()),
  keyString(keyEvent.GetKeyString()),
  keyCode(keyEvent.GetKeyCode()),
  keyModifier(keyEvent.GetKeyModifier()),
  time(keyEvent.GetTime()),
  state(static_cast<State>(keyEvent.GetState())),
  compose(keyEvent.GetCompose()),
  deviceName(keyEvent.GetDeviceName()),
  deviceClass(keyEvent.GetDeviceClass()),
  deviceSubclass(keyEvent.GetDeviceSubclass()),
  isRepeat(keyEvent.IsRepeat()),
  interceptProcessed(GetImplementation(keyEvent).IsInterceptProcessed()),
  windowId(keyEvent.GetWindowId()),
  receiveTime(keyEvent.GetReceiveTime())
{
}

KeyEvent::KeyEvent(const KeyEvent& rhs)
: Event(Key),
  keyName(rhs.keyName),
  logicalKey(rhs.logicalKey),
  keyString(rhs.keyString),
  keyCode(rhs.keyCode),
  keyModifier(rhs.keyModifier),
  time(rhs.time),
  state(rhs.state),
  compose(rhs.compose),
  deviceName(rhs.deviceName),
  deviceClass(rhs.deviceClass),
  deviceSubclass(rhs.deviceSubclass),
  isRepeat(rhs.isRepeat),
  interceptProcessed(rhs.interceptProcessed),
  windowId(rhs.windowId),
  receiveTime(rhs.receiveTime)
{
}

KeyEvent::~KeyEvent() = default;

} // namespace Integration

} // namespace Dali
