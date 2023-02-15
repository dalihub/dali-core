/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

// CLASS HEADERS
#include <dali/internal/common/matrix-utils.h>

// EXTERNAL INCLUDES
#include <cstdint> // uint32_t
#include <cstring> // memcpy

// INTERNAL INCLUDE
#include <dali/internal/render/common/performance-monitor.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/matrix3.h>
#include <dali/public-api/math/quaternion.h>

namespace
{
const uint32_t NUM_BYTES_IN_MATRIX(16 * sizeof(float));
const uint32_t NUM_BYTES_IN_MATRIX3(9 * sizeof(float));

} // namespace

namespace Dali::Internal
{
using Internal::PerformanceMonitor;

namespace MatrixUtils
{
// Dali::Quaternion

void ConvertQuaternion(float*& result, const Dali::Quaternion& rotation)
{
  MATH_INCREASE_COUNTER(PerformanceMonitor::QUATERNION_TO_MATRIX);

  const float xx = rotation.mVector.x * rotation.mVector.x;
  const float yy = rotation.mVector.y * rotation.mVector.y;
  const float zz = rotation.mVector.z * rotation.mVector.z;
  const float xy = rotation.mVector.x * rotation.mVector.y;
  const float xz = rotation.mVector.x * rotation.mVector.z;
  const float wx = rotation.mVector.w * rotation.mVector.x;
  const float wy = rotation.mVector.w * rotation.mVector.y;
  const float wz = rotation.mVector.w * rotation.mVector.z;
  const float yz = rotation.mVector.y * rotation.mVector.z;

  // clang-format off
  result[0] = 1.0f - 2.0f * (yy + zz);
  result[1] =        2.0f * (xy + wz);
  result[2] =        2.0f * (xz - wy);
  result[3] = 0.0f;

  result[4] =        2.0f * (xy - wz);
  result[5] = 1.0f - 2.0f * (xx + zz);
  result[6] =        2.0f * (yz + wx);
  result[7] = 0.0f;

  result[8] =        2.0f * (xz + wy);
  result[9] =        2.0f * (yz - wx);
  result[10]= 1.0f - 2.0f * (xx + yy);
  result[11]= 0.0f;

  result[12]= 0.0f;
  result[13]= 0.0f;
  result[14]= 0.0f;
  result[15]= 1.0f;
  // clang-format on
}

// Dali::Matrix

void Multiply(Dali::Matrix& result, const Dali::Matrix& lhs, const Dali::Matrix& rhs)
{
  MATH_INCREASE_COUNTER(PerformanceMonitor::MATRIX_MULTIPLYS);
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY, 64); // 64 = 16*4

  float*       temp   = result.AsFloat();
  const float* rhsPtr = rhs.AsFloat();
  const float* lhsPtr = lhs.AsFloat();

#ifndef __ARM_NEON__

  for(int32_t i = 0; i < 4; i++)
  {
    // i<<2 gives the first vector / column
    const int32_t loc0 = i << 2;
    const int32_t loc1 = loc0 + 1;
    const int32_t loc2 = loc0 + 2;
    const int32_t loc3 = loc0 + 3;

    const float value0 = lhsPtr[loc0];
    const float value1 = lhsPtr[loc1];
    const float value2 = lhsPtr[loc2];
    const float value3 = lhsPtr[loc3];

    temp[loc0] = (value0 * rhsPtr[0]) +
                 (value1 * rhsPtr[4]) +
                 (value2 * rhsPtr[8]) +
                 (value3 * rhsPtr[12]);

    temp[loc1] = (value0 * rhsPtr[1]) +
                 (value1 * rhsPtr[5]) +
                 (value2 * rhsPtr[9]) +
                 (value3 * rhsPtr[13]);

    temp[loc2] = (value0 * rhsPtr[2]) +
                 (value1 * rhsPtr[6]) +
                 (value2 * rhsPtr[10]) +
                 (value3 * rhsPtr[14]);

    temp[loc3] = (value0 * rhsPtr[3]) +
                 (value1 * rhsPtr[7]) +
                 (value2 * rhsPtr[11]) +
                 (value3 * rhsPtr[15]);
  }

#else

  // 64 32bit registers,
  // aliased to
  // d = 64 bit double-word d0 -d31
  // q =128 bit quad-word   q0 -q15  (enough to handle a column of 4 floats in a matrix)
  // e.g. q0 = d0 and d1

