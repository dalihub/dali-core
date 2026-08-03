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
#include <dali/public-api/events/key-event.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/key-event-impl.h>

namespace Dali
{
KeyEvent KeyEvent::New()
{
  Internal::KeyEventPtr internal = Internal::KeyEvent::New();

  return KeyEvent(internal.Get());
}

KeyEvent::KeyEvent()
: BaseHandle()
{
}

KeyEvent::KeyEvent(const KeyEvent& rhs) = default;

KeyEvent::KeyEvent(KeyEvent&& rhs) noexcept = default;

KeyEvent::~KeyEvent() = default;

KeyEvent& KeyEvent::operator=(const KeyEvent& rhs) = default;

KeyEvent& KeyEvent::operator=(KeyEvent&& rhs) noexcept = default;

bool KeyEvent::IsShiftModifier() const
{
  return GetImplementation(*this).IsShiftModifier();
}

bool KeyEvent::IsCtrlModifier() const
{
  return GetImplementation(*this).IsCtrlModifier();
}

bool KeyEvent::IsAltModifier() const
{
  return GetImplementation(*this).IsAltModifier();
}

const Dali::String& KeyEvent::GetCompose() const
{
  return GetImplementation(*this).GetCompose();
}

const Dali::String& KeyEvent::GetDeviceName() const
{
  return GetImplementation(*this).GetDeviceName();
}

Device::Class::Type KeyEvent::GetDeviceClass() const
{
  return GetImplementation(*this).GetDeviceClass();
}

Device::Subclass::Type KeyEvent::GetDeviceSubclass() const
{
  return GetImplementation(*this).GetDeviceSubclass();
}

const Dali::String& KeyEvent::GetKeyName() const
{
  return GetImplementation(*this).GetKeyName();
}

const Dali::String& KeyEvent::GetKeyString() const
{
  return GetImplementation(*this).GetKeyString();
}

const Dali::String& KeyEvent::GetLogicalKey() const
{
  return GetImplementation(*this).GetLogicalKey();
}

int32_t KeyEvent::GetKeyCode() const
{
  return GetImplementation(*this).GetKeyCode();
}

int32_t KeyEvent::GetKeyModifier() const
{
  return GetImplementation(*this).GetKeyModifier();
}

uint32_t KeyEvent::GetTime() const
{
  return GetImplementation(*this).GetTime();
}

KeyEvent::State KeyEvent::GetState() const
{
  return GetImplementation(*this).GetState();
}

bool KeyEvent::IsRepeat() const
{
  return GetImplementation(*this).IsRepeat();
}

uint32_t KeyEvent::GetWindowId() const
{
  return GetImplementation(*this).GetWindowId();
}

uint32_t KeyEvent::GetReceiveTime() const
{
  return GetImplementation(*this).GetReceiveTime();
}

void KeyEvent::SetCompose(const Dali::String& compose)
{
  GetImplementation(*this).SetCompose(compose);
}

void KeyEvent::SetDeviceName(const Dali::String& deviceName)
{
  GetImplementation(*this).SetDeviceName(deviceName);
}

void KeyEvent::SetDeviceClass(Device::Class::Type deviceClass)
{
  GetImplementation(*this).SetDeviceClass(deviceClass);
}

void KeyEvent::SetDeviceSubclass(Device::Subclass::Type deviceSubclass)
{
  GetImplementation(*this).SetDeviceSubclass(deviceSubclass);
}

void KeyEvent::SetKeyName(const Dali::String& keyName)
{
  GetImplementation(*this).SetKeyName(keyName);
}

void KeyEvent::SetKeyString(const Dali::String& keyString)
{
  GetImplementation(*this).SetKeyString(keyString);
}

void KeyEvent::SetLogicalKey(const Dali::String& logicalKey)
{
  GetImplementation(*this).SetLogicalKey(logicalKey);
}

void KeyEvent::SetKeyCode(int32_t keyCode)
{
  GetImplementation(*this).SetKeyCode(keyCode);
}

void KeyEvent::SetKeyModifier(int32_t keyModifier)
{
  GetImplementation(*this).SetKeyModifier(keyModifier);
}

void KeyEvent::SetTime(uint32_t time)
{
  GetImplementation(*this).SetTime(time);
}

void KeyEvent::SetState(KeyEvent::State state)
{
  GetImplementation(*this).SetState(state);
}

void KeyEvent::SetRepeat(bool repeat)
{
  GetImplementation(*this).SetRepeat(repeat);
}

void KeyEvent::SetWindowId(uint32_t windowId)
{
  GetImplementation(*this).SetWindowId(windowId);
}

KeyEvent::KeyEvent(Internal::KeyEvent* internal)
: BaseHandle(internal)
{
}

} // namespace Dali
