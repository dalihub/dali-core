/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/matrix.h>

// EXTERNAL INCLUDES
#include <cmath>
#include <cstring> // for memcpy
#include <ostream>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/internal/render/common/performance-monitor.h>

namespace
{
const float ROTATION_EPSILON = 0.003f; // Deliberately large

const size_t NUM_BYTES_IN_ROW_OF_3( 3 * sizeof( float ) );
const size_t NUM_BYTES_IN_ROW( 4 * sizeof( float ) );
const size_t NUM_BYTES_IN_MATRIX( 16 * sizeof( float ) );
const size_t ROW1_OFFSET( 4 );
const size_t ROW2_OFFSET( 8 );
const size_t ROW3_OFFSET( 12 );

/**
 * Helper to convert to Quaternion to float16 array
 */
void Convert( float*& m, const Dali::Quaternion& rotation )
{
  const float xx = rotation.mVector.x * rotation.mVector.x;
  const float yy = rotation.mVector.y * rotation.mVector.y;
  const float zz = rotation.mVector.z * rotation.mVector.z;
  const float xy = rotation.mVector.x * rotation.mVector.y;
  const float xz = rotation.mVector.x * rotation.mVector.z;
  const float wx = rotation.mVector.w * rotation.mVector.x;
  const float wy = rotation.mVector.w * rotation.mVector.y;
  const float wz = rotation.mVector.w * rotation.mVector.z;
  const float yz = rotation.mVector.y * rotation.mVector.z;

  m[0] = 1.0f - 2.0f * (yy + zz);
  m[1] =        2.0f * (xy + wz);
  m[2] =        2.0f * (xz - wy);
  m[3] = 0.0f;

  m[4] =        2.0f * (xy - wz);
  m[5] = 1.0f - 2.0f * (xx + zz);
  m[6] =        2.0f * (yz + wx);
  m[7] = 0.0f;

  m[8] =        2.0f * (xz + wy);
  m[9] =        2.0f * (yz - wx);
  m[10]= 1.0f - 2.0f * (xx + yy);
  m[11]= 0.0f;

  m[12]= 0.0f;
  m[13]= 0.0f;
  m[14]= 0.0f;
  m[15]= 1.0f;
}
}

namespace Dali
{

using Internal::PerformanceMonitor;

const float identityArray[] = {1.0f, 0.0f, 0.0f, 0.0f,
                               0.0f, 1.0f, 0.0f, 0.0f,
                               0.0f, 0.0f, 1.0f, 0.0f,
                               0.0f, 0.0f, 0.0f, 1.0f};

const Matrix Matrix::IDENTITY(identityArray);

Matrix::Matrix()
{
  memset( mMatrix, 0, NUM_BYTES_IN_MATRIX );
}

Matrix::Matrix( bool initialize )
{
  if( initialize )
  {
    memset( mMatrix, 0, NUM_BYTES_IN_MATRIX );
  }
}

Matrix::Matrix(const float* array)
{
  memcpy( mMatrix, array, NUM_BYTES_IN_MATRIX );
}

Matrix::Matrix( const Quaternion& rotation )
{
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,18);

  float* matrixPtr = &mMatrix[0];
  Convert( matrixPtr, rotation );
}

Matrix::Matrix( const Matrix& matrix )
{
  memcpy( mMatrix, matrix.mMatrix, NUM_BYTES_IN_MATRIX );
}

Matrix& Matrix::operator=( const Matrix& matrix )
{
  // no point copying if self assigning
  if( this != &matrix )
  {
    memcpy( mMatrix, matrix.mMatrix, NUM_BYTES_IN_MATRIX );
  }
  return *this;
}

void Matrix::InvertTransform(Matrix& result) const
{
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,12);

  float* m1 = result.AsFloat();

  DALI_ASSERT_ALWAYS( EqualsZero( mMatrix[3] ) && EqualsZero( mMatrix[7] ) && EqualsZero( mMatrix[11] ) && Equals( mMatrix[15], 1.0f ) && "Must be a transform matrix" );

  m1[0] = mMatrix[0];
  m1[1] = mMatrix[4];
  m1[2] = mMatrix[8];
  m1[3] = 0.0f;

  m1[4] = mMatrix[1];
  m1[5] = mMatrix[5];
  m1[6] = mMatrix[9];
  m1[7] = 0.0f;

  m1[8] = mMatrix[2];
  m1[9] = mMatrix[6];
  m1[10] = mMatrix[10];
  m1[11] = 0.0f;

  m1[12] = -( ( mMatrix[0] * mMatrix[12] ) + ( mMatrix[1] * mMatrix[13] ) + ( mMatrix[2] * mMatrix[14] ) + ( mMatrix[3] * mMatrix[15] ) );
  m1[13] = -( ( mMatrix[4] * mMatrix[12] ) + ( mMatrix[5] * mMatrix[13] ) + ( mMatrix[6] * mMatrix[14] ) + ( mMatrix[7] * mMatrix[15] ) );
  m1[14] = -( ( mMatrix[8] * mMatrix[12] ) + ( mMatrix[9] * mMatrix[13] ) + ( mMatrix[10] * mMatrix[14] ) + ( mMatrix[11] * mMatrix[15] ) );
  m1[15] = 1.0f;
}

