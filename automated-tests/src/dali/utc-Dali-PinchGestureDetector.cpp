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
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/touch-integ.h>
#include <dali/integration-api/render-task-list-integ.h>
#include <dali/internal/event/events/touch-event-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>
#include <test-touch-event-utils.h>

#include <iostream>

using namespace Dali;

void utc_dali_pinch_gesture_detector_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_pinch_gesture_detector_cleanup(void)
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
    voidFunctorCalled(false),
    needGesturePropagation(false),
    receivedGesture()
  {
  }

  void Reset()
  {
    functorCalled     = false;
    voidFunctorCalled = false;
    needGesturePropagation = false;
    receivedGesture.Reset();

    pinchedActor.Reset();
  }

  bool         functorCalled;
  bool         voidFunctorCalled;
  bool         needGesturePropagation;
  PinchGesture receivedGesture;
  Actor        pinchedActor;
};

// Functor that sets the data when called
struct GestureReceivedFunctor
{
  GestureReceivedFunctor(SignalData& data)
  : signalData(data)
  {
  }

  void operator()(Actor actor, const PinchGesture& pinch)
  {
    signalData.functorCalled   = true;
    signalData.receivedGesture = pinch;
    signalData.pinchedActor    = actor;
    if (signalData.needGesturePropagation)
    {
      Dali::DevelActor::SetNeedGesturePropagation(actor, true);
    }
  }

  void operator()()
  {
    signalData.voidFunctorCalled = true;
  }

  SignalData& signalData;
};

// Functor that removes the gestured actor from stage
struct UnstageActorFunctor : public GestureReceivedFunctor
{
  UnstageActorFunctor(SignalData& data, GestureState& stateToUnstage, Integration::Scene scene)
  : GestureReceivedFunctor(data),
    stateToUnstage(stateToUnstage),
    scene(scene)
  {
  }

  void operator()(Actor actor, const PinchGesture& pinch)
  {
    GestureReceivedFunctor::operator()(actor, pinch);

    if(pinch.GetState() == stateToUnstage)
    {
      scene.Remove(actor);
    }
  }

  GestureState&      stateToUnstage;
  Integration::Scene scene;
};


Integration::TouchEvent GenerateDoubleTouch(PointState::Type stateA, const Vector2& screenPositionA, PointState::Type stateB, const Vector2& screenPositionB, uint32_t time)
{
  Integration::TouchEvent touchEvent;
  Integration::Point      point;
  point.SetState(stateA);
  point.SetScreenPosition(screenPositionA);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);
  point.SetScreenPosition(screenPositionB);
  point.SetState(stateB);
  touchEvent.points.push_back(point);
  touchEvent.time = time;
  return touchEvent;
}

} // namespace

///////////////////////////////////////////////////////////////////////////////

int UtcDaliPinchGestureDetectorConstructor(void)
{
  TestApplication application;

  PinchGestureDetector detector;
  DALI_TEST_CHECK(!detector);
  END_TEST;
}

int UtcDaliPinchGestureDetectorCopyConstructorP(void)
{
  TestApplication application;

  PinchGestureDetector detector = PinchGestureDetector::New();

  PinchGestureDetector copy(detector);
  DALI_TEST_CHECK(detector);
  END_TEST;
}

int UtcDaliPinchGestureDetectorAssignmentOperatorP(void)
{
  TestApplication application;

  PinchGestureDetector detector = PinchGestureDetector::New();

  PinchGestureDetector assign;
  assign = detector;
  DALI_TEST_CHECK(detector);

  DALI_TEST_CHECK(detector == assign);
  END_TEST;
}

int UtcDaliPinchGestureDetectorMoveConstructorP(void)
{
  TestApplication application;

  PinchGestureDetector detector = PinchGestureDetector::New();
  DALI_TEST_CHECK(detector);

  PinchGestureDetector moved = std::move(detector);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!detector);
  END_TEST;
}

int UtcDaliPinchGestureDetectorMoveAssignmentOperatorP(void)
{
  TestApplication application;

  PinchGestureDetector detector;
  detector = PinchGestureDetector::New();
  DALI_TEST_CHECK(detector);

  PinchGestureDetector moved;
  moved = std::move(detector);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!detector);
  END_TEST;
}

