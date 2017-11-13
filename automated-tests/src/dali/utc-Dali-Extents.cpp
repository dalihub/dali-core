/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

namespace
{
/// Compare a uint16_t value with an unsigned int
void DALI_TEST_EQUALS( uint16_t value1, unsigned int value2, const char* location )
{
  ::DALI_TEST_EQUALS< uint16_t >( value1, static_cast< uint16_t >( value2 ), location );
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
  DALI_TEST_EQUALS( extent.start, 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( extent.end, 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( extent.top, 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( extent.bottom, 0u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliExtentsConstructor02(void)
{
  TestApplication application;

  Extents extent( 10u, 20u, 400u, 200u );
  DALI_TEST_EQUALS( extent.start, 10u, TEST_LOCATION );
  DALI_TEST_EQUALS( extent.end, 20u, TEST_LOCATION );
  DALI_TEST_EQUALS( extent.top, 400u, TEST_LOCATION );
  DALI_TEST_EQUALS( extent.bottom, 200u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliExtentsConstructor03(void)
{
  TestApplication application;

  Extents extent( 10u, 20u, 400u, 200u );

  Extents e2 = extent;

  DALI_TEST_EQUALS( e2.start, 10u, TEST_LOCATION );
  DALI_TEST_EQUALS( e2.end, 20u, TEST_LOCATION );
  DALI_TEST_EQUALS( e2.top, 400u, TEST_LOCATION );
  DALI_TEST_EQUALS( e2.bottom, 200u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliExtentsCopyConstructor(void)
{
  TestApplication application;

  Extents extent( 10u, 20u, 400u, 200u );

  Extents extent2( extent );

  DALI_TEST_EQUALS( extent2.start, 10u, TEST_LOCATION );
  DALI_TEST_EQUALS( extent2.end, 20u, TEST_LOCATION );
  DALI_TEST_EQUALS( extent2.top, 400u, TEST_LOCATION );
  DALI_TEST_EQUALS( extent2.bottom, 200u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliExtentsCopyAssignment(void)
{
  TestApplication application;

  Extents extent;

  Extents extent2( 10u, 20u, 400u, 200u );
  extent = extent2;

  DALI_TEST_EQUALS( extent.start, 10u, TEST_LOCATION );
  DALI_TEST_EQUALS( extent.end, 20u, TEST_LOCATION );
  DALI_TEST_EQUALS( extent.top, 400u, TEST_LOCATION );
  DALI_TEST_EQUALS( extent.bottom, 200u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliExtentsAssignP(void)
{
  Extents extent;
  const uint16_t array[] = { 1u, 2u, 3u, 4u };
  extent = (const uint16_t*)array;

  DALI_TEST_EQUALS( extent.start, 1u, TEST_LOCATION);
  DALI_TEST_EQUALS( extent.end, 2u, TEST_LOCATION);
  DALI_TEST_EQUALS( extent.top, 3u, TEST_LOCATION);
  DALI_TEST_EQUALS( extent.bottom, 4u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliExtentsOperatorNotEquals(void)
{
  TestApplication application;

  Extents extent1( 10u, 20u, 200u, 200u );
  Extents extent2( 10u, 120u, 200u, 200u );
  Extents extent3( 10u, 80u, 200u, 200u );

  DALI_TEST_CHECK( extent1 != extent2 );
  DALI_TEST_CHECK( extent1 != extent3 );
  END_TEST;
}

int UtcDaliExtentsOperatorEquals(void)
{
  TestApplication application;

  Extents extent1( 10u, 20u, 200u, 200u );
  Extents extent1p( 10u, 20u, 200u, 200u );

  Extents extent2( 10u, 120u, 200u, 200u );
  Extents extent3( 10u, 80u, 200u, 200u );

  DALI_TEST_CHECK( extent1 == extent1p );
  DALI_TEST_CHECK( extent1 == extent1 );
  DALI_TEST_CHECK( !( extent1 == extent2 ) );
  DALI_TEST_CHECK( !( extent1 == extent3 ) );

  END_TEST;
}

int UtcDaliExtentsOStreamOperatorP(void)
{
  TestApplication application;
  std::ostringstream oss;

  Extents extent( 1u, 2u, 10u, 10u );

  oss << extent;

  std::string expectedOutput = "[1, 2, 10, 10]";

  DALI_TEST_EQUALS( oss.str(), expectedOutput, TEST_LOCATION);
  END_TEST;
}
