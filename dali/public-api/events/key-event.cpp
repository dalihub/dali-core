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
#include <dali/public-api/events/key-event.h>

// INTERNAL INCLUDES
#include <dali/internal/event/events/key-event-impl.h>

namespace Dali
{
KeyEvent::KeyEvent()
: BaseHandle()
{
}

KeyEvent::KeyEvent(const KeyEvent& rhs) = default;

KeyEvent::KeyEvent(KeyEvent&& rhs) = default;

KeyEvent::~KeyEvent() = default;

KeyEvent& KeyEvent::operator=(const KeyEvent& rhs) = default;

KeyEvent& KeyEvent::operator=(KeyEvent&& rhs) = default;

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

const std::string& KeyEvent::GetCompose() const
{
  return GetImplementation(*this).GetCompose();
}

const std::string& KeyEvent::GetDeviceName() const
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

const std::string& KeyEvent::GetKeyName() const
{
  return GetImplementation(*this).GetKeyName();
}

const std::string& KeyEvent::GetKeyString() const
{
  return GetImplementation(*this).GetKeyString();
}

const std::string& KeyEvent::GetLogicalKey() const
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

unsigned long KeyEvent::GetTime() const
{
  return GetImplementation(*this).GetTime();
}

KeyEvent::State KeyEvent::GetState() const
{
  return GetImplementation(*this).GetState();
}

KeyEvent::KeyEvent(Internal::KeyEvent* internal)
: BaseHandle(internal)
{
}

} // namespace Dali
