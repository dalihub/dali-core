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
#include <dali/internal/event/events/key-event-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{
namespace
{
const int32_t KEY_INVALID_CODE = -1;
} // namespace

namespace Internal
{
KeyEvent::KeyEvent()
: mKeyName(""),
  mLogicalKey(""),
  mKeyString(""),
  mCompose(""),
  mDeviceName(""),
  mKeyCode(KEY_INVALID_CODE),
  mKeyModifier(0),
  mTime(0),
  mReceiveTime(0),
  mWindowId(0),
  mState(Dali::KeyEvent::DOWN),
  mDeviceClass(Device::Class::NONE),
  mDeviceSubclass(Device::Subclass::NONE),
  mIsRepeat(false),
  mInterceptProcessed(false)
{
}

KeyEvent::KeyEvent(const Dali::String&          keyName,
                   const Dali::String&          logicalKey,
                   const Dali::String&          keyString,
                   int32_t                      keyCode,
                   int32_t                      keyModifier,
                   uint32_t                     timeStamp,
                   const Dali::KeyEvent::State& keyState,
                   const Dali::String&          compose,
                   const Dali::String&          deviceName,
                   const Device::Class::Type    deviceClass,
                   const Device::Subclass::Type deviceSubclass)
: mKeyName(keyName),
  mLogicalKey(logicalKey),
  mKeyString(keyString),
  mCompose(compose),
  mDeviceName(deviceName),
  mKeyCode(keyCode),
  mKeyModifier(keyModifier),
  mTime(timeStamp),
  mReceiveTime(0),
  mWindowId(0),
  mState(keyState),
  mDeviceClass(deviceClass),
  mDeviceSubclass(deviceSubclass),
  mIsRepeat(false),
  mInterceptProcessed(false)
{
}

KeyEventPtr KeyEvent::New()
{
  KeyEventPtr keyEvent = new KeyEvent();
  return keyEvent;
}

KeyEventPtr KeyEvent::New(const Dali::String&          keyName,
                          const Dali::String&          logicalKey,
                          const Dali::String&          keyString,
                          int32_t                      keyCode,
                          int32_t                      keyModifier,
                          uint32_t                     timeStamp,
                          const Dali::KeyEvent::State& keyState,
                          const Dali::String&          compose,
                          const Dali::String&          deviceName,
                          const Device::Class::Type    deviceClass,
                          const Device::Subclass::Type deviceSubclass)
{
  KeyEventPtr keyEvent = new KeyEvent(keyName, logicalKey, keyString, keyCode, keyModifier, timeStamp, keyState, compose, deviceName, deviceClass, deviceSubclass);
  return keyEvent;
}

bool KeyEvent::IsShiftModifier() const
{
  return (mKeyModifier & Dali::KeyEvent::SHIFT) != 0;
}

bool KeyEvent::IsCtrlModifier() const
{
  return (mKeyModifier & Dali::KeyEvent::CTRL) != 0;
}

bool KeyEvent::IsAltModifier() const
{
  return (mKeyModifier & Dali::KeyEvent::ALT) != 0;
}

bool KeyEvent::IsInterceptProcessed() const
{
  return mInterceptProcessed;
}

const Dali::String& KeyEvent::GetCompose() const
{
  return mCompose;
}

const Dali::String& KeyEvent::GetDeviceName() const
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

const Dali::String& KeyEvent::GetKeyName() const
{
  return mKeyName;
}

const Dali::String& KeyEvent::GetKeyString() const
{
  return mKeyString;
}

const Dali::String& KeyEvent::GetLogicalKey() const
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

uint32_t KeyEvent::GetTime() const
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

uint32_t KeyEvent::GetWindowId() const
{
  return mWindowId;
}

uint32_t KeyEvent::GetReceiveTime() const
{
  return mReceiveTime;
}

void KeyEvent::SetCompose(const Dali::String& compose)
{
  mCompose = compose;
}

void KeyEvent::SetDeviceName(const Dali::String& deviceName)
{
  mDeviceName = deviceName;
}

void KeyEvent::SetDeviceClass(Device::Class::Type deviceClass)
{
  mDeviceClass = deviceClass;
}

void KeyEvent::SetDeviceSubclass(Device::Subclass::Type deviceSubclass)
{
  mDeviceSubclass = deviceSubclass;
}

void KeyEvent::SetKeyName(const Dali::String& keyName)
{
  mKeyName = keyName;
}

void KeyEvent::SetKeyString(const Dali::String& keyString)
{
  mKeyString = keyString;
}

void KeyEvent::SetLogicalKey(const Dali::String& logicalKey)
{
  mLogicalKey = logicalKey;
}

void KeyEvent::SetKeyCode(int32_t keyCode)
{
  mKeyCode = keyCode;
}

void KeyEvent::SetKeyModifier(int32_t keyModifier)
{
  mKeyModifier = keyModifier;
}

void KeyEvent::SetInterceptProcessed(bool interceptProcessed)
{
  mInterceptProcessed = interceptProcessed;
}

void KeyEvent::SetTime(uint32_t time)
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

void KeyEvent::SetWindowId(uint32_t windowId)
{
  mWindowId = windowId;
}

void KeyEvent::SetReceiveTime(uint32_t time)
{
  mReceiveTime = time;
}

} // namespace Internal

} // namespace Dali