static bool InvertMatrix(const float* m, float* out)
{
  float inv[16];

  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,192);  // 12 x 16 multiples

  inv[0] =   m[5]*m[10]*m[15] - m[5]*m[11]*m[14] - m[9]*m[6]*m[15] + m[9]*m[7]*m[14] + m[13]*m[6]*m[11] - m[13]*m[7]*m[10];
  inv[4] =  -m[4]*m[10]*m[15] + m[4]*m[11]*m[14] + m[8]*m[6]*m[15] - m[8]*m[7]*m[14] - m[12]*m[6]*m[11] + m[12]*m[7]*m[10];
  inv[8] =   m[4]*m[9]*m[15] - m[4]*m[11]*m[13] - m[8]*m[5]*m[15] + m[8]*m[7]*m[13] + m[12]*m[5]*m[11] - m[12]*m[7]*m[9];
  inv[12] = -m[4]*m[9]*m[14] + m[4]*m[10]*m[13] + m[8]*m[5]*m[14] - m[8]*m[6]*m[13] - m[12]*m[5]*m[10] + m[12]*m[6]*m[9];
  inv[1] =  -m[1]*m[10]*m[15] + m[1]*m[11]*m[14] + m[9]*m[2]*m[15] - m[9]*m[3]*m[14] - m[13]*m[2]*m[11] + m[13]*m[3]*m[10];
  inv[5] =   m[0]*m[10]*m[15] - m[0]*m[11]*m[14] - m[8]*m[2]*m[15] + m[8]*m[3]*m[14] + m[12]*m[2]*m[11] - m[12]*m[3]*m[10];
  inv[9] =  -m[0]*m[9]*m[15] + m[0]*m[11]*m[13] + m[8]*m[1]*m[15] - m[8]*m[3]*m[13] - m[12]*m[1]*m[11] + m[12]*m[3]*m[9];
  inv[13] =  m[0]*m[9]*m[14] - m[0]*m[10]*m[13] - m[8]*m[1]*m[14] + m[8]*m[2]*m[13] + m[12]*m[1]*m[10] - m[12]*m[2]*m[9];
  inv[2] =   m[1]*m[6]*m[15] - m[1]*m[7]*m[14] - m[5]*m[2]*m[15] + m[5]*m[3]*m[14] + m[13]*m[2]*m[7] - m[13]*m[3]*m[6];
  inv[6] =  -m[0]*m[6]*m[15] + m[0]*m[7]*m[14] + m[4]*m[2]*m[15] - m[4]*m[3]*m[14] - m[12]*m[2]*m[7] + m[12]*m[3]*m[6];
  inv[10] =  m[0]*m[5]*m[15] - m[0]*m[7]*m[13] - m[4]*m[1]*m[15] + m[4]*m[3]*m[13] + m[12]*m[1]*m[7] - m[12]*m[3]*m[5];
  inv[14] = -m[0]*m[5]*m[14] + m[0]*m[6]*m[13] + m[4]*m[1]*m[14] - m[4]*m[2]*m[13] - m[12]*m[1]*m[6] + m[12]*m[2]*m[5];
  inv[3] =  -m[1]*m[6]*m[11] + m[1]*m[7]*m[10] + m[5]*m[2]*m[11] - m[5]*m[3]*m[10] - m[9]*m[2]*m[7] + m[9]*m[3]*m[6];
  inv[7] =   m[0]*m[6]*m[11] - m[0]*m[7]*m[10] - m[4]*m[2]*m[11] + m[4]*m[3]*m[10] + m[8]*m[2]*m[7] - m[8]*m[3]*m[6];
  inv[11] = -m[0]*m[5]*m[11] + m[0]*m[7]*m[9] + m[4]*m[1]*m[11] - m[4]*m[3]*m[9] - m[8]*m[1]*m[7] + m[8]*m[3]*m[5];
  inv[15] =  m[0]*m[5]*m[10] - m[0]*m[6]*m[9] - m[4]*m[1]*m[10] + m[4]*m[2]*m[9] + m[8]*m[1]*m[6] - m[8]*m[2]*m[5];

  float det = m[0]*inv[0] + m[1]*inv[4] + m[2]*inv[8] + m[3]*inv[12];

  // In the case where the determinant is exactly zero, the matrix is non-invertible
  if ( EqualsZero( det ) )
  {
    return false;
  }

  det = 1.0 / det;

  for (int i = 0; i < 16; i++)
  {
    out[i] = inv[i] * det;
  }

  return true;
}

