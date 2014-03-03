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
#include <dali/public-api/math/quaternion.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static void UtcDaliQuaternionOStreamOperator();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliQuaternionCtor01,                   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionCtor02,                   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionCtor03,                   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionFromAxisAngle,            POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionToAxisAngle01,            POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionToAxisAngle02,            NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionToAxisAngle03,            POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionToAxisAngle04,            NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionEulerAngles,              POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionLength,                   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionLengthSquared,            POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionNormalize,                POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionNormalized,               POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionConjugate,                POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionInvert,                   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionToMatrix01,               POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionToMatrix02,               POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionFromMatrix01,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionFromMatrix02,             POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionFromAxes01,               POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionFromAxes02,               POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionOperatorAddition,         POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionOperatorSubtraction,      POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionOperatorMultiplication01, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionOperatorMultiplication02, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionOperatorScale01,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionOperatorScale02,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionOperatorNegation,         POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionOperatorAddAssign,        POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionOperatorSubtractAssign,   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionOperatorMultiplyAssign,   POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionOperatorScaleAssign01,    POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionOperatorScaleAssign02,    POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionOperatorDivision,         POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionOperatorEquality,         POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionOperatorInequality,       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionDot,                      POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionExp01,                    POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionExp02,                    POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionExp03,                    NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionLog01,                    POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionLog02,                    POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionRotate01,                 POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionRotate02,                 POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionLerp,                     POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionSlerp01,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionSlerp02,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionSlerp03,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionSlerp04,                  POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionSlerpNoInvert01,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionSlerpNoInvert02,          POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionSquad,                    POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliAngleBetween,                       POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliQuaternionOStreamOperator,          POSITIVE_TC_IDX );


// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}


