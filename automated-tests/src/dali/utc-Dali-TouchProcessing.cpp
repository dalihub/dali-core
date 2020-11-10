/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/touch-integ.h>
#include <dali/integration-api/render-task-list-integ.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>

using namespace Dali;

void utc_dali_touch_processing_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_touch_processing_cleanup(void)
{
  test_return_value = TET_PASS;
}

///////////////////////////////////////////////////////////////////////////////

namespace
{
struct TestPoint
{
  int32_t                deviceId{-1};
  PointState::Type       state{PointState::FINISHED};
  Actor                  hitActor;
  Vector2                local;
  Vector2                screen;
  float                  radius{0};
  Vector2                ellipseRadius;
  float                  pressure{0};
  Degree                 angle;
  Device::Class::Type    deviceClass{Device::Class::NONE};
  Device::Subclass::Type deviceSubclass{Device::Subclass::NONE};

  TestPoint() = default;
  static const TestPoint ZERO;
};

const TestPoint TestPoint::ZERO;

// Stores data that is populated in the callback and will be read by the TET cases
struct SignalData
{
  SignalData()
  : functorCalled(false),
    receivedTouch(),
    touchedActor()
  {
  }

  struct TestTouchEvent
  {
    unsigned long          time;
    std::vector<TestPoint> points;

    const TestPoint& GetPoint(size_t i)
    {
      if(i < points.size())
      {
        return points[i];
      }
      return TestPoint::ZERO;
    }
    size_t GetPointCount()
    {
      return points.size();
    }
  };

  void Reset()
  {
    functorCalled = false;

    receivedTouch.time = 0u;
    receivedTouch.points.clear();

    touchedActor.Reset();
  }

  bool           functorCalled;
  TestTouchEvent receivedTouch;
  Actor          touchedActor;
};

// Functor that sets the data when called
struct TouchEventFunctor
{
  /**
   * Constructor.
   * @param[in]  data         Reference to the data to store callback information.
   * @param[in]  returnValue  What the functor should return.
   */
  TouchEventFunctor(SignalData& data, bool returnValue = true)
  : signalData(data),
    returnValue(returnValue)
  {
  }

  bool operator()(Actor actor, const TouchEvent& touch)
  {
    signalData.functorCalled = true;
    signalData.touchedActor  = actor;

    signalData.receivedTouch.time = touch.GetTime();
    signalData.receivedTouch.points.clear();

    for(size_t i = 0; i < touch.GetPointCount(); ++i)
    {
      TestPoint p;
      p.deviceId       = touch.GetDeviceId(i);
      p.state          = touch.GetState(i);
      p.hitActor       = touch.GetHitActor(i);
      p.local          = touch.GetLocalPosition(i);
      p.screen         = touch.GetScreenPosition(i);
      p.radius         = touch.GetRadius(i);
      p.ellipseRadius  = touch.GetEllipseRadius(i);
      p.pressure       = touch.GetPressure(i);
      p.angle          = touch.GetAngle(i);
      p.deviceClass    = touch.GetDeviceClass(i);
      p.deviceSubclass = touch.GetDeviceSubclass(i);
      signalData.receivedTouch.points.push_back(p);
    }

    return returnValue;
  }

  SignalData& signalData;
  bool        returnValue;
};

struct HandleData
{
  bool       signalReceived;
  TouchEvent receivedTouchHandle;

  HandleData()
  : signalReceived(false)
  {
  }
};

struct TouchEventHandleFunctor
{
  /**
   * Constructor.
   * @param[in]  data         Reference to the data to store callback information.
   * @param[in]  returnValue  What the functor should return.
   */
  TouchEventHandleFunctor(HandleData& handleData, bool returnValue = true)
  : handleData(handleData),
    returnValue(returnValue)
  {
  }

  bool operator()(Actor actor, const TouchEvent& someTouchEvent)
  {
    handleData.signalReceived      = true;
    handleData.receivedTouchHandle = someTouchEvent;
    return returnValue;
  }

  HandleData& handleData;
  bool        returnValue;
};

// Functor that removes the actor when called.
struct RemoveActorFunctor : public TouchEventFunctor
{
  /**
   * Constructor.
   * @param[in]  data         Reference to the data to store callback information.
   * @param[in]  returnValue  What the functor should return.
   */
  RemoveActorFunctor(SignalData& data, bool returnValue = true)
  : TouchEventFunctor(data, returnValue)
  {
  }

  bool operator()(Actor actor, const TouchEvent& touch)
  {
    Actor parent(actor.GetParent());
    if(parent)
    {
      parent.Remove(actor);
    }

    return TouchEventFunctor::operator()(actor, touch);
  }
};

struct OutOfBoundsData
{
  TestPoint point;
  bool      functorCalled;

  OutOfBoundsData()
  : functorCalled(false)
  {
  }
};

// Functor that reads out of bounds data when called
struct OutOfBoundsFunctor
{
  /**
   * Constructor.
   * @param[in]  data         Reference to the data to store callback information.
   * @param[in]  returnValue  What the functor should return.
   */
  OutOfBoundsFunctor(OutOfBoundsData& data, bool returnValue = true)
  : outOfBoundsData(data),
    returnValue(returnValue)
  {
  }

  bool operator()(Actor actor, const TouchEvent& touch)
  {
    outOfBoundsData.functorCalled = true;
    size_t count                  = touch.GetPointCount();

    // Read out of bounds data
    outOfBoundsData.point.deviceId = touch.GetDeviceId(count + 1);
    outOfBoundsData.point.state    = touch.GetState(count + 1);
    outOfBoundsData.point.hitActor = touch.GetHitActor(count + 1);
    outOfBoundsData.point.local    = touch.GetLocalPosition(count + 1);
    outOfBoundsData.point.screen   = touch.GetScreenPosition(count + 1);

    return returnValue;
  }

