/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/property-types.h>

namespace Dali
{

namespace
{
const char* const PROPERTY_TYPE_NAMES[] =
{
  "NONE",
  "BOOLEAN",
  "FLOAT",
  "INTEGER",
  "UNSIGNED_INTEGER",
  "VECTOR2",
  "VECTOR3",
  "VECTOR4",
  "MATRIX3",
  "MATRIX",
  "RECTANGLE",
  "ROTATION",
  "STRING",
  "ARRAY",
  "MAP",
};
const unsigned int PROPERTY_TYPE_NAMES_COUNT = sizeof( PROPERTY_TYPE_NAMES ) / sizeof( const char* );
}

namespace PropertyTypes
{

DALI_EXPORT_API
const char* const GetName(Property::Type type)
{
  if (type < PROPERTY_TYPE_NAMES_COUNT )
  {
    return PROPERTY_TYPE_NAMES[type];
  }

  return PROPERTY_TYPE_NAMES[Property::NONE];
}

}; // namespace PropertyTypes

} // namespace Dali
