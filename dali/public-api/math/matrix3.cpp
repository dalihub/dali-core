/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/math/matrix3.h>

// EXTERNAL INCLUDES
#include <cstdint> // uint32_t
#include <cstring> // memcpy

// INTERNAL INCLUDES
#include <dali/internal/common/matrix-utils.h>
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
 * S00 S10 S20
 * S01 S11 S21
 * S02 S12 S22
 */

namespace
{
const uint32_t NUM_BYTES_IN_ROW    = 3 * sizeof(float);
const uint32_t NUM_BYTES_IN_MATRIX = 9 * sizeof(float);
} // namespace

namespace DALI_NAMESPACE
{
const Matrix3 Matrix3::IDENTITY(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

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
  if(!EqualsZero(det))
  {
    det = 1.0f / det;
    for(int32_t i = 0; i < 9; i++)
    {
      mElements[i] = cof[i] * det;
    }
    succeeded = true;
  }
  return succeeded;
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
  if(!EqualsZero(det))
  {
    // Use average rather than determinant to remove rounding to zero errors in further multiplication
    float sum = 0;
    for(uint32_t i = 0; i < 9; i++)
    {
      sum += fabsf(cof[i]);
    }
    float scale = 9.0f / sum; // Inverse of the average values
    if(det < 0)
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

void Matrix3::Multiply(Matrix3& result, const Matrix3& lhs, const Matrix3& rhs)
{
  Internal::MatrixUtils::Multiply(result, lhs, rhs);
}

Matrix3 Matrix3::operator*(const Matrix3& rhs) const
{
  Matrix3 result;
  Internal::MatrixUtils::Multiply(result, rhs, *this);
  return result;
}

Matrix3& Matrix3::operator*=(const Matrix3& rhs)
{
  Internal::MatrixUtils::MultiplyAssign(*this, rhs);
  return *this;
}

} //namespace DALI_NAMESPACE