  OutOfBoundsData& outOfBoundsData;
  bool             returnValue;
};

Integration::TouchEvent GenerateSingleTouch(PointState::Type state, const Vector2& screenPosition)
{
  Integration::TouchEvent touchEvent;
  Integration::Point      point;
  point.SetState(state);
  point.SetScreenPosition(screenPosition);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);
  return touchEvent;
}

} // namespace

///////////////////////////////////////////////////////////////////////////////

int UtcDaliTouchEventNormalProcessing01(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touch signal
  SignalData        data;
  TouchEventFunctor functor(data);
  actor.TouchedSignal().Connect(&application, functor);

  Vector2 screenCoordinates(10.0f, 10.0f);
  Vector2 localCoordinates;
  actor.ScreenToLocal(localCoordinates.x, localCoordinates.y, screenCoordinates.x, screenCoordinates.y);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, screenCoordinates));
  const TestPoint* point1 = &data.receivedTouch.GetPoint(0);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedTouch.GetPointCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, point1->state, TEST_LOCATION);
  DALI_TEST_EQUALS(screenCoordinates, point1->screen, TEST_LOCATION);
  DALI_TEST_EQUALS(localCoordinates, point1->local, 0.1f, TEST_LOCATION);
  data.Reset();

  // Emit a motion signal
  screenCoordinates.x = screenCoordinates.y = 11.0f;
  actor.ScreenToLocal(localCoordinates.x, localCoordinates.y, screenCoordinates.x, screenCoordinates.y);
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, screenCoordinates));
  const TestPoint* point2 = &data.receivedTouch.GetPoint(0);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedTouch.GetPointCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::MOTION, point2->state, TEST_LOCATION);
  DALI_TEST_EQUALS(screenCoordinates, point2->screen, TEST_LOCATION);
  DALI_TEST_EQUALS(localCoordinates, point2->local, 0.1f, TEST_LOCATION);
  data.Reset();

  // Emit an up signal
  screenCoordinates.x = screenCoordinates.y = 12.0f;
  actor.ScreenToLocal(localCoordinates.x, localCoordinates.y, screenCoordinates.x, screenCoordinates.y);
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, screenCoordinates));
  const TestPoint* point3 = &data.receivedTouch.GetPoint(0);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedTouch.GetPointCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::UP, point3->state, TEST_LOCATION);
  DALI_TEST_EQUALS(screenCoordinates, point3->screen, TEST_LOCATION);
  DALI_TEST_EQUALS(localCoordinates, point3->local, 0.1f, TEST_LOCATION);
  data.Reset();

  // Emit a down signal where the actor is not present
  screenCoordinates.x = screenCoordinates.y = 200.0f;
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, screenCoordinates));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTouchEventNormalProcessing02(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  HandleData              handleData;
  TouchEventHandleFunctor functor(handleData);
  actor.TouchedSignal().Connect(&application, functor);

  Vector2 screenCoordinates(10.0f, 10.0f);
  Vector2 localCoordinates;
  actor.ScreenToLocal(localCoordinates.x, localCoordinates.y, screenCoordinates.x, screenCoordinates.y);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, screenCoordinates));
  DALI_TEST_EQUALS(true, handleData.signalReceived, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, handleData.receivedTouchHandle.GetPointCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, handleData.receivedTouchHandle.GetState(0), TEST_LOCATION);
  DALI_TEST_EQUALS(screenCoordinates, handleData.receivedTouchHandle.GetScreenPosition(0), TEST_LOCATION);
  DALI_TEST_EQUALS(localCoordinates, handleData.receivedTouchHandle.GetLocalPosition(0), 0.1f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTouchEventAPINegative(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  OutOfBoundsData    data;
  OutOfBoundsFunctor functor(data, true);
  actor.TouchedSignal().Connect(&application, functor);

  Vector2 screenCoordinates(10.0f, 10.0f);
  Vector2 localCoordinates;
  actor.ScreenToLocal(localCoordinates.x, localCoordinates.y, screenCoordinates.x, screenCoordinates.y);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, screenCoordinates));

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(-1, data.point.deviceId, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::FINISHED, data.point.state, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2::ZERO, data.point.screen, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2::ZERO, data.point.local, 0.1f, TEST_LOCATION);
  DALI_TEST_CHECK(!data.point.hitActor);

  END_TEST;
}

int UtcDaliTouchEventOutsideCameraNearFarPlanes(void)
{
  TestApplication application;

  Integration::Scene scene     = application.GetScene();
  Vector2            sceneSize = scene.GetSize();

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  scene.Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Get the camera's near and far planes
  RenderTaskList   taskList  = scene.GetRenderTaskList();
  Dali::RenderTask task      = taskList.GetTask(0);
  CameraActor      camera    = task.GetCameraActor();
  float            nearPlane = camera.GetNearClippingPlane();
  float            farPlane  = camera.GetFarClippingPlane();

  // Calculate the current distance of the actor from the camera
  float tanHalfFov = tanf(camera.GetFieldOfView() * 0.5f);
  float distance   = (sceneSize.y * 0.5f) / tanHalfFov;

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data);
  actor.TouchedSignal().Connect(&application, functor);

  Vector2 screenCoordinates(sceneSize.x * 0.5f, sceneSize.y * 0.5f);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, screenCoordinates));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Emit a down signal where actor is just at the camera's near plane
  actor.SetProperty(Actor::Property::POSITION_Z, distance - nearPlane);

  // Render and notify
  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, screenCoordinates));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Emit a down signal where actor is closer than the camera's near plane
  actor.SetProperty(Actor::Property::POSITION_Z, (distance - nearPlane) + 1.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, screenCoordinates));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Emit a down signal where actor is just at the camera's far plane
  actor.SetProperty(Actor::Property::POSITION_Z, distance - farPlane);

  // Render and notify
  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, screenCoordinates));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Emit a down signal where actor is further than the camera's far plane
  actor.SetProperty(Actor::Property::POSITION_Z, (distance - farPlane) - 1.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, screenCoordinates));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();
  END_TEST;
}

