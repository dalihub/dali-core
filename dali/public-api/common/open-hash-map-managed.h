#ifndef DALI_PUBLIC_API_COMMON_OPEN_HASH_MAP_MANAGED_H
#define DALI_PUBLIC_API_COMMON_OPEN_HASH_MAP_MANAGED_H

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/dali-pair.h>
#include <dali/public-api/common/type-traits.h>
#include <dali/public-api/math/math-utils.h>

namespace Dali
{
/**
 * @addtogroup dali_core_common
 * @{
 */

/**
 * @brief Entry state for ManagedOpenHashMap slots.
 * @SINCE_2_5.20
 */
enum class EntryState : uint8_t
{
  EMPTY,    ///< Slot has never been occupied.
  OCCUPIED, ///< Slot contains a live key-value pair.
  TOMBSTONE ///< Slot was occupied but has been erased.
};

/**
 * @brief A managed open-addressed hash map with linear probing.
 *
 * Unlike OpenHashMap (which requires trivially-copyable/destructible types and
 * uses calloc/memset for zero-overhead operation), ManagedOpenHashMap properly
 * manages the lifecycle of its keys and values via constructors, destructors,
 * and move semantics. This allows it to work with non-trivial types such as
 * Dali::String.
 *
 * Uses a single contiguous allocation (new Entry[]) for the entry table.
 * Power-of-2 capacity, grows at 75% load. Deletion uses tombstones.
 * Entry state is tracked via an explicit EntryState enum per slot.
 *
 * Key requirements:
 * - Must be default-constructible (required by Entry struct and new Entry[n]() in RehashTo/Grow).
 * - Must be copyable and/or movable.
 * - Must support comparison via KeyEqual.
 *
 * Value requirements:
 * - Must be default-constructible (required by Entry struct and new Entry[n]() in RehashTo/Grow).
 * - Must be copyable and/or movable.
 *
 * Note: the default-constructibility requirement exists because Entry holds Key and Value as
 * plain members with default-initialisation (key{}, value{}), and RehashTo allocates new
 * tables with new Entry[n]() which value-initialises every slot. Removing this constraint
 * would require separating storage from construction (e.g. via placement new into raw memory),
 * which is a more invasive change deferred for a future revision.
 *
 * Resource release policy:
 * - Erase() and Clear() do not reset key/value objects in the affected slots.
 *   Resources are released lazily: on slot reuse (Insert overwrites) or when
 *   the table is destroyed (delete[] destructs every Entry). To release
 *   resources eagerly after a Clear(), call Rehash() immediately afterwards —
 *   it frees the entire table buffer when size is zero.
 *
 * Exception safety:
 * - All operations provide at least the basic guarantee (no leaks, no dangling pointers).
 * - Rehash/Grow provides the strong guarantee: if it throws, the map is unchanged.
 * - Move constructor is noexcept conditional on Hash and KeyEqual being nothrow-move-constructible.
 * - Move assignment is noexcept conditional on Hash and KeyEqual being nothrow-move-assignable.
 * - RehashTo always copies (never moves) key/value into the new table. This unconditionally
 *   preserves the strong guarantee: if any copy throws, the old table is still fully intact
 *   and the new (partial) table is discarded. The cost is that rehash is always O(n) copy
 *   work rather than O(n) move work, but correctness is not compromised even for types
 *   whose move-assignment can throw.
 *
 * IMPORTANT — differences from std::unordered_map:
 *
 * 1. Pointer/iterator stability.
 *    std::unordered_map guarantees that pointers and references to values remain
 *    valid as long as the element is not erased. THIS MAP DOES NOT. Any Insert()
 *    that triggers a grow reallocates the entire Entry[] array. Every Value* or
 *    Entry* obtained from a previous Find(), Insert(), operator[], At(), or
 *    iterator dereference is INVALIDATED by any subsequent insert that grows the
 *    table. Do not cache pointers or references across inserts.
 *
 * 2. Insert() always overwrites; std::unordered_map::insert() does not.
 *    In standard maps, insert({key, value}) is a no-op when the key already
 *    exists — only insert_or_assign() and operator[] overwrite. Here, Insert()
 *    always overwrites the existing value on collision. Use TryInsert() for
 *    insert-if-absent semantics that match std::unordered_map::insert().
 *
 * 3. No exception on missing key.
 *    std::unordered_map::at() throws std::out_of_range when the key is absent.
 *    At() in this map calls DALI_ASSERT_ALWAYS instead. operator[] inserts a
 *    default-constructed value for missing keys (same as std::unordered_map).
 *
 * API summary (std::unordered_map equivalents):
 *   map[key]                    -> operator[]          (insert-or-get, overwrites)
 *   map.at(key)                 -> At()                (assert on miss, no insert)
 *   map.insert({key, value})    -> TryInsert()         (insert-if-absent only)
 *   map.insert_or_assign(k, v)  -> Insert()            (always overwrites)
 *   map.find(key)               -> Find()              (returns Value* or nullptr)
 *   map.contains(key)           -> Contains()
 *   map.count(key)              -> Count()
 *   map.erase(key)              -> Erase()
 *   map.clear()                 -> Clear()
 *   for(auto& kv : map)         -> range-for via begin()/end(), use kv.key()/kv.value()
 *
 * @tparam Key      Key type
 * @tparam Value    Mapped value type
 * @tparam Hash     Hash functor: size_t operator()(const Key&) const
 * @tparam KeyEqual Equality functor: bool operator()(const Key&, const Key&) const
 * @SINCE_2_5.20
 */
template<typename Key, typename Value, typename Hash, typename KeyEqual>
class ManagedOpenHashMap
{
public:
  /**
   * @brief A single entry in the hash table.
   * @SINCE_2_5.20
   */
  struct Entry
  {
    Key        key{};
    Value      value{};
    EntryState state{EntryState::EMPTY};
  };

