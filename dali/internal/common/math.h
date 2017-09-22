#ifndef DALI_INTERNAL_MATH_H
#define DALI_INTERNAL_MATH_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

class Vector2;
class Matrix;

namespace Internal
{

typedef float Vec3[3];
typedef float Vec4[4];
typedef float Mat4[16];

/**
 * @brief Applies a transformation matrix to a Vector3
 *
 * @param[out] result The transformed vector
 * @param[in] m The transformation matrix
 * @param[in] v The vector to transform
 */
void TransformVector3( Vec3 result, const Mat4 m, const Vec3 v );

/**
 * @brief Applies a transformation matrix to a Vector2
 * Note: This performs an optimized 2D transformation.
 *
 * @param[in] matrix      The matrix to transform by
 * @param[in] x           The x coordinate to multiply the matrix by
 * @param[in] y           The y coordinate to multiply the matrix by
 * @return                A Vector2 containing the resulting coordinates
 */
Vector2 Transform2D( const Matrix& matrix, const float x, const float y );

/**
 * @brief Computes the length of a vector3
 *
 * @param[in] v The vector
 * @return The lenght of the vector
 */
float Length( const Vec3 v );

} // namespace Internal

} // namespace Dali

#endif  // DALI_INTERNAL_MATH_H
