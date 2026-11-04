#ifndef DALI_MATRIX_H
#define DALI_MATRIX_H

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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/type-traits.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>

namespace DALI_NAMESPACE
{
/**
 * @addtogroup dali_core_math
 * @{
 */

class Quaternion;

/**
 * @brief The Matrix class represents transformations and projections.
 *
 * The matrix is stored as a flat array and is Column Major, i.e. the storage order is as follows (numbers represent
 * indices of array):
 *
 * @code
 *
 * 0   4   8   12
 * 1   5   9   13
 * 2   6   10  14
 * 3   7   11  15
 *
 * @endcode
 *
 * Each axis is contiguous in memory, so the x-axis corresponds to elements 0, 1, 2 and 3, the y-axis corresponds to
 * elements 4, 5, 6, 7, the z-axis corresponds to elements 12, 13, 14 and 15, and the translation vector corresponds to
 * elements 12, 13 and 14.
 *
 * @SINCE_1_0.0
 */
class DALI_CORE_API Matrix
{
public:
  /**
   * @brief Constructor.
   *
   * Zero initializes the matrix.
   * @SINCE_1_0.0
   */
  constexpr Matrix()
  : mMatrix{}
  {
  }

  /**
   * @brief Constructor.
   *
   * @SINCE_1_0.0
   * @param[in] initialize True for initialization by zero or otherwise
   */
  explicit Matrix(bool initialize);

  /**
   * @brief Constructor.
   *
   * The matrix is initialized with the contents of 'array' which must contain 16 floats.
   * The order of the values for a transform matrix is:
   *
   * @code
   * [ xAxis.x, xAxis.y, xAxis.z, 0.0f, yAxis.x, yAxis.y, yAxis.z, 0.0f, zAxis.x, zAxis.y, zAxis.z, 0.0f, trans.x, trans.y, trans.z, 1.0f ]
   * @endcode
   *
   * @SINCE_1_0.0
   * @param[in] array Pointer of 16 floats data
   */
  explicit constexpr Matrix(const float* array)
  : mMatrix{}
  {
    for(unsigned int index = 0; index < 16; ++index)
    {
      mMatrix[index] = array[index];
    }
  }

  /**
   * @brief Constructs a matrix from quaternion.
   *
   * @SINCE_1_0.0
   * @param rotation Rotation as quaternion
   */
  explicit Matrix(const Quaternion& rotation);

  /**
   * @brief Copy constructor.
   *
   * @SINCE_1_0.0
   * @param[in] matrix A reference to the copied matrix
   */
  constexpr Matrix(const Matrix& matrix) = default;

  /**
   * @brief Assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] matrix A reference to the copied matrix
   * @return A reference to this
   */
  constexpr Matrix& operator=(const Matrix& matrix) = default;

  /**
   * @brief Move constructor.
   *
   * @SINCE_1_9.21
   * @param[in] matrix A reference to the moved matrix
   */
  constexpr Matrix(Matrix&& matrix) noexcept = default;

  /**
   * @brief Move assignment operator.
   *
   * @SINCE_1_9.21
   * @param[in] matrix A reference to the moved matrix
   * @return A reference to this
   */
  constexpr Matrix& operator=(Matrix&& matrix) noexcept = default;

  /**
   * @brief The identity matrix.
   */
  static const Matrix IDENTITY;

  /**
   * @brief Sets this matrix to be an identity matrix.
   * @SINCE_1_0.0
   */
  constexpr void SetIdentity()
  {
    for(unsigned int index = 0; index < 16; ++index)
    {
      mMatrix[index] = (index % 5 == 0) ? 1.f : 0.f;
    }
  }

  /**
   * @brief Sets this matrix to be an identity matrix with scale.
   *
   * @SINCE_1_0.0
   * @param[in] scale Scale to set on top of identity matrix
   */
  constexpr void SetIdentityAndScale(const Vector3& scale)
  {
    SetIdentity();
    mMatrix[0]  = scale.x;
    mMatrix[5]  = scale.y;
    mMatrix[10] = scale.z;
  }

  /**
   * @brief Inverts a transform Matrix.
   *
   * Any Matrix representing only a rotation and/or translation
   * can be inverted using this function. It is faster and more accurate then using Invert().
   * @SINCE_1_0.0
   * @param[out] result The inverse of this matrix
   */
  void InvertTransform(Matrix& result) const;

  /**
   * @brief Generic brute force Matrix Invert.
   *
   * Using the Matrix invert function for the specific type
   * of matrix you are dealing with is faster, more accurate.
   * @SINCE_1_0.0
   * @return True if successful
   */
  bool Invert();

