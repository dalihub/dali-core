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
#include <dali/internal/common/fixed-size-memory-pool.h>

using namespace Dali;

void utc_dali_internal_fixedsizememorypool_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_fixedsizememorypool_cleanup(void)
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

int UtcDaliFixedSizeMemoryPoolCreate(void)
{
  gTestObjectConstructed = 0;
  gTestObjectDestructed  = 0;
  gTestObjectMethod      = 0;
  gTestObjectDataAccess  = 0;

  Internal::FixedSizeMemoryPool memoryPool(Internal::TypeSizeWithAlignment<TestObject>::size);

  TestObject* testObject1 = new(memoryPool.Allocate()) TestObject();
  DALI_TEST_CHECK(testObject1);
  DALI_TEST_EQUALS(gTestObjectConstructed, 1U, TEST_LOCATION);

  testObject1->Method();
  DALI_TEST_EQUALS(gTestObjectMethod, 1U, TEST_LOCATION);

  testObject1->DataAccess();
  DALI_TEST_EQUALS(gTestObjectDataAccess, 1U, TEST_LOCATION);

  testObject1->~TestObject();
  memoryPool.Free(testObject1);
  DALI_TEST_EQUALS(gTestObjectDestructed, 1U, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFixedSizeMemoryPoolStressTest01(void)
{
  gTestObjectConstructed = 0;
  gTestObjectDestructed  = 0;
  gTestObjectMethod      = 0;
  gTestObjectDataAccess  = 0;

  const size_t       initialCapacity = 32;
  const unsigned int numObjects      = 7 * 1024 * 1024;

  // Don't specify key layout
  Internal::FixedSizeMemoryPool memoryPool(Internal::TypeSizeWithAlignment<TestObject>::size, initialCapacity /*, default max cap(1M) */);

  Dali::Vector<TestObject*> objects;
  objects.Reserve(numObjects);

  for(unsigned int i = 0; i < numObjects; ++i)
  {
    TestObject* testObject = new(memoryPool.Allocate()) TestObject();
    DALI_TEST_CHECK(testObject);

    objects.PushBack(testObject);

    uint32_t key = memoryPool.GetKeyFromPtr(testObject);
    DALI_TEST_EQUALS(key, i, TEST_LOCATION);
    void* ptr = memoryPool.GetPtrFromKey(key);
    DALI_TEST_EQUALS(static_cast<void*>(testObject), ptr, TEST_LOCATION);
  }

  DALI_TEST_EQUALS(gTestObjectConstructed, numObjects, TEST_LOCATION);

  for(unsigned int i = 0; i < numObjects; ++i)
  {
    objects[i]->~TestObject();
    memoryPool.Free(objects[i]);
  }

  DALI_TEST_EQUALS(gTestObjectDestructed, numObjects, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFixedSizeMemoryPoolStressTest02(void)
{
  gTestObjectConstructed = 0;
  gTestObjectDestructed  = 0;
  gTestObjectMethod      = 0;
  gTestObjectDataAccess  = 0;

  const size_t       initialCapacity = 32;
  const unsigned int numObjects      = 7 * 1024 * 1024;

  // Maxing the number of blocks and providing key layout.
  Internal::FixedSizeMemoryPool memoryPool(Internal::TypeSizeWithAlignment<TestObject>::size, initialCapacity, 4194304, 27);

  Dali::Vector<TestObject*> objects;
  objects.Reserve(numObjects);

  uint32_t     blockSizes[]       = {32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097152, 4194304};
  unsigned int curBlock           = 0;
  unsigned int curBlockStartIndex = 0;
  for(unsigned int i = 0; i < numObjects; ++i)
  {
    TestObject* testObject = new(memoryPool.Allocate()) TestObject();
    DALI_TEST_CHECK(testObject);

    objects.PushBack(testObject);

    if(i == curBlockStartIndex + blockSizes[curBlock])
    {
      curBlockStartIndex += blockSizes[curBlock++];
      tet_printf("Next Block [%d] size %d\n", curBlock, blockSizes[curBlock]);
      tet_printf("  StartIdx [%d]\n", curBlockStartIndex);
    }
    uint32_t key = memoryPool.GetKeyFromPtr(testObject);
    DALI_TEST_EQUALS((key & 0xf8000000) >> 27, curBlock, TEST_LOCATION);
    DALI_TEST_EQUALS((key & 0x07ffffff), i - curBlockStartIndex, TEST_LOCATION);
    void* ptr = memoryPool.GetPtrFromKey(key);
    DALI_TEST_EQUALS(static_cast<void*>(testObject), ptr, TEST_LOCATION);
  }

  DALI_TEST_EQUALS(gTestObjectConstructed, numObjects, TEST_LOCATION);

  for(unsigned int i = 0; i < numObjects; ++i)
  {
    objects[i]->~TestObject();
    memoryPool.Free(objects[i]);
  }

  DALI_TEST_EQUALS(gTestObjectDestructed, numObjects, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFixedSizeMemoryPoolStressTest03(void)
{
  gTestObjectConstructed = 0;
  gTestObjectDestructed  = 0;
  gTestObjectMethod      = 0;
  gTestObjectDataAccess  = 0;

  const size_t initialCapacity = 32;
  const size_t maximumCapacity = 1024; // @todo smaller max cap means higher number of blocks, and bit width should change accordingly.

  const unsigned int numObjects = 1024 * 1024;

  Internal::FixedSizeMemoryPool memoryPool(Internal::TypeSizeWithAlignment<TestObject>::size, initialCapacity, maximumCapacity);

  Dali::Vector<TestObject*> objects;
  objects.Reserve(numObjects);

  for(unsigned int i = 0; i < numObjects; ++i)
  {
    TestObject* testObject = new(memoryPool.Allocate()) TestObject();
    DALI_TEST_CHECK(testObject);

    uint32_t key = memoryPool.GetKeyFromPtr(testObject);
    DALI_TEST_EQUALS(key, i, TEST_LOCATION);
    DALI_TEST_EQUALS((void*)testObject, (void*)memoryPool.GetPtrFromKey(i), TEST_LOCATION);
    objects.PushBack(testObject);
  }

  DALI_TEST_EQUALS(gTestObjectConstructed, numObjects, TEST_LOCATION);

  for(unsigned int i = 0; i < numObjects; ++i)
  {
    objects[i]->~TestObject();
    memoryPool.Free(objects[i]);
  }

  DALI_TEST_EQUALS(gTestObjectDestructed, numObjects, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFixedSizeMemoryPoolPtrFromKeyP01(void)
{
  gTestObjectConstructed = 0;
  gTestObjectDestructed  = 0;
  gTestObjectMethod      = 0;
  gTestObjectDataAccess  = 0;

  const size_t       initialCapacity = 32;
  const unsigned int numObjects      = 1024;

  // Maxing the number of blocks and providing key layout.
  Internal::FixedSizeMemoryPool memoryPool(Internal::TypeSizeWithAlignment<TestObject>::size, initialCapacity, 4194304, 27);

  Dali::Vector<TestObject*> objects;
  objects.Reserve(numObjects);

  uint32_t     blockSizes[]       = {32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576, 2097152, 4194304};
  unsigned int curBlock           = 0;
  unsigned int curBlockStartIndex = 0;
  for(unsigned int i = 0; i < numObjects; ++i)
  {
    TestObject* testObject = new(memoryPool.Allocate()) TestObject();
    DALI_TEST_CHECK(testObject);

    objects.PushBack(testObject);

    if(i == curBlockStartIndex + blockSizes[curBlock])
    {
      curBlockStartIndex += blockSizes[curBlock++];
      tet_printf("Next Block [%d] size %d\n", curBlock, blockSizes[curBlock]);
      tet_printf("  StartIdx [%d]\n", curBlockStartIndex);
    }
    uint32_t key = memoryPool.GetKeyFromPtr(testObject);
    DALI_TEST_EQUALS((key & 0xf8000000) >> 27, curBlock, TEST_LOCATION);
    DALI_TEST_EQUALS((key & 0x07ffffff), i - curBlockStartIndex, TEST_LOCATION);
    void* ptr = memoryPool.GetPtrFromKey(key);
    DALI_TEST_EQUALS(static_cast<void*>(testObject), ptr, TEST_LOCATION);
  }

  DALI_TEST_EQUALS(gTestObjectConstructed, numObjects, TEST_LOCATION);
  END_TEST;
}

int UtcDaliFixedSizeMemoryPoolKeyFromPtrN01(void)
{
  gTestObjectConstructed = 0;
  gTestObjectDestructed  = 0;
  gTestObjectMethod      = 0;
  gTestObjectDataAccess  = 0;

  const size_t       initialCapacity = 32;
  const unsigned int numObjects      = 1024;

  // Maxing the number of blocks and providing key layout.
  Internal::FixedSizeMemoryPool memoryPool(Internal::TypeSizeWithAlignment<TestObject>::size, initialCapacity, 4194304, 27);

  Dali::Vector<TestObject*> objects;
  objects.Reserve(numObjects);

  TestObject* testObject = nullptr;
  for(unsigned int i = 0; i < numObjects; ++i)
  {
    testObject = new(memoryPool.Allocate()) TestObject();
    objects.PushBack(testObject);
  }

  uint32_t key = memoryPool.GetKeyFromPtr(nullptr);
  DALI_TEST_EQUALS(key, (uint32_t)-1, TEST_LOCATION);

  testObject += 1500; // Ensure it's outside block
  key = memoryPool.GetKeyFromPtr(testObject);
  DALI_TEST_EQUALS(key, (uint32_t)-1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFixedSizeMemoryPoolPtrFromKeyN01(void)
{
  gTestObjectConstructed = 0;
  gTestObjectDestructed  = 0;
  gTestObjectMethod      = 0;
  gTestObjectDataAccess  = 0;

  const size_t       initialCapacity = 32;
  const unsigned int numObjects      = 1024;

  // Maxing the number of blocks and providing key layout.
  Internal::FixedSizeMemoryPool memoryPool(Internal::TypeSizeWithAlignment<TestObject>::size, initialCapacity, 4194304, 27);

  Dali::Vector<TestObject*> objects;
  objects.Reserve(numObjects);

  TestObject* testObject = nullptr;
  for(unsigned int i = 0; i < numObjects; ++i)
  {
    testObject = new(memoryPool.Allocate()) TestObject();
    objects.PushBack(testObject);
  }

  void* object = memoryPool.GetPtrFromKey((uint32_t)-1);
  DALI_TEST_CHECK(object == nullptr);

  uint32_t key = (8 << 27) | 33; // Index 33 in block 8 should be out of range

  object = memoryPool.GetPtrFromKey(key);
  DALI_TEST_CHECK(object == nullptr);

  END_TEST;
}

int UtcDaliFixedSizeMemoryPoolKeyFromPtrP02(void)
{
  gTestObjectConstructed = 0;
  gTestObjectDestructed  = 0;
  gTestObjectMethod      = 0;
  gTestObjectDataAccess  = 0;

  const size_t       initialCapacity = 32;
  const size_t       maximumCapacity = 1024;
  const unsigned int numObjects      = 1024;

  Internal::FixedSizeMemoryPool memoryPool(Internal::TypeSizeWithAlignment<TestObject>::size, initialCapacity, maximumCapacity, 0xffffffff /*No block limit*/);

  Dali::Vector<TestObject*> objects;
  objects.Reserve(numObjects);

  TestObject* testObject;
  TestObject* firstObject;

  for(unsigned int i = 0; i < numObjects; ++i)
  {
    testObject = new(memoryPool.Allocate()) TestObject();
    if(i == 0)
    {
      firstObject = testObject;
    }

    objects.PushBack(testObject);
  }

  uint32_t key = memoryPool.GetKeyFromPtr(testObject);
  // Check key == index with no block id
  DALI_TEST_EQUALS(key, 1023, TEST_LOCATION);

  key = memoryPool.GetKeyFromPtr(firstObject);
  // Check key == index with no block id
  DALI_TEST_EQUALS(key, 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFixedSizeMemoryPoolKeyFromPtrN02(void)
{
  gTestObjectConstructed = 0;
  gTestObjectDestructed  = 0;
  gTestObjectMethod      = 0;
  gTestObjectDataAccess  = 0;

  const size_t       initialCapacity = 32;
  const size_t       maximumCapacity = 512;
  const unsigned int numObjects      = 1024;

  Internal::FixedSizeMemoryPool memoryPool(Internal::TypeSizeWithAlignment<TestObject>::size, initialCapacity, maximumCapacity, 0xffffffff /*No block limit*/);

  Dali::Vector<TestObject*> objects;
  objects.Reserve(numObjects);

  TestObject* testObject;
  TestObject* firstObject;

  for(unsigned int i = 0; i < numObjects; ++i)
  {
    testObject = new(memoryPool.Allocate()) TestObject();
    if(i == 0)
    {
      firstObject = testObject;
    }

    objects.PushBack(testObject);
  }

  firstObject--;
  uint32_t key = memoryPool.GetKeyFromPtr(firstObject);
  DALI_TEST_EQUALS(key, (uint32_t)-1, TEST_LOCATION);

  testObject += 1024; // Ensure it's outside the block
  key = memoryPool.GetKeyFromPtr(testObject);
  DALI_TEST_EQUALS(key, (uint32_t)-1, TEST_LOCATION);

  key = memoryPool.GetKeyFromPtr(nullptr);
  DALI_TEST_EQUALS(key, (uint32_t)-1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliFixedSizeMemoryPoolPtrFromKeyP02(void)
{
  gTestObjectConstructed = 0;
  gTestObjectDestructed  = 0;
  gTestObjectMethod      = 0;
  gTestObjectDataAccess  = 0;

  const size_t       initialCapacity = 32;
  const size_t       maximumCapacity = 1024;
  const unsigned int numObjects      = 1024;

  Internal::FixedSizeMemoryPool memoryPool(Internal::TypeSizeWithAlignment<TestObject>::size, initialCapacity, maximumCapacity, 0xffffffff /*No block limit*/);

  Dali::Vector<TestObject*> objects;
  objects.Reserve(numObjects);

  TestObject* testObject;
  TestObject* firstObject;

  for(unsigned int i = 0; i < numObjects; ++i)
  {
    testObject = new(memoryPool.Allocate()) TestObject();
    if(i == 0)
    {
      firstObject = testObject;
    }

    objects.PushBack(testObject);
  }

  void* ptr = memoryPool.GetPtrFromKey(0);
  DALI_TEST_EQUALS(ptr, (void*)firstObject, TEST_LOCATION);

  ptr = memoryPool.GetPtrFromKey(1023);
  DALI_TEST_EQUALS(ptr, (void*)(testObject), TEST_LOCATION);

  END_TEST;
}

int UtcDaliFixedSizeMemoryPoolPtrFromKeyN02(void)
{
  tet_infoline("Negative Test of PtrFromKey in a memory pool with unlimited blocks");

  gTestObjectConstructed = 0;
  gTestObjectDestructed  = 0;
  gTestObjectMethod      = 0;
  gTestObjectDataAccess  = 0;

  const size_t       initialCapacity = 32;
  const size_t       maximumCapacity = 1024;
  const unsigned int numObjects      = 1024;

  Internal::FixedSizeMemoryPool memoryPool(Internal::TypeSizeWithAlignment<TestObject>::size, initialCapacity, maximumCapacity, 0xffffffff /*No block limit*/);

  Dali::Vector<TestObject*> objects;
  objects.Reserve(numObjects);

  TestObject* testObject;

  // There is always 1 block allocated, so testing indices in this range won't fail
  void* ptr = memoryPool.GetPtrFromKey(33);
  DALI_TEST_CHECK(ptr == nullptr);

  for(unsigned int i = 0; i < numObjects; ++i)
  {
    testObject = new(memoryPool.Allocate()) TestObject();
    objects.PushBack(testObject);
  }

  ptr = memoryPool.GetPtrFromKey(1024);
  DALI_TEST_CHECK(ptr != nullptr); // this key successfully finds a block, even though
                                   // it's not been alloc'd.

  ptr = memoryPool.GetPtrFromKey(204029); // Check a key outside the allocd range.
  DALI_TEST_CHECK(ptr == nullptr);

  END_TEST;
}
