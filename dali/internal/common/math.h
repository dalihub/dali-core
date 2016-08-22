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

typedef float Vec3[3];
typedef float Vec4[4];
typedef float Mat4[16];

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

} // namespace Internal

} // namespace Dali

#endif  //__DALI_INTERNAL_MATH_H__
