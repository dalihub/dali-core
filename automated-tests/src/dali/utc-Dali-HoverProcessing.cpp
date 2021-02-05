/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/events/hover-event-devel.h>
#include <dali/integration-api/events/hover-event-integ.h>
#include <dali/integration-api/render-task-list-integ.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>

using namespace Dali;

void utc_dali_hover_processing_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_hover_processing_cleanup(void)
{
  test_return_value = TET_PASS;
}

///////////////////////////////////////////////////////////////////////////////

namespace
{
// Stores data that is populated in the callback and will be read by the TET cases
struct SignalData
{
  SignalData()
  : functorCalled(false),
    hoverEvent(),
    hoveredActor()
  {
  }

  void Reset()
  {
    functorCalled = false;
    hoverEvent.Reset();
    hoveredActor.Reset();
  }

  bool       functorCalled;
  HoverEvent hoverEvent;
  Actor      hoveredActor;
};

// Functor that sets the data when called
struct HoverEventFunctor
{
  /**
   * Constructor.
   * @param[in]  data         Reference to the data to store callback information.
   * @param[in]  returnValue  What the functor should return.
   */
  HoverEventFunctor(SignalData& data, bool returnValue = true)
  : signalData(data),
    returnValue(returnValue)
  {
  }

  bool operator()(Actor actor, const HoverEvent& hoverEvent)
  {
    signalData.functorCalled = true;
    signalData.hoveredActor  = actor;
    signalData.hoverEvent    = hoverEvent;

    return returnValue;
  }

  SignalData& signalData;
  bool        returnValue;
};

// Functor that removes the actor when called.
struct RemoveActorFunctor : public HoverEventFunctor
{
  /**
   * Constructor.
   * @param[in]  data         Reference to the data to store callback information.
   * @param[in]  returnValue  What the functor should return.
   */
  RemoveActorFunctor(SignalData& data, bool returnValue = true)
  : HoverEventFunctor(data, returnValue)
  {
  }

  bool operator()(Actor actor, const HoverEvent& hoverEvent)
  {
    Actor parent(actor.GetParent());
    if(parent)
    {
      parent.Remove(actor);
    }

    return HoverEventFunctor::operator()(actor, hoverEvent);
  }
};

Integration::HoverEvent GenerateSingleHover(PointState::Type state, const Vector2& screenPosition)
{
  Integration::HoverEvent hoverEvent;
  Integration::Point      point;
  point.SetState(state);
  point.SetScreenPosition(screenPosition);
  hoverEvent.points.push_back(point);
  return hoverEvent;
}

} // namespace

///////////////////////////////////////////////////////////////////////////////

