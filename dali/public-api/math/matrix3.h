#ifndef __DALI_MATRIX3_H__
#define __DALI_MATRIX3_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/common/type-traits.h>

namespace Dali
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

  friend std::ostream& operator<< (std::ostream& o, const Matrix3& matrix);

  /**
   * @brief The identity matrix.
   */
  static const Matrix3 IDENTITY;

  /**
   * @brief Constructor.
   * @SINCE_1_0.0
   */
  Matrix3();

  /**
   * @brief Copy Constructor.
   *
   * @SINCE_1_0.0
   * @param[in] m A reference to the copied 3x3 matrix
   */
  Matrix3(const Matrix3& m);

  /**
   * @brief Constructor.
   *
   * @SINCE_1_0.0
   * @param[in] m A 4x4 matrix. The translation and shear components are ignored
   */
  Matrix3(const Matrix& m);

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
  Matrix3(float s00, float s01, float s02, float s10, float s11, float s12, float s20, float s21, float s22);

  /**
   * @brief Assignment Operator.
   * @SINCE_1_0.0
   * @param[in] matrix From which to copy values
   * @return Reference to this object
   */
  Matrix3& operator=( const Matrix3& matrix );

  /**
   * @brief Assignment Operator.
   * @SINCE_1_0.0
   * @param[in] matrix A reference to the copied matrix
   * @return A reference to this
   */
  Matrix3& operator=( const Matrix& matrix );

  /**
   * @brief The equality operator.
   *
   * Utilizes appropriate machine epsilon values.
   *
   * @SINCE_1_0.0
   * @param[in] rhs The Matrix to compare this to
   * @return True if the matrices are equal
   */
  bool operator==(const Matrix3 & rhs) const;

  /**
   * @brief The inequality operator.
   *
   * Utilizes appropriate machine epsilon values.
   *
   * @SINCE_1_0.0
   * @param[in] rhs The Matrix to compare this to
   * @return true if the matrices are equal
   */
  bool operator!=(const Matrix3 & rhs) const;

  /**
   * @brief Destructor.
   * @SINCE_1_0.0
   */
  ~Matrix3()
  {
  }

  /**
   * @brief Sets the matrix to the identity matrix.
   * @SINCE_1_0.0
   */
  void SetIdentity();

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
  const float* AsFloat() const {return &mElements[0];}

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
  float* AsFloat() {return &mElements[0];}

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
  bool Transpose();

  /**
   * @brief Multiplies all elements of the matrix by the scale value.
   *
   * @SINCE_1_0.0
   * @param[in] scale The value by which to scale the whole matrix
   */
  void Scale(float scale);

  /**
   * @brief Magnitude returns the average of the absolute values of the
   * elements * 3.
   *
   * (The Magnitude of the unit matrix is therefore 1)
   * @SINCE_1_0.0
   * @return The magnitude - always positive
   */
  float Magnitude() const;

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
  static void Multiply( Matrix3& result, const Matrix3& lhs, const Matrix3& rhs );

private:

  float mElements[9]; ///< The elements of the matrix
};

/**
 * @brief Prints a 3x3 matrix.
 *
 * @SINCE_1_0.0
 * @param[in] o The output stream operator
 * @param[in] matrix The matrix to print
 * @return The output stream operator
 */
DALI_CORE_API std::ostream& operator<< (std::ostream& o, const Matrix3& matrix);

// Allow Matrix3 to be treated as a POD type
template <> struct TypeTraits< Matrix3 > : public BasicTypes< Matrix3 > { enum { IS_TRIVIAL_TYPE = true }; };

/**
 * @}
 */
} // namespace Dali

#endif //__DALI_MATRIX3_H__