  /**
   * @brief Result type returned by TryInsert.
   *
   * Equivalent to the std::pair<iterator, bool> returned by std::unordered_map::insert:
   *   - first  : pointer to the value for the key (never null)
   *   - second : true if a new entry was inserted, false if the key already existed
   *
   * Uses Dali::Pair so callers get the full Dali::Pair interface (comparison
   * operators, Swap, MakePair) and the familiar first/second field names without
   * depending on any bespoke type.
   * @SINCE_2_5.20
   */
  using InsertResult = Dali::Pair<Value*, bool>;

  // -------------------------------------------------------------------------
  // Iterator — forward iterator over live (OCCUPIED) entries.
  //
  // Satisfies C++ InputIterator requirements:
  //   - operator*()  returns a stable lvalue reference (not a temporary proxy)
  //   - operator->() returns a pointer to the same stable object
  //   - operator++() advances to the next OCCUPIED slot
  //   - operator==() / operator!=() compare positions
  //
  // Design: each iterator owns a value_type member (KeyValuePair / ConstKeyValuePair)
  // that holds non-owning pointers into the live Entry array. operator*() returns a
  // reference to this member and operator->() returns its address — both stable for
  // the lifetime of the iterator between increments.
  //
  // This avoids the dangling-reference problem that arises when operator*()
  // returns a proxy struct by value and operator->() tries to take its address.
  //
  // Invalidated by any operation that may grow or rehash the table (Insert,
  // operator[], TryInsert, Rehash). Safe to use across Erase only (Erase does
  // not reallocate the entry array).
  // -------------------------------------------------------------------------

  /**
   * @brief Value type exposed by Iterator.
   *
   * Provides stable references to the key (const) and value (mutable) of a
   * live entry. The key is exposed as const to match std::unordered_map
   * semantics, which prohibits mutating a key through an iterator.
   *
   * Supports:
   *   it->key() / it->value()   — via operator-> on Iterator
   *   (*it).key() / (*it).value() — via operator* on Iterator
   *   kv.key() / kv.value()     — via range-for: for(auto& kv : map)
   * @SINCE_2_5.20
   */
  struct KeyValuePair
  {
    const Key* keyPtr{nullptr};
    Value*     valuePtr{nullptr};

    const Key& key() const
    {
      return *keyPtr;
    }
    Value& value() const
    {
      return *valuePtr;
    }
  };

  /**
   * @brief Value type exposed by ConstIterator.
   *
   * Provides stable const references to both key and value.
   * @SINCE_2_5.20
   */
  struct ConstKeyValuePair
  {
    const Key*   keyPtr{nullptr};
    const Value* valuePtr{nullptr};

    const Key& key() const
    {
      return *keyPtr;
    }
    const Value& value() const
    {
      return *valuePtr;
    }
  };

  /**
   * @brief Forward iterator over live entries.
   *
   * operator*()  — returns KeyValuePair& (stable lvalue, satisfies InputIterator)
   * operator->() — returns KeyValuePair* (pointer to the same stable object)
   *
   * Usage:
   * @code
   *   for(auto it = map.begin(); it != map.end(); ++it)
   *   {
   *     DoSomething(it->key(), it->value());   // via operator->
   *     DoSomething((*it).key(), (*it).value()); // via operator*
   *   }
   *
   *   // Range-for:
   *   for(auto& kv : map)
   *   {
   *     DoSomething(kv.key(), kv.value());
   *   }
   * @endcode
   * @SINCE_2_5.20
   */
  class Iterator
  {
  public:
    Iterator(Entry* entries, uint32_t capacity, uint32_t index)
    : mEntries(entries),
      mCapacity(capacity),
      mIndex(index)
    {
      Advance();
      Sync();
    }

    KeyValuePair& operator*()
    {
      return mCurrent;
    }

    const KeyValuePair& operator*() const
    {
      return mCurrent;
    }

