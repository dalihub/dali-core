/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/object/property-map-devel.h>

// INTERNAL INCLUDES
#include <dali/integration-api/string-utils.h>

using Dali::Integration::ToDaliString;

namespace Dali
{

Property::Map CreatePropertyMap(std::initializer_list<KeyValuePair> values)
{
  Property::Map map;
  for(auto&& value : values)
  {
    const auto& key = value.first;
    switch(key.type)
    {
      case Property::Key::INDEX:
      {
        map.Insert(key.indexKey, value.second);
        break;
      }
      case Property::Key::STRING:
      {
        map.Insert(ToDaliString(key.stringKey), value.second);
        break;
      }
    }
  }
  return map;
}

} // namespace Dali