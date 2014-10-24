#ifndef __DALI_VECTOR_4_H__
#define __DALI_VECTOR_4_H__

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

// EXTERNAL INCLUDES
#include <algorithm>
#include <iostream>
#include <math.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{
struct Vector2;
struct Vector3;

/**
 * @brief A four dimensional vector.
 *
 * Components can be used as position or offset (x,y,z,w); color (r,g,b,a) or texture coords(s,t,p,q)
 */
struct DALI_IMPORT_API Vector4
{
// NOTE
// xrs, ygt, zbp and waq must be consecutive in memory.
// No other data must be added before xrs member.
// No virtual methods must be added to this struct.

// Construction

  /**
   * @brief Default constructor, initializes the vector to 0.
   *
   */
  Vector4()
  : x(0.0f),
    y(0.0f),
    z(0.0f),
    w(0.0f)
  {
  }

  /**
   * @brief Conversion constructor from four floats.
   *
   * @param [in] x (or r/s) component
   * @param [in] y (or g/t) component
   * @param [in] z (or b/p) component
   * @param [in] w (or a/q) component
   */
  explicit Vector4(float x, float y, float z, float w)
  : x(x),
    y(y),
    z(z),
    w(w)
  {
  }


  /**
   * @brief Conversion constructor from an array of four floats.
   *
   * @param [in] array of either xyzw/rgba/stpq
   */
  explicit Vector4(const float* array)
  : x(array[0]),
    y(array[1]),
    z(array[2]),
    w(array[3])
  {
  }

  /**
   * @brief Conversion constructor from Vector2.
   *
   * @param [in] vec2 to copy from, z and w are initialized to 0
   */
  explicit Vector4( const Vector2& vec2 );

  /**
   * @brief Conversion constructor from Vector3.
   *
   * @param [in] vec3 to copy from, w is initialized to 0
   */
  explicit Vector4( const Vector3& vec3 );

// Constants
  static const Vector4 ONE;   ///< (1.0f,1.0f,1.0f,1.0f)
  static const Vector4 XAXIS; ///< (1.0f,0.0f,0.0f,0.0f)
  static const Vector4 YAXIS; ///< (0.0f,1.0f,0.0f,0.0f)
  static const Vector4 ZAXIS; ///< (0.0f,0.0f,1.0f,0.0f)
  static const Vector4 ZERO;  ///< (0.0f, 0.0f, 0.0f, 0.0f)

// API

  /**
   * @brief Assignment operator.
   *
   * @param [in] array of floats
   * @return itself
   */
  Vector4& operator=(const float* array)
  {
    x = array[0];
    y = array[1];
    z = array[2];
    w = array[3];

    return *this;
  }

  /**
   * @brief Assignment operator.
   *
   * Only sets x and y. z and w are left as they were
   * @param [in] vec2 to assign from.
   * @return itself
   */
  Vector4& operator=(const Vector2& vec2 );

  /**
   * @brief Assignment operator.
   *
   * Only sets x and y and z. w is left as it was
   * @param [in] vec3 to assign from
   * @return itself
   */
  Vector4& operator=(const Vector3& vec3 );

  /**
   * @brief Addition operator.
   *
   * @param[in] rhs vector to add.
   * @return A vector containing the result of the addition
   */
  Vector4 operator+(const Vector4 & rhs) const
  {
    Vector4 temp(*this);

    return temp += rhs;
  }

  /**
   * @brief Addition assignment operator.
   *
   * @param[in] rhs vector to add.
   * @return itself
   */
  Vector4& operator+=(const Vector4& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    w += rhs.w;

    return *this;
  }

  /**
   * @brief Subtraction operator.
   *
   * @param[in] rhs  the vector to subtract
   * @return A vector containing the result of the subtraction
   */
  Vector4 operator-(const Vector4& rhs) const
  {
    Vector4 temp(*this);

    temp-=rhs;

    return temp;
  }

  /**
   * @brief Subtraction assignment operator.
   *
   * @param[in] rhs the vector to subtract
   * @return itself
   */
  Vector4& operator-=(const Vector4& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    w -= rhs.w;

    return *this;
  }

  /**
   * @brief Multiplication operator.
   *
   * @param[in] rhs the vector to multiply
   * @return A vector containing the result of the multiplication
   */
  Vector4 operator*(const Vector4& rhs) const
  {
    Vector4 temp(*this);

    return temp *= rhs;
  }

