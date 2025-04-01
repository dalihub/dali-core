/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>

// Internal headers are allowed here

using namespace Dali;

void utc_dali_internal_core_startup()
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_core_cleanup()
{
  test_return_value = TET_PASS;
}

namespace
{
class RelayoutSignalHandler : public Dali::ConnectionTracker
{
public:
  RelayoutSignalHandler(TestApplication& application)
  : mApplication(application),
    mSignalCalled(false)
  {
  }

  // callback to be connected to RelayoutSignal
  virtual void RelayoutCallback(Actor actor)
  {
    tet_infoline("RelayoutCallback is called");

    mSignalCalled = true;

    mApplication.SendNotification();
  }

  TestApplication& mApplication;
  bool             mSignalCalled;
};

class DoubleRelayoutSignalHandler : public RelayoutSignalHandler
{
public:
  DoubleRelayoutSignalHandler(TestApplication& application)
  : RelayoutSignalHandler(application)
  {
  }

  // callback to be connected to RelayoutSignal
  void RelayoutCallback(Actor actor) override
  {
    tet_infoline("DoubleRelayoutCallback is called");

    auto& core = mApplication.GetCore();
    core.ForceRelayout();

    RelayoutSignalHandler::RelayoutCallback(actor);
  }
};
constexpr size_t FORCIBLE_WAIT_FLUSHED_BUFFER_COUNT_THRESHOLD = 1024;

} // anonymous namespace

int UtcDaliCoreProcessEvents(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Core::ProcessEvents");

  Vector3 size(100.0f, 100.0f, 0.0f);
  Vector3 position(100.0f, 100.0f, 0.0f);

  Actor actor = Actor::New();
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  actor.SetProperty(Actor::Property::SIZE, size);
  actor.SetProperty(Actor::Property::POSITION, position);
  application.GetScene().Add(actor);

  RelayoutSignalHandler relayoutSignal(application);
  actor.OnRelayoutSignal().Connect(&relayoutSignal, &RelayoutSignalHandler::RelayoutCallback);

  application.SendNotification();

  DALI_TEST_EQUALS(relayoutSignal.mSignalCalled, true, TEST_LOCATION);

  DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::SIZE).Get<Vector3>(), size, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::POSITION).Get<Vector3>(), position, TEST_LOCATION);

  END_TEST;
}

int UtcDaliCoreProcessEventsStressTest(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Core::ProcessEvents more than 1k times before render");

  Vector3 size(100.0f, 100.0f, 0.0f);
  Vector3 position(100.0f, 100.0f, 0.0f);

  Actor actor = Actor::New();
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  actor.SetProperty(Actor::Property::SIZE, size);
  actor.SetProperty(Actor::Property::POSITION, position);
  application.GetScene().Add(actor);

  RelayoutSignalHandler relayoutSignal(application);
  actor.OnRelayoutSignal().Connect(&relayoutSignal, &RelayoutSignalHandler::RelayoutCallback);

  application.SendNotification();

  DALI_TEST_EQUALS(relayoutSignal.mSignalCalled, true, TEST_LOCATION);

  DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::SIZE).Get<Vector3>(), size, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::POSITION).Get<Vector3>(), position, TEST_LOCATION);

  relayoutSignal.mSignalCalled = false;

  for(size_t i = 0; i < FORCIBLE_WAIT_FLUSHED_BUFFER_COUNT_THRESHOLD; ++i)
  {
    Vector3 newSize = size + Vector3(i + 1, i + 1, 0);
    actor.SetProperty(Actor::Property::SIZE, newSize);
    application.SendNotification();
    DALI_TEST_EQUALS(relayoutSignal.mSignalCalled, true, TEST_LOCATION);
    DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::SIZE).Get<Vector3>(), newSize, TEST_LOCATION);

    relayoutSignal.mSignalCalled = false;
  }

  application.Render();
  application.SendNotification();
  application.Render();

  END_TEST;
}

