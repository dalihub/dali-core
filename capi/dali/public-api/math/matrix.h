#ifndef __DALI_MATRIX_H__
#define __DALI_MATRIX_H__

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
#include <iostream>

// INTERNAL INCLUDES
#include <dali/public-api/math/vector4.h>

namespace Dali DALI_IMPORT_API
{
class Quaternion;

/**
 * @brief The Matrix class represents transformations and projections.
 * It is agnostic w.r.t. row/column major notation - it operates on a flat array.
 * Each axis is contiguous in memory, so the x axis corresponds to elements 0, 1, 2 and 3, the y axis dorresponds to elements 4, 5, 6, 7, etc.
 */
class DALI_IMPORT_API Matrix
{
public:

  friend std::ostream& operator<< (std::ostream& o, const Matrix& matrix);

  /**
   * @brief Constructor.
   *
   * Zero initialises the matrix
   */
  Matrix();

  /**
   * @brief Constructor.
   *
   * @param initialize to zero or leave uninitialized
   */
  explicit Matrix( bool initialize );

  /**
   * @brief Constructor
   *
   * The matrix is initialised with the contents of 'array' which must contain 16 floats.
   * The order of the values for a transform matrix is:
   *
   *   xAxis.x xAxis.y xAxis.z 0.0f
   *   yAxis.x yAxis.y yAxis.z 0.0f
   *   zAxis.x zAxis.y zAxis.z 0.0f
   *   trans.x trans.y trans.z 1.0f
   *
   * @param [in] array     16 floats
   */
  explicit Matrix(const float* array);

  /**
   * @brief Constructs a matrix from quaternion.
   *
   * @param rotation as quaternion
   */
  explicit Matrix( const Quaternion& rotation );

  /**
   * @brief Copy constructor.
   *
   * @param [in] matrix to copy values from
   */
  Matrix( const Matrix& matrix );

  /**
   * @brief Assignment operator.
   *
   * @param [in] matrix to copy values from
   * @return a reference to this
   */
  Matrix& operator=( const Matrix& matrix );

  /**
   * @brief The identity matrix.
   */
  static const Matrix IDENTITY;

  /**
   * @brief Sets this matrix to be an identity matrix.
   */
  void SetIdentity();

  /**
   * @brief Sets this matrix to be an identity matrix with scale.
   *
   * @param scale to set on top of identity matrix
   */
  void SetIdentityAndScale( const Vector3& scale );

  /**
   * @brief Invert a transform Matrix.
   *
   * Any Matrix representing only a rotation and/or translation
   * can be inverted using this function. It is faster and more accurate then using Invert().
   * @param [out] result     returns the inverse of this matrix
   */
  void InvertTransform(Matrix& result) const;

  /**
   * @brief Generic brute force Matrix Invert.
   *
   * Using the Matrix invert function for the specific type
   * of matrix you are dealing with is faster, more accurate.
   * @return true if successful
   */
  bool Invert();

  /**
   * @brief Swaps the rows to columns.
   */
  void Transpose();

  /**
   * @brief Returns the xAxis from a Transform matrix.
   *
   * @return the x axis
   */
  Vector3 GetXAxis() const;

  /**
   * @brief Returns the yAxis from a Transform matrix.
   *
   * @return the y axis
   */
  Vector3 GetYAxis() const;

  /**
   * @brief Returns the zAxis from a Transform matrix.
   *
   * @return the z axis
   */
  Vector3 GetZAxis() const;

  /**
   * @brief Sets the x axis.
   *
   * This assumes the matrix is a transform matrix.
   * @param [in] axis     the values to set the axis to
   */
  void SetXAxis(const Vector3& axis);

  /**
   * @brief Sets the y axis.
   *
   * This assumes the matrix is a transform matrix.
   * @param [in] axis     the values to set the axis to
   */
  void SetYAxis(const Vector3& axis);

  /**
   * @brief Sets the z axis.
   *
   * This assumes the matrix is a transform matrix.
   * @param [in] axis     the values to set the axis to
   */
  void SetZAxis(const Vector3& axis);

  /**
   * @brief Gets the translation.
   *
   * This assumes the matrix is a transform matrix.
   * @note inlined for performance reasons (generates less code than a function call)
   * @return the translation
   */
  const Vector4& GetTranslation() const { return reinterpret_cast<const Vector4&>(mMatrix[12]); }

  /**
   * @brief Gets the x,y and z components of the translation as a Vector3.
   *
   * This assumes the matrix is a transform matrix.
   * @note inlined for performance reasons (generates less code than a function call)
   * @return the translation
   */
  const Vector3& GetTranslation3() const { return reinterpret_cast<const Vector3&>(mMatrix[12]); }

  /**
   * @brief Sets the translation.
   *
   * This assumes the matrix is a transform matrix.
   * @param [in] translation   the translation
   */
  void SetTranslation(const Vector4& translation);

  /**
   * @brief Sets the x,y and z components of the translation from a Vector3.
   *
   * This assumes the matrix is a transform matrix.
   * @param [in] translation   the translation
   */
  void SetTranslation(const Vector3& translation);