int UtcDaliPinchGestureDetectorNew(void)
{
  TestApplication application;

  PinchGestureDetector detector = PinchGestureDetector::New();

  DALI_TEST_CHECK(detector);

  // Attach an actor and emit a touch event on the actor to ensure complete line coverage
  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  detector.Attach(actor);

  Integration::TouchEvent touchEvent(1);
  Integration::Point      point;
  point.SetDeviceId(1);
  point.SetState(PointState::DOWN);
  point.SetScreenPosition(Vector2(20.0f, 20.0f));
  touchEvent.AddPoint(point);
  application.ProcessEvent(touchEvent);

  Integration::Point point2;
  point.SetDeviceId(1);
  point.SetState(PointState::DOWN);
  point.SetScreenPosition(Vector2(20.0f, 20.0f));
  touchEvent.AddPoint(point2);
  application.ProcessEvent(touchEvent);
  END_TEST;
}

int UtcDaliPinchGestureDetectorDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::PinchGestureDetector::DownCast()");

  PinchGestureDetector detector = PinchGestureDetector::New();

  BaseHandle object(detector);

  PinchGestureDetector detector2 = PinchGestureDetector::DownCast(object);
  DALI_TEST_CHECK(detector2);

  PinchGestureDetector detector3 = DownCast<PinchGestureDetector>(object);
  DALI_TEST_CHECK(detector3);

  BaseHandle           unInitializedObject;
  PinchGestureDetector detector4 = PinchGestureDetector::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!detector4);

  PinchGestureDetector detector5 = DownCast<PinchGestureDetector>(unInitializedObject);
  DALI_TEST_CHECK(!detector5);

  GestureDetector      detector6 = PinchGestureDetector::New();
  PinchGestureDetector detector7 = PinchGestureDetector::DownCast(detector6);
  DALI_TEST_CHECK(detector7);
  END_TEST;
}

