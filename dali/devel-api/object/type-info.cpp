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
#include <dali/devel-api/object/type-info.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali/integration-api/string-utils.h>
#include <dali/internal/event/common/type-info-impl.h>

using Dali::Integration::ToDaliString;
using Dali::Integration::ToDaliStringView;
using Dali::Integration::ToStdString;

namespace Dali
{
TypeInfo::TypeInfo() = default;

TypeInfo::~TypeInfo() = default;

TypeInfo::TypeInfo(const TypeInfo& copy) = default;

TypeInfo& TypeInfo::operator=(const TypeInfo& rhs) = default;

TypeInfo::TypeInfo(TypeInfo&& rhs) noexcept = default;

TypeInfo& TypeInfo::operator=(TypeInfo&& rhs) noexcept = default;

const Dali::String& TypeInfo::GetName() const
{
  return GetImplementation(*this).GetName();
}

const Dali::String& TypeInfo::GetBaseName() const
{
  return GetImplementation(*this).GetBaseName();
}

BaseHandle TypeInfo::CreateInstance() const
{
  return GetImplementation(*this).CreateInstance();
}

TypeInfo::CreateFunction TypeInfo::GetCreator() const
{
  return GetImplementation(*this).GetCreator();
}

size_t TypeInfo::GetActionCount() const
{
  return GetImplementation(*this).GetActionCount();
}

Dali::String TypeInfo::GetActionName(size_t index)
{
  return GetImplementation(*this).GetActionName(static_cast<uint32_t>(index));
}

size_t TypeInfo::GetSignalCount() const
{
  return GetImplementation(*this).GetSignalCount();
}

Dali::String TypeInfo::GetSignalName(size_t index)
{
  return GetImplementation(*this).GetSignalName(static_cast<uint32_t>(index));
}

size_t TypeInfo::GetPropertyCount() const
{
  return GetImplementation(*this).GetPropertyCount();
}

void TypeInfo::GetPropertyIndices(Property::IndexContainer& indices) const
{
  indices.Clear(); // We do not want to clear the container if called internally, so only clear here
  GetImplementation(*this).GetPropertyIndices(indices);
}

Dali::StringView TypeInfo::GetPropertyName(Property::Index index) const
{
  return ToDaliStringView(GetImplementation(*this).GetRegisteredPropertyName(index));
}

Property::Index TypeInfo::GetChildPropertyIndex(const Dali::String& name) const
{
  return GetImplementation(*this).GetChildPropertyIndex(Internal::ConstString(ToStdString(name)));
}

Dali::StringView TypeInfo::GetChildPropertyName(Property::Index index) const
{
  return ToDaliStringView(GetImplementation(*this).GetChildPropertyName(index));
}

Property::Type TypeInfo::GetChildPropertyType(Property::Index index) const
{
  return GetImplementation(*this).GetChildPropertyType(index);
}

void TypeInfo::GetChildPropertyIndices(Property::IndexContainer& indices) const
{
  indices.Clear(); // We do not want to clear the container if called internally, so only clear here
  GetImplementation(*this).GetChildPropertyIndices(indices);
}

TypeInfo::TypeInfo(Internal::TypeInfo* internal)
: BaseHandle(internal)
{
}

} // namespace Dali
