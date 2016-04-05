/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <iostream>

#include <stdlib.h>
#include <string>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

void utc_dali_property_array_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_property_array_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliPropertyArrayPushBackP(void)
{
  Property::Array array;

  DALI_TEST_CHECK( 0 == array.Size() );

  array.PushBack( 1 );

  DALI_TEST_CHECK( 1 == array.Size() );

  DALI_TEST_CHECK( array[0].Get<int>() == 1 );

  END_TEST;
}

int UtcDaliPropertyArrayCapacityP(void)
{
  Property::Array array;
  DALI_TEST_CHECK( array.Empty() );

  array.Reserve(3);

  DALI_TEST_CHECK( 3 == array.Capacity() );
  END_TEST;
}

int UtcDaliPropertyArrayReserveP(void)
{
  Property::Array array;
  DALI_TEST_CHECK( array.Empty() );

  array.Reserve(3);

  DALI_TEST_CHECK( 3 == array.Capacity() );
  DALI_TEST_CHECK( 0 == array.Size() );

  array.PushBack( 1 );
  array.PushBack( "world" );
  array.PushBack( 3 );
  END_TEST;
}

int UtcDaliPropertyArraySizeP(void)
{
  Property::Array array;
  DALI_TEST_CHECK( 0 == array.Capacity() );
  DALI_TEST_CHECK( 0 == array.Size() );

  array.Reserve(3);

  DALI_TEST_CHECK( 3 == array.Capacity() );
  DALI_TEST_CHECK( 0 == array.Size() );

  array.PushBack( 1 );
  array.PushBack( "world" );
  array.PushBack( 3 );

  DALI_TEST_CHECK( 3 == array.Size() );

  END_TEST;
}

int UtcDaliPropertyArrayCountP(void)
{
  Property::Array array;
  DALI_TEST_CHECK( 0 == array.Capacity() );
  DALI_TEST_CHECK( 0 == array.Count() );

  array.Reserve(3);

  DALI_TEST_CHECK( 3 == array.Capacity() );
  DALI_TEST_CHECK( 0 == array.Count() );

  array.PushBack( 1 );
  array.PushBack( "world" );
  array.PushBack( 3 );

  DALI_TEST_CHECK( 3 == array.Count() );

  END_TEST;
}

int UtcDaliPropertyArrayEmptyP(void)
{
  Property::Array array;
  DALI_TEST_CHECK( array.Empty() );

  array.Reserve(3);

  DALI_TEST_CHECK( array.Empty() );

  array.PushBack( 1 );
  array.PushBack( "world" );
  array.PushBack( 3 );

  DALI_TEST_CHECK( !array.Empty() );

  END_TEST;
}

int UtcDaliPropertyArrayClearP(void)
{
  Property::Array array;
  DALI_TEST_CHECK( array.Empty() );

  array.Reserve(3);

  DALI_TEST_CHECK( array.Empty() );

  array.PushBack( 1 );
  array.PushBack( "world" );
  array.PushBack( 3 );

  DALI_TEST_CHECK( !array.Empty() );

  array.Clear();

  DALI_TEST_CHECK( array.Empty() );

  END_TEST;
}

int UtcDaliPropertyArrayIndexOperatorP(void)
{
  Property::Array array;

  array.Reserve(3);
  array.PushBack( 1 );
  array.PushBack( "world" );
  array.PushBack( 3 );

  DALI_TEST_CHECK( array[0].Get<int>() == 1 );
  DALI_TEST_CHECK( array[1].Get<std::string>() == "world" );
  DALI_TEST_CHECK( array[2].Get<int>() == 3 );

  END_TEST;
}

int UtcDaliPropertyArrayConstIndexOperatorP(void)
{
  Property::Array anArray;

  anArray.Reserve(3);
  anArray.PushBack( 1 );
  anArray.PushBack( "world" );
  anArray.PushBack( 3 );

  const Property::Array array(anArray);

  DALI_TEST_CHECK( array[0].Get<int>() == 1 );
  DALI_TEST_CHECK( array[1].Get<std::string>() == "world" );
  DALI_TEST_CHECK( array[2].Get<int>() == 3 );

  END_TEST;
}

int UtcDaliPropertyArrayAssignmentOperatorP(void)
{
  Property::Array anArray;

  anArray.Reserve(3);
  anArray.PushBack( 1 );
  anArray.PushBack( "world" );
  anArray.PushBack( 3 );

  Property::Array array = anArray;

  DALI_TEST_CHECK( array[0].Get<int>() == 1 );
  DALI_TEST_CHECK( array[1].Get<std::string>() == "world" );
  DALI_TEST_CHECK( array[2].Get<int>() == 3 );

  END_TEST;
}

int UtcDaliPropertyArrayResize(void)
{
  Property::Array array;

  array.Resize(3);
  DALI_TEST_CHECK( array.Count() == 3 );

  array.Resize(5);
  DALI_TEST_CHECK( array.Count() == 5 );

  END_TEST;
}

int UtcDaliPropertyArrayOstream01(void)
{
  std::ostringstream oss;

  Property::Array array;
  array.PushBack( 0 );
  array.PushBack( 1 );
  array.PushBack( 2 );

  oss << array;
  DALI_TEST_EQUALS( oss.str().compare("Array(3) = [0, 1, 2]"), 0, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPropertyArrayOstream02(void)
{
  std::ostringstream oss;

  Property::Array array1;
  array1.PushBack( 0 );
  array1.PushBack( 1 );
  array1.PushBack( 2 );

  Property::Array array2;
  array2.PushBack(array1);
  array2.PushBack( 1 );
  array2.PushBack( 2 );

  oss << array2;
  DALI_TEST_EQUALS( oss.str().compare("Array(3) = [Array(3) = [0, 1, 2], 1, 2]"), 0, TEST_LOCATION );

  END_TEST;
}
