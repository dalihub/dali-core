/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/internal/common/const-string.h>

// EXTERNAL INCLUDES
#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <functional>
#include <mutex>
#include <vector>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

// local namespace
namespace
{
class ArenaAllocator
{
public:
  ArenaAllocator()
  {
    mPages.reserve(10);
    EnsureSpace();
  }
  ~ArenaAllocator()
  {
    for(auto& page : mPages)
    {
      delete[] page;
    }
  }
  char* Allocate(uint32_t size, uint32_t alignment)
  {
    uintptr_t mask          = alignment - 1;
    uintptr_t alignedOffset = (~reinterpret_cast<uintptr_t>(mCursor) + 1) & mask;
    uintptr_t totalSize     = size + alignedOffset;

    if(totalSize > static_cast<uintptr_t>(mEnd - mCursor))
    {
      EnsureSpace();
      alignedOffset = (~reinterpret_cast<uintptr_t>(mCursor) + 1) & mask;
    }
    char* ptr = mCursor + alignedOffset;
    mCursor   = ptr + size;
    return ptr;
  }

private:
  void EnsureSpace()
  {
    constexpr size_t pageSize = 4096;
    char*            page     = new char[pageSize];
    mPages.push_back(page);
    mCursor = page;
    mEnd    = mCursor + pageSize;
  }

  char*              mCursor{nullptr};
  char*              mEnd{nullptr};
  std::vector<char*> mPages;
};

class StringEntry
{
public:
  explicit StringEntry(uint16_t stringLength)
  : mStringLength(stringLength)
  {
  }
  size_t getStringLength() const
  {
    return mStringLength;
  }
  const char* GetStringData() const
  {
    return reinterpret_cast<const char*>(this + 1);
  }

  std::string_view GetString() const
  {
    return std::string_view(GetStringData(), getStringLength());
  }
  /**
   * GetStringEntryFromData - Given key data that is known to be embedded
   * into a StringEntry, return the StringEntry itself.
   */
  static StringEntry& GetStringEntryFromData(const char* stringData)
  {
    char* ptr = const_cast<char*>(stringData) - sizeof(StringEntry);
    return *reinterpret_cast<StringEntry*>(ptr);
  }
  /**
   * Create a StringEntry from string_view
   *
   */
  static StringEntry* Create(std::string_view str, ArenaAllocator& allocator)
  {
    size_t length = str.size();

    // Allocate a new item with space for the string at the end and a null
    // terminator.
    size_t allocSize = sizeof(StringEntry) + length + 1;
    size_t alignment = alignof(StringEntry);

    // Construct the value.
    StringEntry* newItem = new(allocator.Allocate(allocSize, alignment)) StringEntry(length);

    // Copy the string information.
    char* strBuffer = const_cast<char*>(newItem->GetStringData());
    if(length > 0)
      memcpy(strBuffer, str.data(), length);
    strBuffer[length] = 0; // Null terminate.
    return newItem;
  }

private:
  uint16_t mStringLength;
};

class StringPool
{
public:
  unsigned Size() const
  {
    return mItems;
  }
  StringPool()
  {
    Init();
  }

  static StringPool& Instance()
  {
    static StringPool object;
    return object;
  }

  const char* Intern(std::string_view str)
  {
    const std::lock_guard<std::mutex> lock(mMutex);

    auto  bucketNumber = FindBucket(str);
    auto& bucket       = mTable[bucketNumber];

    if(bucket)
    {
      // string already exists
      return bucket->GetStringData();
    }

    // assign the newly created StringEntry to the designated bucket.
    bucket = StringEntry::Create(str, mAllocator);

    ++mItems;

    bucketNumber = RehashTable(bucketNumber);

    return mTable[bucketNumber]->GetStringData();
  }

private:
  unsigned NextBucket(unsigned currentBucket, unsigned totalBucket, unsigned ProbeAmount)
  {
    return (currentBucket + ProbeAmount) & (totalBucket - 1);
  }

  void Init()
  {
    unsigned newBuckets = 512;
    /**
     * for memory efficiency and cache locality we store the StringEntry and corresponding HashValue  in separate
     * segments in this way each entry in the bucket will take 12byte instead of 16 if we keep them together.
     * So we allocate one memory and treat 1st segment as a array of StringEntry* and the second segment as array of
     * unsigned*.
     */
    mTable = static_cast<StringEntry**>(calloc(newBuckets + 1, sizeof(StringEntry**) + sizeof(unsigned)));
    DALI_ASSERT_ALWAYS(mTable && "calloc returned nullptr");

    mBuckets = newBuckets;
  }