  /**
   * @brief Multiplication operator.
   *
   * @param[in] rhs the float value to scale the vector
   * @return A vector containing the result of the scaling
   */
  Vector4 operator*(float rhs) const
  {
    return Vector4(x * rhs, y * rhs, z * rhs, w * rhs);
  }

  /**
   * @brief Multiplication assignment operator.
   *
   * @param[in] rhs the vector to multiply
   * @return itself
   */
  Vector4& operator*=(const Vector4& rhs)
  {
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    w *= rhs.w;

    return *this;
  }

  /**
   * @brief Multiplication assignment operator.
   *
   * @param[in] rhs the float value to scale the vector
   * @return itself
   */
  Vector4& operator*=(float rhs)
  {
    x *= rhs;
    y *= rhs;
    z *= rhs;
    w *= rhs;

    return *this;
  }

  /**
   * @brief Division operator.
   *
   * @param[in] rhs the vector to divide
   * @return A vector containing the result of the division
   */
  Vector4 operator/(const Vector4 & rhs) const
  {
    Vector4 temp(*this);

    return temp /= rhs;
  }

  /**
   * @brief Division operator.
   *
   * @param[in] rhs The float value to scale the vector by
   * @return A vector containing the result of the scaling
   */
  Vector4 operator/(float rhs) const
  {
    float oneOver = 1.0f / rhs;
    return Vector4(x * oneOver, y * oneOver, z * oneOver, w * oneOver);
  }

  /**
   * @brief Division assignment operator.
   *
   * @param[in] rhs the vector to divide
   * @return itself
   */
  Vector4& operator/=(const Vector4& rhs)
  {
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    w /= rhs.w;

    return *this;
  }

  /**
   * @brief Division assignment operator.
   *
   * @param[in] rhs the float value to scale the vector by
   * @return itself
   */
  Vector4& operator/=(float rhs)
  {
    const float oneOver = 1.0f / rhs;
    x *= oneOver;
    y *= oneOver;
    z *= oneOver;
    w *= oneOver;

    return *this;
  }

  /**
   * @brief Unary negation operator.
   *
   * @return the negative value
   */
  Vector4 operator-() const
  {
    Vector4 temp(-x, -y, -z, -w);

    return temp;
  }

  /**
   * @brief Equality operator.
   *
   * Utilises appropriate machine epsilon values.
   *
   * @param[in] rhs The vector to test against
   * @return true if the vectors are equal
   */
  bool operator==(const Vector4 &rhs) const;

  /**
   * @brief Inequality operator.
   *
   * Utilises appropriate machine epsilon values.
   *
   * @param[in] rhs The vector to test against
   * @return true if the vectors are not equal
   */
  bool operator!=(const Vector4 &rhs) const
  {
    return !(*this == rhs);
  }

  /**
   * @brief Const array subscript operator overload.
   *
   * Asserts if index is out of range. Should be 0, 1, 2 or 3
   * @param[in] index Subscript
   * @return    The float at the given index
   */
  const float& operator[](const unsigned int index) const
  {
    DALI_ASSERT_ALWAYS( index < 4 && "Vector element index out of bounds" );

    return AsFloat()[index];
  }

  /**
   * @brief Mutable array subscript operator overload.
   *
   * Asserts if index is out of range. Should be 0, 1, 2 or 3
   * @param[in] index Subscript index
   * @return    The float at the given index
   */
  float& operator[](const unsigned int index)
  {
    DALI_ASSERT_ALWAYS( index < 4 && "Vector element index out of bounds" );

    return AsFloat()[index];
  }

  /**
   * @brief Returns the dot product of this vector (4d) and another vector (3d).
   *
   * The dot product is the length of one vector in the direction of another vector.
   * This is great for lighting, threshold testing the angle between two unit vectors,
   * calculating the distance between two points in a particular direction.
   * @param [in] other     the other vector
   * @return               the dot product
   */
  float Dot(const Vector3& other) const;

  /**
   * @brief Returns the dot product of this vector and another vector.
   *
   * The dot product is the length of one vector in the direction of another vector.
   * This is great for lighting, threshold testing the angle between two unit vectors,
   * calculating the distance between two points in a particular direction.
   * @param [in] other     the other vector
   * @return               the dot product
   */
  float Dot(const Vector4& other) const;

