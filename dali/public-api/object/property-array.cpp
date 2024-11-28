/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <limits>

// INTERNAL INCLUDES
#include <dali/internal/common/hash-utils.h>
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{
namespace
{
constexpr std::size_t NOT_HASHED    = 0u;
constexpr std::size_t ALWAYS_REHASH = std::numeric_limits<std::size_t>::max();
} // namespace

struct Property::Array::Impl
{
  typedef std::vector<Value> Array;

public:
  std::size_t GetHash() const
  {
    std::size_t hash = mHash;
    if(hash == ALWAYS_REHASH || hash == NOT_HASHED)
    {
      hash = Dali::Internal::HashUtils::INITIAL_HASH_VALUE;

      for(const auto& iter : mArray)
      {
        // Use ordered hash operation.
        Dali::Internal::HashUtils::HashRawValue(iter.GetHash(), hash);
      }

      if(mHash != ALWAYS_REHASH)
      {
        mHash = hash;
      }
    }
    return hash;
  }

public:
  Array mArray;

  mutable std::size_t mHash{NOT_HASHED};
};

Property::Array::Array()
: mImpl(new Impl)
{
}

Property::Array::Array(const std::initializer_list<Property::Value>& values)
: Array()
{
  for(auto&& value : values)
  {
    PushBack(value);
  }
}

Property::Array::Array(const Property::Array& other)
: mImpl(new Impl)
{
  mImpl->mArray = other.mImpl->mArray;
}

Property::Array::Array(Property::Array&& other) noexcept
: mImpl(other.mImpl)
{
  other.mImpl = nullptr;
}

Property::Array::~Array()
{
  delete mImpl;
}

Property::Array::SizeType Property::Array::Count() const
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");
  return mImpl->mArray.size();
}

void Property::Array::PushBack(const Value& value)
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");
  if(mImpl->mHash != ALWAYS_REHASH && mImpl->mHash != NOT_HASHED)
  {
    // Use ordered hash operation.
    Dali::Internal::HashUtils::HashRawValue(value.GetHash(), mImpl->mHash);
  }
  mImpl->mArray.push_back(value);
}

void Property::Array::Clear()
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");
  mImpl->mArray.clear();
  mImpl->mHash = NOT_HASHED;
}

void Property::Array::Reserve(SizeType size)
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");
  mImpl->mArray.reserve(size);
}

void Property::Array::Resize(SizeType size)
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");
  if(mImpl->mArray.size() != size)
  {
    mImpl->mArray.resize(size);

    // Just reset hash as zero.
    if(mImpl->mHash != ALWAYS_REHASH)
    {
      mImpl->mHash = NOT_HASHED;
    }
  }
}

Property::Array::SizeType Property::Array::Capacity()
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");
  return mImpl->mArray.capacity();
}

const Property::Value& Property::Array::operator[](SizeType index) const
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");

  // Note says no bounds checking is performed so we don't need to verify mImpl as Count() will return 0 anyway
  return mImpl->mArray[index];
}

Property::Value& Property::Array::operator[](SizeType index)
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");

  // Mark as we should rehash always. (Since new value might be changed by application side anytime.)
  if(mImpl->mHash != ALWAYS_REHASH)
  {
    mImpl->mHash = ALWAYS_REHASH;
  }

  // Note says no bounds checking is performed so we don't need to verify mImpl as Count() will return 0 anyway
  return mImpl->mArray[index];
}

Property::Array& Property::Array::operator=(const Property::Array& other)
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");

  if(this != &other)
  {
    mImpl->mArray = other.mImpl->mArray;
    mImpl->mHash  = other.mImpl->mHash;
  }
  return *this;
}

Property::Array& Property::Array::operator=(Property::Array&& other) noexcept
{
  if(this != &other)
  {
    delete mImpl;
    mImpl       = other.mImpl;
    other.mImpl = nullptr;
  }
  return *this;
}

bool Property::Array::operator==(const Property::Array& rhs) const
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");

  // TODO : Need to check epsilon for float comparison in future. For now, just compare hash value and count.
  return Count() == rhs.Count() && GetHash() == rhs.GetHash();
}

std::size_t Property::Array::GetHash() const
{
  DALI_ASSERT_DEBUG(mImpl && "Cannot use an object previously used as an r-value");
  return mImpl->GetHash();
}

std::ostream& operator<<(std::ostream& stream, const Property::Array& array)
{
  stream << "Array(" << array.Count() << ") = [";
  for(Property::Array::SizeType i = 0; i < array.Count(); ++i)
  {
    if(i > 0)
    {
      stream << ", ";
    }
    stream << array.GetElementAt(i);
  }
  stream << "]";

  if(array.mImpl->mHash != NOT_HASHED)
  {
    stream << "(hash=" << array.mImpl->mHash << ")";
  }

  return stream;
}

} // namespace Dali
