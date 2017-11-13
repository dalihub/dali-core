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

// CLASS HEADER
#include <dali/internal/common/math.h>

// EXTERNAL INCLUDES
#include <cmath>

// INTERNAL INCLUDES
#include <dali/internal/render/common/performance-monitor.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/matrix.h>

void Dali::Internal::TransformVector3( Vec3 result, const Mat4 m, const Vec3 v )
{
#ifndef __ARM_NEON__

  result[0] = v[0] * m[0] + v[1] * m[4] + v[2] * m[8];
  result[1] = v[0] * m[1] + v[1] * m[5] + v[2] * m[9];
  result[2] = v[0] * m[2] + v[1] * m[6] + v[2] * m[10];

#else

  Vec4 temp = { v[0], v[1], v[2], 0.0f };
  Vec4 tempResult;

  asm volatile ( "VLD1.F32   {q0}, [%1]     \n\t"  //Load "temp" from memory to register q0
                 "VLD1.F32   {q1}, [%0]!    \n\t"  //Load first row of the matrix from memory to register q1
                 "VMUL.F32   q2, q1, d0[0]  \n\t"  //q2 = (m[0..3] * v.x)
                 "VLD1.F32   {q1}, [%0]!    \n\t"  //Load second row of the matrix from memory
                 "VMLA.F32   q2, q1, d0[1]  \n\t"  //q2 = (m[0..3] * v.x) + (m[4..7] * v.y)
                 "VLD1.F32   {q1}, [%0]!    \n\t"  //Load third row of the matrix from memory
                 "VMLA.F32   q2, q1, d1[0]  \n\t"  //q2 = (m[0..3] * v.x) + (m[4..7] * v.y) + (m[8...11] * v.z)
                 "VST1.F32   {q2}, [%2]     \n\t"  //Write the result back to memory
                 :
                 : "r"(m), "r"(temp), "r"(tempResult)
                 : "q0", "q1", "q2", "memory" );

  result[0] = tempResult[0];
  result[1] = tempResult[1];
  result[2] = tempResult[2];

#endif
}

Dali::Vector2 Dali::Internal::Transform2D( const Dali::Matrix& matrix, const float x, const float y )
{
  MATH_INCREASE_BY( PerformanceMonitor::FLOAT_POINT_MULTIPLY, 4 );

  const float* matrixArray( matrix.AsFloat() );

  // The following optimizations are applied:
  // Matrix[8 -> 11] are optimized out.
  // Matrix[12 -> 15] are always multiplied by 1.
  // z & w results (if we were doing a transformation to a Vector4) are unneeded and so not calculated.
  // As we always multiply by component, we do not store the coordinates in a Vector2 to avoid creation.
  // Note: For this reason the NEON SIMD version is no faster than the Dali::Matrix '*' Vector4 operator, and therefore not used.
  return Dali::Vector2( x * matrixArray[0] + y * matrixArray[4] + matrixArray[12], x * matrixArray[1] + y * matrixArray[5] + matrixArray[13] );
}

float Dali::Internal::Length( const Vec3 v )
{
  return sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}


