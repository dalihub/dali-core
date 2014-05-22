#ifndef __DALI_VECTOR_2_H__
#define __DALI_VECTOR_2_H__

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

/**
 * @addtogroup CAPI_DALI_MATH_MODULE
 * @{
 */

// EXTERNAL INCLUDES
#include <algorithm>
#include <iostream>

// INTERNAL INCLUDES
#include <dali/public-api/math/math-utils.h>

namespace Dali DALI_IMPORT_API
{

struct Vector3;
struct Vector4;

/**
 * @brief A two dimensional vector.
 */
struct DALI_IMPORT_API Vector2
{
// (x width) and (y height) must be consecutive in memory.
// No other data must be added before (x width) member.
// No virtual methods must be added to this struct.

public:

  /**
   * @brief Constructor.
   */
  Vector2()
  : x(0.0f),
    y(0.0f)
  {
  }

  /**
   * @brief Constructor.
   *
   * @param[in] x x or width component
   * @param[in] y y or height component
   */
  explicit Vector2(float x, float y)
  : x(x), y(y)
  {
  }

  /**
   * @brief Conversion constructor from an array of two floats.
   *
   * @param [in] array of xy
   */
  explicit Vector2(const float* array)
  : x(array[0]),
    y(array[1])
  {
  }

  /**
   * @brief Constructor.
   *
   * @param [in] vec3 Vector3 to create this vector from
   */
  explicit Vector2(const Vector3& vec3);

  /**
   * @brief Constructor.
   *
   * @param [in] vec4 Vector4 to create this vector from
   */
  explicit Vector2(const Vector4& vec4);

// Constants

  static const Vector2 ONE;               ///< (1.0f,1.0f)
  static const Vector2 XAXIS;             ///< Vector representing the X axis
  static const Vector2 YAXIS;             ///< Vector representing the Y axis
  static const Vector2 NEGATIVE_XAXIS;    ///< Vector representing the negative X axis
  static const Vector2 NEGATIVE_YAXIS;    ///< Vector representing the negative Y axis
  static const Vector2 ZERO;              ///< (0.0f, 0.0f)

// API

  /**
   * @brief Assignment operator.
   *
   * @param[in] array of floats
   * @return itself
   */
  Vector2& operator=(const float* array)
  {
    x = array[0];
    y = array[1];

    return *this;
  }

  /**
   * @brief Assignment operator.
   *
   * @param[in] rhs vector to assign.
   * @return itself
   */
  Vector2& operator=(const Vector3& rhs);

  /**
   * @brief Assignment operator.
   *
   * @param[in] rhs vector to assign.
   * @return itself
   */
  Vector2& operator=(const Vector4& rhs);

  /**
   * @brief Addition operator.
   *
   * @param[in] rhs vector to add.
   * @return A vector containing the result of the addition
   */
  Vector2 operator+(const Vector2& rhs) const
  {
    Vector2 temp(*this);

    return temp += rhs;
  }

  /**
   * @brief Addition assignment operator.
   *
   * @param[in] rhs vector to add.
   * @return itself
   */
  Vector2& operator+=(const Vector2& rhs)
  {
    x += rhs.x;
    y += rhs.y;

    return *this;
  }

  /**
   * @brief Subtraction operator.
   *
   * @param[in] rhs  the vector to subtract
   * @return A vector containing the result of the subtraction
   */
  Vector2 operator-(const Vector2& rhs) const
  {
    Vector2 temp(*this);

    return temp -= rhs;
  }

  /**
   * @brief Subtraction assignment operator.
   *
   * @param[in] rhs the vector to subtract
   * @return itself
   */
  Vector2& operator-=(const Vector2& rhs)
  {
    x -= rhs.x;
    y -= rhs.y;

    return *this;
  }

  /**
   * @brief Multiplication operator.
   *
   * @param[in] rhs the vector to multiply
   * @return A vector containing the result of the multiplication
   */
  Vector2 operator*(const Vector2& rhs) const
  {
    return Vector2(x * rhs.x, y * rhs.y);
  }

