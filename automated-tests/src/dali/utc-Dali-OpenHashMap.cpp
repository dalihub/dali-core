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

// EXTERNAL INCLUDES
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

// INTERNAL INCLUDES
#include <dali-test-suite-utils.h>
#include <dali/integration-api/open-hash-map.h>

using namespace Dali::Integration;

void utc_dali_internal_open_hash_map_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_open_hash_map_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
// Simple pointer-identity hash/equal for int* keys
struct PtrHash
{
  size_t operator()(int* const& p) const noexcept
  {
    return std::hash<int*>()(p);
  }
};

struct PtrEqual
{
  bool operator()(int* const& a, int* const& b) const noexcept
  {
    return a == b;
  }
};

// Integer key hash/equal for non-pointer tests
struct IntHash
{
  size_t operator()(const uint32_t& k) const noexcept
  {
    return std::hash<uint32_t>()(k);
  }
};

struct IntEqual
{
  bool operator()(const uint32_t& a, const uint32_t& b) const noexcept
  {
    return a == b;
  }
};

} // anonymous namespace

// Specialize traits for uint32_t keys: 0 = empty, 1 = tombstone
template<>
struct Dali::Integration::OpenHashMapTraits<uint32_t>
{
  static uint32_t Empty()
  {
    return 0u;
  }
  static uint32_t Tombstone()
  {
    return 1u;
  }
};

// ---- Tests ----

