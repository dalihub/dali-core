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

namespace DALI_NAMESPACE
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
  receiveTime(0),
  rawKeyCode(0)
{
}

KeyEvent::KeyEvent(const Dali::String&    initKeyName,
                   const Dali::String&    initLogicalKey,
                   const Dali::String&    initKeyString,
                   int32_t                initKeyCode,
                   int32_t                initKeyModifier,
                   uint32_t               timeStamp,
                   const State&           keyState,
                   const Dali::String&    initCompose,
                   const Dali::String&    initDeviceName,
                   Device::Class::Type    initDeviceClass,
                   Device::Subclass::Type initDeviceSubclass)
: Event(Key),
  keyName(initKeyName),
  logicalKey(initLogicalKey),
  keyString(initKeyString),
  keyCode(initKeyCode),
  keyModifier(initKeyModifier),
  time(timeStamp),
  state(keyState),
  compose(initCompose),
  deviceName(initDeviceName),
  deviceClass(initDeviceClass),
  deviceSubclass(initDeviceSubclass),
  isRepeat(false),
  interceptProcessed(false),
  windowId(0),
  receiveTime(0),
  rawKeyCode(0)
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
  receiveTime(keyEvent.GetReceiveTime()),
  rawKeyCode(GetImplementation(keyEvent).GetRawKeyCode())
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
  receiveTime(rhs.receiveTime),
  rawKeyCode(rhs.rawKeyCode)
{
}

KeyEvent::~KeyEvent() = default;

Dali::KeyEvent NewKeyEvent(const KeyEvent& keyEvent)
{
  Internal::KeyEventPtr internal = Internal::KeyEvent::New(keyEvent.keyName, keyEvent.logicalKey, keyEvent.keyString, keyEvent.keyCode, keyEvent.rawKeyCode, keyEvent.keyModifier, keyEvent.time, static_cast<Dali::KeyEvent::State>(keyEvent.state), keyEvent.compose, keyEvent.deviceName, keyEvent.deviceClass, keyEvent.deviceSubclass);
  return Dali::KeyEvent(internal.Get());
}

} // namespace Integration

} //namespace DALI_NAMESPACE
