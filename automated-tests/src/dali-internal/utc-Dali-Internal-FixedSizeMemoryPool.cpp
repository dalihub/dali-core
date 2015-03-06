/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

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

int gTestObjectConstructed = 0;
int gTestObjectDestructed = 0;
int gTestObjectMethod = 0;
int gTestObjectDataAccess = 0;

} // namespace


class TestObject
{
public:

  TestObject()
  : mData1( 0 ),
    mData2( false )
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

  int mData1;
  bool mData2;

};

int UtcDaliFixedSizeMemoryPoolCreate(void)
{
  gTestObjectConstructed = 0;
  gTestObjectDestructed = 0;
  gTestObjectMethod = 0;
  gTestObjectDataAccess = 0;

  Internal::FixedSizeMemoryPool memoryPool( Internal::TypeSizeWithAlignment< TestObject >::size );

  TestObject* testObject1 = new (memoryPool.Allocate()) TestObject();
  DALI_TEST_CHECK( testObject1 );
  DALI_TEST_EQUALS( gTestObjectConstructed, 1, TEST_LOCATION );

  testObject1->Method();
  DALI_TEST_EQUALS( gTestObjectMethod, 1, TEST_LOCATION );

  testObject1->DataAccess();
  DALI_TEST_EQUALS( gTestObjectDataAccess, 1, TEST_LOCATION );

  testObject1->~TestObject();
  memoryPool.Free( testObject1 );
  DALI_TEST_EQUALS( gTestObjectDestructed, 1, TEST_LOCATION );

  END_TEST;
}

int UtcDaliFixedSizeMemoryPoolStressTest(void)
{
  gTestObjectConstructed = 0;
  gTestObjectDestructed = 0;
  gTestObjectMethod = 0;
  gTestObjectDataAccess = 0;

  const size_t initialCapacity = 32;
  const size_t maximumCapacity = 1024;

  const unsigned int numObjects = 1024 * 1024;

  Internal::FixedSizeMemoryPool memoryPool( Internal::TypeSizeWithAlignment< TestObject >::size, initialCapacity, maximumCapacity );

  Dali::Vector<TestObject*> objects;
  objects.Reserve( numObjects );

  for( unsigned int i = 0; i < numObjects; ++i )
  {
    TestObject* testObject = new ( memoryPool.Allocate() ) TestObject();
    DALI_TEST_CHECK( testObject );

    objects.PushBack( testObject );
  }

  DALI_TEST_EQUALS( gTestObjectConstructed, numObjects, TEST_LOCATION );

  for( unsigned int i = 0; i < numObjects; ++i )
  {
    objects[i]->~TestObject();
    memoryPool.Free( objects[i] );
  }

  DALI_TEST_EQUALS( gTestObjectDestructed, numObjects, TEST_LOCATION );

  END_TEST;
}
