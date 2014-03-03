#ifndef __DALI_MATRIX3_H__
#define __DALI_MATRIX3_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/matrix.h>

namespace Dali DALI_IMPORT_API
{

class Matrix;
struct Vector2;

/**
 * A 3x3 matrix.
 */
class DALI_IMPORT_API Matrix3
{
public:

  friend std::ostream& operator<< (std::ostream& o, const Matrix3& matrix);

  /**
   * The identity matrix
   */
  static const Matrix3 IDENTITY;

  /**
   * Constructors
   */
  Matrix3();
  Matrix3(const Matrix3& m);
  Matrix3(const Matrix& m);
  Matrix3(float s00, float s01, float s02, float s10, float s11, float s12, float s20, float s21, float s22);

  /**
   * Assignment Operator
   * @param matrix from which to copy values
   * @return reference to this object
   */
  Matrix3& operator=( const Matrix3& matrix );

  /**
   * Assignment Operator
   * @param matrix from which to copy values
   * @return reference to this object
   */
  Matrix3& operator=( const Matrix& matrix );

  /**
   * The equality operator.
   * Utilises appropriate machine epsilon values.
   *
   * @param [in] rhs    the Matrix to compare this to
   * @return true if the matrices are equal
   */
  bool operator==(const Matrix3 & rhs) const;

  /**
   * The inequality operator.
   * Utilises appropriate machine epsilon values.
   *
   * @param [in] rhs    the Matrix to compare this to
   * @return true if the matrices are equal
   */
  bool operator!=(const Matrix3 & rhs) const;

  /**
   * Destructor
   */
  ~Matrix3()
  {
  }

  /**
   * Sets the matrix to the identity matrix.
   */
  void SetIdentity();

  /**
   * Returns the contents of the matrix as an array of 9 floats.
   * The order of the values for a matrix is:
   *   xAxis.x yAxis.x zAxis.x
   *   xAxis.y yAxis.y zAxis.y
   *   xAxis.z yAxis.z zAxis.z
   * @return the matrix contents as an array of 9 floats.
   */
  const float* AsFloat() const {return &mElements[0];}

  /**
   * Returns the contents of the matrix as an array of 9 floats.
   * The order of the values for a matrix is:
   *   xAxis.x yAxis.x zAxis.x
   *   xAxis.y yAxis.y zAxis.y
   *   xAxis.z yAxis.z zAxis.z
   * @return the matrix contents as an array of 9 floats.
   */
  float* AsFloat() {return &mElements[0];}

  /**
   * Inverts the matrix.
   * @return true if successful
   */
  bool Invert();

  /**
   * Swaps the rows to columns
   * @return true
   */
  bool Transpose();

  /**
   * Multiplies all elements of the matrix by the scale value
   * @param scale - the value by which to scale the whole matrix.
   */
  void Scale(float scale);

  /**
   * Magnitude returns the average of the absolute values of the
   * elements * 3. (The Magnitude of the unit matrix is therefore 1)
   * @return the magnitude - always positive.
   */
  float Magnitude() const;

  /**
   * If the matrix is invertible, then this method inverts, transposes
   * and scales the matrix such that the resultant element values
   * average 1.
   * If the matrix is not invertible, then the matrix is left unchanged.

   * @return true if the matrix is invertible, otherwise false
   */
  bool ScaledInverseTranspose();

  /**
   * Function to multiply two matrices and store the result onto third
   * Use this method in time critical path as it does not require temporaries
   * @param result of the multiplication
   * @param lhs matrix, this can be same matrix as result
   * @param rhs matrix, this cannot be same matrix as result
   */
  static void Multiply( Matrix3& result, const Matrix3& lhs, const Matrix3& rhs );

private:

  float mElements[9];
};

/**
 * Print a 3x3 matrix.
 * @param [in] o The output stream operator.
 * @param [in] matrix The matrix to print.
 * @return The output stream operator.
 */
DALI_IMPORT_API std::ostream& operator<< (std::ostream& o, const Matrix3& matrix);

} // namespace Dali

/**
 * @}
 */
#endif //__DALI_MATRIX3_H__
