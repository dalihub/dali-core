#ifndef __DALI_VECTOR_3_H__
#define __DALI_VECTOR_3_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <iosfwd>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/type-traits.h>

namespace Dali
{
/**
 * @addtogroup dali_core_math
 * @{
 */

struct Vector2;
struct Vector4;
class Quaternion;

/**
 * @brief A three dimensional vector.
 * @SINCE_1_0.0
 */
struct DALI_IMPORT_API Vector3
{
// Construction

  /**
   * @brief Constructor.
   * @SINCE_1_0.0
   */
  // NOTE
  // (x width r), (y height g), (z depth b) must be consecutive in memory.
  // No other data must be added before (x width r) member.
  // No virtual methods must be added to this struct.
  Vector3()
  : x(0.0f),
    y(0.0f),
    z(0.0f)
  {
  }

  /**
   * @brief Constructor.
   *
   * @SINCE_1_0.0
   * @param [in] x (or width) component
   * @param [in] y (or height) component
   * @param [in] z (or depth) component
   */
  explicit Vector3(float x, float y, float z)
  : x(x),
    y(y),
    z(z)
  {
  }

  /**
   * @brief Conversion constructor from an array of three floats.
   *
   * @SINCE_1_0.0
   * @param [in] array of xyz
   */
  explicit Vector3(const float* array)
  : x(array[0]),
    y(array[1]),
    z(array[2])
  {
  }

  /**
   * @brief Constructor.
   *
   * @SINCE_1_0.0
   * @param [in] vec2 Vector2 to create this vector from
   */
  explicit Vector3( const Vector2& vec2 );

  /**
   * @brief Constructor.
   *
   * @SINCE_1_0.0
   * @param [in] vec4 Vector4 to create this vector from
   */
  explicit Vector3( const Vector4& vec4 );

// Constants

  static const Vector3 ONE;               ///< (1.0f,1.0f,1.0f)
  static const Vector3 XAXIS;             ///< Vector representing the X axis
  static const Vector3 YAXIS;             ///< Vector representing the Y axis
  static const Vector3 ZAXIS;             ///< Vector representing the Z axis
  static const Vector3 NEGATIVE_XAXIS;    ///< Vector representing the negative X axis
  static const Vector3 NEGATIVE_YAXIS;    ///< Vector representing the negative Y axis
  static const Vector3 NEGATIVE_ZAXIS;    ///< Vector representing the negative Z axis
  static const Vector3 ZERO;              ///< (0.0f, 0.0f, 0.0f)

// API

  /**
   * @brief Assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] array of floats
   * @return itself
   */
  Vector3& operator=(const float* array)
  {
    x = array[0];
    y = array[1];
    z = array[2];

    return *this;
  }

  /**
   * @brief Assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] rhs vector to assign.
   * @return itself
   */
  Vector3& operator=(const Vector2& rhs);

  /**
   * @brief Assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] rhs vector to assign.
   * @return itself
   */
  Vector3& operator=(const Vector4& rhs);

  /**
   * @brief Addition operator.
   *
   * @SINCE_1_0.0
   * @param[in] rhs vector to add.
   * @return A vector containing the result of the addition
   */
  Vector3 operator+(const Vector3& rhs) const
  {
    Vector3 temp(*this);

    return temp += rhs;
  }

  /**
   * @brief Addition assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] rhs vector to add.
   * @return itself
   */
  Vector3& operator+=(const Vector3& rhs)
  {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;

    return *this;
  }

  /**
   * @brief Subtraction operator.
   *
   * @SINCE_1_0.0
   * @param[in] rhs  the vector to subtract
   * @return A vector containing the result of the subtraction
   */
  Vector3 operator-(const Vector3& rhs) const
  {
    Vector3 temp(*this);

    return temp -= rhs;
  }

  /**
   * @brief Subtraction assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] rhs the vector to subtract
   * @return itself
   */
  Vector3& operator-=(const Vector3& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;

    return *this;
  }

  /**
   * @brief Multiplication operator.
   *
   * @SINCE_1_0.0
   * @param[in] rhs the vector to multiply
   * @return A vector containing the result of the multiplication
   */
  Vector3 operator*(const Vector3& rhs) const
  {
    Vector3 temp(*this);

    return temp *= rhs;
  }

  /**
   * @brief Multiplication operator.
   *
   * @SINCE_1_0.0
   * @param[in] rhs the float value to scale the vector
   * @return A vector containing the result of the scaling
   */
  Vector3 operator*(float rhs) const
  {
    return Vector3(x * rhs, y * rhs, z * rhs);
  }

  /**
   * @brief Multiplication assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] rhs the vector to multiply
   * @return itself
   */
  Vector3& operator*=(const Vector3& rhs)
  {
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;

    return *this;
  }

