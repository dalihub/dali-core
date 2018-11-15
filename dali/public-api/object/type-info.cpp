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
#include <dali/public-api/object/type-info.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali/internal/event/common/type-info-impl.h>

namespace Dali
{

TypeInfo::TypeInfo()
{
}

TypeInfo::~TypeInfo()
{
}

TypeInfo::TypeInfo(const TypeInfo& copy)
: BaseHandle(copy)
{
}

TypeInfo& TypeInfo::operator=(const TypeInfo& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

const std::string& TypeInfo::GetName() const
{
  return GetImplementation(*this).GetName();
}

const std::string& TypeInfo::GetBaseName() const
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

std::string TypeInfo::GetActionName(size_t index)
{
  return GetImplementation(*this).GetActionName(index);
}

size_t TypeInfo::GetSignalCount() const
{
  return GetImplementation(*this).GetSignalCount();
}

std::string TypeInfo::GetSignalName(size_t index)
{
  return GetImplementation(*this).GetSignalName(index);
}

size_t TypeInfo::GetPropertyCount() const
{
  return GetImplementation(*this).GetPropertyCount();
}

void TypeInfo::GetPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.Clear(); // We do not want to clear the container if called internally, so only clear here
  GetImplementation(*this).GetPropertyIndices( indices );
}

const std::string& TypeInfo::GetPropertyName( Property::Index index ) const
{
  return GetImplementation(*this).GetRegisteredPropertyName( index );
}

Property::Index TypeInfo::GetChildPropertyIndex( const std::string& name ) const
{
  return GetImplementation(*this).GetChildPropertyIndex( name );
}

const std::string& TypeInfo::GetChildPropertyName( Property::Index index ) const
{
  return GetImplementation(*this).GetChildPropertyName( index );
}

Property::Type TypeInfo::GetChildPropertyType( Property::Index index ) const
{
  return GetImplementation(*this).GetChildPropertyType( index );
}

void TypeInfo::GetChildPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.Clear(); // We do not want to clear the container if called internally, so only clear here
  GetImplementation(*this).GetChildPropertyIndices( indices );
}

TypeInfo::TypeInfo(Internal::TypeInfo* internal)
: BaseHandle(internal)
{
}

} // namespace Dali
