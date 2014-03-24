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
#include <sstream>

#include <stdlib.h>
#include <tet_api.h>

#include <dali/public-api/dali-core.h>

#include <dali-test-suite-utils.h>

using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliVector3Cons,              POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3Assign,            POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3Add,               POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3Constants,         POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3Cross,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3Dot,               POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3Equals,            POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3Length,            POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3LengthSquared,     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3Max,               POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3Min,               POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3Clamp,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3ClampVector3,      POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3Multiply,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3Divide,            POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3Scale,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3Normalize,         POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3Subtract,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3OperatorSubscript, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3OStreamOperator,   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3Rotate,            POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3AsFloat,           POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3AsVectorXY,        POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3FitKeepAspectRatio, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3FillKeepAspectRatio, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3FillXYKeepAspectRatio, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector3ShrinkInsideKeepAspectRatio, POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliVector3Cons()
{
  Vector3 va;
  Vector3 vb(Vector2(1.0f, 2.0f));
  Vector3 vc(Vector4(5.63f, 2.31f, 9.83f, 11.23f));
  float array[] = {1.0f, 2.0f, 3.0f};
  Vector3 vd(array);

  DALI_TEST_EQUALS(va.x, 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(va.y, 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(va.z, 0.0f, 0.001f, TEST_LOCATION);

  DALI_TEST_EQUALS(vb.x, 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vb.y, 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vb.z, 0.0f, 0.001f, TEST_LOCATION);

  DALI_TEST_EQUALS(vc.x, 5.63f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vc.y, 2.31f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vc.z, 9.83f, 0.001f, TEST_LOCATION);

  DALI_TEST_EQUALS(vd.x, 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vd.y, 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vd.z, 3.0f, 0.001f, TEST_LOCATION);

  Vector3* vecPtr = new Vector3(1.0f, 2.0f, 3.0f);
  DALI_TEST_CHECK( vecPtr != NULL );
  DALI_TEST_EQUALS(vecPtr->x, 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vecPtr->y, 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vecPtr->z, 3.0f, 0.001f, TEST_LOCATION);

  Vector3 vec3;
  vec3 = *vecPtr;
  delete vecPtr;

  Vector3 vec3b(vd);
  DALI_TEST_EQUALS(vec3b.x, 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vec3b.y, 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vec3b.z, 3.0f, 0.001f, TEST_LOCATION);

  Vector3 vec3c = vd;
  DALI_TEST_EQUALS(vec3c.x, 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vec3c.y, 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vec3c.z, 3.0f, 0.001f, TEST_LOCATION);
}

static void UtcDaliVector3Assign()
{
  Vector3 v0;
  const float array[] = { 1.0f, 2.0f, 3.0f };
  v0 = (const float*)array;

  DALI_TEST_EQUALS(v0.x, 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(v0.y, 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(v0.z, 3.0f, 0.001f, TEST_LOCATION);

  Vector2 vec2_q(1.0f, 2.0f);
  Vector3 vec3a;
  vec3a = vec2_q;

  DALI_TEST_EQUALS(vec3a.x, 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vec3a.y, 2.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vec3a.z, 0.0f, 0.001f, TEST_LOCATION);

  Vector4 vec4_q(4.0f, 3.0f, 2.0f, 1.0f);
  Vector3 vec3b;
  vec3b = vec4_q;

  DALI_TEST_EQUALS(vec3b.x, 4.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vec3b.y, 3.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vec3b.z, 2.0f, 0.001f, TEST_LOCATION);
}


// Add
static void UtcDaliVector3Add()
{
  Vector3 v0(1.0f, 2.0f, 3.0f);
  Vector3 v1(10.0f, 20.0f, 30.0f);
  Vector3 r0(11.0f, 22.0f, 33.0f);

  Vector3 v2 = v0+v1;
  DALI_TEST_EQUALS(v2, r0, TEST_LOCATION);

  v0 += v1;
  DALI_TEST_EQUALS(v0, r0, TEST_LOCATION);
}

// Constants
static void UtcDaliVector3Constants()
{
  Vector3 va = Vector3::ZERO;
  Vector3 vb = Vector3::ONE;
  Vector3 vc = Vector3::XAXIS;

  DALI_TEST_EQUALS(va.x, 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(va.y, 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(va.z, 0.0f, 0.001f, TEST_LOCATION);

  DALI_TEST_EQUALS(vb.x, 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vb.y, 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vb.z, 1.0f, 0.001f, TEST_LOCATION);

  DALI_TEST_EQUALS(vc.x, 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vc.y, 0.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(vc.z, 0.0f, 0.001f, TEST_LOCATION);
}

// Cross
static void UtcDaliVector3Cross()
{
  DALI_TEST_EQUALS(Vector3::XAXIS.Cross(Vector3::YAXIS), Vector3::ZAXIS, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector3::YAXIS.Cross(Vector3::ZAXIS), Vector3::XAXIS, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector3::ZAXIS.Cross(Vector3::XAXIS), Vector3::YAXIS, 0.0001f, TEST_LOCATION);

  DALI_TEST_EQUALS(Vector3::XAXIS.Cross(Vector3::ZAXIS), -Vector3::YAXIS, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector3::YAXIS.Cross(Vector3::XAXIS), -Vector3::ZAXIS, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector3::ZAXIS.Cross(Vector3::YAXIS), -Vector3::XAXIS, 0.0001f, TEST_LOCATION);

  Vector3 v0(2.0f, 3.0f, 4.0f);
  Vector3 v1(10.0f, 20.0f, 30.0f);
  Vector3 result(   (v0.y * v1.z) - (v0.z * v1.y),
                    (v0.z * v1.x) - (v0.x * v1.z),
                    (v0.x * v1.y) - (v0.y * v1.x) );


  DALI_TEST_EQUALS(v0.Cross(v1), result, 0.001f, TEST_LOCATION);
}

// Dot
static void UtcDaliVector3Dot()
{
  DALI_TEST_EQUALS(Vector3::XAXIS.Dot(Vector3::YAXIS), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector3::XAXIS.Dot(Vector3::ZAXIS), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector3::XAXIS.Dot(Vector3::XAXIS), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector3::YAXIS.Dot(Vector3::YAXIS), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector3::ZAXIS.Dot(Vector3::ZAXIS), 1.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(Vector3(1.0f, 0.0f, 0.0f).Dot(Vector3(1.0f, 0.0f, 0.0f)), 1.0f, TEST_LOCATION);

  for (float x = 0; x<6.0f; x+=1.0f)
  {
    Vector3 v0(cosf(x), sinf(x), 0.0f);
    Vector3 v1(sinf(x), -cosf(x), 0.0f);
    DALI_TEST_EQUALS(v0.Dot(v1), 0.0f, 0.0001f, TEST_LOCATION);
    DALI_TEST_EQUALS(v0.Dot(v0), 1.0f, 0.0001f, TEST_LOCATION);

    v0 = Vector3(cosf(x), 0.0f, sinf(x));
    v1 = Vector3(sinf(x), 0.0f, -cosf(x));
    DALI_TEST_EQUALS(v0.Dot(v0), 1.0f, 0.0001f, TEST_LOCATION);
  }

  Vector3 v0 = Vector3(12.0f, 7.0f, 9.0f);
  v0.Normalize();

  Vector3 v1 = v0 * 2.0f;
  DALI_TEST_EQUALS(v0.Dot(v1), 2.0f, 0.001f, TEST_LOCATION);
}


// Equals
static void UtcDaliVector3Equals()
{
  Vector3 v0(1.0f, 2.0f,  3.0f);
  Vector3 v1(1.0f, 2.0f,  3.0f);

  DALI_TEST_CHECK(v0 == v1);

  Vector3 v2 = Vector3(0.0f, 2.0f, 3.0f);
  DALI_TEST_CHECK(v0 != v2);

  v2 = Vector3(1.0f, 0.0f, 3.0f);
  DALI_TEST_CHECK(v0 != v2);

  v2 = Vector3(1.0f, 2.0f, 0.0f);
  DALI_TEST_CHECK(v0 != v2);

  v2 = Vector3(11.0f, 22.0f, 33.0f);
  DALI_TEST_CHECK(v0 != v2);
}


// Length
static void UtcDaliVector3Length()
{
  Vector3 v(1.0f, 2.0f, 3.0f);
  DALI_TEST_EQUALS(v.Length(), sqrtf(v.x*v.x + v.y*v.y + v.z*v.z), 0.001f, TEST_LOCATION);

  Vector3 v1(0.0f, 0.0f, 0.0f);
  DALI_TEST_EQUALS(v1.Length(), 0.0f, TEST_LOCATION);
}

// Length squared
static void UtcDaliVector3LengthSquared()
{
  Vector3 v(1.0f, 2.0f, 3.0f);
  DALI_TEST_EQUALS(v.LengthSquared(), v.x*v.x + v.y*v.y + v.z*v.z, 0.001f, TEST_LOCATION);

  Vector3 v1(0.0f, 0.0f, 0.0f);
  DALI_TEST_EQUALS(v1.LengthSquared(), 0.0f, TEST_LOCATION);
}

// Max
static void UtcDaliVector3Max()
{
  Vector3 v0(2.0f, 1.0f, 3.0f);
  Vector3 v1(1.0f, 2.0f, 3.0f);

  DALI_TEST_EQUALS(Max(v0, v1), Vector3(2.0f, 2.0f, 3.0f), 0.01f, TEST_LOCATION);
}

// Min
static void UtcDaliVector3Min()
{
  Vector3 v0(2.0f, 2.0f, 1.0f);
  Vector3 v1(1.0f, 1.0f, 2.0f);

  DALI_TEST_EQUALS(Min(v0, v1), Vector3(1.0f, 1.0f, 1.0f), 0.01f, TEST_LOCATION);
}

static void UtcDaliVector3Clamp()
{
  tet_infoline("Testing Dali::Vector3::Clamp()");

  Vector3 v0( 2.0f, 1.0f, 0.0f );
  Vector3 v1( -1.0f, 2.0f, 1.0f );

  DALI_TEST_EQUALS( Clamp( v0, 0.9f, 1.1f ), Vector3(1.1f, 1.0f, 0.9f), 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( Clamp( v1, 1.0f, 1.0f ), Vector3(1.0f, 1.0f, 1.0f), 0.01f, TEST_LOCATION );
}

static void UtcDaliVector3ClampVector3()
{
  tet_infoline("Testing Dali::Vector3::Clamp( const Vector3& v, const Vector3& min, const Vector3& max )");

  Vector3 v0(2.0f, 0.8f, 0.0f);
  Vector3 v1(-1.0f, 2.0f, 10.0f);
  Vector3 v2(10.0f, 5.0f, 0.0f);
  Vector3 v3(8.0f, 10.0f, 5.0f);
  Vector3 v4(4.9f, 5.1f, 10.0f);
  Vector3 min(1.0f, 4.0f, 1.5f);
  Vector3 max(9.0f, 6.0f, 8.0f);

  v0.Clamp( min, max );
  v1.Clamp( min, max );
  v2.Clamp( min, max );
  v3.Clamp( min, max );
  v4.Clamp( min, max );

  DALI_TEST_EQUALS( v0, Vector3(2.0f, 4.0f, 1.5f), 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( v1, Vector3(1.0f, 4.0f, 8.0f), 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( v2, Vector3(9.0f, 5.0f, 1.5f), 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( v3, Vector3(8.0f, 6.0f, 5.0f), 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( v4, Vector3(4.9f, 5.1f, 8.0f), 0.01f, TEST_LOCATION );
}

// Multiply
static void UtcDaliVector3Multiply()
{
  Vector3 v0(2.0f, 3.0f, 4.0f);
  Vector3 v1(10.0f, 20.0f,  30.0f);
  Vector3 r0(20.0f, 60.0f, 120.0f);

  Vector3 v2 = v0 * v1;
  DALI_TEST_EQUALS(v2, r0, TEST_LOCATION);

  v0 *= v1;
  DALI_TEST_EQUALS(v0, r0, TEST_LOCATION);
}


// Divide
static void UtcDaliVector3Divide()
{
  Vector3 v0(1.0f, 1.0f, 1.0f);
  Vector3 v1(2.0f, 3.0f, 5.0f);
  Vector3 v2(4.0f, 9.0f, 25.0f);

  DALI_TEST_EQUALS( v0/v0, v0, TEST_LOCATION);
  DALI_TEST_EQUALS( v1/v0, v1, TEST_LOCATION);
  DALI_TEST_EQUALS( v1/v1, v0, TEST_LOCATION);
  DALI_TEST_EQUALS( v2/v1, v1, TEST_LOCATION);

  Vector3 v4(v0);
  v4 /= v0;
  DALI_TEST_EQUALS(v4, v0, TEST_LOCATION);

  Vector3 v5(v1);
  v5 /= v0;
  DALI_TEST_EQUALS(v5, v1, TEST_LOCATION);

  Vector3 v6(v1);
  v6 /= v6;
  DALI_TEST_EQUALS(v6, v0, TEST_LOCATION);

  v2 /= v1;
  DALI_TEST_EQUALS(v2, v1, TEST_LOCATION);

}

static void UtcDaliVector3Scale()
{
  Vector3 v0(2.0f,   4.0f,  8.0f);
  const Vector3 r0(20.0f, 40.0f, 80.0f);
  const Vector3 r1(10.0f, 20.0f, 40.0f);
  const Vector3 r2( 1.0f,  2.0f,  4.0f);
  const Vector3 r3(2.0f,   4.0f,  8.0f);

  Vector3 v2 = v0 * 10.0f;
  DALI_TEST_EQUALS(v2, r0, TEST_LOCATION);

  v0 *= 5.0f;
  DALI_TEST_EQUALS(v0, r1, TEST_LOCATION);

  v2 = r0 / 10.0f;
  DALI_TEST_EQUALS(v2, r3, TEST_LOCATION);

  v2 = r1;
  v2 /= 10.0f;
  DALI_TEST_EQUALS(v2, r2, TEST_LOCATION);
}



// Normalize
static void UtcDaliVector3Normalize()
{
  for (float f=0.0f; f<6.0f; f+=1.0f)
  {
    Vector3 v(cosf(f)*10.0f, cosf(f+1.0f)*10.0f, cosf(f+2.0f)*10.0f);
    v.Normalize();
    DALI_TEST_EQUALS(v.LengthSquared(), 1.0f, 0.001f, TEST_LOCATION);
  }

  Vector3 v(0.0f, 0.0f, 0.0f);
  v.Normalize();
  DALI_TEST_EQUALS(v.LengthSquared(), 0.0f, 0.00001f, TEST_LOCATION);
}

// Subtract
static void UtcDaliVector3Subtract()
{
  Vector3 v0(11.0f, 22.0f, 33.0f);
  Vector3 v1(10.0f, 20.0f, 30.0f);
  Vector3 r0(1.0f, 2.0f, 3.0f);

  Vector3 v2 = v0-v1;
  DALI_TEST_EQUALS(v2, r0, TEST_LOCATION);

  v0 -= v1;
  DALI_TEST_EQUALS(v0, r0, TEST_LOCATION);
}

static void UtcDaliVector3OperatorSubscript()
{
  Vector3 testVector(1.0f, 2.0f, 3.0f);

  // read array subscripts
  DALI_TEST_EQUALS( testVector[0], 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( testVector[1], 2.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( testVector[2], 3.0f, TEST_LOCATION );

  // write array subscripts/read struct memebers
  testVector[0] = 4.0f;
  testVector[1] = 5.0f;
  testVector[2] = 6.0f;

  DALI_TEST_EQUALS( testVector.x, 4.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( testVector.y, 5.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( testVector.z, 6.0f, TEST_LOCATION );

  // write struct members/read array subscripts
  testVector.x = 7.0f;
  testVector.y = 8.0f;
  testVector.z = 9.0f;

  DALI_TEST_EQUALS( testVector[0], 7.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( testVector[1], 8.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( testVector[2], 9.0f, TEST_LOCATION );

  // write struct members/read array subscripts
  const Vector3 testVector2(1.0f, 2.0f, 3.0f);
  const float& x = testVector2[0];
  const float& y = testVector2[1];
  const float& z ( testVector2[2] );

  DALI_TEST_EQUALS( x, 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( y, 2.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( z, 3.0f, TEST_LOCATION );

  try
  {
    float& w = testVector[4];
    if(w==0.0f);
    tet_result(TET_FAIL);
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT( e, "index < 3", TEST_LOCATION );
  }

  try
  {
    const float& w = testVector2[4];
    if(w==0.0f);
    tet_result(TET_FAIL);
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT(e, "index < 3", TEST_LOCATION);
  }

}

static void UtcDaliVector3OStreamOperator()
{
  std::ostringstream oss;

  Vector3 vector(1, 2, 3);

  oss << vector;

  std::string expectedOutput = "[1, 2, 3]";

  DALI_TEST_EQUALS( oss.str(), expectedOutput, TEST_LOCATION);
}

static void UtcDaliVector3Rotate()
{
  TestApplication application;

  Vector3 vec3(Vector3::YAXIS);
  Quaternion rotation(Math::PI_2, Vector3::ZAXIS);
  Vector3 result(-Vector3::XAXIS);
  vec3 *= rotation;
  DALI_TEST_EQUALS( vec3, result, 0.001, TEST_LOCATION );
}

static void UtcDaliVector3AsFloat()
{
  float values[] = {0.0f,  1.0f,  2.0f};
  Vector3 v0(values);

  for (int i=0;i<3;++i)
  {
    DALI_TEST_EQUALS(v0.AsFloat()[i], values[i], TEST_LOCATION);
  }

  const Vector3 v1(values);
  for (int i=0;i<3;++i)
  {
    DALI_TEST_EQUALS(v1.AsFloat()[i], values[i], TEST_LOCATION);
  }
}

static void UtcDaliVector3AsVectorXY()
{
  float values[] = {0.0f,  1.0f,  2.0f};
  Vector3 v0(values);
  const Vector3 v1(5.0f, 10.0f, 15.0f);
  // X = 0.0
  // Y = 1.0
  // Z = 2.0

  DALI_TEST_EQUALS(v0.GetVectorXY().x, values[0], TEST_LOCATION);
  DALI_TEST_EQUALS(v0.GetVectorXY().y, values[1], TEST_LOCATION);

  v0.GetVectorXY() = v1.GetVectorYZ();
  // X = y
  // Y = z
  // Z = 2.0
  DALI_TEST_EQUALS(v0.GetVectorXY().x, v1.y, TEST_LOCATION);
  DALI_TEST_EQUALS(v0.GetVectorXY().y, v1.z, TEST_LOCATION);
  DALI_TEST_EQUALS(v0.z, values[2], TEST_LOCATION);

  v0.GetVectorYZ() = v1.GetVectorXY();
  // X = y
  // Y = x
  // Z = y
  DALI_TEST_EQUALS(v0.x, v1.y, TEST_LOCATION);
  DALI_TEST_EQUALS(v0.y, v1.x, TEST_LOCATION);
  DALI_TEST_EQUALS(v0.z, v1.y, TEST_LOCATION);
}


static void UtcDaliVector3FitKeepAspectRatio()
{
  TestApplication application;
  Vector3 target(10.0f, 20.0f, 30.0f);
  Vector3 source1(1.0f, 2.0f, 3.0f);
  Vector3 source2(1.0f, 1.0f, 1.0f);

  DALI_TEST_EQUALS( FitKeepAspectRatio( target, target ), Vector3(1.0f, 1.0f, 1.0f), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( FitKeepAspectRatio( target, source1 ), Vector3(10.0f, 10.0f, 10.0f), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( FitKeepAspectRatio( target, source2 ), Vector3(10.0f, 10.0f, 10.0f), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( FitKeepAspectRatio( source2, target ), Vector3(0.0333f, 0.0333f, 0.0333f), 0.001, TEST_LOCATION );
}

static void UtcDaliVector3FillKeepAspectRatio()
{
  TestApplication application;
  Vector3 target(10.0f, 20.0f, 30.0f);
  Vector3 source1(1.0f, 2.0f, 3.0f);
  Vector3 source2(1.0f, 1.0f, 1.0f);

  DALI_TEST_EQUALS( FillKeepAspectRatio( target, target ), Vector3(1.0f, 1.0f, 1.0f), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( FillKeepAspectRatio( target, source1 ), Vector3(10.0f, 10.0f, 10.0f), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( FillKeepAspectRatio( target, source2 ), Vector3(30.0f, 30.0f, 30.0f), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( FillKeepAspectRatio( source2, target ), Vector3(0.1f, 0.1f, 0.1f), 0.001, TEST_LOCATION );

}

static void UtcDaliVector3FillXYKeepAspectRatio()
{
  TestApplication application;
  Vector3 target(10.0f, 20.0f, 30.0f);
  Vector3 source1(1.0f, 2.0f, 3.0f);
  Vector3 source2(1.0f, 1.0f, 1.0f);

  DALI_TEST_EQUALS( FillXYKeepAspectRatio( target, target ), Vector3(1.0f, 1.0f, 1.0f), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( FillXYKeepAspectRatio( target, source1 ), Vector3(10.0f, 10.0f, 10.0f), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( FillXYKeepAspectRatio( target, source2 ), Vector3(20.0f, 20.0f, 20.0f), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( FillXYKeepAspectRatio( source2, target ), Vector3(0.1f, 0.1f, 0.1f), 0.001, TEST_LOCATION );

}

static void UtcDaliVector3ShrinkInsideKeepAspectRatio()
{
  TestApplication application;
  Vector3 target(1.0f, 2.0f, 3.0f);
  Vector3 source1(10.0f, 20.0f, 30.0f);
  Vector3 source2(10.0f, 10.0f, 10.0f);

  DALI_TEST_EQUALS( ShrinkInsideKeepAspectRatio( target, target ), Vector3(1.0f, 1.0f, 1.0f), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( ShrinkInsideKeepAspectRatio( target, source1 ), Vector3(0.1f, 0.1f, 0.1f), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( ShrinkInsideKeepAspectRatio( target, source2 ), Vector3(0.1f, 0.1f, 0.1f), 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( ShrinkInsideKeepAspectRatio( source2, target ), Vector3::ONE, 0.001, TEST_LOCATION );
}