// Negative test case for a method
int UtcDaliPinchGestureSignalReceptionNegative(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Do a pinch outside actor's area
  TestStartPinch(application, Vector2(112.0f, 62.0f), Vector2(112.0f, 162.0f), Vector2(112.0f, 100.0f), Vector2(112.0f, 124.0f), 100);

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Continue pinch into actor's area - we should still not receive the signal
  data.Reset();
  TestContinuePinch(application, Vector2(112.0f, 100.0f), Vector2(112.0f, 124.0f), Vector2(5.0f, 5.0f), Vector2(35.0f, 35.0f), 200);

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Stop pinching - we should still not receive the signal
  data.Reset();
  TestEndPinch(application, Vector2(6.0f, 6.0f), Vector2(18.0f, 18.0f), Vector2(10.0f, 8.0f), Vector2(14.0f, 16.0f), 300);

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionDownMotionLeave(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pan within the actor's area
  TestStartPinch(application, Vector2(5.0f, 20.0f), Vector2(35.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 100);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::STARTED, data.receivedGesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(0.666f, data.receivedGesture.GetScale(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(66.666f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.GetScreenCenterPoint(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.GetLocalCenterPoint(), 0.01f, TEST_LOCATION);

  // Continue the pan within the actor's area - we should still receive the signal
  data.Reset();
  TestContinuePinch(application, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), Vector2(17.0f, 20.0f), Vector2(25.0f, 20.0f), 400);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::CONTINUING, data.receivedGesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(0.2666f, data.receivedGesture.GetScale(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(80.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(21.0f, 20.0f), data.receivedGesture.GetScreenCenterPoint(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(21.0f, 20.0f), data.receivedGesture.GetLocalCenterPoint(), 0.01f, TEST_LOCATION);

  // Pinch Gesture leaves actor's area - we should still receive the signal
  data.Reset();
  TestContinuePinch(application, Vector2(17.0f, 20.0f), Vector2(25.0f, 20.0f), Vector2(300.0f, 10.0f), Vector2(340.0f, 10.0f), 1000);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::CONTINUING, data.receivedGesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(1.333f, data.receivedGesture.GetScale(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(213.333f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(320.0f, 10.0f), data.receivedGesture.GetScreenCenterPoint(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(320.0f, 10.0f), data.receivedGesture.GetLocalCenterPoint(), 0.01f, TEST_LOCATION);

  // Gesture ends - we would receive a finished state
  data.Reset();
  TestEndPinch(application, Vector2(300.0f, 10.0f), Vector2(340.0f, 10.0f), Vector2(305.0f, 10.0f), Vector2(315.0f, 10.0f), 1500);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::FINISHED, data.receivedGesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(0.333f, data.receivedGesture.GetScale(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(600.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(310.0f, 10.0f), data.receivedGesture.GetScreenCenterPoint(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(310.0f, 10.0f), data.receivedGesture.GetLocalCenterPoint(), 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionDownMotionUp(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pinch within the actor's area
  TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 100);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::STARTED, data.receivedGesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(0.555f, data.receivedGesture.GetScale(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(106.667f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.GetScreenCenterPoint(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.GetLocalCenterPoint(), 0.01f, TEST_LOCATION);

  // Continue the pinch within the actor's area - we should still receive the signal
  data.Reset();
  TestContinuePinch(application, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), 500);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::CONTINUING, data.receivedGesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(0.277f, data.receivedGesture.GetScale(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(66.666f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.GetScreenCenterPoint(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.GetLocalCenterPoint(), 0.01f, TEST_LOCATION);

  // Gesture ends within actor's area - we would receive a finished state
  data.Reset();
  TestEndPinch(application, Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), Vector2(19.0f, 20.0f), Vector2(21.0f, 20.0f), 1000);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::FINISHED, data.receivedGesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(0.055f, data.receivedGesture.GetScale(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(160.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.GetScreenCenterPoint(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.GetLocalCenterPoint(), 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionDetach(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pinch within the actor's area
  TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 100);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::STARTED, data.receivedGesture.GetState(), TEST_LOCATION);

  // Continue the pinch within the actor's area - we should still receive the signal
  data.Reset();
  TestContinuePinch(application, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), 500);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::CONTINUING, data.receivedGesture.GetState(), TEST_LOCATION);

  // Gesture ends within actor's area
  data.Reset();
  TestEndPinch(application, Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), Vector2(19.0f, 20.0f), Vector2(21.0f, 20.0f), 1000);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::FINISHED, data.receivedGesture.GetState(), TEST_LOCATION);

  // Detach actor
  detector.DetachAll();

  // Ensure we are no longer signalled
  data.Reset();
  TestGeneratePinch(application);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionDetachWhilePinching(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pinch within the actor's area
  TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 100);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::STARTED, data.receivedGesture.GetState(), TEST_LOCATION);

  // Continue the pinch within the actor's area - we should still receive the signal
  data.Reset();
  TestContinuePinch(application, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), 500);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::CONTINUING, data.receivedGesture.GetState(), TEST_LOCATION);

  // Detach actor during the pinch, we should not receive the next event
  detector.DetachAll();

  // Gesture ends within actor's area
  data.Reset();
  TestEndPinch(application, Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), Vector2(19.0f, 20.0f), Vector2(21.0f, 20.0f), 1000);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionActorDestroyedWhilePinching(void)
{
  TestApplication application;

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.DetectedSignal().Connect(&application, functor);

  // Attach a temporary actor to stop detector being removed from PinchGestureProcessor when main actor
  // is destroyed.
  Actor tempActor = Actor::New();
  tempActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  tempActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_RIGHT);
  application.GetScene().Add(tempActor);
  detector.Attach(tempActor);

  // Actor lifetime is scoped
  {
    Actor actor = Actor::New();
    actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
    actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
    application.GetScene().Add(actor);

    // Render and notify
    application.SendNotification();
    application.Render();

    detector.Attach(actor);

    // Start pinch within the actor's area
    TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 100);
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_EQUALS(GestureState::STARTED, data.receivedGesture.GetState(), TEST_LOCATION);

    // Continue the pinch within the actor's area - we should still receive the signal
    data.Reset();
    TestContinuePinch(application, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), 500);
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_EQUALS(GestureState::CONTINUING, data.receivedGesture.GetState(), TEST_LOCATION);

    // Remove the actor from stage and reset the data
    application.GetScene().Remove(actor);

    // Render and notify
    application.SendNotification();
    application.Render();
  }

  // Actor should now have been destroyed

  // Gesture ends within the area where the actor used to be
  data.Reset();
  TestEndPinch(application, Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), Vector2(19.0f, 20.0f), Vector2(21.0f, 20.0f), 1000);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionRotatedActor(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ORIENTATION, Quaternion(Dali::Degree(90.0f), Vector3::ZAXIS));
  application.GetScene().Add(actor);

  // Render and notify a couple of times
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Do an entire pinch, only check finished value
  TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 100);
  data.Reset();
  TestEndPinch(application, Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), Vector2(19.0f, 20.0f), Vector2(21.0f, 20.0f), 1000);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(0.055f, data.receivedGesture.GetScale(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(160.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.GetScreenCenterPoint(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(70.0f, 30.0f), data.receivedGesture.GetLocalCenterPoint(), 0.01f, TEST_LOCATION);

  // Rotate actor again and render and notify
  actor.SetProperty(Actor::Property::ORIENTATION, Quaternion(Dali::Degree(180.0f), Vector3::ZAXIS));
  application.SendNotification();
  application.Render();

  // Do an entire pinch, only check finished value
  TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 2100);
  data.Reset();
  TestEndPinch(application, Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), Vector2(19.0f, 20.0f), Vector2(21.0f, 20.0f), 3000);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(0.055f, data.receivedGesture.GetScale(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(160.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.GetScreenCenterPoint(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(30.0f, 30.0f), data.receivedGesture.GetLocalCenterPoint(), 0.01f, TEST_LOCATION);

  // Rotate actor again and render and notify
  actor.SetProperty(Actor::Property::ORIENTATION, Quaternion(Dali::Degree(270.0f), Vector3::ZAXIS));
  application.SendNotification();
  application.Render();

  // Do an entire pinch, only check finished value
  TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 4100);
  data.Reset();
  TestEndPinch(application, Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), Vector2(19.0f, 20.0f), Vector2(21.0f, 20.0f), 5000);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(0.055f, data.receivedGesture.GetScale(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(160.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.GetScreenCenterPoint(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(30.0f, 70.0f), data.receivedGesture.GetLocalCenterPoint(), 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionChildHit(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  parent.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  parent.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(parent);

  // Set child to completely cover parent.
  // Change rotation of child to be different from parent so that we can check if our local coordinate
  // conversion of the parent actor is correct.
  Actor child = Actor::New();
  child.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  child.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER);
  child.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  child.SetProperty(Actor::Property::ORIENTATION, Quaternion(Dali::Degree(90.0f), Vector3::ZAXIS));
  parent.Add(child);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(parent);
  detector.DetectedSignal().Connect(&application, functor);

  // Do an entire pan, only check finished value - hits child area but parent should still receive it
  TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 100);
  data.Reset();
  TestEndPinch(application, Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), Vector2(19.0f, 20.0f), Vector2(21.0f, 20.0f), 1000);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, parent == data.pinchedActor, TEST_LOCATION);
  DALI_TEST_EQUALS(0.055f, data.receivedGesture.GetScale(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(160.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.GetScreenCenterPoint(), 0.01f, TEST_LOCATION);

  // Attach child and generate same touch points to yield same results
  // (Also proves that you can detach and then re-attach another actor)
  detector.Attach(child);
  detector.Detach(parent);

  // Do an entire pan, only check finished value
  TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 2100);
  data.Reset();
  TestEndPinch(application, Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), Vector2(19.0f, 20.0f), Vector2(21.0f, 20.0f), 3000);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, child == data.pinchedActor, TEST_LOCATION);
  DALI_TEST_EQUALS(0.055f, data.receivedGesture.GetScale(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(160.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.GetScreenCenterPoint(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 80.0f), data.receivedGesture.GetLocalCenterPoint(), 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionAttachDetachMany(void)
{
  TestApplication application;

  Actor first = Actor::New();
  first.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  first.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(first);

  Actor second = Actor::New();
  second.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  second.SetProperty(Actor::Property::POSITION_X, 100.0f);
  second.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(second);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(first);
  detector.Attach(second);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pinch within second actor's area
  TestStartPinch(application, Vector2(102.0f, 20.0f), Vector2(138.0f, 20.0f), Vector2(110.0f, 20.0f), Vector2(130.0f, 20.0f), 100);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, second == data.pinchedActor, TEST_LOCATION);

  // Pinch moves into first actor's area - second actor should receive the pinch
  data.Reset();
  TestContinuePinch(application, Vector2(110.0f, 20.0f), Vector2(130.0f, 20.0f), Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), 500);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, second == data.pinchedActor, TEST_LOCATION);

  // Detach the second actor during the pinch, we should not receive the next event
  detector.Detach(second);

  // Gesture ends within actor's area
  data.Reset();
  TestEndPinch(application, Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), Vector2(119.0f, 20.0f), Vector2(121.0f, 20.0f), 3000);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionActorBecomesUntouchable(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pinch in actor's area
  TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 100);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Pan continues within actor's area
  data.Reset();
  TestContinuePinch(application, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), 500);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Actor become invisible - actor should not receive the next pinch
  actor.SetProperty(Actor::Property::VISIBLE, false);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Gesture ends within actor's area
  data.Reset();
  TestEndPinch(application, Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), Vector2(19.0f, 20.0f), Vector2(21.0f, 20.0f), 3000);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionMultipleDetectorsOnActor(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  Actor actor2 = Actor::New();
  actor2.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_RIGHT);
  application.GetScene().Add(actor2);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Attach actor to one detector
  SignalData             firstData;
  GestureReceivedFunctor firstFunctor(firstData);
  PinchGestureDetector   firstDetector = PinchGestureDetector::New();
  firstDetector.Attach(actor);
  firstDetector.DetectedSignal().Connect(&application, firstFunctor);

  // Attach actor to another detector
  SignalData             secondData;
  GestureReceivedFunctor secondFunctor(secondData);
  PinchGestureDetector   secondDetector = PinchGestureDetector::New();
  secondDetector.Attach(actor);
  secondDetector.DetectedSignal().Connect(&application, secondFunctor);

  // Add second actor to second detector, when we remove the actor, this will make sure that this
  // gesture detector is not removed from the GestureDetectorProcessor.  In this scenario, the
  // functor should still not be called (which is what we're also testing).
  secondDetector.Attach(actor2);

  // Pinch in actor's area - both detector's functors should be called
  TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 100);
  DALI_TEST_EQUALS(true, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // Pinch continues in actor's area - both detector's functors should be called
  firstData.Reset();
  secondData.Reset();
  TestContinuePinch(application, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), 500);
  DALI_TEST_EQUALS(true, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // Detach actor from firstDetector and emit pinch on actor, only secondDetector's functor should be called.
  firstDetector.Detach(actor);
  firstData.Reset();
  secondData.Reset();
  TestEndPinch(application, Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), Vector2(19.0f, 20.0f), Vector2(21.0f, 20.0f), 1000);
  DALI_TEST_EQUALS(false, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // New pinch on actor, only secondDetector has actor attached
  firstData.Reset();
  secondData.Reset();
  TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 1500);
  DALI_TEST_EQUALS(false, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // Detach actor from secondDetector
  secondDetector.Detach(actor);
  firstData.Reset();
  secondData.Reset();
  TestContinuePinch(application, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), 2000);
  DALI_TEST_EQUALS(false, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, secondData.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionEnsureCorrectSignalling(void)
{
  TestApplication application;

  Actor actor1 = Actor::New();
  actor1.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor1.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor1);
  SignalData             data1;
  GestureReceivedFunctor functor1(data1);
  PinchGestureDetector   detector1 = PinchGestureDetector::New();
  detector1.Attach(actor1);
  detector1.DetectedSignal().Connect(&application, functor1);

  Actor actor2 = Actor::New();
  actor2.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_RIGHT);
  actor2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_RIGHT);
  application.GetScene().Add(actor2);
  SignalData             data2;
  GestureReceivedFunctor functor2(data2);
  PinchGestureDetector   detector2 = PinchGestureDetector::New();
  detector2.Attach(actor2);
  detector2.DetectedSignal().Connect(&application, functor2);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Start pan in actor1's area, only data1 should be set
  TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 100);
  DALI_TEST_EQUALS(true, data1.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, data2.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureActorUnstaged(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // State to remove actor in.
  GestureState stateToUnstage(GestureState::STARTED);

  // Attach actor to detector
  SignalData           data;
  UnstageActorFunctor  functor(data, stateToUnstage, application.GetScene());
  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Emit signals
  TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 100);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  TestEndPinch(application, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), Vector2(19.0f, 20.0f), Vector2(21.0f, 20.0f), 1000);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Re-add actor to stage
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Change state to GestureState::CONTINUING to remove
  stateToUnstage = GestureState::CONTINUING;

  // Emit signals
  TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 100);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  TestContinuePinch(application, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), 500);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  TestEndPinch(application, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), Vector2(19.0f, 20.0f), Vector2(21.0f, 20.0f), 1000);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Re-add actor to stage
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Change state to GestureState::CONTINUING to remove
  stateToUnstage = GestureState::FINISHED;

  // Emit signals
  TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 100);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  TestContinuePinch(application, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), 500);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  TestEndPinch(application, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), Vector2(19.0f, 20.0f), Vector2(21.0f, 20.0f), 1000);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  tet_result(TET_PASS); // If we get here then we have handled actor stage removal gracefully.
  END_TEST;
}