bool Matrix::Invert()
{
  Matrix temp(*this);

  return InvertMatrix(temp.AsFloat(), mMatrix);
}

void Matrix::Transpose()
{
  float temp = mMatrix[1];
  mMatrix[1] = mMatrix[4];
  mMatrix[4] = temp;

  temp = mMatrix[2];
  mMatrix[2] = mMatrix[8];
  mMatrix[8] = temp;

  temp = mMatrix[3];
  mMatrix[3] = mMatrix[12];
  mMatrix[12] = temp;

  temp = mMatrix[6];
  mMatrix[6] = mMatrix[9];
  mMatrix[9] = temp;

  temp = mMatrix[7];
  mMatrix[7] = mMatrix[13];
  mMatrix[13] = temp;

  temp = mMatrix[11];
  mMatrix[11] = mMatrix[14];
  mMatrix[14] = temp;
}

void Matrix::SetIdentity()
{
  memcpy( mMatrix, identityArray, NUM_BYTES_IN_MATRIX );
}

void Matrix::SetIdentityAndScale( const Vector3& scale )
{
  // initialize to zeros
  memset( mMatrix, 0, NUM_BYTES_IN_MATRIX );

  // just apply scale on the diagonal
  mMatrix[0]  = scale.x;
  mMatrix[5]  = scale.y;
  mMatrix[10] = scale.z;
  mMatrix[15] = 1.0f;
}

void Matrix::SetTranslation(const Vector4& translation)
{
  memcpy( mMatrix + ROW3_OFFSET, &translation, NUM_BYTES_IN_ROW );
}
void Matrix::SetTranslation(const Vector3& other)
{
  memcpy( mMatrix + ROW3_OFFSET, &other, NUM_BYTES_IN_ROW_OF_3 );
  mMatrix[15] = 1.0f;
}

void Matrix::Multiply( Matrix& result, const Matrix& lhs, const Matrix& rhs )
{
  MATH_INCREASE_COUNTER(PerformanceMonitor::MATRIX_MULTIPLYS);
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,64); // 64 = 16*4

  float* temp = result.AsFloat();
  const float* rhsPtr  = rhs.AsFloat();
  const float* lhsPtr = lhs.AsFloat();

#ifndef  __ARM_NEON__

  for( int i=0; i < 4; i++ )
  {
    // i<<2 gives the first vector / column
    int loc = i<<2;
    int loc1 = loc + 1;
    int loc2 = loc + 2;
    int loc3 = loc + 3;
    float value0 = lhsPtr[loc];
    float value1 = lhsPtr[loc1];
    float value2 = lhsPtr[loc2];
    float value3 = lhsPtr[loc3];
    temp[loc]  = (value0 * rhsPtr[0]) +
                 (value1 * rhsPtr[4]) +
                 (value2 * rhsPtr[8]) +
                 (value3 * rhsPtr[12]);

    temp[loc1] = (value0 * rhsPtr[1]) +
                 (value1 * rhsPtr[5]) +
                 (value2 * rhsPtr[9]) +
                 (value3 * rhsPtr[13]);

    temp[loc2] = (value0 * rhsPtr[2]) +
                 (value1 * rhsPtr[6]) +
                 (value2 * rhsPtr[10])+
                 (value3 * rhsPtr[14]);

    temp[loc3] = (value0 * rhsPtr[3]) +
                 (value1 * rhsPtr[7]) +
                 (value2 * rhsPtr[11])+
                 (value3 * rhsPtr[15]);
  }

