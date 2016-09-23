/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/property-key.h>


namespace Dali
{

Property::Key::Key( const std::string& key )
: type(Key::STRING),
  indexKey( Property::INVALID_INDEX ),
  stringKey( key )
{
}

Property::Key::Key( Property::Index key )
: type(Key::INDEX),
  indexKey( key )
{
}

bool Property::Key::operator== (const std::string& rhs)
{
  bool result=false;
  if(type == Key::STRING)
  {
    result = (stringKey == rhs);
  }
  return result;
}

bool Property::Key::operator== (Property::Index rhs)
{
  bool result=false;
  if(type == Key::INDEX)
  {
    result = (indexKey == rhs);
  }
  return result;
}

bool Property::Key::operator== (const Key& rhs)
{
  bool result=false;
  if(type == Key::STRING && rhs.type == Key::STRING )
  {
    result = (stringKey == rhs.stringKey);
  }
  else if( type == Key::INDEX && rhs.type == Key::INDEX )
  {
    result = (indexKey == rhs.indexKey);
  }
  return result;
}

bool Property::Key::operator!= (const std::string& rhs)
{
  return !operator==(rhs);
}

bool Property::Key::operator!= (Property::Index rhs)
{
  return !operator==(rhs);
}

bool Property::Key::operator!= (const Key& rhs)
{
  return !operator==(rhs);
}

std::ostream& operator<<( std::ostream& stream, const Property::Key& key )
{
  if( key.type == Property::Key::INDEX )
  {
    stream << key.indexKey;
  }
  else
  {
    stream << key.stringKey;
  }
  return stream;
}

} // namespace Dali
