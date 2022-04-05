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
#include <dali/devel-api/common/free-list.h>
#include <dali/public-api/dali-core.h>

#include <iostream>

using namespace Dali;

void utc_dali_free_list_startuP(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_free_list_cleanuP(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliFreeListConstructor01P(void)
{
  TestApplication application;
  tet_infoline("UtcDaliFreeListConstructor01P simple constructor check");
  try
  {
    FreeList freeList;
    DALI_TEST_CHECK(true);
  }
  catch(...)
  {
    DALI_TEST_CHECK(false);
  }

  END_TEST;
}

int UtcDaliFreeListAddGetRemove(void)
{
  TestApplication application;
  tet_infoline("UtcDaliFreeListAddGetRemove Add, Get, and Remove test");
  FreeList list1;
  FreeList list2;

  auto FreeListAddTest = [](FreeList& list, std::uint32_t value, std::uint32_t exceptIndex, const char* location) {
    std::uint32_t index = list.Add(value);
    DALI_TEST_EQUALS(index, exceptIndex, location);
  };
  auto FreeListGetTest = [](const FreeList& list, std::uint32_t exceptValue, std::uint32_t index, const char* location) {
    std::uint32_t value = list[index];
    DALI_TEST_EQUALS(value, exceptValue, location);
  };

  tet_printf("Add some values first\n");
  FreeListAddTest(list1, 111, 0, TEST_LOCATION);
  FreeListAddTest(list1, 222, 1, TEST_LOCATION);
  FreeListAddTest(list1, 333, 2, TEST_LOCATION);
  FreeListAddTest(list1, 444, 3, TEST_LOCATION);

  tet_printf("Check input values exist well\n");
  FreeListGetTest(list1, 111, 0, TEST_LOCATION);
  FreeListGetTest(list1, 222, 1, TEST_LOCATION);
  FreeListGetTest(list1, 333, 2, TEST_LOCATION);
  FreeListGetTest(list1, 444, 3, TEST_LOCATION);

  tet_printf("Remove 1 and 3 value\n");

  list1.Remove(1);
  list1.Remove(3);
  tet_printf("Check not-removed values exist well\n");
  FreeListGetTest(list1, 111, 0, TEST_LOCATION);
  FreeListGetTest(list1, 333, 2, TEST_LOCATION);

  tet_printf("Copy list. FreeList is not handle. copy whole info\n");
  list2 = list1;
  FreeListGetTest(list2, 111, 0, TEST_LOCATION);
  FreeListGetTest(list2, 333, 2, TEST_LOCATION);

  tet_printf("Add some values after removed\n");
  FreeListAddTest(list1, 555, 3, TEST_LOCATION);
  FreeListAddTest(list1, 666, 1, TEST_LOCATION);
  FreeListAddTest(list1, 777, 4, TEST_LOCATION);
  FreeListAddTest(list2, 888, 3, TEST_LOCATION);

  tet_printf("Check input values exist well\n");
  FreeListGetTest(list1, 111, 0, TEST_LOCATION);
  FreeListGetTest(list1, 666, 1, TEST_LOCATION);
  FreeListGetTest(list1, 333, 2, TEST_LOCATION);
  FreeListGetTest(list1, 555, 3, TEST_LOCATION);
  FreeListGetTest(list1, 777, 4, TEST_LOCATION);
  FreeListGetTest(list2, 111, 0, TEST_LOCATION);
  FreeListGetTest(list2, 333, 2, TEST_LOCATION);
  FreeListGetTest(list2, 888, 3, TEST_LOCATION);

  tet_printf("Change value directly\n");
  list2.Remove(2);
  list2[3] = 999;
  FreeListGetTest(list2, 111, 0, TEST_LOCATION);
  FreeListGetTest(list2, 999, 3, TEST_LOCATION);

  END_TEST;
}