    KeyValuePair* operator->()
    {
      return &mCurrent;
    }

    const KeyValuePair* operator->() const
    {
      return &mCurrent;
    }

    Iterator& operator++()
    {
      ++mIndex;
      Advance();
      Sync();
      return *this;
    }

    bool operator!=(const Iterator& rhs) const
    {
      return mIndex != rhs.mIndex;
    }

    bool operator==(const Iterator& rhs) const
    {
      return mIndex == rhs.mIndex;
    }

  private:
    // Skip forward past non-OCCUPIED slots.
    void Advance()
    {
      while(mIndex < mCapacity && mEntries[mIndex].state != EntryState::OCCUPIED)
      {
        ++mIndex;
      }
    }

    // Update mCurrent to point into the current slot.
    // When mIndex == mCapacity (end), leaves mCurrent with null pointers —
    // dereferencing end() is UB by contract, consistent with standard iterators.
    void Sync()
    {
      if(mIndex < mCapacity)
      {
        mCurrent.keyPtr   = &mEntries[mIndex].key;
        mCurrent.valuePtr = &mEntries[mIndex].value;
      }
    }

    Entry*       mEntries{nullptr};
    uint32_t     mCapacity{0u};
    uint32_t     mIndex{0u};
    KeyValuePair mCurrent{};
  };

  /**
   * @brief Const forward iterator over live entries.
   *
   * operator*()  — returns ConstKeyValuePair& (stable lvalue)
   * operator->() — returns ConstKeyValuePair*
   *
   * Usage:
   * @code
   *   for(auto it = map.cbegin(); it != map.cend(); ++it)
   *   {
   *     DoSomething(it->key(), it->value());
   *   }
   * @endcode
   * @SINCE_2_5.20
   */
  class ConstIterator
  {
  public:
    ConstIterator(const Entry* entries, uint32_t capacity, uint32_t index)
    : mEntries(entries),
      mCapacity(capacity),
      mIndex(index)
    {
      Advance();
      Sync();
    }

    const ConstKeyValuePair& operator*() const
    {
      return mCurrent;
    }

    const ConstKeyValuePair* operator->() const
    {
      return &mCurrent;
    }

    ConstIterator& operator++()
    {
      ++mIndex;
      Advance();
      Sync();
      return *this;
    }

    bool operator!=(const ConstIterator& rhs) const
    {
      return mIndex != rhs.mIndex;
    }

    bool operator==(const ConstIterator& rhs) const
    {
      return mIndex == rhs.mIndex;
    }

  private:
    void Advance()
    {
      while(mIndex < mCapacity && mEntries[mIndex].state != EntryState::OCCUPIED)
      {
        ++mIndex;
      }
    }

    void Sync()
    {
      if(mIndex < mCapacity)
      {
        mCurrent.keyPtr   = &mEntries[mIndex].key;
        mCurrent.valuePtr = &mEntries[mIndex].value;
      }
    }

    const Entry*       mEntries{nullptr};
    uint32_t           mCapacity{0u};
    uint32_t           mIndex{0u};
    ConstKeyValuePair  mCurrent{};
  };

public:
  /**
   * @brief Default constructor.
   *
   * Creates an empty map with no allocated table (capacity 0).
   * The table is allocated on the first Insert.
   * @SINCE_2_5.20
   */
  ManagedOpenHashMap() = default;

  /**
   * @brief Destructor.
   *
   * Destroys all live key-value pairs and frees the entry table.
   * @SINCE_2_5.20
   */
  ~ManagedOpenHashMap()
  {
    delete[] mEntries;
  }

  /**
   * @brief Copy constructor is deleted.
   *
   * ManagedOpenHashMap is not copyable because copying a hash map with
   * non-trivial key/value types is expensive and easily done unintentionally.
   * Use move semantics or manually copy entries via ForEach() + Insert().
   * @SINCE_2_5.20
   */
  ManagedOpenHashMap(const ManagedOpenHashMap&)            = delete;

  /**
   * @brief Copy assignment operator is deleted.
   *
   * ManagedOpenHashMap is not copy-assignable for the same reason as the
   * copy constructor.
   * @SINCE_2_5.20
   */
  ManagedOpenHashMap& operator=(const ManagedOpenHashMap&) = delete;

  /**
   * @brief Move constructor.
   *
   * Transfers ownership of the entry table from @p rhs to this map.
   * After the move, @p rhs is in a valid empty state (size 0, capacity 0).
   *
   * noexcept is conditional on Hash and KeyEqual being nothrow-move-constructible.
   * If either functor's move constructor can throw, std::terminate would be called.
   *
   * @param[in] rhs The map to move from
   * @SINCE_2_5.20
   */
  ManagedOpenHashMap(ManagedOpenHashMap&& rhs) noexcept(IsNothrowMoveConstructible<Hash>::value && IsNothrowMoveConstructible<KeyEqual>::value)
  : mEntries(rhs.mEntries),
    mCapacity(rhs.mCapacity),
    mSize(rhs.mSize),
    mOccupied(rhs.mOccupied),
    mHash(Dali::Move(rhs.mHash)),
    mKeyEqual(Dali::Move(rhs.mKeyEqual))
  {
    rhs.mEntries  = nullptr;
    rhs.mCapacity = 0;
    rhs.mSize     = 0;
    rhs.mOccupied = 0;
  }

