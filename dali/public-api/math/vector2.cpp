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
#include <dali/public-api/math/vector2.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/internal/render/common/performance-monitor.h>

// EXTERNAL INCLUDES
#include <math.h>
#include <iostream>

namespace Dali
{

const Vector2 Vector2::ONE(1.0f, 1.0f);
const Vector2 Vector2::XAXIS(1.0f, 0.0f);
const Vector2 Vector2::YAXIS(0.0f, 1.0f);
const Vector2 Vector2::NEGATIVE_XAXIS(-1.0f, 0.0f);
const Vector2 Vector2::NEGATIVE_YAXIS(0.0f, -1.0f);
const Vector2 Vector2::ZERO(0.0f, 0.0f);

Vector2::Vector2(const Vector3& vec3)
: x(vec3.x),
  y(vec3.y)
{
}

Vector2::Vector2(const Vector4& vec4)
: x(vec4.x),
  y(vec4.y)
{
}

Vector2& Vector2::operator=(const Vector3& rhs)
{
  x = rhs.x;
  y = rhs.y;

  return *this;
}

Vector2& Vector2::operator=(const Vector4& rhs)
{
  x = rhs.x;
  y = rhs.y;

  return *this;
}


bool Vector2::operator==(const Vector2& rhs) const
{
  if (fabsf(x - rhs.x) > GetRangedEpsilon(x, rhs.x))
  {
    return false;
  }
  if (fabsf(y - rhs.y) > GetRangedEpsilon(y, rhs.y))
  {
    return false;
  }

  return true;
}

float Vector2::Length() const
{
  return sqrtf(LengthSquared());
}

float Vector2::LengthSquared() const
{
  return (x*x) + (y*y);
}

void Vector2::Normalize()
{
  float length = Length();
  if( ! EqualsZero(length) )
  {
    MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,2);

    const float inverseLength = 1.0f / length;
    x *= inverseLength;
    y *= inverseLength;
  }
}

void Vector2::Clamp( const Vector2& min, const Vector2& max )
{
  Dali::ClampInPlace<float>( x, min.x, max.x );
  Dali::ClampInPlace<float>( y, min.y, max.y );
}

std::ostream& operator<< (std::ostream& o, const Vector2& vector)
{
  return o << "[" << vector.x << ", " << vector.y << "]";
}

Vector2 Clamp( const Vector2& v, const float& min, const float& max )
{
  Vector2 result( v );
  result.Clamp( Vector2( min, min ) , Vector2( max, max ) );

  return result;
}

Size FitInside( const Size& target, const Size& source )
{
  Size fitted; //zero initialized
  float widthScale = target.width / source.width;
  float heightScale = target.height / source.height;
  if( widthScale < heightScale )
  {
    // width scale is smaller so fit according to it
    fitted.width = source.width * widthScale;
    fitted.height = source.height * widthScale;
  }
  else
  {
    // height scale is smaller so fit according to it
    fitted.width = source.width * heightScale;
    fitted.height = source.height * heightScale;
  }
  return fitted;
}

Size FitScaleToFill( const Size& target, const Size& source )
{
  Size fitted = target;
  if( target.width > 0.0f && EqualsZero( target.height ) )
  {
    float scale = target.width / source.width;
    fitted.height = scale*source.height;
  }
  else if( target.height > 0.0f && EqualsZero( target.width ) )
  {
    float scale = target.height / source.height;
    fitted.width = scale * source.width;
  }
  else if( target.height > 0.0f && target.width > 0.0f ) // shouldn't be distorted
  {
    float widthScale = source.width / target.width;
    float heightScale = source.height / target.height;
    if (widthScale < heightScale && target.width > source.width)
    {
      //fit by width
      fitted.height *= widthScale;
      fitted.width  *= widthScale;
    }
    else if (target.height > source.height)
    {
      //fit by height
      fitted.width  *= heightScale;
      fitted.height *= heightScale;
    }
    // else requested dimensions don't need to be fitted
  }
  return fitted;
}

Size ShrinkInside( const Size& target, const Size& source )
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
  // use smaller of the scales
  float scale = std::min( widthScale, heightScale );

  // check if we need to scale
  if( scale < 1.0f )
  {
    // scale natural size to fit inside
    return source * scale;
  }
  // there is enough space so use source size
  return source;
}

} // namespace Dali
