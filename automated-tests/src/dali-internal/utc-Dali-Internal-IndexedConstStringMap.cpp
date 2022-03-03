/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

#include <dali-test-suite-utils.h>
#include <dali/public-api/dali-core.h>

#include <random>
#include <string>

// Internal headers are allowed here
#include <dali/internal/common/indexed-const-string-map.h>

using namespace Dali;
using Dali::Internal::ConstString;
using Dali::Internal::IndexedConstStringMap;
namespace
{
std::string RandomString(size_t length)
{
  static auto& chrs =
    "0123456789"
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  thread_local static std::mt19937                                          rg{std::random_device{}()};
  thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(chrs) - 2);

  std::string s;

  s.reserve(length);

  while(length--)
    s += chrs[pick(rg)];

  return s;
}

// Custom << operator to print debugging log.
std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const ConstString& constString)
{
  std::string convertedString = std::string(constString.GetStringView());
  os << convertedString;
  return os;
}

} // namespace

void utc_dali_internal_indexed_conststring_map_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_indexed_conststring_map_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliIndexedConstStringMapEmpty(void)
{
  IndexedConstStringMap<int> indexedMap;
  DALI_TEST_EQUALS(indexedMap.Empty(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.empty(), true, TEST_LOCATION);

  ConstString str("hello");
  int         value = 3;
  DALI_TEST_EQUALS(indexedMap.Register(str, value), true, TEST_LOCATION);

  DALI_TEST_EQUALS(indexedMap.Empty(), false, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.empty(), false, TEST_LOCATION);

  DALI_TEST_EQUALS(indexedMap.Count(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.size(), 1, TEST_LOCATION);

  DALI_TEST_EQUALS(indexedMap.Get(str)->first, str, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.Get(str)->second, value, TEST_LOCATION);

  indexedMap.Clear();
  DALI_TEST_EQUALS(indexedMap.Empty(), true, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.empty(), true, TEST_LOCATION);

  DALI_TEST_CHECK(indexedMap.Get(str) == indexedMap.End());

  END_TEST;
}

int UtcDaliIndexedConstStringMap(void)
{
  IndexedConstStringMap<int> indexedMap;

  auto IndexedMapGetValueTest = [](const IndexedConstStringMap<int>& indexedMap, ConstString key, bool registered, int element, const char* location) {
    const auto& iter = indexedMap.Get(key);
    DALI_TEST_EQUALS(!(iter == indexedMap.end()), registered, location);
    if(registered)
    {
      DALI_TEST_EQUALS(iter->second, element, location);
    }
  };

  ConstString keyFirst  = ConstString("first");
  ConstString keySecond = ConstString("second");
  ConstString keyThird  = ConstString("third");
  ConstString keyFourth = ConstString("fourth");

  // Check. empty state
  DALI_TEST_EQUALS(0u, indexedMap.Count(), TEST_LOCATION);
  DALI_TEST_CHECK(indexedMap.Begin() == indexedMap.End());
  DALI_TEST_CHECK(indexedMap.Empty());
  DALI_TEST_EQUALS(0u, indexedMap.size(), TEST_LOCATION);
  DALI_TEST_CHECK(indexedMap.begin() == indexedMap.end());
  DALI_TEST_CHECK(indexedMap.empty());

  // phase 1 - Regist two element
  DALI_TEST_CHECK(indexedMap.Register(keyFirst, 1));
  DALI_TEST_CHECK(indexedMap.Register(keySecond, 2));

  // Get data by key
  DALI_TEST_EQUALS(2u, indexedMap.Count(), TEST_LOCATION);
  DALI_TEST_CHECK(!indexedMap.Empty());
  IndexedMapGetValueTest(indexedMap, ConstString("first"), true, 1, TEST_LOCATION);
  IndexedMapGetValueTest(indexedMap, ConstString("second"), true, 2, TEST_LOCATION);
  IndexedMapGetValueTest(indexedMap, ConstString("third"), false, 0, TEST_LOCATION);
  IndexedMapGetValueTest(indexedMap, ConstString("fourth"), false, 0, TEST_LOCATION);

  // Get data by index
  DALI_TEST_EQUALS(indexedMap[keyFirst], 1, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap[keySecond], 2, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.GetElementByIndex(0), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.GetElementByIndex(1), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.GetKeyByIndex(0), keyFirst, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.GetKeyByIndex(1), keySecond, TEST_LOCATION);
  DALI_TEST_CHECK(indexedMap.GetKeyElementPairByIndex(0) == IndexedConstStringMap<int>::KeyElementPairType(keyFirst, 1));
  DALI_TEST_CHECK(indexedMap.GetKeyElementPairByIndex(1) == IndexedConstStringMap<int>::KeyElementPairType(keySecond, 2));

  // Const iteration check
  for(const auto& elem : indexedMap)
  {
    if(elem.first == keyFirst)
    {
      DALI_TEST_EQUALS(elem.second, 1, TEST_LOCATION);
    }
    else if(elem.first == keySecond)
    {
      DALI_TEST_EQUALS(elem.second, 2, TEST_LOCATION);
    }
    else
    {
      DALI_TEST_CHECK(false); // Should not get here
    }
  }

  // Iteration and fix data check
  for(auto&& elem : indexedMap)
  {
    if(elem.first == keyFirst)
    {
      elem.second += 110;
    }
  }
  // operator[] fix data check
  indexedMap[keySecond] += 220;

  // Get data by key
  DALI_TEST_EQUALS(2u, indexedMap.Count(), TEST_LOCATION);
  IndexedMapGetValueTest(indexedMap, keyFirst, true, 111, TEST_LOCATION);
  IndexedMapGetValueTest(indexedMap, keySecond, true, 222, TEST_LOCATION);
  IndexedMapGetValueTest(indexedMap, keyThird, false, 0, TEST_LOCATION);
  IndexedMapGetValueTest(indexedMap, keyFourth, false, 0, TEST_LOCATION);

  // Get data by index
  DALI_TEST_EQUALS(indexedMap[keyFirst], 111, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap[keySecond], 222, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.GetElementByIndex(0), 111, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.GetElementByIndex(1), 222, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.GetKeyByIndex(0), keyFirst, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.GetKeyByIndex(1), keySecond, TEST_LOCATION);
  DALI_TEST_CHECK(indexedMap.GetKeyElementPairByIndex(0) == IndexedConstStringMap<int>::KeyElementPairType(keyFirst, 111));
  DALI_TEST_CHECK(indexedMap.GetKeyElementPairByIndex(1) == IndexedConstStringMap<int>::KeyElementPairType(keySecond, 222));

  // Const iteration check
  for(const auto& elem : indexedMap)
  {
    if(elem.first == keyFirst)
    {
      DALI_TEST_EQUALS(elem.second, 111, TEST_LOCATION);
    }
    else if(elem.first == keySecond)
    {
      DALI_TEST_EQUALS(elem.second, 222, TEST_LOCATION);
    }
    else
    {
      DALI_TEST_CHECK(false); // Should not get here
    }
  }

  // phase 2 - Regist two more element
  DALI_TEST_CHECK(!indexedMap.Register(keyFirst, 11));  // Register failed when we try to insert data with same key
  DALI_TEST_CHECK(!indexedMap.Register(keySecond, 22)); // Register failed when we try to insert data with same key
  DALI_TEST_CHECK(indexedMap.Register(keyThird, 3));
  DALI_TEST_CHECK(indexedMap.Register(keyFourth, 4));

  // Get data by key
  DALI_TEST_EQUALS(4, indexedMap.Count(), TEST_LOCATION);
  IndexedMapGetValueTest(indexedMap, keyFirst, true, 111, TEST_LOCATION);
  IndexedMapGetValueTest(indexedMap, keySecond, true, 222, TEST_LOCATION);
  IndexedMapGetValueTest(indexedMap, keyThird, true, 3, TEST_LOCATION);
  IndexedMapGetValueTest(indexedMap, keyFourth, true, 4, TEST_LOCATION);

  // Get elemnt as l-value
  {
    auto iter = indexedMap.Get(keyFourth);
    DALI_TEST_CHECK(iter != indexedMap.end())
    {
      iter->second = 444;
    }
  }

  // Get data by index
  DALI_TEST_EQUALS(indexedMap[keyFirst], 111, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap[keySecond], 222, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap[keyThird], 3, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap[keyFourth], 444, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.GetElementByIndex(0), 111, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.GetElementByIndex(1), 222, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.GetElementByIndex(2), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.GetElementByIndex(3), 444, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.GetKeyByIndex(0), keyFirst, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.GetKeyByIndex(1), keySecond, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.GetKeyByIndex(2), keyThird, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.GetKeyByIndex(3), keyFourth, TEST_LOCATION);
  DALI_TEST_CHECK(indexedMap.GetKeyElementPairByIndex(0) == IndexedConstStringMap<int>::KeyElementPairType(keyFirst, 111));
  DALI_TEST_CHECK(indexedMap.GetKeyElementPairByIndex(1) == IndexedConstStringMap<int>::KeyElementPairType(keySecond, 222));
  DALI_TEST_CHECK(indexedMap.GetKeyElementPairByIndex(2) == IndexedConstStringMap<int>::KeyElementPairType(keyThird, 3));
  DALI_TEST_CHECK(indexedMap.GetKeyElementPairByIndex(3) == IndexedConstStringMap<int>::KeyElementPairType(keyFourth, 444));

  // For coverage
  DALI_TEST_EQUALS(const_cast<const IndexedConstStringMap<int>&>(indexedMap)[keyFirst], 111, TEST_LOCATION);

  // Clear check
  DALI_TEST_CHECK(!indexedMap.Empty());
  indexedMap.Clear();
  DALI_TEST_CHECK(indexedMap.Empty());
  DALI_TEST_EQUALS(0u, indexedMap.Count(), TEST_LOCATION);
  IndexedMapGetValueTest(indexedMap, keyFirst, false, 1, TEST_LOCATION);
  IndexedMapGetValueTest(indexedMap, keySecond, false, 2, TEST_LOCATION);
  IndexedMapGetValueTest(indexedMap, keyThird, false, 3, TEST_LOCATION);
  IndexedMapGetValueTest(indexedMap, keyFourth, false, 4, TEST_LOCATION);

  END_TEST;
}

int UtcDaliIndexedConstStringMapNegative(void)
{
  tet_infoline("Negative test when IndexedConstStringMap access non-exist elements.");

  IndexedConstStringMap<int> indexedMap;

  auto IndexedMapAssertTestWithIndex = [](IndexedConstStringMap<int>& indexedMap, int testIndex) {
    tet_printf("operator[] test");
    // Assert when try to access as const operator[] input with not registered key.
    try
    {
      const auto& elem = const_cast<const IndexedConstStringMap<int>&>(indexedMap)[ConstString("333")];
      if(elem == 0) // Avoid build warning
      {
        DALI_TEST_CHECK(false); // Should not get here
      }
      DALI_TEST_CHECK(false); // Should not get here
    }
    catch(...)
    {
      DALI_TEST_CHECK(true); // Asserted
    }
    // Assert when try to access as operator[] input with not registered key.
    try
    {
      indexedMap[ConstString("333")] = 0;
      DALI_TEST_CHECK(false); // Should not get here
    }
    catch(...)
    {
      DALI_TEST_CHECK(true); // Asserted
    }

    tet_printf("GetElementByIndex test");
    // Assert when try to access as GetElementByIndex input with not registered index.
    try
    {
      const auto& elem = indexedMap.GetElementByIndex(testIndex);
      if(elem == 0) // Avoid build warning
      {
        DALI_TEST_CHECK(false); // Should not get here
      }
      DALI_TEST_CHECK(false); // Should not get here
    }
    catch(...)
    {
      DALI_TEST_CHECK(true); // Asserted
    }

    tet_printf("GetKeyByIndex test");
    // Assert when try to access as GetKeyByIndex input with not registered index.
    try
    {
      const auto& elem = indexedMap.GetKeyByIndex(testIndex);
      if(elem == 0) // Avoid build warning
      {
        DALI_TEST_CHECK(false); // Should not get here
      }
      DALI_TEST_CHECK(false); // Should not get here
    }
    catch(...)
    {
      DALI_TEST_CHECK(true); // Asserted
    }

    tet_printf("GetKeyElementPairByIndex test");
    // Assert when try to access as GetKeyByIndex input with not registered index.
    try
    {
      const auto& elem = indexedMap.GetKeyElementPairByIndex(testIndex);
      if(elem == IndexedConstStringMap<int>::KeyElementPairType(ConstString("zero"), 0)) // Avoid build warning
      {
        DALI_TEST_CHECK(false); // Should not get here
      }
      DALI_TEST_CHECK(false); // Should not get here
    }
    catch(...)
    {
      DALI_TEST_CHECK(true); // Asserted
    }
  };
  // Assert test with empty indexedMap.
  DALI_TEST_CHECK(indexedMap.Empty());
  IndexedMapAssertTestWithIndex(indexedMap, 0);

  // Register 2 data
  DALI_TEST_CHECK(indexedMap.Register(ConstString("first"), 1));
  DALI_TEST_CHECK(indexedMap.Register(ConstString("second"), 2));
  DALI_TEST_EQUALS(2u, indexedMap.Count(), TEST_LOCATION);
  IndexedMapAssertTestWithIndex(indexedMap, 2);

  // Test with always-invalid index like -1
  IndexedMapAssertTestWithIndex(indexedMap, -1);

  END_TEST;
}

int UtcDaliIndexedConstStringMapStressTest(void)
{
  // Copy from utc-Dali-Internal-ConstString.cpp
  static constexpr size_t DB_SIZE = 2000;

  std::vector<std::string> Database;
  Database.reserve(DB_SIZE);

  IndexedConstStringMap<ConstString> constStringDB1;
  constStringDB1.reserve(DB_SIZE);

  IndexedConstStringMap<std::string> constStringDB2;
  constStringDB2.reserve(DB_SIZE);

  for(auto i = 0u; i < DB_SIZE; i++)
  {
    if(i % 3 == 0)
    {
      Database.push_back(RandomString(10));
    }
    else if(i % 4 == 0)
    {
      Database.push_back(RandomString(7));
    }
    else
    {
      Database.push_back(RandomString(11));
    }
    std::string randomValue = RandomString(10);
    DALI_TEST_EQUALS(constStringDB1.Register(ConstString(Database[i]), ConstString(randomValue)), true, TEST_LOCATION);
    DALI_TEST_EQUALS(constStringDB2.Register(ConstString(Database[i]), randomValue), true, TEST_LOCATION);
  }

  // Try to extra regist with same key
  for(auto i = 0u; i < DB_SIZE; i++)
  {
    std::string randomValue = RandomString(2);
    DALI_TEST_EQUALS(constStringDB1.Register(ConstString(Database[i]), ConstString(randomValue)), false, TEST_LOCATION);
    DALI_TEST_EQUALS(constStringDB2.Register(ConstString(Database[i]), randomValue), false, TEST_LOCATION);
  }

  // check eqality betwwen original string and constString
  for(auto i = 0u; i < DB_SIZE; i++)
  {
    DALI_TEST_EQUALS(constStringDB1.GetKeyByIndex(i).GetCString(), Database[i].c_str(), TEST_LOCATION);
    DALI_TEST_EQUALS(constStringDB2.GetKeyByIndex(i).GetCString(), Database[i].c_str(), TEST_LOCATION);
  }

  // check pointer eqality betwwen 2 constString
  for(auto i = 0u; i < DB_SIZE; i++)
  {
    bool pointerEqual = (constStringDB1[ConstString(Database[i])] == ConstString(constStringDB2[ConstString(Database[i])]));
    DALI_TEST_EQUALS(pointerEqual, true, TEST_LOCATION);
  }

  END_TEST;
}