int UtcDaliHoverNormalProcessing(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal
  SignalData        data;
  HoverEventFunctor functor(data);
  actor.HoveredSignal().Connect(&application, functor);

  Vector2 screenCoordinates(10.0f, 10.0f);
  Vector2 localCoordinates;
  actor.ScreenToLocal(localCoordinates.x, localCoordinates.y, screenCoordinates.x, screenCoordinates.y);

  // Emit a started signal
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, screenCoordinates));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.hoverEvent.GetPointCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::STARTED, data.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_EQUALS(screenCoordinates, data.hoverEvent.GetScreenPosition(0), TEST_LOCATION);
  DALI_TEST_EQUALS(localCoordinates, data.hoverEvent.GetLocalPosition(0), 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(0, data.hoverEvent.GetDeviceId(0), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, data.hoverEvent.GetTime(), TEST_LOCATION);
  DALI_TEST_EQUALS(actor, data.hoverEvent.GetHitActor(0), TEST_LOCATION);
  DALI_TEST_EQUALS(-1, data.hoverEvent.GetDeviceId(1), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::FINISHED, data.hoverEvent.GetState(1), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2::ZERO, data.hoverEvent.GetScreenPosition(1), 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2::ZERO, data.hoverEvent.GetLocalPosition(1), 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Actor(), data.hoverEvent.GetHitActor(1), TEST_LOCATION);
  data.Reset();

  // Emit a motion signal
  screenCoordinates.x = screenCoordinates.y = 11.0f;
  actor.ScreenToLocal(localCoordinates.x, localCoordinates.y, screenCoordinates.x, screenCoordinates.y);
  application.ProcessEvent(GenerateSingleHover(PointState::MOTION, screenCoordinates));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.hoverEvent.GetPointCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::MOTION, data.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_EQUALS(screenCoordinates, data.hoverEvent.GetScreenPosition(0), TEST_LOCATION);
  DALI_TEST_EQUALS(localCoordinates, data.hoverEvent.GetLocalPosition(0), 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(0, data.hoverEvent.GetDeviceId(0), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, data.hoverEvent.GetTime(), TEST_LOCATION);
  DALI_TEST_EQUALS(actor, data.hoverEvent.GetHitActor(0), TEST_LOCATION);
  DALI_TEST_EQUALS(-1, data.hoverEvent.GetDeviceId(1), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::FINISHED, data.hoverEvent.GetState(1), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2::ZERO, data.hoverEvent.GetScreenPosition(1), 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2::ZERO, data.hoverEvent.GetLocalPosition(1), 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Actor(), data.hoverEvent.GetHitActor(1), TEST_LOCATION);
  data.Reset();

  // Emit a finished signal
  screenCoordinates.x = screenCoordinates.y = 12.0f;
  actor.ScreenToLocal(localCoordinates.x, localCoordinates.y, screenCoordinates.x, screenCoordinates.y);
  application.ProcessEvent(GenerateSingleHover(PointState::FINISHED, screenCoordinates));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.hoverEvent.GetPointCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::FINISHED, data.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_EQUALS(screenCoordinates, data.hoverEvent.GetScreenPosition(0), TEST_LOCATION);
  DALI_TEST_EQUALS(localCoordinates, data.hoverEvent.GetLocalPosition(0), 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(0, data.hoverEvent.GetDeviceId(0), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, data.hoverEvent.GetTime(), TEST_LOCATION);
  DALI_TEST_EQUALS(actor, data.hoverEvent.GetHitActor(0), TEST_LOCATION);
  DALI_TEST_EQUALS(-1, data.hoverEvent.GetDeviceId(1), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::FINISHED, data.hoverEvent.GetState(1), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2::ZERO, data.hoverEvent.GetScreenPosition(1), 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2::ZERO, data.hoverEvent.GetLocalPosition(1), 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(Dali::Actor(), data.hoverEvent.GetHitActor(1), TEST_LOCATION);
  data.Reset();

  // Emit a started signal where the actor is not present
  screenCoordinates.x = screenCoordinates.y = 200.0f;
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, screenCoordinates));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(!data.hoverEvent);

  END_TEST;
}

int UtcDaliHoverOutsideCameraNearFarPlanes(void)
{
  TestApplication application;

  Integration::Scene stage     = application.GetScene();
  Vector2            stageSize = stage.GetSize();

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  stage.Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Get the camera's near and far planes
  RenderTaskList   taskList  = stage.GetRenderTaskList();
  Dali::RenderTask task      = taskList.GetTask(0);
  CameraActor      camera    = task.GetCameraActor();
  float            nearPlane = camera.GetNearClippingPlane();
  float            farPlane  = camera.GetFarClippingPlane();

  // Calculate the current distance of the actor from the camera
  float tanHalfFov = tanf(camera.GetFieldOfView() * 0.5f);
  float distance   = (stageSize.y * 0.5f) / tanHalfFov;

  // Connect to actor's hovered signal
  SignalData        data;
  HoverEventFunctor functor(data);
  actor.HoveredSignal().Connect(&application, functor);

  Vector2 screenCoordinates(stageSize.x * 0.5f, stageSize.y * 0.5f);

  // Emit a started signal
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, screenCoordinates));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Emit a started signal where actor is just at the camera's near plane
  actor.SetProperty(Actor::Property::POSITION_Z, distance - nearPlane);

  // Render and notify
  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, screenCoordinates));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Emit a started signal where actor is closer than the camera's near plane
  actor.SetProperty(Actor::Property::POSITION_Z, (distance - nearPlane) + 1.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, screenCoordinates));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Emit a started signal where actor is just at the camera's far plane
  actor.SetProperty(Actor::Property::POSITION_Z, distance - farPlane);

  // Render and notify
  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, screenCoordinates));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Emit a started signal where actor is further than the camera's far plane
  actor.SetProperty(Actor::Property::POSITION_Z, (distance - farPlane) - 1.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, screenCoordinates));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();
  END_TEST;
}