  /**
   * @brief Move assignment operator.
   *
   * Frees this map's current entry table, then transfers ownership of the
   * entry table from @p rhs. After the move, @p rhs is in a valid empty state
   * (size 0, capacity 0).
   *
   * noexcept is conditional on Hash and KeyEqual being nothrow-move-assignable.
   *
   * @param[in] rhs The map to move from
   * @return Reference to this map
   * @SINCE_2_5.20
   */
  ManagedOpenHashMap& operator=(ManagedOpenHashMap&& rhs) noexcept(IsNothrowMoveAssignable<Hash>::value && IsNothrowMoveAssignable<KeyEqual>::value)
  {
    if(this != &rhs)
    {
      delete[] mEntries;
      mEntries  = rhs.mEntries;
      mCapacity = rhs.mCapacity;
      mSize     = rhs.mSize;
      mOccupied = rhs.mOccupied;
      mHash     = Dali::Move(rhs.mHash);
      mKeyEqual = Dali::Move(rhs.mKeyEqual);

      rhs.mEntries  = nullptr;
      rhs.mCapacity = 0;
      rhs.mSize     = 0;
      rhs.mOccupied = 0;
    }
    return *this;
  }

  /**
   * @brief Insert or update a key-value pair (copy both).
   *
   * WARNING: Unlike std::unordered_map::insert(), this always overwrites an
   * existing value when the key is already present. Use TryInsert() for
   * insert-if-absent semantics.
   *
   * WARNING: Any pointer or iterator obtained before this call may be
   * invalidated if the insert triggers a table grow.
   *
   * @param[in] key   The key to insert
   * @param[in] value The value to associate
   * @return Pointer to the inserted/updated entry's value
   * @SINCE_2_5.20
   */
  Value* Insert(const Key& key, const Value& value)
  {
    return InsertImpl(key, value);
  }

  /**
   * @brief Insert or update a key-value pair (move both).
   *
   * WARNING: Unlike std::unordered_map::insert(), this always overwrites an
   * existing value when the key is already present. Use TryInsert() for
   * insert-if-absent semantics.
   *
   * WARNING: Any pointer or iterator obtained before this call may be
   * invalidated if the insert triggers a table grow.
   *
   * @param[in] key   The key to insert (rvalue reference)
   * @param[in] value The value to associate (rvalue reference)
   * @return Pointer to the inserted/updated entry's value
   * @SINCE_2_5.20
   */
  Value* Insert(Key&& key, Value&& value)
  {
    return InsertImpl(Dali::Move(key), Dali::Move(value));
  }

  /**
   * @brief Insert or update a key-value pair (move key, copy value).
   *
   * WARNING: Unlike std::unordered_map::insert(), this always overwrites an
   * existing value when the key is already present. Use TryInsert() for
   * insert-if-absent semantics.
   *
   * WARNING: Any pointer or iterator obtained before this call may be
   * invalidated if the insert triggers a table grow.
   *
   * @param[in] key   The key to insert (rvalue reference)
   * @param[in] value The value to associate
   * @return Pointer to the inserted/updated entry's value
   * @SINCE_2_5.20
   */
  Value* Insert(Key&& key, const Value& value)
  {
    return InsertImpl(Dali::Move(key), value);
  }

  /**
   * @brief Insert or update a key-value pair (copy key, move value).
   *
   * WARNING: Unlike std::unordered_map::insert(), this always overwrites an
   * existing value when the key is already present. Use TryInsert() for
   * insert-if-absent semantics.
   *
   * WARNING: Any pointer or iterator obtained before this call may be
   * invalidated if the insert triggers a table grow.
   *
   * @param[in] key   The key to insert
   * @param[in] value The value to associate (rvalue reference)
   * @return Pointer to the inserted/updated entry's value
   * @SINCE_2_5.20
   */
  Value* Insert(const Key& key, Value&& value)
  {
    return InsertImpl(key, Dali::Move(value));
  }