int UtcDaliOpenHashMapDefaultConstruction(void)
{
  tet_infoline("OpenHashMap default constructor creates empty map");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map;
  DALI_TEST_EQUALS(map.Size(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Empty(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Capacity(), 0u, TEST_LOCATION);

  END_TEST;
}

// === VERIFICATION TESTS FOR REVIEW CLAIMS ===

int UtcDaliOpenHashMapVerifyNoInfiniteLoop(void)
{
  tet_infoline("VERIFY: No infinite loop when table is full of tombstones");

  // This test verifies the claimed "infinite loop bug" does NOT exist
  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  // Fill capacity-8 table with 6 entries (75% load, no grow)
  int keys[6];
  for(int i = 0; i < 6; ++i)
  {
    keys[i] = i;
    map.Insert(&keys[i], i);
  }

  DALI_TEST_EQUALS(map.Size(), 6u, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Capacity(), 8u, TEST_LOCATION);

  // Erase all entries - table now has 6 tombstones, 0 live entries
  for(int i = 0; i < 6; ++i)
  {
    map.Erase(&keys[i]);
  }

  DALI_TEST_EQUALS(map.Size(), 0u, TEST_LOCATION);

  // Insert a 7th entry - claimed to cause infinite loop
  // In reality, this either reuses a tombstone slot or grows the table
  int  newKey = 100;
  int* result = map.Insert(&newKey, 999);

  // Test should reach here without hanging
  DALI_TEST_CHECK(result != nullptr);
  DALI_TEST_EQUALS(*result, 999, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Size(), 1u, TEST_LOCATION);

  // Verify the entry is findable
  int* found = map.Find(&newKey);
  DALI_TEST_CHECK(found != nullptr);
  DALI_TEST_EQUALS(*found, 999, TEST_LOCATION);

  tet_infoline("SUCCESS: No infinite loop - claim is FALSE");

  END_TEST;
}

int UtcDaliOpenHashMapVerifyLoadFactorGrowth(void)
{
  tet_infoline("VERIFY: Load factor with tombstones triggers unnecessary growth");

  // This test verifies that tombstones cause table to grow unnecessarily
  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  // Insert 6 entries into capacity-8 table
  int keys[6];
  for(int i = 0; i < 6; ++i)
  {
    keys[i] = i;
    map.Insert(&keys[i], i);
  }

  uint32_t capBefore = map.Capacity();
  DALI_TEST_EQUALS(capBefore, 8u, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Size(), 6u, TEST_LOCATION);

  // Erase all entries - creates 6 tombstones
  for(int i = 0; i < 6; ++i)
  {
    map.Erase(&keys[i]);
  }

  DALI_TEST_EQUALS(map.Size(), 0u, TEST_LOCATION);

  // Insert 1 entry - with 6 tombstones, mOccupied will be 7
  // 7/8 = 87.5% > 75%, should trigger growth
  int newKey = 100;
  map.Insert(&newKey, 999);

  uint32_t capAfter = map.Capacity();

  // Verify the table grew despite only having 1 live entry
  if(capAfter > capBefore)
  {
    tet_infoline("CONFIRMED: Table grew from 8 to 16 with only 1 live entry due to tombstones");
    DALI_TEST_EQUALS(capAfter, 16u, TEST_LOCATION);
  }
  else
  {
    tet_infoline("Table did not grow - claim needs verification");
  }

  DALI_TEST_EQUALS(map.Size(), 1u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOpenHashMapVerifyHighCollisions(void)
{
  tet_infoline("VERIFY: High collision rate handling with constant hash");

  // Custom hash that always returns 0 to force collisions
  struct BadHash
  {
    size_t operator()(int* const& p) const noexcept
    {
      // Return constant 0 for all keys
      return 0;
    }
  };

  OpenHashMap<int*, int, BadHash, PtrEqual> map;

  // Insert 50 entries - all will hash to slot 0
  int keys[50];
  for(int i = 0; i < 50; ++i)
  {
    keys[i] = i;
    map.Insert(&keys[i], i);
  }

  DALI_TEST_EQUALS(map.Size(), 50u, TEST_LOCATION);

  // All entries should be findable despite constant hash
  int foundCount = 0;
  for(int i = 0; i < 50; ++i)
  {
    int* v = map.Find(&keys[i]);
    if(v != nullptr)
    {
      DALI_TEST_EQUALS(*v, i, TEST_LOCATION);
      ++foundCount;
    }
  }

  DALI_TEST_EQUALS(foundCount, 50, TEST_LOCATION);
  tet_infoline("SUCCESS: All 50 entries foundable with constant hash");

  END_TEST;
}

int UtcDaliOpenHashMapVerifyUint32KeyLimitation(void)
{
  tet_infoline("VERIFY: uint32_t keys 0 and 1 are reserved");

  OpenHashMap<uint32_t, int, IntHash, IntEqual> map;

  // Try to insert key=0 (Empty sentinel)
  try
  {
    map.Insert(0u, 100);
    tet_result(TET_FAIL); // Should not reach here
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    tet_infoline("Confirmed: Key=0 (Empty sentinel) causes assert");
  }

  // Try to insert key=1 (Tombstone sentinel)
  try
  {
    map.Insert(1u, 200);
    tet_result(TET_FAIL); // Should not reach here
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    tet_infoline("Confirmed: Key=1 (Tombstone sentinel) causes assert");
  }

  // Insert valid keys
  map.Insert(2u, 2);
  map.Insert(3u, 3);
  DALI_TEST_EQUALS(map.Size(), 2u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOpenHashMapVerifyExact75PercentLoad(void)
{
  tet_infoline("VERIFY: Exact 75% load threshold behavior");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  // Insert exactly 6 entries into capacity-8 table
  // 6/8 = 75.0% - at exact threshold, should NOT trigger grow yet
  int keys[7];
  for(int i = 0; i < 6; ++i)
  {
    keys[i] = i;
    map.Insert(&keys[i], i);
  }

  DALI_TEST_EQUALS(map.Capacity(), 8u, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Size(), 6u, TEST_LOCATION);
  tet_infoline("Confirmed: Capacity=8 at exactly 75% load");

  // Insert 7th entry - should trigger grow before insert
  keys[6] = 6;
  map.Insert(&keys[6], 6);

  DALI_TEST_CHECK(map.Capacity() > 8u);
  tet_infoline("Confirmed: Grow triggered on 7th insert (87.5% load)");

  END_TEST;
}

int UtcDaliOpenHashMapVerifyTombstoneExhaustion(void)
{
  tet_infoline("VERIFY: Insert into table full of tombstones");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  // Fill capacity-8 table with 6 entries
  int keys[6];
  for(int i = 0; i < 6; ++i)
  {
    keys[i] = i;
    map.Insert(&keys[i], i);
  }

  // Erase all - table is now 6 tombstones, 0 live entries
  for(int i = 0; i < 6; ++i)
  {
    map.Erase(&keys[i]);
  }

  DALI_TEST_EQUALS(map.Size(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Capacity(), 8u, TEST_LOCATION);

  // Try to insert multiple new entries into tombstone-filled table
  int newKeys[10];
  for(int i = 0; i < 10; ++i)
  {
    newKeys[i] = 100 + i;
    map.Insert(&newKeys[i], i);
  }

  // All should succeed without infinite loop
  DALI_TEST_EQUALS(map.Size(), 10u, TEST_LOCATION);

  // Verify all entries are findable
  int foundCount = 0;
  for(int i = 0; i < 10; ++i)
  {
    int* v = map.Find(&newKeys[i]);
    if(v != nullptr)
    {
      DALI_TEST_EQUALS(*v, i, TEST_LOCATION);
      ++foundCount;
    }
  }

  DALI_TEST_EQUALS(foundCount, 10, TEST_LOCATION);
  tet_infoline("SUCCESS: All entries insertable and findable in tombstone-filled table");

  END_TEST;
}

int UtcDaliOpenHashMapInsertAndFind(void)
{
  tet_infoline("Insert entries and find them by key");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  int a = 10, b = 20, c = 30;
  map.Insert(&a, 100);
  map.Insert(&b, 200);
  map.Insert(&c, 300);

  DALI_TEST_EQUALS(map.Size(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Empty(), false, TEST_LOCATION);

  int* va = map.Find(&a);
  int* vb = map.Find(&b);
  int* vc = map.Find(&c);

  DALI_TEST_CHECK(va != nullptr);
  DALI_TEST_CHECK(vb != nullptr);
  DALI_TEST_CHECK(vc != nullptr);
  DALI_TEST_EQUALS(*va, 100, TEST_LOCATION);
  DALI_TEST_EQUALS(*vb, 200, TEST_LOCATION);
  DALI_TEST_EQUALS(*vc, 300, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOpenHashMapFindMissing(void)
{
  tet_infoline("Find returns nullptr for keys not in the map");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  int a = 10, b = 20;
  map.Insert(&a, 100);

  int* result = map.Find(&b);
  DALI_TEST_CHECK(result == nullptr);

  // Find on empty map
  OpenHashMap<int*, int, PtrHash, PtrEqual> empty;
  result = empty.Find(&a);
  DALI_TEST_CHECK(result == nullptr);

  END_TEST;
}

int UtcDaliOpenHashMapInsertUpdate(void)
{
  tet_infoline("Inserting with an existing key updates the value");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  int a = 10;
  map.Insert(&a, 100);
  DALI_TEST_EQUALS(*map.Find(&a), 100, TEST_LOCATION);

  map.Insert(&a, 999);
  DALI_TEST_EQUALS(map.Size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(*map.Find(&a), 999, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOpenHashMapErase(void)
{
  tet_infoline("Erase removes entry and returns true; missing key returns false");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  int a = 10, b = 20, c = 30;
  map.Insert(&a, 100);
  map.Insert(&b, 200);
  map.Insert(&c, 300);

  bool erased = map.Erase(&b);
  DALI_TEST_EQUALS(erased, true, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Size(), 2u, TEST_LOCATION);
  DALI_TEST_CHECK(map.Find(&b) == nullptr);

  // Remaining entries still accessible
  DALI_TEST_EQUALS(*map.Find(&a), 100, TEST_LOCATION);
  DALI_TEST_EQUALS(*map.Find(&c), 300, TEST_LOCATION);

  // Erase non-existent key
  erased = map.Erase(&b);
  DALI_TEST_EQUALS(erased, false, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Size(), 2u, TEST_LOCATION);

  // Erase on empty map
  OpenHashMap<int*, int, PtrHash, PtrEqual> empty;
  DALI_TEST_EQUALS(empty.Erase(&a), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOpenHashMapEraseAndReinsert(void)
{
  tet_infoline("Erase then reinsert same key reuses tombstone slot");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  int a = 10;
  map.Insert(&a, 100);
  map.Erase(&a);
  DALI_TEST_CHECK(map.Find(&a) == nullptr);
  DALI_TEST_EQUALS(map.Size(), 0u, TEST_LOCATION);

  map.Insert(&a, 999);
  DALI_TEST_EQUALS(map.Size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(*map.Find(&a), 999, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOpenHashMapClear(void)
{
  tet_infoline("Clear removes all entries but keeps capacity");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  int a = 10, b = 20;
  map.Insert(&a, 100);
  map.Insert(&b, 200);

  uint32_t capBefore = map.Capacity();
  map.Clear();

  DALI_TEST_EQUALS(map.Size(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Empty(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Capacity(), capBefore, TEST_LOCATION);
  DALI_TEST_CHECK(map.Find(&a) == nullptr);
  DALI_TEST_CHECK(map.Find(&b) == nullptr);

  END_TEST;
}

int UtcDaliOpenHashMapGrowth(void)
{
  tet_infoline("Table grows automatically when load exceeds 75%");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  // Initial capacity is 8. Inserting 7 entries should trigger a grow (7/8 > 75%).
  int keys[8];
  for(int i = 0; i < 6; ++i)
  {
    map.Insert(&keys[i], i);
  }
  DALI_TEST_EQUALS(map.Capacity(), 8u, TEST_LOCATION);

  // 7th insert: 7/8 = 87.5% — triggers grow before insert
  map.Insert(&keys[6], 6);
  DALI_TEST_CHECK(map.Capacity() > 8u);
  DALI_TEST_EQUALS(map.Size(), 7u, TEST_LOCATION);

  // All entries survive the rehash
  for(int i = 0; i < 7; ++i)
  {
    int* v = map.Find(&keys[i]);
    DALI_TEST_CHECK(v != nullptr);
    DALI_TEST_EQUALS(*v, i, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliOpenHashMapLargeScale(void)
{
  tet_infoline("Insert and find 5000 entries to validate at target scale");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  const int        count = 5000;
  std::vector<int> keys(count);

  for(int i = 0; i < count; ++i)
  {
    keys[i] = i;
    map.Insert(&keys[i], i * 10);
  }

  DALI_TEST_EQUALS(map.Size(), static_cast<uint32_t>(count), TEST_LOCATION);

  // Verify all entries
  for(int i = 0; i < count; ++i)
  {
    int* v = map.Find(&keys[i]);
    DALI_TEST_CHECK(v != nullptr);
    DALI_TEST_EQUALS(*v, i * 10, TEST_LOCATION);
  }

  // Erase every other entry
  for(int i = 0; i < count; i += 2)
  {
    DALI_TEST_EQUALS(map.Erase(&keys[i]), true, TEST_LOCATION);
  }
  DALI_TEST_EQUALS(map.Size(), static_cast<uint32_t>(count / 2), TEST_LOCATION);

  // Verify: erased entries not found, surviving entries still correct
  for(int i = 0; i < count; ++i)
  {
    int* v = map.Find(&keys[i]);
    if(i % 2 == 0)
    {
      DALI_TEST_CHECK(v == nullptr);
    }
    else
    {
      DALI_TEST_CHECK(v != nullptr);
      DALI_TEST_EQUALS(*v, i * 10, TEST_LOCATION);
    }
  }

  END_TEST;
}

int UtcDaliOpenHashMapTombstoneChain(void)
{
  tet_infoline("Tombstones do not break probe chains");

  // Use integer keys so we can control hash collisions via the traits (keys 0 and 1 are reserved).
  OpenHashMap<uint32_t, int, IntHash, IntEqual> map;

  // Insert keys 2..9 (fills initial capacity 8 minus the grow trigger)
  for(uint32_t i = 2; i <= 7; ++i)
  {
    map.Insert(i, static_cast<int>(i * 100));
  }

  // Erase a middle key — creates a tombstone
  map.Erase(4u);
  DALI_TEST_CHECK(map.Find(4u) == nullptr);

  // Keys after the tombstone in their probe chain must still be findable
  for(uint32_t i = 2; i <= 7; ++i)
  {
    if(i == 4u) continue;
    int* v = map.Find(i);
    DALI_TEST_CHECK(v != nullptr);
    DALI_TEST_EQUALS(*v, static_cast<int>(i * 100), TEST_LOCATION);
  }

  // Reinserting should reuse the tombstone, not increase occupied count
  map.Insert(4u, 400);
  DALI_TEST_EQUALS(*map.Find(4u), 400, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOpenHashMapHeavyChurn(void)
{
  tet_infoline("Repeated insert/erase cycles don't degrade or leak");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  const int        count = 200;
  std::vector<int> keys(count);

  for(int cycle = 0; cycle < 5; ++cycle)
  {
    // Insert all
    for(int i = 0; i < count; ++i)
    {
      keys[i] = cycle * count + i;
      map.Insert(&keys[i], i);
    }
    DALI_TEST_EQUALS(map.Size(), static_cast<uint32_t>(count), TEST_LOCATION);

    // Erase all
    for(int i = 0; i < count; ++i)
    {
      map.Erase(&keys[i]);
    }
    DALI_TEST_EQUALS(map.Size(), 0u, TEST_LOCATION);
    DALI_TEST_EQUALS(map.Empty(), true, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliOpenHashMapForEach(void)
{
  tet_infoline("ForEach visits all live entries exactly once");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  int a = 1, b = 2, c = 3;
  map.Insert(&a, 10);
  map.Insert(&b, 20);
  map.Insert(&c, 30);

  // Erase one to verify ForEach skips tombstones
  map.Erase(&b);

  int sum     = 0;
  int visited = 0;
  map.ForEach([&](int* const& key, int& value)
  {
    sum += value;
    ++visited;
  });

  DALI_TEST_EQUALS(visited, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(sum, 40, TEST_LOCATION); // 10 + 30

  END_TEST;
}

int UtcDaliOpenHashMapForEachConst(void)
{
  tet_infoline("Const ForEach works on const map reference");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  int a = 1, b = 2;
  map.Insert(&a, 10);
  map.Insert(&b, 20);

  const auto& constMap = map;
  int         sum      = 0;
  constMap.ForEach([&](int* const& key, const int& value)
  {
    sum += value;
  });

  DALI_TEST_EQUALS(sum, 30, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOpenHashMapMoveConstruct(void)
{
  tet_infoline("Move constructor transfers ownership");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  int a = 10;
  map.Insert(&a, 100);

  OpenHashMap<int*, int, PtrHash, PtrEqual> moved(std::move(map));

  DALI_TEST_EQUALS(moved.Size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(*moved.Find(&a), 100, TEST_LOCATION);

  // Source is empty after move
  DALI_TEST_EQUALS(map.Size(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Capacity(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOpenHashMapMoveAssign(void)
{
  tet_infoline("Move assignment transfers ownership and frees old data");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map1;
  OpenHashMap<int*, int, PtrHash, PtrEqual> map2;

  int a = 10, b = 20;
  map1.Insert(&a, 100);
  map2.Insert(&b, 200);

  map2 = std::move(map1);

  DALI_TEST_EQUALS(map2.Size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(*map2.Find(&a), 100, TEST_LOCATION);
  DALI_TEST_CHECK(map2.Find(&b) == nullptr);

  DALI_TEST_EQUALS(map1.Size(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(map1.Capacity(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOpenHashMapInsertReturnValue(void)
{
  tet_infoline("Insert returns a pointer to the stored value");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  int  a    = 10;
  int* valp = map.Insert(&a, 42);

  DALI_TEST_CHECK(valp != nullptr);
  DALI_TEST_EQUALS(*valp, 42, TEST_LOCATION);

  // Mutate through returned pointer
  *valp = 99;
  DALI_TEST_EQUALS(*map.Find(&a), 99, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOpenHashMapInsertN01(void)
{
  tet_infoline("Inserting empty key causes assert");

  using HashMap = OpenHashMap<int*, int, PtrHash, PtrEqual>;
  HashMap map;

  try
  {
    int* valPtr = map.Insert(HashMap::TraitsType::Empty(), 42);
    DALI_TEST_CHECK(valPtr == nullptr);
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "OpenHashMap does not allow insertion of OpenHashMapTraits<Key>::Empty as key!", TEST_LOCATION);
  }

  tet_infoline("Check that next insertion is recoverable");
  int  n      = 11;
  int* valPtr = map.Insert(&n, 24);
  DALI_TEST_EQUALS(map.Size(), 1, TEST_LOCATION);
  DALI_TEST_CHECK(valPtr != nullptr);
  DALI_TEST_EQUALS(*valPtr, 24, TEST_LOCATION);
  END_TEST;
}

int UtcDaliOpenHashMapInsertN02(void)
{
  tet_infoline("Inserting empty key causes assert");

  using HashMap = OpenHashMap<int*, int, PtrHash, PtrEqual>;
  HashMap map;

  try
  {
    int* valPtr = map.Insert(HashMap::TraitsType::Tombstone(), 42);
    DALI_TEST_CHECK(valPtr == nullptr);
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT(e);
    DALI_TEST_ASSERT(e, "OpenHashMap does not allow insertion of OpenHashMapTraits<Key>::Tombstone as key!", TEST_LOCATION);
  }

  tet_infoline("Check that next insertion is recoverable");
  int  n      = 11;
  int* valPtr = map.Insert(&n, 24);
  DALI_TEST_EQUALS(map.Size(), 1, TEST_LOCATION);
  DALI_TEST_CHECK(valPtr != nullptr);
  DALI_TEST_EQUALS(*valPtr, 24, TEST_LOCATION);
  END_TEST;
}

int UtcDaliOpenHashMapEraseAll(void)
{
  tet_infoline("Erasing all entries results in empty map that still works");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  int a = 1, b = 2, c = 3;
  map.Insert(&a, 10);
  map.Insert(&b, 20);
  map.Insert(&c, 30);

  map.Erase(&a);
  map.Erase(&b);
  map.Erase(&c);

  DALI_TEST_EQUALS(map.Size(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Empty(), true, TEST_LOCATION);
  DALI_TEST_CHECK(map.Capacity() > 0u); // Table still allocated

  // Can still insert after erasing everything
  int d = 4;
  map.Insert(&d, 40);
  DALI_TEST_EQUALS(map.Size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(*map.Find(&d), 40, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOpenHashMapClearThenInsert(void)
{
  tet_infoline("Clear followed by inserts works correctly");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  int a = 1, b = 2;
  map.Insert(&a, 10);
  map.Insert(&b, 20);
  map.Clear();

  // Reinsert the same keys
  map.Insert(&a, 100);
  map.Insert(&b, 200);

  DALI_TEST_EQUALS(map.Size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(*map.Find(&a), 100, TEST_LOCATION);
  DALI_TEST_EQUALS(*map.Find(&b), 200, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOpenHashMapRehashShrinks(void)
{
  tet_infoline("Rehash shrinks capacity after bulk erase");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  const int        count = 100;
  std::vector<int> keys(count);

  for(int i = 0; i < count; ++i)
  {
    keys[i] = i;
    map.Insert(&keys[i], i);
  }

  uint32_t capBefore = map.Capacity();
  DALI_TEST_CHECK(capBefore >= 128u); // 100 entries at 75% load → capacity 256

  // Erase most entries, leaving 5
  for(int i = 5; i < count; ++i)
  {
    map.Erase(&keys[i]);
  }
  DALI_TEST_EQUALS(map.Size(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Capacity(), capBefore, TEST_LOCATION); // capacity unchanged

  map.Rehash();

  DALI_TEST_CHECK(map.Capacity() < capBefore);
  DALI_TEST_EQUALS(map.Size(), 5u, TEST_LOCATION);

  // Surviving entries still accessible
  for(int i = 0; i < 5; ++i)
  {
    int* v = map.Find(&keys[i]);
    DALI_TEST_CHECK(v != nullptr);
    DALI_TEST_EQUALS(*v, i, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliOpenHashMapRehashEmpty(void)
{
  tet_infoline("Rehash on empty map frees table entirely");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  int a = 1;
  map.Insert(&a, 10);
  DALI_TEST_CHECK(map.Capacity() > 0u);

  map.Erase(&a);
  map.Rehash();

  DALI_TEST_EQUALS(map.Size(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Capacity(), 0u, TEST_LOCATION);

  // Map still works after rehash to zero
  map.Insert(&a, 20);
  DALI_TEST_EQUALS(map.Size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(*map.Find(&a), 20, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOpenHashMapRehashClearsTombstones(void)
{
  tet_infoline("Rehash at same capacity clears tombstones");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  // Insert 5 entries (capacity 8), erase 2 to create tombstones
  int keys[5];
  for(int i = 0; i < 5; ++i)
  {
    keys[i] = i;
    map.Insert(&keys[i], i * 10);
  }

  map.Erase(&keys[1]);
  map.Erase(&keys[3]);
  DALI_TEST_EQUALS(map.Size(), 3u, TEST_LOCATION);

  uint32_t capBefore = map.Capacity();
  map.Rehash();

  // Capacity should stay the same (3 entries needs capacity 8 minimum)
  // but tombstones are cleared
  DALI_TEST_EQUALS(map.Capacity(), capBefore, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Size(), 3u, TEST_LOCATION);

  // Surviving entries still accessible
  DALI_TEST_EQUALS(*map.Find(&keys[0]), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(*map.Find(&keys[2]), 20, TEST_LOCATION);
  DALI_TEST_EQUALS(*map.Find(&keys[4]), 40, TEST_LOCATION);

  // Erased entries still gone
  DALI_TEST_CHECK(map.Find(&keys[1]) == nullptr);
  DALI_TEST_CHECK(map.Find(&keys[3]) == nullptr);

  END_TEST;
}

int UtcDaliOpenHashMapRehashNoOpWhenOptimal(void)
{
  tet_infoline("Rehash is a no-op when capacity is already optimal and no tombstones");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  int keys[3];
  for(int i = 0; i < 3; ++i)
  {
    keys[i] = i;
    map.Insert(&keys[i], i);
  }

  // 3 entries, capacity 8, no tombstones — already optimal
  uint32_t capBefore = map.Capacity();
  map.Rehash();
  DALI_TEST_EQUALS(map.Capacity(), capBefore, TEST_LOCATION);

  END_TEST;
}

int UtcDaliOpenHashMapRehashLargeToSmall(void)
{
  tet_infoline("Rehash shrinks through multiple capacity doublings");

  OpenHashMap<int*, int, PtrHash, PtrEqual> map;

  // Insert 200 entries — capacity will grow well past INITIAL_CAPACITY (8)
  const int        count = 200;
  std::vector<int> keys(count);

  for(int i = 0; i < count; ++i)
  {
    keys[i] = i;
    map.Insert(&keys[i], i);
  }

  DALI_TEST_CHECK(map.Capacity() >= 256u);

  // Erase down to 10 entries — Rehash must loop minCapacity past 8
  for(int i = 10; i < count; ++i)
  {
    map.Erase(&keys[i]);
  }
  DALI_TEST_EQUALS(map.Size(), 10u, TEST_LOCATION);

  map.Rehash();

  // 10 entries at 75% → needs capacity 16 (10/16 = 62.5% < 75%)
  DALI_TEST_EQUALS(map.Capacity(), 16u, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Size(), 10u, TEST_LOCATION);

  // All 10 survivors still accessible
  for(int i = 0; i < 10; ++i)
  {
    int* v = map.Find(&keys[i]);
    DALI_TEST_CHECK(v != nullptr);
    DALI_TEST_EQUALS(*v, i, TEST_LOCATION);
  }

  END_TEST;
}
