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
#include <dali/public-api/object/property-map.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{

namespace
{
typedef std::vector< StringValuePair > StringValueContainer;

typedef std::pair< Property::Index, Property::Value > IndexValuePair;
typedef std::vector< IndexValuePair > IndexValueContainer;

}; // unnamed namespace

struct Property::Map::Impl
{
  StringValueContainer mStringValueContainer;
  IndexValueContainer mIndexValueContainer;
};

Property::Map::Map()
: mImpl( new Impl )
{
}

Property::Map::Map( const Property::Map& other )
: mImpl( new Impl )
{
  mImpl->mStringValueContainer = other.mImpl->mStringValueContainer;
  mImpl->mIndexValueContainer = other.mImpl->mIndexValueContainer;
}

Property::Map::~Map()
{
  delete mImpl;
}

Property::Map::SizeType Property::Map::Count() const
{
  return mImpl->mStringValueContainer.size() + mImpl->mIndexValueContainer.size();
}

bool Property::Map::Empty() const
{
  return mImpl->mStringValueContainer.empty() && mImpl->mIndexValueContainer.empty();
}

void Property::Map::Insert( const char* key, const Value& value )
{
  mImpl->mStringValueContainer.push_back( std::make_pair( key, value ) );
}

void Property::Map::Insert( const std::string& key, const Value& value )
{
  mImpl->mStringValueContainer.push_back( std::make_pair( key, value ) );
}

void Property::Map::Insert( Property::Index key, const Value& value )
{
  mImpl->mIndexValueContainer.push_back( std::make_pair( key, value ) );
}

Property::Value& Property::Map::GetValue( SizeType position ) const
{
  DALI_ASSERT_ALWAYS( position < mImpl->mStringValueContainer.size() && "position out-of-bounds" );

  return mImpl->mStringValueContainer[ position ].second;
}

const std::string& Property::Map::GetKey( SizeType position ) const
{
  DALI_LOG_WARNING( "DEPRECATION WARNING: GetKey() is due to be deprecated in the next release.\n" );

  DALI_ASSERT_ALWAYS( position < mImpl->mStringValueContainer.size() && "position out-of-bounds" );

  return mImpl->mStringValueContainer[ position ].first;
}

StringValuePair& Property::Map::GetPair( SizeType position ) const
{
  DALI_LOG_WARNING( "DEPRECATION WARNING: GetPair() is due to be deprecated in the next release.\n" );

  DALI_ASSERT_ALWAYS( position < mImpl->mStringValueContainer.size() && "position out-of-bounds" );

  return mImpl->mStringValueContainer[ position ];
}

Property::Value* Property::Map::Find( const char* key ) const
{
  for ( StringValueContainer::iterator iter = mImpl->mStringValueContainer.begin(), endIter = mImpl->mStringValueContainer.end(); iter != endIter; ++iter )
  {
    if ( iter->first == key )
    {
      return &iter->second;
    }
  }
  return NULL; // Not found
}

Property::Value* Property::Map::Find( const std::string& key ) const
{
  return Find( key.c_str() );
}

Property::Value* Property::Map::Find( Property::Index key ) const
{
  for ( IndexValueContainer::iterator iter = mImpl->mIndexValueContainer.begin(), endIter = mImpl->mIndexValueContainer.end(); iter != endIter; ++iter )
  {
    if ( iter->first == key )
    {
      return &iter->second;
    }
  }
  return NULL; // Not found
}

Property::Value* Property::Map::Find( Property::Index indexKey, const std::string& stringKey ) const
{
  Property::Value* valuePtr = Find( indexKey );
  if( !valuePtr )
  {
    valuePtr = Find( stringKey );
  }
  return valuePtr;
}

Property::Value* Property::Map::Find( const std::string& key, Property::Type type ) const
{
  for ( StringValueContainer::iterator iter = mImpl->mStringValueContainer.begin(), endIter = mImpl->mStringValueContainer.end(); iter != endIter; ++iter )
  {
    if( (iter->second.GetType() == type) && (iter->first == key) )
    {
      return &iter->second;
    }
  }
  return NULL; // Not found
}