  // load and stores interleaved as NEON can load and store while calculating
  asm volatile(
    "VLDM         %1,  {q0-q3}        \n\t" // load matrix 1 (lhsPtr) q[0..q3]
    "VLDM         %0,  {q8-q11}       \n\t" // load matrix 2 (rhsPtr) q[q8-q11]
    "VMUL.F32     q12, q8, d0[0]      \n\t" // column 0 = rhsPtr[0..3] * lhsPtr[0..3]
    "VMUL.F32     q13, q8, d2[0]      \n\t" // column 1 = rhsPtr[0..3] * lhsPtr[4..7]
    "VMUL.F32     q14, q8, d4[0]      \n\t" // column 2 = rhsPtr[0..3] * lhsPtr[8..11]
    "VMUL.F32     q15, q8, d6[0]      \n\t" // column 3 = rhsPtr[0..3] * lhsPtr[12..15]

    "VMLA.F32     q12, q9, d0[1]      \n\t" // column 0 += rhsPtr[4..7] * lhsPtr[0..3]
    "VMLA.F32     q13, q9, d2[1]      \n\t" // column 1 += rhsPtr[4..7] * lhsPtr[4..7]
    "VMLA.F32     q14, q9, d4[1]      \n\t" // column 2 += rhsPtr[4..7] * lhsPtr[8..11]
    "VMLA.F32     q15, q9, d6[1]      \n\t" // column 3 += rhsPtr[4..7] * lhsPtr[12..15]

    "VMLA.F32     q12, q10, d1[0]     \n\t" // column 0 += rhsPtr[8..11] * lhsPtr[0..3]
    "VMLA.F32     q13, q10, d3[0]     \n\t" // column 1 += rhsPtr[8..11] * lhsPtr[4..7]
    "VMLA.F32     q14, q10, d5[0]     \n\t" // column 2 += rhsPtr[8..11] * lhsPtr[8..11]
    "VMLA.F32     q15, q10, d7[0]     \n\t" // column 3 += rhsPtr[8..11] * lhsPtr[12..15]

    "VMLA.F32     q12, q11, d1[1]     \n\t" // column 0 += rhsPtr[12..15] * lhsPtr[0..3]
    "VMLA.F32     q13, q11, d3[1]     \n\t" // column 1 += rhsPtr[12..15] * lhsPtr[4..7]
    "VMLA.F32     q14, q11, d5[1]     \n\t" // column 2 += rhsPtr[12..15] * lhsPtr[8..11]
    "VMLA.F32     q15, q11, d7[1]     \n\t" // column 3 += rhsPtr[12..15] * lhsPtr[12..15]
    "VSTM         %2,  {q12-q15}      \n\t" // store entire output matrix.
    : "+r"(rhsPtr), "+r"(lhsPtr), "+r"(temp)
    :
    : "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", "memory");

#endif
}

void Multiply(Dali::Matrix& result, const Dali::Matrix& lhs, const Dali::Quaternion& rhs)
{
  MATH_INCREASE_COUNTER(PerformanceMonitor::MATRIX_MULTIPLYS);
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY, 54); // 54 = 36+18

  float  matrix[16];
  float* rhsPtr = &matrix[0];
  ConvertQuaternion(rhsPtr, rhs);

  // quaternion contains just rotation so it really only needs 3x3 matrix

  float*       temp   = result.AsFloat();
  const float* lhsPtr = lhs.AsFloat();

#ifndef __ARM_NEON__

  for(int32_t i = 0; i < 4; i++)
  {
    // i<<2 gives the first vector / column
    const int32_t loc0 = i << 2;
    const int32_t loc1 = loc0 + 1;
    const int32_t loc2 = loc0 + 2;
    const int32_t loc3 = loc0 + 3;

    const float value0 = lhsPtr[loc0];
    const float value1 = lhsPtr[loc1];
    const float value2 = lhsPtr[loc2];
    const float value3 = lhsPtr[loc3];

    temp[loc0] = (value0 * rhsPtr[0]) +
                 (value1 * rhsPtr[4]) +
                 (value2 * rhsPtr[8]) +
                 (0.0f); //value3 * rhsPtr[12] is 0.0f

    temp[loc1] = (value0 * rhsPtr[1]) +
                 (value1 * rhsPtr[5]) +
                 (value2 * rhsPtr[9]) +
                 (0.0f); //value3 * rhsPtr[13] is 0.0f

    temp[loc2] = (value0 * rhsPtr[2]) +
                 (value1 * rhsPtr[6]) +
                 (value2 * rhsPtr[10]) +
                 (0.0f); //value3 * rhsPtr[14] is 0.0f

    temp[loc3] = (0.0f) +  //value0 * rhsPtr[3] is 0.0f
                 (0.0f) +  //value1 * rhsPtr[7] is 0.0f
                 (0.0f) +  //value2 * rhsPtr[11] is 0.0f
                 (value3); // rhsPtr[15] is 1.0f
  }

#else
  // Store 4th row values that might be overwrited.
  const float value0 = lhsPtr[3];
  const float value1 = lhsPtr[7];
  const float value2 = lhsPtr[11];
  const float value3 = lhsPtr[15];

