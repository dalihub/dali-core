#ifndef DALI_INTEGRATION_API_OPEN_HASH_MAP_H
#define DALI_INTEGRATION_API_OPEN_HASH_MAP_H

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
 */

// EXTERNAL INCLUDES
#include <cstdint>
#include <cstdlib>     // calloc, free
#include <cstring>     // memset
#include <type_traits> // is_trivially_copyable_v, is_trivially_destructible_v

#include <dali/public-api/common/dali-common.h>

namespace Dali
{
namespace Integration
{
/**
 * @brief Traits class for OpenHashMap key types.
 *
 * Specialize this to provide Empty and Tombstone sentinels for your key type.
 * The default specialization handles pointer types.
 *
 * @tparam Key The key type
 */
template<typename Key>
struct OpenHashMapTraits
{
  /// Key value representing an empty slot (never occupied).
  static Key Empty()
  {
    return Key{};
  }

  /// Key value representing a deleted slot (tombstone).
  /// Must differ from Empty() and from any valid key.
  static Key Tombstone()
  {
    return reinterpret_cast<Key>(uintptr_t{1u});
  }
};

/**
 * @brief A lightweight open-addressed hash map with linear probing.
 *
 * Uses a single contiguous allocation (calloc) for the entry table.
 * Power-of-2 capacity, grows at 75% load. Deletion uses tombstones.
 *
 * Key requirements:
 * - Must be trivially copyable (stored by value in entries).
 * - Must support comparison to sentinels via ==.
 * - OpenHashMapTraits<Key> must provide Empty() and Tombstone() sentinels
 *   that are distinct from each other and from any valid key.
 *
 * @tparam Key      Key type (typically a pointer)
 * @tparam Value    Mapped value type
 * @tparam Hash     Hash functor: size_t operator()(const Key&) const
 * @tparam KeyEqual Equality functor: bool operator()(const Key&, const Key&) const
 */
template<typename Key, typename Value, typename Hash, typename KeyEqual>
class OpenHashMap
{
  static_assert(std::is_trivially_copyable_v<Key>, "Key must be trivially copyable");
  static_assert(std::is_trivially_destructible_v<Key>, "Key must be trivially destructible");
  static_assert(std::is_trivially_destructible_v<Value>, "Value must be trivially destructible (Clear uses memset)");

public:
  using TraitsType = OpenHashMapTraits<Key>;

private:
  struct Entry
  {
    Key   key;
    Value value;
  };

public:
  OpenHashMap() = default;

  ~OpenHashMap()
  {
    free(mEntries);
  }

  OpenHashMap(const OpenHashMap&)            = delete;
  OpenHashMap& operator=(const OpenHashMap&) = delete;

  OpenHashMap(OpenHashMap&& rhs) noexcept
  : mEntries(rhs.mEntries),
    mCapacity(rhs.mCapacity),
    mSize(rhs.mSize),
    mOccupied(rhs.mOccupied),
    mHash(std::move(rhs.mHash)),
    mKeyEqual(std::move(rhs.mKeyEqual))
  {
    rhs.mEntries  = nullptr;
    rhs.mCapacity = 0;
    rhs.mSize     = 0;
    rhs.mOccupied = 0;
  }

  OpenHashMap& operator=(OpenHashMap&& rhs) noexcept
  {
    if(this != &rhs)
    {
      free(mEntries);
      mEntries  = rhs.mEntries;
      mCapacity = rhs.mCapacity;
      mSize     = rhs.mSize;
      mOccupied = rhs.mOccupied;
      mHash     = std::move(rhs.mHash);
      mKeyEqual = std::move(rhs.mKeyEqual);

      rhs.mEntries  = nullptr;
      rhs.mCapacity = 0;
      rhs.mSize     = 0;
      rhs.mOccupied = 0;
    }
    return *this;
  }

