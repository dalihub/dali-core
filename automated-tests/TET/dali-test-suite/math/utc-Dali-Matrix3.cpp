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

#include <iostream>

#include <stdlib.h>
#include <tet_api.h>

#include <dali/public-api/dali-core.h>
#include <dali/public-api/math/matrix3.h>

#include <dali-test-suite-utils.h>
#include <dali-test-suite-internal-utils.h>

using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static void UtcDaliMatrix3FromMatrix();
static void UtcDaliMatrix3OperatorAssign01();
static void UtcDaliMatrix3OperatorAssign02();
static void UtcDaliMatrix3AsFloat();
static void UtcDaliMatrix3Invert();
static void UtcDaliMatrix3Transpose();
static void UtcDaliMatrix3Scale();
static void UtcDaliMatrix3SetIdentity();
static void UtcDaliMatrix3Magnitude();
static void UtcDaliMatrix3ScaleInverseTranspose();
static void UtcDaliMatrix3OStreamOperator();
static void UtcDaliMatrix3Multiply();
static void UtcDaliMatrix3EqualityOperator();
static void UtcDaliMatrix3InequalityOperator();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] = {
    { UtcDaliMatrix3FromMatrix,            POSITIVE_TC_IDX },
    { UtcDaliMatrix3OperatorAssign01,      POSITIVE_TC_IDX },
    { UtcDaliMatrix3OperatorAssign02,      POSITIVE_TC_IDX },
    { UtcDaliMatrix3AsFloat,               POSITIVE_TC_IDX },
    { UtcDaliMatrix3Invert,                POSITIVE_TC_IDX },
    { UtcDaliMatrix3Transpose,             POSITIVE_TC_IDX },
    { UtcDaliMatrix3Scale,                 POSITIVE_TC_IDX },
    { UtcDaliMatrix3Magnitude,             POSITIVE_TC_IDX },
    { UtcDaliMatrix3SetIdentity,           POSITIVE_TC_IDX },
    { UtcDaliMatrix3ScaleInverseTranspose, POSITIVE_TC_IDX },
    { UtcDaliMatrix3OStreamOperator,       POSITIVE_TC_IDX },
    { UtcDaliMatrix3Multiply,              POSITIVE_TC_IDX },
    { UtcDaliMatrix3EqualityOperator,      POSITIVE_TC_IDX },
    { UtcDaliMatrix3InequalityOperator,    POSITIVE_TC_IDX },
    { NULL, 0 }
  };
}

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}


Matrix3 m1(
  -18.6f, 1.88e-09f, -6.85e-09f,
  0.0f,13.2f, 13.2f,
  -1.36e-08f,13.2f, -13.2f);

Matrix3 m2(
  -18.6f,6.91e-06f, 6.76e-06f,
  8.04e-09f,13.2f, 13.2f,
  3.01e-06f,13.2f, -13.2f);

Matrix3 m3(
  6.24f,-12.4f, -12.4f,
  -17.6f,-4.46f, -4.37f,
  -0.0641f,13.2f, -13.2f);

Matrix3 m4(
  -16.3f,6.42f, 6.38f,
  9.05f,11.6f, 11.4f,
  -0.0371f,13.1f, -13.3f);

Matrix3 m5(
  -2.43f,13.2f, 12.9f,
  18.5f,1.92f, 1.51f,
  -0.257f,13.0f, -13.4f);

Matrix3 m6(
  -2.43f,  -13.2f,   -200.9f,
  18.5f,     1.92f,    1.51f,
   0.257f,  13.0f,    13.4f);


Matrix3 i1(
  -0.05,  -0.00,   0.00,
  -0.00,   0.04,   0.04,
   0.00,   0.04,  -0.04);

Matrix3 i2(
  -0.05,   0.00,  -0.00,
   0.00,   0.04,   0.04,
   0.00,   0.04,  -0.04);

Matrix3 i3(
   0.02,  -0.05,  -0.00,
  -0.04,  -0.01,   0.04,
  -0.04,  -0.01,  -0.04);

Matrix3 i4(
  -0.05,   0.03,  -0.00,
   0.02,   0.03,   0.04,
   0.02,   0.03,  -0.04);

Matrix3 i5(
  -0.01,   0.05,  -0.00,
   0.04,   0.01,   0.04,
   0.04,   0.00,  -0.04);



Matrix3 t1(
  -18.6f, 0.0f, -1.36e-08f,
  1.88e-09f,13.2f, 13.2f,
  -6.85e-09f,13.2f, -13.2f);