  // 64 32bit registers,
  // aliased to
  // d = 64 bit double-word d0 -d31
  // q =128 bit quad-word   q0 -q15  (enough to handle a column of 4 floats in a matrix)
  // e.g. q0 = d0 and d1
  // load and stores interleaved as NEON can load and store while calculating
  asm volatile(
    "VLDM         %1,   {q4-q7}       \n\t" // load matrix 1 (lhsPtr)
    "VLD1.F32     {q8}, [%2]!         \n\t" // load matrix 2 (rhsPtr) [0..3]
    "VMUL.F32     q0,   q8,   d8[0]   \n\t" // column 0 = rhsPtr[0..3] * lhsPtr[0]
    "VMUL.F32     q1,   q8,   d10[0]  \n\t" // column 1 = rhsPtr[0..3] * lhsPtr[4]
    "VMUL.F32     q2,   q8,   d12[0]  \n\t" // column 2 = rhsPtr[0..3] * lhsPtr[8]
    "VMUL.F32     q3,   q8,   d14[0]  \n\t" // column 3 = rhsPtr[0..3] * lhsPtr[12]
    "VLD1.F32     {q8}, [%2]!         \n\t" // load matrix 2 (rhsPtr) [4..7]
    "VMLA.F32     q0,   q8,   d8[1]   \n\t" // column 0+= rhsPtr[4..7] * lhsPtr[1]
    "VMLA.F32     q1,   q8,   d10[1]  \n\t" // column 1+= rhsPtr[4..7] * lhsPtr[5]
    "VMLA.F32     q2,   q8,   d12[1]  \n\t" // column 2+= rhsPtr[4..7] * lhsPtr[9]
    "VMLA.F32     q3,   q8,   d14[1]  \n\t" // column 3+= rhsPtr[4..7] * lhsPtr[13]
    "VLD1.F32     {q8}, [%2]!         \n\t" // load matrix 2 (rhsPtr) [8..11]
    "VMLA.F32     q0,   q8,   d9[0]   \n\t" // column 0+= rhsPtr[8..11] * lhsPtr[2]
    "VMLA.F32     q1,   q8,   d11[0]  \n\t" // column 1+= rhsPtr[8..11] * lhsPtr[6]
    "VMLA.F32     q2,   q8,   d13[0]  \n\t" // column 2+= rhsPtr[8..11] * lhsPtr[10]
    "VMLA.F32     q3,   q8,   d15[0]  \n\t" // column 3+= rhsPtr[8..11] * lhsPtr[14]
    "VSTM         %0,   {q0-q3}       \n\t" // store entire output matrix.
    :
    : "r"(temp), "r"(lhsPtr), "r"(rhsPtr)
    : "%r0", "%q0", "%q1", "%q2", "%q3", "%q4", "%q5", "%q6", "%q7", "%q8", "memory");

  // Restore 4th row values.
  temp[3]  = value0;
  temp[7]  = value1;
  temp[11] = value2;
  temp[15] = value3;
#endif
}

void MultiplyProjectionMatrix(Dali::Matrix& result, const Dali::Matrix& lhs, const Dali::Matrix& projection)
{
  // TODO : Implement with NEON.
  // Current NEON code is copy of Multiply.

  MATH_INCREASE_COUNTER(PerformanceMonitor::MATRIX_MULTIPLYS);
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY, 40); // 40 = 10*4

  float*       temp   = result.AsFloat();
  const float* rhsPtr = projection.AsFloat();
  const float* lhsPtr = lhs.AsFloat();

