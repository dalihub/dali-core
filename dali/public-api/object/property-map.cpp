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
#include <dali/public-api/object/property-map.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{
namespace
{
typedef std::vector<StringValuePair> StringValueContainer;

using IndexValuePair      = std::pair<Property::Index, Property::Value>;
using IndexValueContainer = std::vector<IndexValuePair>;

}; // unnamed namespace

struct Property::Map::Impl
{
  StringValueContainer mStringValueContainer;
  IndexValueContainer  mIndexValueContainer;
};

Property::Map::Map()
: mImpl(new Impl)
{
}

Property::Map::Map(const std::initializer_list<KeyValuePair>& values)
: Map()
{
  for(auto&& value : values)
  {
    const auto& key = value.first;
    switch(key.type)
    {
      case Property::Key::INDEX:
      {
        Property::Map::Insert(key.indexKey, value.second);
        break;
      }
      case Property::Key::STRING:
      {
        Property::Map::Insert(key.stringKey, value.second);
        break;
      }
    }
  }
}

Property::Map::Map(const Property::Map& other)
: mImpl(new Impl)
{
  mImpl->mStringValueContainer = other.mImpl->mStringValueContainer;
  mImpl->mIndexValueContainer  = other.mImpl->mIndexValueContainer;
}

Property::Map::Map(Property::Map&& other)
: mImpl(other.mImpl)
{
  other.mImpl = nullptr;
}

Property::Map::~Map()
{
  delete mImpl;
}

Property::Map::SizeType Property::Map::Count() const
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");
  return mImpl->mStringValueContainer.size() + mImpl->mIndexValueContainer.size();
}

bool Property::Map::Empty() const
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");
  return mImpl->mStringValueContainer.empty() && mImpl->mIndexValueContainer.empty();
}

void Property::Map::Insert(std::string key, Value value)
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");
  mImpl->mStringValueContainer.push_back(std::make_pair(std::move(key), std::move(value)));
}

void Property::Map::Insert(Property::Index key, Value value)
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");
  mImpl->mIndexValueContainer.push_back(std::make_pair(key, std::move(value)));
}

Property::Value& Property::Map::GetValue(SizeType position) const
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");
  SizeType numStringKeys = mImpl->mStringValueContainer.size();
  SizeType numIndexKeys  = mImpl->mIndexValueContainer.size();
  DALI_ASSERT_ALWAYS(position < (numStringKeys + numIndexKeys) && "position out-of-bounds");

  if(position < numStringKeys)
  {
    return mImpl->mStringValueContainer[position].second;
  }
  else
  {
    return mImpl->mIndexValueContainer[position - numStringKeys].second;
  }
}

const std::string& Property::Map::GetKey(SizeType position) const
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: GetKey() is deprecated and will be removed from next release.\n");

  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");
  SizeType numStringKeys = mImpl->mStringValueContainer.size();
  DALI_ASSERT_ALWAYS(position < numStringKeys && "position out-of-bounds");

  return mImpl->mStringValueContainer[position].first;
}

Property::Key Property::Map::GetKeyAt(SizeType position) const
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");

  SizeType numStringKeys = mImpl->mStringValueContainer.size();
  SizeType numIndexKeys  = mImpl->mIndexValueContainer.size();
  DALI_ASSERT_ALWAYS(position < (numStringKeys + numIndexKeys) && "position out-of-bounds");

  if(position < numStringKeys)
  {
    Key key(mImpl->mStringValueContainer[position].first);
    return key;
  }
  else
  {
    Key key(mImpl->mIndexValueContainer[position - numStringKeys].first);
    return key;
  }
}

StringValuePair& Property::Map::GetPair(SizeType position) const
{
  DALI_LOG_WARNING_NOFN("DEPRECATION WARNING: GetPair() is deprecated and will be removed from next release.\n");

  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");

  SizeType numStringKeys = mImpl->mStringValueContainer.size();

  DALI_ASSERT_ALWAYS(position < (numStringKeys) && "position out-of-bounds");

  return mImpl->mStringValueContainer[position];
}

KeyValuePair Property::Map::GetKeyValue(SizeType position) const
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");

  SizeType numStringKeys = mImpl->mStringValueContainer.size();
  SizeType numIndexKeys  = mImpl->mIndexValueContainer.size();

  DALI_ASSERT_ALWAYS(position < (numStringKeys + numIndexKeys) && "position out-of-bounds");

  if(position < numStringKeys)
  {
    Key          key(mImpl->mStringValueContainer[position].first);
    KeyValuePair keyValue(key, mImpl->mStringValueContainer[position].second);
    return keyValue;
  }
  else
  {
    return mImpl->mIndexValueContainer[position - numStringKeys];
  }
}

