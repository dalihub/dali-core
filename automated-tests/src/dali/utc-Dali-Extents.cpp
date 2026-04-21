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

using namespace Dali;

namespace
{
/// Compare a int16_t value with an int
void DALI_TEST_EQUALS(int16_t value1, int value2, const char* location)
{
  ::DALI_TEST_EQUALS<int16_t>(value1, static_cast<int16_t>(value2), location);
}
} // unnamed namespace

void utc_dali_extents_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_extents_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliExtentsConstructor01(void)
{
  TestApplication application;

  Extents extent;
  DALI_TEST_EQUALS(extent.start, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(extent.end, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(extent.top, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(extent.bottom, 0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliExtentsConstructor02(void)
{
  TestApplication application;

  Extents extent(10, -20, 400, -200);
  DALI_TEST_EQUALS(extent.start, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(extent.end, -20, TEST_LOCATION);
  DALI_TEST_EQUALS(extent.top, 400, TEST_LOCATION);
  DALI_TEST_EQUALS(extent.bottom, -200, TEST_LOCATION);
  END_TEST;
}

int UtcDaliExtentsConstructor03(void)
{
  TestApplication application;

  Extents extent(10, -20, 400, -200);

  Extents e2 = extent;

  DALI_TEST_EQUALS(e2.start, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(e2.end, -20, TEST_LOCATION);
  DALI_TEST_EQUALS(e2.top, 400, TEST_LOCATION);
  DALI_TEST_EQUALS(e2.bottom, -200, TEST_LOCATION);
  END_TEST;
}

int UtcDaliExtentsCopyConstructor(void)
{
  TestApplication application;

  Extents extent(10, -20, 400, -200);

  Extents extent2(extent);

  DALI_TEST_EQUALS(extent2.start, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(extent2.end, -20, TEST_LOCATION);
  DALI_TEST_EQUALS(extent2.top, 400, TEST_LOCATION);
  DALI_TEST_EQUALS(extent2.bottom, -200, TEST_LOCATION);
  END_TEST;
}

int UtcDaliExtentsCopyAssignment(void)
{
  TestApplication application;

  Extents extent;

  Extents extent2(10, -20, 400, -200);
  extent = extent2;

  DALI_TEST_EQUALS(extent.start, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(extent.end, -20, TEST_LOCATION);
  DALI_TEST_EQUALS(extent.top, 400, TEST_LOCATION);
  DALI_TEST_EQUALS(extent.bottom, -200, TEST_LOCATION);
  END_TEST;
}

int UtcDaliExtentsMoveConstructor(void)
{
  TestApplication application;

  Extents extent(10, -20, 400, -200);

  Extents extent2(std::move(extent));

  DALI_TEST_EQUALS(extent2.start, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(extent2.end, -20, TEST_LOCATION);
  DALI_TEST_EQUALS(extent2.top, 400, TEST_LOCATION);
  DALI_TEST_EQUALS(extent2.bottom, -200, TEST_LOCATION);
  END_TEST;
}

int UtcDaliExtentsMoveAssignment(void)
{
  TestApplication application;

  Extents extent;

  Extents extent2(10, -20, 400, -200);
  extent = std::move(extent2);

  DALI_TEST_EQUALS(extent.start, 10, TEST_LOCATION);
  DALI_TEST_EQUALS(extent.end, -20, TEST_LOCATION);
  DALI_TEST_EQUALS(extent.top, 400, TEST_LOCATION);
  DALI_TEST_EQUALS(extent.bottom, -200, TEST_LOCATION);
  END_TEST;
}

int UtcDaliExtentsAssignP(void)
{
  Extents       extent;
  const int16_t array[] = {1, 2, 3, 4};
  extent                = (const int16_t*)array;

  DALI_TEST_EQUALS(extent.start, 1, TEST_LOCATION);
  DALI_TEST_EQUALS(extent.end, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(extent.top, 3, TEST_LOCATION);
  DALI_TEST_EQUALS(extent.bottom, 4, TEST_LOCATION);
  END_TEST;
}

int UtcDaliExtentsOperatorNotEquals(void)
{
  TestApplication application;

  Extents extent1(10, 20, 200, 200);
  Extents extent2(10, 120, 200, 200);
  Extents extent3(10, 80, 200, 200);

  DALI_TEST_CHECK(extent1 != extent2);
  DALI_TEST_CHECK(extent1 != extent3);
  END_TEST;
}

int UtcDaliExtentsOperatorEquals(void)
{
  TestApplication application;

  Extents extent1(10, 20, 200, 200);
  Extents extent1p(10, 20, 200, 200);

  Extents extent2(10, 120, 200, 200);
  Extents extent3(10, 80, 200, 200);

  DALI_TEST_CHECK(extent1 == extent1p);
  DALI_TEST_CHECK(extent1 == extent1);
  DALI_TEST_CHECK(!(extent1 == extent2));
  DALI_TEST_CHECK(!(extent1 == extent3));

  END_TEST;
}

int UtcDaliExtentsOStreamOperatorP(void)
{
  TestApplication    application;
  std::ostringstream oss;

  Extents extent(1, 2, 10, 10);

  oss << extent;

  std::string expectedOutput = "[1, 2, 10, 10]";

  DALI_TEST_EQUALS(oss.str(), expectedOutput, TEST_LOCATION);
  END_TEST;
}