int UtcDaliCoreForceRelayout(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Core::ForceRelayout");

  Vector3 size(100.0f, 100.0f, 0.0f);
  Vector3 position(100.0f, 100.0f, 0.0f);

  Actor actor = Actor::New();
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  actor.SetProperty(Actor::Property::SIZE, size);
  actor.SetProperty(Actor::Property::POSITION, position);
  application.GetScene().Add(actor);

  RelayoutSignalHandler relayoutSignal(application);
  actor.OnRelayoutSignal().Connect(&relayoutSignal, &RelayoutSignalHandler::RelayoutCallback);

  // Call ForceRelayout before application.SendNotification();
  auto& core = application.GetCore();
  core.ForceRelayout();

  DALI_TEST_EQUALS(relayoutSignal.mSignalCalled, true, TEST_LOCATION);

  DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::SIZE).Get<Vector3>(), size, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::POSITION).Get<Vector3>(), position, TEST_LOCATION);

  relayoutSignal.mSignalCalled = false;

  application.SendNotification();

  // Check relayout signal not emitted;
  DALI_TEST_EQUALS(relayoutSignal.mSignalCalled, false, TEST_LOCATION);

  END_TEST;
}

int UtcDaliCoreForceRelayout2(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Core::ForceRelayout during force-relayout");

  Vector3 size(100.0f, 100.0f, 0.0f);
  Vector3 position(100.0f, 100.0f, 0.0f);

  Actor actor = Actor::New();
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  actor.SetProperty(Actor::Property::SIZE, size);
  actor.SetProperty(Actor::Property::POSITION, position);
  application.GetScene().Add(actor);

  DoubleRelayoutSignalHandler relayoutSignal(application);
  actor.OnRelayoutSignal().Connect(&relayoutSignal, &DoubleRelayoutSignalHandler::RelayoutCallback);

  // Call ForceRelayout before application.SendNotification();
  auto& core = application.GetCore();
  core.ForceRelayout();

  DALI_TEST_EQUALS(relayoutSignal.mSignalCalled, true, TEST_LOCATION);

  DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::SIZE).Get<Vector3>(), size, TEST_LOCATION);
  DALI_TEST_EQUALS(actor.GetProperty(Actor::Property::POSITION).Get<Vector3>(), position, TEST_LOCATION);

  relayoutSignal.mSignalCalled = false;

  application.SendNotification();

  END_TEST;
}

int UtcDaliCoreClearScene(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Core::ClearScene");

  application.GetScene().SetBackgroundColor(Color::MAGENTA);

  TestGraphicsController& controller = application.GetGraphicsController();
  auto&                   contTrace  = controller.mCallStack;
  auto&                   cmdTrace   = controller.mCommandBufferCallStack;
  contTrace.Enable(true);
  contTrace.EnableLogging(true);
  cmdTrace.Enable(true);
  cmdTrace.EnableLogging(true);

  application.SendNotification();
  application.Render();

  auto& core = application.GetCore();
  core.ClearScene(application.GetScene());

  DALI_TEST_CHECK(cmdTrace.FindMethod("BeginRenderPass"));
  DALI_TEST_CHECK(contTrace.FindMethod("SubmitCommandBuffers"));
  DALI_TEST_CHECK(contTrace.FindMethod("PresentRenderTarget"));
  END_TEST;
}

int UtcDaliCoreClearSceneN(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Core::ClearScene with invalid scene");

  application.GetScene().SetBackgroundColor(Color::MAGENTA);

  // Dummy rendering several frames.
  application.SendNotification();
  application.Render();
  application.SendNotification();
  application.Render();

  TestGraphicsController& controller = application.GetGraphicsController();
  auto&                   cmdTrace   = controller.mCommandBufferCallStack;

  cmdTrace.Enable(true);
  cmdTrace.EnableLogging(true);

  cmdTrace.Reset();

  // Create new scene befoer Render(), and after SendNotification()
  application.SendNotification();
  Dali::Integration::Scene newScene = Dali::Integration::Scene::New(Size(480.0f, 800.0f));
  application.Render();

  DALI_TEST_CHECK(!cmdTrace.FindMethod("BeginRenderPass"));

  auto& core = application.GetCore();

  // Do not create BeginRenderPass for invalid scene
  core.ClearScene(newScene);
  DALI_TEST_CHECK(!cmdTrace.FindMethod("BeginRenderPass"));

  core.ClearScene(application.GetScene());
  DALI_TEST_CHECK(cmdTrace.FindMethod("BeginRenderPass"));

  cmdTrace.Reset();

  // Frame update once. Now newScene is valid
  application.SendNotification();
  application.Render();

  core.ClearScene(newScene);
  DALI_TEST_CHECK(cmdTrace.FindMethod("BeginRenderPass"));

  END_TEST;
}