Matrix3 t2(
  -18.6f,8.04e-09f, 3.01e-06f,
  6.91e-06f,13.2f, 13.2f,
  6.76e-06f,13.2f, -13.2f);

Matrix3 t3(
  6.24f,-17.6f, -0.0641f,
  -12.4f,-4.46f, 13.2f,
  -12.4f, -4.37f, -13.2f);

Matrix3 t4(
  -16.3f,9.05f, -0.0371f,
  6.42f, 11.6f, 13.1f,
  6.38f,11.4f, -13.3f);

Matrix3 t5(
  -2.43f,18.5f, -0.257f,
  13.2f, 1.92f, 13.0f,
  12.9f, 1.51f, -13.4f);



Matrix3* matrices[5] = { &m1, &m2, &m3, &m4, &m5 };
Matrix3* inverseMatrices[5] = { &i1, &i2, &i3, &i4, &i5 };
Matrix3* transposeMatrices[5] = { &t1, &t2, &t3, &t4, &t5 };


static void UtcDaliMatrix3FromMatrix()
{
  float els0[] = { 0.0f,  1.0f,  2.0f, 3.0f,
                   4.0f,  5.0f,  6.0f, 7.0f,
                   8.0f,  9.0f, 10.0f, 11.0f,
                   12.0f, 13.0f, 14.0f, 15.0f};
  Matrix m0(els0);
  Matrix3 m1(0.0f,  1.0f,  2.0f,
             4.0f,  5.0f,  6.0f,
             8.0f,  9.0f, 10.0f);

  Matrix3 m2(m0);

  DALI_TEST_EQUALS(m1, m2, 0.001, TEST_LOCATION);
}

static void UtcDaliMatrix3OperatorAssign01()
{
  float els0[] = { 0.0f,  1.0f,  2.0f, 3.0f,
                   4.0f,  5.0f,  6.0f, 7.0f,
                   8.0f,  9.0f, 10.0f, 11.0f,
                   12.0f, 13.0f, 14.0f, 15.0f};
  Matrix m0(els0);

  Matrix3 m1(0.0f,  1.0f,  2.0f,
             4.0f,  5.0f,  6.0f,
             8.0f,  9.0f, 10.0f);

  Matrix3 m2;
  m2 = m0;
  m2 = m2; // Test branch

  DALI_TEST_EQUALS(m1, m2, 0.001, TEST_LOCATION);
}


static void UtcDaliMatrix3OperatorAssign02()
{
  Matrix3 m0(0.0f,  1.0f,  2.0f,
             4.0f,  5.0f,  6.0f,
             8.0f,  9.0f, 10.0f);

  Matrix3 m1(0.0f,  1.0f,  2.0f,
             4.0f,  5.0f,  6.0f,
             8.0f,  9.0f, 10.0f);

  Matrix3 m2;
  m2 = m0;

  DALI_TEST_EQUALS(m1, m2, 0.001, TEST_LOCATION);
}



// AsFloat
static void UtcDaliMatrix3AsFloat()
{
  float values[] = {0.0f,  1.0f,  2.0f,
                    4.0f,  5.0f,  6.0f,
                    8.0f,  9.0f, 10.0f };

  Matrix3 m1(values[0], values[1], values[2], values[3],values[4], values[5], values[6], values[7],values[8]);

  for (int i=0;i<9;++i)
  {
    DALI_TEST_EQUALS(m1.AsFloat()[i], values[i],       TEST_LOCATION);
  }
}


// Invert works
static void UtcDaliMatrix3Invert()
{
  // We're going to invert a whole load of different matrices to make sure we don't
  // fail on particular orientations.
  for (int i=0;i<5;++i)
  {
    Matrix3 m = *matrices[i];
    Matrix3 inv1 = *inverseMatrices[i];

    // Convert to Mat4, perform inverse, and convert back to Mat3
    float* mf = m.AsFloat();
    float els[] = { mf[0], mf[1], mf[2], 0.0f,
                    mf[3], mf[4], mf[5], 0.0f,
                    mf[6], mf[7], mf[8], 0.0f,
                    0.0f,  0.0f,  0.0f,  1.0f };
    Matrix m4(els);
    m4.Invert();
    Matrix inv2(m4);

    Matrix3 mInv = m;
    mInv.Invert();

    DALI_TEST_EQUALS(mInv, inv1, 0.01f, TEST_LOCATION);
    DALI_TEST_EQUALS(mInv, inv2, 0.01f, TEST_LOCATION);

    Matrix3 m2 = mInv;
    m2.Invert();    // double invert - should be back to m

    DALI_TEST_EQUALS(m, m2, 0.01f, TEST_LOCATION);
  }
}