  /**
   * @brief Access or insert a value by key.
   *
   * If the key is present, returns a reference to the existing value unchanged.
   * If the key is absent, inserts a default-constructed Value and returns
   * a reference to it. This matches std::unordered_map::operator[] exactly.
   *
   * Implementation note: routes through TryInsertImpl (not InsertImpl) so that
   * an existing value is never overwritten by a default-constructed Value{}.
   * Using InsertImpl here would silently reset an existing value to Value{} on
   * every operator[] access for a key that is already present.
   *
   * WARNING: Any reference obtained before this call may be invalidated if
   * the call triggers a table grow.
   *
   * @param[in] key The key to look up or insert
   * @return Reference to the value associated with key
   * @SINCE_2_5.20
   */
  Value& operator[](const Key& key)
  {
    return *TryInsertImpl(key, Value{}).first;
  }

  /**
   * @brief Access a value by key with an assertion on miss (non-const).
   *
   * Equivalent to std::unordered_map::at() but calls DALI_ASSERT_ALWAYS
   * instead of throwing std::out_of_range when the key is absent.
   * Does NOT insert a default value for missing keys (unlike operator[]).
   *
   * @param[in] key The key to look up
   * @return Reference to the value associated with key
   * @SINCE_2_5.20
   */
  Value& At(const Key& key)
  {
    Value* value = Find(key);
    DALI_ASSERT_ALWAYS(value && "ManagedOpenHashMap::At() - Key not found");
    return *value;
  }

  /**
   * @brief Access a value by key with an assertion on miss (const).
   *
   * Equivalent to std::unordered_map::at() const but calls DALI_ASSERT_ALWAYS
   * instead of throwing std::out_of_range when the key is absent.
   *
   * @param[in] key The key to look up
   * @return Const reference to the value associated with key
   * @SINCE_2_5.20
   */
  const Value& At(const Key& key) const
  {
    const Value* value = Find(key);
    DALI_ASSERT_ALWAYS(value && "ManagedOpenHashMap::At() - Key not found");
    return *value;
  }

  /**
   * @brief Insert a key-value pair only if the key is absent (copy both).
   *
   * Equivalent to std::unordered_map::insert(). Unlike Insert(), this never
   * overwrites an existing value. If the key is already present, the existing
   * value is left unchanged.
   *
   * WARNING: Any pointer or iterator obtained before this call may be
   * invalidated if the insert triggers a table grow.
   *
   * @param[in] key   The key to insert
   * @param[in] value The value to associate
   * @return InsertResult::first  — pointer to the value (existing or newly inserted)
   *         InsertResult::second — true if inserted, false if key already existed
   * @SINCE_2_5.20
   */
  InsertResult TryInsert(const Key& key, const Value& value)
  {
    return TryInsertImpl(key, value);
  }

  /**
   * @brief Insert a key-value pair only if the key is absent (move both).
   *
   * @param[in] key   The key to insert (rvalue reference)
   * @param[in] value The value to associate (rvalue reference)
   * @return InsertResult
   * @SINCE_2_5.20
   */
  InsertResult TryInsert(Key&& key, Value&& value)
  {
    return TryInsertImpl(Dali::Move(key), Dali::Move(value));
  }

  /**
   * @brief Insert a key-value pair only if the key is absent (move key, copy value).
   *
   * @param[in] key   The key to insert (rvalue reference)
   * @param[in] value The value to associate
   * @return InsertResult
   * @SINCE_2_5.20
   */
  InsertResult TryInsert(Key&& key, const Value& value)
  {
    return TryInsertImpl(Dali::Move(key), value);
  }

  /**
   * @brief Insert a key-value pair only if the key is absent (copy key, move value).
   *
   * @param[in] key   The key to insert
   * @param[in] value The value to associate (rvalue reference)
   * @return InsertResult
   * @SINCE_2_5.20
   */
  InsertResult TryInsert(const Key& key, Value&& value)
  {
    return TryInsertImpl(key, Dali::Move(value));
  }

  /**
   * @brief Find a value by key.
   * @param[in] key The key to look up
   * @return Pointer to the value, or nullptr if not found
   * @SINCE_2_5.20
   */
  Value* Find(const Key& key)
  {
    if(mCapacity == 0u)
    {
      return nullptr;
    }

    const uint32_t mask = mCapacity - 1u;
    uint32_t       idx  = HashIndex(key, mask);

    // Always terminates: 75% load-factor guarantee ensures at least one EMPTY slot exists.
    while(true)
    {
      Entry& entry = mEntries[idx];

      if(entry.state == EntryState::EMPTY)
      {
        return nullptr;
      }

      if(entry.state == EntryState::OCCUPIED && mKeyEqual(entry.key, key))
      {
        return &entry.value;
      }

      idx = (idx + 1u) & mask;
    }
  }

  /**
   * @brief Find a value by key (const).
   * @param[in] key The key to look up
   * @return Pointer to the value, or nullptr if not found
   * @SINCE_2_5.20
   */
  const Value* Find(const Key& key) const
  {
    if(mCapacity == 0u)
    {
      return nullptr;
    }

    const uint32_t mask = mCapacity - 1u;
    uint32_t       idx  = HashIndex(key, mask);

    // Always terminates: 75% load-factor guarantee ensures at least one EMPTY slot exists.
    while(true)
    {
      const Entry& entry = mEntries[idx];

      if(entry.state == EntryState::EMPTY)
      {
        return nullptr;
      }

      if(entry.state == EntryState::OCCUPIED && mKeyEqual(entry.key, key))
      {
        return &entry.value;
      }

      idx = (idx + 1u) & mask;
    }
  }