#else

  // 64 32bit registers,
  // aliased to
  // d = 64 bit double-word d0 -d31
  // q =128 bit quad-word   q0 -q15  (enough to handle a column of 4 floats in a matrix)
  // e.g. q0 = d0 and d1

  // load and stores interleaved as NEON can load and store while calculating
  asm volatile ( "VLDM         %1,  {q0-q3}        \n\t"   // load matrix 1 (lhsPtr) q[0..q3]
                 "VLDM         %0,  {q8-q11}       \n\t"   // load matrix 2 (rhsPtr) q[q8-q11]
                 "VMUL.F32     q12, q8, d0[0]      \n\t"   // column 0 = rhsPtr[0..3] * lhsPtr[0..3]
                 "VMUL.F32     q13, q8, d2[0]      \n\t"   // column 1 = rhsPtr[0..3] * lhsPtr[4..7]
                 "VMUL.F32     q14, q8, d4[0]      \n\t"   // column 2 = rhsPtr[0..3] * lhsPtr[8..11]
                 "VMUL.F32     q15, q8, d6[0]      \n\t"   // column 3 = rhsPtr[0..3] * lhsPtr[12..15]

                 "VMLA.F32     q12, q9, d0[1]      \n\t"   // column 0 += rhsPtr[4..7] * lhsPtr[0..3]
                 "VMLA.F32     q13, q9, d2[1]      \n\t"   // column 1 += rhsPtr[4..7] * lhsPtr[4..7]
                 "VMLA.F32     q14, q9, d4[1]      \n\t"   // column 2 += rhsPtr[4..7] * lhsPtr[8..11]
                 "VMLA.F32     q15, q9, d6[1]      \n\t"   // column 3 += rhsPtr[4..7] * lhsPtr[12..15]

                 "VMLA.F32     q12, q10, d1[0]     \n\t"   // column 0 += rhsPtr[8..11] * lhsPtr[0..3]
                 "VMLA.F32     q13, q10, d3[0]     \n\t"   // column 1 += rhsPtr[8..11] * lhsPtr[4..7]
                 "VMLA.F32     q14, q10, d5[0]     \n\t"   // column 2 += rhsPtr[8..11] * lhsPtr[8..11]
                 "VMLA.F32     q15, q10, d7[0]     \n\t"   // column 3 += rhsPtr[8..11] * lhsPtr[12..15]

                 "VMLA.F32     q12, q11, d1[1]     \n\t"   // column 0 += rhsPtr[12..15] * lhsPtr[0..3]
                 "VMLA.F32     q13, q11, d3[1]     \n\t"   // column 1 += rhsPtr[12..15] * lhsPtr[4..7]
                 "VMLA.F32     q14, q11, d5[1]     \n\t"   // column 2 += rhsPtr[12..15] * lhsPtr[8..11]
                 "VMLA.F32     q15, q11, d7[1]     \n\t"   // column 3 += rhsPtr[12..15] * lhsPtr[12..15]
                 "VSTM         %2,  {q12-q15}      \n\t"   // store entire output matrix.
                 : "+r"(rhsPtr), "+r"(lhsPtr), "+r"(temp)
                 :
                 : "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15", "memory" );

#endif
}

void Matrix::Multiply( Matrix& result, const Matrix& lhs, const Quaternion& rhs )
{
  MATH_INCREASE_COUNTER(PerformanceMonitor::MATRIX_MULTIPLYS);
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,54); // 54 = 36+18

  float matrix[16];
  float* rhsPtr = &matrix[0];
  Convert( rhsPtr, rhs );

  // quaternion contains just rotation so it really only needs 3x3 matrix

  float* temp = result.AsFloat();
  const float* lhsPtr = lhs.AsFloat();

#ifndef  __ARM_NEON__

  for( int i=0; i < 4; i++ )
  {
    // i<<2 gives the first vector / column
    int loc = i<<2;
    int loc1 = loc + 1;
    int loc2 = loc + 2;
    int loc3 = loc + 3;
    float value0 = lhsPtr[loc];
    float value1 = lhsPtr[loc1];
    float value2 = lhsPtr[loc2];
    float value3 = lhsPtr[loc3];
    temp[loc]  = (value0 * rhsPtr[0]) +
                 (value1 * rhsPtr[4]) +
                 (value2 * rhsPtr[8]) +
                 (0.0f); //value3 * rhsPtr[12] is 0.0f

    temp[loc1] = (value0 * rhsPtr[1]) +
                 (value1 * rhsPtr[5]) +
                 (value2 * rhsPtr[9]) +
                 (0.0f); //value3 * rhsPtr[13] is 0.0f

    temp[loc2] = (value0 * rhsPtr[2]) +
                 (value1 * rhsPtr[6]) +
                 (value2 * rhsPtr[10])+
                 (0.0f); //value3 * rhsPtr[14] is 0.0f

    temp[loc3] = (0.0f) + //value0 * rhsPtr[3] is 0.0f
                 (0.0f) + //value1 * rhsPtr[7] is 0.0f
                 (0.0f) + //value2 * rhsPtr[11] is 0.0f
                 (value3); // rhsPtr[15] is 1.0f
  }

