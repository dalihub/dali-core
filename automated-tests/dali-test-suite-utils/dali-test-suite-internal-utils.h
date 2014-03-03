#ifndef __DALI_TEST_SUITE_INTERNAL_UTILS_H__
#define __DALI_TEST_SUITE_INTERNAL_UTILS_H__

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

// INTERNAL INCLUDES
#include "dali-test-suite-utils.h"

#include <dali/public-api/math/matrix3.h>

namespace Dali
{


/**
 * Test whether two Matrix objects are equal.
 * @param[in] matrix1 The first object
 * @param[in] matrix2 The second object
 * @param[in] location The TEST_LOCATION macro should be used here
 */
void DALI_TEST_EQUALS( const Matrix3& matrix1, const Matrix3& matrix2, const char* location)
{
  const float* m1 = matrix1.AsFloat();
  const float* m2 = matrix2.AsFloat();
  bool identical = true;

  for (int i=0;i<9;++i)
  {
    identical &= (m1[i] != m2[i]);
  }

  if (!identical)
  {
    tet_printf("%s, checking\n"
               "(%f, %f, %f)    (%f, %f, %f)\n"
               "(%f, %f, %f) == (%f, %f, %f)\n"
               "(%f, %f, %f)    (%f, %f, %f)\n",
               location,
               m1[0],  m1[1], m1[2],   m2[0],  m2[1], m2[2],
               m1[3],  m1[4], m1[5],   m2[3],  m2[4], m2[5],
               m1[6],  m1[7], m1[8],   m2[6],  m2[7], m2[8]);

    tet_result(TET_FAIL);
  }
  else
  {
    tet_result(TET_PASS);
  }
}


/**
 * Test whether two Matrix objects are equal.
 * @param[in] matrix1 The first object
 * @param[in] matrix2 The second object
 * @param[in] location The TEST_LOCATION macro should be used here
 */
void DALI_TEST_EQUALS( const Matrix3& matrix1, const Matrix3& matrix2, float epsilon, const char* location)
{
  const float* m1 = matrix1.AsFloat();
  const float* m2 = matrix2.AsFloat();
  bool equivalent = true;

  for (int i=0;i<9;++i)
  {
    equivalent &= (fabsf(m1[i] - m2[i])<epsilon);
  }

  if (!equivalent)
  {
    tet_printf("%s, checking\n"
               "(%f, %f, %f)    (%f, %f, %f)\n"
               "(%f, %f, %f) == (%f, %f, %f)\n"
               "(%f, %f, %f)    (%f, %f, %f)\n",
               location,
               m1[0],  m1[1], m1[2],   m2[0],  m2[1], m2[2],
               m1[3],  m1[4], m1[5],   m2[3],  m2[4], m2[5],
               m1[6],  m1[7], m1[8],   m2[6],  m2[7], m2[8]);

    tet_result(TET_FAIL);
  }
  else
  {
    tet_result(TET_PASS);
  }
}


} // namespace Dali

#endif // __DALI_TEST_SUITE_INTERNAL_UTILS_H__