int UtcDaliPinchGestureActorStagedAndDestroyed(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Create and add a second actor so that GestureDetector destruction does not come into play.
  Actor dummyActor(Actor::New());
  dummyActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  dummyActor.SetProperty(Actor::Property::POSITION, Vector2(100.0f, 100.0f));
  dummyActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(dummyActor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // State to remove actor in.
  GestureState stateToUnstage(GestureState::STARTED);

  // Attach actor to detector
  SignalData           data;
  UnstageActorFunctor  functor(data, stateToUnstage, application.GetScene());
  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.Attach(dummyActor);
  detector.DetectedSignal().Connect(&application, functor);

  // Here we are testing a STARTED actor which is removed in the STARTED callback, but then added back
  // before we get a continuing state.  As we were removed from the stage, even if we're at the same
  // position, we should still not be signalled.

  // Emit signals
  TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 100);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Re add to the stage, we should not be signalled
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Continue signal emission
  TestContinuePinch(application, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), 500);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();
  TestEndPinch(application, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), Vector2(19.0f, 20.0f), Vector2(21.0f, 20.0f), 1000);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Here we delete an actor in started, we should not receive any subsequent signalling.

  // Emit signals
  TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 1500);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Delete actor as well
  actor.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Continue signal emission
  TestContinuePinch(application, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), 2000);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();
  TestEndPinch(application, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), Vector2(19.0f, 20.0f), Vector2(21.0f, 20.0f), 3000);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureLayerConsumesTouch(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Add a detector
  SignalData             data;
  GestureReceivedFunctor functor(data);
  PinchGestureDetector   detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Add a layer to overlap the actor
  Layer layer = Layer::New();
  layer.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  layer.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(layer);
  layer.RaiseToTop();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit signals, should receive
  TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 100);
  TestEndPinch(application, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), Vector2(19.0f, 20.0f), Vector2(21.0f, 20.0f), 1000);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Set layer to consume all touch
  layer.SetProperty(Layer::Property::CONSUMES_TOUCH, true);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit the same signals again, should not receive
  TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 1500);
  TestEndPinch(application, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), Vector2(19.0f, 20.0f), Vector2(21.0f, 20.0f), 2000);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  END_TEST;
}