  unsigned FindBucket(std::string_view name)
  {
    unsigned bucketSize    = mBuckets;
    unsigned fullHashValue = std::hash<std::string_view>{}(name);
    unsigned bucketNumber  = fullHashValue & (bucketSize - 1);
    // point to the start of the hashvalue segment.
    unsigned* hashTable = reinterpret_cast<unsigned*>(mTable + mBuckets + 1);

    unsigned probeAmt = 1;
    while(true)
    {
      StringEntry* bucketItem = mTable[bucketNumber];
      // If we found an empty bucket, this key isn't in the table yet, return it.
      if(!bucketItem)
      {
        hashTable[bucketNumber] = fullHashValue;
        return bucketNumber;
      }

      if(hashTable[bucketNumber] == fullHashValue)
      {
        // If the full hash value matches, check deeply for a match.
        if(name == bucketItem->GetString())
        {
          // We found a match!
          return bucketNumber;
        }
      }

      // Okay, we didn't find the item.  Probe to the next bucket.
      bucketNumber = NextBucket(bucketNumber, bucketSize, probeAmt);

      // Use quadratic probing, it has fewer clumping artifacts than linear
      ++probeAmt;
    }
  }
  unsigned RehashTable(unsigned bucketNumber)
  {
    unsigned  newSize;
    unsigned* hashTable = reinterpret_cast<unsigned*>(mTable + mBuckets + 1);

    // If the hash table is now more than 3/4 full.
    // grow/rehash the table.
    if((mItems * 4) > (mBuckets * 3))
    {
      newSize = mBuckets * 2;
    }
    else
    {
      return bucketNumber;
    }
    unsigned newBucketNumber = bucketNumber;

    // Allocate one extra bucket which will always be non-empty.
    auto newTable = static_cast<StringEntry**>(calloc(newSize + 1, sizeof(StringEntry*) + sizeof(unsigned)));
    DALI_ASSERT_ALWAYS(newTable && "calloc returned nullptr");

    // point to the start of the hashvalue segment. as the pointer is of type StringEntry* , but the
    // second segment keeps only unsigned data hence the reinterpret_cast.
    unsigned* newHashTable = reinterpret_cast<unsigned*>(newTable + newSize + 1);

    // Rehash all the items into their new buckets.
    for(unsigned i = 0, j = mBuckets; i != j; ++i)
    {
      StringEntry* bucket = mTable[i];
      if(bucket)
      {
        // Fast case, bucket available.
        unsigned fullHash  = hashTable[i];
        unsigned newBucket = fullHash & (newSize - 1);
        if(!newTable[newBucket])
        {
          newTable[fullHash & (newSize - 1)]     = bucket;
          newHashTable[fullHash & (newSize - 1)] = fullHash;
          if(i == bucketNumber)
            newBucketNumber = newBucket;
          continue;
        }

        // Otherwise probe for a spot.
        unsigned ProbeSize = 1;
        do
        {
          newBucket = NextBucket(newBucket, newSize, ProbeSize++);
        } while(newTable[newBucket]);

        // Finally found a slot.  Fill it in.
        newTable[newBucket]     = bucket;
        newHashTable[newBucket] = fullHash;
        if(i == bucketNumber)
          newBucketNumber = newBucket;
      }
    }

    free(mTable);

    mTable   = newTable;
    mBuckets = newSize;
    return newBucketNumber;
  }

private:
  std::mutex     mMutex;
  ArenaAllocator mAllocator;
  StringEntry**  mTable{nullptr};
  unsigned       mBuckets{0};
  unsigned       mItems{0};
};

} // namespace

Dali::Internal::ConstString::ConstString(std::string_view str)
{
  mString = StringPool::Instance().Intern(str);
}

size_t Dali::Internal::ConstString::GetLength() const
{
  return mString ? StringEntry::GetStringEntryFromData(mString).getStringLength() : 0;
}

void Dali::Internal::ConstString::SetString(std::string_view str)
{
  if(str.empty())
  {
    mString = nullptr;
  }
  else
  {
    mString = StringPool::Instance().Intern(str);
  }
}
