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

const std::string& TypeInfo::GetName()
{
  return GetImplementation(*this).GetName();
}

const std::string& TypeInfo::GetBaseName()
{
  return GetImplementation(*this).GetBaseName();
}

BaseHandle TypeInfo::CreateInstance()
{
  return GetImplementation(*this).CreateInstance();
}

TypeInfo::CreateFunction TypeInfo::GetCreator()
{
  return GetImplementation(*this).GetCreator();
}

TypeInfo::NameContainer TypeInfo::GetActions()
{
  return GetImplementation(*this).GetActions();
}

TypeInfo::NameContainer TypeInfo::GetSignals()
{
  return GetImplementation(*this).GetSignals();
}

void TypeInfo::GetPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.clear(); // We do not want to clear the container if called internally, so only clear here
  GetImplementation(*this).GetPropertyIndices( indices );
}

const std::string& TypeInfo::GetPropertyName( Property::Index index ) const
{
  return GetImplementation(*this).GetPropertyName( index );
}

TypeInfo::TypeInfo(Internal::TypeInfo* internal)
: BaseHandle(internal)
{
}

} // namespace Dali