static void UtcDaliQuaternionCtor01()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion r;
  DALI_TEST_EQUALS(r.AsVector().w,  1.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(r.AsVector().x,  0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(r.AsVector().y,  0.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(r.AsVector().z,  0.0f, TEST_LOCATION);
}

static void UtcDaliQuaternionCtor02()
{
  TestApplication application; // Reset all test adapter return codes

  Quaternion r(M_PI/2.0f, Vector4(1.0f, 2.0f, 3.0f, M_PI/3.0f));

  // This will be normalised:
  DALI_TEST_EQUALS(r.AsVector().w,  0.707f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(r.AsVector().x,  0.189f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(r.AsVector().y,  0.378f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(r.AsVector().z,  0.567f, 0.001, TEST_LOCATION);
}


static void UtcDaliQuaternionCtor03()
{
  TestApplication application; // Reset all test adapter return codes

  // Test from euler angles
  Quaternion e1(Radian(Degree(45)), 0.0f, 0.0f);
  Vector4 r1(0.383f, 0.0f, 0.0f, 0.924f);

  Quaternion e2(0.0f, Radian(Degree(75)), 0.0f);
  Vector4 r2(0.0f, 0.609f, 0.0f, 0.793f);

  Quaternion e3(0.0f, 0.0f, Radian(Degree(135)));
  Vector4 r3(0.0f, 0.0f, 0.924f, 0.383f);

  Quaternion e4(Radian(Degree(71)), Radian(Degree(36)), Radian(Degree(27)));
  Vector4 r4(0.478f, 0.374f, 0.006f, 0.795f);

  Quaternion e5(Radian(Degree(-31)), Radian(Degree(-91)), Radian(Degree(-173)));
  Vector4 r5(-0.697f, 0.145f, -0.686f, -0.149f);

  DALI_TEST_EQUALS(e1.AsVector(), r1, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(e2.AsVector(), r2, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(e3.AsVector(), r3, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(e4.AsVector(), r4, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(e5.AsVector(), r5, 0.001, TEST_LOCATION);
}


static void UtcDaliQuaternionFromAxisAngle()
{
  TestApplication application; // Reset all test adapter return codes

  Quaternion q = Quaternion::FromAxisAngle(Vector4(1.0f, 2.0f, 3.0f, M_PI/3.0f), M_PI/2.0f);

  Quaternion r(0.707f, 0.189f, 0.378f, 0.567f);

  DALI_TEST_EQUALS(q, r, 0.001, TEST_LOCATION);
}

static void UtcDaliQuaternionToAxisAngle01()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q(0.932f, 1.1f, 3.4f, 2.7f);
  float angle;
  Vector3 axis;
  bool converted = q.ToAxisAngle(axis, angle);
  DALI_TEST_EQUALS(converted, true, TEST_LOCATION);
  DALI_TEST_EQUALS(angle,  0.74f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.x, 3.03f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.y, 9.38f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.z, 7.45f, 0.01f, TEST_LOCATION);
}

static void UtcDaliQuaternionToAxisAngle02()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q(0.932f, 1.1f, 3.4f, 2.7f);
  float angle;
  Vector4 axis;
  bool converted = q.ToAxisAngle(axis, angle);
  DALI_TEST_EQUALS(converted, true, TEST_LOCATION);
  DALI_TEST_EQUALS(angle,  0.74f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.x, 3.03f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.y, 9.38f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.z, 7.45f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.w, 0.0f, 0.01f, TEST_LOCATION);
}


static void UtcDaliQuaternionToAxisAngle03()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q(1, 2, 3, 4);
  float angle;
  Vector3 axis;
  bool converted = q.ToAxisAngle(axis, angle);
  DALI_TEST_EQUALS(converted, false, TEST_LOCATION);
  DALI_TEST_EQUALS(angle,  0.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.x, 0.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.y, 0.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.z, 0.0f, 0.01f, TEST_LOCATION);
}

static void UtcDaliQuaternionToAxisAngle04()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q(1, 2, 3, 4);
  float angle;
  Vector4 axis;
  bool converted = q.ToAxisAngle(axis, angle);
  DALI_TEST_EQUALS(converted, false, TEST_LOCATION);
  DALI_TEST_EQUALS(angle,  0.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.x, 0.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.y, 0.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.z, 0.0f, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.w, 0.0f, 0.01f, TEST_LOCATION);
}



static void UtcDaliQuaternionEulerAngles()
{
  TestApplication application; // Reset all test adapter return codes

  Quaternion q1(0.924f, 0.383f, 0.0f, 0.0f);
  Vector4 r1(Radian(Degree(45)), 0.0f, 0.0f, 0.0f);

  Quaternion q2(0.793f, 0.0f, 0.609f, 0.0f);
  Vector4 r2(0.0f, Radian(Degree(75)), 0.0f, 0.0f);

  Quaternion q3(0.383f, 0.0f, 0.0f, 0.924f);
  Vector4 r3(0.0f, 0.0f, Radian(Degree(135)), 0.0f);

  Quaternion q4(0.795f, 0.478f, 0.374f, 0.006f);
  Vector4 r4(Radian(Degree(71)), Radian(Degree(36)), Radian(Degree(27)), 0.0f);

  Quaternion q5( -0.149f, -0.697f, 0.145f, -0.686f);
  Vector4 r5(Radian(Degree(148.0)), Radian(Degree(-88.2)), Radian(Degree(8.0)), 0.0f);

  DALI_TEST_EQUALS(q1.EulerAngles(), r1, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(q2.EulerAngles(), r2, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(q3.EulerAngles(), r3, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(q4.EulerAngles(), r4, 0.01,  TEST_LOCATION);
  DALI_TEST_EQUALS(q5.EulerAngles(), r5, 0.01,  TEST_LOCATION);
}


static void UtcDaliQuaternionToMatrix01()
{
  TestApplication application; // Reset all test adapter return codes

  Quaternion q(0.69813, Vector4(1.0f, 0.0f, 0.0f, 0.0f)); // 40 degree rotation around X axis

  // Result calculated using a different maths library (with appropriate row/col ordering)

  float els[] = { 1.0f,  0.0f,   0.0f,  0.0f,
                  0.0f,  0.766f, 0.643f, 0.0f,
                  0.0f, -0.643f, 0.766f, 0.0f,
                  0.0f,  0.0f,   0.0f,  1.0f };
  Matrix mRes(els);
  Matrix m(q);

  DALI_TEST_EQUALS(m, mRes, 0.01, TEST_LOCATION);
}

static void UtcDaliQuaternionToMatrix02()
{
  TestApplication application; // Reset all test adapter return codes

  // rotation around arbitrary axis
  Quaternion q2(-1.23918f, Vector4(7.0f, -13.0f, 11.0f, 0.0f));

  float els[] = {  0.423f, -0.746f, -0.514f,  0.00f,
                   0.384f,  0.662f, -0.644f,  0.00f,
                   0.821f,  0.075f,  0.566f,  0.00f,
                   0.000f,  0.000f,  0.000f,  1.00f };
  Matrix mRes2(els);

  Matrix m2(q2);

  DALI_TEST_EQUALS(m2, mRes2, 0.01, TEST_LOCATION);
}


static void UtcDaliQuaternionFromMatrix01()
{
  TestApplication application; // Reset all test adapter return codes

  // IDENTITY rotation
  Quaternion q;

  Matrix m(q);     // Convert to matrix

  Quaternion q2(m); // and back to a quaternion

  DALI_TEST_EQUALS(q, q2, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(m, Matrix::IDENTITY, 0.001f, TEST_LOCATION);
}

static void UtcDaliQuaternionFromMatrix02()
{
  TestApplication application; // Reset all test adapter return codes

  // Create an arbitrary forward vector
  for( float x=-1.0f; x<=1.0f; x+=0.1f )
  {
    for( float y=-1.0f; y<1.0f; y+=0.1f )
    {
      for( float z=-1.0f; z<1.0f; z+=0.1f )
      {
        Vector3 vForward(x, y, z);
        vForward.Normalize();

        // Construct an up vector from a sideways move
        Vector3 vSide;
        Vector3 vUp = vForward.Cross(Vector3(vForward.x+1.0f, vForward.y, vForward.z));
        if(vUp.Length() > 0.01 )
        {
          vUp.Normalize();
          vSide = vUp.Cross(vForward);
          vSide.Normalize();
        }
        else
        {
          vSide = vForward.Cross(Vector3(vForward.x, vForward.y+1.0f, vForward.z));
          vSide.Normalize();
          vUp = vForward.Cross(vSide);
          vUp.Normalize();
        }

        // Generate a matrix, and then a quaternion from it
        Matrix rotMatrix(Matrix::IDENTITY);
        rotMatrix.SetXAxis(vSide);
        rotMatrix.SetYAxis(vUp);
        rotMatrix.SetZAxis(vForward);
        Quaternion q( rotMatrix );

        // Generate a matrix from the quaternion, check they are the same
        Matrix resultMatrix(q);
        DALI_TEST_EQUALS(resultMatrix, rotMatrix, 0.001f, TEST_LOCATION);

        // Rotate an arbitrary vector by both quaternion and rotation matrix,
        // check the result is the same

        Vector4 aVector(-2.983f, -3.213f, 8.2239f, 1.0f);
        Vector3 aVectorRotatedByQ = q.Rotate(Vector3(aVector));
        Vector4 aVectorRotatedByR = rotMatrix*aVector;
        DALI_TEST_EQUALS(aVectorRotatedByQ, Vector3(aVectorRotatedByR), 0.001f, TEST_LOCATION);
      }
    }
  }
}

static void UtcDaliQuaternionFromAxes01()
{
  TestApplication application; // Reset all test adapter return codes

  Vector3 xAxis( Vector3::XAXIS );
  Vector3 yAxis( Vector3::YAXIS );
  Vector3 zAxis( Vector3::ZAXIS );

  Quaternion q1( xAxis, yAxis, zAxis);

  DALI_TEST_EQUALS( q1, Quaternion::IDENTITY, TEST_LOCATION );

  xAxis = Vector3( 1.0f, 1.0f, 0.0f );
  xAxis.Normalize();
  yAxis = Vector3( -1.0f, 1.0f, 0.0f ); // 45 degrees anticlockwise (+ve) around z
  yAxis.Normalize();
  zAxis = xAxis.Cross(yAxis);
  zAxis.Normalize();
  Quaternion q2( xAxis, yAxis, zAxis );

  DALI_TEST_EQUALS( q2, Quaternion(Radian(Degree(45)), Vector3::ZAXIS), 0.001f, TEST_LOCATION);
}


static void UtcDaliQuaternionFromAxes02()
{
  TestApplication application; // Reset all test adapter return codes

  // Create an arbitrary forward vector
  for( float x=-1.0f; x<=1.0f; x+=0.1f )
  {
    for( float y=-1.0f; y<1.0f; y+=0.1f )
    {
      for( float z=-1.0f; z<1.0f; z+=0.1f )
      {
        Vector3 vForward(x, y, z);
        vForward.Normalize();

        // Construct an up vector from a sideways move
        Vector3 vSide;
        Vector3 vUp = vForward.Cross(Vector3(vForward.x+1.0f, vForward.y, vForward.z));
        if(vUp.Length() > 0.01 )
        {
          vUp.Normalize();
          vSide = vUp.Cross(vForward);
          vSide.Normalize();
        }
        else
        {
          vSide = vForward.Cross(Vector3(vForward.x, vForward.y+1.0f, vForward.z));
          vSide.Normalize();
          vUp = vForward.Cross(vSide);
          vUp.Normalize();
        }

        // Generate a quaternion
        Quaternion q( vSide, vUp, vForward );

        Matrix rotMatrix;
        rotMatrix.SetXAxis(vSide);
        rotMatrix.SetYAxis(vUp);
        rotMatrix.SetZAxis(vForward);

        // Generate a matrix from the quaternion, check they are the same
        Matrix m(q);
        DALI_TEST_EQUALS(m.GetXAxis(), vSide, 0.001f, TEST_LOCATION);
        DALI_TEST_EQUALS(m.GetYAxis(), vUp, 0.001f, TEST_LOCATION);
        DALI_TEST_EQUALS(m.GetZAxis(), vForward, 0.001f, TEST_LOCATION);

        // Rotate an arbitrary vector by both quaternion and rotation matrix,
        // check the result is the same

        Vector4 aVector(2.043f, 12.8f, -3.872f, 1.0f);
        Vector3 aVectorRotatedByQ = q.Rotate(Vector3(aVector));
        Vector4 aVectorRotatedByR = rotMatrix*aVector;
        DALI_TEST_EQUALS(aVectorRotatedByQ, Vector3(aVectorRotatedByR), 0.001f, TEST_LOCATION);
      }
    }
  }
}

static void UtcDaliQuaternionOperatorAddition()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q1(0.383f, 0.0f, 0.0f, 0.924f);
  Quaternion q2(0.0f, 0.609f, 0.0f, 0.793f);

  Quaternion r1(0.383f, 0.609f, 0.0f, 1.717f);

  DALI_TEST_EQUALS(q1+q2, r1, 0.001f, TEST_LOCATION);
}

static void UtcDaliQuaternionOperatorSubtraction()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q1(0.383f, 0.450f, 0.123f, 0.924f);
  Quaternion q2(0.383f, 0.690f, 0.234f, 1.917f);

  Quaternion r1(0.0f, 0.240f, 0.111f, 0.993f);

  DALI_TEST_EQUALS(q2-q1, r1, 0.001f, TEST_LOCATION);
}

static void UtcDaliQuaternionConjugate()
{
  TestApplication application; // Reset all test adapter return codes
  float s1=0.784f; Vector3 v1(0.045f, 0.443f, 0.432f);
  float s2=0.697f; Vector3 v2(0.612, 0.344, -0.144);

  Quaternion q1(s1, v1.x, v1.y, v1.z);
  Quaternion q2(s2, v2.x, v2.y, v2.z);
  q1.Conjugate();
  q2.Conjugate();

  Quaternion r1(s1, -v1.x, -v1.y, -v1.z);
  Quaternion r2(s2, -v2.x, -v2.y, -v2.z);

  DALI_TEST_EQUALS(q1, r1, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(q2, r2, 0.001f, TEST_LOCATION);
}

static void UtcDaliQuaternionOperatorMultiplication01()
{
  TestApplication application; // Reset all test adapter return codes
  float s1=0.784f; Vector3 v1(0.045f, 0.443f, 0.432f);
  float s2=0.697f; Vector3 v2(0.612, 0.344, -0.144);

  Quaternion q1(s1, v1.x, v1.y, v1.z);
  Quaternion q2(s2, v2.x, v2.y, v2.z);

  Vector3 vp = v1.Cross(v2) + v2*s1 + v1*s2;
  Quaternion r1(s1*s2-v1.Dot(v2), vp.x, vp.y, vp.z);

  DALI_TEST_EQUALS(q1*q2, r1, 0.001f, TEST_LOCATION);
}

static void UtcDaliQuaternionOperatorDivision()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q1(0.383f, 0.0f, 0.0f, 0.924f);
  Quaternion q2(0.0f, 0.609f, 0.0f, 0.793f);

  // q1 / q2 = q1 * q2^-1
  // q2^-1 = q2* / ||q2||^2
  //       = Conjugate of q2 / Square of Norm of q2

  Quaternion r1 = q2;
  r1.Conjugate();
  r1 *= 1.0f/q2.LengthSquared();
  Quaternion r2 = q1 * r1;

  DALI_TEST_EQUALS(q1 / q2, r2, 0.001f, TEST_LOCATION);
}

static void UtcDaliQuaternionOperatorScale01()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q1(0.383f, 0.0f, 0.0f, 0.924f);
  Quaternion r1(2.0f* 0.383f, 0.0f, 0.0f, 2.0f * 0.924f);

  DALI_TEST_EQUALS(q1 * 2.0f, r1, 0.001f, TEST_LOCATION);
}

static void UtcDaliQuaternionOperatorScale02()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q1(0.383f, 0.0f, 0.0f, 0.924f);
  Quaternion r1(0.5f* 0.383f, 0.0f, 0.0f, 0.5f * 0.924f);

  DALI_TEST_EQUALS(q1 / 2.0f, r1, 0.001f, TEST_LOCATION);
}

static void UtcDaliQuaternionOperatorNegation()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q1(0.383f, 0.0f, 0.0f, 0.924f);
  Quaternion r1(-0.383f, -0.0f, -0.0f, -0.924f);

  DALI_TEST_EQUALS(-q1, r1, 0.001f, TEST_LOCATION);
}

static void UtcDaliQuaternionOperatorAddAssign()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q1(0.383f, 0.0f, 0.0f, 0.924f);
  Quaternion q2(0.0f, 0.609f, 0.0f, 0.793f);

  Quaternion r1(0.383f, 0.609f, 0.0f, 1.717f);

  q1 += q2;
  DALI_TEST_EQUALS(q1, r1, 0.001f, TEST_LOCATION);
}

static void UtcDaliQuaternionOperatorSubtractAssign()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q1(0.383f, 0.450f, 0.123f, 0.924f);
  Quaternion q2(0.383f, 0.690f, 0.234f, 1.917f);

  Quaternion r1(0.0f, 0.240f, 0.111f, 0.993f);
  q2 -= q1;
  DALI_TEST_EQUALS(q2, r1, 0.001f, TEST_LOCATION);
}

static void UtcDaliQuaternionOperatorMultiplyAssign()
{
  TestApplication application; // Reset all test adapter return codes
  float s1=0.784f; Vector3 v1(0.045f, 0.443f, 0.432f);
  float s2=0.697f; Vector3 v2(0.612, 0.344, -0.144);

  Quaternion q1(s1, v1.x, v1.y, v1.z);
  Quaternion q2(s2, v2.x, v2.y, v2.z);

  Quaternion r3 = q2 * q1;
  q2 *= q1;
  DALI_TEST_EQUALS(q2, r3, 0.001f, TEST_LOCATION);
}

static void UtcDaliQuaternionOperatorScaleAssign01()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q1(0.383f, 0.450f, 0.123f, 0.924f);
  float scale = 2.5f;
  Quaternion r1(scale*0.383f, scale*0.450f, scale*0.123f, scale*0.924f);
  q1 *= scale;
  DALI_TEST_EQUALS(q1, r1, 0.001f, TEST_LOCATION);
}

static void UtcDaliQuaternionOperatorScaleAssign02()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q1(0.383f, 0.450f, 0.123f, 0.924f);
  float scale = 2.5f;
  Quaternion r1(0.383f/scale, 0.450f/scale, 0.123f/scale, 0.924f/scale);
  q1 /= scale;
  DALI_TEST_EQUALS(q1, r1, 0.001f, TEST_LOCATION);
}