  /**
   * @brief Swaps the rows to columns.
   * @SINCE_1_0.0
   */
  constexpr void Transpose()
  {
    float temp = mMatrix[1];
    mMatrix[1] = mMatrix[4];
    mMatrix[4] = temp;

    temp       = mMatrix[2];
    mMatrix[2] = mMatrix[8];
    mMatrix[8] = temp;

    temp        = mMatrix[3];
    mMatrix[3]  = mMatrix[12];
    mMatrix[12] = temp;

    temp       = mMatrix[6];
    mMatrix[6] = mMatrix[9];
    mMatrix[9] = temp;

    temp        = mMatrix[7];
    mMatrix[7]  = mMatrix[13];
    mMatrix[13] = temp;

    temp        = mMatrix[11];
    mMatrix[11] = mMatrix[14];
    mMatrix[14] = temp;
  }

  /**
   * @brief Returns the xAxis from a Transform matrix.
   *
   * @SINCE_1_0.0
   * @return The x axis
   */
  constexpr Vector3 GetXAxis() const
  {
    return Vector3(mMatrix[0], mMatrix[1], mMatrix[2]);
  }

  /**
   * @brief Returns the yAxis from a Transform matrix.
   *
   * @SINCE_1_0.0
   * @return The y axis
   */
  constexpr Vector3 GetYAxis() const
  {
    return Vector3(mMatrix[4], mMatrix[5], mMatrix[6]);
  }

  /**
   * @brief Returns the zAxis from a Transform matrix.
   *
   * @SINCE_1_0.0
   * @return The z axis
   */
  constexpr Vector3 GetZAxis() const
  {
    return Vector3(mMatrix[8], mMatrix[9], mMatrix[10]);
  }

  /**
   * @brief Sets the x axis.
   *
   * This assumes the matrix is a transform matrix.
   * @SINCE_1_0.0
   * @param[in] axis The values to set the axis to
   */
  constexpr void SetXAxis(const Vector3& axis)
  {
    mMatrix[0] = axis.x;
    mMatrix[1] = axis.y;
    mMatrix[2] = axis.z;
  }

  /**
   * @brief Sets the y axis.
   *
   * This assumes the matrix is a transform matrix.
   * @SINCE_1_0.0
   * @param[in] axis The values to set the axis to
   */
  constexpr void SetYAxis(const Vector3& axis)
  {
    mMatrix[4] = axis.x;
    mMatrix[5] = axis.y;
    mMatrix[6] = axis.z;
  }

  /**
   * @brief Sets the z axis.
   *
   * This assumes the matrix is a transform matrix.
   * @SINCE_1_0.0
   * @param[in] axis The values to set the axis to
   */
  constexpr void SetZAxis(const Vector3& axis)
  {
    mMatrix[8]  = axis.x;
    mMatrix[9]  = axis.y;
    mMatrix[10] = axis.z;
  }

  /**
   * @brief Gets the translation.
   *
   * This assumes the matrix is a transform matrix.
   * @SINCE_1_0.0
   * @return The translation
   * @note inlined for performance reasons (generates less code than a function call)
   */
  const Vector4& GetTranslation() const
  {
    return reinterpret_cast<const Vector4&>(mMatrix[12]);
  }

  /**
   * @brief Gets the x,y and z components of the translation as a Vector3.
   *
   * This assumes the matrix is a transform matrix.
   * @SINCE_1_0.0
   * @return The translation
   * @note inlined for performance reasons (generates less code than a function call)
   */
  const Vector3& GetTranslation3() const
  {
    return reinterpret_cast<const Vector3&>(mMatrix[12]);
  }

  /**
   * @brief Gets the x,y and z components of the scale as a Vector3.
   * Note that transform scale always has positive components.
   *
   * This assumes the matrix is a transform matrix.
   * @SINCE_2_2.17
   * @return The scale
   */
  Vector3 GetScale() const;

  /**
   * @brief Gets the x,y components of the scale as a Vector2.
   * Note that transform scale always has positive components.
   *
   * This assumes the matrix is a transform matrix.
   * @SINCE_2_4.19
   * @return The scale of xy components
   */
  Vector2 GetScaleXY() const;

  /**
   * @brief Sets the translation.
   *
   * This assumes the matrix is a transform matrix.
   * @SINCE_1_0.0
   * @param[in] translation The translation
   */
  constexpr void SetTranslation(const Vector4& translation)
  {
    mMatrix[12] = translation.x;
    mMatrix[13] = translation.y;
    mMatrix[14] = translation.z;
    mMatrix[15] = translation.w;
  }

  /**
   * @brief Sets the x,y and z components of the translation from a Vector3.
   *
   * This assumes the matrix is a transform matrix.
   * @SINCE_1_0.0
   * @param[in] translation The translation
   */
  constexpr void SetTranslation(const Vector3& translation)
  {
    mMatrix[12] = translation.x;
    mMatrix[13] = translation.y;
    mMatrix[14] = translation.z;
    mMatrix[15] = 1.f;
  }

