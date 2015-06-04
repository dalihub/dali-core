/*
 * Copyright ( c ) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 ( the "License" );
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

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using namespace Dali;


void utc_dali_quaternion_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_quaternion_cleanup(void)
{
  test_return_value = TET_PASS;
}


int UtcDaliQuaternionCtorDefaultP(void)
{
  Quaternion q;
  DALI_TEST_EQUALS( q.AsVector().w,  1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( q.AsVector().x,  0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( q.AsVector().y,  0.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( q.AsVector().z,  0.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionCtorCosSinThetaP(void)
{
  Quaternion q( 1.0f, 0.1f, 0.2f, 0.3f );

  DALI_TEST_EQUALS( q.AsVector().w,  1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( q.AsVector().x,  0.1f, TEST_LOCATION );
  DALI_TEST_EQUALS( q.AsVector().y,  0.2f, TEST_LOCATION );
  DALI_TEST_EQUALS( q.AsVector().z,  0.3f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionCtorVector4P(void)
{
  Quaternion q( Vector4( 1.0f, 0.1f, 0.2f, 0.3f ) );

  DALI_TEST_EQUALS( q.AsVector().x,  1.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( q.AsVector().y,  0.1f, TEST_LOCATION );
  DALI_TEST_EQUALS( q.AsVector().z,  0.2f, TEST_LOCATION );
  DALI_TEST_EQUALS( q.AsVector().w,  0.3f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionCtorAxisAngleVector3P(void)
{
  Quaternion q( Dali::ANGLE_90, Vector3( 1.0f, 2.0f, 3.0f ) );

  // This will be normalised:
  DALI_TEST_EQUALS( q.AsVector().w,  0.707f, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( q.AsVector().x,  0.189f, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( q.AsVector().y,  0.378f, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( q.AsVector().z,  0.567f, 0.001, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionCtorEulerAngleP(void)
{
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
  END_TEST;
}

int UtcDaliQuaternionCtorMatrixP01(void)
{
  // angle: 60 deg, axis: [1,2,3]
  float Mref_raw[16] = { 0.535714f, 0.765794f, -0.355767f, 0.0f,
                      -0.622936f, 0.642857f, 0.445741f, 0.0f,
                      0.570053f, -0.0171693f, 0.821429f, 0.0f,
                      0.0f,       0.0f,       0.0f,        1.0f};
  Matrix Mref( Mref_raw );

  Quaternion q1( Radian(M_PI/3.0f), Vector3( 1.0f, 2.0f, 3.0f ) );
  Quaternion q2( Mref );

  DALI_TEST_EQUALS( q1, q2, 0.001, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionCtorMatrixP02(void)
{
  // IDENTITY rotation
  Quaternion q;

  Matrix m( q );     // Convert to matrix

  Quaternion q2( m ); // and back to a quaternion

  DALI_TEST_EQUALS( q, q2, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( m, Matrix::IDENTITY, 0.001f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionCtorMatrixP03(void)
{
  // Create an arbitrary forward vector
  for( float x=-1.0f; x<=1.0f; x+=0.1f )
  {
    for( float y=-1.0f; y<1.0f; y+=0.1f )
    {
      for( float z=-1.0f; z<1.0f; z+=0.1f )
      {
        Vector3 vForward( x, y, z );
        vForward.Normalize();

        // Construct an up vector from a sideways move
        Vector3 vSide;
        Vector3 vUp = vForward.Cross( Vector3( vForward.x+1.0f, vForward.y, vForward.z ) );
        if( vUp.Length() > 0.01 )
        {
          vUp.Normalize();
          vSide = vUp.Cross( vForward );
          vSide.Normalize();
        }
        else
        {
          vSide = vForward.Cross( Vector3( vForward.x, vForward.y+1.0f, vForward.z ) );
          vSide.Normalize();
          vUp = vForward.Cross( vSide );
          vUp.Normalize();
        }

        // Generate a matrix, and then a quaternion from it
        Matrix rotMatrix( Matrix::IDENTITY );
        rotMatrix.SetXAxis( vSide );
        rotMatrix.SetYAxis( vUp );
        rotMatrix.SetZAxis( vForward );
        Quaternion q( rotMatrix );

        // Generate a matrix from the quaternion, check they are the same
        Matrix resultMatrix( q );
        DALI_TEST_EQUALS( resultMatrix, rotMatrix, 0.001f, TEST_LOCATION );

        // Rotate an arbitrary vector by both quaternion and rotation matrix,
        // check the result is the same

        Vector4 aVector( -2.983f, -3.213f, 8.2239f, 1.0f );
        Vector3 aVectorRotatedByQ = q.Rotate( Vector3( aVector ) );
        Vector4 aVectorRotatedByR = rotMatrix*aVector;
        DALI_TEST_EQUALS( aVectorRotatedByQ, Vector3( aVectorRotatedByR ), 0.001f, TEST_LOCATION );
      }
    }
  }
  END_TEST;
}

int UtcDaliQuaternionCtorAxesP01(void)
{
  // angle: 60 deg, axis: [1,2,3]
  float Mref_raw[16] = { 0.535714f, 0.765794f, -0.355767f, 0.0f,
                      -0.622936f, 0.642857f, 0.445741f, 0.0f,
                      0.570053f, -0.0171693f, 0.821429f, 0.0f,
                      0.0f,       0.0f,       0.0f,        1.0f};
  Matrix Mref( Mref_raw );

  Quaternion q1( Radian(M_PI/3.0f), Vector3( 1.0f, 2.0f, 3.0f ) );
  Quaternion q2( Mref.GetXAxis(), Mref.GetYAxis(), Mref.GetZAxis() );

  DALI_TEST_EQUALS( q1, q2, 0.001, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionCtorAxesP02(void)
{
  Vector3 xAxis( Vector3::XAXIS );
  Vector3 yAxis( Vector3::YAXIS );
  Vector3 zAxis( Vector3::ZAXIS );

  Quaternion q1( xAxis, yAxis, zAxis );

  DALI_TEST_EQUALS( q1, Quaternion::IDENTITY, TEST_LOCATION );

  xAxis = Vector3( 1.0f, 1.0f, 0.0f );
  xAxis.Normalize();
  yAxis = Vector3( -1.0f, 1.0f, 0.0f ); // 45 degrees anticlockwise ( +ve ) around z
  yAxis.Normalize();
  zAxis = xAxis.Cross( yAxis );
  zAxis.Normalize();
  Quaternion q2( xAxis, yAxis, zAxis );

  DALI_TEST_EQUALS( q2, Quaternion( Radian( Degree( 45 ) ), Vector3::ZAXIS ), 0.001f, TEST_LOCATION );
  END_TEST;
}


int UtcDaliQuaternionCtorAxesP03(void)
{
  // Create an arbitrary forward vector
  for( float x=-1.0f; x<=1.0f; x+=0.1f )
  {
    for( float y=-1.0f; y<1.0f; y+=0.1f )
    {
      for( float z=-1.0f; z<1.0f; z+=0.1f )
      {
        Vector3 vForward( x, y, z );
        vForward.Normalize();

        // Construct an up vector from a sideways move
        Vector3 vSide;
        Vector3 vUp = vForward.Cross( Vector3( vForward.x+1.0f, vForward.y, vForward.z ) );
        if( vUp.Length() > 0.01 )
        {
          vUp.Normalize();
          vSide = vUp.Cross( vForward );
          vSide.Normalize();
        }
        else
        {
          vSide = vForward.Cross( Vector3( vForward.x, vForward.y+1.0f, vForward.z ) );
          vSide.Normalize();
          vUp = vForward.Cross( vSide );
          vUp.Normalize();
        }

        // Generate a quaternion
        Quaternion q( vSide, vUp, vForward );

        Matrix rotMatrix;
        rotMatrix.SetXAxis( vSide );
        rotMatrix.SetYAxis( vUp );
        rotMatrix.SetZAxis( vForward );

        // Generate a matrix from the quaternion, check they are the same
        Matrix m( q );
        DALI_TEST_EQUALS( m.GetXAxis(), vSide, 0.001f, TEST_LOCATION );
        DALI_TEST_EQUALS( m.GetYAxis(), vUp, 0.001f, TEST_LOCATION );
        DALI_TEST_EQUALS( m.GetZAxis(), vForward, 0.001f, TEST_LOCATION );

        // Rotate an arbitrary vector by both quaternion and rotation matrix,
        // check the result is the same

        Vector4 aVector( 2.043f, 12.8f, -3.872f, 1.0f );
        Vector3 aVectorRotatedByQ = q.Rotate( Vector3( aVector ) );
        Vector4 aVectorRotatedByR = rotMatrix*aVector;
        DALI_TEST_EQUALS( aVectorRotatedByQ, Vector3( aVectorRotatedByR ), 0.001f, TEST_LOCATION );
      }
    }
  }
  END_TEST;
}

int UtcDaliQuaternionCtorTwoVectorsP(void)
{
  Vector3 v0( 1.0f, 2.0f, 3.0f );
  Vector3 v1( -2.0f, 10.0f, -1.0f );
  v0.Normalize();
  v1.Normalize();
  Quaternion q( v0, v1 );

  DALI_TEST_EQUALS( q*v0, v1, 0.001, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionAsVectorP(void)
{
  Vector4 v( 1.0f, 0.1f, 0.2f, 0.3f );
  Quaternion q( v );

  DALI_TEST_EQUALS( v, q.AsVector(), TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionToAxisAngleVector3P(void)
{
  Quaternion q( 0.932f, 1.1f, 3.4f, 2.7f );
  Radian angle;
  Vector3 axis;
  bool converted = q.ToAxisAngle( axis, angle );
  DALI_TEST_EQUALS( converted, true, TEST_LOCATION );
  DALI_TEST_EQUALS( angle.radian,  0.74f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( axis.x, 3.03f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( axis.y, 9.38f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( axis.z, 7.45f, 0.01f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionToAxisAngleVector3N(void)
{
  Quaternion q( 1, 2, 3, 4 );
  Radian angle;
  Vector3 axis;
  bool converted = q.ToAxisAngle( axis, angle );
  DALI_TEST_EQUALS( converted, false, TEST_LOCATION );
  DALI_TEST_EQUALS( angle.radian,  0.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( axis.x, 0.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( axis.y, 0.0f, 0.01f, TEST_LOCATION );
  DALI_TEST_EQUALS( axis.z, 0.0f, 0.01f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionSetEulerP(void)
{
  // Test from euler angles
  Quaternion e1;
  e1.SetEuler( Dali::ANGLE_45, Dali::ANGLE_0, Dali::ANGLE_0 );
  Vector4 r1( 0.383f, 0.0f, 0.0f, 0.924f );

  Quaternion e2;
  e2.SetEuler( Dali::ANGLE_0, Radian( Degree( 75 ) ), Dali::ANGLE_0 );
  Vector4 r2( 0.0f, 0.609f, 0.0f, 0.793f );

  Quaternion e3;
  e3.SetEuler( Dali::ANGLE_0, Dali::ANGLE_0, Dali::ANGLE_135 );
  Vector4 r3( 0.0f, 0.0f, 0.924f, 0.383f );

  Quaternion e4;
  e4.SetEuler( Radian( Degree( 71 ) ), Radian( Degree( 36 ) ), Radian( Degree( 27 ) ) );
  Vector4 r4( 0.478f, 0.374f, 0.006f, 0.795f );

  Quaternion e5;
  e5.SetEuler( Radian( Degree( -31 ) ), Radian( Degree( -91 ) ), Radian( Degree( -173 ) ) );
  Vector4 r5( -0.697f, 0.145f, -0.686f, -0.149f );

  DALI_TEST_EQUALS( e1.AsVector(), r1, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( e2.AsVector(), r2, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( e3.AsVector(), r3, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( e4.AsVector(), r4, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( e5.AsVector(), r5, 0.001, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionEulerAnglesP(void)
{
  Quaternion q1( 0.924f, 0.383f, 0.0f, 0.0f );
  Vector4 r1( Radian( Degree( 45 ) ), 0.0f, 0.0f, 0.0f );

  Quaternion q2( 0.793f, 0.0f, 0.609f, 0.0f );
  Vector4 r2( 0.0f, Radian( Degree( 75 ) ), 0.0f, 0.0f );

  Quaternion q3( 0.383f, 0.0f, 0.0f, 0.924f );
  Vector4 r3( 0.0f, 0.0f, Radian( Degree( 135 ) ), 0.0f );

  Quaternion q4( 0.795f, 0.478f, 0.374f, 0.006f );
  Vector4 r4( Radian( Degree( 71 ) ), Radian( Degree( 36 ) ), Radian( Degree( 27 ) ), 0.0f );

  Quaternion q5(  -0.149f, -0.697f, 0.145f, -0.686f );
  Vector4 r5( Radian( Degree( 148.0 ) ), Radian( Degree( -88.2 ) ), Radian( Degree( 8.0 ) ), 0.0f );

  DALI_TEST_EQUALS( q1.EulerAngles(), r1, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( q2.EulerAngles(), r2, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( q3.EulerAngles(), r3, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( q4.EulerAngles(), r4, 0.01,  TEST_LOCATION );
  DALI_TEST_EQUALS( q5.EulerAngles(), r5, 0.01,  TEST_LOCATION );
  END_TEST;
}


int UtcDaliQuaternionToMatrixP01(void)
{
  Quaternion q( Radian( 0.69813 ), Vector3( 1.0f, 0.0f, 0.0f ) ); // 40 degree rotation around X axis

  // Result calculated using a different maths library ( with appropriate row/col ordering )

  float els[] = { 1.0f,  0.0f,   0.0f,  0.0f,
                  0.0f,  0.766f, 0.643f, 0.0f,
                  0.0f, -0.643f, 0.766f, 0.0f,
                  0.0f,  0.0f,   0.0f,  1.0f };
  Matrix mRes( els );
  Matrix m( q );

  DALI_TEST_EQUALS( m, mRes, 0.01, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionToMatrixP02(void)
{
  // rotation around arbitrary axis
  Quaternion q2( Radian( -1.23918f ), Vector3( 7.0f, -13.0f, 11.0f ) );

  float els[] = {  0.423f, -0.746f, -0.514f,  0.00f,
                   0.384f,  0.662f, -0.644f,  0.00f,
                   0.821f,  0.075f,  0.566f,  0.00f,
                   0.000f,  0.000f,  0.000f,  1.00f };
  Matrix mRes2( els );

  Matrix m2( q2 );

  DALI_TEST_EQUALS( m2, mRes2, 0.01, TEST_LOCATION );
  END_TEST;
}


int UtcDaliQuaternionOperatorAdditionP(void)
{
  Quaternion q1( 0.383f, 0.0f, 0.0f, 0.924f );
  Quaternion q2( 0.0f, 0.609f, 0.0f, 0.793f );

  Quaternion r1( 0.383f, 0.609f, 0.0f, 1.717f );

  DALI_TEST_EQUALS( q1+q2, r1, 0.001f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionOperatorSubtractionP(void)
{
  Quaternion q1( 0.383f, 0.450f, 0.123f, 0.924f );
  Quaternion q2( 0.383f, 0.690f, 0.234f, 1.917f );

  Quaternion r1( 0.0f, 0.240f, 0.111f, 0.993f );

  DALI_TEST_EQUALS( q2-q1, r1, 0.001f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionConjugateP(void)
{
  float s1 = 0.784f; Vector3 v1( 0.045f, 0.443f, 0.432f );
  float s2 = 0.697f; Vector3 v2( 0.612, 0.344, -0.144 );

  Quaternion q1( s1, v1.x, v1.y, v1.z );
  Quaternion q2( s2, v2.x, v2.y, v2.z );
  q1.Conjugate();
  q2.Conjugate();

  Quaternion r1( s1, -v1.x, -v1.y, -v1.z );
  Quaternion r2( s2, -v2.x, -v2.y, -v2.z );

  DALI_TEST_EQUALS( q1, r1, 0.001f, TEST_LOCATION );
  DALI_TEST_EQUALS( q2, r2, 0.001f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionOperatorMultiplicationQuaternionP(void)
{
  float s1=0.784f; Vector3 v1( 0.045f, 0.443f, 0.432f );
  float s2=0.697f; Vector3 v2( 0.612, 0.344, -0.144 );

  Quaternion q1( s1, v1.x, v1.y, v1.z );
  Quaternion q2( s2, v2.x, v2.y, v2.z );

  Vector3 vp = v1.Cross( v2 ) + v2*s1 + v1*s2;
  Quaternion r1( s1*s2-v1.Dot( v2 ), vp.x, vp.y, vp.z );

  DALI_TEST_EQUALS( q1*q2, r1, 0.001f, TEST_LOCATION );
  END_TEST;
}

// Quaternion * vector == Vector rotation
int UtcDaliQuaternionOperatorMultiplicationVector3P(void)
{
  // Rotation of vector p = ( x,y,z ) by Quaternion q == q [0,p] q^-1
  Vector3 v( 2, 3, 4 );
  Quaternion q( Radian( Degree( 72 ) ), Vector3::ZAXIS );
  Quaternion qI = q;
  qI.Invert();
  Quaternion qv( 0.0f, v.x, v.y, v.z );
  Quaternion r1 = ( q * qv ) * qI;

  Vector3 r2 = q * v;

  DALI_TEST_EQUALS( r1.mVector.x, r2.x, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( r1.mVector.y, r2.y, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( r1.mVector.z, r2.z, 0.001, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionOperatorMultiplicationFloatP01(void)
{
  // Rotation of vector p = ( x,y,z ) by Quaternion q == q [0,p] q^-1
  Quaternion q( Vector4( 0.1f, 0.2f, 0.3f, 1.0f ) );
  Quaternion q2 = q * 2.f;
  Vector4 v2( 0.2f, 0.4f, 0.6f, 2.0f );

  DALI_TEST_EQUALS( q2.AsVector(), v2, 0.001, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionOperatorMultiplicationFloatP02(void)
{
  Quaternion q1( 0.383f, 0.0f, 0.0f, 0.924f );
  Quaternion r1( 2.0f* 0.383f, 0.0f, 0.0f, 2.0f * 0.924f );

  DALI_TEST_EQUALS( q1 * 2.0f, r1, 0.001f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionOperatorMultiplicationFloatP03(void)
{
  Quaternion q1( 0.383f, 0.0f, 0.0f, 0.924f );
  Quaternion r1( 0.5f* 0.383f, 0.0f, 0.0f, 0.5f * 0.924f );

  DALI_TEST_EQUALS( q1 / 2.0f, r1, 0.001f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionOperatorDivisionQuaternionP(void)
{
  Quaternion q1( 0.383f, 0.0f, 0.0f, 0.924f );
  Quaternion q2( 0.0f, 0.609f, 0.0f, 0.793f );

  // q1 / q2 = q1 * q2^-1
  // q2^-1 = q2* / ||q2||^2
  //       = Conjugate of q2 / Square of Norm of q2

  Quaternion r1 = q2;
  r1.Conjugate();
  r1 *= 1.0f/q2.LengthSquared();
  Quaternion r2 = q1 * r1;

  DALI_TEST_EQUALS( q1 / q2, r2, 0.001f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionOperatorDivisionFloatP(void)
{
  Quaternion q1( 0.383f, 0.0f, 0.0f, 0.924f );
  Quaternion r1( 2.0f* 0.383f, 0.0f, 0.0f, 2.0f * 0.924f );

  DALI_TEST_EQUALS( q1, r1/2.0f, 0.001f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionOperatorDivideAssignedFloatP(void)
{
  Quaternion q1( 0.383f, 0.0f, 0.0f, 0.924f );
  Quaternion r1( 2.0f* 0.383f, 0.0f, 0.0f, 2.0f * 0.924f );
  r1 /= 2.0f;

  DALI_TEST_EQUALS( q1, r1, 0.001f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionOperatorNegationP(void)
{
  Quaternion q1( 0.383f, 0.0f, 0.0f, 0.924f );
  Quaternion r1( -0.383f, -0.0f, -0.0f, -0.924f );

  DALI_TEST_EQUALS( -q1, r1, 0.001f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionOperatorAddAssignP(void)
{
  Quaternion q1( 0.383f, 0.0f, 0.0f, 0.924f );
  Quaternion q2( 0.0f, 0.609f, 0.0f, 0.793f );

  Quaternion r1( 0.383f, 0.609f, 0.0f, 1.717f );

  q1 += q2;
  DALI_TEST_EQUALS( q1, r1, 0.001f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionOperatorSubtractAssignP(void)
{
  Quaternion q1( 0.383f, 0.450f, 0.123f, 0.924f );
  Quaternion q2( 0.383f, 0.690f, 0.234f, 1.917f );

  Quaternion r1( 0.0f, 0.240f, 0.111f, 0.993f );
  q2 -= q1;
  DALI_TEST_EQUALS( q2, r1, 0.001f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionOperatorMultiplyAssignQuaternionP(void)
{
  float s1=0.784f; Vector3 v1( 0.045f, 0.443f, 0.432f );
  float s2=0.697f; Vector3 v2( 0.612, 0.344, -0.144 );

  Quaternion q1( s1, v1.x, v1.y, v1.z );
  Quaternion q2( s2, v2.x, v2.y, v2.z );

  Quaternion r3 = q2 * q1;
  q2 *= q1;
  DALI_TEST_EQUALS( q2, r3, 0.001f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionOperatorMultiplyAssignFloatP01(void)
{
  Quaternion q1( 0.383f, 0.450f, 0.123f, 0.924f );
  float scale = 2.5f;
  Quaternion r1( scale*0.383f, scale*0.450f, scale*0.123f, scale*0.924f );
  q1 *= scale;
  DALI_TEST_EQUALS( q1, r1, 0.001f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionOperatorMultiplyAssignFloatP02(void)
{
  Quaternion q1( 0.383f, 0.450f, 0.123f, 0.924f );
  float scale = 2.5f;
  Quaternion r1( 0.383f/scale, 0.450f/scale, 0.123f/scale, 0.924f/scale );
  q1 /= scale;
  DALI_TEST_EQUALS( q1, r1, 0.001f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionOperatorEqualityP(void)
{
  Quaternion q1( 0.383f, 0.450f, 0.123f, 0.924f );
  Quaternion q2( 0.383f, 0.450f, 0.123f, 0.924f );
  Quaternion q3( 0.383f, 0.450f, 0.123f, 0.800f );
  Quaternion q4( 0.383f, 0.450f, 0.100f, 0.800f );
  Quaternion q5( 0.383f, 0.100f, 0.100f, 0.800f );
  Quaternion q6( 0.100f, 0.100f, 0.100f, 0.800f );

  Quaternion q7( -0.383f, -0.450f, -0.123f, -0.924f );
  Quaternion q8( -0.383f, -0.450f, -0.123f,  0.924f );
  Quaternion q9( -0.383f, -0.450f,  0.123f,  0.924f );
  Quaternion q10( -0.383f,  0.450f,  0.123f,  0.924f );

  DALI_TEST_CHECK( q1 == q2 );
  DALI_TEST_CHECK( !( q1 == q3 ) );
  DALI_TEST_CHECK( !( q1 == q4 ) );
  DALI_TEST_CHECK( !( q1 == q5 ) );
  DALI_TEST_CHECK( !( q1 == q6 ) );
  DALI_TEST_CHECK( ( q1 == q7 ) );
  DALI_TEST_CHECK( !( q1 == q8 ) );
  DALI_TEST_CHECK( !( q1 == q9 ) );
  DALI_TEST_CHECK( !( q1 == q10 ) );
  END_TEST;
}

int UtcDaliQuaternionOperatorInequalityP(void)
{
  Quaternion q1( 0.383f, 0.450f, 0.123f, 0.924f );
  Quaternion q2( 0.383f, 0.450f, 0.123f, 0.924f );
  Quaternion q3( -0.383f, -0.0f, -0.0f, -0.924f );
  DALI_TEST_CHECK( !( q1 != q2 ) );
  DALI_TEST_CHECK( q1 != q3 );
  END_TEST;
}

int UtcDaliQuaternionLengthP(void)
{
  Quaternion q1( 0.383f, 0.450f, 0.123f, 0.924f );
  float length = sqrtf( 0.383f*0.383f + 0.450f*0.450f + 0.123f*0.123f + 0.924f*0.924f );
  DALI_TEST_EQUALS( q1.Length(), length, 0.001f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionLengthSquaredP(void)
{
  Quaternion q1( 0.383f, 0.450f, 0.123f, 0.924f );
  float lengthSquared = 0.383f*0.383f + 0.450f*0.450f + 0.123f*0.123f + 0.924f*0.924f;
  DALI_TEST_EQUALS( q1.LengthSquared(), lengthSquared, 0.01f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionNormalizeP(void)
{
  Quaternion q1( 0.118f, 0.692f, -0.127f, 0.701f );
  Quaternion q2 = q1;
  q2 *= 5.0f;
  q2.Normalize();
  DALI_TEST_EQUALS( q1, q2, 0.001f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionNormalizedP(void)
{
  Quaternion q1( 0.118f, 0.692f, -0.127f, 0.701f );
  Quaternion q2 = q1;
  q2 *= 5.0f;
  DALI_TEST_EQUALS( q1, q2.Normalized(), 0.001f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionIsIdentityP(void)
{
  Quaternion q( 1.0f, 0.0f, 0.0f, 0.0f );
  DALI_TEST_EQUALS( q.IsIdentity(),  true, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionIsIdentityN(void)
{
  Quaternion q( 1.0f, 0.1f, 0.0f, 0.0f );
  DALI_TEST_EQUALS( q.IsIdentity(),  false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionInvertP(void)
{
  Quaternion q1( 0.383f, 0.0f, 0.0f, 0.924f );

  // q1^-1 = q1* / ||q1||^2
  //       = Conjugate of q1 / Square of Norm of q1

  Quaternion r1 = q1;
  r1.Conjugate();
  r1 *= 1.0f/q1.LengthSquared();

  Quaternion q2 = q1;
  q2.Invert();
  DALI_TEST_EQUALS( q2, r1, 0.001f, TEST_LOCATION );
  END_TEST;
}


int UtcDaliQuaternionDotP(void)
{
  // q.q' = s*s' + v dot v'
  float s1 = 0.784f; Vector3 v1( 0.045f, 0.443f, 0.432f );
  float s2 = 0.697f; Vector3 v2( 0.612, 0.344, -0.144 );

  Quaternion q1( s1, v1.x, v1.y, v1.z );
  Quaternion q2( s2, v2.x, v2.y, v2.z );

  float r1 = s1*s2 + v1.Dot( v2 );

  DALI_TEST_EQUALS( Quaternion::Dot( q1, q2 ), r1, TEST_LOCATION );
  END_TEST;
}


int UtcDaliQuaternionRotateVector3P(void)
{
  // Rotation of vector p = ( x,y,z ) by Quaternion q == q [0,p] q^-1
  Vector3 v( 2, 3, 4 );
  Quaternion q( Radian( Degree( 72 ) ), Vector3::ZAXIS );
  Quaternion qI = q;
  qI.Invert();
  Quaternion qv( 0.0f, v.x, v.y, v.z );
  Quaternion r1 = q * qv * qI;

  Vector3 r2 = q.Rotate( v );

  DALI_TEST_EQUALS( r1.mVector.x, r2.x, 0.001f, TEST_LOCATION );
  DALI_TEST_EQUALS( r1.mVector.y, r2.y, 0.001f, TEST_LOCATION );
  DALI_TEST_EQUALS( r1.mVector.z, r2.z, 0.001f, TEST_LOCATION );

  DALI_TEST_EQUALS( q.Rotate( v ), q*v, 0.001f, TEST_LOCATION );
  END_TEST;
}


int UtcDaliQuaternionRotateVector4P(void)
{
  // Rotation of vector p = ( x,y,z ) by Quaternion q == q [0,p] q^-1
  Vector4 v( 2, 3, 4, 5 );
  Quaternion q( Radian( Degree( 72 ) ), Vector3::ZAXIS );
  Quaternion qI = q;
  qI.Invert();
  Quaternion qv( 0.0f, v.x, v.y, v.z );
  Quaternion r1 = q * qv * qI;

  Vector4 r2 = q.Rotate( v );

  DALI_TEST_EQUALS( r1.mVector.x, r2.x, 0.001f, TEST_LOCATION );
  DALI_TEST_EQUALS( r1.mVector.y, r2.y, 0.001f, TEST_LOCATION );
  DALI_TEST_EQUALS( r1.mVector.z, r2.z, 0.001f, TEST_LOCATION );
  DALI_TEST_EQUALS( r1.mVector.w, 0.0f, 0.001f, TEST_LOCATION );
  END_TEST;
}


int UtcDaliQuaternionExpP01(void)
{
  Quaternion q1( 0.0f, 1.0f, 1.2f, 1.3f );
  Quaternion q2 = q1.Exp();
  Quaternion r2( -0.4452, 0.4406, 0.5287, 0.5728 );

  DALI_TEST_EQUALS( q2.Length(), 1.0f, 0.01f, TEST_LOCATION );

  DALI_TEST_EQUALS( q2, r2, 0.001f, TEST_LOCATION );

  // Note, this trick only works when |v| < pi, which it is!
  Quaternion q3 = q2.Log();
  DALI_TEST_EQUALS( q1, q3, 0.01f, TEST_LOCATION );
  END_TEST;
}


int UtcDaliQuaternionExpP02(void)
{
  Quaternion q1( 0.0f, 0.0f, 0.0f, 0.0f );
  Quaternion q2 = q1.Exp();
  Quaternion r2( 1.0f, 0.0f, 0.0f, 0.0f );

  DALI_TEST_EQUALS( q2.Length(), 1.0f, 0.01f, TEST_LOCATION );

  DALI_TEST_EQUALS( q2, r2, 0.001f, TEST_LOCATION );

  // Note, this trick only works when |v| < pi, which it is!
  Quaternion q3 = q2.Log();
  DALI_TEST_EQUALS( q1, q3, 0.01f, TEST_LOCATION );
  END_TEST;
}


int UtcDaliQuaternionExpN(void)
{
  Quaternion q( Radian( 0.0f ), Vector3(5.0f, 6.0f, 7.0f) );

  // q.w is non-zero. Should assert.
  try
  {
    q.Exp();
    DALI_TEST_CHECK( false );
  }
  catch( DaliException& e )
  {
    DALI_TEST_CHECK( true );
  }
  END_TEST;
}


int UtcDaliQuaternionLogP01(void)
{
  Quaternion q( Radian( Math::PI*0.73f ), Vector3(2,3,4) );
  Quaternion q2 = q;
  q2.Normalize();

  Quaternion r = q2.Log();
  DALI_TEST_EQUALS( r.mVector.w, 0.0f, 0.01f, TEST_LOCATION );

  Quaternion r2 = r.Exp();
  DALI_TEST_EQUALS( r2, q2, 0.01f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionLogP02(void)
{
  Quaternion q1( 1.0f, 0.0f, 0.0f, 0.0f );
  Quaternion r1( 0.0f, 0.0f, 0.0f, 0.0f );

  Quaternion q2 = q1.Log();

  DALI_TEST_EQUALS( q2, r1, 0.01f, TEST_LOCATION );

  Quaternion q3 = q2.Exp();
  DALI_TEST_EQUALS( q1, q3, 0.01f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionLerpP(void)
{
  Quaternion q1( Radian( Degree( -80 ) ), Vector3( 0.0f, 0.0f, 1.0f ) );
  Quaternion q2( Radian( Degree( 80 ) ), Vector3( 0.0f, 0.0f, 1.0f ) );

  Quaternion p = Quaternion::Lerp( q1, q2, 0.0f );
  DALI_TEST_EQUALS( p, q1, 0.001f, TEST_LOCATION );

  p = Quaternion::Lerp( q1, q2, 1.0f );
  DALI_TEST_EQUALS( p, q2, 0.001f, TEST_LOCATION );

  p = Quaternion::Lerp( q1, q2, 0.5f );
  Quaternion r1 = ( q1 + q2 ) * 0.5f;
  r1.Normalize();
  DALI_TEST_EQUALS( p, r1, 0.001f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionSlerpP01(void)
{
  Quaternion q1(Radian(M_PI/4.0f), Vector3(0.0f, 0.0f, 1.0f));
  Quaternion q2(Radian(-M_PI/4.0f), Vector3(0.0f, 0.0f, 1.0f));

  Quaternion q = Quaternion::Slerp( q1, q2, 0.0f );
  DALI_TEST_EQUALS( q, q1, 0.001, TEST_LOCATION );

  q = Quaternion::Slerp( q1, q2, 1.0f );
  DALI_TEST_EQUALS( q, q2, 0.001, TEST_LOCATION );

  // @ 25%, will be at M_PI/8
  q = Quaternion::Slerp( q1, q2, 0.25f );
  Vector3 axis;
  Radian angle;
  bool converted = q.ToAxisAngle( axis, angle );
  DALI_TEST_EQUALS( converted, true, TEST_LOCATION );
  DALI_TEST_EQUALS( angle.radian, Math::PI/8.0f, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( axis.x, 0.0f, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( axis.y, 0.0f, 0.001, TEST_LOCATION );
  DALI_TEST_EQUALS( axis.z, 1.0f, 0.001, TEST_LOCATION );
  END_TEST;
}



int UtcDaliQuaternionSlerpP02(void)
{
  Quaternion q1( Dali::ANGLE_30, Vector3(0.0f, 0.0f, 1.0f));
  Quaternion q2( Dali::ANGLE_90, Vector3(0.0f, 0.0f, 1.0f));

  Quaternion q = Quaternion::Slerp( q1, q2, 0.0f );

  DALI_TEST_EQUALS( q,  q1, 0.001, TEST_LOCATION );

  q = Quaternion::Slerp( q1, q2, 1.0f );

  DALI_TEST_EQUALS( q,  q2, 0.001, TEST_LOCATION );

  // @ 50%, will be at M_PI/3 around z
  q = Quaternion::Slerp( q1, q2, 0.5f );

  Quaternion r( Dali::ANGLE_120, Vector3( 0.0f, 0.0f, 1.0f));
  DALI_TEST_EQUALS( q, r, 0.001, TEST_LOCATION );
  END_TEST;
}


int UtcDaliQuaternionSlerpP03(void)
{
  Quaternion q1( Radian( Degree( 125 ) ), Vector3( 0.0f, 0.0f, 1.0f ) );
  Quaternion q2( Radian( Degree( -125 ) ), Vector3( 0.002f, 0.001f, 1.001f ) );

  Quaternion q = Quaternion::Slerp( q1, q2, 0.0f );
  DALI_TEST_EQUALS( q,  q1, 0.001, TEST_LOCATION );

  q = Quaternion::Slerp( q1, q2, 1.0f );
  DALI_TEST_EQUALS( q,  q2, 0.001, TEST_LOCATION );

  q = Quaternion::Slerp(q1, q2, 0.05f);
  Vector3 axis;
  Radian angle;
  bool converted = q.ToAxisAngle(axis, angle);
  DALI_TEST_EQUALS(converted, true, TEST_LOCATION);

  DALI_TEST_EQUALS( axis.x,  0.0f, 0.01, TEST_LOCATION );
  DALI_TEST_EQUALS( axis.y,  0.0f, 0.01, TEST_LOCATION );
  DALI_TEST_EQUALS( axis.z,  1.0f, 0.01, TEST_LOCATION );
  END_TEST;
}



int UtcDaliQuaternionSlerpP04(void)
{
  Quaternion q1( Radian( Degree( 120 ) ), Vector3( 0.0f, 0.0f, 1.0f ) );
  Quaternion q2( Radian( Degree( 130 ) ), Vector3( 0.0f, 0.0f, 1.0f ) );

  Quaternion q = Quaternion::Slerp( q1, q2, 0.0f );
  DALI_TEST_EQUALS( q,  q1, 0.001, TEST_LOCATION );

  q = Quaternion::Slerp( q1, q2, 1.0f );
  DALI_TEST_EQUALS( q,  q2, 0.001, TEST_LOCATION );

  q = Quaternion::Slerp(q1, q2, 0.5f);
  Vector3 axis;
  Radian angle;
  bool converted = q.ToAxisAngle(axis, angle);
  DALI_TEST_EQUALS(converted, true, TEST_LOCATION);
  DALI_TEST_EQUALS(angle.radian, float(Radian(Degree(125))), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.x,  0.0f, 0.01, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.y,  0.0f, 0.01, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.z,  1.0f, 0.01, TEST_LOCATION);
  END_TEST;
}



int UtcDaliQuaternionSlerpNoInvertP01(void)
{
  Quaternion q1( Dali::ANGLE_45, Vector3(0.0f, 0.0f, 1.0f));
  Quaternion q2(-Dali::ANGLE_45, Vector3(0.0f, 0.0f, 1.0f));

  Quaternion q = Quaternion::SlerpNoInvert( q1, q2, 0.0f );
  DALI_TEST_EQUALS( q, q1, 0.001, TEST_LOCATION );

  q = Quaternion::SlerpNoInvert( q1, q2, 1.0f );
  DALI_TEST_EQUALS( q, q2, 0.001, TEST_LOCATION );

  // @ 25%, will be at M_PI/8
  q = Quaternion::SlerpNoInvert(q1, q2, 0.25f);
  Vector3 axis;
  Radian angle;
  bool converted = q.ToAxisAngle(axis, angle);
  DALI_TEST_EQUALS(converted, true, TEST_LOCATION);
  DALI_TEST_EQUALS(angle.radian, Math::PI/8.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.x, 0.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.y, 0.0f, 0.001, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.z, 1.0f, 0.001, TEST_LOCATION);
  END_TEST;
}


int UtcDaliQuaternionSlerpNoInvertP02(void)
{
  Quaternion q1( Radian( Degree( 120 ) ), Vector3( 0.0f, 0.0f, 1.0f ) );
  Quaternion q2( Radian( Degree( 130 ) ), Vector3( 0.0f, 0.0f, 1.0f ) );

  Quaternion q = Quaternion::SlerpNoInvert( q1, q2, 0.0f );
  DALI_TEST_EQUALS( q,  q1, 0.001, TEST_LOCATION );

  q = Quaternion::SlerpNoInvert( q1, q2, 1.0f );
  DALI_TEST_EQUALS( q,  q2, 0.001, TEST_LOCATION );

  q = Quaternion::SlerpNoInvert(q1, q2, 0.5f);
  Vector3 axis;
  Radian angle;
  bool converted = q.ToAxisAngle(axis, angle);
  DALI_TEST_EQUALS(converted, true, TEST_LOCATION);
  DALI_TEST_EQUALS(angle.radian, float(Radian(Degree(125))), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.x,  0.0f, 0.01, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.y,  0.0f, 0.01, TEST_LOCATION);
  DALI_TEST_EQUALS(axis.z,  1.0f, 0.01, TEST_LOCATION);
  END_TEST;
}


int UtcDaliQuaternionSquadP(void)
{
  Quaternion q1( Radian( Degree( 45 ) ),     Vector3( 0.0f, 0.0f, 1.0f ) );
  Quaternion q1out( Radian( Degree( 40 ) ),  Vector3( 0.0f, 1.0f, 2.0f ) );
  Quaternion q2in( Radian( Degree( 35 ) ),   Vector3( 0.0f, 2.0f, 3.0f ) );
  Quaternion q2( Radian( Degree( 30 ) ),     Vector3( 0.0f, 1.0f, 3.0f ) );

  Quaternion q = Quaternion::Squad( q1, q2, q1out, q2in, 0.0f );
  DALI_TEST_EQUALS( q, q1, 0.001f, TEST_LOCATION );

  q = Quaternion::Squad( q1, q2, q1out, q2in, 1.0f );
  DALI_TEST_EQUALS( q, q2, 0.001f, TEST_LOCATION );

  // Don't know what actual value should be, but can make some informed guesses.
  q = Quaternion::Squad(q1, q2, q1out, q2in, 0.5f);
  Radian angle;
  Vector3 axis;
  q.Normalize();
  q.ToAxisAngle( axis, angle );

  if( angle < 0.0f )
  {
    q = -q; // Might get negative quat
    q.ToAxisAngle( axis, angle );
  }
  float deg = Degree(angle).degree;
  DALI_TEST_CHECK(deg >= 0 && deg <= 90);
  DALI_TEST_CHECK(axis.y > 0);
  DALI_TEST_CHECK(axis.z > 0);
  END_TEST;
}

int UtcDaliAngleBetweenP(void)
{
  Quaternion q1( ANGLE_45, ANGLE_0, ANGLE_0 );
  Quaternion q2(Radian(Degree(47)), ANGLE_0, ANGLE_0 );
  DALI_TEST_EQUALS(Quaternion::AngleBetween(q1, q2), fabsf(Radian(Degree(45)) - Radian(Degree(47))), 0.001f, TEST_LOCATION);

  Quaternion q3( Radian( Degree( 80 ) ), Vector3::YAXIS );
  Quaternion q4( Radian( Degree( 90 ) ), Vector3::YAXIS );
  DALI_TEST_EQUALS( Quaternion::AngleBetween( q3, q4 ), fabsf( Radian( Degree( 80 ) ) - Radian( Degree( 90 ) ) ), 0.001f, TEST_LOCATION );

  Quaternion q5( Radian( Degree( 0 ) ), Vector3::YAXIS );
  Quaternion q6( Radian( Degree( 90 ) ), Vector3::XAXIS );
  DALI_TEST_EQUALS( Quaternion::AngleBetween( q5, q6 ), fabsf( Radian( Degree( 0 ) ) - Radian( Degree( 90 ) ) ), 0.001f, TEST_LOCATION );

  Quaternion q7( Radian( Degree( 0 ) ), Vector3::YAXIS );
  Quaternion q8( Radian( Degree( 0 ) ), Vector3::XAXIS );
  DALI_TEST_EQUALS( Quaternion::AngleBetween( q7, q8 ), fabsf( Radian( Degree( 0 ) ) - Radian( Degree( 0 ) ) ), 0.001f, TEST_LOCATION );

  Quaternion q9( Radian( Degree( 0 ) ), Vector3::XAXIS );
  Quaternion q10( Radian( Degree( 180 ) ), Vector3::XAXIS );
  DALI_TEST_EQUALS( Quaternion::AngleBetween( q9, q10 ), fabsf( Radian( Degree( 0 ) ) - Radian( Degree( 180 ) ) ), 0.001f, TEST_LOCATION );

  Quaternion q11( Radian( Degree( 1 ) ), Vector3::YAXIS );
  Quaternion q12( Radian( Degree( 240 ) ), Vector3::YAXIS );
  DALI_TEST_EQUALS( Quaternion::AngleBetween( q11, q12 ), fabsf( Radian( Degree( 1 - 240 + 360 ) ) ), 0.001f, TEST_LOCATION );

  Quaternion q13( Radian( Degree( 240 ) ), Vector3::YAXIS );
  Quaternion q14( Radian( Degree( 1 ) ), Vector3::YAXIS );
  DALI_TEST_EQUALS( Quaternion::AngleBetween( q13, q14 ), fabsf( Radian( Degree( 240 - 1 - 360 ) ) ), 0.001f, TEST_LOCATION );

  Quaternion q15( Radian( Degree( 240 ) ), Vector3::YAXIS );
  Quaternion q16( Radian( Degree( 1 ) ), Vector3::ZAXIS );
  DALI_TEST_EQUALS( Quaternion::AngleBetween( q15, q16 ), Quaternion::AngleBetween( q16, q15 ), 0.001f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliQuaternionOStreamOperatorP(void)
{
  std::ostringstream oss;

  Quaternion quaternion( Dali::ANGLE_180, Vector3::YAXIS );

  oss << quaternion;

  std::string expectedOutput = "[ Axis: [0, 1, 0], Angle: 180 degrees ]";

  DALI_TEST_EQUALS( oss.str(), expectedOutput, TEST_LOCATION );
  END_TEST;
}
