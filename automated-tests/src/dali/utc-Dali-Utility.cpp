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

#include <dali-test-suite-utils.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>

using namespace Dali;

void utc_dali_utility_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_utility_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliUtilityMinMaxP(void)
{
  Dali::TestApplication testApp;

  // floats
  DALI_TEST_EQUALS(Min(1.0f, 2.0f), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Min(2.0f, 1.0f), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Min(-1.0f, 1.0f), -1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Min(1.0f, -1.0f), -1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Min(0.0f, 0.0f), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Min(-5.5f, -3.3f), -5.5f, TEST_LOCATION);

  DALI_TEST_EQUALS(Max(1.0f, 2.0f), 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Max(2.0f, 1.0f), 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Max(-1.0f, 1.0f), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Max(1.0f, -1.0f), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Max(0.0f, 0.0f), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Max(-5.5f, -3.3f), -3.3f, TEST_LOCATION);

  // integers
  DALI_TEST_EQUALS(Min(1, 2), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(Min(2, 1), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(Min(-1, 1), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(Min(1, -1), -1, TEST_LOCATION);
  DALI_TEST_EQUALS(Min(0, 0), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(Min(-10, -5), -10, TEST_LOCATION);

  DALI_TEST_EQUALS(Max(1, 2), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(Max(2, 1), 2, TEST_LOCATION);
  DALI_TEST_EQUALS(Max(-1, 1), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(Max(1, -1), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(Max(0, 0), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(Max(-10, -5), -5, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUtilitySwapP01(void)
{
  Dali::TestApplication testApp;

  // floats
  float f1 = 1.0f, f2 = 2.0f;
  Dali::Swap(f1, f2);
  DALI_TEST_EQUALS(f1, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(f2, 1.0f, TEST_LOCATION);

  f1 = -5.5f;
  f2 = -3.3f;
  Dali::Swap(f1, f2);
  DALI_TEST_EQUALS(f1, -3.3f, TEST_LOCATION);
  DALI_TEST_EQUALS(f2, -5.5f, TEST_LOCATION);

  f1 = 0.0f;
  f2 = 0.0f;
  Dali::Swap(f1, f2);
  DALI_TEST_EQUALS(f1, 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(f2, 0.0f, TEST_LOCATION);

  // integers
  int i1 = 1, i2 = 2;
  Dali::Swap(i1, i2);
  DALI_TEST_EQUALS(i1, 2, TEST_LOCATION);
  DALI_TEST_EQUALS(i2, 1, TEST_LOCATION);

  i1 = -10;
  i2 = -5;
  Dali::Swap(i1, i2);
  DALI_TEST_EQUALS(i1, -5, TEST_LOCATION);
  DALI_TEST_EQUALS(i2, -10, TEST_LOCATION);

  i1 = 0;
  i2 = 0;
  Dali::Swap(i1, i2);
  DALI_TEST_EQUALS(i1, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(i2, 0, TEST_LOCATION);

  // unsigned integers
  uint32_t u1 = 10u, u2 = 20u;
  Dali::Swap(u1, u2);
  DALI_TEST_EQUALS(u1, 20u, TEST_LOCATION);
  DALI_TEST_EQUALS(u2, 10u, TEST_LOCATION);

  // double
  double d1 = 1.5, d2 = 2.5;
  Dali::Swap(d1, d2);
  DALI_TEST_EQUALS(d1, 2.5, TEST_LOCATION);
  DALI_TEST_EQUALS(d2, 1.5, TEST_LOCATION);

  END_TEST;
}

int UtcDaliUtilitySwapP02(void)
{
  Dali::TestApplication testApp;

  Dali::Actor origin1 = Dali::Actor::New();
  Dali::Actor origin2 = Dali::Actor::New();

  Dali::Actor copy1 = origin1;
  Dali::Actor copy2 = origin2;
  DALI_TEST_EQUALS(origin1, copy1, TEST_LOCATION);
  DALI_TEST_EQUALS(origin2, copy2, TEST_LOCATION);

  Dali::Swap(copy1, copy2);
  DALI_TEST_EQUALS(origin2, copy1, TEST_LOCATION);
  DALI_TEST_EQUALS(origin1, copy2, TEST_LOCATION);

  Dali::Swap(copy1, copy2);
  DALI_TEST_EQUALS(origin1, copy1, TEST_LOCATION);
  DALI_TEST_EQUALS(origin2, copy2, TEST_LOCATION);

  END_TEST;
}

/*

// TODO : Need to pass below UTC when we implement Dali::Move

namespace
{
enum FunctionType
{
  CONSTRUCTOR,
  DESTRUCTOR,
  COPY_CONSTRUCTOR,
  MOVE_CONSTRUCTOR,
  COPY_ASSIGN,
  MOVE_ASSIGN,

  MAX_COUNT
};
uint32_t gFunctionCall[FunctionType::MAX_COUNT];
void     ClearFunctionCalls()
{
  memset(&gFunctionCall[0], 0, sizeof(gFunctionCall));
}

void TestFunctionCall(FunctionType type, uint32_t expect, const char* location)
{
  DALI_TEST_EQUALS(gFunctionCall[type], expect, location);
}

void TestFunctionCalls(uint32_t expect0, uint32_t expect1, uint32_t expect2, uint32_t expect3, uint32_t expect4, uint32_t expect5, const char* location)
{
  tet_printf("expect : %d %d %d %d %d %d\n", expect0, expect1, expect2, expect3, expect4, expect5);
  tet_printf("real   : %d %d %d %d %d %d\n", gFunctionCall[FunctionType::CONSTRUCTOR], gFunctionCall[FunctionType::DESTRUCTOR], gFunctionCall[FunctionType::COPY_CONSTRUCTOR], gFunctionCall[FunctionType::MOVE_CONSTRUCTOR], gFunctionCall[FunctionType::COPY_ASSIGN], gFunctionCall[FunctionType::MOVE_ASSIGN]);

  TestFunctionCall(FunctionType::CONSTRUCTOR, expect0, location);
  TestFunctionCall(FunctionType::DESTRUCTOR, expect1, location);
  TestFunctionCall(FunctionType::COPY_CONSTRUCTOR, expect2, location);
  TestFunctionCall(FunctionType::MOVE_CONSTRUCTOR, expect3, location);
  TestFunctionCall(FunctionType::COPY_ASSIGN, expect4, location);
  TestFunctionCall(FunctionType::MOVE_ASSIGN, expect5, location);
}

struct A
{
public:
  A()
  : val()
  {
    gFunctionCall[FunctionType::CONSTRUCTOR]++;
  }
  ~A()
  {
    gFunctionCall[FunctionType::DESTRUCTOR]++;
  }
  A(const A& rhs)
  : val(rhs.val)
  {
    gFunctionCall[FunctionType::COPY_CONSTRUCTOR]++;
  }
  A(A&& rhs) noexcept
  : val(std::move(rhs.val))
  {
    gFunctionCall[FunctionType::MOVE_CONSTRUCTOR]++;
  }

  A& operator=(const A& rhs)
  {
    val = rhs.val;
    gFunctionCall[FunctionType::COPY_ASSIGN]++;
    return *this;
  }

  A& operator=(A&& rhs) noexcept
  {
    if(this != &rhs)
    {
      val = std::move(rhs.val);
      gFunctionCall[FunctionType::MOVE_ASSIGN]++;
    }
    return *this;
  }

  std::string val; ///< Some non-trivial member value.
};
} //namespace

int UtcDaliUtilityMoveP01( void)
{
  Dali::TestApplication testApp;

  ClearFunctionCalls();

  {
    A a1;
    TestFunctionCalls(1, 0, 0, 0, 0, 0, TEST_LOCATION);
    A a2(Dali::Move(a1));
    TestFunctionCalls(1, 0, 0, 1, 0, 0, TEST_LOCATION);
    A a3;
    TestFunctionCalls(2, 0, 0, 1, 0, 0, TEST_LOCATION);
    a3 = Dali::Move(a2);
    TestFunctionCalls(2, 0, 0, 1, 0, 1, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliUtilityMoveP02( void)
{
  Dali::TestApplication testApp;

  ClearFunctionCalls();

  // Check const A& call copy.

  {
    const A a1;
    TestFunctionCalls(1, 0, 0, 0, 0, 0, TEST_LOCATION);
    A a2(Dali::Move(a1));
    TestFunctionCalls(1, 0, 1, 0, 0, 0, TEST_LOCATION);
    A a3;
    TestFunctionCalls(2, 0, 1, 0, 0, 0, TEST_LOCATION);
    a3 = Dali::Move(a2);
    TestFunctionCalls(2, 0, 1, 0, 0, 1, TEST_LOCATION);
    a3 = Dali::Move(a1);
    TestFunctionCalls(2, 0, 1, 0, 1, 1, TEST_LOCATION);
  }

  END_TEST;
}

*/