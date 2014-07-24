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
#include <dali/public-api/math/vector4.h>

// EXTERNAL INCLUDES
#include <math.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/internal/render/common/performance-monitor.h>

namespace Dali
{

using Internal::PerformanceMonitor;

const Vector4 Vector4::ONE(1.0f, 1.0f, 1.0f, 1.0f);
const Vector4 Vector4::XAXIS(1.0f, 0.0f, 0.0f, 0.0f);
const Vector4 Vector4::YAXIS(0.0f, 1.0f, 0.0f, 0.0f);
const Vector4 Vector4::ZAXIS(0.0f, 0.0f, 1.0f, 0.0f);
const Vector4 Vector4::ZERO(0.0f, 0.0f, 0.0f, 0.0f);

Vector4::Vector4( const Vector2& vec2 )
: x(vec2.x),
  y(vec2.y),
  z(0.0f),
  w(0.0f)
{
}

Vector4::Vector4( const Vector3& vec3 )
: x(vec3.x),
  y(vec3.y),
  z(vec3.z),
  w(0.0f)
{
}

Vector4& Vector4::operator=(const Vector2& vec2 )
{
  // only set x and y
  x = vec2.x;
  y = vec2.y;
  return *this;
}

Vector4& Vector4::operator=(const Vector3& vec3 )
{
  // only set x, y and z
  x = vec3.x;
  y = vec3.y;
  z = vec3.z;
  return *this;
}


bool Vector4::operator==(const Vector4& rhs) const
{
  if (fabsf(x - rhs.x) > GetRangedEpsilon(x, rhs.x) )
  {
    return false;
  }
  if (fabsf(y - rhs.y) > GetRangedEpsilon(y, rhs.y) )
  {
    return false;
  }
  if (fabsf(z - rhs.z) > GetRangedEpsilon(z, rhs.z) )
  {
    return false;
  }
  if (fabsf(w - rhs.w) > GetRangedEpsilon(w, rhs.w) )
  {
    return false;
  }

  return true;
}

float Vector4::Dot(const Vector3 &other) const
{
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,3);

  return x * other.x + y * other.y + z * other.z;
}

float Vector4::Dot(const Vector4 &other) const
{
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,3);

  return x * other.x + y * other.y + z * other.z;
}

float Vector4::Dot4(const Vector4 &other) const
{
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,4);

  return x * other.x + y * other.y + z * other.z + w * other.w;
}

Vector4 Vector4::Cross(const Vector4 &other) const
{
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,6);

  return Vector4( (y * other.z) - (z * other.y),
                  (z * other.x) - (x * other.z),
                  (x * other.y) - (y * other.x),
                  0.0f);
}

// Will always return positive square root
float Vector4::Length() const
{
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,3);

  return sqrtf(x*x + y*y + z*z);
}

float Vector4::LengthSquared() const
{
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,3);

  return x*x + y*y + z*z;
}

void Vector4::Normalize()
{
  const float length = Length();

  // In the case where the length is exactly zero, the vector cannot be normalized.
  if ( ! EqualsZero( length ) )
  {
    MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY, 3);

    const float inverseLength = 1.0f / length;
    x *= inverseLength;
    y *= inverseLength;
    z *= inverseLength;
  }
}

void Vector4::Clamp( const Vector4& min, const Vector4& max )
{
  Dali::ClampInPlace<float>( x, min.x, max.x );
  Dali::ClampInPlace<float>( y, min.y, max.y );
  Dali::ClampInPlace<float>( z, min.z, max.z );
  Dali::ClampInPlace<float>( w, min.w, max.w );
}

std::ostream& operator<< (std::ostream& o, const Vector4& vector)
{
  return o << "[" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << "]";
}

Vector4 Clamp( const Vector4& v, const float& min, const float& max )
{
  Vector4 result( v );
  result.Clamp( Vector4( min, min, min, min ), Vector4( max, max, max, max) );

  return result;
}

} // namespace Dali
