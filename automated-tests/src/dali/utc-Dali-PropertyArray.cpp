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

  // Self std::move assignment make compile warning over gcc-13. Let we ignore the warning.
#if(__GNUC__ >= 13)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-move"
#endif
  // Self assignemnt
  array2 = std::move(array2);
  DALI_TEST_EQUALS(3u, array2.Size(), TEST_LOCATION); // still works, no debug assert
#if(__GNUC__ >= 13)
#pragma GCC diagnostic pop
#endif

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

int UtcDaliPropertyArrayGetHashP01(void)
{
  tet_infoline("Check Property::Array::GetHash()");

  Property::Array array;

  tet_printf("Check empty array is not zero.\n");
  auto emptyArrayHash = array.GetHash();

  DALI_TEST_NOT_EQUALS(emptyArrayHash, static_cast<decltype(emptyArrayHash)>(0u), Math::MACHINE_EPSILON_100, TEST_LOCATION);

  array.PushBack(1);
  array.PushBack(Vector2(2.0f, 3.0f));
  array.PushBack(4);

  DALI_TEST_NOT_EQUALS(emptyArrayHash, array.GetHash(), Math::MACHINE_EPSILON_100, TEST_LOCATION);

  Property::Value& lValue = array.GetElementAt(1);

  auto originalHash = array.GetHash();

  tet_printf("Check if l-value of some element changeness applied.\n");

  DALI_TEST_EQUALS(originalHash, array.GetHash(), TEST_LOCATION);

  lValue       = Property::Value(Vector3(2.0f, 3.0f, 0.0f));
  auto newHash = array.GetHash();

  DALI_TEST_NOT_EQUALS(originalHash, newHash, Math::MACHINE_EPSILON_100, TEST_LOCATION);
  DALI_TEST_EQUALS(array.GetElementAt(1).Get<Vector3>(), Vector3(2.0f, 3.0f, 0.0f), TEST_LOCATION);

  tet_printf("Revert l-value as original value, and check it applied.\n");
  lValue  = Property::Value(Vector2(2.0f, 3.0f));
  newHash = array.GetHash();

  DALI_TEST_EQUALS(originalHash, newHash, TEST_LOCATION);

  Property::Array otherArray;

  otherArray.PushBack(1);
  otherArray.PushBack(Vector2(2.0f, 3.0f));
  otherArray.PushBack(4);

  tet_printf("Check same array return same hash.\n");
  DALI_TEST_EQUALS(originalHash, otherArray.GetHash(), TEST_LOCATION);

  otherArray.Resize(4);

  tet_printf("Check array with difference size return difference hash.\n");
  DALI_TEST_NOT_EQUALS(originalHash, otherArray.GetHash(), Math::MACHINE_EPSILON_100, TEST_LOCATION);

  otherArray[3] = Property::Value("5");
  DALI_TEST_NOT_EQUALS(originalHash, otherArray.GetHash(), Math::MACHINE_EPSILON_100, TEST_LOCATION);

  otherArray.Resize(3);

  tet_printf("Check resized array return same hash.\n");
  DALI_TEST_EQUALS(originalHash, otherArray.GetHash(), TEST_LOCATION);

  Property::Array otherArray2;

  otherArray2.PushBack(4);
  otherArray2.PushBack(Vector2(2.0f, 3.0f));
  otherArray2.PushBack(1);

  tet_printf("Check array with difference order return difference hash.\n");
  DALI_TEST_NOT_EQUALS(originalHash, otherArray2.GetHash(), Math::MACHINE_EPSILON_100, TEST_LOCATION);

  array.Clear();
  tet_printf("Check cleared arrayy has same value with empty array.\n");
  DALI_TEST_EQUALS(emptyArrayHash, array.GetHash(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliPropertyArrayGetHashP02(void)
{
  tet_infoline("Check Property::Array::GetHash() if value is Map or Array.");

  Property::Array array;
  Property::Array subArray;
  Property::Map   subMap;

  subArray.PushBack(2);
  subArray.PushBack(3);

  subMap.Insert(0, "0");
  subMap.Insert("1", 1);

  array.PushBack(1);
  array.PushBack(Vector2(2.0f, 3.0f));
  array.PushBack(subArray);
  array.PushBack(subMap);
  array.PushBack(4);

  auto originalHash = array.GetHash();

  Property::Array* subArrayPtr = array.GetElementAt(2).GetArray();
  Property::Map*   subMapPtr   = array.GetElementAt(3).GetMap();

  DALI_TEST_CHECK(subArrayPtr);
  DALI_TEST_CHECK(subMapPtr);

  tet_printf("Check if l-value of some element changeness applied.\n");

  subArrayPtr->PushBack(-2);
  DALI_TEST_NOT_EQUALS(originalHash, array.GetHash(), Math::MACHINE_EPSILON_100, TEST_LOCATION);
  subArrayPtr->Resize(2);
  DALI_TEST_EQUALS(originalHash, array.GetHash(), TEST_LOCATION);

  subMapPtr->Insert(2, 2);
  subMapPtr->Insert("3", "3");
  auto newHash = array.GetHash();
  DALI_TEST_NOT_EQUALS(originalHash, newHash, Math::MACHINE_EPSILON_100, TEST_LOCATION);
  subMapPtr->Remove(2);
  DALI_TEST_NOT_EQUALS(originalHash, array.GetHash(), Math::MACHINE_EPSILON_100, TEST_LOCATION);
  DALI_TEST_NOT_EQUALS(newHash, array.GetHash(), Math::MACHINE_EPSILON_100, TEST_LOCATION);
  subMapPtr->Remove("3");
  DALI_TEST_EQUALS(originalHash, array.GetHash(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliPropertyArrayEqualNonFloatType(void)
{
  tet_infoline("Check Property::Array equality if all values don't need to consider epsilon");

  Property::Array array1;
  Property::Array subArray1;
  Property::Map   subMap1;

  subArray1.PushBack(2);
  subArray1.PushBack(3);

  subMap1.Insert(0, "0");
  subMap1.Insert("1", 1);

  array1.PushBack(1);
  array1.PushBack(false);
  array1.PushBack(subArray1);
  array1.PushBack(subMap1);
  array1.PushBack(4);

  tet_printf("Check self-equality return true\n");
  DALI_TEST_CHECK(array1 == array1);
  DALI_TEST_EQUALS(array1, array1, TEST_LOCATION);

  tet_printf("Generate exactly same Property::Array with array1\n");

  Property::Array array2;
  Property::Array subArray2;
  Property::Map   subMap2;

  subArray2.PushBack(2);
  subArray2.PushBack(3);

  subMap2.Insert(0, "0");
  subMap2.Insert("1", 1);

  array2.PushBack(1);
  array2.PushBack(false);
  array2.PushBack(subArray1);
  array2.PushBack(subMap1);
  array2.PushBack(4);

  DALI_TEST_CHECK(array1 == array2);
  DALI_TEST_EQUALS(array1, array2, TEST_LOCATION);

  tet_printf("Change array2\n");

  auto oldCount = array2.Count();
  array2.PushBack(2);
  DALI_TEST_CHECK(array1 != array2);

  tet_printf("Change array2 again\n");

  array2.Resize(oldCount);
  DALI_TEST_CHECK(array1 == array2);

  tet_printf("Change array2\n");

  Property::Value& value = array2.GetElementAt(1);
  value                  = true;
  DALI_TEST_CHECK(array1 != array2);

  tet_printf("Change array2 again\n");

  value = false;
  DALI_TEST_CHECK(array1 == array2);

  END_TEST;
}
