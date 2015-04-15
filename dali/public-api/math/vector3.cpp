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
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/quaternion.h>

// EXTERNAL INCLUDES
#include <math.h>
#include <ostream>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/internal/render/common/performance-monitor.h>

namespace Dali
{
using Internal::PerformanceMonitor;

const Vector3 Vector3::ONE(1.0f, 1.0f, 1.0f);
const Vector3 Vector3::XAXIS(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::YAXIS(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::ZAXIS(0.0f, 0.0f, 1.0f);
const Vector3 Vector3::NEGATIVE_XAXIS(-1.0f, 0.0f, 0.0f);
const Vector3 Vector3::NEGATIVE_YAXIS(0.0f, -1.0f, 0.0f);
const Vector3 Vector3::NEGATIVE_ZAXIS(0.0f, 0.0f, -1.0f);
const Vector3 Vector3::ZERO(0.0f, 0.0f, 0.0f);

Vector3::Vector3( const Vector2& vec2 )
: x(vec2.x),
  y(vec2.y),
  z(0.0f)
{
}

Vector3::Vector3( const Vector4& vec4 )
: x(vec4.x),
  y(vec4.y),
  z(vec4.z)
{
}

Vector3& Vector3::operator=(const Vector2& rhs)
{
  x = rhs.x;
  y = rhs.y;
  z = 0.0f;

  return *this;
}

Vector3& Vector3::operator=(const Vector4& rhs)
{
  x = rhs.x;
  y = rhs.y;
  z = rhs.z;

  return *this;
}

Vector3& Vector3::operator*=(const Quaternion& rhs)
{
  // nVidia SDK implementation
  Vector3 qvec(rhs.mVector.x, rhs.mVector.y, rhs.mVector.z);

  Vector3 uv( (qvec.y * z) - (qvec.z * y),
              (qvec.z * x) - (qvec.x * z),
              (qvec.x * y) - (qvec.y * x) );

  Vector3 uuv( (qvec.y * uv.z) - (qvec.z * uv.y),
               (qvec.z * uv.x) - (qvec.x * uv.z),
               (qvec.x * uv.y) - (qvec.y * uv.x) );

  uv *= rhs.mVector.w;

  x += (uv.x + uuv.x) * 2.0f;
  y += (uv.y + uuv.y) * 2.0f;
  z += (uv.z + uuv.z) * 2.0f;

  return *this;
}

bool Vector3::operator==(const Vector3& rhs) const
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

  return true;
}

float Vector3::Dot(const Vector3 &other) const
{
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,3);

  return x * other.x + y * other.y + z * other.z;
}

Vector3 Vector3::Cross(const Vector3 &other) const
{
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,6);

  return Vector3( (y * other.z) - (z * other.y),
                  (z * other.x) - (x * other.z),
                  (x * other.y) - (y * other.x));
}

float Vector3::Length() const
{
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,3);

  return sqrtf(x*x + y*y + z*z);
}

float Vector3::LengthSquared() const
{
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,3);

  return x*x + y*y + z*z;
}

void Vector3::Normalize()
{
  float length = Length();
  if( ! EqualsZero(length) )
  {
    MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,3);

    const float inverseLength = 1.0f / length;
    x *= inverseLength;
    y *= inverseLength;
    z *= inverseLength;
  }
}

void Vector3::Clamp( const Vector3& min, const Vector3& max )
{
  Dali::ClampInPlace<float>( x, min.x, max.x );
  Dali::ClampInPlace<float>( y, min.y, max.y );
  Dali::ClampInPlace<float>( z, min.z, max.z );
}

std::ostream& operator<< (std::ostream& o, const Vector3& vector)
{
  return o << "[" << vector.x << ", " << vector.y << ", " << vector.z << "]";
}

Vector3 Clamp( const Vector3& v, const float& min, const float& max )
{
  Vector3 result( v );
  result.Clamp( Vector3( min, min, min ) , Vector3( max, max, max ) );

  return result;
}

Vector3 FitKeepAspectRatio( const Vector3& target, const Vector3& source )
{
  float scale = 0.0f;

  if ( fabsf(source.x) > 0.0f )
  {
    scale = target.x / source.x;
  }

  if ( fabsf(source.y) > 0.0f )
  {
    if ( scale > Math::MACHINE_EPSILON_1 )
    {
      scale =  std::min( scale, target.y / source.y );
    }
    else
    {
      scale = target.y / source.y;
    }
  }

  if ( fabsf(source.z) > 0.0f )
  {
    if ( scale > Math::MACHINE_EPSILON_1 )
    {
      scale =  std::min( scale, target.z / source.z );
    }
    else
    {
      scale = target.z / source.z;
    }
  }

  if ( scale < Math::MACHINE_EPSILON_1 )
  {
    scale = 1.0f;
  }

  return Vector3( scale, scale, scale );
}

Vector3 FillKeepAspectRatio( const Vector3& target, const Vector3& source )
{
  float scale = 0.0f;

  if ( fabsf(source.x) > 0.0f )
  {
    scale = target.x / source.x;
  }

  if ( fabsf(source.y) > 0.0f )
  {
    if ( scale > Math::MACHINE_EPSILON_1 )
    {
      scale =  std::max( scale, target.y / source.y );
    }
    else
    {
      scale = target.y / source.y;
    }
  }

  if ( fabsf(source.z) > 0.0f )
  {
    if ( scale > Math::MACHINE_EPSILON_1 )
    {
      scale =  std::max( scale, target.z / source.z );
    }
    else
    {
      scale = target.z / source.z;
    }
  }

  if ( scale < Math::MACHINE_EPSILON_1 )
  {
    scale = 1.0f;
  }

  return Vector3( scale, scale, scale );
}

Vector3 FillXYKeepAspectRatio( const Vector3& target, const Vector3& source )
{
  float scale = 0.0f;

  if ( fabsf(source.x) > 0.0f )
  {
    scale = target.x / source.x;
  }

  if ( fabsf(source.y) > 0.0f )
  {
    if ( scale > Math::MACHINE_EPSILON_1 )
    {
      scale =  std::max( scale, target.y / source.y );
    }
    else
    {
      scale = target.y / source.y;
    }
  }

  if ( scale < Math::MACHINE_EPSILON_1 )
  {
    scale = 1.0f;
  }

  return Vector3( scale, scale, scale );
}

Vector3 ShrinkInsideKeepAspectRatio( const Vector3& target, const Vector3& source )
{
  // calculate source size vs target size to see if we need to shrink
  float widthScale = 1.0f;
  if( target.width < source.width )
  {
    // not enough width, width needs to shrink
    widthScale = target.width / source.width;
  }
  float heightScale = 1.0f;
  if( target.height < source.height )
  {
    // not enough height, height needs to shrink
    heightScale = target.height / source.height;
  }
  float depthScale = 1.0f;
  if( target.depth < source.depth )
  {
    // not enough depth, depth needs to shrink
    depthScale = target.depth / source.depth;
  }
  // use smaller of the scales
  float scale = std::min( std::min( widthScale, heightScale ), depthScale );

  // check if we need to scale
  if( scale < 1.0f )
  {
    // scale natural size to fit inside
    return Vector3( scale, scale, scale );
  }
  // there is enough space so use source size
  return Vector3::ONE;
}

} // namespace Dali