  /**
   * @brief Multiplication assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] rhs the float value to scale the vector
   * @return itself
   */
  Vector3& operator*=(float rhs)
  {
    x *= rhs;
    y *= rhs;
    z *= rhs;

    return *this;
  }

  /**
   * @brief Multiplication assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] rhs the Quaternion value to multiply the vector by
   * @return itself
   */
  Vector3& operator*=(const Quaternion& rhs);

  /**
   * @brief Division operator.
   *
   * @SINCE_1_0.0
   * @param[in] rhs the vector to divide
   * @return A vector containing the result of the division
   */
  Vector3 operator/(const Vector3& rhs) const
  {
    Vector3 temp(*this);

    return temp /= rhs;
  }

  /**
   * @brief Division operator.
   *
   * @SINCE_1_0.0
   * @param[in] rhs The float value to scale the vector by
   * @return A vector containing the result of the scaling
   */
  Vector3 operator/(float rhs) const
  {
    return Vector3(x / rhs, y / rhs, z / rhs);
  }

  /**
   * @brief Division assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] rhs the vector to divide
   * @return itself
   */
  Vector3& operator/=(const Vector3& rhs)
  {
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;

    return *this;
  }

  /**
   * @brief Division assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] rhs the float value to scale the vector by
   * @return itself
   */
  Vector3& operator/=(float rhs)
  {
    float oneOverRhs = 1.0f / rhs;
    x *= oneOverRhs;
    y *= oneOverRhs;
    z *= oneOverRhs;

    return *this;
  }

  /**
   * @brief Unary negation operator.
   *
   * @SINCE_1_0.0
   * @return A vector containg the negation
   */
  Vector3 operator-() const
  {
    Vector3 temp(-x, -y, -z);

    return temp;
  }

  /**
   * @brief Equality operator.
   *
   * utilises appropriate machine epsilon values;
   *
   * @SINCE_1_0.0
   * @param[in] rhs The vector to test against
   * @return true if the vectors are equal
   */
  bool operator==(const Vector3& rhs) const;

  /**
   * @brief Inequality operator.
   *
   * utilises appropriate machine epsilon values;
   *
   * @SINCE_1_0.0
   * @param[in] rhs The vector to test against
   * @return true if the vectors are not equal
   */
  bool operator!=(const Vector3& rhs) const
  {
    return !(*this == rhs);
  }

  /**
   * @brief Const array subscript operator overload.
   *
   * Asserts if index is out of range. Should be 0, 1 or 2
   * @SINCE_1_0.0
   * @param[in] index Subscript
   * @return    The float at the given index.
   */
  const float& operator[](const unsigned int index) const
  {
    DALI_ASSERT_ALWAYS( index < 3 && "Vector element index out of bounds" );

    return AsFloat()[index];
  }

  /**
   * @brief Mutable array subscript operator overload.
   *
   * Asserts if index is out of range. Should be 0, 1 or 2
   * @SINCE_1_0.0
   * @param[in] index Subscript index
   * @return    The float at the given index.
   */
  float& operator[](const unsigned int index)
  {
    DALI_ASSERT_ALWAYS( index < 3 && "Vector element index out of bounds" );

    return AsFloat()[index];
  }

  /**
   * @brief Returns the dot product of this vector and another vector.
   *
   * The dot product is the length of one vector in the direction of another vector.
   * This is great for lighting, threshold testing the angle between two unit vectors,
   * calculating the distance between two points in a particular direction.
   * @SINCE_1_0.0
   * @param [in]  other     the other vector
   * @return          the dot product
   */
  float Dot(const Vector3& other) const;

  /**
   * @brief Returns the cross produce of this vector and another vector.
   *
   * The cross produce of two vectors is a vector which is perpendicular to the plane of the
   * two vectors. This is great for calculating normals and making matrices orthogonal.
   *
   * @SINCE_1_0.0
   * @param [in] other    the other vector
   * @return         the cross product
   */
  Vector3 Cross(const Vector3& other) const;

  /**
   * @brief Returns the length of the vector.
   *
   * @SINCE_1_0.0
   * @return the length of the vector
   */
  float Length() const;

  /**
   * @brief Returns the length of the vector squared.
   *
   * This is more efficient than Length() for threshold
   * testing as it avoids the use of a square root.
   * @SINCE_1_0.0
   * @return the length of the vector squared.
   */
  float LengthSquared() const;

  /**
   * @brief Sets the vector to be unit length, whilst maintaining its direction.
   *
   * @SINCE_1_0.0
   */
  void Normalize();

  /**
   * @brief Clamps the vector between minimum and maximum vectors.
   *
   * @SINCE_1_0.0
   * @param [in] min the minimum vector
   * @param [in] max the maximum vector
   */
  void Clamp( const Vector3& min, const Vector3& max );