  /**
   * @brief Insert or update a key-value pair.
   * @param[in] key   The key to insert
   * @param[in] value The value to associate
   * @return Pointer to the inserted/updated entry's value
   */
  Value* Insert(const Key& key, const Value& value)
  {
    if(mOccupied * 4u >= mCapacity * 3u) // >= 75% load
    {
      Grow();
    }

    const Key empty     = TraitsType::Empty();
    const Key tombstone = TraitsType::Tombstone();

    if(key == empty)
    {
      DALI_ABORT("OpenHashMap does not allow insertion of OpenHashMapTraits<Key>::Empty as key!");
    }
    if(key == tombstone)
    {
      DALI_ABORT("OpenHashMap does not allow insertion of OpenHashMapTraits<Key>::Tombstone as key!");
    }
    const uint32_t mask           = mCapacity - 1u;
    uint32_t       idx            = HashIndex(key, mask);
    int32_t        firstTombstone = -1;

    while(true)
    {
      const Key& k = mEntries[idx].key;

      if(k == empty)
      {
        // Slot is empty — key not present. Insert here (or at earlier tombstone).
        if(firstTombstone >= 0)
        {
          idx = static_cast<uint32_t>(firstTombstone);
          // Reusing a tombstone doesn't increase mOccupied
        }
        else
        {
          ++mOccupied;
        }
        mEntries[idx].key   = key;
        mEntries[idx].value = value;
        ++mSize;
        return &mEntries[idx].value;
      }

      if(k == tombstone)
      {
        if(firstTombstone < 0)
        {
          firstTombstone = static_cast<int32_t>(idx);
        }
      }
      else if(mKeyEqual(k, key))
      {
        // Key already present — update value.
        mEntries[idx].value = value;
        return &mEntries[idx].value;
      }

      idx = (idx + 1u) & mask;
    }
  }

  /**
   * @brief Find a value by key.
   * @param[in] key The key to look up
   * @return Pointer to the value, or nullptr if not found
   */
  Value* Find(const Key& key) const
  {
    if(mCapacity == 0u)
    {
      return nullptr;
    }

    const Key empty     = TraitsType::Empty();
    const Key tombstone = TraitsType::Tombstone();
    uint32_t  mask      = mCapacity - 1u;
    uint32_t  idx       = HashIndex(key, mask);

    while(true)
    {
      const Key& k = mEntries[idx].key;

      if(k == empty)
      {
        return nullptr;
      }

      if(k != tombstone && mKeyEqual(k, key))
      {
        return &mEntries[idx].value;
      }

      idx = (idx + 1u) & mask;
    }
  }

  /**
   * @brief Erase an entry by key.
   * @param[in] key The key to erase
   * @return true if the key was found and erased, false otherwise
   */
  bool Erase(const Key& key)
  {
    if(mCapacity == 0u)
    {
      return false;
    }

    const Key empty     = TraitsType::Empty();
    const Key tombstone = TraitsType::Tombstone();
    uint32_t  mask      = mCapacity - 1u;
    uint32_t  idx       = HashIndex(key, mask);

    while(true)
    {
      const Key& k = mEntries[idx].key;

      if(k == empty)
      {
        return false;
      }

      if(k != tombstone && mKeyEqual(k, key))
      {
        mEntries[idx].key   = tombstone;
        mEntries[idx].value = Value{};
        --mSize;
        return true;
      }

      idx = (idx + 1u) & mask;
    }
  }

  /**
   * @brief Remove all entries.
   */
  void Clear()
  {
    // If the table exists, and is occupied, clear it.
    if(mEntries && mOccupied > 0)
    {
      // Zero the entire table — all keys become Empty (nullptr for pointers).
      memset(mEntries, 0, mCapacity * sizeof(Entry));
    }
    mSize     = 0u;
    mOccupied = 0u;
  }

  /**
   * @brief Call a functor for each live entry.
   * @param[in] functor Called with (const Key& key, Value& value) for each entry
   */
  template<typename Functor>
  void ForEach(Functor&& functor)
  {
    const Key empty     = TraitsType::Empty();
    const Key tombstone = TraitsType::Tombstone();

    for(uint32_t i = 0u; i < mCapacity; ++i)
    {
      const Key& k = mEntries[i].key;
      if(k != empty && k != tombstone)
      {
        functor(k, mEntries[i].value);
      }
    }
  }