static void UtcDaliQuaternionOperatorEquality()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q1(0.383f, 0.450f, 0.123f, 0.924f);
  Quaternion q2(0.383f, 0.450f, 0.123f, 0.924f);
  Quaternion q3(0.383f, 0.450f, 0.123f, 0.800f);
  Quaternion q4(0.383f, 0.450f, 0.100f, 0.800f);
  Quaternion q5(0.383f, 0.100f, 0.100f, 0.800f);
  Quaternion q6(0.100f, 0.100f, 0.100f, 0.800f);

  Quaternion q7(-0.383f, -0.450f, -0.123f, -0.924f);
  Quaternion q8(-0.383f, -0.450f, -0.123f,  0.924f);
  Quaternion q9(-0.383f, -0.450f,  0.123f,  0.924f);
  Quaternion q10(-0.383f,  0.450f,  0.123f,  0.924f);

  DALI_TEST_CHECK( q1 == q2 );
  DALI_TEST_CHECK( !(q1 == q3) );
  DALI_TEST_CHECK( !(q1 == q4) );
  DALI_TEST_CHECK( !(q1 == q5) );
  DALI_TEST_CHECK( !(q1 == q6) );
  DALI_TEST_CHECK( (q1 == q7) );
  DALI_TEST_CHECK( !(q1 == q8) );
  DALI_TEST_CHECK( !(q1 == q9) );
  DALI_TEST_CHECK( !(q1 == q10) );
}

