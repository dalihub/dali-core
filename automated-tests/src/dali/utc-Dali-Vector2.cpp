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
#include <sstream>
#include <cmath> // isfinite

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

void utc_dali_vector2_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_vector2_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliVector2Constructor01P(void)
{
  TestApplication application;
  Vector2 vec2;
  DALI_TEST_EQUALS(vec2.x, 0.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(vec2.y, 0.0f, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVector2Constructor02P(void)
{
  TestApplication application;
  Vector2 vec2(1.f,1.f);
  DALI_TEST_EQUALS(vec2.x, 1.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(vec2.y, 1.0f, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVector2Constructor03P(void)
{
  TestApplication application;
  float array[] = {1.f,1.f};
  Vector2 vec2(array);
  DALI_TEST_EQUALS(vec2.x, 1.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(vec2.y, 1.0f, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVector2Constructor04P(void)
{
  TestApplication application;
  Vector3 vec3(1.f,1.f,1.f);
  Vector3 vec2(vec3);
  DALI_TEST_EQUALS(vec2.x, 1.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(vec2.y, 1.0f, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVector2Constructor05P(void)
{
  TestApplication application;
  Vector4 vec4(1.f,1.f,1.f,1.f);
  Vector2 vec2(vec4);
  DALI_TEST_EQUALS(vec2.x, 1.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(vec2.y, 1.0f, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVector2CopyConstructor01P(void)
{
  TestApplication application;
  float array[] = {1.f,1.f};
  Vector2 vec2;
  vec2 = array;
  DALI_TEST_EQUALS(vec2.x, 1.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(vec2.y, 1.0f, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVector2CopyConstructor02P(void)
{
  TestApplication application;
  Vector3 vec3(1.f,1.f,1.f);
  Vector3 vec2;
  vec2 = vec3;
  DALI_TEST_EQUALS(vec2.x, 1.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(vec2.y, 1.0f, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVector2CopyConstructor03P(void)
{
  TestApplication application;
  Vector4 vec4(1.f,1.f,1.f,1.f);
  Vector3 vec2;
  vec2 = vec4;
  DALI_TEST_EQUALS(vec2.x, 1.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(vec2.y, 1.0f, 0.001, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVector2AssignP(void)
{
  TestApplication application;
  Vector2 v1(10.0f, 20.0f);
  Vector2 r0(11.0f, 22.0f);

  v1 = r0;
  DALI_TEST_EQUALS(v1, r0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVector2AssignP02(void)
{
  TestApplication application;
  Vector2 v1(10.0f, 20.0f);
  Vector4 r0(11.0f, 22.0f, 33.f, 44.f);

  v1 = r0;
  DALI_TEST_EQUALS(v1, Vector2(r0.x, r0.y), TEST_LOCATION);
  END_TEST;
}

int UtcDaliVector2AssignP03(void)
{
  TestApplication application;
  Vector2 v1(10.0f, 20.0f);
  Vector3 r0(11.0f, 22.0f, 33.f);

  v1 = r0;
  DALI_TEST_EQUALS(v1, Vector2(r0.x, r0.y), TEST_LOCATION);
  END_TEST;
}

int UtcDaliVector2AddP(void)
{
  TestApplication application;
  Vector2 v0(1.0f, 2.0f);
  Vector2 v1(10.0f, 20.0f);
  Vector2 r0(11.0f, 22.0f);

  Vector2 v2 = v0+v1;
  DALI_TEST_EQUALS(v2, r0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVector2AddAssignP(void)
{
  TestApplication application;
  Vector2 v0(1.0f, 2.0f);
  Vector2 v1(10.0f, 20.0f);
  Vector2 r0(11.0f, 22.0f);

  v0 += v1;
  DALI_TEST_EQUALS(v0, r0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVector2SubtractP(void)
{
  TestApplication application;
  Vector2 v0(11.0f, 22.0f);
  Vector2 v1(10.0f, 20.0f);
  Vector2 r0(1.0f, 2.0f);

  Vector2 v2 = v0-v1;
  DALI_TEST_EQUALS(v2, r0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVector2SubtractAssignP(void)
{
  TestApplication application;
  Vector2 v0(11.0f, 22.0f);
  Vector2 v1(10.0f, 20.0f);
  Vector2 r0(1.0f, 2.0f);

  v0 -= v1;
  DALI_TEST_EQUALS(v0, r0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVector2NegateP(void)
{
  TestApplication application;
  Vector2 v1(10.0f, 20.0f);
  Vector2 r0(-10.0f, -20.0f);

  Vector2 v2 = -v1;
  DALI_TEST_EQUALS(v2, r0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVector2Multiply01P(void)
{
  TestApplication application;
  Vector2 v0(2.0f, 3.0f);
  Vector2 v1(10.0f, 20.0f);
  Vector2 r0(20.0f, 60.0f);

  Vector2 v2 = v0 * v1;
  DALI_TEST_EQUALS(v2, r0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVector2Multiply02P(void)
{
  TestApplication application;
  Vector2 v0(2.0f, 3.0f);
  Vector2 r0(20.0f, 30.0f);

  Vector2 v2 = v0 * 10.f;
  DALI_TEST_EQUALS(v2, r0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVector2MultiplyAssign01P(void)
{
  TestApplication application;
  Vector2 v0(2.0f, 3.0f);
  Vector2 v1(10.0f, 20.0f);
  Vector2 r0(20.0f, 60.0f);

  v0 *= v1;
  DALI_TEST_EQUALS(v0, r0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVector2MultiplyAssign02P(void)
{
  TestApplication application;
  Vector2 v0(2.0f, 3.0f);
  Vector2 r0(20.0f, 30.0f);

  v0 *= 10.f;
  DALI_TEST_EQUALS(v0, r0, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVector2Divide01P(void)
{
  TestApplication application;
  Vector2 v0(1.0f, 1.0f);
  Vector2 v1(2.0f, 3.0f);
  Vector2 v2(4.0f, 9.0f);

  DALI_TEST_EQUALS( v0/v0, v0, TEST_LOCATION);
  DALI_TEST_EQUALS( v1/v0, v1, TEST_LOCATION);
  DALI_TEST_EQUALS( v1/v1, v0, TEST_LOCATION);
  DALI_TEST_EQUALS( v2/v1, v1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVector2Divide02P(void)
{
  TestApplication application;
  Vector2 v0(1.0f, 1.0f);
  Vector2 v1(3.0f, 3.0f);
  Vector2 v2(9.0f, 9.0f);

  DALI_TEST_EQUALS( v0/1.f, v0, TEST_LOCATION);
  DALI_TEST_EQUALS( v1/1.f, v1, TEST_LOCATION);
  DALI_TEST_EQUALS( v1/3.f, v0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVector2DivideAssign01P(void)
{
  TestApplication application;
  Vector2 v0(1.0f, 1.0f);
  Vector2 v1(2.0f, 3.0f);
  Vector2 v2(4.0f, 9.0f);

  Vector2 v4(v0);
  v4 /= v0;
  DALI_TEST_EQUALS(v4, v0, TEST_LOCATION);

  Vector2 v5(v1);
  v5 /= v0;
  DALI_TEST_EQUALS(v5, v1, TEST_LOCATION);

  Vector2 v6(v1);
  v6 /= v6;
  DALI_TEST_EQUALS(v6, v0, TEST_LOCATION);

  v2 /= v1;
  DALI_TEST_EQUALS(v2, v1, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVector2DivideAssign02P(void)
{
  TestApplication application;
  Vector2 v0(1.0f, 1.0f);
  Vector2 v1(3.0f, 3.0f);

  Vector2 v4(v0);
  v4 /= 1.f;
  DALI_TEST_EQUALS(v4, v0, TEST_LOCATION);

  Vector2 v5(v1);
  v5 /= 1.f;
  DALI_TEST_EQUALS(v5, v1, TEST_LOCATION);

  Vector2 v6(v1);
  v6 /= 3.f;
  DALI_TEST_EQUALS(v6, v0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliVector2EqualsP(void)
{
  TestApplication application;
  Vector2 v0(1.0f, 2.0f);
  Vector2 v1(1.0f, 2.0f);

  DALI_TEST_CHECK(v0 == v1);

  END_TEST;
}

int UtcDaliVector2NotEqualsP(void)
{
  TestApplication application;
  Vector2 v0(1.0f, 2.0f);
  Vector2 v1(1.0f, 2.0f);

  Vector2 v2 = Vector2(0.0f, 2.0f);
  DALI_TEST_CHECK(v0 != v2);

  v2 = Vector2(1.0f, 0.0f);
  DALI_TEST_CHECK(v0 != v2);

  v2 = Vector2(1.0f, 77.0f);
  DALI_TEST_CHECK(v0 != v2);

  v2 = Vector2(33.0f, 44.0f);
  DALI_TEST_CHECK(v0 != v2);
  END_TEST;
}

int UtcDaliVector2OperatorSubscriptP(void)
{
  TestApplication application;
  Vector2 testVector(1.0f, 2.0f);

  // read array subscripts
  DALI_TEST_EQUALS( testVector[0], 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( testVector[1], 2.0f, TEST_LOCATION );

  // write array subscripts/read struct memebers
  testVector[0] = 3.0f;
  testVector[1] = 4.0f;

  DALI_TEST_EQUALS( testVector.x, 3.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( testVector.y, 4.0f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliVector2OperatorSubscriptN(void)
{
  TestApplication application;
  Vector2 testVector(1.0f, 2.0f);

  try
  {
    float& w = testVector[4];
    if(w==0.0f);
    tet_result(TET_FAIL);
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "Vector element index out of bounds", TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliVector2OperatorConstSubscriptP(void)
{
  TestApplication application;
  const Vector2 testVector2(3.0f, 4.0f);
  const float& x = testVector2[0];
  const float& y = testVector2[1];
  DALI_TEST_EQUALS( x, 3.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( y, 4.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliVector2OperatorConstSubscriptN(void)
{
  TestApplication application;
  const Vector2 testVector2(3.0f, 4.0f);

  try
  {
    const float& w = testVector2[4];
    if(w==0.0f);
    tet_result(TET_FAIL);
  }
  catch (Dali::DaliException& e)
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "Vector element index out of bounds" , TEST_LOCATION);
  }


  END_TEST;
}


int UtcDaliVector2LengthP(void)
{
  TestApplication application;
  Vector2 v(1.0f, 2.0f);
  DALI_TEST_EQUALS(v.Length(), sqrtf(v.x*v.x + v.y*v.y), 0.001f, TEST_LOCATION);

  Vector2 v1(0.0f, 0.0f);
  DALI_TEST_EQUALS(v1.Length(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVector2LengthSquaredP(void)
{
  TestApplication application;
  Vector2 v(1.0f, 2.0f);
  DALI_TEST_EQUALS(v.LengthSquared(), v.x*v.x + v.y*v.y, 0.001f, TEST_LOCATION);

  Vector2 v1(0.0f, 0.0f);
  DALI_TEST_EQUALS(v1.LengthSquared(), 0.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVector2NormalizeP(void)
{
  TestApplication application;
  for (float f=0.0f; f<6.0f; f+=1.0f)
  {
    Vector2 v(cosf(f)*10.0f, cosf(f+1.0f)*10.0f);
    v.Normalize();
    DALI_TEST_EQUALS(v.LengthSquared(), 1.0f, 0.001f, TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliVector2NormalizeN(void)
{
  TestApplication application;
  Vector2 v(0.0f, 0.0f);
  v.Normalize();
  DALI_TEST_EQUALS(v.LengthSquared(), 0.0f, 0.00001f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVector2ClampVector2P(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Vector2::Clamp( const Vector2& v, const Vector2& min, const Vector2& max )");

  Vector2 v0(2.0f, 0.8f);
  Vector2 v1(-1.0f, 2.0f);
  Vector2 v2(10.0f, 5.0f);
  Vector2 v3(8.0f, 10.0f);
  Vector2 v4(4.9f, 5.1f);
  Vector2 min(1.0f, 4.0f);
  Vector2 max(9.0f, 6.0f);

  v0.Clamp( min, max );
  v1.Clamp( min, max );
  v2.Clamp( min, max );
  v3.Clamp( min, max );
  v4.Clamp( min, max );

  DALI_TEST_EQUALS( v0, Vector2(2.0f, 4.0f), 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( v1, Vector2(1.0f, 4.0f), 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( v2, Vector2(9.0f, 5.0f), 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( v3, Vector2(8.0f, 6.0f), 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( v4, Vector2(4.9f, 5.1f), 0.01f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliVector2AsFloatP(void)
{
  TestApplication application;
  Vector2 vec2(1.f,1.f);
  float* p = vec2.AsFloat();

  DALI_TEST_EQUALS( p[0], 1.f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( p[1], 1.f, 0.01f, TEST_LOCATION );

  p[0] = 2.f;
  p[1] = 2.f;

  DALI_TEST_EQUALS( p[0], 2.f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( p[1], 2.f, 0.01f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliVector2ConstAsFloatP(void)
{
  TestApplication application;
  Vector2 vec2(1.f,1.f);
  const float* p = vec2.AsFloat();

  DALI_TEST_EQUALS( p[0], 1.f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( p[1], 1.f, 0.01f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliVector2XWidthP(void)
{
  TestApplication application;
  Vector2 vec2(1.f,1.f);

  DALI_TEST_EQUALS( vec2.x, 1.f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( vec2.width, 1.f, 0.01f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliVector2YHeightP(void)
{
  TestApplication application;
  Vector2 vec2(1.f,1.f);

  DALI_TEST_EQUALS( vec2.y, 1.f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( vec2.height, 1.f, 0.01f, TEST_LOCATION );

  END_TEST;
}

int UtcDaliVector2OStreamOperatorP(void)
{
  TestApplication application;
  std::ostringstream oss;

  Vector2 vector(1, 2);

  oss << vector;

  std::string expectedOutput = "[1, 2]";

  DALI_TEST_EQUALS( oss.str(), expectedOutput, TEST_LOCATION);
  END_TEST;
}


int UtcDaliVector2MaxP(void)
{
  TestApplication application;
  Vector2 v0(2.0f, 1.0f);
  Vector2 v1(1.0f, 2.0f);

  DALI_TEST_EQUALS(Max(v0, v1), Vector2(2.0f, 2.0f), 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVector2MinP(void)
{
  TestApplication application;
  Vector2 v0(2.0f, 1.0f);
  Vector2 v1(1.0f, 2.0f);

  DALI_TEST_EQUALS(Min(v0, v1), Vector2(1.0f, 1.0f), 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliVector2ClampP(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Vector2::Clamp( const Vector2& v, const float& min, const float& max )");

  Vector2 v0(2.0f, 0.8f);
  Vector2 v1(-1.0f, 2.0f);

  DALI_TEST_EQUALS( Clamp( v0, 0.9f, 1.1f ), Vector2(1.1f, 0.9f), 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( Clamp( v1, 1.0f, 1.0f ), Vector2(1.0f, 1.0f), 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( Clamp( v1, 0.0f, 3.0f ), Vector2(0.0f, 2.0f), 0.01f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliVector2FitInsideP(void)
{
  TestApplication application;
  DALI_TEST_EQUALS( Vector2(  1.0f,  2.0f ), FitInside( Vector2(   1.0f,  2.0f ), Vector2( 10.0f, 20.0f ) ), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2(  1.0f,  0.5f ), FitInside( Vector2(   1.0f,  2.0f ), Vector2( 20.0f, 10.0f ) ), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2( 10.0f, 20.0f ), FitInside( Vector2(  10.0f, 20.0f ), Vector2(  1.0f,  2.0f ) ), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2( 10.0f, 10.0f ), FitInside( Vector2( 100.0f, 10.0f ), Vector2( 10.0f, 10.0f ) ), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2( 20.0f, 10.0f ), FitInside( Vector2(  20.0f, 20.0f ), Vector2( 10.0f,  5.0f ) ), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2( 10.0f, 20.0f ), FitInside( Vector2(  20.0f, 20.0f ), Vector2(  5.0f, 10.0f ) ), TEST_LOCATION );
  END_TEST;
}

int UtcDaliVector2FitInside01N(void)
{
  TestApplication application;
  Size size = FitInside( Vector2(   1.0f,  2.0f ), Vector2( 0.0f, 0.0f ) );
  DALI_TEST_EQUALS( false, std::isfinite(size.y), TEST_LOCATION );
  END_TEST;
}

int UtcDaliVector2FitScaleToFillP(void)
{
  TestApplication application;
  DALI_TEST_EQUALS( Vector2(  1.0f,  2.0f ), FitScaleToFill( Vector2(   1.0f,  2.0f ), Vector2( 10.0f, 20.0f ) ), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2(  1.0f,  2.0f ), FitScaleToFill( Vector2(  10.0f, 20.0f ), Vector2(  1.0f,  2.0f ) ), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2( 10.0f,  1.0f ), FitScaleToFill( Vector2( 100.0f, 10.0f ), Vector2( 10.0f, 10.0f ) ), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2(  5.0f,  5.0f ), FitScaleToFill( Vector2(  20.0f, 20.0f ), Vector2( 10.0f,  5.0f ) ), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2(  5.0f,  5.0f ), FitScaleToFill( Vector2(  20.0f, 20.0f ), Vector2(  5.0f, 10.0f ) ), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2( 20.0f, 40.0f ), FitScaleToFill( Vector2(  20.0f,  0.0f ), Vector2(  5.0f, 10.0f ) ), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2( 10.0f, 20.0f ), FitScaleToFill( Vector2(   0.0f, 20.0f ), Vector2(  5.0f, 10.0f ) ), TEST_LOCATION );
  END_TEST;
}


int UtcDaliVector2FitScaleToFillN(void)
{
  TestApplication application;
  Vector2 target(0.f, 0.f);
  Size size = FitScaleToFill( target, Vector2( 0.0f, 0.0f ) );
  DALI_TEST_EQUALS( size, target, TEST_LOCATION );
  END_TEST;
}

int UtcDaliVector2ShrinkInsideP(void)
{
  TestApplication application;
  DALI_TEST_EQUALS( Vector2(  1.0f,  2.0f ), ShrinkInside( Vector2(   1.0f,  2.0f ), Vector2( 10.0f, 20.0f ) ), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2(  1.0f,  2.0f ), ShrinkInside( Vector2(  10.0f, 20.0f ), Vector2(  1.0f,  2.0f ) ), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2( 10.0f, 10.0f ), ShrinkInside( Vector2( 100.0f, 10.0f ), Vector2( 10.0f, 10.0f ) ), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2( 10.0f,  5.0f ), ShrinkInside( Vector2(  20.0f, 20.0f ), Vector2( 10.0f,  5.0f ) ), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2(  5.0f, 10.0f ), ShrinkInside( Vector2(  20.0f, 20.0f ), Vector2(  5.0f, 10.0f ) ), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2(  5.0f, 10.0f ), ShrinkInside( Vector2(  10.0f, 10.0f ), Vector2( 10.0f, 20.0f ) ), TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2( 10.0f,  5.0f ), ShrinkInside( Vector2(  10.0f, 10.0f ), Vector2( 20.0f, 10.0f ) ), TEST_LOCATION );
  END_TEST;
}

int UtcDaliVector2ShrinkInside01N(void)
{
  TestApplication application;
  Vector2 target(0.f, 0.f);
  Size size = ShrinkInside( Vector2( -1.0f,  1.0f ), Vector2( 0.0f, 1.0f ) );
  DALI_TEST_EQUALS( false, std::isfinite(size.x), TEST_LOCATION );
  END_TEST;
}

int UtcDaliVector2ShrinkInside02N(void)
{
  TestApplication application;
  Vector2 target(0.f, 0.f);
  Size size = ShrinkInside( Vector2( -1.0f,  -1.0f ), Vector2( 1.0f, 0.0f ) );
  DALI_TEST_EQUALS( false, std::isfinite(size.y), TEST_LOCATION );
  END_TEST;
}

