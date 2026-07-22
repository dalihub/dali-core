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

#include <dali-test-suite-utils.h>
#include <dali/devel-api/common/stage.h>
#include <dali/public-api/dali-core.h>

#include <utility>

using namespace Dali;

void stage_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void stage_test_cleanup(void)
{
  test_return_value = TET_PASS;
}

// Note: Dali::Stage is a deprecated stub kept only for binary compatibility with
// legacy applications. Only GetCurrent(), GetDpi() and KeepRendering() remain.

int UtcDaliStageDefaultConstructorP(void)
{
  TestApplication application;

  Stage stage;
  // The stub no longer wraps an internal object, so the handle is always empty.
  DALI_TEST_CHECK(!stage);
  END_TEST;
}

int UtcDaliStageGetCurrentP(void)
{
  TestApplication application;

  // GetCurrent() is retained for binary compatibility; it returns an empty handle.
  Stage stage = Stage::GetCurrent();
  DALI_TEST_CHECK(!stage);
  END_TEST;
}

int UtcDaliStageCopyAndMoveP(void)
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  // Copy constructor
  Stage copyStage(stage);
  DALI_TEST_CHECK(copyStage == stage);

  // Copy assignment
  Stage assignStage;
  assignStage = stage;
  DALI_TEST_CHECK(assignStage == stage);

  // Move constructor
  Stage moveStage(std::move(copyStage));
  DALI_TEST_CHECK(moveStage == stage);

  // Move assignment
  Stage moveAssignStage;
  moveAssignStage = std::move(assignStage);
  DALI_TEST_CHECK(moveAssignStage == stage);

  END_TEST;
}

int UtcDaliStageGetDpiDefaultP(void)
{
  TestApplication application; // Initializes core DPI to default values

  Stage stage = Stage::GetCurrent();

  Vector2 dpi = stage.GetDpi();
  DALI_TEST_EQUALS(dpi.x, static_cast<float>(TestApplication::DEFAULT_HORIZONTAL_DPI), TEST_LOCATION);
  DALI_TEST_EQUALS(dpi.y, static_cast<float>(TestApplication::DEFAULT_VERTICAL_DPI), TEST_LOCATION);
  END_TEST;
}

int UtcDaliStageGetDpiExplicitP(void)
{
  TestApplication application;

  // Setting the DPI on the scene must be reflected by the stub's GetDpi().
  Dali::Integration::Scene scene = application.GetScene();
  scene.SetDpi(Vector2(200.0f, 180.0f));

  Stage   stage = Stage::GetCurrent();
  Vector2 dpi   = stage.GetDpi();
  DALI_TEST_EQUALS(dpi.x, 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(dpi.y, 180.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStageGetDpiSpecificP(void)
{
  TestApplication application(480, 800, 72, 120); // Initializes core DPI with specific values

  Stage   stage = Stage::GetCurrent();
  Vector2 dpi   = stage.GetDpi();
  DALI_TEST_EQUALS(dpi.x, 72.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(dpi.y, 120.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliStageKeepRenderingP(void)
{
  TestApplication application;

  Actor actor = CreateRenderableActor();
  application.GetScene().Add(actor);

  // Run core until it wants to sleep
  bool keepUpdating(true);
  while(keepUpdating)
  {
    application.SendNotification();
    keepUpdating = application.Render(1000.0f /*1 second*/);
  }

  // Force rendering for the next 5 seconds
  Stage stage = Stage::GetCurrent();
  stage.KeepRendering(5.0f);

  application.SendNotification();

  // Test that core keeps updating until 5 seconds have elapsed
  keepUpdating = application.Render(1000.0f /*1 second*/);
  DALI_TEST_CHECK(keepUpdating);
  keepUpdating = application.Render(1000.0f /*2 seconds*/);
  DALI_TEST_CHECK(keepUpdating);
  keepUpdating = application.Render(1000.0f /*3 seconds*/);
  DALI_TEST_CHECK(keepUpdating);
  keepUpdating = application.Render(1000.0f /*4 seconds*/);
  DALI_TEST_CHECK(keepUpdating);
  keepUpdating = application.Render(1000.0f /*5 seconds*/);
  DALI_TEST_CHECK(keepUpdating);
  keepUpdating = application.Render(1000.0f /*6 seconds*/); // After 5 sec
  DALI_TEST_CHECK(!keepUpdating);
  END_TEST;
}

int UtcDaliStageKeepRenderingN(void)
{
  TestApplication application;

  // Run core until it wants to sleep
  bool keepUpdating(true);
  while(keepUpdating)
  {
    application.SendNotification();
    keepUpdating = application.Render(1000.0f /*1 second*/);
  }

  // A negative duration does not keep rendering
  Stage stage = Stage::GetCurrent();
  stage.KeepRendering(-1.0f);

  application.SendNotification();

  keepUpdating = application.Render(1000.0f /*1 second*/);
  DALI_TEST_CHECK(!keepUpdating);
  END_TEST;
}

int UtcDaliStageNoCoreN(void)
{
  // Create and destroy a core so that the thread-local storage is removed.
  {
    TestApplication application;
  }

  // Without an installed core the stub must return a zero DPI and KeepRendering()
  // must be a safe no-op.
  Stage stage = Stage::GetCurrent();
  DALI_TEST_EQUALS(stage.GetDpi(), Vector2::ZERO, TEST_LOCATION);

  stage.KeepRendering(5.0f);
  DALI_TEST_CHECK(!stage);
  END_TEST;
}
