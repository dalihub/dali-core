/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

#include <dali-test-suite-utils.h>
#include <dali/devel-api/actors/camera-actor-devel.h>
#include <dali/internal/common/matrix-utils.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>

using namespace Dali;

namespace
{
// Final projection matrix logic, calculated at scene-graph-camera.
Matrix CalculateFinalProjectionMatrix(Matrix projection, uint32_t mProjectionRotation)
{
  Matrix     finalProjection;
  Quaternion rotationAngle;
  switch(mProjectionRotation)
  {
    case 90:
    {
      rotationAngle = Quaternion(Dali::ANGLE_90, Vector3::ZAXIS);
      break;
    }
    case 180:
    {
      rotationAngle = Quaternion(Dali::ANGLE_180, Vector3::ZAXIS);
      break;
    }
    case 270:
    {
      rotationAngle = Quaternion(Dali::ANGLE_270, Vector3::ZAXIS);
      break;
    }
    default:
      rotationAngle = Quaternion(Dali::ANGLE_0, Vector3::ZAXIS);
      break;
  }

  Matrix rotation;
  rotation.SetIdentity();
  rotation.SetTransformComponents(Vector3(1.0f, 1.0f, 1.0f), rotationAngle, Vector3(0.0f, 0.0f, 0.0f));

  Dali::Internal::MatrixUtils::Multiply(finalProjection, projection, rotation);

  return finalProjection;
}

} // namespace

void utc_dali_internal_matrix_utils_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_matrix_utils_cleanup(void)
{
  test_return_value = TET_PASS;
}

// Dali::Matrix

