#ifndef __DALI_INTERNAL_MATH_H__
#define __DALI_INTERNAL_MATH_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

namespace Internal
{

typedef float Vec2[2];
typedef float Vec3[3];
typedef float Vec4[4];
typedef float Mat4[16];
typedef Vec3  Size3;

/**
 * @brief Applies a transformation matrix to a vector
 *
 * @param[out] result The transformed vector
 * @param[in] m The transformation matrix
 * @param[in] v The vector to transform
 */
void TransformVector3( Vec3 result, const Mat4 m, const Vec3 v );

/**
 * @brief Computes the length of a vector3
 *
 * @param[in] v The vector
 * @return The lenght of the vector
 */
float Length( const Vec3 v );

/**
 * @brief Transforms 2D vector by the Size3 and stores value in Vec2
 *
 * @param[out] result Vec2 type to store final value
 * @param[in] v Vector to transform
 * @param[in] s Size to transform with
 */
void MultiplyVectorBySize( Vec2& result, const Vec2 v, const Size3 s );

/**
 * @brief Transforms 3D vector by the Size3 and stores value in Vec3
 *
 * @param[out] result Vec3 type to store final value
 * @param[in] v Vector to transform
 * @param[in] s Size to transform with
 */
void MultiplyVectorBySize( Vec3& result, const Vec3 v, const Size3 s );

/**
 * @brief Transforms 4D vector by the Size3 and stores value in Vec4
 *
 * @param[out] result Vec4 type to store final value
 * @param[in] v Vector to transform
 * @param[in] s Size to transform with
 */
void MultiplyVectorBySize( Vec4& result, const Vec4 v, const Size3 s );

/**
 * @brief Multiplies 2D vector by the matrix
 *
 * @param[out] result Result of the multiplication
 * @param[in] m Matrix to use
 * @param[in] v Vector to multiply
 */
void MultiplyVectorByMatrix4( Vec2& result, const Mat4 m, const Vec2 v );

/**
 * @brief Multiplies 3D vector by the matrix
 *
 * @param[out] result Result of the multiplication
 * @param[in] m Matrix to use
 * @param[in] v Vector to multiply
 */
void MultiplyVectorByMatrix4( Vec3& result, const Mat4 m, const Vec3 v );

/**
 * @brief Multiplies 4D vector by the matrix
 *
 * @param[out] result Result of the multiplication
 * @param[in] m Matrix to use
 * @param[in] v Vector to multiply
 */
void MultiplyVectorByMatrix4( Vec4& result, const Mat4 m, const Vec4 v );

/**
 * @brief Multiplies two Mat4 matrices
 *
 * @param[out] result Result of multiplication
 * @param[in] lhs Left-hand-side matrix to use
 * @param[in] rhs Right-hand-side matrix to use
 */
void MultiplyMatrices( float* result, const Mat4 lhs, const Mat4 rhs );


} // namespace Internal

} // namespace Dali

#endif  //__DALI_INTERNAL_MATH_H__