static void UtcDaliQuaternionOperatorInequality()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q1(0.383f, 0.450f, 0.123f, 0.924f);
  Quaternion q2(0.383f, 0.450f, 0.123f, 0.924f);
  Quaternion q3(-0.383f, -0.0f, -0.0f, -0.924f);
  DALI_TEST_CHECK( !(q1 != q2) );
  DALI_TEST_CHECK( q1 != q3 );
}

static void UtcDaliQuaternionLength()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q1(0.383f, 0.450f, 0.123f, 0.924f);
  float length = sqrtf(0.383f*0.383f + 0.450f*0.450f + 0.123f*0.123f + 0.924f*0.924f);
  DALI_TEST_EQUALS(q1.Length(), length, 0.01f, TEST_LOCATION);
}

static void UtcDaliQuaternionLengthSquared()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q1(0.383f, 0.450f, 0.123f, 0.924f);
  float lengthSquared = 0.383f*0.383f + 0.450f*0.450f + 0.123f*0.123f + 0.924f*0.924f;
  DALI_TEST_EQUALS(q1.LengthSquared(), lengthSquared, 0.01f, TEST_LOCATION);
}

static void UtcDaliQuaternionNormalize()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q1(0.118f, 0.692f, -0.127f, 0.701f);
  Quaternion q2 = q1;
  q2 *= 5.0f;
  q2.Normalize();
  DALI_TEST_EQUALS(q1, q2, 0.001f, TEST_LOCATION);
}