Property::Value* Property::Map::Find(std::string_view key) const
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");

  for(auto&& iter : mImpl->mStringValueContainer)
  {
    if(key == iter.first)
    {
      return &iter.second;
    }
  }
  return nullptr; // Not found
}

Property::Value* Property::Map::Find(Property::Index key) const
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");

  for(auto&& iter : mImpl->mIndexValueContainer)
  {
    if(iter.first == key)
    {
      return &iter.second;
    }
  }
  return nullptr; // Not found
}

Property::Value* Property::Map::Find(Property::Index indexKey, std::string_view stringKey) const
{
  Property::Value* valuePtr = Find(indexKey);
  if(!valuePtr)
  {
    valuePtr = Find(stringKey);
  }
  return valuePtr;
}

Property::Value* Property::Map::Find(std::string_view key, Property::Type type) const
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");

  for(auto&& iter : mImpl->mStringValueContainer)
  {
    if((iter.second.GetType() == type) && (key == iter.first))
    {
      return &iter.second;
    }
  }
  return nullptr; // Not found
}

Property::Value* Property::Map::Find(Property::Index key, Property::Type type) const
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");

  for(auto&& iter : mImpl->mIndexValueContainer)
  {
    if((iter.second.GetType() == type) && (iter.first == key))
    {
      return &iter.second;
    }
  }
  return nullptr; // Not found
}

void Property::Map::Clear()
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");

  mImpl->mStringValueContainer.clear();
  mImpl->mIndexValueContainer.clear();
}

void Property::Map::Merge(const Property::Map& from)
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");

  // Ensure we're not attempting to merge with ourself
  if(this != &from)
  {
    if(Count())
    {
      for(auto&& iter : from.mImpl->mStringValueContainer)
      {
        (*this)[iter.first] = iter.second;
      }

      for(auto&& iter : from.mImpl->mIndexValueContainer)
      {
        (*this)[iter.first] = iter.second;
      }
    }
    else
    {
      // If we're empty, then just copy
      *this = from;
    }
  }
}

const Property::Value& Property::Map::operator[](std::string_view key) const
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");

  for(auto&& iter : mImpl->mStringValueContainer)
  {
    if(iter.first == key)
    {
      return iter.second;
    }
  }

  DALI_ASSERT_ALWAYS(!"Invalid Key");
}

Property::Value& Property::Map::operator[](std::string_view key)
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");

  for(auto&& iter : mImpl->mStringValueContainer)
  {
    if(iter.first == key)
    {
      return iter.second;
    }
  }

  // Create and return reference to new value
  mImpl->mStringValueContainer.push_back(std::make_pair(std::string(key), Property::Value()));
  return mImpl->mStringValueContainer.back().second;
}

const Property::Value& Property::Map::operator[](Property::Index key) const
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");

  for(auto&& iter : mImpl->mIndexValueContainer)
  {
    if(iter.first == key)
    {
      return iter.second;
    }
  }

  DALI_ASSERT_ALWAYS(!"Invalid Key");
}

Property::Value& Property::Map::operator[](Property::Index key)
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");

  for(auto&& iter : mImpl->mIndexValueContainer)
  {
    if(iter.first == key)
    {
      return iter.second;
    }
  }

  // Create and return reference to new value
  mImpl->mIndexValueContainer.push_back(std::make_pair(key, Property::Value()));
  return mImpl->mIndexValueContainer.back().second;
}

Property::Map& Property::Map::operator=(const Property::Map& other)
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");

  if(this != &other)
  {
    mImpl->mStringValueContainer = other.mImpl->mStringValueContainer;
    mImpl->mIndexValueContainer  = other.mImpl->mIndexValueContainer;
  }
  return *this;
}

Property::Map& Property::Map::operator=(Property::Map&& other)
{
  if(this != &other)
  {
    delete mImpl;
    mImpl       = other.mImpl;
    other.mImpl = nullptr;
  }
  return *this;
}

std::ostream& operator<<(std::ostream& stream, const Property::Map& map)
{
  stream << "Map(" << map.Count() << ") = {";

  if(map.mImpl)
  {
    int32_t count = 0;
    // Output the String-Value pairs
    for(auto&& iter : map.mImpl->mStringValueContainer)
    {
      if(count++ > 0)
      {
        stream << ", ";
      }
      stream << iter.first << ":" << iter.second;
    }

    // Output the Index-Value pairs
    for(auto&& iter : map.mImpl->mIndexValueContainer)
    {
      if(count++ > 0)
      {
        stream << ", ";
      }
      stream << iter.first << ":" << iter.second;
    }
  }

  stream << "}";

  return stream;
}

} // namespace Dali