#else

  // 64 32bit registers,
  // aliased to
  // d = 64 bit double-word d0 -d31
  // q =128 bit quad-word   q0 -q15  (enough to handle a column of 4 floats in a matrix)
  // e.g. q0 = d0 and d1
  // load and stores interleaved as NEON can load and store while calculating
  asm volatile ( "VLDM         %1,   {q4-q6}       \n\t" // load matrix 1 (lhsPtr)
                 "VLD1.F32     {q7}, [%2]!         \n\t" // load matrix 2 (rhsPtr) [0..3]
                 "VMUL.F32     q0,   q7,   d8[0]   \n\t" // column 0 = rhsPtr[0..3] * lhsPtr[0..3]
                 "VMUL.F32     q1,   q7,   d10[0]  \n\t" // column 1 = rhsPtr[0..3] * lhsPtr[4..7]
                 "VMUL.F32     q2,   q7,   d12[0]  \n\t" // column 2 = rhsPtr[0..3] * lhsPtr[8..11]
                 "VLD1.F32     {q7}, [%2]!         \n\t" // load matrix 2 (rhsPtr) [4..7]
                 "VMLA.F32     q0,   q7,   d8[1]   \n\t" // column 0+= rhsPtr[4..7] * lhsPtr[0..3]
                 "VMLA.F32     q1,   q7,   d10[1]  \n\t" // column 1+= rhsPtr[4..7] * lhsPtr[4..7]
                 "VMLA.F32     q2,   q7,   d12[1]  \n\t" // column 2+= rhsPtr[4..7] * lhsPtr[8..11]
                 "VLD1.F32     {q7}, [%2]!         \n\t" // load matrix 2 (rhsPtr) [8..11]
                 "VMLA.F32     q0,   q7,   d9[0]   \n\t" // column 0+= rhsPtr[8..11] * lhsPtr[0..3]
                 "VMLA.F32     q1,   q7,   d11[0]  \n\t" // column 1+= rhsPtr[8..11] * lhsPtr[4..7]
                 "VMLA.F32     q2,   q7,   d13[0]  \n\t" // column 2+= rhsPtr[8..11] * lhsPtr[8..11]
                 "VSTM         %0,   {q0-q2}       \n\t" // store entire output matrix.
                 :
                 : "r"(temp), "r"(lhsPtr), "r" (rhsPtr)
                 : "%r0", "%q0", "%q1", "%q2", "%q4", "%q5", "%q6", "%q7", "memory" );

  temp[ 12 ] = 0.0f;
  temp[ 13 ] = 0.0f;
  temp[ 14 ] = 0.0f;
  temp[ 15 ] = 1.0f;
#endif
}

Vector4 Matrix::operator*(const Vector4& rhs) const
{
  MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,16);

  Vector4 temp;

#ifndef  __ARM_NEON__

  temp.x = rhs.x * mMatrix[0] + rhs.y * mMatrix[4] + rhs.z * mMatrix[8]  +  rhs.w * mMatrix[12];
  temp.y = rhs.x * mMatrix[1] + rhs.y * mMatrix[5] + rhs.z * mMatrix[9]  +  rhs.w * mMatrix[13];
  temp.z = rhs.x * mMatrix[2] + rhs.y * mMatrix[6] + rhs.z * mMatrix[10] +  rhs.w * mMatrix[14];
  temp.w = rhs.x * mMatrix[3] + rhs.y * mMatrix[7] + rhs.z * mMatrix[11] +  rhs.w * mMatrix[15];

#else

  // 64 32bit registers,
  // aliased to
  // d = 64 bit double-word d0 -d31
  // q =128 bit quad-word   q0 -q15  (enough to handle a column of 4 floats in a matrix)
  // e.g. q0 = d0 and d1
  // load and stores interleaved as NEON can load and store while calculating
  asm volatile ( "VLD1.F32     {q0}, [%1]        \n\t"   //q0 = rhs
                 "VLD1.F32     {q9}, [%0]!       \n\t"
                 "VMUL.F32     q10,  q9,   d0[0] \n\t"
                 "VLD1.F32     {q9}, [%0]!       \n\t"
                 "VMLA.F32     q10,  q9,   d0[1] \n\t"   //q10 = mMatrix[0..3] * rhs + mMatrix[4..7] * rhs
                 "VLD1.F32     {q9}, [%0]!       \n\t"
                 "VMUL.F32     q11,  q9,   d1[0] \n\t"
                 "VLD1.F32     {q9}, [%0]!       \n\t"
                 "VMLA.F32     q11,  q9,   d1[1] \n\t"   //q11 = mMatrix[8..11] * rhs + mMatrix[12..15] * rhs
                 "VADD.F32     q10,  q10,  q11   \n\t"
                 "VST1.F32     {q10},[%2]        \n\t"   //temp = q10 + q11
                 :
                 : "r"(mMatrix), "r"(&rhs), "r"(&temp)
                 : "q0", "q9", "q10", "q11", "memory" );
#endif
  return temp;
}

