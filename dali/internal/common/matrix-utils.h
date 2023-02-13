#ifndef DALI_INTERNAL_MATRIX_UTILS_H
#define DALI_INTERNAL_MATRIX_UTILS_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

namespace Dali
{
class Matrix;
class Matrix3;
class Quaternion;

namespace Internal
{
namespace MatrixUtils
{
// Quaternion

/**
 * @brief Function to convert from Quaternion to Matrix as float16 array form
 *
 * @SINCE_2_1.46
 * @param[out] result Result stored float16 array. It must have at least 16 * sizeof(float) bytes.
 * @param[in] rotation Input Quaternion.
 */
void ConvertQuaternion(float*& result, const Dali::Quaternion& rotation);

// Matrix

/**
 * @copydoc Dali::Matrix::Multiply
 */
void Multiply(Dali::Matrix& result, const Dali::Matrix& lhs, const Dali::Matrix& rhs);

/**
 * @copydoc Dali::Matrix::Multiply
 */
void Multiply(Dali::Matrix& result, const Dali::Matrix& lhs, const Dali::Quaternion& rhs);

/**
 * @brief Function to multiply projection matrix and store the result onto third.
 *
 * This API assume that projection is Projection Matrix which top/bottom/left/right is symmetrical.
 *
 * Perspective matrix only has 0, 5, 10, 11, 14 (14 is const value, 1.0f).
 * Orthographic matrix only has 0, 5, 10, 14, 15 (15 is const value, 1.0f).
 * If window rotated, we use 1, 4 index instead of 0, 5.
 * So we only need 8 values to multiplication.
 *
 * Use this method in time critical path as it does not require temporaries.
 *
 * result = projection * lhs
 *
 * @SINCE_2_1.46
 * @param[out] result Result of the multiplication
 * @param[in] lhs Matrix, this cannot be same matrix as result
 * @param[in] projection Projection Matrix, this can be same matrix as result
 */
void MultiplyProjectionMatrix(Dali::Matrix& result, const Dali::Matrix& lhs, const Dali::Matrix& projection);

/**
 * @brief Function to multiply two matrices and store the result onto first one.
 *
 * result = result * rhs
 * result *= rhs
 *
 * @note This method might copy data internally.
 *
 * @SINCE_2_1.46
 * @param[in,out] result Result of the multiplication
 * @param[in] rhs Matrix, this can be same matrix as result
 */
void MultiplyAssign(Dali::Matrix& result, const Dali::Matrix& rhs);

// Matrix3

/**
 * @copydoc Dali::Matrix3::Multiply
 */
void Multiply(Dali::Matrix3& result, const Dali::Matrix3& lhs, const Dali::Matrix3& rhs);

/**
 * @brief Function to multiply two matrices and store the result onto first one.
 *
 * result = result * rhs
 * result *= rhs
 *
 * @note This method might copy data internally.
 *
 * @SINCE_2_1.46
 * @param[in,out] result Result of the multiplication
 * @param[in] rhs Matrix, this can be same matrix as result
 */
void MultiplyAssign(Dali::Matrix3& result, const Dali::Matrix3& rhs);

} // namespace MatrixUtils
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_MATH_UTILS_H
