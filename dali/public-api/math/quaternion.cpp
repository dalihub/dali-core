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

// CLASS HEADER
#include <dali/public-api/math/quaternion.h>

// EXTERNAL INCLUDES
#include <ostream>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/degree.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/internal/render/common/performance-monitor.h>

namespace Dali
{
using Internal::PerformanceMonitor;

const Quaternion Quaternion::IDENTITY;


/**
 * Default Constructor
 */
Quaternion::Quaternion()
 : mVector( 0.0f, 0.0f, 0.0f, 1.0f )
{
}

Quaternion::Quaternion( float cosThetaBy2, float iBySineTheta, float jBySineTheta, float kBySineTheta ) :
  mVector( iBySineTheta, jBySineTheta, kBySineTheta, cosThetaBy2 )
{
}

Quaternion::Quaternion( const Vector4& vector )
{
  mVector = vector;
}

Quaternion::Quaternion( Radian angle, const Vector3& axis )
{
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,4);

  Vector3 tmpAxis = axis;
  tmpAxis.Normalize();
  const float halfAngle = angle.radian * 0.5f;
  const float sinThetaByTwo = sinf(halfAngle);
  const float cosThetaByTwo = cosf(halfAngle);
  mVector.x = tmpAxis.x * sinThetaByTwo;
  mVector.y = tmpAxis.y * sinThetaByTwo;
  mVector.z = tmpAxis.z * sinThetaByTwo;
  mVector.w = cosThetaByTwo;
}

Quaternion::Quaternion( Radian pitch, Radian yaw, Radian roll )
{
  SetEuler( pitch, yaw, roll );
}

Quaternion::Quaternion( const Matrix& matrix )
{
  Vector3 xAxis( matrix.GetXAxis() );
  Vector3 yAxis( matrix.GetYAxis() );
  Vector3 zAxis( matrix.GetZAxis() );

  SetFromAxes( xAxis, yAxis, zAxis );
}

Quaternion::Quaternion( const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis )
{
  SetFromAxes( xAxis, yAxis, zAxis );
}

Quaternion::Quaternion( const Vector3& v0, const Vector3& v1 )
{
  float dot = v0.Dot(v1);
  if( dot > 1.0f - Math::MACHINE_EPSILON_1 )
  {
    //Identity quaternion
    mVector.x = mVector.y = mVector.z = 0.0f;
    mVector.w = 1.0f;
  }
  else if( dot < -1.0f + Math::MACHINE_EPSILON_1)
  {
    //180 degree rotation across the Z axis
    mVector.x = mVector.y = mVector.w = 0.0f;
    mVector.z = 1.0f;
  }
  else
  {
    Vector3 w = v0.Cross(v1);
    mVector.w = 1.0f + dot;
    mVector.x = w.x;
    mVector.y = w.y;
    mVector.z = w.z;
    Normalize();
  }
}

Quaternion::~Quaternion()
{
}

bool Quaternion::IsIdentity() const
{
  // start from w as its unlikely that any real rotation has w == 1
  // Uses a relaxed epsilon, as composition of rotation introduces error
  return ( ( fabsf( mVector.w - 1.0f ) < Math::MACHINE_EPSILON_10 )&&
           ( fabsf( mVector.x ) < Math::MACHINE_EPSILON_10 )&&
           ( fabsf( mVector.y ) < Math::MACHINE_EPSILON_10 )&&
           ( fabsf( mVector.z ) < Math::MACHINE_EPSILON_10 ) );
}

bool Quaternion::ToAxisAngle(Vector3& axis, Radian& angle) const
{
  angle = acosf(mVector.w);
  bool converted = false;
  // pre-compute to save time
  const float sine = sinf( angle.radian );

  // If sine(angle) is zero, conversion is not possible

  if ( ! EqualsZero( sine ) )
  {
    MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,3);

    float sinf_theta_inv = 1.0f / sine;

    axis.x = mVector.x*sinf_theta_inv;
    axis.y = mVector.y*sinf_theta_inv;
    axis.z = mVector.z*sinf_theta_inv;
    angle.radian *= 2.0f;
    converted = true;
  }
  return converted;
}

const Vector4& Quaternion::AsVector() const
{
  return mVector;
}