  /**
   * @brief Returns the 4d dot product of this vector and another vector.
   *
   * @param [in] other     the other vector
   * @return          the dot product
   */
  float Dot4(const Vector4& other) const;

  /**
   * @brief Returns the cross produce of this vector and another vector.
   *
   * The cross produce of two vectors is a vector which is perpendicular to the plane of the
   * two vectors. This is great for calculating normals and making matrices orthogonal.
   *
   * @param [in] other    the other vector
   * @return A vector containing the cross product
   */
  Vector4 Cross(const Vector4& other) const;

  /**
   * @brief Returns the length of the vector.
   *
   * @return the length.
   */
  float Length() const;

  /**
   * @brief Returns the length of the vector squared.
   *
   * This is faster than using Length() when performing
   * threshold checks as it avoids use of the square root.
   * @return the length of the vector squared.
   */
  float LengthSquared() const;

  /**
   * @brief Normalizes the vector.
   *
   * Sets the vector to unit length whilst maintaining its direction.
   */
  void Normalize();

  /**
   * @brief Clamps the vector between minimum and maximum vectors.
   *
   * @param [in] min the minimum vector
   * @param [in] max the maximum vector
   */
  void Clamp( const Vector4& min, const Vector4& max );

  /**
   * @brief Returns the contents of the vector as an array of 4 floats.
   *
   * The order of the values in this array are as follows:
   * 0: x (or r, or s)
   * 1: y (or g, or t)
   * 2: z (or b, or p)
   * 3: w (or a, or q)
   * @note inlined for performance reasons (generates less code than a function call)
   * @return the vector contents as an array of 4 floats.
   */
  const float* AsFloat() const {return &x;}

  /**
   * @brief Returns the contents of the vector as an array of 4 floats.
   *
   * The order of the values in this array are as follows:
   * 0: x (or r, or s)
   * 1: y (or g, or t)
   * 2: z (or b, or p)
   * 3: w (or a, or q)
   * @note inlined for performance reasons (generates less code than a function call)
   * @return the vector contents as an array of 4 floats.
   */
  float* AsFloat() {return &x;}

// Data

  // NOTE
  // xrs, ygt, zbp and waq must be consecutive in memory.
  // No other data must be added before xrs member.
  // No virtual methods must be added to this struct.

  union
  {
    float x; ///< x component
    float r; ///< red component
    float s; ///< s component
  };
  union
  {
    float y; ///< y component
    float g; ///< green component
    float t; ///< t component
  };
  union
  {
    float z; ///< z component
    float b; ///< blue component
    float p; ///< p component
  };
  union
  {
    float w; ///< w component
    float a; ///< alpha component
    float q; ///< q component
  };
};

/**
 * @brief Print a Vector4.
 *
 * @param [in] o The output stream operator.
 * @param [in] vector The vector to print.
 * @return The output stream operator.
 */
DALI_IMPORT_API std::ostream& operator<<(std::ostream& o, const Vector4& vector);

/**
 * @brief Returns a vector with components set to the minimum of the corresponding component in a and b.
 *
 * If a=0,1,2,3 and b=4,0,1,2  returns a vector of 0,0,1,2
 * @param [in] a     a vector
 * @param [in] b     a vector
 * @return      a vector containing the minimum of each component from a and b
 */
inline Vector4 Min( const Vector4& a, const Vector4& b )
{
  return Vector4( std::min(a.x,b.x), std::min(a.y,b.y), std::min(a.z,b.z), std::min(a.w,b.w) );
}

/**
 * @brief Returns a vector with components set to the maximum of the corresponding component in a and b.
 *
 * If a=0,1,2,3 and b=4,0,1,2  returns a vector of 4,1,2,3
 * @param [in] a     a vector
 * @param [in] b     a vector
 * @return      a vector containing the maximum of each component from a and b
 */
inline Vector4 Max( const Vector4& a, const Vector4& b )
{
  return Vector4( std::max(a.x,b.x), std::max(a.y,b.y), std::max(a.z,b.z), std::max(a.w,b.w) );
}

/**
 * @brief Clamps each of vector v's components between minimum and maximum values.
 *
 * @param [in] v     a vector
 * @param [in] min the minimum value
 * @param [in] max the maximum value
 * @return     a vector containing the clamped components of v
 */
DALI_IMPORT_API Vector4 Clamp( const Vector4& v, const float& min, const float& max );

} // namespace Dali

#endif // __DALI_VECTOR_4_H__