bool Matrix::operator==(const Matrix& rhs) const
{
  return (
  ( fabsf( mMatrix[0] - rhs.mMatrix[0] ) <= GetRangedEpsilon( mMatrix[0], rhs.mMatrix[0] ) ) &&
  ( fabsf( mMatrix[1] - rhs.mMatrix[1] ) <= GetRangedEpsilon( mMatrix[1], rhs.mMatrix[1] ) ) &&
  ( fabsf( mMatrix[2] - rhs.mMatrix[2] ) <= GetRangedEpsilon( mMatrix[2], rhs.mMatrix[2] ) ) &&
  ( fabsf( mMatrix[3] - rhs.mMatrix[3] ) <= GetRangedEpsilon( mMatrix[3], rhs.mMatrix[3] ) ) &&
  ( fabsf( mMatrix[4] - rhs.mMatrix[4] ) <= GetRangedEpsilon( mMatrix[4], rhs.mMatrix[4] ) ) &&
  ( fabsf( mMatrix[5] - rhs.mMatrix[5] ) <= GetRangedEpsilon( mMatrix[5], rhs.mMatrix[5] ) ) &&
  ( fabsf( mMatrix[6] - rhs.mMatrix[6] ) <= GetRangedEpsilon( mMatrix[6], rhs.mMatrix[6] ) ) &&
  ( fabsf( mMatrix[7] - rhs.mMatrix[7] ) <= GetRangedEpsilon( mMatrix[7], rhs.mMatrix[7] ) ) &&
  ( fabsf( mMatrix[8] - rhs.mMatrix[8] ) <= GetRangedEpsilon( mMatrix[8], rhs.mMatrix[8] ) ) &&
  ( fabsf( mMatrix[9] - rhs.mMatrix[9] ) <= GetRangedEpsilon( mMatrix[9], rhs.mMatrix[9] ) ) &&
  ( fabsf( mMatrix[10] - rhs.mMatrix[10] ) <= GetRangedEpsilon( mMatrix[10], rhs.mMatrix[10] ) ) &&
  ( fabsf( mMatrix[11] - rhs.mMatrix[11] ) <= GetRangedEpsilon( mMatrix[11], rhs.mMatrix[11] ) ) &&
  ( fabsf( mMatrix[12] - rhs.mMatrix[12] ) <= GetRangedEpsilon( mMatrix[12], rhs.mMatrix[12] ) ) &&
  ( fabsf( mMatrix[13] - rhs.mMatrix[13] ) <= GetRangedEpsilon( mMatrix[13], rhs.mMatrix[13] ) ) &&
  ( fabsf( mMatrix[14] - rhs.mMatrix[14] ) <= GetRangedEpsilon( mMatrix[14], rhs.mMatrix[14] ) ) &&
  ( fabsf( mMatrix[15] - rhs.mMatrix[15] ) <= GetRangedEpsilon( mMatrix[15], rhs.mMatrix[15] ) ) );
}

bool Matrix::operator!=(const Matrix& rhs) const
{
  if (*this == rhs)
  {
    return false;
  }

  return true;
}

void Matrix::OrthoNormalize()
{
  Vector4 vector0(GetXAxis());
  Vector4 vector1(GetYAxis());
  Vector4 vector2(GetZAxis());

  vector0.Normalize();
  vector1.Normalize();
  vector2 = vector0.Cross( vector1 );
  vector1 = vector2.Cross( vector0 );

  memcpy( mMatrix, &vector0, NUM_BYTES_IN_ROW );
  memcpy( mMatrix + ROW1_OFFSET, &vector1, NUM_BYTES_IN_ROW );
  memcpy( mMatrix + ROW2_OFFSET, &vector2, NUM_BYTES_IN_ROW );
}

Vector3 Matrix::GetXAxis() const
{
  return Vector3(mMatrix[0], mMatrix[1], mMatrix[2]);
}

Vector3 Matrix::GetYAxis() const
{
  return Vector3(mMatrix[4], mMatrix[5], mMatrix[6]);
}

Vector3 Matrix::GetZAxis() const
{
  return Vector3(mMatrix[8], mMatrix[9], mMatrix[10]);
}

void Matrix::SetXAxis(const Vector3& axis)
{
  mMatrix[0] = axis.x;
  mMatrix[1] = axis.y;
  mMatrix[2] = axis.z;
}

void Matrix::SetYAxis(const Vector3& axis)
{
  mMatrix[4] = axis.x;
  mMatrix[5] = axis.y;
  mMatrix[6] = axis.z;
}

void Matrix::SetZAxis(const Vector3& axis)
{
  mMatrix[8] = axis.x;
  mMatrix[9] = axis.y;
  mMatrix[10] = axis.z;
}