  /**
   * @brief Makes the axes of the matrix orthogonal to each other and of unit length.
   *
   * This function is used to correct floating point errors which would otherwise accumulate
   * as operations are applied to the matrix. This function assumes the matrix is a transform
   * matrix.
   * @SINCE_1_0.0
   */
  void OrthoNormalize();

  /**
   * @brief Returns the contents of the matrix as an array of 16 floats.
   *
   * The order of the values for a transform matrix is:
   *
   * @code
   * [ xAxis.x, xAxis.y, xAxis.z, 0.0f, yAxis.x, yAxis.y, yAxis.z, 0.0f, zAxis.x, zAxis.y, zAxis.z, 0.0f, trans.x, trans.y, trans.z, 1.0f ]
   * @endcode
   *
   * @SINCE_1_0.0
   * @return The matrix contents as an array of 16 floats
   * @note inlined for performance reasons (generates less code than a function call)
   */
  constexpr const float* AsFloat() const
  {
    return mMatrix;
  }

  /**
   * @brief Returns the contents of the matrix as an array of 16 floats.
   *
   * The order of the values for a transform matrix is:
   *
   * @code
   * [ xAxis.x, xAxis.y, xAxis.z, 0.0f, yAxis.x, yAxis.y, yAxis.z, 0.0f, zAxis.x, zAxis.y, zAxis.z, 0.0f, trans.x, trans.y, trans.z, 1.0f ]
   * @endcode
   *
   * @SINCE_1_0.0
   * @return The matrix contents as an array of 16 floats
   * @note inlined for performance reasons (generates less code than a function call)
   */
  constexpr float* AsFloat()
  {
    return mMatrix;
  }
  /**
   * @brief Function to multiply two matrices and store the result onto third.
   *
   * Use this method in time critical path as it does not require temporaries.
   *
   * result = rhs * lhs
   *
   * @SINCE_1_0.0
   * @param[out] result Result of the multiplication
   * @param[in] lhs Matrix, this can be same matrix as result
   * @param[in] rhs Matrix, this cannot be same matrix as result
   */
  static void Multiply(Matrix& result, const Matrix& lhs, const Matrix& rhs);

  /**
   * @brief Function to multiply a matrix and quaternion and store the result onto third.
   *
   * Use this method in time critical path as it does not require temporaries.
   * @SINCE_1_0.0
   * @param[out] result Result of the multiplication
   * @param[in] lhs Matrix, this can be same matrix as result
   * @param[in] rhs Quaternion
   */
  static void Multiply(Matrix& result, const Matrix& lhs, const Quaternion& rhs);

  /**
   * @brief Multiplication operator.
   *
   * Returned Matrix = This Matrix * rhs
   *
   * @note It makes some memory allocate & copy internally.
   * Use Matrix::Multiply API for time critical path.
   *
   * @SINCE_2_1.44
   * @param[in] rhs The Matrix to multiply this by
   * @return A Matrix containing the result
   */
  Matrix operator*(const Matrix& rhs) const;

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
  Matrix& operator*=(const Matrix& rhs);

  /**
   * @brief The multiplication operator.
   *
   * Returned Vector = This Matrix * rhs
   *
   * @SINCE_1_0.0
   * @param[in] rhs The Vector4 to multiply this by
   * @return A Vector4 containing the result
   */
  Vector4 operator*(const Vector4& rhs) const;

