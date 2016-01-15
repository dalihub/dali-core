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
  Uint16Pair v;

  DALI_TEST_EQUALS( v.GetX(), 0u, TEST_LOCATION );
  DALI_TEST_EQUALS( v.GetY(), 0u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliUint16PairConstructor02P(void)
{
  Uint16Pair v( 10u, 10u );

  DALI_TEST_EQUALS( v.GetX(), 10u, TEST_LOCATION );
  DALI_TEST_EQUALS( v.GetY(), 10u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliUint16PairCopyConstructor01P(void)
{
  Uint16Pair u( 5u, 5u );
  Uint16Pair v( u );
  DALI_TEST_EQUALS( v.GetX(), 5u, TEST_LOCATION );
  DALI_TEST_EQUALS( v.GetY(), 5u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliUint16PairGetWidthP(void)
{
  Uint16Pair v( 5u ,5u );
  DALI_TEST_EQUALS( v.GetWidth(), 5u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliUint16PairGetHeightP(void)
{
  Uint16Pair v( 5u, 5u );
  DALI_TEST_EQUALS( v.GetHeight(), 5u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliUint16PairGetXP(void)
{
  Uint16Pair v( 5u, 5u );
  DALI_TEST_EQUALS( v.GetX(), 5u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliUint16PairGetYP(void)
{
  Uint16Pair v( 5u, 5u );
  DALI_TEST_EQUALS( v.GetY(), 5u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliUint16PairSetXP(void)
{
  Uint16Pair v( 5u, 5u );
  DALI_TEST_EQUALS( v.GetX(), 5u, TEST_LOCATION );
  v.SetX( 10u );
  DALI_TEST_EQUALS( v.GetX(), 10u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliUint16PairSetWidthP(void)
{
  Uint16Pair v( 5u, 5u );
  DALI_TEST_EQUALS( v.GetWidth(), 5u, TEST_LOCATION );
  v.SetWidth( 10u );
  DALI_TEST_EQUALS( v.GetWidth(), 10u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliUint16PairSetYP(void)
{
  Uint16Pair v( 5u, 5u );
  DALI_TEST_EQUALS( v.GetY(), 5u, TEST_LOCATION );
  v.SetY( 10u );
  DALI_TEST_EQUALS( v.GetY(), 10u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliUint16PairSetHeightP(void)
{
  Uint16Pair v( 5u, 5u );
  DALI_TEST_EQUALS( v.GetHeight(), 5u, TEST_LOCATION );
  v.SetHeight( 10u );
  DALI_TEST_EQUALS( v.GetHeight(), 10u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliUint16PairEqualsP(void)
{
  Uint16Pair v( 5u, 5u );
  Uint16Pair u( 5u, 5u );
  DALI_TEST_EQUALS( v == u, true, TEST_LOCATION );

  v = Uint16Pair( 5u, 4u );
  u = Uint16Pair( 5u, 5u );
  DALI_TEST_EQUALS( v == u, false, TEST_LOCATION );

  END_TEST;
}

int UtcDaliUint16PairNotEqualsP(void)
{
  Uint16Pair v( 5u, 5u );
  Uint16Pair u( 5u, 5u );
  DALI_TEST_EQUALS( v != u, false, TEST_LOCATION );

  v = Uint16Pair( 5u, 4u );
  u = Uint16Pair( 5u, 5u );
  DALI_TEST_EQUALS( v != u, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliUint16PairLessThanP(void)
{
  Uint16Pair u( 5u, 5u );
  Uint16Pair v( 6u, 6u );
  DALI_TEST_EQUALS( u < v, true, TEST_LOCATION );

  u = Uint16Pair( 0u, 1u );
  v = Uint16Pair( 1u, 0u );
  DALI_TEST_EQUALS( v < u, true, TEST_LOCATION );

  u = Uint16Pair( 1u, 0u );
  v = Uint16Pair( 0u, 1u );
  DALI_TEST_EQUALS( v < u, false, TEST_LOCATION );

  END_TEST;
}

int UtcDaliUint16PairGreaterThanP(void)
{
  Uint16Pair u;
  Uint16Pair v;

  u = Uint16Pair( 0u, 1u );
  v = Uint16Pair( 1u, 0u );
  DALI_TEST_EQUALS( u > v, true, TEST_LOCATION );

  u = Uint16Pair( 1u, 0u );
  v = Uint16Pair( 0u, 1u );
  DALI_TEST_EQUALS( v > u, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliUint16PairFromFloatVecP(void)
{
  Dali::Vector2 v2( 5.f, 5.f );

  Uint16Pair u = Uint16Pair::FromFloatVec2( v2 );
  DALI_TEST_EQUALS( u.GetX(), 5u, TEST_LOCATION );
  DALI_TEST_EQUALS( u.GetY(), 5u, TEST_LOCATION );

  Dali::Vector3 v3( 5.f, 5.f, 5.f );

  u = Uint16Pair::FromFloatVec2( v3 );
  DALI_TEST_EQUALS( u.GetX(), 5u, TEST_LOCATION );
  DALI_TEST_EQUALS( u.GetY(), 5u, TEST_LOCATION );

  END_TEST;
}

int UtcDaliUint16PairFromFloatArrayP(void)
{
  float array[] = { 5.f, 5.f };

  Uint16Pair u = Uint16Pair::FromFloatArray( array );
  DALI_TEST_EQUALS( u.GetX(), 5u, TEST_LOCATION );
  DALI_TEST_EQUALS( u.GetY(), 5u, TEST_LOCATION );

  END_TEST;
}