#ifndef __ARM_NEON__

  // We only use rhsPtr's 0, 1, 2, 4, 5, 6, 10, 11, 14, 15 index.
  const float rhs0  = rhsPtr[0];
  const float rhs1  = rhsPtr[1];
  const float rhs2  = rhsPtr[2];
  const float rhs4  = rhsPtr[4];
  const float rhs5  = rhsPtr[5];
  const float rhs6  = rhsPtr[6];
  const float rhs10 = rhsPtr[10];
  const float rhs11 = rhsPtr[11];
  const float rhs14 = rhsPtr[14];
  const float rhs15 = rhsPtr[15];

  for(int32_t i = 0; i < 4; i++)
  {
    // i<<2 gives the first vector / column
    const int32_t loc0 = i << 2;
    const int32_t loc1 = loc0 + 1;
    const int32_t loc2 = loc0 + 2;
    const int32_t loc3 = loc0 + 3;

    const float value0 = lhsPtr[loc0];
    const float value1 = lhsPtr[loc1];
    const float value2 = lhsPtr[loc2];
    const float value3 = lhsPtr[loc3];

    temp[loc0] = (value0 * rhs0) + (value1 * rhs4);
    temp[loc1] = (value0 * rhs1) + (value1 * rhs5);
    temp[loc2] = (value0 * rhs2) + (value1 * rhs6) + (value2 * rhs10) + (value3 * rhs14);
    temp[loc3] = (value2 * rhs11) + (value3 * rhs15);
  }

#else

  // 64 32bit registers,
  // aliased to
  // d = 64 bit double-word d0 -d31
  // q =128 bit quad-word   q0 -q15  (enough to handle a column of 4 floats in a matrix)
  // e.g. q0 = d0 and d1

  // load and stores interleaved as NEON can load and store while calculating
  asm volatile(
    "VLDM         %1,  {q0-q3}        \n\t" // load matrix 1 (lhsPtr) q[0..q3]
    "VLDM         %0,  {q8-q11}       \n\t" // load matrix 2 (rhsPtr) q[q8-q11]
    "VMUL.F32     q12, q8, d0[0]      \n\t" // column 0 = rhsPtr[0..3] * lhsPtr[0..3]
    "VMUL.F32     q13, q8, d2[0]      \n\t" // column 1 = rhsPtr[0..3] * lhsPtr[4..7]
    "VMUL.F32     q14, q8, d4[0]      \n\t" // column 2 = rhsPtr[0..3] * lhsPtr[8..11]
    "VMUL.F32     q15, q8, d6[0]      \n\t" // column 3 = rhsPtr[0..3] * lhsPtr[12..15]

    "VMLA.F32     q12, q9, d0[1]      \n\t" // column 0 += rhsPtr[4..7] * lhsPtr[0..3]
    "VMLA.F32     q13, q9, d2[1]      \n\t" // column 1 += rhsPtr[4..7] * lhsPtr[4..7]
    "VMLA.F32     q14, q9, d4[1]      \n\t" // column 2 += rhsPtr[4..7] * lhsPtr[8..11]
    "VMLA.F32     q15, q9, d6[1]      \n\t" // column 3 += rhsPtr[4..7] * lhsPtr[12..15]

    "VMLA.F32     q12, q10, d1[0]     \n\t" // column 0 += rhsPtr[8..11] * lhsPtr[0..3]
    "VMLA.F32     q13, q10, d3[0]     \n\t" // column 1 += rhsPtr[8..11] * lhsPtr[4..7]
    "VMLA.F32     q14, q10, d5[0]     \n\t" // column 2 += rhsPtr[8..11] * lhsPtr[8..11]
    "VMLA.F32     q15, q10, d7[0]     \n\t" // column 3 += rhsPtr[8..11] * lhsPtr[12..15]

    "VMLA.F32     q12, q11, d1[1]     \n\t" // column 0 += rhsPtr[12..15] * lhsPtr[0..3]
    "VMLA.F32     q13, q11, d3[1]     \n\t" // column 1 += rhsPtr[12..15] * lhsPtr[4..7]
    "VMLA.F32     q14, q11, d5[1]     \n\t" // column 2 += rhsPtr[12..15] * lhsPtr[8..11]
    "VMLA.F32     q15, q11, d7[1]     \n\t" // column 3 += rhsPtr[12..15] * lhsPtr[12..15]
    "VSTM         %2,  {q12-q15}      \n\t" // store entire output matrix.
    : "+r"(rhsPtr), "+r"(lhsPtr), "+r"(temp)
    :
    : "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", "memory");

#endif
}