int UtcDaliHoverEmitEmpty(void)
{
  TestApplication application;

  try
  {
    // Emit an empty HoverEvent
    Integration::HoverEvent event;
    application.ProcessEvent(event);
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_ASSERT(e, "!event.points.empty()", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliHoverInterrupted(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal
  SignalData        data;
  HoverEventFunctor functor(data);
  actor.HoveredSignal().Connect(&application, functor);

  // Emit a started signal
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::STARTED, data.hoverEvent.GetState(0), TEST_LOCATION);
  data.Reset();

  // Emit an interrupted signal, we should be signalled regardless of whether there is a hit or not.
  application.ProcessEvent(GenerateSingleHover(PointState::INTERRUPTED, Vector2(200.0f, 200.0f /* Outside actor */)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::INTERRUPTED, data.hoverEvent.GetState(0), TEST_LOCATION);
  data.Reset();

  // Emit another interrupted signal, our signal handler should not be called.
  application.ProcessEvent(GenerateSingleHover(PointState::INTERRUPTED, Vector2(200.0f, 200.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliHoverParentConsumer(void)
{
  TestApplication application;
  Actor           rootActor(application.GetScene().GetRootLayer());

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal
  SignalData        data;
  HoverEventFunctor functor(data, false);
  actor.HoveredSignal().Connect(&application, functor);

  // Connect to root actor's hovered signal
  SignalData        rootData;
  HoverEventFunctor rootFunctor(rootData); // Consumes signal
  rootActor.HoveredSignal().Connect(&application, rootFunctor);

  Vector2 screenCoordinates(10.0f, 10.0f);
  Vector2 actorCoordinates, rootCoordinates;
  actor.ScreenToLocal(actorCoordinates.x, actorCoordinates.y, screenCoordinates.x, screenCoordinates.y);
  rootActor.ScreenToLocal(rootCoordinates.x, rootCoordinates.y, screenCoordinates.x, screenCoordinates.y);

  // Emit a started signal
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, screenCoordinates));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.hoverEvent.GetPointCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, rootData.hoverEvent.GetPointCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::STARTED, data.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::STARTED, rootData.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_EQUALS(screenCoordinates, data.hoverEvent.GetScreenPosition(0), TEST_LOCATION);
  DALI_TEST_EQUALS(screenCoordinates, rootData.hoverEvent.GetScreenPosition(0), TEST_LOCATION);
  DALI_TEST_EQUALS(actorCoordinates, data.hoverEvent.GetLocalPosition(0), 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(rootCoordinates, rootData.hoverEvent.GetLocalPosition(0), 0.1f, TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.hoverEvent.GetHitActor(0));
  DALI_TEST_CHECK(actor == rootData.hoverEvent.GetHitActor(0));
  data.Reset();
  rootData.Reset();

  // Emit a motion signal
  screenCoordinates.x = screenCoordinates.y = 11.0f;
  actor.ScreenToLocal(actorCoordinates.x, actorCoordinates.y, screenCoordinates.x, screenCoordinates.y);
  rootActor.ScreenToLocal(rootCoordinates.x, rootCoordinates.y, screenCoordinates.x, screenCoordinates.y);
  application.ProcessEvent(GenerateSingleHover(PointState::MOTION, screenCoordinates));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.hoverEvent.GetPointCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, rootData.hoverEvent.GetPointCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::MOTION, data.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::MOTION, rootData.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_EQUALS(screenCoordinates, data.hoverEvent.GetScreenPosition(0), TEST_LOCATION);
  DALI_TEST_EQUALS(screenCoordinates, rootData.hoverEvent.GetScreenPosition(0), TEST_LOCATION);
  DALI_TEST_EQUALS(actorCoordinates, data.hoverEvent.GetLocalPosition(0), 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(rootCoordinates, rootData.hoverEvent.GetLocalPosition(0), 0.1f, TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.hoverEvent.GetHitActor(0));
  DALI_TEST_CHECK(actor == rootData.hoverEvent.GetHitActor(0));
  data.Reset();
  rootData.Reset();

  // Emit a finished signal
  screenCoordinates.x = screenCoordinates.y = 12.0f;
  actor.ScreenToLocal(actorCoordinates.x, actorCoordinates.y, screenCoordinates.x, screenCoordinates.y);
  rootActor.ScreenToLocal(rootCoordinates.x, rootCoordinates.y, screenCoordinates.x, screenCoordinates.y);
  application.ProcessEvent(GenerateSingleHover(PointState::FINISHED, screenCoordinates));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.hoverEvent.GetPointCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, rootData.hoverEvent.GetPointCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::FINISHED, data.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::FINISHED, rootData.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_EQUALS(screenCoordinates, data.hoverEvent.GetScreenPosition(0), TEST_LOCATION);
  DALI_TEST_EQUALS(screenCoordinates, rootData.hoverEvent.GetScreenPosition(0), TEST_LOCATION);
  DALI_TEST_EQUALS(actorCoordinates, data.hoverEvent.GetLocalPosition(0), 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(rootCoordinates, rootData.hoverEvent.GetLocalPosition(0), 0.1f, TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.hoverEvent.GetHitActor(0));
  DALI_TEST_CHECK(actor == rootData.hoverEvent.GetHitActor(0));
  data.Reset();
  rootData.Reset();

  // Emit a started signal where the actor is not present, will hit the root actor though
  screenCoordinates.x = screenCoordinates.y = 200.0f;
  rootActor.ScreenToLocal(rootCoordinates.x, rootCoordinates.y, screenCoordinates.x, screenCoordinates.y);
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, screenCoordinates));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, rootData.hoverEvent.GetPointCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::STARTED, rootData.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_EQUALS(screenCoordinates, rootData.hoverEvent.GetScreenPosition(0), TEST_LOCATION);
  DALI_TEST_EQUALS(rootCoordinates, rootData.hoverEvent.GetLocalPosition(0), 0.1f, TEST_LOCATION);
  DALI_TEST_CHECK(rootActor == rootData.hoverEvent.GetHitActor(0));
  END_TEST;
}

int UtcDaliHoverInterruptedParentConsumer(void)
{
  TestApplication application;
  Actor           rootActor(application.GetScene().GetRootLayer());

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal
  SignalData        data;
  HoverEventFunctor functor(data, false);
  actor.HoveredSignal().Connect(&application, functor);

  // Connect to root actor's hovered signal
  SignalData        rootData;
  HoverEventFunctor rootFunctor(rootData); // Consumes signal
  rootActor.HoveredSignal().Connect(&application, rootFunctor);

  // Emit a started signal
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::STARTED, data.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::STARTED, rootData.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.hoverEvent.GetHitActor(0));
  DALI_TEST_CHECK(actor == rootData.hoverEvent.GetHitActor(0));
  data.Reset();
  rootData.Reset();

  // Emit an interrupted signal
  application.ProcessEvent(GenerateSingleHover(PointState::INTERRUPTED, Vector2(200.0f, 200.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::INTERRUPTED, data.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::INTERRUPTED, rootData.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.hoverEvent.GetHitActor(0));
  DALI_TEST_CHECK(actor == rootData.hoverEvent.GetHitActor(0));
  data.Reset();
  rootData.Reset();

  // Emit another started signal
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::STARTED, data.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::STARTED, rootData.hoverEvent.GetState(0), TEST_LOCATION);
  data.Reset();
  rootData.Reset();

  // Remove actor from Stage
  application.GetScene().Remove(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit an interrupted signal, only root actor's signal should be called.
  application.ProcessEvent(GenerateSingleHover(PointState::INTERRUPTED, Vector2(200.0f, 200.0f /* Outside actor */)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::INTERRUPTED, rootData.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_CHECK(rootActor == rootData.hoverEvent.GetHitActor(0));
  data.Reset();
  rootData.Reset();

  // Emit another interrupted state, none of the signal's should be called.
  application.ProcessEvent(GenerateSingleHover(PointState::INTERRUPTED, Vector2(200.0f, 200.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, rootData.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliHoverLeave(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal
  SignalData        data;
  HoverEventFunctor functor(data);
  actor.HoveredSignal().Connect(&application, functor);

  // Set actor to require leave events
  actor.SetProperty(Actor::Property::LEAVE_REQUIRED, true);

  // Emit a started signal
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::STARTED, data.hoverEvent.GetState(0), TEST_LOCATION);
  data.Reset();

  // Emit a motion signal outside of actor, should be signalled with a Leave
  application.ProcessEvent(GenerateSingleHover(PointState::MOTION, Vector2(200.0f, 200.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::LEAVE, data.hoverEvent.GetState(0), TEST_LOCATION);
  data.Reset();

  // Another motion outside of actor, no signalling
  application.ProcessEvent(GenerateSingleHover(PointState::MOTION, Vector2(201.0f, 201.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Another motion event inside actor, signalled with motion
  application.ProcessEvent(GenerateSingleHover(PointState::MOTION, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::MOTION, data.hoverEvent.GetState(0), TEST_LOCATION);
  data.Reset();

  // We do not want to listen to leave events anymore
  actor.SetProperty(Actor::Property::LEAVE_REQUIRED, false);

  // Another motion event outside of actor, no signalling
  application.ProcessEvent(GenerateSingleHover(PointState::MOTION, Vector2(200.0f, 200.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();
  END_TEST;
}

int UtcDaliHoverLeaveParentConsumer(void)
{
  TestApplication application;
  Actor           rootActor(application.GetScene().GetRootLayer());

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal
  SignalData        data;
  HoverEventFunctor functor(data, false);
  actor.HoveredSignal().Connect(&application, functor);

  // Connect to root actor's hovered signal
  SignalData        rootData;
  HoverEventFunctor rootFunctor(rootData); // Consumes signal
  rootActor.HoveredSignal().Connect(&application, rootFunctor);

  // Set actor to require leave events
  actor.SetProperty(Actor::Property::LEAVE_REQUIRED, true);
  rootActor.SetProperty(Actor::Property::LEAVE_REQUIRED, true);

  // Emit a started signal
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::STARTED, data.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::STARTED, rootData.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.hoverEvent.GetHitActor(0));
  DALI_TEST_CHECK(actor == rootData.hoverEvent.GetHitActor(0));
  data.Reset();
  rootData.Reset();

  // Emit a motion signal outside of actor, should be signalled with a Leave
  application.ProcessEvent(GenerateSingleHover(PointState::MOTION, Vector2(200.0f, 200.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::LEAVE, data.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::LEAVE, rootData.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.hoverEvent.GetHitActor(0));
  DALI_TEST_CHECK(actor == rootData.hoverEvent.GetHitActor(0));
  data.Reset();
  rootData.Reset();

  // Another motion outside of actor, only rootActor signalled
  application.ProcessEvent(GenerateSingleHover(PointState::MOTION, Vector2(201.0f, 201.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::MOTION, rootData.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_CHECK(rootActor == rootData.hoverEvent.GetHitActor(0));
  data.Reset();
  rootData.Reset();

  // Another motion event inside actor, signalled with motion
  application.ProcessEvent(GenerateSingleHover(PointState::MOTION, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::MOTION, data.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::MOTION, rootData.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.hoverEvent.GetHitActor(0));
  DALI_TEST_CHECK(actor == rootData.hoverEvent.GetHitActor(0));
  data.Reset();
  rootData.Reset();

  // We do not want to listen to leave events of actor anymore
  actor.SetProperty(Actor::Property::LEAVE_REQUIRED, false);

  // Another motion event outside of root actor, only root signalled
  Vector2 stageSize(application.GetScene().GetSize());
  application.ProcessEvent(GenerateSingleHover(PointState::MOTION, Vector2(stageSize.width + 10.0f, stageSize.height + 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::LEAVE, rootData.hoverEvent.GetState(0), TEST_LOCATION);
  END_TEST;
}

int UtcDaliHoverActorBecomesInsensitive(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal
  SignalData        data;
  HoverEventFunctor functor(data);
  actor.HoveredSignal().Connect(&application, functor);

  // Emit a started signal
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::STARTED, data.hoverEvent.GetState(0), TEST_LOCATION);
  data.Reset();

  // Change actor to insensitive
  actor.SetProperty(Actor::Property::SENSITIVE, false);

  // Emit a motion signal, signalled with an interrupted
  application.ProcessEvent(GenerateSingleHover(PointState::MOTION, Vector2(200.0f, 200.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::INTERRUPTED, data.hoverEvent.GetState(0), TEST_LOCATION);
  data.Reset();
  END_TEST;
}

int UtcDaliHoverActorBecomesInsensitiveParentConsumer(void)
{
  TestApplication application;
  Actor           rootActor(application.GetScene().GetRootLayer());

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal
  SignalData        data;
  HoverEventFunctor functor(data, false);
  actor.HoveredSignal().Connect(&application, functor);

  // Connect to root actor's hovered signal
  SignalData        rootData;
  HoverEventFunctor rootFunctor(rootData); // Consumes signal
  rootActor.HoveredSignal().Connect(&application, rootFunctor);

  // Emit a started signal
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::STARTED, data.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::STARTED, rootData.hoverEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.hoverEvent.GetHitActor(0));
  DALI_TEST_CHECK(actor == rootData.hoverEvent.GetHitActor(0));
  data.Reset();
  rootData.Reset();

  // Remove actor from Stage
  application.GetScene().Remove(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Make root actor insensitive
  rootActor.SetProperty(Actor::Property::SENSITIVE, false);

  // Emit a motion signal, signalled with an interrupted (should get interrupted even if within root actor)
  application.ProcessEvent(GenerateSingleHover(PointState::MOTION, Vector2(200.0f, 200.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::INTERRUPTED, rootData.hoverEvent.GetState(0), TEST_LOCATION);
  END_TEST;
}

int UtcDaliHoverMultipleLayers(void)
{
  TestApplication application;
  Actor           rootActor(application.GetScene().GetRootLayer());

  // Connect to actor's hovered signal
  SignalData        data;
  HoverEventFunctor functor(data);

  Layer layer1(Layer::New());
  layer1.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  layer1.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(layer1);

  Actor actor1(Actor::New());
  actor1.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor1.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor1.SetProperty(Actor::Property::POSITION_Z, 1.0f); // Should hit actor1 in this layer
  layer1.Add(actor1);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to layer1 and actor1
  layer1.HoveredSignal().Connect(&application, functor);
  actor1.HoveredSignal().Connect(&application, functor);

  // Hit in hittable area, actor1 should be hit
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(data.hoveredActor == actor1);
  data.Reset();

  // Make layer1 insensitive, nothing should be hit
  layer1.SetProperty(Actor::Property::SENSITIVE, false);
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Make layer1 sensitive again, again actor1 will be hit
  layer1.SetProperty(Actor::Property::SENSITIVE, true);
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(data.hoveredActor == actor1);
  data.Reset();

  // Make rootActor insensitive, nothing should be hit
  rootActor.SetProperty(Actor::Property::SENSITIVE, false);
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Make rootActor sensitive
  rootActor.SetProperty(Actor::Property::SENSITIVE, true);

  // Add another layer
  Layer layer2(Layer::New());
  layer2.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  layer2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  layer2.SetProperty(Actor::Property::POSITION_Z, 10.0f); // Should hit layer2 in this layer rather than actor2
  application.GetScene().Add(layer2);

  Actor actor2(Actor::New());
  actor2.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  layer2.Add(actor2);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to layer2 and actor2
  layer2.HoveredSignal().Connect(&application, functor);
  actor2.HoveredSignal().Connect(&application, functor);

  // Emit an event, should hit layer2
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  //DALI_TEST_CHECK( data.hoveredActor == layer2 ); // TODO: Uncomment this after removing renderable hack!
  data.Reset();

  // Make layer2 insensitive, should hit actor1
  layer2.SetProperty(Actor::Property::SENSITIVE, false);
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(data.hoveredActor == actor1);
  data.Reset();

  // Make layer2 sensitive again, should hit layer2
  layer2.SetProperty(Actor::Property::SENSITIVE, true);
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  //DALI_TEST_CHECK( data.hoveredActor == layer2 ); // TODO: Uncomment this after removing renderable hack!
  data.Reset();

  // Make layer2 invisible, render and notify
  layer2.SetProperty(Actor::Property::VISIBLE, false);
  application.SendNotification();
  application.Render();

  // Should hit actor1
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(data.hoveredActor == actor1);
  data.Reset();

  // Make rootActor invisible, render and notify
  rootActor.SetProperty(Actor::Property::VISIBLE, false);
  application.SendNotification();
  application.Render();

  // Should not hit anything
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();
  END_TEST;
}

int UtcDaliHoverMultipleRenderTasks(void)
{
  TestApplication    application;
  Integration::Scene stage(application.GetScene());
  Vector2            stageSize(stage.GetSize());

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  stage.Add(actor);

  // Create render task
  Viewport   viewport(stageSize.width * 0.5f, stageSize.height * 0.5f, stageSize.width * 0.5f, stageSize.height * 0.5f);
  RenderTask renderTask(application.GetScene().GetRenderTaskList().CreateTask());
  renderTask.SetViewport(viewport);
  renderTask.SetInputEnabled(true);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal
  SignalData        data;
  HoverEventFunctor functor(data);
  actor.HoveredSignal().Connect(&application, functor);

  // Emit a started signal
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Ensure renderTask actor can be hit too.
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(viewport.x + 5.0f, viewport.y + 5.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Disable input on renderTask, should not be hittable
  renderTask.SetInputEnabled(false);
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(viewport.x + 5.0f, viewport.y + 5.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();
  END_TEST;
}

int UtcDaliHoverMultipleRenderTasksWithChildLayer(void)
{
  TestApplication    application;
  Integration::Scene stage(application.GetScene());
  Vector2            stageSize(stage.GetSize());

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  stage.Add(actor);

  Layer layer = Layer::New();
  layer.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  layer.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.Add(layer);

  // Create render task
  Viewport   viewport(stageSize.width * 0.5f, stageSize.height * 0.5f, stageSize.width * 0.5f, stageSize.height * 0.5f);
  RenderTask renderTask(application.GetScene().GetRenderTaskList().CreateTask());
  renderTask.SetViewport(viewport);
  renderTask.SetInputEnabled(true);
  renderTask.SetSourceActor(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to layer's hovered signal
  SignalData        data;
  HoverEventFunctor functor(data);
  actor.HoveredSignal().Connect(&application, functor);
  layer.HoveredSignal().Connect(&application, functor);

  // Emit a started signal
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Ensure renderTask actor can be hit too.
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(viewport.x + 5.0f, viewport.y + 5.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Disable input on renderTask, should not be hittable
  renderTask.SetInputEnabled(false);
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(viewport.x + 5.0f, viewport.y + 5.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();
  END_TEST;
}

int UtcDaliHoverOffscreenRenderTasks(void)
{
  TestApplication    application;
  Integration::Scene stage(application.GetScene());
  Vector2            stageSize(stage.GetSize());

  // FrameBufferImage for offscreen RenderTask
  FrameBuffer frameBuffer = FrameBuffer::New(stageSize.width, stageSize.height);

  // Create a renderable actor to display the FrameBufferImage
  Actor renderableActor = CreateRenderableActor(frameBuffer.GetColorTexture());
  renderableActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  renderableActor.SetProperty(Actor::Property::SIZE, Vector2(stageSize.x, stageSize.y));
  renderableActor.ScaleBy(Vector3(1.0f, -1.0f, 1.0f)); // FIXME
  stage.Add(renderableActor);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  stage.Add(actor);
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE); // Ensure framebuffer connects

  stage.GetRenderTaskList().GetTask(0u).SetScreenToFrameBufferFunction(RenderTask::FULLSCREEN_FRAMEBUFFER_FUNCTION);

  // Create a RenderTask
  RenderTask renderTask = stage.GetRenderTaskList().CreateTask();
  renderTask.SetSourceActor(actor);
  renderTask.SetFrameBuffer(frameBuffer);
  renderTask.SetInputEnabled(true);

  // Create another RenderTask
  RenderTask renderTask2(stage.GetRenderTaskList().CreateTask());
  renderTask2.SetInputEnabled(true);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal
  SignalData        data;
  HoverEventFunctor functor(data);
  actor.HoveredSignal().Connect(&application, functor);

  // Emit a started signal
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  END_TEST;
}

int UtcDaliHoverMultipleRenderableActors(void)
{
  TestApplication    application;
  Integration::Scene stage(application.GetScene());
  Vector2            stageSize(stage.GetSize());

  Actor parent = CreateRenderableActor();
  parent.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  parent.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  stage.Add(parent);

  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  parent.Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to layer's hovered signal
  SignalData        data;
  HoverEventFunctor functor(data);
  parent.HoveredSignal().Connect(&application, functor);
  actor.HoveredSignal().Connect(&application, functor);

  // Emit a started signal
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.hoveredActor);
  END_TEST;
}

int UtcDaliHoverActorRemovedInSignal(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal
  SignalData         data;
  RemoveActorFunctor functor(data);
  actor.HoveredSignal().Connect(&application, functor);

  // Register for leave events
  actor.SetProperty(Actor::Property::LEAVE_REQUIRED, true);

  // Emit a started signal
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Re-add, render and notify
  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render();

  // Emit another signal outside of actor's area, should not get anything as the scene has changed.
  application.ProcessEvent(GenerateSingleHover(PointState::MOTION, Vector2(210.0f, 210.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Emit a started signal
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit another signal outside of actor's area, should not get anything as the scene has changed.
  application.ProcessEvent(GenerateSingleHover(PointState::MOTION, Vector2(210.0f, 210.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Re-add actor back to stage, render and notify
  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render();

  // Emit another started event
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Completely delete the actor
  actor.Reset();

  // Emit event, should not crash and should not receive an event.
  application.ProcessEvent(GenerateSingleHover(PointState::MOTION, Vector2(210.0f, 210.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliHoverActorSignalNotConsumed(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal
  SignalData        data;
  HoverEventFunctor functor(data, false);
  actor.HoveredSignal().Connect(&application, functor);

  // Emit a started signal
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliHoverActorUnStaged(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal
  SignalData        data;
  HoverEventFunctor functor(data);
  actor.HoveredSignal().Connect(&application, functor);

  // Emit a started signal
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Remove actor from stage
  application.GetScene().Remove(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit a move at the same point, we should not be signalled.
  application.ProcessEvent(GenerateSingleHover(PointState::MOTION, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();
  END_TEST;
}

int UtcDaliHoverLeaveActorReadded(void)
{
  TestApplication    application;
  Integration::Scene stage = application.GetScene();

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  stage.Add(actor);

  // Set actor to receive hover-events
  actor.SetProperty(Actor::Property::LEAVE_REQUIRED, true);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal
  SignalData        data;
  HoverEventFunctor functor(data);
  actor.HoveredSignal().Connect(&application, functor);

  // Emit a started and motion
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  application.ProcessEvent(GenerateSingleHover(PointState::MOTION, Vector2(11.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Remove actor from stage and add again
  stage.Remove(actor);
  stage.Add(actor);

  // Emit a motion within the actor's bounds
  application.ProcessEvent(GenerateSingleHover(PointState::MOTION, Vector2(12.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Emit a motion outside the actor's bounds
  application.ProcessEvent(GenerateSingleHover(PointState::MOTION, Vector2(200.0f, 200.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::LEAVE, data.hoverEvent.GetState(0), TEST_LOCATION);
  data.Reset();

  END_TEST;
}

int UtcDaliHoverClippingActor(void)
{
  TestApplication    application;
  Integration::Scene stage = application.GetScene();

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  stage.Add(actor);

  Actor clippingActor = Actor::New();
  clippingActor.SetProperty(Actor::Property::SIZE, Vector2(50.0f, 50.0f));
  clippingActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  clippingActor.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);
  stage.Add(clippingActor);

  // Add a child to the clipped region.
  Actor clippingChild = Actor::New();
  clippingChild.SetProperty(Actor::Property::SIZE, Vector2(50.0f, 50.0f));
  clippingChild.SetProperty(Actor::Property::POSITION, Vector2(25.0f, 25.0f));
  clippingChild.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  clippingActor.Add(clippingChild);

  // Render and notify.
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal.
  SignalData        data;
  HoverEventFunctor functor(data);
  actor.HoveredSignal().Connect(&application, functor);

  // Emit an event within clipped area - we should have a hit.
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Emit an event outside the clipped area but within the actor area, we should have a hit.
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(60.0f, 60.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  clippingChild.HoveredSignal().Connect(&application, functor);

  // Emit an event inside part of the child which is within the clipped area, we should have a hit.
  application.ProcessEvent(GenerateSingleHover(PointState::STARTED, Vector2(30.0f, 30.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  END_TEST;
}

int UtcDaliHoverEventCreate(void)
{
  TestApplication application;

  Dali::HoverEvent hoverEvent = DevelHoverEvent::New(100);
  DALI_TEST_CHECK(hoverEvent);

  // Emit a started signal
  DALI_TEST_EQUALS(100, hoverEvent.GetTime(), TEST_LOCATION);
  DALI_TEST_EQUALS(0, hoverEvent.GetPointCount(), TEST_LOCATION);

  END_TEST;
}
