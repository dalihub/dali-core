/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
/**
 * This container stores a mapping between public key event and impl as we cannot add data members in public one.
 * In practice this keeps the impl "alive" in between KeyEvent constructor and destructor calls so that getter
 * methods can be called to access the new data members. There is a 1:1 mapping between KeyEvent and KeyEventImpl.
 */
struct KeyImplMapping
{
  KeyEvent* keyEvent;
  Internal::KeyEventImpl* impl;
};
Vector< KeyImplMapping > gKeyEventToImplMapping;

}

namespace Internal
{

KeyEventImpl::KeyEventImpl( KeyEvent* keyEvent )
: mDeviceName( "" ),
  mDeviceClass( Device::Class::NONE ),
  mDeviceSubclass( Device::Subclass::NONE )
{
  gKeyEventToImplMapping.PushBack( { keyEvent, this } );
}

KeyEventImpl::~KeyEventImpl()
{
  for( auto&& iter : gKeyEventToImplMapping )
  {
    if( this == iter.impl )
    {
      gKeyEventToImplMapping.Erase( &iter ); // iter is reference to KeyImplMapping, take address of it for Erase
      return;
    }
  }
}

KeyEventImpl& KeyEventImpl::operator=( const KeyEventImpl& rhs )
{
  if( this != &rhs )
  {
    mDeviceName = rhs.mDeviceName;
    mDeviceClass = rhs.mDeviceClass;
    mDeviceSubclass = rhs.mDeviceSubclass;
  }

  return *this;
}

std::string KeyEventImpl::GetDeviceName() const
{
  return mDeviceName;
}

void KeyEventImpl::SetDeviceName( const std::string& deviceName )
{
  mDeviceName = deviceName;
}

Device::Class::Type KeyEventImpl::GetDeviceClass() const
{
  return mDeviceClass;
}

void KeyEventImpl::SetDeviceClass( Device::Class::Type deviceClass )
{
  mDeviceClass = deviceClass;
}

Device::Subclass::Type KeyEventImpl::GetDeviceSubclass() const
{
  return mDeviceSubclass;
}

void KeyEventImpl::SetDeviceSubclass( Device::Subclass::Type deviceSubclass )
{
  mDeviceSubclass = deviceSubclass;
}

} // namsespace Internal

Internal::KeyEventImpl* GetImplementation( KeyEvent* keyEvent )
{
  Internal::KeyEventImpl* impl( NULL );
  for( auto&& iter : gKeyEventToImplMapping )
  {
    if( iter.keyEvent == keyEvent )
    {
      impl = iter.impl;
    }
  }
  return impl;
}

const Internal::KeyEventImpl* GetImplementation( const KeyEvent* keyEvent )
{
  Internal::KeyEventImpl* impl( NULL );
  for( auto&& iter : gKeyEventToImplMapping )
  {
    if( iter.keyEvent == keyEvent )
    {
      impl = iter.impl;
    }
  }
  return impl;
}

} // namespace Dali
