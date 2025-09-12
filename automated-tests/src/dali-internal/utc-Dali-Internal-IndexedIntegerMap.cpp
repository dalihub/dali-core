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

#include <dali-test-suite-utils.h>

#include <stdlib.h>
#include <string>

#include <dali/internal/common/indexed-integer-map.h>

using namespace Dali;
using Dali::Internal::IndexedIntegerMap;

void utc_dali_indexed_integer_map_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_indexed_integer_map_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliIndexedIntegerMap(void)
{
  IndexedIntegerMap<int> indexedMap;
  const std::uint32_t    keyFirst  = 111111u;
  const std::uint32_t    keySecond = 222222u;
  const std::uint32_t    keyThird  = 333333u;
  const std::uint32_t    keyFourth = 444444u;

  auto IndexedMapGetValueTest = [](const IndexedIntegerMap<int>& indexedMap, std::uint32_t key, bool registered, int element, const char* location)
  {
    const auto& iter = indexedMap.Get(key);
    DALI_TEST_EQUALS(!(iter == indexedMap.end()), registered, location);
    if(registered)
    {
      DALI_TEST_EQUALS(iter->second, element, location);
    }
  };

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
  IndexedMapGetValueTest(indexedMap, keyFirst, true, 1, TEST_LOCATION);
  IndexedMapGetValueTest(indexedMap, keySecond, true, 2, TEST_LOCATION);
  IndexedMapGetValueTest(indexedMap, keyThird, false, 0, TEST_LOCATION);
  IndexedMapGetValueTest(indexedMap, keyFourth, false, 0, TEST_LOCATION);

  // Get data by index
  DALI_TEST_EQUALS(indexedMap[keyFirst], 1, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap[keySecond], 2, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.GetElementByIndex(0), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.GetElementByIndex(1), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.GetKeyByIndex(0), keyFirst, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap.GetKeyByIndex(1), keySecond, TEST_LOCATION);
  DALI_TEST_CHECK(indexedMap.GetKeyElementPairByIndex(0) == IndexedIntegerMap<int>::KeyElementPairType(keyFirst, 1));
  DALI_TEST_CHECK(indexedMap.GetKeyElementPairByIndex(1) == IndexedIntegerMap<int>::KeyElementPairType(keySecond, 2));

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
  DALI_TEST_CHECK(indexedMap.GetKeyElementPairByIndex(0) == IndexedIntegerMap<int>::KeyElementPairType(keyFirst, 111));
  DALI_TEST_CHECK(indexedMap.GetKeyElementPairByIndex(1) == IndexedIntegerMap<int>::KeyElementPairType(keySecond, 222));

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
  DALI_TEST_CHECK(indexedMap.GetKeyElementPairByIndex(0) == IndexedIntegerMap<int>::KeyElementPairType(keyFirst, 111));
  DALI_TEST_CHECK(indexedMap.GetKeyElementPairByIndex(1) == IndexedIntegerMap<int>::KeyElementPairType(keySecond, 222));
  DALI_TEST_CHECK(indexedMap.GetKeyElementPairByIndex(2) == IndexedIntegerMap<int>::KeyElementPairType(keyThird, 3));
  DALI_TEST_CHECK(indexedMap.GetKeyElementPairByIndex(3) == IndexedIntegerMap<int>::KeyElementPairType(keyFourth, 444));

  // For coverage
  DALI_TEST_EQUALS(const_cast<const IndexedIntegerMap<int>&>(indexedMap)[keyFirst], 111, TEST_LOCATION);

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

int UtcDaliIndexedIntegerMapNegative(void)
{
  tet_infoline("Negative test when IndexedIntegerMap access non-exist elements.");

  IndexedIntegerMap<int> indexedMap;

  auto IndexedMapAssertTestWithIndex = [](IndexedIntegerMap<int>& indexedMap, int testIndex)
  {
    tet_printf("operator[] test");
    // Assert when try to access as const operator[] input with not registered key.
    try
    {
      const auto& elem = const_cast<const IndexedIntegerMap<int>&>(indexedMap)[333];
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
      indexedMap[333] = 0;
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
      if(elem == IndexedIntegerMap<int>::KeyElementPairType(0, 0)) // Avoid build warning
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
  DALI_TEST_CHECK(indexedMap.Register(111, 1));
  DALI_TEST_CHECK(indexedMap.Register(222, 2));
  DALI_TEST_EQUALS(2u, indexedMap.Count(), TEST_LOCATION);
  IndexedMapAssertTestWithIndex(indexedMap, 2);

  // Test with always-invalid index like -1
  IndexedMapAssertTestWithIndex(indexedMap, -1);

  END_TEST;
}

int UtcDaliIndexedIntegerMapStressTest(void)
{
  IndexedIntegerMap<int> indexedMap;

  std::vector<std::uint32_t> keyList(2 * (256 + 2));

  // Add 256 kind keys and +- 1
  for(std::uint32_t i = 0; i < 256 + 2; i++)
  {
    // Intention overflow here
    keyList[i]           = i - 1;
    keyList[i + 256 + 2] = ((i - 1) << 24) + (((i - 1) >> 8) & 15);
    if(i == 1)
    {
      //To avoid same Key
      keyList[i + 256 + 2] = 0x0f0f0f0f;
    }
    if(i == 257)
    {
      //To avoid same Key
      keyList[i + 256 + 2] = 0xf0f0f0f0;
    }
  }

  auto IndexedMapGetValueTest = [](const IndexedIntegerMap<int>& indexedMap, std::uint32_t key, bool registered, int element)
  {
    const auto& iter = indexedMap.Get(key);
    DALI_TEST_CHECK(!(iter == indexedMap.end()) == registered);
    if(registered)
    {
      DALI_TEST_CHECK(iter->second == element);
    }
  };

  for(std::size_t i = 0; i < keyList.size(); i++)
  {
    // We don't need to check whole key. Just compare near 0 and 256+2
    for(std::size_t j = 0; j < i && j < 4; j++)
    {
      DALI_TEST_CHECK(!indexedMap.Register(keyList[j], -j - 1));
    }
    for(std::size_t j = 256; j < i && j < 256 + 6; j++)
    {
      DALI_TEST_CHECK(!indexedMap.Register(keyList[j], -j - 1));
    }

    // Regist i'th keylist
    DALI_TEST_CHECK(indexedMap.Register(keyList[i], i));

    // We don't need to check whole key. Just compare near 0 and 256+2 and end and i
    for(std::size_t j = 0; j < keyList.size() && j < 20; j++)
    {
      IndexedMapGetValueTest(indexedMap, keyList[j], j <= i, j);
    }
    for(std::size_t j = 256 - 18; j < keyList.size() && j < 256 + 22; j++)
    {
      IndexedMapGetValueTest(indexedMap, keyList[j], j <= i, j);
    }
    for(std::size_t j = keyList.size() - 20; j < keyList.size(); j++)
    {
      IndexedMapGetValueTest(indexedMap, keyList[j], j <= i, j);
    }
    // When i < 20 overflow occured, but dont care.
    for(std::size_t j = i - 20; j < keyList.size() && j < i + 20; j++)
    {
      IndexedMapGetValueTest(indexedMap, keyList[j], j <= i, j);
    }

    // Keylist have over 500 kind of keys. Print debug rarely.
    if(i % 50 == 0)
    {
      tet_printf("%u / %u pass\n", i, keyList.size());
    }
  }

  END_TEST;
}

int UtcDaliIndexedIntegerMapMoveTest(void)
{
  IndexedIntegerMap<std::string> indexedMap;

  std::string expectString = "wahaha";

  std::string p = expectString; // copy string;
  DALI_TEST_CHECK(indexedMap.Register(111, p));
  DALI_TEST_CHECK(!indexedMap.Register(111, p));

  DALI_TEST_EQUALS(p, expectString, TEST_LOCATION);
  DALI_TEST_EQUALS(indexedMap[111], expectString, TEST_LOCATION);

  // Change expect string
  expectString = "wehihi";
  p            = expectString;

  DALI_TEST_CHECK(indexedMap.Register(222, std::move(p)));

  DALI_TEST_CHECK(p.empty()); // string moved.
  DALI_TEST_EQUALS(indexedMap[222], expectString, TEST_LOCATION);

  p = expectString;

  DALI_TEST_CHECK(!indexedMap.Register(222, std::move(p)));

  END_TEST;
}