static void UtcDaliMatrix3Transpose()
{
  for (int i=0;i<5;++i)
  {
    Matrix3 m0    = *matrices[i];
    Matrix3 trans = *transposeMatrices[i];

    Matrix3 m1 = m0;
    m1.Transpose();

    DALI_TEST_EQUALS(m1, trans, 0.001f, TEST_LOCATION);

    Matrix3 m2 = m1;
    m2.Transpose();

    DALI_TEST_EQUALS(m0, m2, 0.001f, TEST_LOCATION);
  }
}

// SetIdentity
static void UtcDaliMatrix3SetIdentity()
{
  Matrix3 m( 0.0f,  1.0f,  2.0f,
             4.0f,  5.0f,  6.0f,
             8.0f,  9.0f, 10.0f);
  m.SetIdentity();

  DALI_TEST_EQUALS(m, Matrix3::IDENTITY, 0.001f, TEST_LOCATION);
}


static void UtcDaliMatrix3Scale()
{
  Matrix3 m1( 0.0f,  1.0f,  2.0f,
             4.0f,  5.0f,  6.0f,
             8.0f,  9.0f, 10.0f);

  Matrix3 m2( 0.0f,  3.0f,  6.0f,
             12.0f,  15.0f, 18.0f,
             24.0f,  27.0f, 30.0f);

  m1.Scale(3.0f);

  DALI_TEST_EQUALS(m1, m2, 0.001, TEST_LOCATION);
}

static void UtcDaliMatrix3Magnitude()
{
  Matrix3 m1( 0.0f,  1.0f,  -2.0f,
              3.0f,  -4.0f,  5.0f,
              -6.0f,  7.0f,  8.0f);

  DALI_TEST_EQUALS(Matrix3::IDENTITY.Magnitude(), 1.0f, 0.001f, TEST_LOCATION);
  DALI_TEST_EQUALS(m1.Magnitude(), 12.0f, 0.001f, TEST_LOCATION);
}



static void UtcDaliMatrix3ScaleInverseTranspose()
{
  Matrix3* matrices[6] = { &m1, &m2, &m3, &m4, &m5, &m6 };


  for (int i=0;i<6;++i)
  {
    Matrix3 m0    = *matrices[i];

    Matrix3 m1 = m0;
    m1.Invert();
    m1.Transpose();
    m1.Scale(3.0f/(m1.Magnitude()));

    Matrix3 m2 = m0;
    m2.ScaledInverseTranspose();

    DALI_TEST_EQUALS(m1, m2, 0.001f, TEST_LOCATION);
  }
}

static void UtcDaliMatrix3OStreamOperator()
{
  std::ostringstream oss;

  Matrix3 matrix( 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f );

  oss << matrix;

  std::string expectedOutput = "[ [1, 2, 3], [4, 5, 6], [7, 8, 9] ]";

  DALI_TEST_EQUALS( oss.str(), expectedOutput, TEST_LOCATION);
}

static void UtcDaliMatrix3Multiply()
{
  Matrix3 m1( 0.0f,  3.0f,  6.0f,
             12.0f,  15.0f, 18.0f,
             24.0f,  27.0f, 30.0f);

  Matrix3 m2( 0.0f,  1.0f,  0.0f,
             -1.0f,  0.0f,  0.0f,
              0.0f,  0.0f,  1.0f);

  Matrix3 m3( -3.0f,  0.0f,  6.0f,
             -15.0f, 12.0f, 18.0f,
             -27.0f, 24.0f, 30.0f);

  Matrix3 result;
  Matrix3::Multiply(result, m1, m2);

  DALI_TEST_EQUALS(m3, result, 0.01f, TEST_LOCATION);
}

static void UtcDaliMatrix3EqualityOperator()
{
  Matrix3 m1( 0.0f,  3.0f,  6.0f,
              12.0f,  15.0f, 18.0f,
              24.0f,  27.0f, 30.0f);

  Matrix3 m2( 0.0f,  3.0f,  6.0f,
             12.0f,  15.0f, 18.0f,
             24.0f,  27.0f, 30.0f);

  DALI_TEST_CHECK(m1 == m2);
}

static void UtcDaliMatrix3InequalityOperator()
{
  Matrix3 m1( 1.0f,  0.0f,  0.0f,
              0.0f,  1.0f,  0.0f,
              0.0f,  0.0f,  1.0f);

  Matrix3 m2( 0.0f,  3.0f,  6.0f,
             12.0f,  15.0f, 18.0f,
             24.0f,  27.0f, 30.0f);

  DALI_TEST_CHECK(m1 != m2);
}

