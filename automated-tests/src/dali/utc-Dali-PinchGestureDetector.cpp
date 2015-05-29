/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <iostream>

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/pinch-gesture-event.h>
#include <dali/integration-api/system-overlay.h>
#include <dali-test-suite-utils.h>
#include <test-touch-utils.h>

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
    receivedGesture(Gesture::Started)
  {}

  void Reset()
  {
    functorCalled = false;
    voidFunctorCalled = false;

    receivedGesture.state = Gesture::Started;
    receivedGesture.scale = 0.0f;
    receivedGesture.speed = 0.0f;
    receivedGesture.screenCenterPoint = Vector2(0.0f, 0.0f);
    receivedGesture.localCenterPoint = Vector2(0.0f, 0.0f);

    pinchedActor.Reset();
  }

  bool functorCalled;
  bool voidFunctorCalled;
  PinchGesture receivedGesture;
  Actor pinchedActor;
};

// Functor that sets the data when called
struct GestureReceivedFunctor
{
  GestureReceivedFunctor(SignalData& data) : signalData(data) { }

  void operator()(Actor actor, const PinchGesture& pinch)
  {
    signalData.functorCalled = true;
    signalData.receivedGesture = pinch;
    signalData.pinchedActor = actor;
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
  UnstageActorFunctor( SignalData& data, Gesture::State& stateToUnstage )
  : GestureReceivedFunctor( data ),
    stateToUnstage( stateToUnstage )
  {
  }

  void operator()( Actor actor, const PinchGesture& pinch )
  {
    GestureReceivedFunctor::operator()( actor, pinch );

    if ( pinch.state == stateToUnstage )
    {
      Stage::GetCurrent().Remove( actor );
    }
  }

  Gesture::State& stateToUnstage;
};

// Functor for receiving a touch event
struct TouchEventFunctor
{
  bool operator()(Actor actor, const TouchEvent& touch)
  {
    return false;
  }
};

// Generate a PinchGestureEvent to send to Core
Integration::PinchGestureEvent GeneratePinch(
    Gesture::State state,
    float scale,
    float speed,
    Vector2 centerpoint)
{
  Integration::PinchGestureEvent pinch(state);

  pinch.scale = scale;
  pinch.speed = speed;
  pinch.centerPoint = centerpoint;

  return pinch;
}

} // anon namespace

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

  PinchGestureDetector detector = PinchGestureDetector::New();;

  PinchGestureDetector copy( detector );
  DALI_TEST_CHECK( detector );
  END_TEST;
}

int UtcDaliPinchGestureDetectorAssignmentOperatorP(void)
{
  TestApplication application;

  PinchGestureDetector detector = PinchGestureDetector::New();;

  PinchGestureDetector assign;
  assign = detector;
  DALI_TEST_CHECK( detector );

  DALI_TEST_CHECK( detector == assign );
  END_TEST;
}

int UtcDaliPinchGestureDetectorNew(void)
{
  TestApplication application;

  PinchGestureDetector detector = PinchGestureDetector::New();

  DALI_TEST_CHECK(detector);

  // Attach an actor and emit a touch event on the actor to ensure complete line coverage
  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  detector.Attach(actor);

  Integration::TouchEvent touchEvent(1);
  TouchPoint point(1, TouchPoint::Down, 20.0f, 20.0f);
  touchEvent.AddPoint(point);
  application.ProcessEvent(touchEvent);

  TouchPoint point2(1, TouchPoint::Down, 20.0f, 20.0f, 20.0f, 20.0f);
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

  PinchGestureDetector detector3 = DownCast< PinchGestureDetector >(object);
  DALI_TEST_CHECK(detector3);

  BaseHandle unInitializedObject;
  PinchGestureDetector detector4 = PinchGestureDetector::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!detector4);

  PinchGestureDetector detector5 = DownCast< PinchGestureDetector >(unInitializedObject);
  DALI_TEST_CHECK(!detector5);

  GestureDetector detector6 = PinchGestureDetector::New();
  PinchGestureDetector detector7 = PinchGestureDetector::DownCast(detector6);
  DALI_TEST_CHECK(detector7);
  END_TEST;
}