void Matrix::SetTransformComponents(const Vector3&    scale,
                                    const Quaternion& rotation,
                                    const Vector3&    translation )
{
  if( rotation.IsIdentity() )
  {
    mMatrix[0] = scale.x;
    mMatrix[1] = 0.0f;
    mMatrix[2] = 0.0f;
    mMatrix[3] = 0.0f;

    mMatrix[4] = 0.0f;
    mMatrix[5] = scale.y;
    mMatrix[6] = 0.0f;
    mMatrix[7] = 0.0f;

    mMatrix[8] = 0.0f;
    mMatrix[9] = 0.0f;
    mMatrix[10]= scale.z;
    mMatrix[11]= 0.0f;
  }
  else
  {
    MATH_INCREASE_COUNTER(PerformanceMonitor::MATRIX_MULTIPLYS);
    MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,27); // 27 = 9+18

    const float xx = rotation.mVector.x * rotation.mVector.x;
    const float yy = rotation.mVector.y * rotation.mVector.y;
    const float zz = rotation.mVector.z * rotation.mVector.z;
    const float xy = rotation.mVector.x * rotation.mVector.y;
    const float xz = rotation.mVector.x * rotation.mVector.z;
    const float wx = rotation.mVector.w * rotation.mVector.x;
    const float wy = rotation.mVector.w * rotation.mVector.y;
    const float wz = rotation.mVector.w * rotation.mVector.z;
    const float yz = rotation.mVector.y * rotation.mVector.z;

    mMatrix[0] = (scale.x * (1.0f - 2.0f * (yy + zz)));
    mMatrix[1] = (scale.x * (       2.0f * (xy + wz)));
    mMatrix[2] = (scale.x * (       2.0f * (xz - wy)));
    mMatrix[3] = 0.0f;

    mMatrix[4] = (scale.y * (       2.0f * (xy - wz)));
    mMatrix[5] = (scale.y * (1.0f - 2.0f * (xx + zz)));
    mMatrix[6] = (scale.y * (       2.0f * (yz + wx)));
    mMatrix[7] = 0.0f;

    mMatrix[8] = (scale.z * (       2.0f * (xz + wy)));
    mMatrix[9] = (scale.z * (       2.0f * (yz - wx)));
    mMatrix[10]= (scale.z * (1.0f - 2.0f * (xx + yy)));
    mMatrix[11]= 0.0f;
  }
  // apply translation
  mMatrix[12] = translation.x;
  mMatrix[13] = translation.y;
  mMatrix[14] = translation.z;
  mMatrix[15] = 1.0f;
}

void Matrix::SetInverseTransformComponents(const Vector3&    scale,
                                           const Quaternion& rotation,
                                           const Vector3&    translation )
{
  Vector3 inverseTranslation = -translation;
  Vector3 inverseScale( 1.0f/scale.x, 1.0f/scale.y, 1.0f/scale.z);
  Quaternion inverseRotation(rotation);
  bool isRotated = ! inverseRotation.IsIdentity();

  // Order of application is translation, rotation, scale.
  // Ensure translation is relative to scale & rotation:

  if( isRotated )
  {
    inverseRotation.Invert();
    inverseTranslation = inverseRotation.Rotate(inverseTranslation);
  }

  inverseTranslation *= inverseScale;

  if( isRotated )
  {
    MATH_INCREASE_COUNTER(PerformanceMonitor::MATRIX_MULTIPLYS);
    MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,27); // 27 = 9+18

    const float xx = inverseRotation.mVector.x * inverseRotation.mVector.x;
    const float yy = inverseRotation.mVector.y * inverseRotation.mVector.y;
    const float zz = inverseRotation.mVector.z * inverseRotation.mVector.z;
    const float xy = inverseRotation.mVector.x * inverseRotation.mVector.y;
    const float xz = inverseRotation.mVector.x * inverseRotation.mVector.z;
    const float wx = inverseRotation.mVector.w * inverseRotation.mVector.x;
    const float wy = inverseRotation.mVector.w * inverseRotation.mVector.y;
    const float wz = inverseRotation.mVector.w * inverseRotation.mVector.z;
    const float yz = inverseRotation.mVector.y * inverseRotation.mVector.z;

    mMatrix[0] = (inverseScale.x * (1.0f - 2.0f * (yy + zz)));
    mMatrix[1] = (inverseScale.y * (2.0f * (xy + wz)));
    mMatrix[2] = (inverseScale.z * (2.0f * (xz - wy)));
    mMatrix[3] = 0.0f;

    mMatrix[4] = (inverseScale.x * (2.0f * (xy - wz)));
    mMatrix[5] = (inverseScale.y * (1.0f - 2.0f * (xx + zz)));
    mMatrix[6] = (inverseScale.z * (2.0f * (yz + wx)));
    mMatrix[7] = 0.0f;

    mMatrix[8] = (inverseScale.x * (2.0f * (xz + wy)));
    mMatrix[9] = (inverseScale.y * (2.0f * (yz - wx)));
    mMatrix[10]= (inverseScale.z * (1.0f - 2.0f * (xx + yy)));
    mMatrix[11]= 0.0f;
  }
  else
  {
    mMatrix[0] = inverseScale.x;
    mMatrix[1] = 0.0f;
    mMatrix[2] = 0.0f;
    mMatrix[3] = 0.0f;

    mMatrix[4] = 0.0f;
    mMatrix[5] = inverseScale.y;
    mMatrix[6] = 0.0f;
    mMatrix[7] = 0.0f;

    mMatrix[8] = 0.0f;
    mMatrix[9] = 0.0f;
    mMatrix[10]= inverseScale.z;
    mMatrix[11]= 0.0f;
  }

  // apply translation
  mMatrix[12] = inverseTranslation.x;
  mMatrix[13] = inverseTranslation.y;
  mMatrix[14] = inverseTranslation.z;
  mMatrix[15] = 1.0f;
}