  /**
   * @brief Returns the contents of the vector as an array of 3 floats.
   *
   * The order of the values in this array are as follows:
   * 0: x (or width, or r)
   * 1: y (or height, or g)
   * 2: z (or depth, or b)
   * @SINCE_1_0.0
   * @return the vector contents as an array of 3 floats.
   * @note inlined for performance reasons (generates less code than a function call)
   */
  const float* AsFloat() const {return &x;}

  /**
   * @brief Returns the contents of the vector as an array of 3 floats.
   *
   * The order of the values in this array are as follows:
   * 0: x (or width, or r)
   * 1: y (or height, or g)
   * 2: z (or depth, or b)
   * @SINCE_1_0.0
   * @return the vector contents as an array of 3 floats.
   * @note inlined for performance reasons (generates less code than a function call)
   */
  float* AsFloat() {return &x;}

  /**
   * @brief Returns the x & y components (or width & height, or r & g) as a Vector2.
   *
   * @SINCE_1_0.0
   * @return the partial vector contents as Vector2 (x,y)
   * @note inlined for performance reasons (generates less code than a function call)
   */
  const Vector2& GetVectorXY() const {return reinterpret_cast<const Vector2&>(x);}

  /**
   * @brief Returns the x & y components (or width & height, or r & g) as a Vector2.
   *
   * @SINCE_1_0.0
   * @return the partial vector contents as Vector2 (x,y)
   * @note inlined for performance reasons (generates less code than a function call)
   */
  Vector2& GetVectorXY() {return reinterpret_cast<Vector2&>(x);}

  /**
   * @brief Returns the y & z components (or height & depth, or g & b) as a Vector2.
   *
   * @SINCE_1_0.0
   * @return the partial vector contents as Vector2 (y,z)
   * @note inlined for performance reasons (generates less code than a function call)
   */
  const Vector2& GetVectorYZ() const {return reinterpret_cast<const Vector2&>(y);}

  /**
   * @brief Returns the y & z components (or height & depth, or g & b) as a Vector2.
   *
   * @SINCE_1_0.0
   * @return the partial vector contents as Vector2 (y,z)
   * @note inlined for performance reasons (generates less code than a function call)
   */
  Vector2& GetVectorYZ() {return reinterpret_cast<Vector2&>(y);}

// Data

  // NOTE
  // (x width r), (y height g), (z depth b) must be consecutive in memory.
  // No other data must be added before (x width r) member.
  // No virtual methods must be added to this struct.
  union
  {
    float x;      ///< x component
    float width;  ///< width component
    float r;      ///< red component
  };
  union
  {
    float y;      ///< y component
    float height; ///< height component
    float g;      ///< green component
  };
  union
  {
    float z;      ///< z component
    float depth;  ///< depth component
    float b;      ///< blue component
  };
};

/**
 * @brief Print a Vector3.
 *
 * @SINCE_1_0.0
 * @param [in] o The output stream operator.
 * @param [in] vector The vector to print.
 * @return The output stream operator.
 */
DALI_IMPORT_API std::ostream& operator<< (std::ostream& o, const Vector3& vector);

/**
 * @brief Returns a vector with components set to the minimum of the corresponding component in a and b.
 *
 * If a=0,1,2 and b=2,1,0  returns a vector of 2,1,2.
 * @SINCE_1_0.0
 * @param [in] a     a vector
 * @param [in] b     a vector
 * @return      a vector containing the minimum of each component from a and b
 */
inline Vector3 Min( const Vector3& a, const Vector3& b )
{
  return Vector3( a.x < b.x ? a.x : b.x ,
                  a.y < b.y ? a.y : b.y,
                  a.z < b.z ? a.z : b.z );
}

/**
 * @brief Returns a vector with components set to the maximum of the corresponding component in a and b.
 *
 * If a=0,1 and b=1,0  returns a vector of 1,1
 * @SINCE_1_0.0
 * @param [in] a     a vector
 * @param [in] b     a vector
 * @return      a vector containing the maximum of each component from a and b
 */
inline Vector3 Max( const Vector3& a, const Vector3& b )
{
  return Vector3( a.x > b.x ? a.x : b.x,
                  a.y > b.y ? a.y : b.y,
                  a.z > b.z ? a.z : b.z );
}

/**
 * @brief Clamps each of vector v's components between minimum and maximum values.
 *
 * @SINCE_1_0.0
 * @param [in] v     a vector
 * @param [in] min the minimum value
 * @param [in] max the maximum value
 * @return     a vector containing the clamped components of v
 */
DALI_IMPORT_API Vector3 Clamp( const Vector3& v, const float& min, const float& max );

// Allow Vector3 to be treated as a POD type
template <> struct TypeTraits< Vector3 > : public BasicTypes< Vector3 > { enum { IS_TRIVIAL_TYPE = true }; };

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_VECTOR_3_H__
