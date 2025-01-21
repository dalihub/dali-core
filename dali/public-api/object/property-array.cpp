/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
  if(DALI_LIKELY(other.mImpl))
  {
    mImpl->mArray = other.mImpl->mArray;
    // Keep mHash as NOT_HASHED.
  }
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
  return DALI_LIKELY(mImpl) ? mImpl->mArray.size() : 0;
}

void Property::Array::PushBack(const Value& value)
{
  if(DALI_UNLIKELY(!mImpl))
  {
    mImpl = new Impl();
  }

  if(mImpl->mHash != ALWAYS_REHASH && mImpl->mHash != NOT_HASHED)
  {
    // Use ordered hash operation.
    Dali::Internal::HashUtils::HashRawValue(value.GetHash(), mImpl->mHash);
  }
  mImpl->mArray.push_back(value);
}

void Property::Array::Clear()
{
  if(DALI_LIKELY(mImpl))
  {
    mImpl->mArray.clear();
    mImpl->mHash = NOT_HASHED;
  }
}

void Property::Array::Reserve(SizeType size)
{
  if(DALI_UNLIKELY(!mImpl))
  {
    mImpl = new Impl();
  }

  mImpl->mArray.reserve(size);
}

void Property::Array::Resize(SizeType size)
{
  if(DALI_UNLIKELY(!mImpl))
  {
    mImpl = new Impl();
  }

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
  return DALI_LIKELY(mImpl) ? mImpl->mArray.capacity() : 0;
}

const Property::Value& Property::Array::operator[](SizeType index) const
{
  DALI_ASSERT_ALWAYS(mImpl && "Cannot use an object previously used as an r-value");

  // Note says no bounds checking is performed so we don't need to verify mImpl as Count() will return 0 anyway
  return mImpl->mArray[index];
}

Property::Value& Property::Array::operator[](SizeType index)
{
  DALI_ASSERT_ALWAYS(mImpl && "Cannot use an object previously used as an r-value");

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
  if(this != &other)
  {
    if(DALI_UNLIKELY(other.mImpl == nullptr))
    {
      Clear();
    }
    else
    {
      if(DALI_UNLIKELY(!mImpl))
      {
        mImpl = new Impl();
      }
      mImpl->mArray = other.mImpl->mArray;
      mImpl->mHash  = other.mImpl->mHash;
    }
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
  if(DALI_UNLIKELY(this == &rhs))
  {
    // Fast out for self comparision
    return true;
  }

  const auto lhsCount = Count();
  if(lhsCount != rhs.Count())
  {
    return false;
  }
  if(DALI_UNLIKELY(mImpl == nullptr))
  {
    return rhs.Empty();
  }
  if(DALI_UNLIKELY(rhs.mImpl == nullptr))
  {
    return Empty();
  }

  for(SizeType i = 0u; i < lhsCount; ++i)
  {
    if(mImpl->mArray[i] != rhs.mImpl->mArray[i])
    {
      return false;
    }
  }
  return true;
}

std::size_t Property::Array::GetHash() const
{
  return DALI_LIKELY(mImpl) ? mImpl->GetHash() : Dali::Internal::HashUtils::INITIAL_HASH_VALUE;
}

std::ostream& operator<<(std::ostream& stream, const Property::Array& array)
{
  stream << "Array(" << array.Count() << ") = [";

  if(DALI_LIKELY(array.mImpl))
  {
    for(Property::Array::SizeType i = 0; i < array.Count(); ++i)
    {
      if(i > 0)
      {
        stream << ", ";
      }
      stream << array.GetElementAt(i);
    }

    if(array.mImpl->mHash != NOT_HASHED)
    {
      stream << "(hash=" << array.mImpl->mHash << ")";
    }
  }

  stream << "]";

  return stream;
}

} // namespace Dali