void Quaternion::SetEuler( Radian pitch, Radian yaw, Radian roll )
{
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,19);

  const float halfX = 0.5f * pitch.radian;
  const float halfY = 0.5f * yaw.radian;
  const float halfZ = 0.5f * roll.radian;

  float cosX2 = cosf(halfX);
  float cosY2 = cosf(halfY);
  float cosZ2 = cosf(halfZ);

  float sinX2 = sinf(halfX);
  float sinY2 = sinf(halfY);
  float sinZ2 = sinf(halfZ);

  mVector.w = cosZ2 * cosY2 * cosX2 + sinZ2 * sinY2 * sinX2;
  mVector.x = cosZ2 * cosY2 * sinX2 - sinZ2 * sinY2 * cosX2;
  mVector.y = cosZ2 * sinY2 * cosX2 + sinZ2 * cosY2 * sinX2;
  mVector.z = sinZ2 * cosY2 * cosX2 - cosZ2 * sinY2 * sinX2;
}

Vector4 Quaternion::EulerAngles() const
{
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,13);

  float sqw = mVector.w*mVector.w;
  float sqx = mVector.x*mVector.x;
  float sqy = mVector.y*mVector.y;
  float sqz = mVector.z*mVector.z;

  Vector4 euler;
  euler.x = atan2f(2.0f * (mVector.y*mVector.z + mVector.x*mVector.w), -sqx - sqy + sqz + sqw);
  euler.y = asinf(-2.0f * (mVector.x*mVector.z - mVector.y*mVector.w));
  euler.z = atan2f(2.0f * (mVector.x*mVector.y + mVector.z*mVector.w), sqx - sqy - sqz + sqw);
  return euler;
}

const Quaternion Quaternion::operator+( const Quaternion& other ) const
{
  return Quaternion(mVector + other.mVector);
}

const Quaternion Quaternion::operator-( const Quaternion& other ) const
{
  return Quaternion(mVector - other.mVector);
}

const Quaternion Quaternion::operator*( const Quaternion& other ) const
{
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,12);

  return Quaternion(mVector.w * other.mVector.w - mVector.Dot(other.mVector),
                    mVector.y * other.mVector.z - mVector.z * other.mVector.y + mVector.w * other.mVector.x + mVector.x * other.mVector.w,
                    mVector.z * other.mVector.x - mVector.x * other.mVector.z + mVector.w * other.mVector.y + mVector.y * other.mVector.w,
                    mVector.x * other.mVector.y - mVector.y * other.mVector.x + mVector.w * other.mVector.z + mVector.z * other.mVector.w);
}

Vector3 Quaternion::operator*( const Vector3& other ) const
{
  Vector3 qvec(mVector.x, mVector.y, mVector.z);
  Vector3 uv = qvec.Cross( other );
  Vector3 uuv = qvec.Cross(uv);
  uv *= (2.0f * mVector.w);
  uuv *= 2.0f;

  return other + uv + uuv;
}

const Quaternion Quaternion::operator/( const Quaternion& q ) const
{
  Quaternion p(q);
  p.Invert();
  return *this * p;
}

const Quaternion Quaternion::operator*( float scale ) const
{
  return Quaternion(mVector*scale);
}

const Quaternion Quaternion::operator/( float scale ) const
{
  return Quaternion(mVector/scale);
}

Quaternion Quaternion::operator-() const
{
  return Quaternion(-mVector.w, -mVector.x, -mVector.y, -mVector.z);
}

const Quaternion& Quaternion::operator+=( const Quaternion& q )
{
  mVector += q.mVector; return *this;
}

const Quaternion& Quaternion::operator-=( const Quaternion& q )
{
  mVector -= q.mVector; return *this;
}

const Quaternion& Quaternion::operator*=( const Quaternion& q )
{
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,12);

  float x = mVector.x, y = mVector.y, z = mVector.z, w = mVector.w;

  mVector.w = mVector.w * q.mVector.w - mVector.Dot(q.mVector);
  mVector.x = y*q.mVector.z - z*q.mVector.y + w*q.mVector.x + x*q.mVector.w;
  mVector.y = z*q.mVector.x - x*q.mVector.z + w*q.mVector.y + y*q.mVector.w;
  mVector.z = x*q.mVector.y - y*q.mVector.x + w*q.mVector.z + z*q.mVector.w;
  return *this;
}

const Quaternion& Quaternion::operator*=( float scale )
{
  mVector*=scale; return *this;
}

const Quaternion& Quaternion::operator/=( float scale )
{
  mVector/=scale; return *this;
}

