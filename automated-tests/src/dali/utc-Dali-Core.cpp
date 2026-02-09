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
#include <dali/integration-api/core.h>
#include <stdlib.h>

#include <cmath> // isfinite
#include <iostream>
#include <sstream>

using namespace Dali;

void utc_dali_core_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_core_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliCoreGetMaximumUpdateCount(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::GetMaximumUpdateCount");

  DALI_TEST_EQUALS(2, application.GetCore().GetMaximumUpdateCount(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliCoreGetObjectRegistry(void)
{
  TestApplication application;
  DALI_TEST_CHECK(application.GetCore().GetObjectRegistry());
  END_TEST;
}

int UtcDaliCoreCheckMemoryPool(void)
{
  TestApplication application;

  // Calling LogMemoryPools should check capacity across the board.
  application.GetCore().LogMemoryPools();

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliCoreChangeCorePolicy(void)
{
  TestApplication application;

  Dali::Integration::CorePolicyFlags corePolicyFlags = Dali::Integration::CorePolicyFlags::DEPTH_BUFFER_AVAILABLE | Dali::Integration::CorePolicyFlags::STENCIL_BUFFER_AVAILABLE | Dali::Integration::CorePolicyFlags::PARTIAL_UPDATE_AVAILABLE;

  // Note that we should not call this method after ContextCreated() has been called.
  // But here is UTC, so we can call it after context creation at TestApplication initialized.
  application.GetCore().ChangeCorePolicy(corePolicyFlags);

  // Render something after policy changed.
  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 200.0f));
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render(16);

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliCoreChangeCorePolicyN(void)
{
  TestApplication application;

  Dali::Integration::CorePolicyFlags corePolicyFlags = Dali::Integration::CorePolicyFlags::DEPTH_BUFFER_AVAILABLE | Dali::Integration::CorePolicyFlags::STENCIL_BUFFER_AVAILABLE | Dali::Integration::CorePolicyFlags::PARTIAL_UPDATE_AVAILABLE;

  // Render something.
  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 200.0f));
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render(16);

  // Must throw an exception if we try to change the core policy after the context has been created.
  try
  {
    application.GetCore().ChangeCorePolicy(corePolicyFlags);
    tet_result(TET_FAIL);
  }
  catch(...)
  {
    tet_result(TET_PASS);
  }

  END_TEST;
}

int UtcDaliCoreChangeGraphicsController(void)
{
  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    false, // Skip initialize().
    false);

  application.CreateCore();

  // Just insert duplicated graphics controller, for line coverage
  // Note that we should not call this method after ContextCreated() has been called.
  // But here is UTC, so we can call it after context creation at TestApplication initialized.
  Graphics::Controller& graphicsController = application.GetGraphicsController();
  application.GetCore().ChangeGraphicsController(graphicsController);

  application.CreateScene();
  application.InitializeCore();

  // Render something after graphics controller changed.
  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 200.0f));
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render(16);

  tet_result(TET_PASS);
  END_TEST;
}

int UtcDaliCoreChangeGraphicsControllerN(void)
{
  TestApplication application;

  // Render something.
  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 200.0f));
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render(16);

  // Must throw an exception if we try to change the graphics controller after the context has been created.
  try
  {
    application.GetCore().ChangeGraphicsController(application.GetGraphicsController());
    tet_result(TET_FAIL);
  }
  catch(...)
  {
    tet_result(TET_PASS);
  }

  END_TEST;
}

int UtcDaliCorePreInitializeCompleted(void)
{
  TestApplication application;

  // Create dummy actor.
  Actor dummyActor = CreateRenderableActor();

  application.GetCore().PreInitializeCompleted();

  // Render something after pre-initialize completed.
  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 200.0f));
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render(16);

  tet_result(TET_PASS);
  END_TEST;
}