  /**
   * @brief The equality operator.
   *
   * Utilizes appropriate machine epsilon values.
   *
   * @SINCE_1_0.0
   * @param[in] rhs The Matrix to compare this to
   * @return true if the matrices are equal
   */
  constexpr bool operator==(const Matrix& rhs) const
  {
    return (
      (Internal::MathUtils::Abs(mMatrix[0] - rhs.mMatrix[0]) <= GetRangedEpsilon(mMatrix[0], rhs.mMatrix[0])) &&
      (Internal::MathUtils::Abs(mMatrix[1] - rhs.mMatrix[1]) <= GetRangedEpsilon(mMatrix[1], rhs.mMatrix[1])) &&
      (Internal::MathUtils::Abs(mMatrix[2] - rhs.mMatrix[2]) <= GetRangedEpsilon(mMatrix[2], rhs.mMatrix[2])) &&
      (Internal::MathUtils::Abs(mMatrix[3] - rhs.mMatrix[3]) <= GetRangedEpsilon(mMatrix[3], rhs.mMatrix[3])) &&
      (Internal::MathUtils::Abs(mMatrix[4] - rhs.mMatrix[4]) <= GetRangedEpsilon(mMatrix[4], rhs.mMatrix[4])) &&
      (Internal::MathUtils::Abs(mMatrix[5] - rhs.mMatrix[5]) <= GetRangedEpsilon(mMatrix[5], rhs.mMatrix[5])) &&
      (Internal::MathUtils::Abs(mMatrix[6] - rhs.mMatrix[6]) <= GetRangedEpsilon(mMatrix[6], rhs.mMatrix[6])) &&
      (Internal::MathUtils::Abs(mMatrix[7] - rhs.mMatrix[7]) <= GetRangedEpsilon(mMatrix[7], rhs.mMatrix[7])) &&
      (Internal::MathUtils::Abs(mMatrix[8] - rhs.mMatrix[8]) <= GetRangedEpsilon(mMatrix[8], rhs.mMatrix[8])) &&
      (Internal::MathUtils::Abs(mMatrix[9] - rhs.mMatrix[9]) <= GetRangedEpsilon(mMatrix[9], rhs.mMatrix[9])) &&
      (Internal::MathUtils::Abs(mMatrix[10] - rhs.mMatrix[10]) <= GetRangedEpsilon(mMatrix[10], rhs.mMatrix[10])) &&
      (Internal::MathUtils::Abs(mMatrix[11] - rhs.mMatrix[11]) <= GetRangedEpsilon(mMatrix[11], rhs.mMatrix[11])) &&
      (Internal::MathUtils::Abs(mMatrix[12] - rhs.mMatrix[12]) <= GetRangedEpsilon(mMatrix[12], rhs.mMatrix[12])) &&
      (Internal::MathUtils::Abs(mMatrix[13] - rhs.mMatrix[13]) <= GetRangedEpsilon(mMatrix[13], rhs.mMatrix[13])) &&
      (Internal::MathUtils::Abs(mMatrix[14] - rhs.mMatrix[14]) <= GetRangedEpsilon(mMatrix[14], rhs.mMatrix[14])) &&
      (Internal::MathUtils::Abs(mMatrix[15] - rhs.mMatrix[15]) <= GetRangedEpsilon(mMatrix[15], rhs.mMatrix[15])));
  }

  /**
   * @brief The inequality operator.
   *
   * Utilizes appropriate machine epsilon values.
   * @SINCE_1_0.0
   * @param[in] rhs The Matrix to compare this to
   * @return true if the matrices are not equal.
   */
  constexpr bool operator!=(const Matrix& rhs) const
  {
    if(*this == rhs)
    {
      return false;
    }

    return true;
  }

  /**
   * @brief Sets this matrix to contain the position, scale and rotation components.
   *
   * Performs scale, rotation, then translation
   * @SINCE_1_0.0
   * @param[in] scale Scale to apply
   * @param[in] rotation Rotation to apply
   * @param[in] translation Translation to apply
   */
  void SetTransformComponents(const Vector3&    scale,
                              const Quaternion& rotation,
                              const Vector3&    translation);

  /**
   * @brief Sets this matrix to contain the inverse of the position, scale and rotation components.
   *
   * Performs translation, then rotation, then scale.
   * @SINCE_1_0.0
   * @param[in] scale Scale to apply
   * @param[in] rotation Rotation to apply
   * @param[in] translation Translation to apply
   */
  void SetInverseTransformComponents(const Vector3&    scale,
                                     const Quaternion& rotation,
                                     const Vector3&    translation);

  /**
   * @brief Sets this matrix to contain the inverse of the orthonormal basis and position components.
   *
   * Performs translation, then rotation.
   * @SINCE_1_0.0
   * @param[in] xAxis The X axis of the basis
   * @param[in] yAxis The Y axis of the basis
   * @param[in] zAxis The Z axis of the basis
   * @param[in] translation Translation to apply
   */
  void SetInverseTransformComponents(const Vector3& xAxis,
                                     const Vector3& yAxis,
                                     const Vector3& zAxis,
                                     const Vector3& translation);

  /**
   * @brief Gets the position, scale and rotation components from the given transform matrix.
   *
   * @SINCE_1_0.0
   * @param[out] position Position to set
   * @param[out] rotation Rotation to set - only valid if the transform matrix has not been skewed or sheared
   * @param[out] scale Scale to set - only valid if the transform matrix has not been skewed or sheared
   * @pre This matrix must not contain skews or shears.
   */
  void GetTransformComponents(Vector3&    position,
                              Quaternion& rotation,
                              Vector3&    scale) const;

private:
  float mMatrix[16]; ///< The elements of the matrix
};

// Allow Matrix to be treated as a POD type
template<>
struct TypeTraits<Matrix> : public BasicTypes<Matrix>
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

#endif // DALI_MATRIX_H