  /**
   * @brief Erase an entry by key.
   * @param[in] key The key to erase
   * @return true if the key was found and erased, false otherwise
   * @SINCE_2_5.20
   */
  bool Erase(const Key& key)
  {
    if(mCapacity == 0u)
    {
      return false;
    }

    const uint32_t mask = mCapacity - 1u;
    uint32_t       idx  = HashIndex(key, mask);

    // Always terminates: 75% load-factor guarantee ensures at least one EMPTY slot exists.
    while(true)
    {
      Entry& entry = mEntries[idx];

      if(entry.state == EntryState::EMPTY)
      {
        return false;
      }

      if(entry.state == EntryState::OCCUPIED && mKeyEqual(entry.key, key))
      {
        // Flip the state to TOMBSTONE. The existing key/value objects remain
        // in their current (valid) state and will be properly destroyed when
        // the slot is reused or the table is freed.
        entry.state = EntryState::TOMBSTONE;
        --mSize;
        return true;
      }

      idx = (idx + 1u) & mask;
    }
  }

  /**
   * @brief Mark all entries as empty.
   *
   * Every slot (OCCUPIED or TOMBSTONE) is reset to EMPTY and the size counters
   * are zeroed. The table buffer itself is retained for reuse.
   *
   * Key and Value objects in formerly-occupied slots are left in their current
   * state: they will be overwritten when the slot is reused by a subsequent
   * Insert, or destroyed when the table is freed.
   *
   * If you need resources released immediately rather than lazily, follow this
   * call with Rehash(), which frees the entire table when size is zero:
   * @code
   *   map.Clear();
   *   map.Rehash(); // releases the table buffer and all Key/Value storage immediately
   * @endcode
   * @SINCE_2_5.20
   */
  void Clear()
  {
    if(mEntries)
    {
      for(uint32_t i = 0u; i < mCapacity; ++i)
      {
        mEntries[i].state = EntryState::EMPTY;
      }
    }
    mSize     = 0u;
    mOccupied = 0u;
  }

  /**
   * @brief Call a functor for each live entry.
   * @param[in] functor Called with (const Key& key, Value& value) for each entry
   * @SINCE_2_5.20
   */
  template<typename Functor>
  void ForEach(Functor&& functor)
  {
    for(uint32_t i = 0u; i < mCapacity; ++i)
    {
      Entry& entry = mEntries[i];
      if(entry.state == EntryState::OCCUPIED)
      {
        functor(entry.key, entry.value);
      }
    }
  }

  /**
   * @brief Call a functor for each live entry (const version).
   * @param[in] functor Called with (const Key& key, const Value& value) for each entry
   * @SINCE_2_5.20
   */
  template<typename Functor>
  void ForEach(Functor&& functor) const
  {
    for(uint32_t i = 0u; i < mCapacity; ++i)
    {
      const Entry& entry = mEntries[i];
      if(entry.state == EntryState::OCCUPIED)
      {
        functor(entry.key, entry.value);
      }
    }
  }

  /**
   * @return Number of live entries.
   * @SINCE_2_5.20
   */
  uint32_t Size() const
  {
    return mSize;
  }

  /**
   * @return true if there are no live entries.
   * @SINCE_2_5.20
   */
  bool Empty() const
  {
    return mSize == 0u;
  }

  /**
   * @return Current table capacity (always a power of 2, or 0).
   * @SINCE_2_5.20
   */
  uint32_t Capacity() const
  {
    return mCapacity;
  }

  /**
   * @brief Test whether a key exists in the map.
   *
   * Equivalent to std::unordered_map::contains() (C++20).
   *
   * @param[in] key The key to test
   * @return true if the key is present, false otherwise
   * @SINCE_2_5.20
   */
  bool Contains(const Key& key) const
  {
    return Find(key) != nullptr;
  }

  /**
   * @brief Return the number of entries with the given key (0 or 1).
   *
   * Equivalent to std::unordered_map::count(). Since this is a map (not a
   * multimap), the return value is always 0 or 1.
   *
   * @param[in] key The key to count
   * @return 1 if the key is present, 0 otherwise
   * @SINCE_2_5.20
   */
  uint32_t Count(const Key& key) const
  {
    return Contains(key) ? 1u : 0u;
  }

  /**
   * @brief Return an iterator to the first live entry.
   *
   * Iteration order is unspecified (depends on hash distribution).
   * The iterator is invalidated by any operation that may grow or rehash the table.
   * @SINCE_2_5.20
   */
  Iterator begin()
  {
    return Iterator(mEntries, mCapacity, 0u);
  }