bool Quaternion::operator==( const Quaternion& rhs ) const
{
  return ( ( fabsf(mVector.x - rhs.mVector.x) < Math::MACHINE_EPSILON_1 &&
             fabsf(mVector.y - rhs.mVector.y) < Math::MACHINE_EPSILON_1 &&
             fabsf(mVector.z - rhs.mVector.z) < Math::MACHINE_EPSILON_1 &&
             fabsf(mVector.w - rhs.mVector.w) < Math::MACHINE_EPSILON_1 ) ||
           // Or equal to negation of rhs
           ( fabsf(mVector.x + rhs.mVector.x) < Math::MACHINE_EPSILON_1 &&
             fabsf(mVector.y + rhs.mVector.y) < Math::MACHINE_EPSILON_1 &&
             fabsf(mVector.z + rhs.mVector.z) < Math::MACHINE_EPSILON_1 &&
             fabsf(mVector.w + rhs.mVector.w) < Math::MACHINE_EPSILON_1 )
         );
}

bool Quaternion::operator!=( const Quaternion& rhs ) const
{
  return !operator==(rhs);
}

float Quaternion::Length() const
{
  return (float)sqrt(mVector.w * mVector.w + mVector.Dot(mVector));
}

float Quaternion::LengthSquared() const
{
  return (float)(mVector.w * mVector.w + mVector.Dot(mVector));
}

void Quaternion::Normalize()
{
  *this/=Length();
}

Quaternion Quaternion::Normalized() const
{
  return  *this/Length();
}

void Quaternion::Conjugate()
{
  mVector.x = -mVector.x;
  mVector.y = -mVector.y;
  mVector.z = -mVector.z;
}

void Quaternion::Invert()
{
  Conjugate();
  *this/=LengthSquared();
}

Quaternion Quaternion::Log() const
{
  float a = acosf(mVector.w);
  float sina = sinf(a);
  Quaternion ret;

  ret.mVector.w = 0;
  if (fabsf(sina) >= Math::MACHINE_EPSILON_1)
  {
    MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,4);

    float angleBySinAngle = a * (1.0f / sina);
    ret.mVector.x = mVector.x * angleBySinAngle;
    ret.mVector.y = mVector.y * angleBySinAngle;
    ret.mVector.z = mVector.z * angleBySinAngle;
  }
  else
  {
    ret.mVector.x= ret.mVector.y= ret.mVector.z= 0;
  }
  return ret;
}

Quaternion Quaternion::Exp() const
{
  DALI_ASSERT_ALWAYS( EqualsZero( mVector.w ) && "Cannot perform Exponent" );

  float a = mVector.Length();
  float sina = sinf(a);
  Quaternion ret;

  ret.mVector.w = cosf(a);

  if (a >= Math::MACHINE_EPSILON_1)
  {
    MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,4);

    float sinAOverA = sina * (1.0f / a);
    ret.mVector.x = mVector.x * sinAOverA;
    ret.mVector.y = mVector.y * sinAOverA;
    ret.mVector.z = mVector.z * sinAOverA;
  }
  else
  {
    ret.mVector.x = ret.mVector.y = ret.mVector.z = 0.0f;
  }
  return ret;
}

float Quaternion::Dot( const Quaternion& q1, const Quaternion& q2 )
{
  return q1.mVector.Dot4(q2.mVector);
}

Quaternion Quaternion::Lerp(const Quaternion& q1, const Quaternion& q2, float t )
{
  return (q1*(1.0f-t) + q2*t).Normalized();
}

Quaternion Quaternion::Slerp( const Quaternion& q1, const Quaternion& q2, float progress )
{
  Quaternion q3;
  float cosTheta = Quaternion::Dot(q1, q2);

  /**
   * If cos(theta) < 0, q1 and q2 are more than 90 degrees apart,
   * so invert one to reduce spinning.
   */
  if (cosTheta < 0.0f)
  {
    cosTheta = -cosTheta;
    q3 = -q2;
  }
  else
  {
    q3 = q2;
  }

  if (fabsf(cosTheta) < 0.95f)
  {
    MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,5);

    // Normal SLERP
    float sine = sqrtf(1.0f - cosTheta*cosTheta);
    float angle = atan2f(sine, cosTheta);
    float invSine = 1.0f / sine;
    float coeff0 = sinf((1.0f - progress) * angle) * invSine;
    float coeff1 = sinf(progress * angle) * invSine;

    return q1*coeff0 + q3*coeff1;
  }
  else
  {
    // If the angle is small, use linear interpolation
    Quaternion result = q1*(1.0f - progress) + q3*progress;

    return result.Normalized();
  }
}

