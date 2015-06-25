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
#include <dali/public-api/object/property-array.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{

namespace
{
}; // unnamed namespace

struct Property::Array::Impl
{
  typedef std::vector<Value> Array;

  Array mArray;
};

Property::Array::Array()
: mImpl( new Impl )
{
}

Property::Array::Array( const Property::Array& other )
: mImpl( new Impl )
{
  mImpl->mArray = other.mImpl->mArray;
}

Property::Array::~Array()
{
  delete mImpl;
}

Property::Array::SizeType Property::Array::Count() const
{
  return mImpl->mArray.size();
}

void Property::Array::PushBack( const Value& value )
{
  mImpl->mArray.push_back( value );
}

void Property::Array::Clear()
{
  mImpl->mArray.clear();
}

void Property::Array::Reserve( SizeType size )
{
  mImpl->mArray.reserve(size);
}

void Property::Array::Resize( SizeType size )
{
  mImpl->mArray.resize(size);
}

Property::Array::SizeType Property::Array::Capacity()
{
  return mImpl->mArray.capacity();
}

const Property::Value& Property::Array::operator[]( SizeType index ) const
{
  return mImpl->mArray[ index ];
}

Property::Value& Property::Array::operator[]( SizeType index )
{
  return mImpl->mArray[ index ];
}

Property::Array& Property::Array::operator=( const Property::Array& other )
{
  if( this != &other )
  {
    delete mImpl;
    mImpl = new Impl;
    mImpl->mArray = other.mImpl->mArray;
  }
  return *this;
}

} // namespace Dali