int UtcDaliPinchGestureDisableDetectionDuringPinchN(void)
{
  // Crash sometimes occurred when gesture-recognizer was deleted internally during a signal when the attached actor was detached

  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Add a detector
  PinchGestureDetector detector      = PinchGestureDetector::New();
  bool                 functorCalled = false;
  detector.Attach(actor);
  detector.DetectedSignal().Connect(
    &application,
    [&detector, &functorCalled](Actor actor, const PinchGesture& gesture) {
      if(gesture.GetState() == GestureState::FINISHED)
      {
        detector.Detach(actor);
        functorCalled = true;
      }
    });

  // Render and notify
  application.SendNotification();
  application.Render();

  // Try the gesture, should not crash
  try
  {
    TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 100);
    TestContinuePinch(application, Vector2(112.0f, 100.0f), Vector2(112.0f, 124.0f), Vector2(5.0f, 5.0f), Vector2(35.0f, 35.0f), 200);
    TestEndPinch(application, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), Vector2(19.0f, 20.0f), Vector2(21.0f, 20.0f), 1000);

    DALI_TEST_CHECK(true); // No crash, test has passed
    DALI_TEST_EQUALS(functorCalled, true, TEST_LOCATION);
  }
  catch(...)
  {
    DALI_TEST_CHECK(false); // If we crash, the test has failed
  }

  END_TEST;
}