void Matrix::SetInverseTransformComponents(const Vector3&    xAxis,
                                           const Vector3&    yAxis,
                                           const Vector3&    zAxis,
                                           const Vector3&    translation )
{
  // x, y, z axis parameters represent a orthonormal basis with no scaling, i.e. a rotation matrix.
  // Invert rotation by transposing in place

  // Order of application is translation, rotation

  mMatrix[0]  = xAxis.x;
  mMatrix[1]  = yAxis.x;
  mMatrix[2]  = zAxis.x;
  mMatrix[3]  = 0.0f;

  mMatrix[4]  = xAxis.y;
  mMatrix[5]  = yAxis.y;
  mMatrix[6]  = zAxis.y;
  mMatrix[7]  = 0.0f;

  mMatrix[8]  = xAxis.z;
  mMatrix[9]  = yAxis.z;
  mMatrix[10] = zAxis.z;
  mMatrix[11] = 0.0f;
  mMatrix[12] = 0.0f;
  mMatrix[13] = 0.0f;
  mMatrix[14] = 0.0f;
  mMatrix[15] = 1.0f;

  // Ensure translation is relative to scale & rotation:

  Vector4 inverseTranslation( -translation.x, -translation.y, -translation.z, 1.0f);
  inverseTranslation = *this * inverseTranslation; // Rotate inverse translation
  inverseTranslation.w = 1.0f;
  SetTranslation(inverseTranslation);
}


void Matrix::GetTransformComponents(Vector3&     position,
                                    Quaternion&  rotation,
                                    Vector3&     scale) const
{
  position = GetTranslation3();

  // Derive scale from axis lengths.
  Vector3 theScale(GetXAxis().Length(), GetYAxis().Length(), GetZAxis().Length());
  scale = theScale;

  if( ! ( fabs(theScale.x - Vector3::ONE.x) < ROTATION_EPSILON &&
          fabs(theScale.y - Vector3::ONE.y) < ROTATION_EPSILON &&
          fabs(theScale.z - Vector3::ONE.z) < ROTATION_EPSILON ) )
  {
    MATH_INCREASE_COUNTER(PerformanceMonitor::MATRIX_MULTIPLYS);
    MATH_INCREASE_BY(PerformanceMonitor::FLOAT_POINT_MULTIPLY,9);

    // Non-identity scale is embedded into rotation matrix. Remove it first:
    Matrix m(*this);
    Vector3 inverseScale(1.0f/theScale.x, 1.0f/theScale.y, 1.0f/theScale.z);
    m.mMatrix[0] *= inverseScale.x;
    m.mMatrix[1] *= inverseScale.x;
    m.mMatrix[2] *= inverseScale.x;
    m.mMatrix[4] *= inverseScale.y;
    m.mMatrix[5] *= inverseScale.y;
    m.mMatrix[6] *= inverseScale.y;
    m.mMatrix[8] *= inverseScale.z;
    m.mMatrix[9] *= inverseScale.z;
    m.mMatrix[10] *= inverseScale.z;

    Quaternion theRotation(m);

    // If the imaginary components are close to zero, then use null quaternion instead.
    if( fabs(theRotation.mVector.x) < ROTATION_EPSILON &&
        fabs(theRotation.mVector.y) < ROTATION_EPSILON &&
        fabs(theRotation.mVector.z) < ROTATION_EPSILON )
    {
      theRotation = Quaternion();
    }
    rotation = theRotation;
  }
  else
  {
    Quaternion theRotation(*this);

    // If the imaginary components are close to zero, then use null quaternion instead.
    if( fabs(theRotation.mVector.x) < ROTATION_EPSILON &&
        fabs(theRotation.mVector.y) < ROTATION_EPSILON &&
        fabs(theRotation.mVector.z) < ROTATION_EPSILON )
    {
      theRotation = Quaternion();
    }
    rotation = theRotation;
  }
}



std::ostream& operator<< (std::ostream& o, const Matrix& matrix)
{
  return o << "[ [" << matrix.mMatrix[0] << ", " << matrix.mMatrix[1] << ", " << matrix.mMatrix[2]  << ", " << matrix.mMatrix[3] << "], "
             << "[" << matrix.mMatrix[4] << ", " << matrix.mMatrix[5] << ", " << matrix.mMatrix[6]  << ", " << matrix.mMatrix[7] << "], "
             << "[" << matrix.mMatrix[8] << ", " << matrix.mMatrix[9] << ", " << matrix.mMatrix[10] << ", " << matrix.mMatrix[11] << "], "
             << "[" << matrix.mMatrix[12] << ", " << matrix.mMatrix[13] << ", " << matrix.mMatrix[14] << ", " << matrix.mMatrix[15] << "] ]";
}

} // namespace Dali
