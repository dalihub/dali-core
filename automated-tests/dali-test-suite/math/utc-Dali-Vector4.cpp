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
TEST_FUNCTION( UtcDaliVector4Vector4,            POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector4Add,                POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector4Constants,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector4Cross,              POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector4Dot,                POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector4DotVector3,         POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector4Dot4,               POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector4Equals,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector4Length,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector4LengthSquared,      POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector4Max,                POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector4Min,                POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector4Clamp,              POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector4ClampVector4,       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector4Multiply,           POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector4Divide,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector4Scale,              POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector4Normalize,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector4Subtract,           POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector4OperatorSubscript,  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector4OStreamOperator,    POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliVector4AsFloat,            POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}


// Constructor
static void UtcDaliVector4Vector4()
{
  float f[] = {2.0f, 3.0f, 4.0f, 5.0f};
  Vector2 vec2(1.0f, 2.0f);
  Vector3 vec3(1.0f, 2.0f, 3.0f);
  Vector4 v0(f);
  Vector4 v1(f[0], f[1], f[2], f[3]);
  Vector4 v2(v0);
  Vector4 v3;
  v3 = (const float*) f;
  Vector4 v4(vec2);
  Vector4 v5(vec3);

  DALI_TEST_EQUALS(v0, v1, TEST_LOCATION);
  DALI_TEST_EQUALS(v0, v2, TEST_LOCATION);
  DALI_TEST_EQUALS(v3, v0, TEST_LOCATION);
  DALI_TEST_EQUALS(v4, Vector4(1.0f, 2.0f, 0.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(v5, Vector4(1.0f, 2.0f, 3.0f, 0.0f), TEST_LOCATION);
  DALI_TEST_CHECK(v0 == v1);

  v1 = vec2;
  v2 = vec3;
  DALI_TEST_EQUALS(v1.x, 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(v1.y, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(v2.x, 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(v2.y, 2.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(v2.z, 3.0f, TEST_LOCATION);
}

// Add
static void UtcDaliVector4Add()
{
  Vector4 v0(1.0f, 2.0f, 3.0f, 4.0f);
  Vector4 v1(10.0f, 20.0f, 30.0f, 40.0f);
  Vector4 r0(11.0f, 22.0f, 33.0f, 44.0f);

  Vector4 v2 = v0+v1;
  DALI_TEST_EQUALS(v2, r0, TEST_LOCATION);

  v0 += v1;
  DALI_TEST_EQUALS(v0, r0, TEST_LOCATION);
}

// Constants
static void UtcDaliVector4Constants()
{
  float f[] = {2.0f, 3.0f, 4.0f, 5.0f};
  Vector4 v0(f);
  Vector4 v1(f[0], f[1], f[2], f[3]);
  Vector4 v2(v0);

  DALI_TEST_EQUALS(v0, v1, TEST_LOCATION);
  DALI_TEST_EQUALS(v0, v2, TEST_LOCATION);
  DALI_TEST_CHECK(v0 == v1);
}

// Cross
static void UtcDaliVector4Cross()
{
  DALI_TEST_EQUALS(Vector4::XAXIS.Cross(Vector4::YAXIS), Vector4::ZAXIS, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector4::YAXIS.Cross(Vector4::ZAXIS), Vector4::XAXIS, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector4::ZAXIS.Cross(Vector4::XAXIS), Vector4::YAXIS, 0.0001f, TEST_LOCATION);

  DALI_TEST_EQUALS(Vector4::XAXIS.Cross(Vector4::ZAXIS), -Vector4::YAXIS, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector4::YAXIS.Cross(Vector4::XAXIS), -Vector4::ZAXIS, 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector4::ZAXIS.Cross(Vector4::YAXIS), -Vector4::XAXIS, 0.0001f, TEST_LOCATION);

  Vector4 v0(2.0f, 3.0f, 4.0f, 5.0f);
  Vector4 v1(10.0f, 20.0f, 30.0f, 40.0f);
  Vector4 result(   (v0.y * v1.z) - (v0.z * v1.y),
                    (v0.z * v1.x) - (v0.x * v1.z),
                    (v0.x * v1.y) - (v0.y * v1.x),
                    0.0f);

  DALI_TEST_EQUALS(v0.Cross(v1), result, 0.001f, TEST_LOCATION);
}

// Dot
static void UtcDaliVector4Dot()
{
  DALI_TEST_EQUALS(Vector4::XAXIS.Dot(Vector4::YAXIS), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector4::XAXIS.Dot(Vector4::ZAXIS), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector4::XAXIS.Dot(Vector4::XAXIS), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector4::YAXIS.Dot(Vector4::YAXIS), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector4::ZAXIS.Dot(Vector4::ZAXIS), 1.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(Vector4(1.0f, 0.0f, 0.0f, 1.0f).Dot(Vector4(1.0f, 0.0f, 0.0f, 1.0f)), 1.0f, TEST_LOCATION);

  // Test v0 . v0 and v0 . v1 (v1 is always 90 degrees out of phase with v0)
  for (float x = 0; x<6.0f; x+=1.0f)
  {
    // vectors rotating in the XY plane.
    Vector4 v0(cosf(x), sinf(x), 0.0f, 1.0f);
    Vector4 v1(sinf(x), -cosf(x), 0.0f, 1.0f);
    DALI_TEST_EQUALS(v0.Dot(v1), 0.0f, 0.0001f, TEST_LOCATION);
    DALI_TEST_EQUALS(v0.Dot(v0), 1.0f, 0.0001f, TEST_LOCATION);

    // vectors rotating in the XZ plane.
    v0 = Vector4(cosf(x), 0.0f, sinf(x), 0.0f);
    v1 = Vector4(sinf(x), 0.0f, -cosf(x), 0.0f);
    DALI_TEST_EQUALS(v0.Dot(v1), 0.0f, 0.0001f, TEST_LOCATION);
    DALI_TEST_EQUALS(v0.Dot(v0), 1.0f, 0.0001f, TEST_LOCATION);
  }

  Vector4 v0 = Vector4(12.0f, 7.0f, 9.0f, 14.0f);
  v0.Normalize();

  Vector4 v1 = v0 * 2.0f;
  DALI_TEST_EQUALS(v0.Dot(v1), 2.0f, 0.001f, TEST_LOCATION);
}

// Dot (with vector3)
static void UtcDaliVector4DotVector3()
{
  DALI_TEST_EQUALS(Vector4::XAXIS.Dot(Vector3::YAXIS), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector4::XAXIS.Dot(Vector3::ZAXIS), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector4::XAXIS.Dot(Vector3::XAXIS), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector4::YAXIS.Dot(Vector3::YAXIS), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector4::ZAXIS.Dot(Vector3::ZAXIS), 1.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(Vector4(1.0f, 0.0f, 0.0f, 1.0f).Dot(Vector3(1.0f, 0.0f, 0.0f)), 1.0f, TEST_LOCATION);

  // Test v0 . v0b and v0 . v1 (v1 is always 90 degrees out of phase with v0)
  for (float x = 0; x<6.0f; x+=1.0f)
  {
    // vectors rotating in the XY plane.
    Vector4 v0(cosf(x), sinf(x), 0.0f, 1.0f);
    Vector3 v0b(cosf(x), sinf(x), 0.0f);
    Vector3 v1(sinf(x), -cosf(x), 0.0f);
    DALI_TEST_EQUALS(v0.Dot(v1), 0.0f, 0.0001f, TEST_LOCATION);
    DALI_TEST_EQUALS(v0.Dot(v0b), 1.0f, 0.0001f, TEST_LOCATION);

    // vectors rotating in the XZ plane.
    v0 = Vector4(cosf(x), 0.0f, sinf(x), 0.0f);
    v0b = Vector3(cosf(x), 0.0f, sinf(x));
    v1 = Vector3(sinf(x), 0.0f, -cosf(x));
    DALI_TEST_EQUALS(v0.Dot(v1), 0.0f, 0.0001f, TEST_LOCATION);
    DALI_TEST_EQUALS(v0.Dot(v0b), 1.0f, 0.0001f, TEST_LOCATION);
  }

  Vector4 v0 = Vector4(12.0f, 7.0f, 9.0f, 14.0f);
  v0.Normalize();

  Vector3 v1(v0 * 2.0f);
  DALI_TEST_EQUALS(v0.Dot(v1), 2.0f, 0.001f, TEST_LOCATION);
}

// Dot4
static void UtcDaliVector4Dot4()
{
  DALI_TEST_EQUALS(Vector4::XAXIS.Dot4(Vector4::YAXIS), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector4::XAXIS.Dot4(Vector4::ZAXIS), 0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector4::YAXIS.Dot4(Vector4::ZAXIS), 0.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(Vector4::XAXIS.Dot4(Vector4::XAXIS), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector4::YAXIS.Dot4(Vector4::YAXIS), 1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector4::ZAXIS.Dot4(Vector4::ZAXIS), 1.0f, TEST_LOCATION);

  DALI_TEST_EQUALS(Vector4(1.0f, 0.0f, 0.0f, 1.0f).Dot4(Vector4(1.0f, 0.0f, 0.0f, 1.0f)), 2.0f, TEST_LOCATION);

  for (float x = 0; x<6.0f; x+=1.0f)
  {
    Vector4 v0(cosf(x), sinf(x), 0.0f, 1.0f);
    Vector4 v1(sinf(x), -cosf(x), 0.0f, 1.0f);
    DALI_TEST_EQUALS(v0.Dot4(v1), 1.0f, 0.0001f, TEST_LOCATION);
    DALI_TEST_EQUALS(v0.Dot4(v0), 2.0f, 0.0001f, TEST_LOCATION);

    v0 = Vector4(cosf(x), 0.0f, sinf(x), 0.0f);
    v1 = Vector4(sinf(x), 0.0f, -cosf(x), 0.0f);
    DALI_TEST_EQUALS(v0.Dot4(v1), 0.0f, 0.0001f, TEST_LOCATION);
    DALI_TEST_EQUALS(v0.Dot4(v0), 1.0f, 0.0001f, TEST_LOCATION);
  }

  Vector4 v0(12.0f, 7.0f, 9.0f, 3.0f);
  v0.Normalize();

  Vector4 v1 = v0 * 2.0f;
  DALI_TEST_EQUALS(v0.Dot4(v1), 2.0f + 3.0f*6.0f, 0.001f, TEST_LOCATION);
}

// Equals
static void UtcDaliVector4Equals()
{
  Vector4 v0(1.0f, 2.0f,  3.0f, 4.0f);
  Vector4 v1(1.0f, 2.0f,  3.0f, 4.0f);

  DALI_TEST_CHECK(v0 == v1);

  Vector4 v2 = Vector4(0.0f, 2.0f, 3.0f, 4.0f);
  DALI_TEST_CHECK(v0 != v2);

  v2 = Vector4(1.0f, 0.0f, 3.0f, 4.0f);
  DALI_TEST_CHECK(v0 != v2);

  v2 = Vector4(1.0f, 2.0f, 0.0f, 4.0f);
  DALI_TEST_CHECK(v0 != v2);

  v2 = Vector4(1.0f, 2.0f, 3.0f, 0.0f);
  DALI_TEST_CHECK(v0 != v2);
}

#if 0
// Equals
static void UtcDaliVector4IsEquivalent()
{
  Vector4 v0(1.0f, 2.0f, 3.0f, 4.0f);

  DALI_TEST_CHECK(v0.IsEquivalent(v0));
  DALI_TEST_CHECK(!v0.IsEquivalent(Vector4(1.1f, 2.0f, 3.0f, 4.0f)));
  DALI_TEST_CHECK(!v0.IsEquivalent(Vector4(1.0f, 2.1f, 3.0f, 4.0f)));
  DALI_TEST_CHECK(!v0.IsEquivalent(Vector4(1.0f, 2.0f, 3.1f, 4.0f)));
  DALI_TEST_CHECK(!v0.IsEquivalent(Vector4(1.0f, 2.0f, 3.0f, 4.1f)));
}
#endif

// Length
static void UtcDaliVector4Length()
{
  Vector4 v(1.0f, 2.0f, 3.0f, 4.0f);
  DALI_TEST_EQUALS(v.Length(), sqrtf(v.x*v.x + v.y*v.y + v.z*v.z), 0.001f, TEST_LOCATION);

  Vector4 v1(0.0f, 0.0f, 0.0f, 0.0f);
  DALI_TEST_EQUALS(v1.Length(), 0.0f, TEST_LOCATION);
}

// Length squared
static void UtcDaliVector4LengthSquared()
{
  Vector4 v(1.0f, 2.0f, 3.0f, 4.0f);
  DALI_TEST_EQUALS(v.LengthSquared(), v.x*v.x + v.y*v.y + v.z*v.z, 0.001f, TEST_LOCATION);

  Vector4 v1(0.0f, 0.0f, 0.0f, 0.0f);
  DALI_TEST_EQUALS(v1.LengthSquared(), 0.0f, TEST_LOCATION);
}

// Max
static void UtcDaliVector4Max()
{
  Vector4 v0(2.0f, 2.0f, 1.0f, 1.0f);
  Vector4 v1(1.0f, 1.0f, 2.0f, 2.0f);

  DALI_TEST_EQUALS(Max(v0, v1), Vector4(2.0f, 2.0f, 2.0f, 2.0f), 0.01f, TEST_LOCATION);
}

// Min
static void UtcDaliVector4Min()
{
  Vector4 v0(2.0f, 2.0f, 1.0f, 1.0f);
  Vector4 v1(1.0f, 1.0f, 2.0f, 2.0f);

  DALI_TEST_EQUALS(Min(v0, v1), Vector4(1.0f, 1.0f, 1.0f, 1.0f), 0.01f, TEST_LOCATION);
}

// Clamp
static void UtcDaliVector4Clamp()
{
  tet_infoline("Testing Dali::Vector4::Clamp()");

  Vector4 v0(2.0f, 2.0f, -2.0f, -2.0f);
  DALI_TEST_EQUALS(Clamp(v0, -1.0f, 1.0f), Vector4(1.0f, 1.0f, -1.0f, -1.0f), 0.01f, TEST_LOCATION);

  Vector4 v1(1.0f, 0.0f, 0.0f, -1.0f);
  DALI_TEST_EQUALS(Clamp(v1, -1.0f, 1.0f), v1, 0.01f, TEST_LOCATION);

}

static void UtcDaliVector4ClampVector4()
{
  tet_infoline("Testing Dali::Vector4::Clamp( const Vector4& v, const Vector4& min, const Vector4& max )");

  Vector4 v0(2.0f, 0.8f, 0.0f, 5.0f);
  Vector4 v1(-1.0f, 2.0f, 10.0f, -10.0f);
  Vector4 v2(10.0f, 5.0f, 0.0f, 10.0f);
  Vector4 v3(8.0f, 10.0f, 5.0f, -20.0f);
  Vector4 v4(4.9f, 5.1f, 10.0f, 0.0f);

  Vector4 min(1.0f, -2.0f, -8.0f, -16.0f);
  Vector4 max(2.0f, 4.0f, 4.0f, -8.0f);

  v0.Clamp( min, max );
  v1.Clamp( min, max );
  v2.Clamp( min, max );
  v3.Clamp( min, max );
  v4.Clamp( min, max );

  DALI_TEST_EQUALS( v0, Vector4( 2.0f, 0.8f, 0.0f, -8.0f), 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( v1, Vector4( 1.0f, 2.0f, 4.0f, -10.0f), 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( v2, Vector4( 2.0f, 4.0f, 0.0f, -8.0f), 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( v3, Vector4( 2.0f, 4.0f, 4.0f, -16.0f), 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( v4, Vector4( 2.0f, 4.0f, 4.0f, -8.0f), 0.01f, TEST_LOCATION );
}

// Multiply
static void UtcDaliVector4Multiply()
{
  Vector4 v0(2.0f, 3.0f, 4.0f, 5.0f);
  Vector4 v1(10.0f, 20.0f,  30.0f,  40.0f);
  Vector4 r0(20.0f, 60.0f, 120.0f, 200.0f);

  Vector4 v2 = v0 * v1;
  DALI_TEST_EQUALS(v2, r0, TEST_LOCATION);

  v0 *= v1;
  DALI_TEST_EQUALS(v0, r0, TEST_LOCATION);
}

// Divide
static void UtcDaliVector4Divide()
{
  Vector4 v0(1.0f, 1.0f, 1.0f, 1.0f);
  Vector4 v1(2.0f, 3.0f, 5.0f, 7.0f);
  Vector4 v2(4.0f, 9.0f, 25.0f, 49.0f);

  DALI_TEST_EQUALS( v0/v0, v0, TEST_LOCATION);
  DALI_TEST_EQUALS( v1/v0, v1, TEST_LOCATION);
  DALI_TEST_EQUALS( v1/v1, v0, TEST_LOCATION);
  DALI_TEST_EQUALS( v2/v1, v1, TEST_LOCATION);

  Vector4 v4(v0);
  v4 /= v0;
  DALI_TEST_EQUALS(v4, v0, TEST_LOCATION);

  Vector4 v5(v1);
  v5 /= v0;
  DALI_TEST_EQUALS(v5, v1, TEST_LOCATION);

  Vector4 v6(v1);
  v6 /= v6;
  DALI_TEST_EQUALS(v6, v0, TEST_LOCATION);

  v2 /= v1;
  DALI_TEST_EQUALS(v2, v1, TEST_LOCATION);

}

static void UtcDaliVector4Scale()
{
  Vector4 v0(2.0f,   4.0f,  8.0f,  16.0f);
  const Vector4 r0(20.0f, 40.0f, 80.0f, 160.0f);
  const Vector4 r1(10.0f, 20.0f, 40.0f,  80.0f);
  const Vector4 r2( 1.0f,  2.0f,  4.0f,   8.0f);
  const Vector4 r3(2.0f,   4.0f,  8.0f,  16.0f);

  Vector4 v2 = v0 * 10.0f;
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
static void UtcDaliVector4Normalize()
{
  for (float f=0.0f; f<6.0f; f+=1.0f)
  {
    Vector4 v(cosf(f)*10.0f, cosf(f+1.0f)*10.0f, cosf(f+2.0f)*10.0f, 1.0f);
    v.Normalize();
    DALI_TEST_EQUALS(v.LengthSquared(), 1.0f, 0.001f, TEST_LOCATION);
  }

  Vector4 v(0.0f, 0.0f, 0.0f, 1.0f);
  v.Normalize();
  DALI_TEST_EQUALS(v.LengthSquared(), 0.0f, 0.00001f, TEST_LOCATION);
}

// Subtract
static void UtcDaliVector4Subtract()
{
  Vector4 v0(11.0f, 22.0f, 33.0f, 44.0f);
  Vector4 v1(10.0f, 20.0f, 30.0f, 40.0f);
  Vector4 r0(1.0f, 2.0f, 3.0f, 4.0f);

  Vector4 v2 = v0-v1;
  DALI_TEST_EQUALS(v2, r0, TEST_LOCATION);

  v0 -= v1;
  DALI_TEST_EQUALS(v0, r0, TEST_LOCATION);
}

static void UtcDaliVector4OperatorSubscript()
{
  Vector4 testVector(1.0f, 2.0f, 3.0f, 4.0f);

  // read array subscripts
  DALI_TEST_EQUALS( testVector[0], 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( testVector[1], 2.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( testVector[2], 3.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( testVector[3], 4.0f, TEST_LOCATION );

  // write array subscripts/read struct memebers
  testVector[0] = 5.0f;
  testVector[1] = 6.0f;
  testVector[2] = 7.0f;
  testVector[3] = 8.0f;

  DALI_TEST_EQUALS( testVector.x, 5.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( testVector.y, 6.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( testVector.z, 7.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( testVector.w, 8.0f, TEST_LOCATION );

  // write struct members/read array subscripts
  testVector.x = 9.0f;
  testVector.y = 10.0f;
  testVector.z = 11.0f;
  testVector.w = 12.0f;

  DALI_TEST_EQUALS( testVector[0],  9.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( testVector[1], 10.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( testVector[2], 11.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( testVector[3], 12.0f, TEST_LOCATION );


  // write struct members/read array subscripts
  const Vector4 testVector2(1.0f, 2.0f, 3.0f, 4.0f);
  const float& x = testVector2[0];
  const float& y = testVector2[1];
  const float& z ( testVector2[2] );
  const float& w ( testVector2[3] );

  DALI_TEST_EQUALS( x, 1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( y, 2.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( z, 3.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( w, 4.0f, TEST_LOCATION );

  try
  {
    float& w = testVector[4];
    if(w==0.0f);
    tet_result(TET_FAIL);
  }
  catch (Dali::DaliException& e)
  {
    tet_printf("Assertion %s failed at %s\n", e.mCondition.c_str(), e.mLocation.c_str());
    DALI_TEST_ASSERT( e, "index < 4", TEST_LOCATION );
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
    DALI_TEST_ASSERT( e, "index < 4", TEST_LOCATION );
  }
}

static void UtcDaliVector4OStreamOperator()
{
  std::ostringstream oss;

  Vector4 vector(1.0f, 2.0f, 3.0f, 4.0f);

  oss << vector;

  std::string expectedOutput = "[1, 2, 3, 4]";

  DALI_TEST_EQUALS( oss.str(), expectedOutput, TEST_LOCATION);
}

static void UtcDaliVector4AsFloat()
{
  float values[] = {0.0f,  1.0f,  2.0f, 3.0f};
  Vector4 v0(values);

  for (int i=0;i<4;++i)
  {
    DALI_TEST_EQUALS(v0.AsFloat()[i], values[i], TEST_LOCATION);
  }

  const Vector4 v1(values);
  for (int i=0;i<4;++i)
  {
    DALI_TEST_EQUALS(v1.AsFloat()[i], values[i], TEST_LOCATION);
  }
}
