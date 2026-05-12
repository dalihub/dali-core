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
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/common/open-hash-map-managed.h>

using namespace Dali;

void utc_dali_internal_managed_open_hash_map_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_managed_open_hash_map_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

// String key hash/equal for std::string keys
struct StdStringHash
{
  size_t operator()(const std::string& k) const noexcept
  {
    return std::hash<std::string>()(k);
  }
};

struct StdStringEqual
{
  bool operator()(const std::string& a, const std::string& b) const noexcept
  {
    return a == b;
  }
};

// Integer key hash/equal for uint32_t keys
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

using StringIntMap = ManagedOpenHashMap<std::string, int, StdStringHash, StdStringEqual>;
using IntIntMap    = ManagedOpenHashMap<uint32_t, int, IntHash, IntEqual>;

} // anonymous namespace

int UtcDaliManagedOpenHashMapDefaultConstruction(void)
{
  tet_infoline("ManagedOpenHashMap default constructor creates empty map");

  StringIntMap map;
  DALI_TEST_EQUALS(map.Size(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Empty(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Capacity(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Find("hello"), static_cast<int*>(nullptr), TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapInsertAndFind(void)
{
  tet_infoline("ManagedOpenHashMap Insert and Find");

  StringIntMap map;
  map.Insert("alpha", 1);
  map.Insert("beta", 2);
  map.Insert("gamma", 3);

  DALI_TEST_EQUALS(map.Size(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Empty(), false, TEST_LOCATION);
  DALI_TEST_CHECK(map.Capacity() >= 3u);

  int* val = map.Find("alpha");
  DALI_TEST_CHECK(val != nullptr);
  DALI_TEST_EQUALS(*val, 1, TEST_LOCATION);

  val = map.Find("beta");
  DALI_TEST_CHECK(val != nullptr);
  DALI_TEST_EQUALS(*val, 2, TEST_LOCATION);

  val = map.Find("gamma");
  DALI_TEST_CHECK(val != nullptr);
  DALI_TEST_EQUALS(*val, 3, TEST_LOCATION);

  // Not found
  val = map.Find("delta");
  DALI_TEST_EQUALS(val, static_cast<int*>(nullptr), TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapFindMissing(void)
{
  tet_infoline("ManagedOpenHashMap Find on empty and missing keys");

  StringIntMap empty;
  DALI_TEST_EQUALS(empty.Find("anything"), static_cast<int*>(nullptr), TEST_LOCATION);

  StringIntMap map;
  map.Insert("one", 1);
  DALI_TEST_EQUALS(map.Find("two"), static_cast<int*>(nullptr), TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapInsertUpdate(void)
{
  tet_infoline("ManagedOpenHashMap Insert updates existing key");

  StringIntMap map;
  map.Insert("key", 10);
  DALI_TEST_EQUALS(*map.Find("key"), 10, TEST_LOCATION);

  map.Insert("key", 20);
  DALI_TEST_EQUALS(*map.Find("key"), 20, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Size(), 1u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapErase(void)
{
  tet_infoline("ManagedOpenHashMap Erase");

  StringIntMap map;
  map.Insert("a", 1);
  map.Insert("b", 2);
  map.Insert("c", 3);

  DALI_TEST_EQUALS(map.Erase("b"), true, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Find("b"), static_cast<int*>(nullptr), TEST_LOCATION);

  // Erase non-existent key
  DALI_TEST_EQUALS(map.Erase("b"), false, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Erase("z"), false, TEST_LOCATION);

  // Other keys still accessible
  DALI_TEST_EQUALS(*map.Find("a"), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(*map.Find("c"), 3, TEST_LOCATION);

  // Erase on empty map
  StringIntMap empty;
  DALI_TEST_EQUALS(empty.Erase("a"), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapEraseAndReinsert(void)
{
  tet_infoline("ManagedOpenHashMap Erase then reinsert");

  StringIntMap map;
  map.Insert("x", 10);
  map.Insert("y", 20);

  map.Erase("x");
  DALI_TEST_EQUALS(map.Find("x"), static_cast<int*>(nullptr), TEST_LOCATION);

  // Reinsert at same key
  map.Insert("x", 30);
  DALI_TEST_EQUALS(*map.Find("x"), 30, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Size(), 2u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapClear(void)
{
  tet_infoline("ManagedOpenHashMap Clear");

  StringIntMap map;
  map.Insert("a", 1);
  map.Insert("b", 2);
  map.Insert("c", 3);

  map.Clear();
  DALI_TEST_EQUALS(map.Size(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Empty(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Find("a"), static_cast<int*>(nullptr), TEST_LOCATION);

  // Can insert after clear
  map.Insert("d", 4);
  DALI_TEST_EQUALS(map.Size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(*map.Find("d"), 4, TEST_LOCATION);

  // Clear on empty map is safe
  StringIntMap empty;
  empty.Clear();
  DALI_TEST_EQUALS(empty.Size(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapGrowth(void)
{
  tet_infoline("ManagedOpenHashMap grows beyond initial capacity");

  StringIntMap map;
  // Insert more than INITIAL_CAPACITY (8) * 0.75 = 6 entries
  for(int i = 0; i < 100; ++i)
  {
    map.Insert("key_" + std::to_string(i), i);
  }

  DALI_TEST_EQUALS(map.Size(), 100u, TEST_LOCATION);

  // Verify all entries are still accessible after growth
  for(int i = 0; i < 100; ++i)
  {
    int* val = map.Find("key_" + std::to_string(i));
    DALI_TEST_CHECK(val != nullptr);
    DALI_TEST_EQUALS(*val, i, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliManagedOpenHashMapLargeScale(void)
{
  tet_infoline("ManagedOpenHashMap large scale insert and find");

  StringIntMap map;
  const int    N = 1000;

  for(int i = 0; i < N; ++i)
  {
    map.Insert("item_" + std::to_string(i), i * 10);
  }

  DALI_TEST_EQUALS(map.Size(), static_cast<uint32_t>(N), TEST_LOCATION);

  for(int i = 0; i < N; ++i)
  {
    int* val = map.Find("item_" + std::to_string(i));
    DALI_TEST_CHECK(val != nullptr);
    DALI_TEST_EQUALS(*val, i * 10, TEST_LOCATION);
  }

  // Verify missing keys
  for(int i = N; i < N + 10; ++i)
  {
    DALI_TEST_EQUALS(map.Find("item_" + std::to_string(i)), static_cast<int*>(nullptr), TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliManagedOpenHashMapForEach(void)
{
  tet_infoline("ManagedOpenHashMap ForEach");

  StringIntMap map;
  map.Insert("one", 1);
  map.Insert("two", 2);
  map.Insert("three", 3);

  std::vector<std::string> keys;
  std::vector<int>         values;
  map.ForEach([&](const std::string& key, int& value)
  {
    keys.push_back(key);
    values.push_back(value);
  });

  DALI_TEST_EQUALS(keys.size(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(values.size(), 3u, TEST_LOCATION);

  // Verify all keys and values are present (order not guaranteed)
  int sum = 0;
  for(auto v : values)
  {
    sum += v;
  }
  DALI_TEST_EQUALS(sum, 6, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapForEachConst(void)
{
  tet_infoline("ManagedOpenHashMap ForEach const");

  StringIntMap map;
  map.Insert("a", 10);
  map.Insert("b", 20);

  const StringIntMap& cmap = map;
  int                 sum  = 0;
  cmap.ForEach([&](const std::string& key, const int& value)
  {
    sum += value;
  });

  DALI_TEST_EQUALS(sum, 30, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapMoveConstruct(void)
{
  tet_infoline("ManagedOpenHashMap move constructor");

  StringIntMap map;
  map.Insert("hello", 42);
  map.Insert("world", 99);

  StringIntMap moved(std::move(map));

  DALI_TEST_EQUALS(moved.Size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(*moved.Find("hello"), 42, TEST_LOCATION);
  DALI_TEST_EQUALS(*moved.Find("world"), 99, TEST_LOCATION);

  // Source is in valid empty state
  DALI_TEST_EQUALS(map.Size(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Capacity(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapMoveAssign(void)
{
  tet_infoline("ManagedOpenHashMap move assignment");

  StringIntMap map1;
  map1.Insert("a", 1);

  StringIntMap map2;
  map2.Insert("b", 2);
  map2.Insert("c", 3);

  map1 = std::move(map2);

  DALI_TEST_EQUALS(map1.Size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(*map1.Find("b"), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(*map1.Find("c"), 3, TEST_LOCATION);

  // Source is in valid empty state
  DALI_TEST_EQUALS(map2.Size(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapInsertReturnValue(void)
{
  tet_infoline("ManagedOpenHashMap Insert returns pointer to value");

  StringIntMap map;
  int*         ptr = map.Insert("key", 100);
  DALI_TEST_CHECK(ptr != nullptr);
  DALI_TEST_EQUALS(*ptr, 100, TEST_LOCATION);

  // Modify through returned pointer
  *ptr = 200;
  DALI_TEST_EQUALS(*map.Find("key"), 200, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapEraseAll(void)
{
  tet_infoline("ManagedOpenHashMap erase all entries");

  StringIntMap map;
  map.Insert("a", 1);
  map.Insert("b", 2);
  map.Insert("c", 3);

  DALI_TEST_EQUALS(map.Erase("a"), true, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Erase("b"), true, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Erase("c"), true, TEST_LOCATION);

  DALI_TEST_EQUALS(map.Size(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Empty(), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapClearThenInsert(void)
{
  tet_infoline("ManagedOpenHashMap Clear then Insert");

  StringIntMap map;
  map.Insert("a", 1);
  map.Insert("b", 2);

  map.Clear();
  DALI_TEST_EQUALS(map.Size(), 0u, TEST_LOCATION);

  map.Insert("c", 3);
  DALI_TEST_EQUALS(map.Size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(*map.Find("c"), 3, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapRehashShrinks(void)
{
  tet_infoline("ManagedOpenHashMap Rehash shrinks table");

  StringIntMap map;
  for(int i = 0; i < 100; ++i)
  {
    map.Insert("key_" + std::to_string(i), i);
  }

  uint32_t bigCapacity = map.Capacity();
  DALI_TEST_CHECK(bigCapacity > 0);

  // Erase most entries
  for(int i = 0; i < 95; ++i)
  {
    map.Erase("key_" + std::to_string(i));
  }

  map.Rehash();
  uint32_t smallCapacity = map.Capacity();
  DALI_TEST_CHECK(smallCapacity < bigCapacity);
  DALI_TEST_EQUALS(map.Size(), 5u, TEST_LOCATION);

  // Remaining entries still accessible
  for(int i = 95; i < 100; ++i)
  {
    DALI_TEST_EQUALS(*map.Find("key_" + std::to_string(i)), i, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliManagedOpenHashMapDaliString(void)
{
  tet_infoline("ManagedOpenHashMap works with Dali::String keys via StringHash and StringEqual");

  using StringIntMap = ManagedOpenHashMap<String, int, StringHash, StringEqual>;

  StringIntMap map;
  map.Insert(String("alpha"), 1);
  map.Insert(String("beta"), 2);
  map.Insert(String("gamma"), 3);

  DALI_TEST_EQUALS(map.Size(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(*map.Find(String("alpha")), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(*map.Find(String("beta")), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(*map.Find(String("gamma")), 3, TEST_LOCATION);

  // Update existing key
  map.Insert(String("beta"), 20);
  DALI_TEST_EQUALS(*map.Find(String("beta")), 20, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Size(), 3u, TEST_LOCATION);

  // Erase and reinsert
  map.Erase(String("alpha"));
  DALI_TEST_EQUALS(map.Find(String("alpha")), static_cast<int*>(nullptr), TEST_LOCATION);
  map.Insert(String("alpha"), 100);
  DALI_TEST_EQUALS(*map.Find(String("alpha")), 100, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapRehashEmpty(void)
{
  tet_infoline("ManagedOpenHashMap Rehash on empty map frees table");

  StringIntMap map;
  map.Insert("a", 1);
  map.Erase("a");

  map.Rehash();
  DALI_TEST_EQUALS(map.Size(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Capacity(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapRehashClearsTombstones(void)
{
  tet_infoline("ManagedOpenHashMap Rehash clears tombstones");

  StringIntMap map;
  for(int i = 0; i < 20; ++i)
  {
    map.Insert("key_" + std::to_string(i), i);
  }

  // Erase half to create tombstones
  for(int i = 0; i < 10; ++i)
  {
    map.Erase("key_" + std::to_string(i));
  }

  // Size is 10 but occupied (including tombstones) is higher
  DALI_TEST_EQUALS(map.Size(), 10u, TEST_LOCATION);

  map.Rehash();

  // After rehash, remaining entries should still be accessible
  for(int i = 10; i < 20; ++i)
  {
    DALI_TEST_EQUALS(*map.Find("key_" + std::to_string(i)), i, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliManagedOpenHashMapRehashNoOpWhenOptimal(void)
{
  tet_infoline("ManagedOpenHashMap Rehash is no-op when already optimal");

  StringIntMap map;
  map.Insert("a", 1);
  map.Insert("b", 2);

  uint32_t capBefore = map.Capacity();
  map.Rehash();
  uint32_t capAfter = map.Capacity();

  // Capacity should not change (no tombstones, already minimal)
  DALI_TEST_EQUALS(capBefore, capAfter, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Size(), 2u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapNonTrivialValue(void)
{
  tet_infoline("ManagedOpenHashMap with non-trivial value type (std::string)");

  ManagedOpenHashMap<std::string, std::string, StdStringHash, StdStringEqual> map;
  map.Insert("greeting", "hello");
  map.Insert("farewell", "goodbye");

  DALI_TEST_EQUALS(*map.Find("greeting"), std::string("hello"), TEST_LOCATION);
  DALI_TEST_EQUALS(*map.Find("farewell"), std::string("goodbye"), TEST_LOCATION);

  // Erase and verify
  map.Erase("greeting");
  DALI_TEST_EQUALS(map.Find("greeting"), static_cast<std::string*>(nullptr), TEST_LOCATION);
  DALI_TEST_EQUALS(*map.Find("farewell"), std::string("goodbye"), TEST_LOCATION);

  // Clear and verify
  map.Clear();
  DALI_TEST_EQUALS(map.Size(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapHeavyChurn(void)
{
  tet_infoline("ManagedOpenHashMap heavy churn (insert/erase cycle)");

  StringIntMap map;
  for(int round = 0; round < 10; ++round)
  {
    for(int i = 0; i < 50; ++i)
    {
      map.Insert("key_" + std::to_string(i), i + round * 100);
    }
    for(int i = 0; i < 50; ++i)
    {
      if(i % 3 == 0)
      {
        map.Erase("key_" + std::to_string(i));
      }
    }
  }

  // Verify remaining entries are consistent
  map.ForEach([](const std::string& key, int& value)
  {
    // Value should be from the last round that inserted it
    DALI_TEST_CHECK(value >= 0);
  });

  END_TEST;
}

int UtcDaliManagedOpenHashMapTombstoneChain(void)
{
  tet_infoline("ManagedOpenHashMap tombstone chain: find past deleted entries");

  StringIntMap map;
  map.Insert("a", 1);
  map.Insert("b", 2);
  map.Insert("c", 3);

  // Erase "a" to create a tombstone
  map.Erase("a");

  // "b" and "c" should still be findable (may need to probe past tombstone)
  DALI_TEST_EQUALS(*map.Find("b"), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(*map.Find("c"), 3, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapUint32Key(void)
{
  tet_infoline("ManagedOpenHashMap with uint32_t keys");

  IntIntMap map;
  map.Insert(10, 100);
  map.Insert(20, 200);
  map.Insert(30, 300);

  DALI_TEST_EQUALS(*map.Find(10), 100, TEST_LOCATION);
  DALI_TEST_EQUALS(*map.Find(20), 200, TEST_LOCATION);
  DALI_TEST_EQUALS(*map.Find(30), 300, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Find(40), static_cast<int*>(nullptr), TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapConstFind(void)
{
  tet_infoline("ManagedOpenHashMap const Find returns const pointer");

  StringIntMap map;
  map.Insert("key", 42);

  const StringIntMap& cmap = map;

  // const Find returns const int*
  const int* val = cmap.Find("key");
  DALI_TEST_CHECK(val != nullptr);
  DALI_TEST_EQUALS(*val, 42, TEST_LOCATION);

  // Not found on const map
  DALI_TEST_EQUALS(cmap.Find("missing"), static_cast<const int*>(nullptr), TEST_LOCATION);

  // Non-const Find returns int*
  int* nval = map.Find("key");
  DALI_TEST_CHECK(nval != nullptr);
  *nval = 99;
  DALI_TEST_EQUALS(*cmap.Find("key"), 99, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapMoveInsert(void)
{
  tet_infoline("ManagedOpenHashMap Insert with move semantics");

  ManagedOpenHashMap<std::string, std::string, StdStringHash, StdStringEqual> map;

  // Move both key and value
  std::string key1 = "move_key";
  std::string val1 = "move_value";
  map.Insert(std::move(key1), std::move(val1));

  DALI_TEST_EQUALS(*map.Find("move_key"), std::string("move_value"), TEST_LOCATION);

  // Move key, copy value
  std::string key2 = "move_key2";
  std::string val2 = "copy_value";
  map.Insert(std::move(key2), val2);
  DALI_TEST_CHECK(!val2.empty()); // val2 should still be intact (copied)

  // Copy key, move value
  std::string key3 = "copy_key";
  std::string val3 = "move_val3";
  map.Insert(key3, std::move(val3));
  DALI_TEST_CHECK(!key3.empty()); // key3 should still be intact (copied)

  DALI_TEST_EQUALS(map.Size(), 3u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapOperatorBracket(void)
{
  tet_infoline("ManagedOpenHashMap operator[] access-or-insert");

  StringIntMap map;

  // Insert new key via operator[]
  map["hello"] = 42;
  DALI_TEST_EQUALS(map.Size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(*map.Find("hello"), 42, TEST_LOCATION);

  // Access existing key via operator[] (does not overwrite)
  map["hello"] = 99;
  DALI_TEST_EQUALS(map.Size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(*map.Find("hello"), 99, TEST_LOCATION);

  // operator[] on missing key inserts default-constructed value (0 for int)
  int& ref = map["new_key"];
  DALI_TEST_EQUALS(map.Size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(ref, 0, TEST_LOCATION);
  ref = 77;
  DALI_TEST_EQUALS(*map.Find("new_key"), 77, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapAt(void)
{
  tet_infoline("ManagedOpenHashMap At() access with assertion on miss");

  StringIntMap map;
  map.Insert("key", 10);

  // Non-const At
  DALI_TEST_EQUALS(map.At("key"), 10, TEST_LOCATION);

  // Modify through At
  map.At("key") = 20;
  DALI_TEST_EQUALS(*map.Find("key"), 20, TEST_LOCATION);

  // Const At
  const StringIntMap& cmap = map;
  DALI_TEST_EQUALS(cmap.At("key"), 20, TEST_LOCATION);

  // At on missing key triggers DALI_ASSERT_ALWAYS — cannot test in unit tests
  // (would abort). This is by design.

  END_TEST;
}

int UtcDaliManagedOpenHashMapTryInsert(void)
{
  tet_infoline("ManagedOpenHashMap TryInsert insert-if-absent");

  StringIntMap map;

  // TryInsert new key — should insert
  auto result1 = map.TryInsert("key", 10);
  DALI_TEST_CHECK(result1.first != nullptr);
  DALI_TEST_EQUALS(*result1.first, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(result1.second, true, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Size(), 1u, TEST_LOCATION);

  // TryInsert same key — should NOT overwrite
  auto result2 = map.TryInsert("key", 99);
  DALI_TEST_CHECK(result2.first != nullptr);
  DALI_TEST_EQUALS(*result2.first, 10, TEST_LOCATION); // value unchanged
  DALI_TEST_EQUALS(result2.second, false, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Size(), 1u, TEST_LOCATION);

  // TryInsert different key — should insert
  auto result3 = map.TryInsert("other", 20);
  DALI_TEST_EQUALS(*result3.first, 20, TEST_LOCATION);
  DALI_TEST_EQUALS(result3.second, true, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Size(), 2u, TEST_LOCATION);

  // Compare with Insert which DOES overwrite
  map.Insert("key", 99);
  DALI_TEST_EQUALS(*map.Find("key"), 99, TEST_LOCATION); // overwritten
  DALI_TEST_EQUALS(map.Size(), 2u, TEST_LOCATION);       // size unchanged

  END_TEST;
}

int UtcDaliManagedOpenHashMapTryInsertMove(void)
{
  tet_infoline("ManagedOpenHashMap TryInsert with move semantics");

  ManagedOpenHashMap<std::string, std::string, StdStringHash, StdStringEqual> map;

  // Move both
  std::string k1 = "move_key", v1 = "move_val";
  auto        r1 = map.TryInsert(std::move(k1), std::move(v1));
  DALI_TEST_EQUALS(r1.second, true, TEST_LOCATION);
  DALI_TEST_EQUALS(*r1.first, std::string("move_val"), TEST_LOCATION);

  // Move key, copy value
  std::string k2 = "mk2", v2 = "cv2";
  auto        r2 = map.TryInsert(std::move(k2), v2);
  DALI_TEST_EQUALS(r2.second, true, TEST_LOCATION);
  DALI_TEST_CHECK(!v2.empty()); // v2 was copied, not moved

  // Copy key, move value
  std::string k3 = "ck3", v3 = "mv3";
  auto        r3 = map.TryInsert(k3, std::move(v3));
  DALI_TEST_EQUALS(r3.second, true, TEST_LOCATION);
  DALI_TEST_CHECK(!k3.empty()); // k3 was copied, not moved

  END_TEST;
}

int UtcDaliManagedOpenHashMapContains(void)
{
  tet_infoline("ManagedOpenHashMap Contains");

  StringIntMap map;
  map.Insert("alpha", 1);
  map.Insert("beta", 2);

  DALI_TEST_EQUALS(map.Contains("alpha"), true, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Contains("beta"), true, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Contains("gamma"), false, TEST_LOCATION);

  // After erase
  map.Erase("alpha");
  DALI_TEST_EQUALS(map.Contains("alpha"), false, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Contains("beta"), true, TEST_LOCATION);

  // Empty map
  StringIntMap empty;
  DALI_TEST_EQUALS(empty.Contains("anything"), false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapCount(void)
{
  tet_infoline("ManagedOpenHashMap Count");

  StringIntMap map;
  map.Insert("key", 1);

  DALI_TEST_EQUALS(map.Count("key"), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(map.Count("missing"), 0u, TEST_LOCATION);

  // After erase
  map.Erase("key");
  DALI_TEST_EQUALS(map.Count("key"), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapIterator(void)
{
  tet_infoline("ManagedOpenHashMap Iterator and range-for");

  StringIntMap map;
  map.Insert("one", 1);
  map.Insert("two", 2);
  map.Insert("three", 3);

  // Range-for iteration using key()/value() accessors
  int                      sum = 0;
  std::vector<std::string> keys;
  for(auto& kv : map)
  {
    keys.push_back(kv.key());
    sum += kv.value();
  }
  DALI_TEST_EQUALS(sum, 6, TEST_LOCATION);
  DALI_TEST_EQUALS(keys.size(), 3u, TEST_LOCATION);

  // Manual iterator with operator* and key()/value()
  int manualSum = 0;
  for(auto it = map.begin(); it != map.end(); ++it)
  {
    manualSum += (*it).value();
  }
  DALI_TEST_EQUALS(manualSum, 6, TEST_LOCATION);

  // operator-> with key()/value()
  int arrowSum = 0;
  for(auto it = map.begin(); it != map.end(); ++it)
  {
    arrowSum += it->value();
  }
  DALI_TEST_EQUALS(arrowSum, 6, TEST_LOCATION);

  // Verify operator* returns stable lvalue reference (InputIterator compliance)
  for(auto it = map.begin(); it != map.end(); ++it)
  {
    auto& ref = *it;
    // ref is a KeyValuePair& — stable lvalue, not a temporary proxy
    (void)ref.key();
    (void)ref.value();
  }

  END_TEST;
}

int UtcDaliManagedOpenHashMapConstIterator(void)
{
  tet_infoline("ManagedOpenHashMap ConstIterator and const range-for");

  StringIntMap map;
  map.Insert("a", 10);
  map.Insert("b", 20);

  const StringIntMap& cmap = map;

  // Const range-for
  int sum = 0;
  for(auto& kv : cmap)
  {
    sum += kv.value();
  }
  DALI_TEST_EQUALS(sum, 30, TEST_LOCATION);

  // cbegin/cend with operator->
  int csum = 0;
  for(auto it = cmap.cbegin(); it != cmap.cend(); ++it)
  {
    csum += it->value();
  }
  DALI_TEST_EQUALS(csum, 30, TEST_LOCATION);

  // Const operator* returns const reference
  for(auto it = cmap.cbegin(); it != cmap.cend(); ++it)
  {
    const auto& ref = *it;
    (void)ref.key();
    (void)ref.value();
  }

  END_TEST;
}

int UtcDaliManagedOpenHashMapIteratorEmpty(void)
{
  tet_infoline("ManagedOpenHashMap Iterator on empty map");

  StringIntMap empty;

  // begin() == end() on empty map
  DALI_TEST_CHECK(empty.begin() == empty.end());

  // Range-for on empty map should not execute
  int count = 0;
  for(auto& kv : empty)
  {
    (void)kv;
    ++count;
  }
  DALI_TEST_EQUALS(count, 0, TEST_LOCATION);

  // Cleared map
  StringIntMap map;
  map.Insert("x", 1);
  map.Clear();

  count = 0;
  for(auto& kv : map)
  {
    (void)kv;
    ++count;
  }
  DALI_TEST_EQUALS(count, 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapOperatorBracketDefaultInsert(void)
{
  tet_infoline("ManagedOpenHashMap operator[] inserts default value for missing key");

  ManagedOpenHashMap<std::string, std::string, StdStringHash, StdStringEqual> map;

  // operator[] on missing key inserts default-constructed std::string (empty)
  std::string& ref = map["new_key"];
  DALI_TEST_EQUALS(ref, std::string(""), TEST_LOCATION);
  DALI_TEST_EQUALS(map.Size(), 1u, TEST_LOCATION);

  // Assign through the reference
  ref = "assigned";
  DALI_TEST_EQUALS(*map.Find("new_key"), std::string("assigned"), TEST_LOCATION);

  // operator[] on existing key returns existing value (does NOT reset to default)
  map["existing"]   = "value";
  std::string& ref2 = map["existing"];
  DALI_TEST_EQUALS(ref2, std::string("value"), TEST_LOCATION);
  DALI_TEST_EQUALS(map.Size(), 2u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapRehashWithTargetCapacity(void)
{
  tet_infoline("ManagedOpenHashMap Rehash with targetCapacity parameter");

  StringIntMap map;

  // Pre-allocate for 100 entries
  map.Rehash(100);
  DALI_TEST_CHECK(map.Capacity() >= 100u);
  DALI_TEST_EQUALS(map.Size(), 0u, TEST_LOCATION);

  // Insert entries — should not trigger grow since capacity is pre-allocated
  uint32_t capBefore = map.Capacity();
  for(int i = 0; i < 50; ++i)
  {
    map.Insert("key_" + std::to_string(i), i);
  }
  DALI_TEST_EQUALS(map.Capacity(), capBefore, TEST_LOCATION); // no grow needed
  DALI_TEST_EQUALS(map.Size(), 50u, TEST_LOCATION);

  // Rehash(0) shrinks to the minimum capacity that keeps load under 75%.
  // With 50 entries, the minimum power-of-2 capacity is 128 (since 64*3=192 <= 50*4=200,
  // but 128*3=384 > 200). If capBefore was already 128, no shrink occurs.
  // Erase most entries so the table is clearly over-capacity, then shrink.
  for(int i = 0; i < 45; ++i)
  {
    map.Erase("key_" + std::to_string(i));
  }
  DALI_TEST_EQUALS(map.Size(), 5u, TEST_LOCATION);

  map.Rehash(0);
  DALI_TEST_CHECK(map.Capacity() < capBefore);
  DALI_TEST_EQUALS(map.Size(), 5u, TEST_LOCATION);

  // Verify remaining entries still accessible after shrink
  for(int i = 45; i < 50; ++i)
  {
    DALI_TEST_EQUALS(*map.Find("key_" + std::to_string(i)), i, TEST_LOCATION);
  }

  // Empty map with targetCapacity > 0 allocates table
  StringIntMap emptyMap;
  emptyMap.Rehash(50);
  DALI_TEST_CHECK(emptyMap.Capacity() >= 50u);
  DALI_TEST_EQUALS(emptyMap.Size(), 0u, TEST_LOCATION);

  // Empty map with targetCapacity == 0 frees table
  emptyMap.Rehash(0);
  DALI_TEST_EQUALS(emptyMap.Capacity(), 0u, TEST_LOCATION);

  // Rehash with targetCapacity that forces a grow on a non-empty table
  StringIntMap growMap;
  growMap.Insert("a", 1);
  growMap.Insert("b", 2);
  uint32_t smallCap = growMap.Capacity();

  growMap.Rehash(200);
  DALI_TEST_CHECK(growMap.Capacity() >= 200u);
  DALI_TEST_CHECK(growMap.Capacity() > smallCap);
  DALI_TEST_EQUALS(growMap.Size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(*growMap.Find("a"), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(*growMap.Find("b"), 2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliManagedOpenHashMapExceptionSafetyRehash(void)
{
  tet_infoline("ManagedOpenHashMap Rehash uses move_if_noexcept for exception safety");

  // This test verifies that rehash works correctly with types that have
  // noexcept move-assignment (like std::string). For types with throwing
  // move-assignment, move_if_noexcept would fall back to copy.
  StringIntMap map;
  for(int i = 0; i < 20; ++i)
  {
    map.Insert("key_" + std::to_string(i), i);
  }

  // Force a rehash by erasing most entries and calling Rehash()
  for(int i = 0; i < 15; ++i)
  {
    map.Erase("key_" + std::to_string(i));
  }

  map.Rehash();

  // Verify remaining entries are intact after rehash
  for(int i = 15; i < 20; ++i)
  {
    DALI_TEST_EQUALS(*map.Find("key_" + std::to_string(i)), i, TEST_LOCATION);
  }

  END_TEST;
}