  /**
   * @brief Call a functor for each live entry (const version).
   * @param[in] functor Called with (const Key& key, const Value& value) for each entry
   */
  template<typename Functor>
  void ForEach(Functor&& functor) const
  {
    const Key empty     = TraitsType::Empty();
    const Key tombstone = TraitsType::Tombstone();

    for(uint32_t i = 0u; i < mCapacity; ++i)
    {
      const Key& k = mEntries[i].key;
      if(k != empty && k != tombstone)
      {
        functor(k, mEntries[i].value);
      }
    }
  }

  /**
   * @return Number of live entries.
   */
  uint32_t Size() const
  {
    return mSize;
  }

  /**
   * @return true if there are no live entries.
   */
  bool Empty() const
  {
    return mSize == 0u;
  }

  /**
   * @return Current table capacity (always a power of 2, or 0).
   */
  uint32_t Capacity() const
  {
    return mCapacity;
  }

  /**
   * @brief Shrink the table to the smallest power-of-2 capacity that keeps
   *        load under 75%, clearing all tombstones in the process.
   *
   * If the map is empty, the table is freed entirely (capacity becomes 0).
   * If the current capacity is already optimal, this is a no-op.
   */
  void Rehash()
  {
    if(mSize == 0u)
    {
      free(mEntries);
      mEntries  = nullptr;
      mCapacity = 0u;
      mOccupied = 0u;
      return;
    }

    // Find the smallest power-of-2 capacity where mSize / capacity < 75%.
    // That is: capacity > mSize * 4 / 3.
    uint32_t minCapacity = INITIAL_CAPACITY;
    while(minCapacity * 3u <= mSize * 4u)
    {
      minCapacity *= 2u;
    }

    if(minCapacity < mCapacity)
    {
      RehashTo(minCapacity);
    }
    else if(mOccupied > mSize)
    {
      // Capacity is already minimal, but there are tombstones — rehash in-place to clear them.
      RehashTo(mCapacity);
    }
  }

private:
  static constexpr uint32_t INITIAL_CAPACITY = 8u;

  /**
   * @brief Hash a key and return a table index.
   *
   * Applies a finalizer (splitmix64-style) to the raw hash to improve
   * bit distribution for linear probing with power-of-2 table sizes.
   */
  uint32_t HashIndex(const Key& key, uint32_t mask) const
  {
    uint64_t h = static_cast<uint64_t>(mHash(key));
    h ^= h >> 33;
    h *= 0xff51afd7ed558ccdULL;
    h ^= h >> 33;
    return static_cast<uint32_t>(h) & mask;
  }

  void Grow()
  {
    RehashTo((mCapacity == 0u) ? INITIAL_CAPACITY : mCapacity * 2u);
  }

  void RehashTo(uint32_t newCapacity)
  {
    Entry* newEntries = static_cast<Entry*>(calloc(newCapacity, sizeof(Entry)));
    DALI_ASSERT_ALWAYS(newEntries && "OpenHashMap::RehashTo() - Memory allocation failed");

    const Key empty     = TraitsType::Empty();
    const Key tombstone = TraitsType::Tombstone();
    uint32_t  newMask   = newCapacity - 1u;

    // Re-insert all live entries into the new table.
    for(uint32_t i = 0u; i < mCapacity; ++i)
    {
      const Key& k = mEntries[i].key;
      if(k != empty && k != tombstone)
      {
        uint32_t idx = HashIndex(k, newMask);
        while(newEntries[idx].key != empty)
        {
          idx = (idx + 1u) & newMask;
        }
        newEntries[idx].key   = k;
        newEntries[idx].value = mEntries[i].value;
      }
    }

    free(mEntries);
    mEntries  = newEntries;
    mCapacity = newCapacity;
    mOccupied = mSize; // Tombstones are gone after rehash.
  }

  Entry*   mEntries{nullptr};
  uint32_t mCapacity{0};
  uint32_t mSize{0};     ///< Live entries.
  uint32_t mOccupied{0}; ///< Live entries + tombstones (drives load-factor check).

  Hash     mHash{};
  KeyEqual mKeyEqual{};
};

} // namespace Integration
} // namespace Dali

#endif // DALI_INTEGRATION_API_OPEN_HASH_MAP_H