int UtcDaliMatrixUtilsMultiplyMatrixP(void)
{
  const float ll[16] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
  const float rr[16] = {1.0f, 5.0f, 0.0f, 0.0f, 2.0f, 6.0f, 0.0f, 0.0f, 3.0f, 7.0f, 0.0f, 0.0f, 4.0f, 8.0f, 0.0f, 0.0f};
  Matrix      left(ll);
  Matrix      right(rr);

  const float els[16] = {26.0f, 32.0f, 38.0f, 44.0f, 32.0f, 40.0f, 48.0f, 56.0f, 38.0f, 48.0f, 58.0f, 68.0f, 44.0f, 56.0f, 68.0f, 80.0f};
  Matrix      result(els);

  Matrix multResult;

  // Get result by Multiply API
  Internal::MatrixUtils::Multiply(multResult, right, left);
  DALI_TEST_EQUALS(multResult, result, 0.01f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliMatrixUtilsMultiplyMatrixQuaternionP(void)
{
  Matrix m1 = Matrix::IDENTITY;

  float  els[] = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.707f, 0.707f, 0.0f, 0.0f, -0.707f, 0.707f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
  Matrix result(els);

  Quaternion q(Radian(Degree(45.0f)), Vector3::XAXIS);
  Matrix     m2(false);
  Internal::MatrixUtils::Multiply(m2, m1, q);

  DALI_TEST_EQUALS(m2, result, 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliMatrixUtilsMultiplyTransformMatrix(void)
{
  tet_infoline("Multiplication two transform matrixs\n");

  Matrix expectMatrix;
  Matrix resultMatrix;
  for(int32_t repeatCount = 0; repeatCount < 10; repeatCount++)
  {
    Vector3    lpos         = Vector3(Dali::Random::Range(-50.0f, 50.0f), Dali::Random::Range(-50.0f, 50.0f), Dali::Random::Range(-50.0f, 50.0f));
    Vector3    laxis        = Vector3(Dali::Random::Range(1.0f, 50.0f), Dali::Random::Range(-50.0f, 50.0f), Dali::Random::Range(-50.0f, 50.0f));
    float      lradian      = Dali::Random::Range(0.0f, 5.0f);
    Quaternion lorientation = Quaternion(Radian(lradian), laxis);
    Vector3    lscale       = Vector3(Dali::Random::Range(-50.0f, 50.0f), Dali::Random::Range(-50.0f, 50.0f), Dali::Random::Range(-50.0f, 50.0f));

    Vector3    rpos         = Vector3(Dali::Random::Range(-50.0f, 50.0f), Dali::Random::Range(-50.0f, 50.0f), Dali::Random::Range(-50.0f, 50.0f));
    Vector3    raxis        = Vector3(Dali::Random::Range(1.0f, 50.0f), Dali::Random::Range(-50.0f, 50.0f), Dali::Random::Range(-50.0f, 50.0f));
    float      rradian      = Dali::Random::Range(0.0f, 5.0f);
    Quaternion rorientation = Quaternion(Radian(rradian), raxis);
    Vector3    rscale       = Vector3(Dali::Random::Range(-50.0f, 50.0f), Dali::Random::Range(-50.0f, 50.0f), Dali::Random::Range(-50.0f, 50.0f));

    Matrix lhs, rhs;
    lhs.SetTransformComponents(lscale, lorientation, lpos);
    rhs.SetTransformComponents(rscale, rorientation, rpos);

    // Get result by Multiply API
    Internal::MatrixUtils::Multiply(expectMatrix, lhs, rhs);
    // Get result by MultiplyTransformMatrix API
    Internal::MatrixUtils::MultiplyTransformMatrix(resultMatrix, lhs, rhs);

    {
      std::ostringstream oss;
      oss << "lhs          : " << lhs << "\n";
      oss << "lpos         : " << lpos << "\n";
      oss << "lorientation : " << lorientation << "\n";
      oss << "lscale       : " << lscale << "\n";

      oss << "rhs          : " << rhs << "\n";
      oss << "rpos         : " << rpos << "\n";
      oss << "rorientation : " << rorientation << "\n";
      oss << "rscale       : " << rscale << "\n";

      oss << "expect     : " << expectMatrix << "\n";
      oss << "result     : " << resultMatrix << "\n";
      tet_printf("test result : \n%s\n", oss.str().c_str());
    }

    DALI_TEST_EQUALS(expectMatrix, resultMatrix, 0.01f, TEST_LOCATION);
  }

  END_TEST;
}

int UtcDaliMatrixUtilsMultiplyProjectionMatrix(void)
{
  tet_infoline("Multiplication projection matrix and random matrix\n");

  Matrix viewMatrix;
  Matrix projectionMatrix;
  Matrix finalProjectionMatrix;

  Matrix expectViewProjection;
  Matrix resultViewProjection;

  TestApplication application;

  Vector2 sceneSize = application.GetScene().GetSize();

  CameraActor camera = CameraActor::New();
  DALI_TEST_CHECK(camera);
  application.GetScene().Add(camera);

  // Rotate camera 4 times
  for(int32_t i = 0; i < 4; i++)
  {
    int32_t rotationAngle = i * 90;
    tet_printf("Window rotation angle : %d\n", rotationAngle);

    application.SendNotification();
    application.Render();
    application.SendNotification();
    application.Render();

    for(int32_t repeatCount = 0; repeatCount < 10; repeatCount++)
    {
      Vector3    pos         = Vector3(Dali::Random::Range(-50.0f, 50.0f), Dali::Random::Range(-50.0f, 50.0f), Dali::Random::Range(-50.0f, 50.0f));
      Vector3    axis        = Vector3(Dali::Random::Range(1.0f, 50.0f), Dali::Random::Range(-50.0f, 50.0f), Dali::Random::Range(-50.0f, 50.0f));
      float      radian      = Dali::Random::Range(0.0f, 5.0f);
      Quaternion orientation = Quaternion(Radian(radian), axis);

      camera.SetPerspectiveProjection(sceneSize);

      // Change ViewMatrix randomly.
      camera.SetProperty(Dali::Actor::Property::POSITION, pos);
      camera.SetProperty(Dali::Actor::Property::ORIENTATION, orientation);

      application.SendNotification();
      application.Render();
      application.SendNotification();
      application.Render();

      camera.GetProperty(CameraActor::CameraActor::Property::PROJECTION_MATRIX).Get(projectionMatrix);
      camera.GetProperty(CameraActor::CameraActor::Property::VIEW_MATRIX).Get(viewMatrix);
      finalProjectionMatrix = CalculateFinalProjectionMatrix(projectionMatrix, rotationAngle);

      // Get result by Multiply API
      Internal::MatrixUtils::Multiply(expectViewProjection, viewMatrix, finalProjectionMatrix);
      // Get result by MultiplyProjectionMatrix API
      Internal::MatrixUtils::MultiplyProjectionMatrix(resultViewProjection, viewMatrix, finalProjectionMatrix);

      {
        std::ostringstream oss;
        oss << "projection : " << projectionMatrix << "\n";
        oss << "final      : " << finalProjectionMatrix << "\n";
        oss << "view       : " << viewMatrix << "\n";
        oss << "expect     : " << expectViewProjection << "\n";
        oss << "result     : " << resultViewProjection << "\n";
        tet_printf("pespective : \n%s\n", oss.str().c_str());
      }

      DALI_TEST_EQUALS(resultViewProjection, expectViewProjection, 0.01f, TEST_LOCATION);

      camera.SetOrthographicProjection(sceneSize);

      // Change ViewMatrix randomly.
      camera.SetProperty(Dali::Actor::Property::POSITION, pos);
      camera.SetProperty(Dali::Actor::Property::ORIENTATION, orientation);

      application.SendNotification();
      application.Render();
      application.SendNotification();
      application.Render();

      camera.GetProperty(CameraActor::CameraActor::Property::PROJECTION_MATRIX).Get(projectionMatrix);
      camera.GetProperty(CameraActor::CameraActor::Property::VIEW_MATRIX).Get(viewMatrix);
      finalProjectionMatrix = CalculateFinalProjectionMatrix(projectionMatrix, rotationAngle);

      // Get result by Multiply API
      Internal::MatrixUtils::Multiply(expectViewProjection, viewMatrix, finalProjectionMatrix);
      // Get result by MultiplyProjectionMatrix API
      Internal::MatrixUtils::MultiplyProjectionMatrix(resultViewProjection, viewMatrix, finalProjectionMatrix);

      {
        std::ostringstream oss;
        oss << "projection : " << projectionMatrix << "\n";
        oss << "final      : " << finalProjectionMatrix << "\n";
        oss << "view       : " << viewMatrix << "\n";
        oss << "expect     : " << expectViewProjection << "\n";
        oss << "result     : " << resultViewProjection << "\n";
        tet_printf("orthographic : \n%s\n", oss.str().c_str());
      }

      DALI_TEST_EQUALS(resultViewProjection, expectViewProjection, 0.01f, TEST_LOCATION);
    }
  }

  END_TEST;
}

int UtcDaliMatrixUtilsMultiplyAssignMatrix01P(void)
{
  tet_infoline("Multiplication Assign operator\n");
  const float ll[16] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 0.0f, 0.0f, 11.0f, 12.0f, 0.0f, 0.0f};
  const float rr[16] = {1.0f, 5.0f, 9.0f, 10.0f, 2.0f, 6.0f, 11.0f, 12.0f, 3.0f, 7.0f, 0.0f, 0.0f, 4.0f, 8.0f, 0.0f, 0.0f};
  Matrix      left(ll);
  Matrix      right(rr);

  const float els[16] = {217.0f, 242.0f, 38.0f, 44.0f, 263.0f, 294.0f, 48.0f, 56.0f, 38.0f, 48.0f, 58.0f, 68.0f, 44.0f, 56.0f, 68.0f, 80.0f};
  Matrix      result(els);

  // Get result by MultiplyAssign API
  Internal::MatrixUtils::MultiplyAssign(left, right);
  DALI_TEST_EQUALS(left, result, 0.01f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliMatrixUtilsMultiplyAssignMatrix02P(void)
{
  tet_infoline("Multiplication Assign operator with self matrix\n");
  const float ll[16] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f, 0.0f, 0.0f, 11.0f, 12.0f, 0.0f, 0.0f};
  Matrix      left(ll);

  const float els[16] = {82.0f, 92.0f, 17.0f, 20.0f, 186.0f, 212.0f, 57.0f, 68.0f, 59.0f, 78.0f, 97.0f, 116.0f, 71.0f, 94.0f, 117.0f, 140.0f};
  Matrix      result(els);

  // Get result by MultiplyAssign API
  Internal::MatrixUtils::MultiplyAssign(left, left);
  DALI_TEST_EQUALS(left, result, 0.01f, TEST_LOCATION);

  END_TEST;
}

// Dali::Matrix3

int UtcDaliMatrixUtilsMultiplyMatrix3P(void)
{
  Matrix3 left(
    1.0f, 2.0f, 3.0f, 5.0f, 6.0f, 7.0f, 0.0f, 0.0f, 0.0f);
  Matrix3 right(
    1.0f, 5.0f, 0.0f, 2.0f, 6.0f, 0.0f, 3.0f, 7.0f, 0.0f);

  Matrix3 result(
    26.0f, 32.0f, 38.0f, 32.0f, 40.0f, 48.0f, 38.0f, 48.0f, 58.0f);

  Matrix3 multResult;

  // Get result by Multiply API
  Internal::MatrixUtils::Multiply(multResult, right, left);
  DALI_TEST_EQUALS(multResult, result, 0.01f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliMatrixUtilsMultiplyAssignMatrix301P(void)
{
  tet_infoline("Multiplication Assign operator\n");
  Matrix3 left(
    1.0f, 2.0f, 3.0f, 5.0f, 6.0f, 7.0f, 9.0f, 10.0f, 0.0f);
  Matrix3 right(
    1.0f, 5.0f, 9.0f, 2.0f, 6.0f, 11.0f, 3.0f, 7.0f, 0.0f);

  Matrix3 result(
    107.0f, 122.0f, 38.0f, 131.0f, 150.0f, 48.0f, 38.0f, 48.0f, 58.0f);

  // Get result by MultiplyAssign API
  Internal::MatrixUtils::MultiplyAssign(left, right);
  DALI_TEST_EQUALS(left, result, 0.01f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliMatrixUtilsMultiplyAssignMatrix302P(void)
{
  tet_infoline("Multiplication Assign operator with self matrix\n");
  Matrix3 left(
    1.0f, 2.0f, 3.0f, 5.0f, 6.0f, 7.0f, 9.0f, 10.0f, 0.0f);

  Matrix3 result(
    38.0f, 44.0f, 17.0f, 98.0f, 116.0f, 57.0f, 59.0f, 78.0f, 97.0f);

  // Get result by MultiplyAssign API
  Internal::MatrixUtils::MultiplyAssign(left, left);
  DALI_TEST_EQUALS(left, result, 0.01f, TEST_LOCATION);

  END_TEST;
}
