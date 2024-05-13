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

#include <dali-test-suite-utils.h>
#include <dali/public-api/dali-core.h>

// Internal headers are allowed here
#include <dali/internal/common/dummy-memory-pool.h>

using namespace Dali;

void utc_dali_internal_dummymemorypool_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_dummymemorypool_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
unsigned int gTestObjectConstructed = 0;
unsigned int gTestObjectDestructed  = 0;
unsigned int gTestObjectMethod      = 0;
unsigned int gTestObjectDataAccess  = 0;

class TestObject
{
public:
  TestObject()
  : mData1(0),
    mData2(false)
  {
    gTestObjectConstructed++;
  }

  ~TestObject()
  {
    gTestObjectDestructed++;
  }

  void Method()
  {
    gTestObjectMethod++;
  }

  void DataAccess()
  {
    mData1++;
    mData2 = true;

    gTestObjectDataAccess++;
  }

private:
  unsigned int mData1;
  bool         mData2;
};

} // namespace

int UtcDaliDummyMemoryPoolCreate(void)
{
  gTestObjectConstructed = 0;
  gTestObjectDestructed  = 0;
  gTestObjectMethod      = 0;
  gTestObjectDataAccess  = 0;

  Internal::DummyMemoryPool memoryPool(Internal::TypeSizeWithAlignment<TestObject>::size);

  TestObject* testObject1 = new(memoryPool.AllocateThreadSafe()) TestObject();
  DALI_TEST_CHECK(testObject1);
  DALI_TEST_EQUALS(gTestObjectConstructed, 1U, TEST_LOCATION);

  // Dummy memory pool depend on real count of objects
  DALI_TEST_EQUALS(memoryPool.GetCapacity(), Internal::TypeSizeWithAlignment<TestObject>::size, TEST_LOCATION);

  testObject1->Method();
  DALI_TEST_EQUALS(gTestObjectMethod, 1U, TEST_LOCATION);

  testObject1->DataAccess();
  DALI_TEST_EQUALS(gTestObjectDataAccess, 1U, TEST_LOCATION);

  testObject1->~TestObject();
  memoryPool.FreeThreadSafe(testObject1);
  DALI_TEST_EQUALS(gTestObjectDestructed, 1U, TEST_LOCATION);

  // Dummy memory pool depend on real count of objects
  DALI_TEST_EQUALS(memoryPool.GetCapacity(), 0U, TEST_LOCATION);

  END_TEST;
}

int UtcDaliDummyMemoryPoolKeyFromPtrN01(void)
{
  Internal::DummyMemoryPool memoryPool(Internal::TypeSizeWithAlignment<TestObject>::size);

#if defined(__LP64__)
  try
  {
    memoryPool.GetKeyFromPtr(nullptr);
    DALI_TEST_CHECK(false);
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
  }
#else
  tet_printf("We don't have any testcase for 32bit OS. Just skip here\n");
  DALI_TEST_CHECK(true);
#endif

  END_TEST;
}

int UtcDaliDummyMemoryPoolPtrFromKeyN01(void)
{
  Internal::DummyMemoryPool memoryPool(Internal::TypeSizeWithAlignment<TestObject>::size);

#if defined(__LP64__)
  try
  {
    memoryPool.GetPtrFromKey(static_cast<Internal::DummyMemoryPool::KeyType>(-1));
    DALI_TEST_CHECK(false);
  }
  catch(...)
  {
    DALI_TEST_CHECK(true);
  }
#else
  tet_printf("We don't have any testcase for 32bit OS. Just skip here\n");
  DALI_TEST_CHECK(true);
#endif

  END_TEST;
}
