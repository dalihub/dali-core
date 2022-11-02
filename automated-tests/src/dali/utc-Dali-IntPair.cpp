/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
/// Compare a uint16_t value with an unsigned int
void DALI_TEST_EQUALS(uint16_t value1, unsigned int value2, const char* location)
{
  ::DALI_TEST_EQUALS<uint16_t>(value1, static_cast<uint16_t>(value2), location);
}
} // unnamed namespace

// class Uint16Pair

int UtcDaliUint16PairConstructor01P(void)
{
  Uint16Pair v;

  DALI_TEST_EQUALS(v.GetX(), 0u, TEST_LOCATION);
  DALI_TEST_EQUALS(v.GetY(), 0u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairConstructor02P(void)
{
  Uint16Pair v(10u, 10u);

  DALI_TEST_EQUALS(v.GetX(), 10u, TEST_LOCATION);
  DALI_TEST_EQUALS(v.GetY(), 10u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairCopyConstructor(void)
{
  Uint16Pair u(5u, 5u);
  Uint16Pair v(u);
  DALI_TEST_EQUALS(v.GetX(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(v.GetY(), 5u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairMoveConstructor(void)
{
  Uint16Pair u(5u, 5u);
  Uint16Pair v = std::move(u);
  DALI_TEST_EQUALS(v.GetX(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(v.GetY(), 5u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairCopyAssignment(void)
{
  Uint16Pair u(5u, 5u);
  Uint16Pair v;
  v = u;
  DALI_TEST_EQUALS(v.GetX(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(v.GetY(), 5u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairMoveAssignment(void)
{
  Uint16Pair u(5u, 5u);
  Uint16Pair v;
  v = std::move(u);
  DALI_TEST_EQUALS(v.GetX(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(v.GetY(), 5u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairGetWidthP(void)
{
  Uint16Pair v(5u, 5u);
  DALI_TEST_EQUALS(v.GetWidth(), 5u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairGetHeightP(void)
{
  Uint16Pair v(5u, 5u);
  DALI_TEST_EQUALS(v.GetHeight(), 5u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairGetXP(void)
{
  Uint16Pair v(5u, 5u);
  DALI_TEST_EQUALS(v.GetX(), 5u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairGetYP(void)
{
  Uint16Pair v(5u, 5u);
  DALI_TEST_EQUALS(v.GetY(), 5u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairSetXP(void)
{
  Uint16Pair v(5u, 5u);
  DALI_TEST_EQUALS(v.GetX(), 5u, TEST_LOCATION);
  v.SetX(10u);
  DALI_TEST_EQUALS(v.GetX(), 10u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairSetWidthP(void)
{
  Uint16Pair v(5u, 5u);
  DALI_TEST_EQUALS(v.GetWidth(), 5u, TEST_LOCATION);
  v.SetWidth(10u);
  DALI_TEST_EQUALS(v.GetWidth(), 10u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairSetYP(void)
{
  Uint16Pair v(5u, 5u);
  DALI_TEST_EQUALS(v.GetY(), 5u, TEST_LOCATION);
  v.SetY(10u);
  DALI_TEST_EQUALS(v.GetY(), 10u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairSetHeightP(void)
{
  Uint16Pair v(5u, 5u);
  DALI_TEST_EQUALS(v.GetHeight(), 5u, TEST_LOCATION);
  v.SetHeight(10u);
  DALI_TEST_EQUALS(v.GetHeight(), 10u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairEqualsP(void)
{
  Uint16Pair v(5u, 5u);
  Uint16Pair u(5u, 5u);
  DALI_TEST_EQUALS(v == u, true, TEST_LOCATION);

  v = Uint16Pair(5u, 4u);
  u = Uint16Pair(5u, 5u);
  DALI_TEST_EQUALS(v == u, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairNotEqualsP(void)
{
  Uint16Pair v(5u, 5u);
  Uint16Pair u(5u, 5u);
  DALI_TEST_EQUALS(v != u, false, TEST_LOCATION);

  v = Uint16Pair(5u, 4u);
  u = Uint16Pair(5u, 5u);
  DALI_TEST_EQUALS(v != u, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairLessThanP(void)
{
  Uint16Pair u(5u, 5u);
  Uint16Pair v(6u, 6u);
  DALI_TEST_EQUALS(u < v, true, TEST_LOCATION);

  u = Uint16Pair(0u, 1u);
  v = Uint16Pair(1u, 0u);
  DALI_TEST_EQUALS(v < u, true, TEST_LOCATION);

  u = Uint16Pair(1u, 0u);
  v = Uint16Pair(0u, 1u);
  DALI_TEST_EQUALS(v < u, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairGreaterThanP(void)
{
  Uint16Pair u;
  Uint16Pair v;

  u = Uint16Pair(0u, 1u);
  v = Uint16Pair(1u, 0u);
  DALI_TEST_EQUALS(u > v, true, TEST_LOCATION);

  u = Uint16Pair(1u, 0u);
  v = Uint16Pair(0u, 1u);
  DALI_TEST_EQUALS(v > u, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUint16PairFromFloatVecP(void)
{
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
  float array[] = {5.f, 5.f};

  Uint16Pair u = Uint16Pair::FromFloatArray(array);
  DALI_TEST_EQUALS(u.GetX(), 5u, TEST_LOCATION);
  DALI_TEST_EQUALS(u.GetY(), 5u, TEST_LOCATION);

  END_TEST;
}

// class Int32Pair

int UtcDaliInt32PairConstructor01P(void)
{
  Int32Pair v;

  DALI_TEST_EQUALS(v.GetX(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(v.GetY(), 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInt32PairConstructor02P(void)
{
  Int32Pair v(-10, 20);

  DALI_TEST_EQUALS(v.GetX(), -10, TEST_LOCATION);
  DALI_TEST_EQUALS(v.GetY(), 20, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInt32PairCopyConstructor(void)
{
  Int32Pair u(-5, -10);
  Int32Pair v(u);
  DALI_TEST_EQUALS(v.GetX(), -5, TEST_LOCATION);
  DALI_TEST_EQUALS(v.GetY(), -10, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInt32PairMoveConstructor(void)
{
  Int32Pair u(5, -10);
  Int32Pair v = std::move(u);
  DALI_TEST_EQUALS(v.GetX(), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(v.GetY(), -10, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInt32PairCopyAssignment(void)
{
  Int32Pair u(5, 10);
  Int32Pair v;
  v = u;
  DALI_TEST_EQUALS(v.GetX(), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(v.GetY(), 10, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInt32PairMoveAssignment(void)
{
  Int32Pair u(5, 10);
  Int32Pair v;
  v = std::move(u);
  DALI_TEST_EQUALS(v.GetX(), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(v.GetY(), 10, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInt32PairGetXP(void)
{
  Int32Pair v(5, 10);
  DALI_TEST_EQUALS(v.GetX(), 5, TEST_LOCATION);
  DALI_TEST_EQUALS(v.GetWidth(), 5, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInt32PairGetYP(void)
{
  Int32Pair v(5, 10);
  DALI_TEST_EQUALS(v.GetY(), 10, TEST_LOCATION);
  DALI_TEST_EQUALS(v.GetHeight(), 10, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInt32PairSetXP(void)
{
  Int32Pair v(5, 10);
  DALI_TEST_EQUALS(v.GetX(), 5, TEST_LOCATION);
  v.SetX(10);
  DALI_TEST_EQUALS(v.GetX(), 10, TEST_LOCATION);
  v.SetWidth(65539);
  DALI_TEST_EQUALS(v.GetWidth(), 65539, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInt32PairSetYP(void)
{
  Int32Pair v(5, 10);
  DALI_TEST_EQUALS(v.GetY(), 10, TEST_LOCATION);
  v.SetY(-5);
  DALI_TEST_EQUALS(v.GetY(), -5, TEST_LOCATION);
  v.SetHeight(65537);
  DALI_TEST_EQUALS(v.GetHeight(), 65537, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInt32PairEqualsP(void)
{
  Int32Pair v(5, 5);
  Int32Pair u(5, 5);
  DALI_TEST_EQUALS(v == u, true, TEST_LOCATION);

  v = Int32Pair(5, 4);
  u = Int32Pair(5, 5);
  DALI_TEST_EQUALS(v == u, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliInt32PairNotEqualsP(void)
{
  Int32Pair v(5, 5);
  Int32Pair u(5, 5);
  DALI_TEST_EQUALS(v != u, false, TEST_LOCATION);

  v = Int32Pair(5, 4);
  u = Int32Pair(5, 5);
  DALI_TEST_EQUALS(v != u, true, TEST_LOCATION);

  END_TEST;
}
