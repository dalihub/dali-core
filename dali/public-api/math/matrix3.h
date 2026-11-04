#ifndef DALI_MATRIX3_H
#define DALI_MATRIX3_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/public-api/common/type-traits.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/vector3.h>

namespace DALI_NAMESPACE
{
/**
 * @addtogroup dali_core_math
 * @{
 */

class Matrix;
struct Vector2;

/**
 * @brief A 3x3 matrix.
 *
 * The matrix is stored as a flat array and is Column Major, i.e. the storage order is as follows (numbers represent
 * indices of array):
 *
 * @code
 *
 * 0  3  6
 * 1  4  7
 * 2  5  8
 *
 * @endcode
 *
 * Each axis is contiguous in memory, so the x-axis corresponds to elements 0, 1 and 2, the y-axis corresponds to
 * elements 3, 4 and 5, and the z-axis corresponds to elements 6, 7 and 8.
 *
 * @SINCE_1_0.0
 */
class DALI_CORE_API Matrix3
{
public:
  /**
   * @brief The identity matrix.
   */
  static const Matrix3 IDENTITY;

  /**
   * @brief Constructor.
   * @SINCE_1_0.0
   */
  constexpr Matrix3()
  : mElements{}
  {
  }

  /**
   * @brief Copy Constructor.
   *
   * @SINCE_1_0.0
   * @param[in] m A reference to the copied 3x3 matrix
   */
  constexpr Matrix3(const Matrix3& m) = default;

  /**
   * @brief Constructor.
   *
   * @SINCE_1_0.0
   * @param[in] m A 4x4 matrix. The translation and shear components are ignored
   */
  constexpr Matrix3(const Matrix& m)
  : mElements{m.AsFloat()[0], m.AsFloat()[1], m.AsFloat()[2], m.AsFloat()[4], m.AsFloat()[5], m.AsFloat()[6], m.AsFloat()[8], m.AsFloat()[9], m.AsFloat()[10]}
  {
  }

  /**
   * @brief Constructor.
   *
   * @SINCE_1_0.0
   * @param[in] s00 First element
   * @param[in] s01 Second element
   * @param[in] s02 Third element
   * @param[in] s10 Fourth element
   * @param[in] s11 Fifth element
   * @param[in] s12 Sixth element
   * @param[in] s20 Seventh element
   * @param[in] s21 Eighth element
   * @param[in] s22 Ninth element
   */
  constexpr Matrix3(float s00, float s01, float s02, float s10, float s11, float s12, float s20, float s21, float s22)
  : mElements{s00, s01, s02, s10, s11, s12, s20, s21, s22}
  {
  }

  /**
   * @brief Move constructor.
   *
   * @SINCE_1_9.21
   * @param[in] matrix A reference to the moved matrix
   */
  constexpr Matrix3(Matrix3&& matrix) noexcept = default;

  /**
   * @brief Move assignment operator.
   *
   * @SINCE_1_9.21
   * @param[in] matrix A reference to the moved matrix
   * @return A reference to this
   */
  constexpr Matrix3& operator=(Matrix3&& matrix) noexcept = default;

  /**
   * @brief Assignment Operator.
   * @SINCE_1_0.0
   * @param[in] matrix From which to copy values
   * @return Reference to this object
   */
  constexpr Matrix3& operator=(const Matrix3& matrix) = default;

  /**
   * @brief Assignment Operator.
   * @SINCE_1_0.0
   * @param[in] matrix A reference to the copied matrix
   * @return A reference to this
   */
  constexpr Matrix3& operator=(const Matrix& matrix)
  {
    for(unsigned int column = 0; column < 3; ++column)
    {
      for(unsigned int row = 0; row < 3; ++row)
      {
        mElements[column * 3 + row] = matrix.AsFloat()[column * 4 + row];
      }
    }
    return *this;
  }

  /**
   * @brief The equality operator.
   *
   * Utilizes appropriate machine epsilon values.
   *
   * @SINCE_1_0.0
   * @param[in] rhs The Matrix to compare this to
   * @return True if the matrices are equal
   */
  constexpr bool operator==(const Matrix3& rhs) const
  {
    return (
      Equals(mElements[0], rhs.mElements[0]) &&
      Equals(mElements[1], rhs.mElements[1]) &&
      Equals(mElements[2], rhs.mElements[2]) &&
      Equals(mElements[3], rhs.mElements[3]) &&
      Equals(mElements[4], rhs.mElements[4]) &&
      Equals(mElements[5], rhs.mElements[5]) &&
      Equals(mElements[6], rhs.mElements[6]) &&
      Equals(mElements[7], rhs.mElements[7]) &&
      Equals(mElements[8], rhs.mElements[8]));
  }

  /**
   * @brief The inequality operator.
   *
   * Utilizes appropriate machine epsilon values.
   *
   * @SINCE_1_0.0
   * @param[in] rhs The Matrix to compare this to
   * @return true if the matrices are equal
   */
  constexpr bool operator!=(const Matrix3& rhs) const
  {
    return !(*this == rhs);
  }

  /**
   * @brief Destructor.
   * @SINCE_1_0.0
   */
  ~Matrix3() = default;

  /**
   * @brief Sets the matrix to the identity matrix.
   * @SINCE_1_0.0
   */
  constexpr void SetIdentity()
  {
    for(unsigned int index = 0; index < 9; ++index)
    {
      mElements[index] = (index % 4 == 0) ? 1.f : 0.f;
    }
  }

