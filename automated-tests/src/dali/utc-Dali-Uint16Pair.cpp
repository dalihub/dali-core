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

int UtcDaliUint16PairConstructor01P(void)
{
  Dali::TestApplication testApp;

  Uint16Pair v;

  DALI_TEST_EQUALS(v.GetX(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(v.GetY(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairConstructor02P(void)
{
  Dali::TestApplication testApp;

  Uint16Pair v(10,10);

  DALI_TEST_EQUALS(v.GetX(), 10u, TEST_LOCATION);
  DALI_TEST_EQUALS(v.GetY(), 10u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairCopyConstructor01P(void)
{
  Dali::TestApplication testApp;

  Uint16Pair u(5,5);
  Uint16Pair v(u);
  DALI_TEST_EQUALS(v.GetX(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(v.GetY(), 5u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairGetWidthP(void)
{
  Dali::TestApplication testApp;

  Uint16Pair v(5,5);
  DALI_TEST_EQUALS(v.GetWidth(), 5u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairGetHeightP(void)
{
  Dali::TestApplication testApp;

  Uint16Pair v(5,5);
  DALI_TEST_EQUALS(v.GetHeight(), 5u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairGetXP(void)
{
  Dali::TestApplication testApp;

  Uint16Pair v(5,5);
  DALI_TEST_EQUALS(v.GetX(), 5u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairGetYP(void)
{
  Dali::TestApplication testApp;

  Uint16Pair v(5,5);
  DALI_TEST_EQUALS(v.GetY(), 5u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairEqualsP(void)
{
  Dali::TestApplication testApp;

  Uint16Pair v(5,5);
  Uint16Pair u(5,5);
  DALI_TEST_EQUALS(v == u, true, TEST_LOCATION);

  v = Uint16Pair(5,4);
  u = Uint16Pair(5,5);
  DALI_TEST_EQUALS(v == u, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairNotEqualsP(void)
{
  Dali::TestApplication testApp;

  Uint16Pair v(5,5);
  Uint16Pair u(5,5);
  DALI_TEST_EQUALS(v != u, false, TEST_LOCATION);

  v = Uint16Pair(5,4);
  u = Uint16Pair(5,5);
  DALI_TEST_EQUALS(v != u, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairLessThanP(void)
{
  Dali::TestApplication testApp;

  Uint16Pair u(5,5);
  Uint16Pair v(6,6);
  DALI_TEST_EQUALS(u < v, true, TEST_LOCATION);

  u = Uint16Pair(0,1);
  v = Uint16Pair(1,0);
  DALI_TEST_EQUALS(v < u, true, TEST_LOCATION);

  u = Uint16Pair(1,0);
  v = Uint16Pair(0,1);
  DALI_TEST_EQUALS(v < u, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairGreaterThanP(void)
{
  Dali::TestApplication testApp;

  Uint16Pair u;
  Uint16Pair v;

  u = Uint16Pair(0,1);
  v = Uint16Pair(1,0);
  DALI_TEST_EQUALS(u > v, true, TEST_LOCATION);

  u = Uint16Pair(1,0);
  v = Uint16Pair(0,1);
  DALI_TEST_EQUALS(v > u, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairFromFloatVecP(void)
{
  Dali::TestApplication testApp;

  Dali::Vector2 v2(5.f, 5.f);

  Uint16Pair u = Uint16Pair::FromFloatVec2(v2);
  DALI_TEST_EQUALS(u.GetX(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(u.GetY(), 5u, TEST_LOCATION);

  Dali::Vector3 v3(5.f, 5.f, 5.f);

  u = Uint16Pair::FromFloatVec2(v3);
  DALI_TEST_EQUALS(u.GetX(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(u.GetY(), 5u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairFromFloatArrayP(void)
{
  Dali::TestApplication testApp;

  float array[] = {5.f, 5.f};

  Uint16Pair u = Uint16Pair::FromFloatArray(array);
  DALI_TEST_EQUALS(u.GetX(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(u.GetY(), 5u, TEST_LOCATION);

  END_TEST;
}