  /**
   * @brief Makes the axes of the matrix orthogonal to each other and of unit length.
   *
   * This function is used to correct floating point errors which would otherwise accumulate
   * as operations are applied to the matrix. This function assumes the matrix is a transform
   * matrix.
   */
  void OrthoNormalize();

  /**
   * @brief Returns the contents of the matrix as an array of 16 floats.
   *
   * The order of the values for a transform matrix is:
   *   xAxis.x xAxis.y xAxis.z 0.0f
   *   yAxis.x yAxis.y yAxis.z 0.0f
   *   zAxis.x zAxis.y zAxis.z 0.0f
   *   trans.x trans.y trans.z 1.0f
   * @note inlined for performance reasons (generates less code than a function call)
   * @return the matrix contents as an array of 16 floats.
   */
  const float* AsFloat() const {return mMatrix;}

  /**
   * @brief Returns the contents of the matrix as an array of 16 floats.
   *
   * The order of the values for a transform matrix is:
   *
   *   xAxis.x xAxis.y xAxis.z 0.0f
   *   yAxis.x yAxis.y yAxis.z 0.0f
   *   zAxis.x zAxis.y zAxis.z 0.0f
   *   trans.x trans.y trans.z 1.0f
   * @note inlined for performance reasons (generates less code than a function call)
   * @return the matrix contents as an array of 16 floats.
   */
  float* AsFloat() {return mMatrix;}

  /**
   * @brief Function to multiply two matrices and store the result onto third.
   *
   * Use this method in time critical path as it does not require temporaries
   * @param result of the multiplication
   * @param lhs matrix, this can be same matrix as result
   * @param rhs matrix, this cannot be same matrix as result
   */
  static void Multiply( Matrix& result, const Matrix& lhs, const Matrix& rhs );

  /**
   * @brief Function to multiply a matrix and quaternion and store the result onto third.
   *
   * Use this method in time critical path as it does not require temporaries
   * @param result of the multiplication
   * @param lhs matrix, this can be same matrix as result
   * @param rhs quaternion
   */
  static void Multiply( Matrix& result, const Matrix& lhs, const Quaternion& rhs );

  /**
   * @brief The multiplication operator.
   *
   * @param [in] rhs    the Matrix to multiply this by
   * @return A matrix containing the result
   */
  Vector4 operator*(const Vector4& rhs) const;

  /**
   * @brief The equality operator.
   *
   * Utilises appropriate machine epsilon values.
   *
   * @param [in] rhs    the Matrix to compare this to
   * @return true if the matrices are equal
   */
  bool operator==(const Matrix & rhs) const;

  /**
   * @brief The inequality operator.
   *
   * Utilises appropriate machine epsilon values.
   * @param [in] rhs    the Matrix to compare this to
   * @return true if the matrices are not equal.
   */
  bool operator!=(const Matrix & rhs) const;

  /**
   * @brief Sets this matrix to contain the position, scale and rotation components.
   *
   * Performs scale, rotation, then translation
   * @param[in] scale to apply
   * @param[in] rotation to apply
   * @param[in] translation to apply
   */
  void SetTransformComponents(const Vector3& scale,
                              const Quaternion& rotation,
                              const Vector3& translation );

  /**
   * @brief Sets this matrix to contain the inverse of the position, scale and rotation components.
   *
   * Performs translation, then rotation, then scale.
   * @param[in] scale to apply
   * @param[in] rotation to apply
   * @param[in] translation to apply
   */
  void SetInverseTransformComponents(const Vector3&    scale,
                                     const Quaternion& rotation,
                                     const Vector3&    translation );


  /**
   * @brief Sets this matrix to contain the inverse of the orthonormal basis and position components.
   *
   * Performs translation, then rotation.
   * @param[in] xAxis The X axis of the basis
   * @param[in] yAxis The Y axis of the basis
   * @param[in] zAxis The Z axis of the basis
   * @param[in] translation to apply
   */
  void SetInverseTransformComponents(const Vector3&    xAxis,
                                     const Vector3&    yAxis,
                                     const Vector3&    zAxis,
                                     const Vector3&    translation );

  /**
   * @brief Gets the position, scale and rotation components from the given transform matrix.
   *
   * @pre This matrix must not contain skews or shears.
   * @param[out] position to set
   * @param[out] rotation to set - only valid if the transform matrix has not been skewed or sheared
   * @param[out] scale to set - only valid if the transform matrix has not been skewed or sheared
   */
  void GetTransformComponents(Vector3& position,
                              Quaternion& rotation,
                              Vector3& scale) const;

private:

  float mMatrix[16]; ///< The elements of the matrix
};

/**
 * @brief Print a matrix.
 *
 * It is printed in memory order, i.e. each printed row is contiguous in memory.
 * @param [in] o The output stream operator.
 * @param [in] matrix The matrix to print.
 * @return The output stream operator.
 */
DALI_IMPORT_API std::ostream& operator<< (std::ostream& o, const Matrix& matrix);

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_MATRIX_H__