  /**
   * @brief Multiplication operator.
   *
   * @param[in] rhs the float value to scale the vector
   * @return A vector containing the result of the scaling
   */
  Vector2 operator*(float rhs) const
  {
    return Vector2(x * rhs, y * rhs);
  }

  /**
   * @brief Multiplication assignment operator.
   *
   * @param[in] rhs the vector to multiply
   * @return itself
   */
  Vector2& operator*=(const Vector2& rhs)
  {
    x *= rhs.x;
    y *= rhs.y;

    return *this;
  }

  /**
   * @brief Multiplication assignment operator.
   *
   * @param[in] rhs the float value to scale the vector
   * @return itself
   */
  Vector2& operator*=(float rhs)
  {
    x *= rhs;
    y *= rhs;

    return *this;
  }

  /**
   * @brief Division operator.
   *
   * @param[in] rhs the vector to divide
   * @return A vector containing the result of the division
   */
  Vector2 operator/(const Vector2& rhs) const
  {
    return Vector2(x / rhs.x, y / rhs.y);
  }

  /**
   * @brief Division operator.
   *
   * @param[in] rhs The float value to scale the vector by
   * @return A vector containing the result of the scaling
   */
  Vector2 operator/(float rhs) const
  {
    return Vector2(x / rhs, y / rhs);
  }


  /**
   * @brief Division assignment operator.
   *
   * @param[in] rhs the vector to divide
   * @return itself
   */
  Vector2& operator/=(const Vector2& rhs)
  {
    x /= rhs.x;
    y /= rhs.y;

    return *this;
  }

  /**
   * @brief Division assignment operator.
   *
   * @param[in] rhs the float value to scale the vector by
   * @return itself
   */
  Vector2& operator/=(float rhs)
  {
    x /= rhs;
    y /= rhs;

    return *this;
  }

  /**
   * @brief Unary negation operator.
   *
   * @return A vector containg the negation
   */
  Vector2 operator-() const
  {
    Vector2 temp(-x, -y);

    return temp;
  }

  /**
   * @brief Equality operator.
   *
   * utilises appropriate machine epsilon values;
   *
   * @param[in] rhs The vector to test against
   * @return true if the vectors are equal
   */
  bool operator==(const Vector2& rhs) const;

  /**
   * @brief Inequality operator.
   *
   * utilises appropriate machine epsilon values;
   *
   * @param[in] rhs The vector to test against
   * @return true if the vectors are not equal
   */
  bool operator!=(const Vector2& rhs) const
  {
    return !(*this == rhs);
  }

  /**
   * @brief Const array subscript operator overload.
   *
   * Asserts if index is out of range. Should be 0 or 1
   * @param[in] index Subscript
   * @return    The float at the given index
   */
  const float& operator[](const unsigned int index) const
  {
    DALI_ASSERT_ALWAYS( index < 2 && "Vector element index out of bounds" );

    return AsFloat()[index];
  }

  /**
   * @brief Mutable array subscript operator overload.
   *
   * Asserts if index is out of range. Should be 0 or 1
   * @param[in] index Subscript index
   * @return    The float at the given index.
   */
  float& operator[](const unsigned int index)
  {
    DALI_ASSERT_ALWAYS( index < 2 && "Vector element index out of bounds" );

    return AsFloat()[index];
  }

  /**
   * @brief Returns the length of the vector.
   *
   * @return the length of the vector
   */
  float Length() const;

  /**
   * @brief Returns the length of the vector squared.
   *
   * This is more efficient than Length() for threshold
   * testing as it avoids the use of a square root.
   * @return the length of the vector squared.
   */
  float LengthSquared() const;

  /**
   * @brief Sets the vector to be unit length, whilst maintaining its direction.
   *
   */
  void Normalize();

  /**
    * @brief Clamps the vector between minimum and maximum vectors.
    *
    * @param [in] min the minimum vector
    * @param [in] max the maximum vector
   */
  void Clamp( const Vector2& min, const Vector2& max );

