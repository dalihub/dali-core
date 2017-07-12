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
#include <dali/devel-api/common/map-wrapper.h>

namespace Dali
{

namespace
{

typedef std::map< const KeyEvent*, Internal::KeyEventImpl*> KeyEventMap;
typedef KeyEventMap::iterator KeyEventMapIter;

KeyEventMap keyEventImplMap;

}

namespace Internal
{

KeyEventImpl::KeyEventImpl( KeyEvent* keyEvent )
: mDeviceName( "" ),
  mDeviceClass( DevelDevice::Class::NONE ),
  mDeviceSubclass( DevelDevice::Subclass::NONE )
{
  keyEventImplMap[keyEvent] = this;
}

KeyEventImpl::~KeyEventImpl()
{
  for( KeyEventMapIter iter = keyEventImplMap.begin(); iter != keyEventImplMap.end(); ++iter )
  {
    if( this == iter->second )
    {
      keyEventImplMap.erase( iter );
      break;
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

DevelDevice::Class::Type KeyEventImpl::GetDeviceClass() const
{
  return mDeviceClass;
}

void KeyEventImpl::SetDeviceClass( DevelDevice::Class::Type deviceClass )
{
  mDeviceClass = deviceClass;
}

DevelDevice::Subclass::Type KeyEventImpl::GetDeviceSubclass() const
{
  return mDeviceSubclass;
}

void KeyEventImpl::SetDeviceSubclass( DevelDevice::Subclass::Type deviceSubclass )
{
  mDeviceSubclass = deviceSubclass;
}

} // namsespace Internal

Internal::KeyEventImpl* GetImplementation( KeyEvent* keyEvent )
{
  return keyEventImplMap[keyEvent];
}

const Internal::KeyEventImpl* GetImplementation( const KeyEvent* keyEvent )
{
  return keyEventImplMap[keyEvent];
}

} // namespace Dali
