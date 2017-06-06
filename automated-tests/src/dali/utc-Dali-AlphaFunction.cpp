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

float customAlphaFunction( float progress )
{
  return progress;
}

void utc_dali_alpha_function_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_alpha_function_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliAlphaFunctionDefaultConstructorP(void)
{
  TestApplication application;
  AlphaFunction alpha;

  //Should return the default alpha function
  DALI_TEST_EQUALS( alpha.GetBuiltinFunction(), AlphaFunction::DEFAULT, TEST_LOCATION);

  //Check the mode is BUILTIN_FUNCTION
  DALI_TEST_EQUALS( alpha.GetMode(), AlphaFunction::BUILTIN_FUNCTION, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAlphaFunctionConstructorFromBuiltinP(void)
{
  TestApplication application;

  //Construct the alpha function with a built-in function
  AlphaFunction alpha( AlphaFunction::EASE_IN_OUT);

  //Check if the built-in alpha function is EASE_IN_OUT
  DALI_TEST_EQUALS( alpha.GetBuiltinFunction(), AlphaFunction::EASE_IN_OUT, TEST_LOCATION);

  //Check the mode is BUILTIN_FUNCTION
  DALI_TEST_EQUALS( alpha.GetMode(), AlphaFunction::BUILTIN_FUNCTION, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAlphaFunctionConstructorFromFunctionPointerdP(void)
{
  TestApplication application;

  //Construct the alpha function with a function pointer
  AlphaFunction alpha( &customAlphaFunction );

  //Check that the custom function points to the custom alpha function
  DALI_TEST_EQUALS( alpha.GetCustomFunction(), &customAlphaFunction, TEST_LOCATION);

  //Check the mode is CUSTOM_FUNCTION
  DALI_TEST_EQUALS( alpha.GetMode(), AlphaFunction::CUSTOM_FUNCTION, TEST_LOCATION);


  END_TEST;
}

int UtcDaliAlphaFunctionConstructorFromControlPointsP(void)
{
  TestApplication application;

  //Construct the alpha function with two control points
  Vector2 controlPoint0 = Vector2(0.0f,1.0f);
  Vector2 controlPoint1 = Vector2(1.0f,0.0f);
  AlphaFunction alpha(controlPoint0,controlPoint1);

  //Check if the control points have the correct value
  Vector4 controlPoints = alpha.GetBezierControlPoints();
  DALI_TEST_EQUALS( Vector2(controlPoints.x,controlPoints.y), controlPoint0, TEST_LOCATION);
  DALI_TEST_EQUALS( Vector2(controlPoints.z,controlPoints.w), controlPoint1, TEST_LOCATION);

  //Check the mode is BEZIER
  DALI_TEST_EQUALS( alpha.GetMode(), AlphaFunction::BEZIER, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAlphaFunctionConstructorFromControlPointsN(void)
{
  TestApplication application;

  //Construct the alpha function with two control points
  Vector2 controlPoint0 = Vector2(-10.0f,1.0f);
  Vector2 controlPoint1 = Vector2(10.0f,0.0f);
  AlphaFunction alpha(controlPoint0,controlPoint1);

  //x components of the control points should have been clamped to [0,1] to ensure the curve is monotonic
  Vector4 controlPoints = alpha.GetBezierControlPoints();
  DALI_TEST_EQUALS( Vector2(controlPoints.x,controlPoints.y), Vector2(0.0f,1.0f), TEST_LOCATION);
  DALI_TEST_EQUALS( Vector2(controlPoints.z,controlPoints.w), Vector2(1.0f,0.0f), TEST_LOCATION);

  //Check the mode is BEZIER
  DALI_TEST_EQUALS( alpha.GetMode(), AlphaFunction::BEZIER, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAlphaFunctionGetBuiltinFunctionP(void)
{
  TestApplication application;
  AlphaFunction alpha( AlphaFunction::EASE_IN);

  //Check if the builtin alpha function is EASE_IN
  DALI_TEST_EQUALS( alpha.GetBuiltinFunction(), AlphaFunction::EASE_IN, TEST_LOCATION);

  //Check the mode is BUILTIN_FUNCTION
  DALI_TEST_EQUALS( alpha.GetMode(), AlphaFunction::BUILTIN_FUNCTION, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAlphaFunctionGetCustomFunctionP(void)
{
  TestApplication application;
  AlphaFunction alpha( &customAlphaFunction );

  //Check that the custom function points to the custom alpha function
  DALI_TEST_EQUALS( alpha.GetCustomFunction(), &customAlphaFunction, TEST_LOCATION);

  //Check the mode is CUSTOM_FUNCTION
  DALI_TEST_EQUALS( alpha.GetMode(), AlphaFunction::CUSTOM_FUNCTION, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAlphaFunctionGetControlPointsFunctionP(void)
{
  TestApplication application;

  Vector2 controlPoint0 = Vector2(0.0f,1.0f);
  Vector2 controlPoint1 = Vector2(1.0f,0.0f);
  AlphaFunction alpha( controlPoint0,controlPoint1 );

  //Check if the control points have the correct value
  Vector4 controlPoints = alpha.GetBezierControlPoints();
  DALI_TEST_EQUALS( Vector2(controlPoints.x,controlPoints.y), controlPoint0, TEST_LOCATION);
  DALI_TEST_EQUALS( Vector2(controlPoints.z,controlPoints.w), controlPoint1, TEST_LOCATION);

  //Check the mode is BEZIER
  DALI_TEST_EQUALS( alpha.GetMode(), AlphaFunction::BEZIER, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAlphaFunctionGetModeP(void)
{
  TestApplication application;

  //Create alpha function using a built-in function
  AlphaFunction alphaBuiltin( AlphaFunction::EASE_IN);

  //Check the mode is BUILTIN_FUNCTION
  DALI_TEST_EQUALS( alphaBuiltin.GetMode(), AlphaFunction::BUILTIN_FUNCTION, TEST_LOCATION);

  //Create alpha function with pointer to function
  AlphaFunction alphaCustom( &customAlphaFunction );
  //Check the mode is CUSTOM_FUNCTION
  DALI_TEST_EQUALS( alphaCustom.GetMode(), AlphaFunction::CUSTOM_FUNCTION, TEST_LOCATION);

  //Create alpha function with control points
  Vector2 controlPoint0 = Vector2(0.0f,1.0f);
  Vector2 controlPoint1 = Vector2(1.0f,0.0f);
  AlphaFunction alphaBezier( controlPoint0,controlPoint1 );
  //Check the mode is BEZIER
  DALI_TEST_EQUALS( alphaBezier.GetMode(), AlphaFunction::BEZIER, TEST_LOCATION);

  END_TEST;
}

int UtcDaliAlphaFunctionBezier(void)
{
  TestApplication application;

  Actor actor = Actor::New();

  // Register a float property
  float startValue(0.0f);
  Property::Index index = actor.RegisterProperty( "testProperty",  startValue );
  Stage::GetCurrent().Add(actor);
  DALI_TEST_EQUALS( actor.GetProperty<float>(index), startValue, TEST_LOCATION );

  // Build the animation
  float durationSeconds(1.0f);
  Animation animation = Animation::New(durationSeconds);
  float targetValue(1.0f);

  Vector2 controlPoint0 = Vector2(0.25f,0.5f);
  Vector2 controlPoint1 = Vector2(0.75f,0.5f);
  animation.AnimateTo(Property(actor, "testProperty"), targetValue, AlphaFunction(controlPoint0,controlPoint1));

  // Start the animation
  animation.Play();

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f)/* 20% progress */);
  application.SendNotification();
  float epsilon(0.01f);
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), 0.271964f, epsilon, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f) + 1u/*40% progress*/);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), 0.432387f, epsilon, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f) + 1u/*60% progress*/);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), 0.567613f, epsilon, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f) + 1u/*80% progress*/);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), 0.728037f, epsilon, TEST_LOCATION );

  application.SendNotification();
  application.Render(static_cast<unsigned int>(durationSeconds*200.0f) + 1u/*just beyond the animation duration*/);
  application.SendNotification();
  DALI_TEST_EQUALS( actor.GetCurrentProperty< float >( index ), targetValue, TEST_LOCATION );

  END_TEST;
}

