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

// CLASS HEADER
#include <dali/public-api/math/matrix3.h>

// EXTERNAL INCLUDES
#include <string.h>

// INTERNAL INCLUDES
#include <dali/public-api/math/math-utils.h>

#define S00 0
#define S01 1
#define S02 2
#define S10 3
#define S11 4
#define S12 5
#define S20 6
#define S21 7
#define S22 8

/*
 * S00 S01 S02
 * S10 S11 S12
 * S20 S21 S22
 */

namespace
{
const size_t NUM_BYTES_IN_ROW    = 3*sizeof(float);
const size_t NUM_BYTES_IN_MATRIX = 9*sizeof(float);
}

namespace Dali
{

const Matrix3 Matrix3::IDENTITY(1.0f, 0.0f, 0.0f,
                                0.0f, 1.0f, 0.0f,
                                0.0f, 0.0f, 1.0f);

Matrix3::Matrix3()
{
  float* m = AsFloat();
  memset(m, 0, NUM_BYTES_IN_MATRIX);
  mElements[S00]=1.0f;
  mElements[S11]=1.0f;
  mElements[S22]=1.0f;
}

Matrix3::Matrix3(const Matrix3& m)
{
  memcpy( mElements, m.mElements, NUM_BYTES_IN_MATRIX );
}

Matrix3::Matrix3(const Matrix& matrix)
{
  const float* m4 = matrix.AsFloat();
  memcpy(&mElements[S00], m4,   NUM_BYTES_IN_ROW);
  memcpy(&mElements[S10], m4+4, NUM_BYTES_IN_ROW);
  memcpy(&mElements[S20], m4+8, NUM_BYTES_IN_ROW);
}

Matrix3::Matrix3(float s00, float s01, float s02, float s10, float s11, float s12, float s20, float s21, float s22)
{
  mElements[S00] = s00;
  mElements[S01] = s01;
  mElements[S02] = s02;
  mElements[S10] = s10;
  mElements[S11] = s11;
  mElements[S12] = s12;
  mElements[S20] = s20;
  mElements[S21] = s21;
  mElements[S22] = s22;
}


void Matrix3::SetIdentity()
{
  memset(mElements, 0, NUM_BYTES_IN_MATRIX);
  mElements[S00]=1.0f;
  mElements[S11]=1.0f;
  mElements[S22]=1.0f;
}

Matrix3& Matrix3::operator=( const Matrix3& matrix )
{
  // no point copying if self assigning
  if( this != &matrix )
  {
    memcpy( AsFloat(), matrix.AsFloat(), NUM_BYTES_IN_MATRIX );
  }
  return *this;
}

Matrix3& Matrix3::operator=( const Matrix& matrix )
{
  const float* m4 = matrix.AsFloat();
  memcpy(&mElements[S00], m4,   NUM_BYTES_IN_ROW);
  memcpy(&mElements[S10], m4+4, NUM_BYTES_IN_ROW);
  memcpy(&mElements[S20], m4+8, NUM_BYTES_IN_ROW);
  return *this;
}

bool Matrix3::Invert()
{
  bool succeeded = false;

  float cof[9];
  cof[S00] = (mElements[S11] * mElements[S22] - mElements[S12] * mElements[S21]);
  cof[S01] = (mElements[S02] * mElements[S21] - mElements[S01] * mElements[S22]);
  cof[S02] = (mElements[S01] * mElements[S12] - mElements[S02] * mElements[S11]);

  cof[S10] = (mElements[S12] * mElements[S20] - mElements[S10] * mElements[S22]);
  cof[S11] = (mElements[S00] * mElements[S22] - mElements[S02] * mElements[S20]);
  cof[S12] = (mElements[S02] * mElements[S10] - mElements[S00] * mElements[S12]);

  cof[S20] = (mElements[S10] * mElements[S21] - mElements[S11] * mElements[S20]);
  cof[S21] = (mElements[S01] * mElements[S20] - mElements[S00] * mElements[S21]);
  cof[S22] = (mElements[S00] * mElements[S11] - mElements[S01] * mElements[S10]);

  float det = mElements[S00] * cof[S00] + mElements[S01] * cof[S10] + mElements[S02] * cof[S20];

  // In the case where the determinant is exactly zero, the matrix is non-invertible
  if( ! EqualsZero( det ) )
  {
    det = 1.0 / det;
    for (int i = 0; i < 9; i++)
    {
      mElements[i] = cof[i] * det;
    }
    succeeded = true;
  }
  return succeeded;
}

bool Matrix3::Transpose()
{
  float tmp;
  tmp = mElements[S01]; mElements[S01] = mElements[S10]; mElements[S10]=tmp;
  tmp = mElements[S02]; mElements[S02] = mElements[S20]; mElements[S20]=tmp;
  tmp = mElements[S21]; mElements[S21] = mElements[S12]; mElements[S12]=tmp;
  return true;
}

bool Matrix3::ScaledInverseTranspose()
{
  bool succeeded = false;

  float cof[9];
  cof[S00] = (mElements[S11] * mElements[S22] - mElements[S12] * mElements[S21]);
  cof[S01] = (mElements[S02] * mElements[S21] - mElements[S01] * mElements[S22]);
  cof[S02] = (mElements[S01] * mElements[S12] - mElements[S02] * mElements[S11]);

  cof[S10] = (mElements[S12] * mElements[S20] - mElements[S10] * mElements[S22]);
  cof[S11] = (mElements[S00] * mElements[S22] - mElements[S02] * mElements[S20]);
  cof[S12] = (mElements[S02] * mElements[S10] - mElements[S00] * mElements[S12]);

  cof[S20] = (mElements[S10] * mElements[S21] - mElements[S11] * mElements[S20]);
  cof[S21] = (mElements[S01] * mElements[S20] - mElements[S00] * mElements[S21]);
  cof[S22] = (mElements[S00] * mElements[S11] - mElements[S01] * mElements[S10]);

  float det = mElements[S00] * cof[S00] + mElements[S01] * cof[S10] + mElements[S02] * cof[S20];

  // In the case where the determinant is exactly zero, the matrix is non-invertible
  if( ! EqualsZero( det ) )
  {
    // Use average rather than determinant to remove rounding to zero errors in further multiplication
    float sum=0;
    for(size_t i=0;i<9;i++)
    {
      sum+=fabsf(cof[i]);
    }
    float scale = 9.0f/sum; // Inverse of the average values
    if (det < 0)
    {
      // Ensure the signs of the inverse are correct
      scale = -scale;
    }

    mElements[S00] = cof[S00] * scale;
    mElements[S01] = cof[S10] * scale;
    mElements[S02] = cof[S20] * scale;

    mElements[S10] = cof[S01] * scale;
    mElements[S11] = cof[S11] * scale;
    mElements[S12] = cof[S21] * scale;

    mElements[S20] = cof[S02] * scale;
    mElements[S21] = cof[S12] * scale;
    mElements[S22] = cof[S22] * scale;

    succeeded = true;
  }
  return succeeded;
}

void Matrix3::Scale(float scale)
{
  mElements[S00] *= scale;
  mElements[S01] *= scale;
  mElements[S02] *= scale;
  mElements[S10] *= scale;
  mElements[S11] *= scale;
  mElements[S12] *= scale;
  mElements[S20] *= scale;
  mElements[S21] *= scale;
  mElements[S22] *= scale;
}

float Matrix3::Magnitude() const
{
  float avg=0;
  for(size_t i=0;i<9;i++)
  {
    avg+=fabsf(mElements[i]);
  }
  return avg/3.0f;
}


void Matrix3::Multiply( Matrix3& result, const Matrix3& lhs, const Matrix3& rhs )
{
  float* temp = result.AsFloat();
  const float* rhsPtr  = rhs.AsFloat();
  const float* lhsPtr = lhs.AsFloat();

  for( int i=0; i < 3; i++ )
  {
    int loc = i * 3;
    int loc1 = loc + 1;
    int loc2 = loc + 2;

    float value0 = lhsPtr[loc];
    float value1 = lhsPtr[loc1];
    float value2 = lhsPtr[loc2];
    temp[loc]  = (value0 * rhsPtr[0]) +
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

bool Matrix3::operator==(const Matrix3 & rhs) const
{
  return (
    Equals( mElements[0], rhs.mElements[0]) &&
    Equals( mElements[1], rhs.mElements[1]) &&
    Equals( mElements[2], rhs.mElements[2]) &&
    Equals( mElements[3], rhs.mElements[3]) &&
    Equals( mElements[4], rhs.mElements[4]) &&
    Equals( mElements[5], rhs.mElements[5]) &&
    Equals( mElements[6], rhs.mElements[6]) &&
    Equals( mElements[7], rhs.mElements[7]) &&
    Equals( mElements[8], rhs.mElements[8]));
}

bool Matrix3::operator!=(const Matrix3& rhs) const
{
  return !(*this == rhs);
}

std::ostream& operator<< (std::ostream& o, const Matrix3& matrix)
{
  return o << "[ [" << matrix.mElements[0] << ", " << matrix.mElements[1] << ", " << matrix.mElements[2]  << "], "
             << "[" << matrix.mElements[3] << ", " << matrix.mElements[4] << ", " << matrix.mElements[5]  << "], "
             << "[" << matrix.mElements[6] << ", " << matrix.mElements[7] << ", " << matrix.mElements[8]  << "] ]";
}

} // namespace Dali