int UtcDaliTouchEventEmitEmpty(void)
{
  TestApplication application;

  try
  {
    // Emit an empty TouchEvent
    Integration::TouchEvent event;
    application.ProcessEvent(event);
    tet_result(TET_FAIL);
  }
  catch(Dali::DaliException& e)
  {
    DALI_TEST_ASSERT(e, "!event.points.empty()", TEST_LOCATION);
  }
  END_TEST;
}

int UtcDaliTouchEventInterrupted(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data);
  actor.TouchedSignal().Connect(&application, functor);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, data.receivedTouch.points[0].state, TEST_LOCATION);
  data.Reset();

  // Emit an interrupted signal, we should be signalled regardless of whether there is a hit or not.
  application.ProcessEvent(GenerateSingleTouch(PointState::INTERRUPTED, Vector2(200.0f, 200.0f /* Outside actor */)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::INTERRUPTED, data.receivedTouch.points[0].state, TEST_LOCATION);
  data.Reset();

  // Emit another interrupted signal, our signal handler should not be called.
  application.ProcessEvent(GenerateSingleTouch(PointState::INTERRUPTED, Vector2(200.0f, 200.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTouchEventParentConsumer(void)
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

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data, false);
  actor.TouchedSignal().Connect(&application, functor);

  // Connect to root actor's touched signal
  SignalData        rootData;
  TouchEventFunctor rootFunctor(rootData); // Consumes signal
  rootActor.TouchedSignal().Connect(&application, rootFunctor);

  Vector2 screenCoordinates(10.0f, 10.0f);
  Vector2 actorCoordinates, rootCoordinates;
  actor.ScreenToLocal(actorCoordinates.x, actorCoordinates.y, screenCoordinates.x, screenCoordinates.y);
  rootActor.ScreenToLocal(rootCoordinates.x, rootCoordinates.y, screenCoordinates.x, screenCoordinates.y);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, screenCoordinates));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedTouch.GetPointCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, rootData.receivedTouch.GetPointCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, rootData.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(screenCoordinates, data.receivedTouch.points[0].screen, TEST_LOCATION);
  DALI_TEST_EQUALS(screenCoordinates, rootData.receivedTouch.points[0].screen, TEST_LOCATION);
  DALI_TEST_EQUALS(actorCoordinates, data.receivedTouch.points[0].local, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(rootCoordinates, rootData.receivedTouch.points[0].local, 0.1f, TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.receivedTouch.points[0].hitActor);
  DALI_TEST_CHECK(actor == rootData.receivedTouch.points[0].hitActor);
  data.Reset();
  rootData.Reset();

  // Emit a motion signal
  screenCoordinates.x = screenCoordinates.y = 11.0f;
  actor.ScreenToLocal(actorCoordinates.x, actorCoordinates.y, screenCoordinates.x, screenCoordinates.y);
  rootActor.ScreenToLocal(rootCoordinates.x, rootCoordinates.y, screenCoordinates.x, screenCoordinates.y);
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, screenCoordinates));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedTouch.GetPointCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, rootData.receivedTouch.GetPointCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::MOTION, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::MOTION, rootData.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(screenCoordinates, data.receivedTouch.points[0].screen, TEST_LOCATION);
  DALI_TEST_EQUALS(screenCoordinates, rootData.receivedTouch.points[0].screen, TEST_LOCATION);
  DALI_TEST_EQUALS(actorCoordinates, data.receivedTouch.points[0].local, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(rootCoordinates, rootData.receivedTouch.points[0].local, 0.1f, TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.receivedTouch.points[0].hitActor);
  DALI_TEST_CHECK(actor == rootData.receivedTouch.points[0].hitActor);
  data.Reset();
  rootData.Reset();

  // Emit an up signal
  screenCoordinates.x = screenCoordinates.y = 12.0f;
  actor.ScreenToLocal(actorCoordinates.x, actorCoordinates.y, screenCoordinates.x, screenCoordinates.y);
  rootActor.ScreenToLocal(rootCoordinates.x, rootCoordinates.y, screenCoordinates.x, screenCoordinates.y);
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, screenCoordinates));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedTouch.GetPointCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, rootData.receivedTouch.GetPointCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::UP, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::UP, rootData.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(screenCoordinates, data.receivedTouch.points[0].screen, TEST_LOCATION);
  DALI_TEST_EQUALS(screenCoordinates, rootData.receivedTouch.points[0].screen, TEST_LOCATION);
  DALI_TEST_EQUALS(actorCoordinates, data.receivedTouch.points[0].local, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(rootCoordinates, rootData.receivedTouch.points[0].local, 0.1f, TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.receivedTouch.points[0].hitActor);
  DALI_TEST_CHECK(actor == rootData.receivedTouch.points[0].hitActor);
  data.Reset();
  rootData.Reset();

  // Emit a down signal where the actor is not present, will hit the root actor though
  screenCoordinates.x = screenCoordinates.y = 200.0f;
  rootActor.ScreenToLocal(rootCoordinates.x, rootCoordinates.y, screenCoordinates.x, screenCoordinates.y);
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, screenCoordinates));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, rootData.receivedTouch.GetPointCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, rootData.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(screenCoordinates, rootData.receivedTouch.points[0].screen, TEST_LOCATION);
  DALI_TEST_EQUALS(rootCoordinates, rootData.receivedTouch.points[0].local, 0.1f, TEST_LOCATION);
  DALI_TEST_CHECK(rootActor == rootData.receivedTouch.points[0].hitActor);
  END_TEST;
}