void MultiplyAssign(Dali::Matrix& result, const Dali::Matrix& rhs)
{
  MATH_INCREASE_COUNTER(PerformanceMonitor::MATRIX_MULTIPLYS);
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY, 64); // 64 = 16*4

  // TODO : Implement with NEON.

  float*       lhsPtr = result.AsFloat();
  const float* rhsPtr = rhs.AsFloat();
  float*       temp   = nullptr;

  if(lhsPtr == rhsPtr)
  {
    // If rhs is same matrix with result, we need to copy temperal vaules.
    temp = static_cast<float*>(malloc(NUM_BYTES_IN_MATRIX));
    memcpy(temp, rhsPtr, NUM_BYTES_IN_MATRIX);
    rhsPtr = temp;
  }

  // Calculate and store as row major.
  for(int32_t i = 0; i < 4; i++)
  {
    const int32_t loc0 = i;
    const int32_t loc1 = loc0 | 4;
    const int32_t loc2 = loc0 | 8;
    const int32_t loc3 = loc0 | 12;

    const float value0 = lhsPtr[loc0];
    const float value1 = lhsPtr[loc1];
    const float value2 = lhsPtr[loc2];
    const float value3 = lhsPtr[loc3];

    lhsPtr[loc0] = (value0 * rhsPtr[0]) +
                   (value1 * rhsPtr[1]) +
                   (value2 * rhsPtr[2]) +
                   (value3 * rhsPtr[3]);

    lhsPtr[loc1] = (value0 * rhsPtr[4]) +
                   (value1 * rhsPtr[5]) +
                   (value2 * rhsPtr[6]) +
                   (value3 * rhsPtr[7]);

    lhsPtr[loc2] = (value0 * rhsPtr[8]) +
                   (value1 * rhsPtr[9]) +
                   (value2 * rhsPtr[10]) +
                   (value3 * rhsPtr[11]);

    lhsPtr[loc3] = (value0 * rhsPtr[12]) +
                   (value1 * rhsPtr[13]) +
                   (value2 * rhsPtr[14]) +
                   (value3 * rhsPtr[15]);
  }

  if(temp)
  {
    // If we allocate temperal memory, we should free it.
    free(temp);
  }
}

// Dali::Matrix3

void Multiply(Dali::Matrix3& result, const Dali::Matrix3& lhs, const Dali::Matrix3& rhs)
{
  float*       temp   = result.AsFloat();
  const float* rhsPtr = rhs.AsFloat();
  const float* lhsPtr = lhs.AsFloat();

  for(int32_t i = 0; i < 3; i++)
  {
    const int32_t loc0 = i * 3;
    const int32_t loc1 = loc0 + 1;
    const int32_t loc2 = loc0 + 2;

    const float value0 = lhsPtr[loc0];
    const float value1 = lhsPtr[loc1];
    const float value2 = lhsPtr[loc2];

    temp[loc0] = (value0 * rhsPtr[0]) +
                 (value1 * rhsPtr[3]) +
                 (value2 * rhsPtr[6]);

    temp[loc1] = (value0 * rhsPtr[1]) +
                 (value1 * rhsPtr[4]) +
                 (value2 * rhsPtr[7]);

    temp[loc2] = (value0 * rhsPtr[2]) +
                 (value1 * rhsPtr[5]) +
                 (value2 * rhsPtr[8]);
  }
}

void MultiplyAssign(Dali::Matrix3& result, const Dali::Matrix3& rhs)
{
  float*       lhsPtr = result.AsFloat();
  const float* rhsPtr = rhs.AsFloat();
  float*       temp   = nullptr;

  if(lhsPtr == rhsPtr)
  {
    // If rhs is same matrix with result, we need to copy temperal vaules.
    temp = static_cast<float*>(malloc(NUM_BYTES_IN_MATRIX3));
    memcpy(temp, rhsPtr, NUM_BYTES_IN_MATRIX3);
    rhsPtr = temp;
  }

  // Calculate and store as row major.
  for(int32_t i = 0; i < 3; i++)
  {
    const int32_t loc0 = i;
    const int32_t loc1 = loc0 + 3;
    const int32_t loc2 = loc0 + 6;

    const float value0 = lhsPtr[loc0];
    const float value1 = lhsPtr[loc1];
    const float value2 = lhsPtr[loc2];

    lhsPtr[loc0] = (value0 * rhsPtr[0]) +
                   (value1 * rhsPtr[1]) +
                   (value2 * rhsPtr[2]);

    lhsPtr[loc1] = (value0 * rhsPtr[3]) +
                   (value1 * rhsPtr[4]) +
                   (value2 * rhsPtr[5]);

    lhsPtr[loc2] = (value0 * rhsPtr[6]) +
                   (value1 * rhsPtr[7]) +
                   (value2 * rhsPtr[8]);
  }

  if(temp)
  {
    // If we allocate temperal memory, we should free it.
    free(temp);
  }
}

} // namespace MatrixUtils
} // namespace Dali::Internal