static void UtcDaliQuaternionNormalized()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q1(0.118f, 0.692f, -0.127f, 0.701f);
  Quaternion q2 = q1;
  q2 *= 5.0f;
  DALI_TEST_EQUALS(q1, q2.Normalized(), 0.001f, TEST_LOCATION);
}

static void UtcDaliQuaternionInvert()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q1(0.383f, 0.0f, 0.0f, 0.924f);

  // q1^-1 = q1* / ||q1||^2
  //       = Conjugate of q1 / Square of Norm of q1

  Quaternion r1 = q1;
  r1.Conjugate();
  r1 *= 1.0f/q1.LengthSquared();

  Quaternion q2 = q1;
  q2.Invert();
  DALI_TEST_EQUALS(q2, r1, 0.001f, TEST_LOCATION);
}


static void UtcDaliQuaternionDot()
{
  TestApplication application; // Reset all test adapter return codes
  // q.q' = s*s' + v dot v'
  float s1=0.784f; Vector3 v1(0.045f, 0.443f, 0.432f);
  float s2=0.697f; Vector3 v2(0.612, 0.344, -0.144);

  Quaternion q1(s1, v1.x, v1.y, v1.z);
  Quaternion q2(s2, v2.x, v2.y, v2.z);

  float r1 = s1*s2 + v1.Dot(v2);

  DALI_TEST_EQUALS(Quaternion::Dot(q1, q2), r1, TEST_LOCATION);
}


