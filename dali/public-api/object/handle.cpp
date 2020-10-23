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
#include <dali/public-api/object/handle.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/event/animation/constraint-impl.h>
#include <dali/internal/event/common/object-impl.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/object/property-conditions.h>
#include <dali/public-api/object/property-notification.h>

namespace Dali
{
Handle::Handle(Dali::Internal::Object* handle)
: BaseHandle(handle)
{
}

Handle::Handle() = default;

Handle Handle::New()
{
  return Handle(Internal::Object::New().Get());
}

Handle::~Handle() = default;

Handle::Handle(const Handle& handle) = default;

Handle& Handle::operator=(const Handle& rhs) = default;

Handle::Handle(Handle&& rhs) = default;

Handle& Handle::operator=(Handle&& rhs) = default;

Handle Handle::DownCast(BaseHandle handle)
{
  return Handle(dynamic_cast<Dali::Internal::Object*>(handle.GetObjectPtr()));
}

bool Handle::Supports(Capability capability) const
{
  return GetImplementation(*this).Supports(capability);
}

uint32_t Handle::GetPropertyCount() const
{
  return GetImplementation(*this).GetPropertyCount();
}

std::string Handle::GetPropertyName(Property::Index index) const
{
  return GetImplementation(*this).GetPropertyName(index);
}

Property::Index Handle::GetPropertyIndex(Property::Key key) const
{
  return GetImplementation(*this).GetPropertyIndex(key);
}

bool Handle::IsPropertyWritable(Property::Index index) const
{
  return GetImplementation(*this).IsPropertyWritable(index);
}

bool Handle::IsPropertyAnimatable(Property::Index index) const
{
  return GetImplementation(*this).IsPropertyAnimatable(index);
}

bool Handle::IsPropertyAConstraintInput(Property::Index index) const
{
  return GetImplementation(*this).IsPropertyAConstraintInput(index);
}

Property::Type Handle::GetPropertyType(Property::Index index) const
{
  return GetImplementation(*this).GetPropertyType(index);
}

void Handle::SetProperty(Property::Index index, Property::Value propertyValue)
{
  GetImplementation(*this).SetProperty(index, std::move(propertyValue));
}

Property::Index Handle::RegisterProperty(std::string name, Property::Value propertyValue)
{
  return GetImplementation(*this).RegisterProperty(std::move(name), std::move(propertyValue));
}

Property::Index Handle::RegisterProperty(Property::Index key, std::string name, Property::Value propertyValue)
{
  return GetImplementation(*this).RegisterProperty(std::move(name), key, std::move(propertyValue));
}

Property::Index Handle::RegisterProperty(std::string name, Property::Value propertyValue, Property::AccessMode accessMode)
{
  return GetImplementation(*this).RegisterProperty(std::move(name), std::move(propertyValue), accessMode);
}

Property::Value Handle::GetProperty(Property::Index index) const
{
  return GetImplementation(*this).GetProperty(index);
}

Property::Value Handle::GetCurrentProperty(Property::Index index) const
{
  return GetImplementation(*this).GetCurrentProperty(index);
}

void Handle::SetProperties(const Property::Map& properties)
{
  GetImplementation(*this).SetProperties(properties);
}

void Handle::GetProperties(Property::Map& properties)
{
  GetImplementation(*this).GetProperties(properties);
}

void Handle::GetPropertyIndices(Property::IndexContainer& indices) const
{
  GetImplementation(*this).GetPropertyIndices(indices);
}

bool Handle::DoesCustomPropertyExist(Property::Index index)
{
  return GetImplementation(*this).DoesCustomPropertyExist(index);
}

Dali::PropertyNotification Handle::AddPropertyNotification(Property::Index          index,
                                                           const PropertyCondition& condition)
{
  return GetImplementation(*this).AddPropertyNotification(index, -1, condition);
}

Dali::PropertyNotification Handle::AddPropertyNotification(Property::Index          index,
                                                           int                      componentIndex,
                                                           const PropertyCondition& condition)
{
  return GetImplementation(*this).AddPropertyNotification(index, componentIndex, condition);
}

void Handle::RemovePropertyNotification(Dali::PropertyNotification propertyNotification)
{
  GetImplementation(*this).RemovePropertyNotification(propertyNotification);
}

void Handle::RemovePropertyNotifications()
{
  GetImplementation(*this).RemovePropertyNotifications();
}

void Handle::RemoveConstraints()
{
  GetImplementation(*this).RemoveConstraints();
}

void Handle::RemoveConstraints(uint32_t tag)
{
  GetImplementation(*this).RemoveConstraints(tag);
}

IndirectValue Handle::operator[](Property::Index index)
{
  // Will assert on access if handle is empty
  return IndirectValue(*this, index);
}

IndirectValue Handle::operator[](const std::string& name)
{
  // Will assert immediately when GetPropertyIndex is called if handle is empty
  return IndirectValue(*this, GetPropertyIndex(name));
}

Handle::PropertySetSignalType& Handle::PropertySetSignal()
{
  return GetImplementation(*this).PropertySetSignal();
}

namespace WeightObject
{
const Property::Index WEIGHT = PROPERTY_CUSTOM_START_INDEX;

Handle New()
{
  Handle handle = Handle::New();

  handle.RegisterProperty("weight", 0.0f);

  return handle;
}

} // namespace WeightObject

} // namespace Dali