// Negative test case for a method
int UtcDaliPinchGestureSignalReceptionNegative(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Do a pinch outside actor's area
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 45.0f, Vector2(112.0f, 112.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Continue pinch into actor's area - we should still not receive the signal
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Continuing, 4.5f, 95.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Stop pinching - we should still not receive the signal
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Finished, 10.0f, 50.0f, Vector2(12.0f, 12.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionDownMotionLeave(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pan within the actor's area
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Started, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(10.0f, data.receivedGesture.scale, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(50.0f, data.receivedGesture.speed, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.screenCenterPoint, 0.01f, TEST_LOCATION);

  // Continue the pan within the actor's area - we should still receive the signal
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Continuing, 5.0f, 90.0f, Vector2(21.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Continuing, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(5.0f, data.receivedGesture.scale, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(90.0f, data.receivedGesture.speed, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(21.0f, 20.0f), data.receivedGesture.screenCenterPoint, 0.01f, TEST_LOCATION);

  // Pan Gesture leaves actor's area - we should still receive the signal
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Continuing, 10.0f, 15.5f, Vector2(320.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Continuing, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(10.0f, data.receivedGesture.scale, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(15.5f, data.receivedGesture.speed, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(320.0f, 10.0f), data.receivedGesture.screenCenterPoint, 0.01f, TEST_LOCATION);

  // Gesture ends - we would receive a finished state
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Finished, 15.2f, 12.1f, Vector2(310.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Finished, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(15.2f, data.receivedGesture.scale, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(12.1f, data.receivedGesture.speed, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(310.0f, 10.0f), data.receivedGesture.screenCenterPoint, 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionDownMotionUp(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pinch within the actor's area
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Started, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(10.0f, data.receivedGesture.scale, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(50.0f, data.receivedGesture.speed, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.screenCenterPoint, 0.01f, TEST_LOCATION);

  // Continue the pinch within the actor's area - we should still receive the signal
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Continuing, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Continuing, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(5.0f, data.receivedGesture.scale, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(25.0f, data.receivedGesture.speed, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.screenCenterPoint, 0.01f, TEST_LOCATION);

  // Gesture ends within actor's area - we would receive a finished state
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Finished, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Finished, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(5.0f, data.receivedGesture.scale, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(25.0f, data.receivedGesture.speed, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.screenCenterPoint, 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionCancelled(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pinch within the actor's area
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Started, data.receivedGesture.state, TEST_LOCATION);


  // Continue the pinch within the actor's area - we should still receive the signal
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Continuing, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Continuing, data.receivedGesture.state, TEST_LOCATION);

  // The gesture is cancelled
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Cancelled, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Cancelled, data.receivedGesture.state, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionDetach(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pinch within the actor's area
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Started, data.receivedGesture.state, TEST_LOCATION);


  // Continue the pinch within the actor's area - we should still receive the signal
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Continuing, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Continuing, data.receivedGesture.state, TEST_LOCATION);

  // Gesture ends within actor's area
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Finished, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Finished, data.receivedGesture.state, TEST_LOCATION);

  // Detach actor
  detector.DetachAll();

  // Ensure we are no longer signalled
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 20.0f)));
  application.ProcessEvent(GeneratePinch(Gesture::Continuing, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
  application.ProcessEvent(GeneratePinch(Gesture::Finished, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionDetachWhilePinching(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pinch within the actor's area
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Started, data.receivedGesture.state, TEST_LOCATION);

  // Continue the pinch within the actor's area - we should still receive the signal
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Continuing, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Continuing, data.receivedGesture.state, TEST_LOCATION);

  // Detach actor during the pinch, we should not receive the next event
  detector.DetachAll();

  // Gesture ends within actor's area
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Finished, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionActorDestroyedWhilePinching(void)
{
  TestApplication application;

  SignalData data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.DetectedSignal().Connect(&application, functor);

  // Attach a temporary actor to stop detector being removed from PinchGestureProcessor when main actor
  // is destroyed.
  Actor tempActor = Actor::New();
  tempActor.SetSize(100.0f, 100.0f);
  tempActor.SetAnchorPoint(AnchorPoint::BOTTOM_RIGHT);
  Stage::GetCurrent().Add(tempActor);
  detector.Attach(tempActor);

  // Actor lifetime is scoped
  {
    Actor actor = Actor::New();
    actor.SetSize(100.0f, 100.0f);
    actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    Stage::GetCurrent().Add(actor);

    // Render and notify
    application.SendNotification();
    application.Render();

    detector.Attach(actor);

    // Start pinch within the actor's area
    application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(10.0f, 10.0f)));
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_EQUALS(Gesture::Started, data.receivedGesture.state, TEST_LOCATION);

    // Continue the pinch within the actor's area - we should still receive the signal
    data.Reset();
    application.ProcessEvent(GeneratePinch(Gesture::Continuing, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_EQUALS(Gesture::Continuing, data.receivedGesture.state, TEST_LOCATION);

    // Remove the actor from stage and reset the data
    Stage::GetCurrent().Remove(actor);

    // Render and notify
    application.SendNotification();
    application.Render();
  }

  // Actor should now have been destroyed

  // Gesture ends within the area where the actor used to be
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Finished, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionRotatedActor(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetOrientation(Dali::Degree(90.0f), Vector3::ZAXIS);
  Stage::GetCurrent().Add(actor);

  // Render and notify a couple of times
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Do an entire pinch, only check finished value
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(10.0f, 10.0f)));
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Finished, 10.0f, 50.0f, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(10.0f, data.receivedGesture.scale, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(50.0f, data.receivedGesture.speed, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(10.0f, 10.0f), data.receivedGesture.screenCenterPoint, 0.01f, TEST_LOCATION);

  // Rotate actor again and render and notify
  actor.SetOrientation(Dali::Degree(180.0f), Vector3::ZAXIS);
  application.SendNotification();
  application.Render();

  // Do an entire pinch, only check finished value
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(10.0f, 10.0f)));
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Finished, 10.0f, 50.0f, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(10.0f, data.receivedGesture.scale, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(50.0f, data.receivedGesture.speed, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(10.0f, 10.0f), data.receivedGesture.screenCenterPoint, 0.01f, TEST_LOCATION);

  // Rotate actor again and render and notify
  actor.SetOrientation(Dali::Degree(270.0f), Vector3::ZAXIS);
  application.SendNotification();
  application.Render();

  // Do an entire pinch, only check finished value
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(10.0f, 10.0f)));
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Finished, 10.0f, 50.0f, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(10.0f, data.receivedGesture.scale, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(50.0f, data.receivedGesture.speed, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(10.0f, 10.0f), data.receivedGesture.screenCenterPoint, 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionChildHit(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  parent.SetSize(100.0f, 100.0f);
  parent.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(parent);

  // Set child to completely cover parent.
  // Change rotation of child to be different from parent so that we can check if our local coordinate
  // conversion of the parent actor is correct.
  Actor child = Actor::New();
  child.SetSize(100.0f, 100.0f);
  child.SetAnchorPoint(AnchorPoint::CENTER);
  child.SetParentOrigin(ParentOrigin::CENTER);
  child.SetOrientation(Dali::Degree(90.0f), Vector3::ZAXIS);
  parent.Add(child);

  TouchEventFunctor touchFunctor;
  child.TouchedSignal().Connect(&application, touchFunctor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(parent);
  detector.DetectedSignal().Connect(&application, functor);

  // Do an entire pan, only check finished value - hits child area but parent should still receive it
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(10.0f, 10.0f)));
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Finished, 5.0f, 50.0f, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, parent == data.pinchedActor, TEST_LOCATION);
  DALI_TEST_EQUALS(5.0f, data.receivedGesture.scale, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(50.0f, data.receivedGesture.speed, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(10.0f, 10.0f), data.receivedGesture.screenCenterPoint, 0.01f, TEST_LOCATION);

  // Attach child and generate same touch points to yield same results
  // (Also proves that you can detach and then re-attach another actor)
  detector.Attach(child);
  detector.Detach(parent);

  // Do an entire pan, only check finished value
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(10.0f, 10.0f)));
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Finished, 5.0f, 50.0f, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, child == data.pinchedActor, TEST_LOCATION);
  DALI_TEST_EQUALS(5.0f, data.receivedGesture.scale, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(50.0f, data.receivedGesture.speed, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(10.0f, 10.0f), data.receivedGesture.screenCenterPoint, 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionAttachDetachMany(void)
{
  TestApplication application;

  Actor first = Actor::New();
  first.SetSize(100.0f, 100.0f);
  first.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(first);

  Actor second = Actor::New();
  second.SetSize(100.0f, 100.0f);
  second.SetX(100.0f);
  second.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(second);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(first);
  detector.Attach(second);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pinch within second actor's area
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(120.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, second == data.pinchedActor, TEST_LOCATION);

  // Pinch moves into first actor's area - second actor should receive the pinch
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Continuing, 10.0f, 50.0f, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, second == data.pinchedActor, TEST_LOCATION);

  // Detach the second actor during the pinch, we should not receive the next event
  detector.Detach(second);

  // Gesture ends within actor's area
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Finished, 10.0f, 50.0f, Vector2(120.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionActorBecomesUntouchable(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pinch in actor's area
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Pan continues within actor's area
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Started, 5.0f, 50.0f, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Actor become invisible - actor should not receive the next pinch
  actor.SetVisible(false);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Gesture ends within actor's area
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Finished, 5.0f, 50.0f, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionMultipleDetectorsOnActor(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  Actor actor2 = Actor::New();
  actor2.SetSize(100.0f, 100.0f);
  actor2.SetAnchorPoint(AnchorPoint::BOTTOM_RIGHT);
  Stage::GetCurrent().Add(actor2);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Attach actor to one detector
  SignalData firstData;
  GestureReceivedFunctor firstFunctor(firstData);
  PinchGestureDetector firstDetector = PinchGestureDetector::New();
  firstDetector.Attach(actor);
  firstDetector.DetectedSignal().Connect(&application, firstFunctor);

  // Attach actor to another detector
  SignalData secondData;
  GestureReceivedFunctor secondFunctor(secondData);
  PinchGestureDetector secondDetector = PinchGestureDetector::New();
  secondDetector.Attach(actor);
  secondDetector.DetectedSignal().Connect(&application, secondFunctor);

  // Add second actor to second detector, when we remove the actor, this will make sure that this
  // gesture detector is not removed from the GestureDetectorProcessor.  In this scenario, the
  // functor should still not be called (which is what we're also testing).
  secondDetector.Attach(actor2);

  // Pinch in actor's area - both detector's functors should be called
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // Pinch continues in actor's area - both detector's functors should be called
  firstData.Reset();
  secondData.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Continuing, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // Detach actor from firstDetector and emit pinch on actor, only secondDetector's functor should be called.
  firstDetector.Detach(actor);
  firstData.Reset();
  secondData.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Finished, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(false, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // New pinch on actor, only secondDetector has actor attached
  firstData.Reset();
  secondData.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(false, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // Detach actor from secondDetector
  secondDetector.Detach(actor);
  firstData.Reset();
  secondData.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Continuing, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(false, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, secondData.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionMultipleStarted(void)
{
  // Should handle two started events gracefully.

  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  SignalData data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Start pan in actor's area
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Send another start in actor's area
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Add a child actor to overlap actor and send another start in actor's area
  Actor child = Actor::New();
  child.SetSize(100.0f, 100.0f);
  child.SetAnchorPoint(AnchorPoint::CENTER);
  child.SetParentOrigin(ParentOrigin::CENTER);
  actor.Add(child);

  TouchEventFunctor touchFunctor;
  child.TouchedSignal().Connect(&application, touchFunctor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Send another start in actor's area
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Send another start in actor's area
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSignalReceptionEnsureCorrectSignalling(void)
{
  TestApplication application;

  Actor actor1 = Actor::New();
  actor1.SetSize(100.0f, 100.0f);
  actor1.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor1);
  SignalData data1;
  GestureReceivedFunctor functor1(data1);
  PinchGestureDetector detector1 = PinchGestureDetector::New();
  detector1.Attach(actor1);
  detector1.DetectedSignal().Connect(&application, functor1);

  Actor actor2 = Actor::New();
  actor2.SetSize(100.0f, 100.0f);
  actor2.SetAnchorPoint(AnchorPoint::BOTTOM_RIGHT);
  actor2.SetParentOrigin(ParentOrigin::BOTTOM_RIGHT);
  Stage::GetCurrent().Add(actor2);
  SignalData data2;
  GestureReceivedFunctor functor2(data2);
  PinchGestureDetector detector2 = PinchGestureDetector::New();
  detector2.Attach(actor2);
  detector2.DetectedSignal().Connect(&application, functor2);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Start pan in actor1's area, only data1 should be set
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data1.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, data2.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureEmitIncorrectStateClear(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Attach actor to detector
  SignalData data;
  GestureReceivedFunctor functor( data );
  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Try a Clear state
  try
  {
    application.ProcessEvent(GeneratePinch(Gesture::Clear, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
    tet_result(TET_FAIL);
  }
  catch ( Dali::DaliException& e )
  {
    DALI_TEST_ASSERT( e, "false", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliPinchGestureEmitIncorrectStatePossible(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Attach actor to detector
  SignalData data;
  GestureReceivedFunctor functor( data );
  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Try a Possible state
  try
  {
    application.ProcessEvent(GeneratePinch(Gesture::Possible, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
    tet_result(TET_FAIL);
  }
  catch ( Dali::DaliException& e )
  {
    DALI_TEST_ASSERT( e, "false", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliPinchGestureActorUnstaged(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // State to remove actor in.
  Gesture::State stateToUnstage( Gesture::Started );

  // Attach actor to detector
  SignalData data;
  UnstageActorFunctor functor( data, stateToUnstage );
  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Emit signals
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Finished, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Re-add actor to stage
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Change state to Gesture::Continuing to remove
  stateToUnstage = Gesture::Continuing;

  // Emit signals
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Continuing, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Finished, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Re-add actor to stage
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Change state to Gesture::Continuing to remove
  stateToUnstage = Gesture::Finished;

  // Emit signals
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Continuing, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Finished, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  tet_result( TET_PASS ); // If we get here then we have handled actor stage removal gracefully.
  END_TEST;
}

int UtcDaliPinchGestureActorStagedAndDestroyed(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Create and add a second actor so that GestureDetector destruction does not come into play.
  Actor dummyActor( Actor::New() );
  dummyActor.SetSize( 100.0f, 100.0f );
  dummyActor.SetPosition( 100.0f, 100.0f );
  dummyActor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(dummyActor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // State to remove actor in.
  Gesture::State stateToUnstage( Gesture::Started );

  // Attach actor to detector
  SignalData data;
  UnstageActorFunctor functor( data, stateToUnstage );
  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.Attach(dummyActor);
  detector.DetectedSignal().Connect( &application, functor );

  // Here we are testing a Started actor which is removed in the Started callback, but then added back
  // before we get a continuing state.  As we were removed from the stage, even if we're at the same
  // position, we should still not be signalled.

  // Emit signals
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Re add to the stage, we should not be signalled
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Continue signal emission
  application.ProcessEvent(GeneratePinch(Gesture::Continuing, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Finished, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Here we delete an actor in started, we should not receive any subsequent signalling.

  // Emit signals
  application.ProcessEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
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
  application.ProcessEvent(GeneratePinch(Gesture::Continuing, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();
  application.ProcessEvent(GeneratePinch(Gesture::Finished, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPinchGestureSystemOverlay(void)
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();
  Dali::Integration::SystemOverlay& systemOverlay( core.GetSystemOverlay() );
  systemOverlay.GetOverlayRenderTasks().CreateTask();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  systemOverlay.Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  Vector2 screenCoords( 50.0f, 50.0f );
  float scale ( 10.0f );
  float speed ( 50.0f );

  // Start pan within the actor's area
  application.ProcessEvent( GeneratePinch( Gesture::Started, scale, speed, screenCoords ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  END_TEST;
}

int UtcDaliPinchGestureBehindTouchableSystemOverlay(void)
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();
  Dali::Integration::SystemOverlay& systemOverlay( core.GetSystemOverlay() );
  systemOverlay.GetOverlayRenderTasks().CreateTask();

  // SystemOverlay actor
  Actor systemOverlayActor = Actor::New();
  systemOverlayActor.SetSize(100.0f, 100.0f);
  systemOverlayActor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  systemOverlay.Add(systemOverlayActor);

  // Stage actor
  Actor stageActor = Actor::New();
  stageActor.SetSize(100.0f, 100.0f);
  stageActor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(stageActor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Set system-overlay actor to touchable
  TouchEventData touchData;
  TouchEventDataFunctor touchFunctor( touchData );
  systemOverlayActor.TouchedSignal().Connect(&application, touchFunctor);

  // Set stage actor to receive the gesture
  SignalData data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(stageActor);
  detector.DetectedSignal().Connect(&application, functor);

  Vector2 screenCoords( 50.0f, 50.0f );
  float scale ( 10.0f );
  float speed ( 50.0f );

  // Start pinch within the two actors' area
  application.ProcessEvent( GeneratePinch( Gesture::Started, scale, speed, screenCoords ) );
  application.ProcessEvent( GeneratePinch( Gesture::Finished, scale, speed, screenCoords ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, touchData.functorCalled, TEST_LOCATION );

  data.Reset();
  touchData.Reset();

  // Do touch in the same area
  application.ProcessEvent( touchFunctor.GenerateSingleTouch( TouchPoint::Down, screenCoords ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, touchData.functorCalled, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPinchGestureTouchBehindGesturedSystemOverlay(void)
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();
  Dali::Integration::SystemOverlay& systemOverlay( core.GetSystemOverlay() );
  systemOverlay.GetOverlayRenderTasks().CreateTask();

  // SystemOverlay actor
  Actor systemOverlayActor = Actor::New();
  systemOverlayActor.SetSize(100.0f, 100.0f);
  systemOverlayActor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  systemOverlay.Add(systemOverlayActor);

  // Stage actor
  Actor stageActor = Actor::New();
  stageActor.SetSize(100.0f, 100.0f);
  stageActor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(stageActor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Set stage actor to touchable
  TouchEventData touchData;
  TouchEventDataFunctor touchFunctor( touchData );
  stageActor.TouchedSignal().Connect(&application, touchFunctor);

  // Set system-overlay actor to have the gesture
  SignalData data;
  GestureReceivedFunctor functor(data);

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(systemOverlayActor);
  detector.DetectedSignal().Connect(&application, functor);

  Vector2 screenCoords( 50.0f, 50.0f );
  float scale ( 10.0f );
  float speed ( 50.0f );

  // Start pinch within the two actors' area
  application.ProcessEvent( GeneratePinch( Gesture::Started, scale, speed, screenCoords ) );
  application.ProcessEvent( GeneratePinch( Gesture::Finished, scale, speed, screenCoords ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, touchData.functorCalled, TEST_LOCATION );

  data.Reset();
  touchData.Reset();

  // Do touch in the same area
  application.ProcessEvent( touchFunctor.GenerateSingleTouch( TouchPoint::Down, screenCoords ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, touchData.functorCalled, TEST_LOCATION );

  END_TEST;
}

int UtcDaliPinchGestureLayerConsumesTouch(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Add a detector
  SignalData data;
  GestureReceivedFunctor functor(data);
  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Add a layer to overlap the actor
  Layer layer = Layer::New();
  layer.SetSize(100.0f, 100.0f);
  layer.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add( layer );
  layer.RaiseToTop();

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector2 screenCoords( 50.0f, 50.0f );
  float scale ( 10.0f );
  float speed ( 50.0f );

  // Emit signals, should receive
  application.ProcessEvent( GeneratePinch( Gesture::Started, scale, speed, screenCoords ) );
  application.ProcessEvent( GeneratePinch( Gesture::Finished, scale, speed, screenCoords ) );
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Set layer to consume all touch
  layer.SetTouchConsumed( true );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit the same signals again, should not receive
  application.ProcessEvent( GeneratePinch( Gesture::Started, scale, speed, screenCoords ) );
  application.ProcessEvent( GeneratePinch( Gesture::Finished, scale, speed, screenCoords ) );
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  END_TEST;
}
