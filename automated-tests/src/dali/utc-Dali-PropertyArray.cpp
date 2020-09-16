/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <stdlib.h>

#include <iostream>
#include <string>

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

  DALI_TEST_CHECK(0 == array.Size());

  array.PushBack(1);

  DALI_TEST_CHECK(1 == array.Size());

  DALI_TEST_CHECK(array[0].Get<int>() == 1);

  END_TEST;
}

int UtcDaliPropertyArrayAddP(void)
{
  Property::Array array;

  DALI_TEST_CHECK(0 == array.Size());

  array.Add(1);

  DALI_TEST_CHECK(1 == array.Size());
  DALI_TEST_CHECK(array[0].Get<int>() == 1);

  array.Add(2)
    .Add(3)
    .Add(4.f);

  DALI_TEST_EQUALS(4u, array.Size(), TEST_LOCATION);
  DALI_TEST_EQUALS(array[1].Get<int>(), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(array[2].Get<int>(), 3, TEST_LOCATION);
  DALI_TEST_EQUALS(array[3].Get<float>(), 4.f, Math::MACHINE_EPSILON_1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPropertyArrayCapacityP(void)
{
  Property::Array array;
  DALI_TEST_CHECK(array.Empty());

  array.Reserve(3);

  DALI_TEST_CHECK(3 == array.Capacity());
  END_TEST;
}

int UtcDaliPropertyArrayReserveP(void)
{
  Property::Array array;
  DALI_TEST_CHECK(array.Empty());

  array.Reserve(3);

  DALI_TEST_CHECK(3 == array.Capacity());
  DALI_TEST_CHECK(0 == array.Size());

  array.PushBack(1);
  array.PushBack("world");
  array.PushBack(3);
  END_TEST;
}

int UtcDaliPropertyArraySizeP(void)
{
  Property::Array array;
  DALI_TEST_CHECK(0 == array.Capacity());
  DALI_TEST_CHECK(0 == array.Size());

  array.Reserve(3);

  DALI_TEST_CHECK(3 == array.Capacity());
  DALI_TEST_CHECK(0 == array.Size());

  array.PushBack(1);
  array.PushBack("world");
  array.PushBack(3);

  DALI_TEST_CHECK(3 == array.Size());

  END_TEST;
}

int UtcDaliPropertyArrayCountP(void)
{
  Property::Array array;
  DALI_TEST_CHECK(0 == array.Capacity());
  DALI_TEST_CHECK(0 == array.Count());

  array.Reserve(3);

  DALI_TEST_CHECK(3 == array.Capacity());
  DALI_TEST_CHECK(0 == array.Count());

  array.PushBack(1);
  array.PushBack("world");
  array.PushBack(3);

  DALI_TEST_CHECK(3 == array.Count());

  END_TEST;
}

int UtcDaliPropertyArrayEmptyP(void)
{
  Property::Array array;
  DALI_TEST_CHECK(array.Empty());

  array.Reserve(3);

  DALI_TEST_CHECK(array.Empty());

  array.PushBack(1);
  array.PushBack("world");
  array.PushBack(3);

  DALI_TEST_CHECK(!array.Empty());

  END_TEST;
}

int UtcDaliPropertyArrayClearP(void)
{
  Property::Array array;
  DALI_TEST_CHECK(array.Empty());

  array.Reserve(3);

  DALI_TEST_CHECK(array.Empty());

  array.PushBack(1);
  array.PushBack("world");
  array.PushBack(3);

  DALI_TEST_CHECK(!array.Empty());

  array.Clear();

  DALI_TEST_CHECK(array.Empty());

  END_TEST;
}

int UtcDaliPropertyArrayIndexOperatorP(void)
{
  Property::Array array;

  array.Reserve(3);
  array.PushBack(1);
  array.PushBack("world");
  array.PushBack(3);

  DALI_TEST_CHECK(array[0].Get<int>() == 1);
  DALI_TEST_CHECK(array[1].Get<std::string>() == "world");
  DALI_TEST_CHECK(array[2].Get<int>() == 3);

  END_TEST;
}

int UtcDaliPropertyArrayConstIndexOperatorP(void)
{
  Property::Array anArray;

  anArray.Reserve(3);
  anArray.PushBack(1);
  anArray.PushBack("world");
  anArray.PushBack(3);

  const Property::Array array(anArray);

  DALI_TEST_CHECK(array[0].Get<int>() == 1);
  DALI_TEST_CHECK(array[1].Get<std::string>() == "world");
  DALI_TEST_CHECK(array[2].Get<int>() == 3);

  END_TEST;
}

int UtcDaliPropertyArrayAssignmentOperatorP(void)
{
  Property::Array anArray;

  anArray.Reserve(3);
  anArray.PushBack(1);
  anArray.PushBack("world");
  anArray.PushBack(3);

  Property::Array array = anArray;

  DALI_TEST_CHECK(array[0].Get<int>() == 1);
  DALI_TEST_CHECK(array[1].Get<std::string>() == "world");
  DALI_TEST_CHECK(array[2].Get<int>() == 3);

  END_TEST;
}

int UtcDaliPropertyArrayResize(void)
{
  Property::Array array;

  array.Resize(3);
  DALI_TEST_CHECK(array.Count() == 3);

  array.Resize(5);
  DALI_TEST_CHECK(array.Count() == 5);

  END_TEST;
}

int UtcDaliPropertyArrayOstream01(void)
{
  std::ostringstream oss;

  Property::Array array;
  array.PushBack(0);
  array.PushBack(1);
  array.PushBack(2);

  oss << array;
  DALI_TEST_EQUALS(oss.str().compare("Array(3) = [0, 1, 2]"), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPropertyArrayOstream02(void)
{
  std::ostringstream oss;

  Property::Array array1;
  array1.PushBack(0);
  array1.PushBack(1);
  array1.PushBack(2);

  Property::Array array2;
  array2.PushBack(array1);
  array2.PushBack(1);
  array2.PushBack(2);

  oss << array2;
  DALI_TEST_EQUALS(oss.str().compare("Array(3) = [Array(3) = [0, 1, 2], 1, 2]"), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPropertyArrayCopyConstructor(void)
{
  Property::Array array1;
  array1.PushBack(0);
  array1.PushBack(1);
  array1.PushBack(2);
  DALI_TEST_EQUALS(3u, array1.Size(), TEST_LOCATION);

  Property::Array array2(array1);
  DALI_TEST_EQUALS(3u, array1.Size(), TEST_LOCATION);
  DALI_TEST_EQUALS(3u, array2.Size(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliPropertyArrayAssignmentOperator(void)
{
  Property::Array array1;
  array1.PushBack(0);
  array1.PushBack(1);
  array1.PushBack(2);
  DALI_TEST_EQUALS(3u, array1.Size(), TEST_LOCATION);

  Property::Array array2;
  array2.PushBack(4);
  DALI_TEST_EQUALS(1u, array2.Size(), TEST_LOCATION);

  array2 = array1;
  DALI_TEST_EQUALS(3u, array1.Size(), TEST_LOCATION);
  DALI_TEST_EQUALS(3u, array2.Size(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliPropertyArrayMoveConstructor(void)
{
  Property::Array array1;
  array1.PushBack(0);
  array1.PushBack(1);
  array1.PushBack(2);
  DALI_TEST_EQUALS(3u, array1.Size(), TEST_LOCATION);

  Property::Array array2(std::move(array1));
  DALI_TEST_EQUALS(3u, array2.Size(), TEST_LOCATION);

  // Calling any methods on array1 will debug assert
  const char* exceptionMessage = "Cannot use an object previously used as an r-value";
  DALI_TEST_ASSERTION(array1.Count(), exceptionMessage);
  DALI_TEST_ASSERTION(array1.PushBack(Property::Value()), exceptionMessage);
  DALI_TEST_ASSERTION(array1.Count(), exceptionMessage);
  DALI_TEST_ASSERTION(array1.Clear(), exceptionMessage);
  DALI_TEST_ASSERTION(array1.Reserve(1), exceptionMessage);
  DALI_TEST_ASSERTION(array1.Resize(1), exceptionMessage);
  DALI_TEST_ASSERTION(array1.Capacity(), exceptionMessage);
  DALI_TEST_ASSERTION(array1[0], exceptionMessage);
  DALI_TEST_ASSERTION(const_cast<const Property::Array&>(array1)[0], exceptionMessage);
  DALI_TEST_ASSERTION(Property::Array temp; array1 = temp, exceptionMessage);

  END_TEST;
}

int UtcDaliPropertyArrayMoveAssignmentOperator(void)
{
  Property::Array array1;
  array1.PushBack(0);
  array1.PushBack(1);
  array1.PushBack(2);
  DALI_TEST_EQUALS(3u, array1.Size(), TEST_LOCATION);

  Property::Array array2;
  array2.PushBack(4);
  DALI_TEST_EQUALS(1u, array2.Size(), TEST_LOCATION);

  array2 = std::move(array1);
  DALI_TEST_EQUALS(3u, array2.Size(), TEST_LOCATION);

  // Calling any methods on array1 will debug assert
  const char* exceptionMessage = "Cannot use an object previously used as an r-value";
  DALI_TEST_ASSERTION(array1.Count(), exceptionMessage);
  DALI_TEST_ASSERTION(array1.PushBack(Property::Value()), exceptionMessage);
  DALI_TEST_ASSERTION(array1.Count(), exceptionMessage);
  DALI_TEST_ASSERTION(array1.Clear(), exceptionMessage);
  DALI_TEST_ASSERTION(array1.Reserve(1), exceptionMessage);
  DALI_TEST_ASSERTION(array1.Resize(1), exceptionMessage);
  DALI_TEST_ASSERTION(array1.Capacity(), exceptionMessage);
  DALI_TEST_ASSERTION(array1[0], exceptionMessage);
  DALI_TEST_ASSERTION(const_cast<const Property::Array&>(array1)[0], exceptionMessage);
  DALI_TEST_ASSERTION(Property::Array temp; array1 = temp, exceptionMessage);

  // Self assignemnt
  array2 = std::move(array2);
  DALI_TEST_EQUALS(3u, array2.Size(), TEST_LOCATION); // still works, no debug assert

  END_TEST;
}

int UtcDaliPropertyArrayInitializerListConstructor(void)
{
  Property::Array array{1, 2, "hello"};
  DALI_TEST_EQUALS(3u, array.Size(), TEST_LOCATION);
  DALI_TEST_EQUALS(Property::INTEGER, array.GetElementAt(0).GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(Property::INTEGER, array.GetElementAt(1).GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(Property::STRING, array.GetElementAt(2).GetType(), TEST_LOCATION);

  END_TEST;
}