int UtcDaliTouchEventInterruptedParentConsumer(void)
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

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data, false);
  actor.TouchedSignal().Connect(&application, functor);

  // Connect to root actor's touched signal
  SignalData        rootData;
  TouchEventFunctor rootFunctor(rootData); // Consumes signal
  rootActor.TouchedSignal().Connect(&application, rootFunctor);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, rootData.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.receivedTouch.points[0].hitActor);
  DALI_TEST_CHECK(actor == rootData.receivedTouch.points[0].hitActor);
  data.Reset();
  rootData.Reset();

  // Emit an interrupted signal
  application.ProcessEvent(GenerateSingleTouch(PointState::INTERRUPTED, Vector2(200.0f, 200.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::INTERRUPTED, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::INTERRUPTED, rootData.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.receivedTouch.points[0].hitActor);
  DALI_TEST_CHECK(actor == rootData.receivedTouch.points[0].hitActor);
  data.Reset();
  rootData.Reset();

  // Emit another down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, rootData.receivedTouch.points[0].state, TEST_LOCATION);
  data.Reset();
  rootData.Reset();

  // Remove actor from scene
  application.GetScene().Remove(actor);
  data.Reset();
  rootData.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit an interrupted signal, only root actor's signal should be called.
  application.ProcessEvent(GenerateSingleTouch(PointState::INTERRUPTED, Vector2(200.0f, 200.0f /* Outside actor */)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::INTERRUPTED, rootData.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_CHECK(rootActor == rootData.receivedTouch.points[0].hitActor);
  data.Reset();
  rootData.Reset();

  // Emit another interrupted state, none of the signal's should be called.
  application.ProcessEvent(GenerateSingleTouch(PointState::INTERRUPTED, Vector2(200.0f, 200.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, rootData.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTouchEventLeave(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data);
  actor.TouchedSignal().Connect(&application, functor);

  // Set actor to require leave events
  actor.SetProperty(Actor::Property::LEAVE_REQUIRED, true);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, data.receivedTouch.points[0].state, TEST_LOCATION);
  data.Reset();

  // Emit a motion signal outside of actor, should be signalled with a Leave
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(200.0f, 200.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::LEAVE, data.receivedTouch.points[0].state, TEST_LOCATION);
  data.Reset();

  // Another motion outside of actor, no signalling
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(201.0f, 201.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Another motion event inside actor, signalled with motion
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::MOTION, data.receivedTouch.points[0].state, TEST_LOCATION);
  data.Reset();

  // We do not want to listen to leave events anymore
  actor.SetProperty(Actor::Property::LEAVE_REQUIRED, false);

  // Another motion event outside of actor, no signalling
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(200.0f, 200.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();
  END_TEST;
}

int UtcDaliTouchEventLeaveParentConsumer(void)
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

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data, false);
  actor.TouchedSignal().Connect(&application, functor);

  // Connect to root actor's touched signal
  SignalData        rootData;
  TouchEventFunctor rootFunctor(rootData); // Consumes signal
  rootActor.TouchedSignal().Connect(&application, rootFunctor);

  // Set actor to require leave events
  actor.SetProperty(Actor::Property::LEAVE_REQUIRED, true);
  rootActor.SetProperty(Actor::Property::LEAVE_REQUIRED, true);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, rootData.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.receivedTouch.points[0].hitActor);
  DALI_TEST_CHECK(actor == rootData.receivedTouch.points[0].hitActor);
  data.Reset();
  rootData.Reset();

  // Emit a motion signal outside of actor, should be signalled with a Leave
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(200.0f, 200.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::LEAVE, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::LEAVE, rootData.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.receivedTouch.points[0].hitActor);
  DALI_TEST_CHECK(actor == rootData.receivedTouch.points[0].hitActor);
  data.Reset();
  rootData.Reset();

  // Another motion outside of actor, only rootActor signalled
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(201.0f, 201.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::MOTION, rootData.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_CHECK(rootActor == rootData.receivedTouch.points[0].hitActor);
  data.Reset();
  rootData.Reset();

  // Another motion event inside actor, signalled with motion
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::MOTION, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::MOTION, rootData.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.receivedTouch.points[0].hitActor);
  DALI_TEST_CHECK(actor == rootData.receivedTouch.points[0].hitActor);
  data.Reset();
  rootData.Reset();

  // We do not want to listen to leave events of actor anymore
  actor.SetProperty(Actor::Property::LEAVE_REQUIRED, false);

  // Another motion event outside of root actor, only root signalled
  Vector2 sceneSize(application.GetScene().GetSize());
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(sceneSize.width + 10.0f, sceneSize.height + 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::LEAVE, rootData.receivedTouch.points[0].state, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTouchEventActorBecomesInsensitive(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data);
  actor.TouchedSignal().Connect(&application, functor);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, data.receivedTouch.points[0].state, TEST_LOCATION);
  data.Reset();

  // Change actor to insensitive
  actor.SetProperty(Actor::Property::SENSITIVE, false);

  // Emit a motion signal, signalled with an interrupted
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(200.0f, 200.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::INTERRUPTED, data.receivedTouch.points[0].state, TEST_LOCATION);
  data.Reset();
  END_TEST;
}

int UtcDaliTouchEventActorBecomesInsensitiveParentConsumer(void)
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

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data, false);
  actor.TouchedSignal().Connect(&application, functor);

  // Connect to root actor's touched signal
  SignalData        rootData;
  TouchEventFunctor rootFunctor(rootData); // Consumes signal
  rootActor.TouchedSignal().Connect(&application, rootFunctor);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, rootData.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.receivedTouch.points[0].hitActor);
  DALI_TEST_CHECK(actor == rootData.receivedTouch.points[0].hitActor);
  data.Reset();
  rootData.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Make root actor insensitive
  rootActor.SetProperty(Actor::Property::SENSITIVE, false);

  // Emit a motion signal, signalled with an interrupted (should get interrupted even if within root actor)
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(200.0f, 200.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::INTERRUPTED, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::INTERRUPTED, rootData.receivedTouch.points[0].state, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTouchEventMultipleLayers(void)
{
  TestApplication application;
  Actor           rootActor(application.GetScene().GetRootLayer());

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data);

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
  layer1.TouchedSignal().Connect(&application, functor);
  actor1.TouchedSignal().Connect(&application, functor);

  // Hit in hittable area, actor1 should be hit
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(data.touchedActor == actor1);
  data.Reset();

  // Make layer1 insensitive, nothing should be hit
  layer1.SetProperty(Actor::Property::SENSITIVE, false);
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Make layer1 sensitive again, again actor1 will be hit
  layer1.SetProperty(Actor::Property::SENSITIVE, true);
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(data.touchedActor == actor1);
  data.Reset();

  // Make rootActor insensitive, nothing should be hit
  rootActor.SetProperty(Actor::Property::SENSITIVE, false);
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
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
  layer2.TouchedSignal().Connect(&application, functor);
  actor2.TouchedSignal().Connect(&application, functor);

  // Emit an event, should hit layer2
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  //DALI_TEST_CHECK( data.touchedActor == layer2 ); // TODO: Uncomment this after removing renderable hack!
  data.Reset();

  // Make layer2 insensitive, should hit actor1
  layer2.SetProperty(Actor::Property::SENSITIVE, false);
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(data.touchedActor == actor1);
  data.Reset();

  // Make layer2 sensitive again, should hit layer2
  layer2.SetProperty(Actor::Property::SENSITIVE, true);
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  //DALI_TEST_CHECK( data.touchedActor == layer2 ); // TODO: Uncomment this after removing renderable hack!
  data.Reset();

  // Make layer2 invisible, render and notify
  layer2.SetProperty(Actor::Property::VISIBLE, false);
  application.SendNotification();
  application.Render();

  // Should hit actor1
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(data.touchedActor == actor1);
  data.Reset();

  // Make rootActor invisible, render and notify
  rootActor.SetProperty(Actor::Property::VISIBLE, false);
  application.SendNotification();
  application.Render();

  // Should not hit anything
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();
  END_TEST;
}

int UtcDaliTouchEventMultipleRenderTasks(void)
{
  TestApplication    application;
  Integration::Scene scene(application.GetScene());
  Vector2            sceneSize(scene.GetSize());

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  scene.Add(actor);

  // Create render task
  Viewport   viewport(sceneSize.width * 0.5f, sceneSize.height * 0.5f, sceneSize.width * 0.5f, sceneSize.height * 0.5f);
  RenderTask renderTask(application.GetScene().GetRenderTaskList().CreateTask());
  renderTask.SetViewport(viewport);
  renderTask.SetInputEnabled(true);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data);
  actor.TouchedSignal().Connect(&application, functor);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Ensure renderTask actor can be hit too.
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(viewport.x + 5.0f, viewport.y + 5.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Disable input on renderTask, should not be hittable
  renderTask.SetInputEnabled(false);
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(viewport.x + 5.0f, viewport.y + 5.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();
  END_TEST;
}

int UtcDaliTouchEventMultipleRenderTasksWithChildLayer(void)
{
  TestApplication    application;
  Integration::Scene scene(application.GetScene());
  Vector2            sceneSize(scene.GetSize());

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  scene.Add(actor);

  Layer layer = Layer::New();
  layer.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  layer.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.Add(layer);

  // Create render task
  Viewport   viewport(sceneSize.width * 0.5f, sceneSize.height * 0.5f, sceneSize.width * 0.5f, sceneSize.height * 0.5f);
  RenderTask renderTask(application.GetScene().GetRenderTaskList().CreateTask());
  renderTask.SetViewport(viewport);
  renderTask.SetInputEnabled(true);
  renderTask.SetSourceActor(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to layer's touched signal
  SignalData        data;
  TouchEventFunctor functor(data);
  actor.TouchedSignal().Connect(&application, functor);
  layer.TouchedSignal().Connect(&application, functor);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Ensure renderTask actor can be hit too.
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(viewport.x + 5.0f, viewport.y + 5.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Disable input on renderTask, should not be hittable
  renderTask.SetInputEnabled(false);
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(viewport.x + 5.0f, viewport.y + 5.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();
  END_TEST;
}

int UtcDaliTouchEventOffscreenRenderTasks(void)
{
  TestApplication    application;
  Integration::Scene scene(application.GetScene());
  Vector2            sceneSize(scene.GetSize());

  // FrameBufferImage for offscreen RenderTask
  FrameBuffer frameBuffer = FrameBuffer::New(sceneSize.width, sceneSize.height);

  // Create a renderable actor to display the FrameBufferImage
  Actor renderableActor = CreateRenderableActor(frameBuffer.GetColorTexture());
  renderableActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  renderableActor.SetProperty(Actor::Property::SIZE, Vector2(sceneSize.x, sceneSize.y));
  renderableActor.ScaleBy(Vector3(1.0f, -1.0f, 1.0f)); // FIXME
  scene.Add(renderableActor);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  scene.Add(actor);
  application.GetGlAbstraction().SetCheckFramebufferStatusResult(GL_FRAMEBUFFER_COMPLETE); // Ensure framebuffer connects

  scene.GetRenderTaskList().GetTask(0u).SetScreenToFrameBufferFunction(RenderTask::FULLSCREEN_FRAMEBUFFER_FUNCTION);

  // Create a RenderTask
  RenderTask renderTask = scene.GetRenderTaskList().CreateTask();
  renderTask.SetSourceActor(actor);
  renderTask.SetFrameBuffer(frameBuffer);
  renderTask.SetInputEnabled(true);

  // Create another RenderTask
  RenderTask renderTask2(scene.GetRenderTaskList().CreateTask());
  renderTask2.SetInputEnabled(true);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data);
  actor.TouchedSignal().Connect(&application, functor);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  END_TEST;
}

int UtcDaliTouchEventMultipleRenderableActors(void)
{
  TestApplication    application;
  Integration::Scene scene(application.GetScene());
  Vector2            sceneSize(scene.GetSize());

  Actor parent = CreateRenderableActor();
  parent.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  parent.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  scene.Add(parent);

  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  parent.Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to layer's touched signal
  SignalData        data;
  TouchEventFunctor functor(data);
  parent.TouchedSignal().Connect(&application, functor);
  actor.TouchedSignal().Connect(&application, functor);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.touchedActor);
  END_TEST;
}

int UtcDaliTouchEventActorRemovedInSignal(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData         data;
  RemoveActorFunctor functor(data);
  actor.TouchedSignal().Connect(&application, functor);

  // Register for leave events
  actor.SetProperty(Actor::Property::LEAVE_REQUIRED, true);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Re-add, render and notify
  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render();

  // Emit another signal outside of actor's area, should not get anything as the scene has changed.
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(210.0f, 210.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit another signal outside of actor's area, should not get anything as the scene has changed.
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(210.0f, 210.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Re-add actor back to scene, render and notify
  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render();

  // Emit another down event
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Completely delete the actor
  actor.Reset();

  // Emit event, should not crash and should not receive an event.
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(210.0f, 210.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTouchEventActorSignalNotConsumed(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data, false);
  actor.TouchedSignal().Connect(&application, functor);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTouchEventActorRemovedFromScene(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data);
  actor.TouchedSignal().Connect(&application, functor);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Remove actor from scene
  application.GetScene().Remove(actor);
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit a move at the same point, we should not be signalled.
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();
  END_TEST;
}

int UtcDaliTouchEventLayerConsumesTouch(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data);
  actor.TouchedSignal().Connect(&application, functor);

  // Add a layer to overlap the actor
  Layer layer = Layer::New();
  layer.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  layer.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(layer);
  layer.RaiseToTop();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit a few touch signals
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Set layer to consume all touch
  layer.SetProperty(Layer::Property::CONSUMES_TOUCH, true);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit the same signals again, should not receive
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  END_TEST;
}

int UtcDaliTouchEventLeaveActorReadded(void)
{
  TestApplication    application;
  Integration::Scene scene = application.GetScene();

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  scene.Add(actor);

  // Set actor to receive touch-events
  actor.SetProperty(Actor::Property::LEAVE_REQUIRED, true);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data);
  actor.TouchedSignal().Connect(&application, functor);

  // Emit a down and motion
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(11.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Remove actor from scene and add again
  scene.Remove(actor);
  scene.Add(actor);

  // Emit a motion within the actor's bounds
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(12.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Emit a motion outside the actor's bounds
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(200.0f, 200.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::LEAVE, data.receivedTouch.points[0].state, TEST_LOCATION);
  data.Reset();

  END_TEST;
}

int UtcDaliTouchEventClippedActor(void)
{
  TestApplication    application;
  Integration::Scene scene = application.GetScene();

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  scene.Add(actor);

  Actor clippingActor = Actor::New();
  clippingActor.SetProperty(Actor::Property::SIZE, Vector2(50.0f, 50.0f));
  clippingActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  clippingActor.SetProperty(Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN);
  scene.Add(clippingActor);

  // Add a child to the clipped region.
  Actor clippingChild = Actor::New();
  clippingChild.SetProperty(Actor::Property::SIZE, Vector2(50.0f, 50.0f));
  clippingChild.SetProperty(Actor::Property::POSITION, Vector2(25.0f, 25.0f));
  clippingChild.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  clippingActor.Add(clippingChild);

  // Render and notify.
  application.SendNotification();
  application.Render();

  // Connect to actor's touch signal.
  SignalData        data;
  TouchEventFunctor functor(data);
  actor.TouchedSignal().Connect(&application, functor);

  // Emit an event within clipped area - no hit.
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Emit an event outside the clipped area but within the actor area, we should have a hit.
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(60.0f, 60.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  clippingChild.TouchedSignal().Connect(&application, functor);

  // Emit an event inside part of the child which is within the clipped area, we should have a hit.
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(30.0f, 30.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  END_TEST;
}

int UtcDaliTouchEventActorUnparented(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data);
  actor.TouchedSignal().Connect(&application, functor);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.receivedTouch.points[0].hitActor);
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Unparent the actor
  actor.Unparent();

  // Should receive an interrupted event
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::INTERRUPTED, data.receivedTouch.points[0].state, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTouchEventParentRemovedFromScene(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  parent.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  parent.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(parent);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  parent.Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data);
  actor.TouchedSignal().Connect(&application, functor);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.receivedTouch.points[0].hitActor);
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Unparent the parent of the touchable actor
  parent.Unparent();

  // Should receive an interrupted event
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::INTERRUPTED, data.receivedTouch.points[0].state, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTouchEventActorRemovedFromSceneDifferentConsumer(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  parent.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  parent.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(parent);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  parent.Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data, false /* Do not consume */);
  actor.TouchedSignal().Connect(&application, functor);

  // Connect to parent's touched signal
  SignalData        parentData;
  TouchEventFunctor parentFunctor(parentData);
  parent.TouchedSignal().Connect(&application, parentFunctor);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.receivedTouch.points[0].hitActor);
  DALI_TEST_CHECK(actor == data.touchedActor);
  DALI_TEST_EQUALS(true, parentData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, parentData.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_CHECK(actor == parentData.receivedTouch.points[0].hitActor);
  DALI_TEST_CHECK(parent == parentData.touchedActor);
  data.Reset();
  parentData.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Unparent the actor
  actor.Unparent();

  // Should receive an interrupted event for both actor & parent
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::INTERRUPTED, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(true, parentData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::INTERRUPTED, parentData.receivedTouch.points[0].state, TEST_LOCATION);
  data.Reset();
  parentData.Reset();

  // Readd actor to parent
  parent.Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit a motion signal
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, parentData.functorCalled, TEST_LOCATION);
  data.Reset();
  parentData.Reset();

  // Parent is now consumer, connect again to the touched signal of the actor so that it becomes the consumer
  SignalData        secondData;
  TouchEventFunctor secondFunctor(secondData /* Consume */);
  actor.TouchedSignal().Connect(&application, secondFunctor);

  // Unparent the actor
  actor.Unparent();

  // Should receive an interrupted event for both actor functors & the parent as well as it was last consumer
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::INTERRUPTED, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(true, parentData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::INTERRUPTED, parentData.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::INTERRUPTED, secondData.receivedTouch.points[0].state, TEST_LOCATION);
  data.Reset();
  parentData.Reset();
  secondData.Reset();

  END_TEST;
}

int UtcDaliTouchEventInterruptedDifferentConsumer(void)
{
  TestApplication application;
  Actor           rootActor(application.GetScene().GetRootLayer());

  Actor parent = Actor::New();
  parent.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  parent.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(parent);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  parent.Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data, false /* Do not consume */);
  actor.TouchedSignal().Connect(&application, functor);

  // Connect to parent's touched signal
  SignalData        parentData;
  TouchEventFunctor parentFunctor(parentData, false /* Do not consume */);
  parent.TouchedSignal().Connect(&application, parentFunctor);

  // Connect to root's touched signal and consume
  SignalData        rootData;
  TouchEventFunctor rootFunctor(rootData);
  rootActor.TouchedSignal().Connect(&application, rootFunctor);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.receivedTouch.points[0].hitActor);
  DALI_TEST_CHECK(actor == data.touchedActor);
  DALI_TEST_EQUALS(true, parentData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, parentData.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_CHECK(actor == parentData.receivedTouch.points[0].hitActor);
  DALI_TEST_CHECK(parent == parentData.touchedActor);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, rootData.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_CHECK(actor == rootData.receivedTouch.points[0].hitActor);
  DALI_TEST_CHECK(rootActor == rootData.touchedActor);
  data.Reset();
  parentData.Reset();
  rootData.Reset();

  // Root is now consumer, connect to the touched signal of the parent so that it becomes the consumer
  SignalData        secondData;
  TouchEventFunctor secondFunctor(secondData /* Consume */);
  parent.TouchedSignal().Connect(&application, secondFunctor);

  // Emit an interrupted signal, all three should STILL be called
  application.ProcessEvent(GenerateSingleTouch(PointState::INTERRUPTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::INTERRUPTED, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(true, parentData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::INTERRUPTED, parentData.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(true, rootData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::INTERRUPTED, rootData.receivedTouch.points[0].state, TEST_LOCATION);
  data.Reset();
  parentData.Reset();
  rootData.Reset();

  END_TEST;
}

int UtcDaliTouchEventGetRadius(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data);
  actor.TouchedSignal().Connect(&application, functor);

  // Emit a down signal with an angle
  Integration::TouchEvent touchEvent = GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f));
  touchEvent.points[0].SetRadius(100.0f);
  application.ProcessEvent(touchEvent);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(100.0f, data.receivedTouch.points[0].radius, TEST_LOCATION);
  DALI_TEST_EQUALS(100.0f, data.receivedTouch.points[0].ellipseRadius.x, TEST_LOCATION);
  DALI_TEST_EQUALS(100.0f, data.receivedTouch.points[0].ellipseRadius.y, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTouchEventGetEllipseRadius(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data);
  actor.TouchedSignal().Connect(&application, functor);

  // Emit a down signal with an angle
  Integration::TouchEvent touchEvent = GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f));
  touchEvent.points[0].SetRadius(100.0f, Vector2(20.0f, 10.0f));
  application.ProcessEvent(touchEvent);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(100.0f, data.receivedTouch.points[0].radius, TEST_LOCATION);
  DALI_TEST_EQUALS(20.0f, data.receivedTouch.points[0].ellipseRadius.x, TEST_LOCATION);
  DALI_TEST_EQUALS(10.0f, data.receivedTouch.points[0].ellipseRadius.y, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTouchEventGetAngle(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data);
  actor.TouchedSignal().Connect(&application, functor);

  // Emit a down signal with an angle
  Integration::TouchEvent touchEvent = GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f));
  touchEvent.points[0].SetAngle(Degree(90.0f));
  application.ProcessEvent(touchEvent);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(Degree(90.0f), data.receivedTouch.points[0].angle, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTouchEventGetPressure(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data);
  actor.TouchedSignal().Connect(&application, functor);

  // Emit a down signal with an angle
  Integration::TouchEvent touchEvent = GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f));
  touchEvent.points[0].SetPressure(10.0f);
  application.ProcessEvent(touchEvent);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_EQUALS(10.0f, data.receivedTouch.points[0].pressure, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTouchEventUsage(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data);
  actor.TouchedSignal().Connect(&application, functor);

  // Emit a down signal with an angle
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTouchEventGetDeviceAPINegative(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  HandleData              handleData;
  TouchEventHandleFunctor functor(handleData);
  actor.TouchedSignal().Connect(&application, functor);

  Vector2 screenCoordinates(10.0f, 10.0f);
  Vector2 localCoordinates;
  actor.ScreenToLocal(localCoordinates.x, localCoordinates.y, screenCoordinates.x, screenCoordinates.y);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, screenCoordinates));

  TouchEvent data = handleData.receivedTouchHandle;
  DALI_TEST_EQUALS(data.GetDeviceClass(-1), Device::Class::NONE, TEST_LOCATION);
  DALI_TEST_EQUALS(data.GetDeviceSubclass(-1), Device::Subclass::NONE, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTouchEventGetMouseButtonPositive(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  HandleData              handleData;
  TouchEventHandleFunctor functor(handleData);
  actor.TouchedSignal().Connect(&application, functor);

  // Emit a down signal with MouseButton
  Integration::TouchEvent touchEvent = GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f));
  touchEvent.points[0].SetMouseButton(static_cast<MouseButton::Type>(3));
  application.ProcessEvent(touchEvent);

  TouchEvent data = handleData.receivedTouchHandle;
  DALI_TEST_EQUALS(data.GetMouseButton(0), MouseButton::SECONDARY, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTouchEventGetMouseButtonNagative(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  HandleData              handleData;
  TouchEventHandleFunctor functor(handleData);
  actor.TouchedSignal().Connect(&application, functor);

  // Emit a down signal with MouseButton
  Integration::TouchEvent touchEvent = GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f));
  touchEvent.points[0].SetMouseButton(static_cast<MouseButton::Type>(2));
  application.ProcessEvent(touchEvent);

  TouchEvent data = handleData.receivedTouchHandle;
  DALI_TEST_EQUALS(data.GetMouseButton(0), MouseButton::TERTIARY, TEST_LOCATION);
  DALI_TEST_EQUALS(data.GetMouseButton(3), MouseButton::INVALID, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTouchEventCapturePropertySet(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data);
  actor.TouchedSignal().Connect(&application, functor);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Now motion outside of actor, we should not receive the event
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(110.0f, 110.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Up event, should receive an interrupted
  application.ProcessEvent(GenerateSingleTouch(PointState::FINISHED, Vector2(110.0f, 110.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(data.receivedTouch.GetPoint(0).state, PointState::INTERRUPTED, TEST_LOCATION);

  // Now set the capture property
  actor.SetProperty(DevelActor::Property::CAPTURE_ALL_TOUCH_AFTER_START, true);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::STARTED, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Now motion outside of actor, we now SHOULD receive the event
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(110.0f, 110.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Up event, we should receive it again, but as ended rather than interrupted
  application.ProcessEvent(GenerateSingleTouch(PointState::FINISHED, Vector2(110.0f, 110.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(data.receivedTouch.GetPoint(0).state, PointState::FINISHED, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTouchEventIntegNewTouchEvent(void)
{
  uint32_t         timestamp = 92858u;
  TouchPoint       tp(1, PointState::STARTED, 34.4f, 123.89f, 5.0f, 7.0f);
  Dali::TouchEvent touchEvent = Integration::NewTouchEvent(timestamp, tp);

  DALI_TEST_EQUALS(touchEvent.GetPointCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(touchEvent.GetState(0), PointState::STARTED, TEST_LOCATION);
  DALI_TEST_EQUALS(touchEvent.GetLocalPosition(0), Vector2(5.0f, 7.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(touchEvent.GetScreenPosition(0), Vector2(34.4f, 123.89f), TEST_LOCATION);

  END_TEST;
}


int UtcDaliTouchEventIntercept(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  parent.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  parent.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(parent);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  parent.Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data, false /* Do not consume */);
  actor.TouchedSignal().Connect(&application, functor);


  // Connect to parent's touched signal
  SignalData        parentData;
  TouchEventFunctor parentFunctor(parentData, false /* Do not consume */);
  parent.TouchedSignal().Connect(&application, parentFunctor);
  // Connect to parent's intercept touched signal
  SignalData        interceptData;
  TouchEventFunctor interceptFunctor(interceptData, true /* Do intercept */);
  Dali::DevelActor::InterceptTouchedSignal(parent).Connect(&application, interceptFunctor);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  // The actor touched signal is not called because the touch is intercepted in the parent.
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, interceptData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, interceptData.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_CHECK(actor == interceptData.receivedTouch.points[0].hitActor);
  DALI_TEST_CHECK(parent == interceptData.touchedActor);
  DALI_TEST_EQUALS(true, parentData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, parentData.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_CHECK(actor == parentData.receivedTouch.points[0].hitActor);
  DALI_TEST_CHECK(parent == parentData.touchedActor);
  data.Reset();
  parentData.Reset();

  END_TEST;
}

int UtcDaliTouchArea(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData        data;
  TouchEventFunctor functor(data, false /* Do not consume */);
  actor.TouchedSignal().Connect(&application, functor);

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(110.0f, 110.0f)));
  // The actor touched signal is not called because the touch area is outside actor.
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // set a bigger touch area
  actor.SetProperty(DevelActor::Property::TOUCH_AREA, Vector2(200.0f, 200.0f));

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(110.0f, 110.0f)));
  // The actor touched signal is called because the touch area is inside touchArea.
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.receivedTouch.points[0].hitActor);
  data.Reset();

  // set a smaller touch area
  actor.SetProperty(DevelActor::Property::TOUCH_AREA, Vector2(50.0f, 50.0f));

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(80.0f, 80.0f)));
  // The actor touched signal is not called because the touch area is outside touchArea.
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Emit a down signal
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(30.0f, 30.0f)));
  // The actor touched signal is called because the touch area is inside touchArea.
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, data.receivedTouch.points[0].state, TEST_LOCATION);
  DALI_TEST_CHECK(actor == data.receivedTouch.points[0].hitActor);
  data.Reset();

  END_TEST;
}