  /**
   * @brief Return a past-the-end iterator.
   * @SINCE_2_5.20
   */
  Iterator end()
  {
    return Iterator(mEntries, mCapacity, mCapacity);
  }

  /**
   * @brief Return a const iterator to the first live entry.
   * @SINCE_2_5.20
   */
  ConstIterator begin() const
  {
    return ConstIterator(mEntries, mCapacity, 0u);
  }

  /**
   * @brief Return a past-the-end const iterator.
   * @SINCE_2_5.20
   */
  ConstIterator end() const
  {
    return ConstIterator(mEntries, mCapacity, mCapacity);
  }

  /**
   * @brief Return a const iterator to the first live entry (explicit const).
   * @SINCE_2_5.20
   */
  ConstIterator cbegin() const
  {
    return ConstIterator(mEntries, mCapacity, 0u);
  }

  /**
   * @brief Return a past-the-end const iterator (explicit const).
   * @SINCE_2_5.20
   */
  ConstIterator cend() const
  {
    return ConstIterator(mEntries, mCapacity, mCapacity);
  }

  /**
   * @brief Resize the table to the smallest power-of-2 capacity that keeps
   *        load under 75%, clearing all tombstones in the process.
   *
   * If the input capacity is bigger than zero, the table will be resized to
   * at least that capacity (rounded up to the next power of 2).
   *
   * If the map is empty and targetCapacity is 0, the table is freed entirely
   * (capacity becomes 0). This releases all Key/Value storage immediately via
   * delete[], which is the recommended way to eagerly release resources after
   * Clear():
   * @code
   *   map.Clear();
   *   map.Rehash(); // frees the table buffer, destructing all Key/Value objects
   * @endcode
   *
   * If the current capacity is already optimal, this is a no-op (unless there
   * are tombstones, in which case an in-place rehash clears them).
   *
   * Provides the strong exception guarantee: if RehashTo throws during the
   * copy, the map is left unchanged.
   *
   * WARNING: Invalidates all pointers, references, and iterators obtained
   * before this call.
   *
   * @param[in] targetCapacity Minimum desired capacity (0 to shrink to fit).
   * @SINCE_2_5.20
   */
  void Rehash(uint32_t targetCapacity = 0u)
  {
    if(mSize == 0u && targetCapacity == 0u)
    {
      delete[] mEntries;
      mEntries  = nullptr;
      mCapacity = 0u;
      mOccupied = 0u;
      return;
    }

    // Find the smallest power-of-2 capacity where mSize / capacity < 75%.
    // That is: capacity > mSize * 4 / 3.
    uint32_t minCapacity = Dali::Max(Dali::NextPowerOfTwo(targetCapacity), INITIAL_CAPACITY);
    while(minCapacity * 3u <= mSize * 4u)
    {
      minCapacity *= 2u;
    }

    if(minCapacity != mCapacity)
    {
      RehashTo(minCapacity);
    }
    else if(mOccupied > mSize)
    {
      // Capacity not changed, but there are tombstones — rehash in-place to clear them.
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

  /**
   * @brief Rehash the table to a new capacity.
   *
   * Always copies (never moves) live entries into the new table so that the old
   * table remains completely untouched until the full copy succeeds and the swap
   * is committed.
   *
   * Trade-off: rehash always performs O(n) copy work rather than O(n) move work for
   * nothrow-movable types. This is acceptable because rehashing is amortised and
   * infrequent, and correctness is not compromised.
   *
   * If the application profile shows rehash to be a measurable hot path and all Key/Value
   * types used are guaranteed nothrow-movable, this policy can be revisited by re-adding
   * a MoveIfNoexcept path — but only after the old table has been fully abandoned (i.e.
   * move everything first with no fallback, accepting the basic guarantee only).
   */
  void RehashTo(uint32_t newCapacity)
  {
    Entry* newEntries = new Entry[newCapacity]();
    DALI_ASSERT_ALWAYS(newEntries && "ManagedOpenHashMap::RehashTo() - Memory allocation failed");

    const uint32_t newMask = newCapacity - 1u;

    // Always copy (never move) live entries into the new table.
    // The old table is left completely untouched until this loop succeeds.
    // If any copy-assignment throws, the old table is still fully intact
    // and the partially-filled new table is discarded in the catch block.
    try
    {
      for(uint32_t i = 0u; i < mCapacity; ++i)
      {
        const Entry& entry = mEntries[i]; // const ref: we do not modify the old table
        if(entry.state == EntryState::OCCUPIED)
        {
          uint32_t idx = HashIndex(entry.key, newMask);
          while(newEntries[idx].state != EntryState::EMPTY)
          {
            idx = (idx + 1u) & newMask;
          }
          newEntries[idx].key   = entry.key;   // copy-assign
          newEntries[idx].value = entry.value; // copy-assign
          newEntries[idx].state = EntryState::OCCUPIED;
        }
      }
    }
    catch(...)
    {
      // A copy threw. The old table is fully intact (we never touched it).
      // Discard the partial new table and re-throw so the caller can react.
      delete[] newEntries;
      throw;
    }

    // Full copy succeeded — now commit the swap.
    delete[] mEntries;
    mEntries  = newEntries;
    mCapacity = newCapacity;
    mOccupied = mSize; // Tombstones are gone after rehash.
  }

  /**
   * @brief Unified insert implementation that forwards key/value perfectly.
   */
  template<typename K, typename V>
  Value* InsertImpl(K&& key, V&& value)
  {
    if(mOccupied * 4u >= mCapacity * 3u) // >= 75% load
    {
      Grow();
    }

    const uint32_t mask           = mCapacity - 1u;
    uint32_t       idx            = HashIndex(key, mask);
    int32_t        firstTombstone = -1;

    // Always terminates: 75% load-factor guarantee ensures at least one EMPTY slot exists.
    while(true)
    {
      Entry& entry = mEntries[idx];

      if(entry.state == EntryState::EMPTY)
      {
        // Slot is empty — key not present. Insert here (or at earlier tombstone).
        if(firstTombstone >= 0)
        {
          idx = static_cast<uint32_t>(firstTombstone);
          // Reusing a tombstone: assign new key/value.
          mEntries[idx].key   = Dali::Forward<K>(key);
          mEntries[idx].value = Dali::Forward<V>(value);
          // Reusing a tombstone doesn't increase mOccupied
        }
        else
        {
          mEntries[idx].key   = Dali::Forward<K>(key);
          mEntries[idx].value = Dali::Forward<V>(value);
          ++mOccupied;
        }
        mEntries[idx].state = EntryState::OCCUPIED;
        ++mSize;
        return &mEntries[idx].value;
      }

      if(entry.state == EntryState::TOMBSTONE)
      {
        if(firstTombstone < 0)
        {
          firstTombstone = static_cast<int32_t>(idx);
        }
      }
      else if(entry.state == EntryState::OCCUPIED && mKeyEqual(entry.key, key))
      {
        // Key already present — update value.
        entry.value = Dali::Forward<V>(value);
        return &entry.value;
      }

      idx = (idx + 1u) & mask;
    }
  }

  /**
   * @brief Unified insert-or-ignore implementation.
   *
   * Like InsertImpl but never overwrites an existing value.
   * Returns {ptr, true} on insertion, {ptr, false} if key was already present.
   */
  template<typename K, typename V>
  InsertResult TryInsertImpl(K&& key, V&& value)
  {
    if(mOccupied * 4u >= mCapacity * 3u) // >= 75% load
    {
      Grow();
    }

    const uint32_t mask           = mCapacity - 1u;
    uint32_t       idx            = HashIndex(key, mask);
    int32_t        firstTombstone = -1;

    // Always terminates: 75% load-factor guarantee ensures at least one EMPTY slot exists.
    while(true)
    {
      Entry& entry = mEntries[idx];

      if(entry.state == EntryState::EMPTY)
      {
        // Key not present — insert here (or at earlier tombstone).
        if(firstTombstone >= 0)
        {
          idx = static_cast<uint32_t>(firstTombstone);
          mEntries[idx].key   = Dali::Forward<K>(key);
          mEntries[idx].value = Dali::Forward<V>(value);
          // Reusing a tombstone doesn't increase mOccupied.
        }
        else
        {
          mEntries[idx].key   = Dali::Forward<K>(key);
          mEntries[idx].value = Dali::Forward<V>(value);
          ++mOccupied;
        }
        mEntries[idx].state = EntryState::OCCUPIED;
        ++mSize;
        return {&mEntries[idx].value, true};
      }

      if(entry.state == EntryState::TOMBSTONE)
      {
        if(firstTombstone < 0)
        {
          firstTombstone = static_cast<int32_t>(idx);
        }
      }
      else if(entry.state == EntryState::OCCUPIED && mKeyEqual(entry.key, key))
      {
        // Key already present — do NOT overwrite. Return existing value + false.
        return {&entry.value, false};
      }

      idx = (idx + 1u) & mask;
    }
  }

  Entry*   mEntries{nullptr};
  uint32_t mCapacity{0};
  uint32_t mSize{0};     ///< Live entries.
  uint32_t mOccupied{0}; ///< Live entries + tombstones (drives load-factor check).

  DALI_NO_UNIQUE_ADDRESS Hash     mHash{};
  DALI_NO_UNIQUE_ADDRESS KeyEqual mKeyEqual{};
};

/**
 * @}
 */

} // namespace Dali

#endif // DALI_PUBLIC_API_COMMON_OPEN_HASH_MAP_MANAGED_H