// Quaternion * vector == Vector rotation
static void UtcDaliQuaternionOperatorMultiplication02()
{
  TestApplication application; // Reset all test adapter return codes
  // Rotation of vector p = (x,y,z) by Quaternion q == q [0,p] q^-1
  Vector3 v(2, 3, 4);
  Quaternion q(Radian(Degree(72)), Vector3::ZAXIS);
  Quaternion qI = q;
  qI.Invert();
  Quaternion qv(0.0f, v.x, v.y, v.z);
  Quaternion r1 = (q * qv) * qI;

  Vector3 r2 = q * v;

  DALI_TEST_EQUALS(r1.mVector.x, r2.x, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(r1.mVector.y, r2.y, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(r1.mVector.z, r2.z, 0.001, TEST_LOCATION);
}

static void UtcDaliQuaternionRotate01()
{
  TestApplication application; // Reset all test adapter return codes
  // Rotation of vector p = (x,y,z) by Quaternion q == q [0,p] q^-1
  Vector3 v(2, 3, 4);
  Quaternion q(Radian(Degree(72)), Vector3::ZAXIS);
  Quaternion qI = q;
  qI.Invert();
  Quaternion qv(0.0f, v.x, v.y, v.z);
  Quaternion r1 = q * qv * qI;

  Vector3 r2 = q.Rotate(v);

  DALI_TEST_EQUALS(r1.mVector.x, r2.x, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(r1.mVector.y, r2.y, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(r1.mVector.z, r2.z, 0.001f, TEST_LOCATION);

  DALI_TEST_EQUALS(q.Rotate(v), q*v, 0.001f, TEST_LOCATION);
}


static void UtcDaliQuaternionRotate02()
{
  TestApplication application; // Reset all test adapter return codes
  // Rotation of vector p = (x,y,z) by Quaternion q == q [0,p] q^-1
  Vector4 v(2, 3, 4, 5);
  Quaternion q(Radian(Degree(72)), Vector3::ZAXIS);
  Quaternion qI = q;
  qI.Invert();
  Quaternion qv(0.0f, v.x, v.y, v.z);
  Quaternion r1 = q * qv * qI;

  Vector4 r2 = q.Rotate(v);

  DALI_TEST_EQUALS(r1.mVector.x, r2.x, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(r1.mVector.y, r2.y, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(r1.mVector.z, r2.z, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(r1.mVector.w, 0.0f, 0.001f, TEST_LOCATION);
}


static void UtcDaliQuaternionExp01()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q1(0.0f, 1.0f, 1.2f, 1.3f);
  Quaternion q2 = q1.Exp();
  Quaternion r2(-0.4452, 0.4406, 0.5287, 0.5728);

  DALI_TEST_EQUALS(q2.Length(), 1.0f, 0.01f, TEST_LOCATION);

  DALI_TEST_EQUALS(q2, r2, 0.001f, TEST_LOCATION);

  // Note, this trick only works when |v| < pi, which it is!
  Quaternion q3 = q2.Log();
  DALI_TEST_EQUALS(q1, q3, 0.01f, TEST_LOCATION);
}


static void UtcDaliQuaternionExp02()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q1(0.0f, 0.0f, 0.0f, 0.0f);
  Quaternion q2 = q1.Exp();
  Quaternion r2(1.0f, 0.0f, 0.0f, 0.0f);

  DALI_TEST_EQUALS(q2.Length(), 1.0f, 0.01f, TEST_LOCATION);

  DALI_TEST_EQUALS(q2, r2, 0.001f, TEST_LOCATION);

  // Note, this trick only works when |v| < pi, which it is!
  Quaternion q3 = q2.Log();
  DALI_TEST_EQUALS(q1, q3, 0.01f, TEST_LOCATION);
}


static void UtcDaliQuaternionExp03()
{
  TestApplication app;

  Quaternion q(0.0f, Vector3(5.0f, 6.0f, 7.0f));

  // q.w is non-zero. Should assert.
  try
  {
    q.Exp();
    DALI_TEST_CHECK(false);
  }
  catch(DaliException& e)
  {
    DALI_TEST_CHECK(true);
  }
}


static void UtcDaliQuaternionLog01()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q(Math::PI*0.73f, Vector3(2,3,4));
  Quaternion q2 = q;
  q2.Normalize();

  Quaternion r = q2.Log();
  DALI_TEST_EQUALS(r.mVector.w, 0.0f, 0.01f, TEST_LOCATION);

  Quaternion r2 = r.Exp();
  DALI_TEST_EQUALS(r2, q2, 0.01f, TEST_LOCATION);
}

static void UtcDaliQuaternionLog02()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q1(1.0f, 0.0f, 0.0f, 0.0f);
  Quaternion r1(0.0f, 0.0f, 0.0f, 0.0f);

  Quaternion q2 = q1.Log();

  DALI_TEST_EQUALS(q2, r1, 0.01f, TEST_LOCATION);

  Quaternion q3 = q2.Exp();
  DALI_TEST_EQUALS(q1, q3, 0.01f, TEST_LOCATION);
}



static void UtcDaliQuaternionLerp()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q1(Radian(Degree(-80)), Vector3(0.0f, 0.0f, 1.0f));
  Quaternion q2(Radian(Degree( 80)), Vector3(0.0f, 0.0f, 1.0f));

  Quaternion p = Quaternion::Lerp(q1, q2, 0.0f);
  DALI_TEST_EQUALS(p, q1, 0.001f, TEST_LOCATION);

  p = Quaternion::Lerp(q1, q2, 1.0f);
  DALI_TEST_EQUALS(p, q2, 0.001f, TEST_LOCATION);

  p = Quaternion::Lerp(q1, q2, 0.5f);
  Quaternion r1 = (q1 + q2) * 0.5f;
  r1.Normalize();
  DALI_TEST_EQUALS(p, r1, 0.001f, TEST_LOCATION);
}



static void UtcDaliQuaternionSlerp01()
{
  TestApplication application;

  Quaternion q1(M_PI/4.0f, Vector4(0.0f, 0.0f, 1.0f, 0.0f));
  Quaternion q2(-M_PI/4.0f, Vector4(0.0f, 0.0f, 1.0f, 0.0f));

  Quaternion q = Quaternion::Slerp(q1, q2, 0.0f);
  DALI_TEST_EQUALS(q, q1, 0.001, TEST_LOCATION);

  q = Quaternion::Slerp(q1, q2, 1.0f);
  DALI_TEST_EQUALS(q, q2, 0.001, TEST_LOCATION);

  // @ 25%, will be at M_PI/8
  q = Quaternion::Slerp(q1, q2, 0.25f);
  Vector4 axis;
  float angle;
  bool converted = q.ToAxisAngle(axis, angle);
  DALI_TEST_EQUALS(converted, true, TEST_LOCATION);
  DALI_TEST_EQUALS(angle, Math::PI/8.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.x, 0.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.y, 0.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.z, 1.0f, 0.001, TEST_LOCATION);
}



static void UtcDaliQuaternionSlerp02()
{
  TestApplication application;

  Quaternion q1(M_PI/6, Vector3(0.0f, 0.0f, 1.0f));
  Quaternion q2(M_PI/2, Vector3(0.0f, 0.0f, 1.0f));

  Quaternion q = Quaternion::Slerp(q1, q2, 0.0f);

  DALI_TEST_EQUALS(q,  q1, 0.001, TEST_LOCATION);

  q = Quaternion::Slerp(q1, q2, 1.0f);

  DALI_TEST_EQUALS(q,  q2, 0.001, TEST_LOCATION);

  // @ 50%, will be at M_PI/3 around z
  q = Quaternion::Slerp(q1, q2, 0.5f);

  Quaternion r( M_PI/3, Vector3( 0.0f, 0.0f, 1.0f));
  DALI_TEST_EQUALS( q, r, 0.001, TEST_LOCATION );
}


static void UtcDaliQuaternionSlerp03()
{
  TestApplication application;

  Quaternion q1(Radian(Degree(125)), Vector3(0.0f, 0.0f, 1.0f));
  Quaternion q2(Radian(Degree(-125)), Vector3(0.002f, 0.001f, 1.001f));

  Quaternion q = Quaternion::Slerp(q1, q2, 0.0f);
  DALI_TEST_EQUALS(q,  q1, 0.001, TEST_LOCATION);

  q = Quaternion::Slerp(q1, q2, 1.0f);
  DALI_TEST_EQUALS(q,  q2, 0.001, TEST_LOCATION);

  q = Quaternion::Slerp(q1, q2, 0.05f);
  Vector4 axis;
  float angle;
  bool converted = q.ToAxisAngle(axis, angle);
  DALI_TEST_EQUALS(converted, true, TEST_LOCATION);

  DALI_TEST_EQUALS(axis.x,  0.0f, 0.01, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.y,  0.0f, 0.01, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.z,  1.0f, 0.01, TEST_LOCATION);
}



static void UtcDaliQuaternionSlerp04()
{
  TestApplication application;

  Quaternion q1(Radian(Degree(120)), Vector3(0.0f, 0.0f, 1.0f));
  Quaternion q2(Radian(Degree(130)), Vector3(0.0f, 0.0f, 1.0f));

  Quaternion q = Quaternion::Slerp(q1, q2, 0.0f);
  DALI_TEST_EQUALS(q,  q1, 0.001, TEST_LOCATION);

  q = Quaternion::Slerp(q1, q2, 1.0f);
  DALI_TEST_EQUALS(q,  q2, 0.001, TEST_LOCATION);

  q = Quaternion::Slerp(q1, q2, 0.5f);
  Vector4 axis;
  float angle;
  bool converted = q.ToAxisAngle(axis, angle);
  DALI_TEST_EQUALS(converted, true, TEST_LOCATION);
  DALI_TEST_EQUALS(angle, float(Radian(Degree(125))), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.x,  0.0f, 0.01, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.y,  0.0f, 0.01, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.z,  1.0f, 0.01, TEST_LOCATION);
}



static void UtcDaliQuaternionSlerpNoInvert01()
{
  TestApplication application;

  Quaternion q1(M_PI/4.0f, Vector4(0.0f, 0.0f, 1.0f, 0.0f));
  Quaternion q2(-M_PI/4.0f, Vector4(0.0f, 0.0f, 1.0f, 0.0f));

  Quaternion q = Quaternion::SlerpNoInvert(q1, q2, 0.0f);
  DALI_TEST_EQUALS(q, q1, 0.001, TEST_LOCATION);

  q = Quaternion::SlerpNoInvert(q1, q2, 1.0f);
  DALI_TEST_EQUALS(q, q2, 0.001, TEST_LOCATION);

  // @ 25%, will be at M_PI/8
  q = Quaternion::SlerpNoInvert(q1, q2, 0.25f);
  Vector4 axis;
  float angle;
  bool converted = q.ToAxisAngle(axis, angle);
  DALI_TEST_EQUALS(converted, true, TEST_LOCATION);
  DALI_TEST_EQUALS(angle, Math::PI/8.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.x, 0.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.y, 0.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.z, 1.0f, 0.001, TEST_LOCATION);
}


static void UtcDaliQuaternionSlerpNoInvert02()
{
  TestApplication application;

  Quaternion q1(Radian(Degree(120)), Vector3(0.0f, 0.0f, 1.0f));
  Quaternion q2(Radian(Degree(130)), Vector3(0.0f, 0.0f, 1.0f));

  Quaternion q = Quaternion::SlerpNoInvert(q1, q2, 0.0f);
  DALI_TEST_EQUALS(q,  q1, 0.001, TEST_LOCATION);

  q = Quaternion::SlerpNoInvert(q1, q2, 1.0f);
  DALI_TEST_EQUALS(q,  q2, 0.001, TEST_LOCATION);

  q = Quaternion::SlerpNoInvert(q1, q2, 0.5f);
  Vector4 axis;
  float angle;
  bool converted = q.ToAxisAngle(axis, angle);
  DALI_TEST_EQUALS(converted, true, TEST_LOCATION);
  DALI_TEST_EQUALS(angle, float(Radian(Degree(125))), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.x,  0.0f, 0.01, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.y,  0.0f, 0.01, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.z,  1.0f, 0.01, TEST_LOCATION);
}


static void UtcDaliQuaternionSquad()
{
  TestApplication application; // Reset all test adapter return codes
  Quaternion q1(Radian(Degree(45)),     Vector3(0.0f, 0.0f, 1.0f));
  Quaternion q1out(Radian(Degree(40)),  Vector3(0.0f, 1.0f, 2.0f));
  Quaternion q2in(Radian(Degree(35)),   Vector3(0.0f, 2.0f, 3.0f));
  Quaternion q2(Radian(Degree(30)),     Vector3(0.0f, 1.0f, 3.0f));

  Quaternion q = Quaternion::Squad(q1, q2, q1out, q2in, 0.0f);
  DALI_TEST_EQUALS(q, q1, 0.001f, TEST_LOCATION);

  q = Quaternion::Squad(q1, q2, q1out, q2in, 1.0f);
  DALI_TEST_EQUALS(q, q2, 0.001f, TEST_LOCATION);

  // Don't know what actual value should be, but can make some informed guesses.
  q = Quaternion::Squad(q1, q2, q1out, q2in, 0.5f);
  float angle;
  Vector3 axis;
  q.Normalize();
  q.ToAxisAngle(axis, angle);

  if(angle < 0.0f)
  {
    q = -q; // Might get negative quat
    q.ToAxisAngle(axis, angle);
  }
  float deg = Degree(Radian(angle));
  DALI_TEST_CHECK(deg >= 0 && deg <= 90);
  DALI_TEST_CHECK(axis.y > 0);
  DALI_TEST_CHECK(axis.z > 0);
}

static void UtcDaliAngleBetween()
{
  TestApplication application; // Reset all test adapter return codes

  Quaternion q1(Radian(Degree(45)), 0.0f, 0.0f);
  Quaternion q2(Radian(Degree(47)), 0.0f, 0.0f);
  DALI_TEST_EQUALS(Quaternion::AngleBetween(q1, q2), fabsf(Radian(Degree(45)) - Radian(Degree(47))), 0.001f, TEST_LOCATION);

  Quaternion q3(Radian(Degree(80)), Vector3::YAXIS);
  Quaternion q4(Radian(Degree(90)), Vector3::YAXIS);
  DALI_TEST_EQUALS(Quaternion::AngleBetween(q3, q4), fabsf(Radian(Degree(80)) - Radian(Degree(90))), 0.001f, TEST_LOCATION);

  Quaternion q5(Radian(Degree(0)), Vector3::YAXIS);
  Quaternion q6(Radian(Degree(90)), Vector3::XAXIS);
  DALI_TEST_EQUALS(Quaternion::AngleBetween(q5, q6), fabsf(Radian(Degree(0)) - Radian(Degree(90))), 0.001f, TEST_LOCATION);

  Quaternion q7(Radian(Degree(0)), Vector3::YAXIS);
  Quaternion q8(Radian(Degree(0)), Vector3::XAXIS);
  DALI_TEST_EQUALS(Quaternion::AngleBetween(q7, q8), fabsf(Radian(Degree(0)) - Radian(Degree(0))), 0.001f, TEST_LOCATION);

  Quaternion q9(Radian(Degree(0)), Vector3::XAXIS);
  Quaternion q10(Radian(Degree(180)), Vector3::XAXIS);
  DALI_TEST_EQUALS(Quaternion::AngleBetween(q9, q10), fabsf(Radian(Degree(0)) - Radian(Degree(180))), 0.001f, TEST_LOCATION);

  Quaternion q11(Radian(Degree(1)), Vector3::YAXIS);
  Quaternion q12(Radian(Degree(240)), Vector3::YAXIS);
  DALI_TEST_EQUALS(Quaternion::AngleBetween(q11, q12), fabsf(Radian( Degree(1 - 240 + 360) )), 0.001f, TEST_LOCATION);

  Quaternion q13(Radian(Degree(240)), Vector3::YAXIS);
  Quaternion q14(Radian(Degree(1)), Vector3::YAXIS);
  DALI_TEST_EQUALS(Quaternion::AngleBetween(q13, q14), fabsf(Radian( Degree(240 - 1 - 360) )), 0.001f, TEST_LOCATION);

  Quaternion q15(Radian(Degree(240)), Vector3::YAXIS);
  Quaternion q16(Radian(Degree(1)), Vector3::ZAXIS);
  DALI_TEST_EQUALS(Quaternion::AngleBetween(q15, q16), Quaternion::AngleBetween(q16, q15), 0.001f, TEST_LOCATION);
}

static void UtcDaliQuaternionOStreamOperator()
{
  TestApplication application; // Reset all test adapter return codes

  std::ostringstream oss;

  Quaternion quaternion(M_PI, Vector3::YAXIS);

  oss << quaternion;

  std::string expectedOutput = "[ Axis: [0, 1, 0], Angle: 180 degrees ]";

  DALI_TEST_EQUALS( oss.str(), expectedOutput, TEST_LOCATION);
}
