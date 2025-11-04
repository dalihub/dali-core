/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/property.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/handle.h>

namespace Dali
{
Property::Property(Handle& obj, Property::Index propIndex)
: Property(obj, propIndex, Property::INVALID_COMPONENT_INDEX)
{
}

Property::Property(Handle& obj, Property::Index propIndex, int32_t compIndex)
: object(obj),
  propertyIndex(propIndex),
  componentIndex(compIndex)
{
}

Property::Property(Handle& obj, const std::string& propertyName)
: Property(obj, propertyName, Property::INVALID_COMPONENT_INDEX)
{
}

Property::Property(Handle& obj, const std::string& propertyName, int32_t compIndex)
: object(obj),
  propertyIndex(Property::INVALID_INDEX),
  componentIndex(compIndex)
{
  propertyIndex = object.GetPropertyIndex(propertyName);
}

Property::~Property() = default;

} // namespace Dali