  /**
   * @brief Returns the contents of the vector as an array of 2 floats.
   *
   * The order of the values in this array are as follows:
   * 0: x (or width)
   * 1: y (or height)
   * @note inlined for performance reasons (generates less code than a function call)
   * @return the vector contents as an array of 2 floats.
   */
  const float* AsFloat() const {return &x;}

  /**
   * @brief Returns the contents of the vector as an array of 2 floats.
   *
   * The order of the values in this array are as follows:
   * 0: x (or width)
   * 1: y (or height)
   * @note inlined for performance reasons (generates less code than a function call)
   * @return the vector contents as an array of 2 floats.
   */
  float* AsFloat() {return &x;}

public: // Data

  // NOTE
  // (x width) and (y height) must be consecutive in memory.
  // No other data must be added before (x width) member.
  // No virtual methods must be added to this struct.
  union
  {
    float x;     ///< x component
    float width; ///< width
  };

  union
  {
    float y;      ///< y component
    float height; ///< height
  };

};

/**
 * @brief Size is an alias of Dali::Vector2
 */
typedef Vector2 Size;

/**
 * @brief Print a Vector2.
 *
 * @param [in] o The output stream operator.
 * @param [in] vector The vector to print.
 * @return The output stream operator.
 */
DALI_IMPORT_API std::ostream& operator<< (std::ostream& o, const Vector2& vector);

/**
 * @brief Returns a vector with components set to the minimum of the corresponding component in a and b.
 *
 * If a=0,1 and b=1,0  returns a vector of 0,0.
 * @param [in] a     a vector
 * @param [in] b     a vector
 * @return      a vector containing the minimum of each component from a and b
 */
inline Vector2 Min( const Vector2& a, const Vector2& b )
{
  return Vector2( std::min(a.x,b.x), std::min(a.y,b.y) );
}

/**
 * @brief Returns a vector with components set to the maximum of the corresponding component in a and b.
 *
 * If a=0,1 and b=1,0  returns a vector of 1,1
 * @param [in] a     a vector
 * @param [in] b     a vector
 * @return      a vector containing the maximum of each component from a and b
 */
inline Vector2 Max( const Vector2& a, const Vector2& b )
{
  return Vector2( std::max(a.x,b.x), std::max(a.y,b.y) );
}

/**
 * @brief Clamps each of vector v's components between minimum and maximum values.
 *
 * @param [in] v     a vector
 * @param [in] min the minimum value
 * @param [in] max the maximum value
 * @return     a vector containing the clamped components of v
 */
DALI_IMPORT_API Vector2 Clamp( const Vector2& v, const float& min, const float& max );

/**
 * @brief Fits source size inside the target size maintaining aspect ratio.
 *
 * @pre source width and height > 0
 * @param [in] target size
 * @param [in] source size
 * @return target scaled inside source
 */
DALI_IMPORT_API Size FitInside( const Size& target, const Size& source );

/**
 * @brief Fits or scales to fill.
 *
 * a) If target width and height are non-zero
 *    Fits source size into target aspect ratio
 *    If source is bigger, simply returns target.
 *    Does not scale larger than source
 * b) If target width or height is zero
 *    maintains the aspect ratio of source (as target has no aspect ratio)
 *    returns target width and scaled height or target height and scaled width
 * This algorithm is usefull when you want for example a square thumbnail of a rectangular image data
 * @param [in] target size
 * @param [in] source size
 * @return target scaled inside source
 */
DALI_IMPORT_API Size FitScaleToFill( const Size& target, const Size& source );

/**
 * @brief Shrinks source size inside the target size maintaining aspect ratio of source.
 *
 * If source is smaller than target it returns source.
 * @pre source width and height > 0
 * @param [in] target size
 * @param [in] source size
 * @return target scaled inside source
 */
DALI_IMPORT_API Size ShrinkInside( const Size& target, const Size& source );

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_VECTOR_2_H__
