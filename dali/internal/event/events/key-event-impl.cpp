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

// CLASS HEADER
#include <dali/internal/event/events/key-event-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{
namespace
{
const uint32_t MODIFIER_SHIFT   = 0x1;
const uint32_t MODIFIER_CTRL    = 0x2;
const uint32_t MODIFIER_ALT     = 0x4;
const int32_t  KEY_INVALID_CODE = -1;
} // namespace

namespace Internal
{
KeyEvent::KeyEvent()
: mKeyName(""),
  mLogicalKey(""),
  mKeyString(""),
  mKeyCode(KEY_INVALID_CODE),
  mKeyModifier(0),
  mTime(0),
  mState(Dali::KeyEvent::DOWN),
  mCompose(""),
  mDeviceName(""),
  mDeviceClass(Device::Class::NONE),
  mDeviceSubclass(Device::Subclass::NONE),
  mIsRepeat(false)
{
}

KeyEvent::KeyEvent(const std::string&           keyName,
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
: mKeyName(keyName),
  mLogicalKey(logicalKey),
  mKeyString(keyString),
  mKeyCode(keyCode),
  mKeyModifier(keyModifier),
  mTime(timeStamp),
  mState(keyState),
  mCompose(compose),
  mDeviceName(deviceName),
  mDeviceClass(deviceClass),
  mDeviceSubclass(deviceSubclass),
  mIsRepeat(false)
{
}

KeyEventPtr KeyEvent::New(const std::string&           keyName,
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
  KeyEventPtr keyEvent = new KeyEvent(keyName, logicalKey, keyString, keyCode, keyModifier, timeStamp, keyState, compose, deviceName, deviceClass, deviceSubclass);
  return keyEvent;
}

bool KeyEvent::IsShiftModifier() const
{
  return ((MODIFIER_SHIFT & mKeyModifier) == MODIFIER_SHIFT);
}

bool KeyEvent::IsCtrlModifier() const
{
  return ((MODIFIER_CTRL & mKeyModifier) == MODIFIER_CTRL);
}

bool KeyEvent::IsAltModifier() const
{
  return ((MODIFIER_ALT & mKeyModifier) == MODIFIER_ALT);
}

const std::string& KeyEvent::GetCompose() const
{
  return mCompose;
}

const std::string& KeyEvent::GetDeviceName() const
{
  return mDeviceName;
}

Device::Class::Type KeyEvent::GetDeviceClass() const
{
  return mDeviceClass;
}

Device::Subclass::Type KeyEvent::GetDeviceSubclass() const
{
  return mDeviceSubclass;
}

const std::string& KeyEvent::GetKeyName() const
{
  return mKeyName;
}

const std::string& KeyEvent::GetKeyString() const
{
  return mKeyString;
}

const std::string& KeyEvent::GetLogicalKey() const
{
  return mLogicalKey;
}

int32_t KeyEvent::GetKeyCode() const
{
  return mKeyCode;
}

int32_t KeyEvent::GetKeyModifier() const
{
  return mKeyModifier;
}

unsigned long KeyEvent::GetTime() const
{
  return mTime;
}

Dali::KeyEvent::State KeyEvent::GetState() const
{
  return mState;
}

bool KeyEvent::IsRepeat() const
{
  return mIsRepeat;
}

void KeyEvent::SetKeyName(const std::string& keyName)
{
  mKeyName = keyName;
}

void KeyEvent::SetKeyString(const std::string& keyString)
{
  mKeyString = keyString;
}

void KeyEvent::SetKeyCode(int32_t keyCode)
{
  mKeyCode = keyCode;
}

void KeyEvent::SetKeyModifier(int32_t keyModifier)
{
  mKeyModifier = keyModifier;
}

void KeyEvent::SetTime(unsigned long time)
{
  mTime = time;
}

void KeyEvent::SetState(const Dali::KeyEvent::State& state)
{
  mState = state;
}

void KeyEvent::SetRepeat(const bool repeat)
{
  mIsRepeat = repeat;
}

} // namespace Internal

} // namespace Dali