Quaternion Quaternion::SlerpNoInvert( const Quaternion& q1, const Quaternion& q2, float t )
{
  float cosTheta = Quaternion::Dot(q1, q2);

  if (cosTheta > -0.95f && cosTheta < 0.95f)
  {
    MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,2);

    float theta = acosf(cosTheta);
    return (q1*sinf(theta*(1.0f-t)) + q2*sinf(theta*t))/sinf(theta);
  }
  else
  {
    return Lerp(q1, q2, t);
  }
}

Quaternion Quaternion::Squad( const Quaternion& start, const Quaternion& end,  const Quaternion& ctrl1,  const Quaternion& ctrl2, float t )
{
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,2);

  Quaternion c = SlerpNoInvert( start, end, t );
  Quaternion d = SlerpNoInvert( ctrl1, ctrl2, t );
  return SlerpNoInvert( c, d, 2*t*(1-t) );
}

float Quaternion::AngleBetween( const Quaternion& q1, const Quaternion& q2 )
{
  Quaternion from(q1);
  Quaternion to(q2);

  from.Normalize();
  to.Normalize();

  //Formula for angle θ between two quaternion is:
  //θ = cos^−1 (2⟨q1,q2⟩^2 − 1), Where (q1,q2) is inner product of the quaternions.
  float X = from.mVector.Dot4(to.mVector);
  float theta = acos( (2 * X * X) - 1);

  return theta;
}

Vector4 Quaternion::Rotate( const Vector4& vector ) const
{
  Quaternion V(0.0f, vector.x, vector.y, vector.z);
  Quaternion conjugate(*this);
  conjugate.Conjugate();
  return (*this * V * conjugate).mVector;
}

Vector3 Quaternion::Rotate( const Vector3& vector ) const
{
  Quaternion V(0.0f, vector.x, vector.y, vector.z);
  Quaternion conjugate(*this);
  conjugate.Conjugate();
  return Vector3((*this * V * conjugate).mVector);
}

void Quaternion::SetFromAxes( const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis )
{
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,4);

  float t = xAxis.x + yAxis.y + zAxis.z;
  if ( t > 0.0f )                                      // w is largest
  {
    float root = sqrtf( t + 1.0f );
    float one_over_4w = 0.5f / root;
    mVector.x = ( yAxis.z - zAxis.y ) * one_over_4w;
    mVector.y = ( zAxis.x - xAxis.z ) * one_over_4w;
    mVector.z = ( xAxis.y - yAxis.x ) * one_over_4w;
    mVector.w = root * 0.5f;
  }
  else if( zAxis.z > xAxis.x && zAxis.z > yAxis.y )    // z is largest
  {
    float root = sqrtf( zAxis.z - xAxis.x - yAxis.y + 1.0f );
    float one_over_4w = 0.5f / root;
    mVector.x = ( xAxis.z + zAxis.x ) * one_over_4w;
    mVector.y = ( yAxis.z + zAxis.y ) * one_over_4w;
    mVector.z = root * 0.5f;
    mVector.w = ( xAxis.y - yAxis.x ) * one_over_4w;
  }
  else if( yAxis.y > xAxis.x )                         // y is largest
  {
    float root = sqrtf(yAxis.y - zAxis.z - xAxis.x + 1.0f );
    float one_over_4w = 0.5f / root;

    mVector.x = ( xAxis.y + yAxis.x ) * one_over_4w;
    mVector.y = root * 0.5f;
    mVector.z = ( zAxis.y + yAxis.z ) * one_over_4w;
    mVector.w = ( zAxis.x - xAxis.z ) * one_over_4w;
  }
  else                                                 // x is largest
  {
    float root = sqrtf( xAxis.x - yAxis.y - zAxis.z + 1.0f );
    float one_over_4w = 0.5f / root;
    mVector.x = root * 0.5f;
    mVector.y = ( yAxis.x + xAxis.y ) * one_over_4w;
    mVector.z = ( zAxis.x + xAxis.z ) * one_over_4w;
    mVector.w = ( yAxis.z - zAxis.y ) * one_over_4w;
  }

  Normalize();
}

std::ostream& operator<<( std::ostream& o, const Quaternion& quaternion )
{
  Vector3 axis;
  Radian angleRadians;

  quaternion.ToAxisAngle( axis, angleRadians );
  Degree degrees = Radian( angleRadians );

  return o << "[ Axis: [" << axis.x << ", " << axis.y << ", " << axis.z << "], Angle: " << degrees.degree << " degrees ]";
}

} // namespace Dali