int UtcDaliPinchGestureWhenGesturePropargation(void)
{
  TestApplication application;

  Actor parentActor = Actor::New();
  parentActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  parentActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  Actor childActor = Actor::New();
  childActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  childActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  parentActor.Add(childActor);
  application.GetScene().Add(parentActor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             pData;
  GestureReceivedFunctor pFunctor(pData);

  PinchGestureDetector parentDetector = PinchGestureDetector::New();
  parentDetector.Attach(parentActor);
  parentDetector.DetectedSignal().Connect(&application, pFunctor);

  SignalData             cData;
  GestureReceivedFunctor cFunctor(cData);

  PinchGestureDetector childDetector = PinchGestureDetector::New();
  childDetector.Attach(childActor);
  childDetector.DetectedSignal().Connect(&application, cFunctor);

  // Start gesture within the actor's area, we receive the gesture not parent actor but child actor.
  TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 100);
  DALI_TEST_EQUALS(true, cData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, pData.functorCalled, TEST_LOCATION);
  cData.Reset();
  pData.Reset();

  TestContinuePinch(application, Vector2(112.0f, 100.0f), Vector2(112.0f, 124.0f), Vector2(5.0f, 5.0f), Vector2(35.0f, 35.0f), 200);
  DALI_TEST_EQUALS(true, cData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, pData.functorCalled, TEST_LOCATION);
  cData.Reset();
  pData.Reset();

  TestEndPinch(application, Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), Vector2(19.0f, 20.0f), Vector2(21.0f, 20.0f), 300);
  DALI_TEST_EQUALS(true, cData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, pData.functorCalled, TEST_LOCATION);
  cData.Reset();
  pData.Reset();

  // If GesturePropargation is set, a gesture event is to pass over to the parent.
  cData.needGesturePropagation = true;

  // So now the parent got the gesture event.
  TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 700);
  DALI_TEST_EQUALS(true, cData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, pData.functorCalled, TEST_LOCATION);
  cData.Reset();
  pData.Reset();

  // child does not receive gestures. This is because we have passed the permission of the gesture to the parent.
  TestContinuePinch(application, Vector2(112.0f, 100.0f), Vector2(112.0f, 124.0f), Vector2(5.0f, 5.0f), Vector2(35.0f, 35.0f), 800);
  DALI_TEST_EQUALS(false, cData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, pData.functorCalled, TEST_LOCATION);
  cData.Reset();
  pData.Reset();

  TestEndPinch(application, Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), Vector2(19.0f, 20.0f), Vector2(21.0f, 20.0f), 900);
  DALI_TEST_EQUALS(false, cData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, pData.functorCalled, TEST_LOCATION);
  cData.Reset();
  pData.Reset();

  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionWithGeometryHittest(void)
{
  TestApplication application;
  application.GetScene().SetGeometryHittestEnabled(true);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pinch within the actor's area
  TestStartPinch(application, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), 100);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::STARTED, data.receivedGesture.GetState(), TEST_LOCATION);

  // Continue the pinch within the actor's area - we should still receive the signal
  data.Reset();
  TestContinuePinch(application, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), 500);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::CONTINUING, data.receivedGesture.GetState(), TEST_LOCATION);

  // Gesture ends within actor's area
  data.Reset();
  TestEndPinch(application, Vector2(15.0f, 20.0f), Vector2(25.0f, 20.0f), Vector2(19.0f, 20.0f), Vector2(21.0f, 20.0f), 1000);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::FINISHED, data.receivedGesture.GetState(), TEST_LOCATION);

  // Detach actor
  detector.DetachAll();

  // Ensure we are no longer signalled
  data.Reset();
  TestGeneratePinch(application);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureHandleEvent(void)
{
  TestApplication application;
  Integration::Scene scene     = application.GetScene();
  RenderTaskList   taskList  = scene.GetRenderTaskList();
  Dali::RenderTask task      = taskList.GetTask(0);

  Actor parentActor = Actor::New();
  parentActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  parentActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  Actor childActor = Actor::New();
  childActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  childActor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

  parentActor.Add(childActor);
  application.GetScene().Add(parentActor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             pData;
  GestureReceivedFunctor pFunctor(pData);

  PinchGestureDetector parentDetector = PinchGestureDetector::New();
  parentDetector.DetectedSignal().Connect(&application, pFunctor);

  Integration::TouchEvent tp = GenerateDoubleTouch(PointState::DOWN, Vector2(2.0f, 20.0f), PointState::DOWN, Vector2(38.0f, 20.0f), 100);
  Internal::TouchEventPtr touchEventImpl(new Internal::TouchEvent(100));
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->AddPoint(tp.GetPoint(1));
  touchEventImpl->SetRenderTask(task);
  Dali::TouchEvent touchEventHandle(touchEventImpl.Get());
  parentDetector.HandleEvent(parentActor, touchEventHandle);

  tp = GenerateDoubleTouch(PointState::MOTION, Vector2(10.0f, 20.0f), PointState::MOTION, Vector2(30.0f, 20.0f), 150);
  touchEventImpl = new Internal::TouchEvent(150);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->AddPoint(tp.GetPoint(1));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  parentDetector.HandleEvent(parentActor, touchEventHandle);

  tp = GenerateDoubleTouch(PointState::MOTION, Vector2(10.0f, 20.0f), PointState::MOTION, Vector2(30.0f, 20.0f), 200);
  touchEventImpl = new Internal::TouchEvent(200);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->AddPoint(tp.GetPoint(1));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  parentDetector.HandleEvent(parentActor, touchEventHandle);

  tp = GenerateDoubleTouch(PointState::MOTION, Vector2(10.0f, 20.0f), PointState::MOTION, Vector2(30.0f, 20.0f), 250);
  touchEventImpl = new Internal::TouchEvent(250);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->AddPoint(tp.GetPoint(1));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  parentDetector.HandleEvent(parentActor, touchEventHandle);

  tp = GenerateDoubleTouch(PointState::MOTION, Vector2(10.0f, 20.0f), PointState::MOTION, Vector2(30.0f, 20.0f), 300);
  touchEventImpl = new Internal::TouchEvent(300);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->AddPoint(tp.GetPoint(1));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  parentDetector.HandleEvent(parentActor, touchEventHandle);

  tp = GenerateDoubleTouch(PointState::UP, Vector2(10.0f, 20.0f), PointState::UP, Vector2(30.0f, 20.0f), 350);
  touchEventImpl = new Internal::TouchEvent(350);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->AddPoint(tp.GetPoint(1));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  parentDetector.HandleEvent(parentActor, touchEventHandle);


  DALI_TEST_EQUALS(true, pData.functorCalled, TEST_LOCATION);
  pData.Reset();

  END_TEST;
}
