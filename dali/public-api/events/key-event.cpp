/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

namespace
{
const uint32_t MODIFIER_SHIFT = 0x1;
const uint32_t MODIFIER_CTRL  = 0x2;
const uint32_t MODIFIER_ALT   = 0x4;
const int32_t KEY_INVALID_CODE = -1;
}

KeyEvent::KeyEvent()
: keyPressedName(""),
  keyPressed(""),
  keyCode(KEY_INVALID_CODE),
  keyModifier(0),
  time(0),
  state(KeyEvent::Down)
{
  new Internal::KeyEventImpl( this );
}

KeyEvent::KeyEvent(const std::string& keyName, const std::string& keyString, int32_t keyCode, int32_t keyModifier,unsigned long timeStamp, const State& keyState)
: keyPressedName(keyName),
  keyPressed(keyString),
  keyCode(keyCode),
  keyModifier(keyModifier),
  time(timeStamp),
  state(keyState)
{
  new Internal::KeyEventImpl( this );
}

KeyEvent::KeyEvent( const KeyEvent& rhs )
: keyPressedName( rhs.keyPressedName ),
  keyPressed( rhs.keyPressed ),
  keyCode( rhs.keyCode ),
  keyModifier( rhs.keyModifier ),
  time( rhs.time ),
  state( rhs.state )
{
  Internal::KeyEventImpl* impl = new Internal::KeyEventImpl( this );
  *impl = *GetImplementation( &rhs );
}

KeyEvent& KeyEvent::operator=( const KeyEvent& rhs )
{
  if( this != &rhs )
  {
    keyPressedName = rhs.keyPressedName;
    keyPressed = rhs.keyPressed;
    keyCode = rhs.keyCode;
    keyModifier = rhs.keyModifier;
    time = rhs.time;
    state = rhs.state;

    *GetImplementation( this ) = *GetImplementation( &rhs );
  }

  return *this;
}

KeyEvent::~KeyEvent()
{
  delete GetImplementation( this );
}

bool KeyEvent::IsShiftModifier() const
{
  if ((MODIFIER_SHIFT & keyModifier) == MODIFIER_SHIFT)
  {
    return true;
  }

  return false;
}

bool KeyEvent::IsCtrlModifier() const
{
  if ((MODIFIER_CTRL & keyModifier) == MODIFIER_CTRL)
  {
    return true;
  }

  return false;
}

bool KeyEvent::IsAltModifier() const
{
  if ((MODIFIER_ALT & keyModifier) == MODIFIER_ALT)
  {
    return true;
  }

  return false;
}

std::string KeyEvent::GetCompose() const
{
  return GetImplementation( this )->GetCompose();
}

std::string KeyEvent::GetDeviceName() const
{
  return GetImplementation( this )->GetDeviceName();
}

Device::Class::Type KeyEvent::GetDeviceClass() const
{
  return GetImplementation( this )->GetDeviceClass();
}

Device::Subclass::Type KeyEvent::GetDeviceSubclass() const
{
  return GetImplementation( this )->GetDeviceSubclass();
}


} // namespace Dali
