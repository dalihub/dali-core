//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali/public-api/object/handle.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/property-conditions.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/internal/event/common/object-impl.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

Handle::Handle(Dali::Internal::Object* handle)
  : BaseHandle(handle)
{
}

Handle::Handle()
{
}

Handle::~Handle()
{
}

Handle::Handle(const Handle& handle)
  : BaseHandle(handle)
{
}

Handle& Handle::operator=(const Handle& rhs)
{
  if( this != &rhs )
  {
    BaseHandle::operator=(rhs);
  }

  return *this;
}

Handle Handle::DownCast( BaseHandle handle )
{
  return Handle( dynamic_cast<Dali::Internal::Object*>(handle.GetObjectPtr()) );
}


bool Handle::Supports(Capability capability) const
{
  return GetImplementation(*this).Supports( capability );
}

unsigned int Handle::GetPropertyCount() const
{
  return GetImplementation(*this).GetPropertyCount();
}

const std::string& Handle::GetPropertyName(Property::Index index) const
{
  return GetImplementation(*this).GetPropertyName( index );
}

Property::Index Handle::GetPropertyIndex(std::string name) const
{
  return GetImplementation(*this).GetPropertyIndex( name );
}

bool Handle::IsPropertyWritable(Property::Index index) const
{
  return GetImplementation(*this).IsPropertyWritable( index );
}

bool Handle::IsPropertyAnimatable(Property::Index index) const
{
  return GetImplementation(*this).IsPropertyAnimatable( index );
}

Property::Type Handle::GetPropertyType(Property::Index index) const
{
  return GetImplementation(*this).GetPropertyType( index );
}

void Handle::SetProperty(Property::Index index, Property::Value propertyValue)
{
  GetImplementation(*this).SetProperty( index, propertyValue );
}

Property::Index Handle::RegisterProperty(std::string name, Property::Value propertyValue)
{
  return GetImplementation(*this).RegisterProperty( name, propertyValue );
}

Property::Index Handle::RegisterProperty(std::string name, Property::Value propertyValue, Property::AccessMode accessMode)
{
  return GetImplementation(*this).RegisterProperty( name, propertyValue, accessMode );
}

Property::Value Handle::GetProperty(Property::Index index) const
{
  return GetImplementation(*this).GetProperty( index );
}

Dali::PropertyNotification Handle::AddPropertyNotification(Property::Index index,
                                                           const PropertyCondition& condition)
{
  return GetImplementation(*this).AddPropertyNotification( index, condition );
}

void Handle::RemovePropertyNotification(Dali::PropertyNotification propertyNotification)
{
  GetImplementation(*this).RemovePropertyNotification( propertyNotification );
}

void Handle::RemovePropertyNotifications()
{
  GetImplementation(*this).RemovePropertyNotifications();
}

} // namespace Dali
