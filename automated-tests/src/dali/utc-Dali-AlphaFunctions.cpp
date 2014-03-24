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

void utc_dali_alpha_functions_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_alpha_functions_cleanup(void)
{
  test_return_value = TET_PASS;
}


// Positive test case for a method
int UtcDaliAlphaFunctionsDefault(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( AlphaFunctions::Default( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Default( 0.25f ), 0.25f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Default( 0.5f ), 0.5f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Default( 0.75f ), 0.75f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Default( 1.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliAlphaFunctionsLinear(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( AlphaFunctions::Linear( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Linear( 0.25f ), 0.25f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Linear( 0.5f ), 0.5f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Linear( 0.75f ), 0.75f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Linear( 1.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliAlphaFunctionsReverse(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( AlphaFunctions::Reverse( 0.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Reverse( 0.25f ), 0.75f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Reverse( 0.5f ), 0.5f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Reverse( 0.75f ), 0.25f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Reverse( 1.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliAlphaFunctionsEaseIn(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( AlphaFunctions::EaseIn( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseIn( 0.25f ), 0.015625f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseIn( 0.5f ), 0.125f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseIn( 0.75f ), 0.421875f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseIn( 1.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliAlphaFunctionsEaseOut(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( AlphaFunctions::EaseOut( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOut( 0.25f ), 0.578125f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOut( 0.5f ), 0.875f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOut( 0.75f ), 0.984375f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOut( 1.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliAlphaFunctionsEaseInOut(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( AlphaFunctions::EaseInOut( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInOut( 0.25f ), 0.0625f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInOut( 0.5f ), 0.5f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInOut( 0.75f ), 0.9375f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInOut( 1.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliAlphaFunctionsEaseInSine(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( AlphaFunctions::EaseInSine( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInSine( 0.25f ), 0.07612f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInSine( 0.5f ), 0.292893f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInSine( 0.75f ), 0.617317f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInSine( 1.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliAlphaFunctionsEaseOutSine(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( AlphaFunctions::EaseOutSine( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOutSine( 0.25f ), 0.382683f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOutSine( 0.5f ), 0.707107f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOutSine( 0.75f ), 0.92388f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOutSine( 1.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliAlphaFunctionsEaseInOutSine(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine( 0.25f ), 0.146447f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine( 0.5f ), 0.5f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine( 0.75f ), 0.853553f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine( 1.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliAlphaFunctionsBounce(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( AlphaFunctions::Bounce( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Bounce( 0.25f ), 0.707107f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Bounce( 0.5f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Bounce( 0.75f ), 0.707107f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Bounce( 1.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliAlphaFunctionsBounceBack(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( AlphaFunctions::BounceBack( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::BounceBack( 0.25f ), 0.900316f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::BounceBack( 0.5f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::BounceBack( 0.75f ), -0.300105f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::BounceBack( 1.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliAlphaFunctionsEaseOutBack(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( AlphaFunctions::EaseOutBack( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOutBack( 0.25f ), 0.817410f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOutBack( 0.5f ), 1.087698f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOutBack( 0.75f ), 1.064137f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOutBack( 1.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliAlphaFunctionsSin(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( AlphaFunctions::Sin( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Sin( 0.25f ), 0.5f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Sin( 0.5f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Sin( 0.75f ), 0.5f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Sin( 1.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliAlphaFunctionsSin2x(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( AlphaFunctions::Sin2x( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Sin2x( 0.25f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Sin2x( 0.5f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Sin2x( 0.75f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Sin2x( 1.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliAlphaFunctionsSquare(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( AlphaFunctions::Square( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Square( 0.25f ), 0.0625f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Square( 0.5f ), 0.25f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Square( 0.75f ), 0.5625f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::Square( 1.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliAlphaFunctionsEaseInSine33(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( AlphaFunctions::EaseInSine33( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInSine33( 0.25f ), 0.064146f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInSine33( 0.5f ), 0.255256f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInSine33( 0.75f ), 0.569374f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInSine33( 1.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliAlphaFunctionsEaseOutSine33(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( AlphaFunctions::EaseOutSine33( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOutSine33( 0.25f ), 0.430626f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOutSine33( 0.5f ), 0.744744f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOutSine33( 0.75f ), 0.935854f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOutSine33( 1.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliAlphaFunctionsEaseInOutSineXX(void)
{
  TestApplication application;

  {
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine33( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine33( 0.25f ), 0.239263f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine33( 0.5f ), 0.5f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine33( 0.75f ), 0.760737f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine33( 1.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  }
  {
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine50( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine50( 0.25f ), 0.224156f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine50( 0.5f ), 0.5f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine50( 0.75f ), 0.775844f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine50( 1.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  }
  {
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine60( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine60( 0.25f ), 0.211325f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine60( 0.5f ), 0.5f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine60( 0.75f ), 0.788675f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine60( 1.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  }
  {
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine70( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine70( 0.25f ), 0.194806f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine70( 0.5f ), 0.5f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine70( 0.75f ), 0.805194f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine70( 1.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  }
  {
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine80( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine80( 0.25f ), 0.173648f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine80( 0.5f ), 0.5f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine80( 0.75f ), 0.826352f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine80( 1.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  }
  {
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine90( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine90( 0.25f ), 0.146447f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine90( 0.5f ), 0.5f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine90( 0.75f ), 0.853553f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
    DALI_TEST_EQUALS( AlphaFunctions::EaseInOutSine90( 1.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  }
  END_TEST;
}

// Positive test case for a method
int UtcDaliAlphaFunctionsDoubleEaseInOutSine60(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( AlphaFunctions::DoubleEaseInOutSine60( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::DoubleEaseInOutSine60( 0.25f ), 0.25f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::DoubleEaseInOutSine60( 0.5f ), 0.5f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::DoubleEaseInOutSine60( 0.75f ), 0.75f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::DoubleEaseInOutSine60( 1.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliAlphaFunctionsEaseOutQuint50(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( AlphaFunctions::EaseOutQuint50( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOutQuint50( 0.25f ), 0.386797f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOutQuint50( 0.5f ), 0.692214f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOutQuint50( 0.75f ), 0.905268f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOutQuint50( 1.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliAlphaFunctionsEaseOutQuint80(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( AlphaFunctions::EaseOutQuint80( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOutQuint80( 0.25f ), 0.484010f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOutQuint80( 0.5f ), 0.796937f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOutQuint80( 0.75f ), 0.958765f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseOutQuint80( 1.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliAlphaFunctionsEaseInBack(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( AlphaFunctions::EaseInBack( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInBack( 0.25f ), -0.064137f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInBack( 0.5f ), -0.087698f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInBack( 0.75f ), 0.182590f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInBack( 1.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  END_TEST;
}

// Positive test case for a method
int UtcDaliAlphaFunctionsEaseInOutBack(void)
{
  TestApplication application;

  DALI_TEST_EQUALS( AlphaFunctions::EaseInOutBack( 0.0f ), 0.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInOutBack( 0.25f ), -0.043849f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInOutBack( 0.5f ), 0.5f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInOutBack( 0.75f ), 1.043849f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  DALI_TEST_EQUALS( AlphaFunctions::EaseInOutBack( 1.0f ), 1.0f, Math::MACHINE_EPSILON_10, TEST_LOCATION);
  END_TEST;
}
