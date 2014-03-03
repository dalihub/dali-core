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
#include <dali/public-api/object/property.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/handle.h>

namespace Dali
{

const int Property::INVALID_INDEX = -1;

Property::Property(Handle& obj, Property::Index propIndex)
: object(obj),
  propertyIndex(propIndex)
{
}

Property::Property(Handle& obj, const std::string& propertyName)
: object(obj),
  propertyIndex(Property::INVALID_INDEX)
{
  propertyIndex = object.GetPropertyIndex( propertyName );
}

Property::~Property()
{
}

} // namespace Dali