  /**
   * @brief Returns the contents of the matrix as an array of 9 floats.
   *
   * The order of the values for a matrix is:
   *
   * @code
   * [ xAxis.x, xAxis.y, xAxis.z, yAxis.x, yAxis.y, yAxis.z, zAxis.x, zAxis.y, zAxis.z ]
   * @endcode
   *
   * @SINCE_1_0.0
   * @return The matrix contents as an array of 9 floats
   */
  constexpr const float* AsFloat() const
  {
    return &mElements[0];
  }

  /**
   * @brief Returns the contents of the matrix as an array of 9 floats.
   *
   * The order of the values for a matrix is:
   *
   * @code
   * [ xAxis.x, xAxis.y, xAxis.z, yAxis.x, yAxis.y, yAxis.z, zAxis.x, zAxis.y, zAxis.z ]
   * @endcode
   *
   * @SINCE_1_0.0
   * @return The matrix contents as an array of 9 floats
   */
  constexpr float* AsFloat()
  {
    return &mElements[0];
  }

  /**
   * @brief Inverts the matrix.
   *
   * @SINCE_1_0.0
   * @return True if successful
   */
  bool Invert();

  /**
   * @brief Swaps the rows to columns.
   * @SINCE_1_0.0
   * @return True if successful
   */
  constexpr bool Transpose()
  {
    float tmp    = 0.f;
    tmp          = mElements[1];
    mElements[1] = mElements[3];
    mElements[3] = tmp;
    tmp          = mElements[2];
    mElements[2] = mElements[6];
    mElements[6] = tmp;
    tmp          = mElements[7];
    mElements[7] = mElements[5];
    mElements[5] = tmp;
    return true;
  }

  /**
   * @brief Multiplies all elements of the matrix by the scale value.
   *
   * @SINCE_1_0.0
   * @param[in] scale The value by which to scale the whole matrix
   */
  constexpr void Scale(float scale)
  {
    mElements[0] *= scale;
    mElements[1] *= scale;
    mElements[2] *= scale;
    mElements[3] *= scale;
    mElements[4] *= scale;
    mElements[5] *= scale;
    mElements[6] *= scale;
    mElements[7] *= scale;
    mElements[8] *= scale;
  }

  /**
   * @brief Magnitude returns the average of the absolute values of the
   * elements * 3.
   *
   * (The Magnitude of the unit matrix is therefore 1)
   * @SINCE_1_0.0
   * @return The magnitude - always positive
   */
  constexpr float Magnitude() const
  {
    float avg = 0;
    for(uint32_t i = 0; i < 9; i++)
    {
      avg += Internal::MathUtils::Abs(mElements[i]);
    }
    return avg / 3.0f;
  }

  /**
   * @brief If the matrix is invertible, then this method inverts, transposes
   * and scales the matrix such that the resultant element values
   * average 1.
   *
   * If the matrix is not invertible, then the matrix is left unchanged.
   *
   * @SINCE_1_0.0
   * @return @c true if the matrix is invertible, otherwise @c false
   */
  bool ScaledInverseTranspose();

  /**
   * @brief Function to multiply two matrices and store the result onto third.
   *
   * Use this method in time critical path as it does not require temporaries
   *
   * result = rhs * lhs
   *
   * @SINCE_1_0.0
   * @param[out] result Result of the multiplication
   * @param[in] lhs Matrix, this can be same matrix as result
   * @param[in] rhs Matrix, this cannot be same matrix as result
   */
  static void Multiply(Matrix3& result, const Matrix3& lhs, const Matrix3& rhs);

  /**
   * @brief Multiplication operator.
   *
   * Returned Matrix = This Matrix * rhs
   *
   * @note It makes some memory allocate & copy internally.
   * Use Matrix3::Multiply API for time critical path.
   *
   * @SINCE_2_1.44
   * @param[in] rhs The Matrix to multiply this by
   * @return A Matrix containing the result
   */
  Matrix3 operator*(const Matrix3& rhs) const;

  /**
   * @brief Multiplication assignment operator.
   *
   * This Matrix *= rhs
   *
   * @note It makes some memory allocate & copy internally.
   *
   * @SINCE_2_1.46
   * @param[in] rhs The Matrix to multiply this by
   * @return Itself
   */
  Matrix3& operator*=(const Matrix3& rhs);

  /**
   * @brief The multiplication operator.
   *
   * Returned Vector = This Matrix * rhs
   *
   * @SINCE_2_1.34
   * @param[in] rhs The Vector3 to multiply this by
   * @return A Vector3 containing the result
   */
  constexpr Vector3 operator*(const Vector3& rhs) const
  {
    return Vector3(rhs.x * mElements[0] + rhs.y * mElements[3] + rhs.z * mElements[6],
                   rhs.x * mElements[1] + rhs.y * mElements[4] + rhs.z * mElements[7],
                   rhs.x * mElements[2] + rhs.y * mElements[5] + rhs.z * mElements[8]);
  }

private:
  float mElements[9]; ///< The elements of the matrix
};

// Allow Matrix3 to be treated as a POD type
template<>
struct TypeTraits<Matrix3> : public BasicTypes<Matrix3>
{
  enum
  {
    IS_TRIVIAL_TYPE = true
  };
};

/**
 * @}
 */
} //namespace DALI_NAMESPACE

#endif //DALI_MATRIX3_H
