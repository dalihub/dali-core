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

// CLASS HEADER
#include <dali/internal/common/math.h>

//EXTERNAL INCLUDES
#include <cmath>

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

float Dali::Internal::Length( const Vec3 v )
{
  return sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

void Dali::Internal::MultiplyVectorBySize( Vec2& result, const Vec2 v, const Size3 s )
{
  result[0] = v[0] * s[0];
  result[1] = v[1] * s[1];
}

void Dali::Internal::MultiplyVectorBySize( Vec3& result, const Vec3 v, const Size3 s )
{
  result[0] = v[0] * s[0];
  result[1] = v[1] * s[1];
  result[2] = v[2] * s[2];
}

void Dali::Internal::MultiplyVectorBySize( Vec4& result, const Vec4 v, const Size3 s )
{
  result[0] = v[0] * s[0];
  result[1] = v[1] * s[1];
  result[2] = v[2] * s[2];
  result[3] = 1.0f;
}

void Dali::Internal::MultiplyVectorByMatrix4( Vec2& result, const Mat4 m, const Vec2 v )
{
  result[0] = v[0] * m[0] + v[1] * m[4] + m[12];
  result[1] = v[0] * m[1] + v[1] * m[5] + m[13];
}

void Dali::Internal::MultiplyVectorByMatrix4( Vec3& result, const Mat4 m, const Vec3 v )
{
  result[0] = v[0] * m[0] + v[1] * m[4] + v[2] * m[8]  +  m[12];
  result[1] = v[0] * m[1] + v[1] * m[5] + v[2] * m[9]  +  m[13];
  result[2] = v[0] * m[2] + v[1] * m[6] + v[2] * m[10] +  m[14];
}

void Dali::Internal::MultiplyVectorByMatrix4( Vec4& result, const Mat4 m, const Vec4 rhs )
{
  result[0] = rhs[0] * m[0] + rhs[1] * m[4] + rhs[2] * m[8]  +  rhs[3] * m[12];
  result[1] = rhs[0] * m[1] + rhs[1] * m[5] + rhs[2] * m[9]  +  rhs[3] * m[13];
  result[2] = rhs[0] * m[2] + rhs[1] * m[6] + rhs[2] * m[10] +  rhs[3] * m[14];
  result[3] = rhs[0] * m[3] + rhs[1] * m[7] + rhs[2] * m[11] +  rhs[3] * m[15];
}

void Dali::Internal::MultiplyMatrices( float* result, const Mat4 lhs, const Mat4 rhs )
{
  for( int i=0; i < 4; i++ )
  {
    // i<<2 gives the first vector / column
    int loc = i<<2;
    int loc1 = loc + 1;
    int loc2 = loc + 2;
    int loc3 = loc + 3;
    float value0 = lhs[loc];
    float value1 = lhs[loc1];
    float value2 = lhs[loc2];
    float value3 = lhs[loc3];
    result[loc]  = (value0 * rhs[0]) +
                   (value1 * rhs[4]) +
                   (value2 * rhs[8]) +
                   (value3 * rhs[12]);

    result[loc1] = (value0 * rhs[1]) +
                   (value1 * rhs[5]) +
                   (value2 * rhs[9]) +
                   (value3 * rhs[13]);

    result[loc2] = (value0 * rhs[2]) +
                   (value1 * rhs[6]) +
                   (value2 * rhs[10])+
                   (value3 * rhs[14]);

    result[loc3] = (value0 * rhs[3]) +
                   (value1 * rhs[7]) +
                   (value2 * rhs[11])+
                   (value3 * rhs[15]);
  }
}