Property::Value* Property::Map::Find( Property::Index key, Property::Type type ) const
{
  for ( IndexValueContainer::iterator iter = mImpl->mIndexValueContainer.begin(), endIter = mImpl->mIndexValueContainer.end(); iter != endIter; ++iter )
  {
    if( (iter->second.GetType() == type) && (iter->first == key) )
    {
      return &iter->second;
    }
  }
  return NULL; // Not found
}

void Property::Map::Clear()
{
  mImpl->mStringValueContainer.clear();
  mImpl->mIndexValueContainer.clear();
}

void Property::Map::Merge( const Property::Map& from )
{
  // Ensure we're not attempting to merge with ourself
  if ( this != &from )
  {
    if ( Count() )
    {
      for ( StringValueContainer::const_iterator iter = from.mImpl->mStringValueContainer.begin(), endIter = from.mImpl->mStringValueContainer.end(); iter != endIter; ++iter )
      {
        (*this)[iter->first] = iter->second;
      }

      for ( IndexValueContainer::const_iterator iter = from.mImpl->mIndexValueContainer.begin(), endIter = from.mImpl->mIndexValueContainer.end(); iter != endIter; ++iter )
      {
        (*this)[iter->first] = iter->second;
      }
    }
    else
    {
      // If we're empty, then just copy
      *this = from;
    }
  }
}

const Property::Value& Property::Map::operator[]( const std::string& key ) const
{
  for ( StringValueContainer::const_iterator iter = mImpl->mStringValueContainer.begin(), endIter = mImpl->mStringValueContainer.end(); iter != endIter; ++iter )
  {
    if ( iter->first == key )
    {
      return iter->second;
    }
  }

  DALI_ASSERT_ALWAYS( ! "Invalid Key" );
}

Property::Value& Property::Map::operator[]( const std::string& key )
{
  for ( StringValueContainer::iterator iter = mImpl->mStringValueContainer.begin(), endIter = mImpl->mStringValueContainer.end(); iter != endIter; ++iter )
  {
    if ( iter->first == key )
    {
      return iter->second;
    }
  }

  // Create and return reference to new value
  mImpl->mStringValueContainer.push_back( std::make_pair( key, Property::Value() ) );
  return (mImpl->mStringValueContainer.end() - 1)->second;
}

const Property::Value& Property::Map::operator[]( Property::Index key ) const
{
  for ( IndexValueContainer::const_iterator iter = mImpl->mIndexValueContainer.begin(), endIter = mImpl->mIndexValueContainer.end(); iter != endIter; ++iter )
  {
    if ( iter->first == key )
    {
      return iter->second;
    }
  }

  DALI_ASSERT_ALWAYS( ! "Invalid Key" );
}

Property::Value& Property::Map::operator[]( Property::Index key )
{
  for ( IndexValueContainer::iterator iter = mImpl->mIndexValueContainer.begin(), endIter = mImpl->mIndexValueContainer.end(); iter != endIter; ++iter )
  {
    if ( iter->first == key )
    {
      return iter->second;
    }
  }

  // Create and return reference to new value
  mImpl->mIndexValueContainer.push_back( std::make_pair( key, Property::Value() ) );
  return (mImpl->mIndexValueContainer.end() - 1)->second;
}

Property::Map& Property::Map::operator=( const Property::Map& other )
{
  if( this != &other )
  {
    delete mImpl;
    mImpl = new Impl;
    mImpl->mStringValueContainer = other.mImpl->mStringValueContainer;
    mImpl->mIndexValueContainer = other.mImpl->mIndexValueContainer;
  }
  return *this;
}

std::ostream& operator<<( std::ostream& stream, const Property::Map& map )
{
  stream << "Map(" << map.Count() << ") = {";

  int count = 0;
  // Output the String-Value pairs
  for ( StringValueContainer::iterator iter = map.mImpl->mStringValueContainer.begin(), endIter = map.mImpl->mStringValueContainer.end(); iter != endIter; ++iter )
  {
    if( count++ > 0 )
    {
      stream<<", ";
    }
    stream<< iter->first << ":"<<iter->second;
  }

  // Output the Index-Value pairs
  for ( IndexValueContainer::iterator iter = map.mImpl->mIndexValueContainer.begin(), endIter = map.mImpl->mIndexValueContainer.end(); iter != endIter; ++iter )
  {
    if( count++ > 0 )
    {
      stream<<", ";
    }
    stream<< iter->first << ":"<<iter->second;
  }

  stream << "}";

  return stream;
}

} // namespace Dali
