//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <iostream>

#include <stdlib.h>
#include <dali/dali.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

void utc_dali_math_utils_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_math_utils_cleanup(void)
{
  test_return_value = TET_PASS;
}


// Positive test case for a method
int UtcDaliMathUtilsNextPowerOfTwo(void)
{
  Dali::TestApplication testApp;

  DALI_TEST_EQUALS(NextPowerOfTwo(0), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(NextPowerOfTwo(1), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(NextPowerOfTwo(2), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(NextPowerOfTwo(3), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(NextPowerOfTwo(4), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(NextPowerOfTwo(5), 8u, TEST_LOCATION);
  DALI_TEST_EQUALS(NextPowerOfTwo(6), 8u, TEST_LOCATION);
  DALI_TEST_EQUALS(NextPowerOfTwo(7), 8u, TEST_LOCATION);
  DALI_TEST_EQUALS(NextPowerOfTwo(8), 8u, TEST_LOCATION);
  DALI_TEST_EQUALS(NextPowerOfTwo(255), 256u, TEST_LOCATION);
  DALI_TEST_EQUALS(NextPowerOfTwo(256), 256u, TEST_LOCATION);
  DALI_TEST_EQUALS(NextPowerOfTwo(257), 512u, TEST_LOCATION);
  DALI_TEST_EQUALS(NextPowerOfTwo(511), 512u, TEST_LOCATION);
  DALI_TEST_EQUALS(NextPowerOfTwo(512), 512u, TEST_LOCATION);
  DALI_TEST_EQUALS(NextPowerOfTwo(513), 1024u, TEST_LOCATION);
  DALI_TEST_EQUALS(NextPowerOfTwo(768), 1024u, TEST_LOCATION);
  DALI_TEST_EQUALS(NextPowerOfTwo(1023), 1024u, TEST_LOCATION);
  DALI_TEST_EQUALS(NextPowerOfTwo(1024), 1024u, TEST_LOCATION);
  DALI_TEST_EQUALS(NextPowerOfTwo(1025), 2048u, TEST_LOCATION);
  END_TEST;
}


// Positive test case for a method
int UtcDaliMathUtilsIsPowerOfTwo(void)
{
  Dali::TestApplication testApp;

  DALI_TEST_EQUALS(IsPowerOfTwo(0), false, TEST_LOCATION);

  DALI_TEST_EQUALS(IsPowerOfTwo(1), true, TEST_LOCATION);
  DALI_TEST_EQUALS(IsPowerOfTwo(2), true, TEST_LOCATION);
  DALI_TEST_EQUALS(IsPowerOfTwo(3), false, TEST_LOCATION);
  DALI_TEST_EQUALS(IsPowerOfTwo(4), true, TEST_LOCATION);
  DALI_TEST_EQUALS(IsPowerOfTwo(5), false, TEST_LOCATION);
  DALI_TEST_EQUALS(IsPowerOfTwo(6), false, TEST_LOCATION);
  DALI_TEST_EQUALS(IsPowerOfTwo(7), false, TEST_LOCATION);
  DALI_TEST_EQUALS(IsPowerOfTwo(8), true, TEST_LOCATION);
  DALI_TEST_EQUALS(IsPowerOfTwo(255), false, TEST_LOCATION);
  DALI_TEST_EQUALS(IsPowerOfTwo(256), true, TEST_LOCATION);
  DALI_TEST_EQUALS(IsPowerOfTwo(257), false, TEST_LOCATION);
  DALI_TEST_EQUALS(IsPowerOfTwo(511), false, TEST_LOCATION);
  DALI_TEST_EQUALS(IsPowerOfTwo(512), true, TEST_LOCATION);
  DALI_TEST_EQUALS(IsPowerOfTwo(513), false, TEST_LOCATION);
  DALI_TEST_EQUALS(IsPowerOfTwo(768), false, TEST_LOCATION);
  DALI_TEST_EQUALS(IsPowerOfTwo(1023), false, TEST_LOCATION);
  DALI_TEST_EQUALS(IsPowerOfTwo(1024), true, TEST_LOCATION);
  DALI_TEST_EQUALS(IsPowerOfTwo(1025), false, TEST_LOCATION);
  END_TEST;
}



// Positive test case for a method
int UtcDaliMathUtilsGetRangedEpsilon(void)
{
  Dali::TestApplication testApp;

  DALI_TEST_EQUALS(GetRangedEpsilon(0.05f, 0.02f), Dali::Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(0.05f, 0.02f), Dali::Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(0.05f, 0.099f),  Dali::Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(0.099f,  0.02f), Dali::Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(0.05f, 0.5f),  Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(0.99f, 0.5f),  Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(0.99f, 0.98f), Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(1.05f, 0.99f), Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(1.99f, 1.05f), Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);

  DALI_TEST_EQUALS(GetRangedEpsilon(2.0f,  1.99f), Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(0.05f, 2.0f),  Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(1.0f,  3.0f),  Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(9.99f, 0.5f), Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(9.99f, 1.5f), Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(9.99f, 9.99f), Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(9.99f, 10.0f), Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(19.99f, 10.0f), Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);

  DALI_TEST_EQUALS(GetRangedEpsilon(20.0f, 10.0f), Dali::Math::MACHINE_EPSILON_100, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(20.0f, 30.0f), Dali::Math::MACHINE_EPSILON_100, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(80.0f, 90.0f), Dali::Math::MACHINE_EPSILON_100, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(180.0f, 190.0f), Dali::Math::MACHINE_EPSILON_100, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(185.0f, 190.0f), Dali::Math::MACHINE_EPSILON_100, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(199.0f, 199.0f), Dali::Math::MACHINE_EPSILON_100, TEST_LOCATION);

  DALI_TEST_EQUALS(GetRangedEpsilon(200.0f, 190.0f), Dali::Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(0.005f, 1999.0f), Dali::Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(2000.0f, 190.0f), Dali::Math::MACHINE_EPSILON_10000, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(0.005f, 19999.0f), Dali::Math::MACHINE_EPSILON_10000, TEST_LOCATION);

  DALI_TEST_EQUALS(GetRangedEpsilon(1e07f, 0.99e09f), Dali::Math::MACHINE_EPSILON_10000, TEST_LOCATION);


  DALI_TEST_EQUALS(GetRangedEpsilon(-0.05f, -0.02f), Dali::Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(-0.05f, -0.02f), Dali::Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(-0.05f, -0.099f),  Dali::Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(-0.099f, - 0.02f), Dali::Math::MACHINE_EPSILON_0, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(-0.05f, -0.5f),  Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(-0.99f, -0.5f),  Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(-0.99f, -0.98f), Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(-1.05f, -0.99f), Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(-1.99f, -1.05f), Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);

  DALI_TEST_EQUALS(GetRangedEpsilon(-2.0f, - 1.99f), Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(-0.05f, -2.0f),  Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(-1.0f, - 3.0f),  Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(-9.99f, -0.5f), Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(-9.99f, -1.5f), Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(-9.99f, -9.99f), Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(-9.99f, -10.0f), Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(-19.99f, -10.0f), Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);

  DALI_TEST_EQUALS(GetRangedEpsilon(-20.0f, -10.0f), Dali::Math::MACHINE_EPSILON_100, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(-20.0f, -30.0f), Dali::Math::MACHINE_EPSILON_100, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(-80.0f, -90.0f), Dali::Math::MACHINE_EPSILON_100, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(-180.0f, -190.0f), Dali::Math::MACHINE_EPSILON_100, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(-185.0f, -190.0f), Dali::Math::MACHINE_EPSILON_100, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(-199.0f, -199.0f), Dali::Math::MACHINE_EPSILON_100, TEST_LOCATION);

  DALI_TEST_EQUALS(GetRangedEpsilon(-200.0f, -190.0f), Dali::Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(-0.005f, -1999.0f), Dali::Math::MACHINE_EPSILON_1000, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(-2000.0f, -190.0f), Dali::Math::MACHINE_EPSILON_10000, TEST_LOCATION);
  DALI_TEST_EQUALS(GetRangedEpsilon(-0.005f, -19999.0f), Dali::Math::MACHINE_EPSILON_10000, TEST_LOCATION);

  DALI_TEST_EQUALS(GetRangedEpsilon(-1e07f, -0.99e09f), Dali::Math::MACHINE_EPSILON_10000, TEST_LOCATION);


  END_TEST;
}

// Positive test case for a method
int UtcDaliMathUtilsRound(void)
{
  Dali::TestApplication testApp;

  DALI_TEST_EQUALS(Round(1.00001, 4), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Round(0.99999f, 4), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Round(-1.00001, 4), -1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Round(-0.99999f, 4), -1.0f, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliMathUtilsClamp(void)
{
  Dali::TestApplication testApp;

  //floats
  DALI_TEST_EQUALS(Clamp(-1.0f, 0.0f, 1.0f), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Clamp(0.0f, -1.0f, 1.0f), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Clamp(1.0f, 0.0f, 1.0f), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Clamp(2.0f, 0.0f, 1.0f), 1.0f, TEST_LOCATION);

  // integers
  DALI_TEST_EQUALS(Clamp(-10, 0, 10), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(Clamp(0, -10, 10), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(Clamp(20, 0, 10), 10, TEST_LOCATION);

  float value=-10.0f, min=-2.0f, max=4.0f;
  ClampInPlace(value, min, max);
  DALI_TEST_EQUALS(value, min, 0.001, TEST_LOCATION);

  value = 10.0f;
  ClampInPlace(value, min, max);
  DALI_TEST_EQUALS(value, max, 0.001, TEST_LOCATION);

  value = 3.0f;
  ClampInPlace(value, min, max);
  DALI_TEST_EQUALS(value, 3.0f, 0.001, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliMathUtilsWrapInDomain(void)
{
  Dali::TestApplication testApp;

  DALI_TEST_EQUALS(WrapInDomain(0.0f, 0.0f, 0.0f), 0.0f, Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(WrapInDomain(-5.0f, 0.0f, 0.0f), 0.0f, Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(WrapInDomain(5.0f, 0.0f, 0.0f), 0.0f, Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(WrapInDomain(0.0f, 0.0f, 10.0f), 0.0f, Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(WrapInDomain(-5.0f, 0.0f, 10.0f), 5.0f, Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(WrapInDomain(5.0f, 0.0f, 10.0f), 5.0f, Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(WrapInDomain(-2.5f, 0.0f, 10.0f), 7.5f, Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(WrapInDomain(2.5f, 0.0f, 10.0f), 2.5f, Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(WrapInDomain(2.5f, 0.0f, 1.0f), 0.5f, Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(WrapInDomain(2.5f, -2.0f, -1.0f), -1.5f, Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);
  DALI_TEST_EQUALS(WrapInDomain(-2.9f, -2.0f, -1.0f), -1.9f, Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS(WrapInDomain(-1.1f, -2.0f, -1.0f), -1.1f, Dali::Math::MACHINE_EPSILON_1, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliMathUtilsShortestDistanceInDomain(void)
{
  Dali::TestApplication testApp;

  DALI_TEST_EQUALS(ShortestDistanceInDomain(1.0f, 8.0f, 0.0f, 10.0f), -3.0f, Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS(ShortestDistanceInDomain(5.0f, 8.0f, 0.0f, 10.0f), 3.0f, Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS(ShortestDistanceInDomain(5.0f, 8.0f, 4.0f, 9.0f), -2.0f, Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS(ShortestDistanceInDomain(8.0f, 5.0f, 4.0f, 9.0f), 2.0f, Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS(ShortestDistanceInDomain(0.65f, 0.1f, -1.0f, 1.0f), -0.55f, Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS(ShortestDistanceInDomain(0.95f, -0.9f, -1.0f, 1.0f), 0.15f, Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS(ShortestDistanceInDomain(0.0f, -0.9f, -1.0f, 1.0f), -0.9f, Dali::Math::MACHINE_EPSILON_10, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMathUtilsEquals(void)
{
  float v=0.0f;

  DALI_TEST_CHECK(EqualsZero(v));

  v  = Math::PI;
  v -= (Math::PI_2 * 2.0f);
  DALI_TEST_CHECK(EqualsZero(v));

  float w=100.0f;
  float x=w+1e-8f;
  DALI_TEST_CHECK( Equals(w, x, GetRangedEpsilon( w, x )) );
  END_TEST;
}
