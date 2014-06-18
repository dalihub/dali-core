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
#include <dali/dali.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/tap-gesture-event.h>
#include <dali/integration-api/system-overlay.h>
#include <dali-test-suite-utils.h>
#include <test-touch-utils.h>

using namespace Dali;

void utc_dali_tap_gesture_detector_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_tap_gesture_detector_cleanup(void)
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
    voidFunctorCalled(false)
  {}

  void Reset()
  {
    functorCalled = false;
    voidFunctorCalled = false;

    receivedGesture.numberOfTaps = 0u;
    receivedGesture.numberOfTouches = 0u;
    receivedGesture.screenPoint = Vector2(0.0f, 0.0f);
    receivedGesture.localPoint = Vector2(0.0f, 0.0f);

    tappedActor = NULL;
  }

  bool functorCalled;
  bool voidFunctorCalled;
  TapGesture receivedGesture;
  Actor tappedActor;
};

// Functor that sets the data when called
struct GestureReceivedFunctor
{
  GestureReceivedFunctor(SignalData& data) : signalData(data) { }

  void operator()(Actor actor, TapGesture tap)
  {
    signalData.functorCalled = true;
    signalData.receivedGesture = tap;
    signalData.tappedActor = actor;
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
  UnstageActorFunctor( SignalData& data ) : GestureReceivedFunctor( data ) { }

  void operator()(Actor actor, TapGesture tap)
  {
    GestureReceivedFunctor::operator()( actor, tap );
    Stage::GetCurrent().Remove( actor );
  }
};

// Functor for receiving a touch event
struct TouchEventFunctor
{
  bool operator()(Actor actor, const TouchEvent& touch)
  {
    //For line coverage
    unsigned int points = touch.GetPointCount();
    if( points > 0)
    {
      const TouchPoint& touchPoint = touch.GetPoint(0);
      tet_printf("Touch Point state = %d\n", touchPoint.state);
    }
    return false;
  }
};

// Generate a TapGestureEvent to send to Core
Integration::TapGestureEvent GenerateTap(
    Gesture::State state,
    unsigned int numberOfTaps,
    unsigned int numberOfTouches,
    Vector2 point)
{
  Integration::TapGestureEvent tap( state );

  tap.numberOfTaps = numberOfTaps;
  tap.numberOfTouches = numberOfTouches;
  tap.point = point;

  return tap;
}

} // anon namespace

///////////////////////////////////////////////////////////////////////////////


// Positive test case for a method
int UtcDaliTapGestureDetectorConstructor(void)
{
  TestApplication application;

  TapGestureDetector detector;
  DALI_TEST_CHECK(!detector);
  END_TEST;
}


int UtcDaliTapGestureDetectorNew(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();
  DALI_TEST_CHECK(detector);
  DALI_TEST_EQUALS(1u, detector.GetTapsRequired(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, detector.GetTouchesRequired(), TEST_LOCATION);

  TapGestureDetector detector2 = TapGestureDetector::New(5u, 5u);
  DALI_TEST_CHECK(detector2);
  DALI_TEST_EQUALS(5u, detector2.GetTapsRequired(), TEST_LOCATION);
  DALI_TEST_EQUALS(5u, detector2.GetTouchesRequired(), TEST_LOCATION);

  //Scoped test to test destructor
  {
    TapGestureDetector detector3 = TapGestureDetector::New();
    DALI_TEST_CHECK(detector3);
  }

  // Attach an actor and emit a touch event on the actor to ensure complete line coverage
  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  detector.Attach(actor);

  TouchEventFunctor touchFunctor;
  actor.TouchedSignal().Connect( &application, touchFunctor );

  Integration::TouchEvent touchEvent(1);
  TouchPoint point(1, TouchPoint::Down, 20.0f, 20.0f);
  touchEvent.AddPoint(point);
  application.ProcessEvent(touchEvent);

  // Render and notify
  application.SendNotification();
  application.Render();

  // For line coverage, Initialise default constructor
  TouchEvent touchEvent2;
  END_TEST;
}

int UtcDaliTapGestureDetectorDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::TapGestureDetector::DownCast()");

  TapGestureDetector detector = TapGestureDetector::New();

  BaseHandle object(detector);

  TapGestureDetector detector2 = TapGestureDetector::DownCast(object);
  DALI_TEST_CHECK(detector2);

  TapGestureDetector detector3 = DownCast< TapGestureDetector >(object);
  DALI_TEST_CHECK(detector3);

  BaseHandle unInitializedObject;
  TapGestureDetector detector4 = TapGestureDetector::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!detector4);

  TapGestureDetector detector5 = DownCast< TapGestureDetector >(unInitializedObject);
  DALI_TEST_CHECK(!detector5);

  GestureDetector detector6 = TapGestureDetector::New();
  TapGestureDetector detector7 = TapGestureDetector::DownCast(detector6);
  DALI_TEST_CHECK(detector7);
  END_TEST;
}

int UtcDaliTapGestureSetTapsRequired(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();

  unsigned int taps = 3;

  DALI_TEST_CHECK(taps != detector.GetTapsRequired());

  detector.SetTapsRequired(taps);

  DALI_TEST_EQUALS(taps, detector.GetTapsRequired(), TEST_LOCATION);

  // Attach an actor and change the required touches

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  TestGestureManager& gestureManager = application.GetGestureManager();
  gestureManager.Initialize();

  detector.SetTapsRequired(4);

  // Gesture detection should have been updated only
  DALI_TEST_EQUALS(true, gestureManager.WasCalled(TestGestureManager::UpdateType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::RegisterType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);

  // Reset values
  gestureManager.Initialize();

  // Create a second gesture detector that requires even less maximum touches
  TapGestureDetector secondDetector = TapGestureDetector::New();
  secondDetector.Attach(actor);

  // Gesture detection should have been updated
  DALI_TEST_EQUALS(true, gestureManager.WasCalled(TestGestureManager::UpdateType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::RegisterType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureGetTapsRequired(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();
  DALI_TEST_EQUALS(1u, detector.GetTapsRequired(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSetTouchesRequired(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();

  unsigned int max = 3;

  DALI_TEST_CHECK(max != detector.GetTouchesRequired());

  detector.SetTouchesRequired(max);

  DALI_TEST_EQUALS(max, detector.GetTouchesRequired(), TEST_LOCATION);

  // Attach an actor and change the maximum touches

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  TestGestureManager& gestureManager = application.GetGestureManager();
  gestureManager.Initialize();

  detector.SetTouchesRequired(4);

  // Gesture detection should have been updated only
  DALI_TEST_EQUALS(true, gestureManager.WasCalled(TestGestureManager::UpdateType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::RegisterType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);

  // Reset values
  gestureManager.Initialize();

  // Create a second gesture detector that requires even less maximum touches
  TapGestureDetector secondDetector = TapGestureDetector::New();
  secondDetector.Attach(actor);

  // Gesture detection should have been updated
  DALI_TEST_EQUALS(true, gestureManager.WasCalled(TestGestureManager::UpdateType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::RegisterType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureGetTouchesRequired(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();
  DALI_TEST_EQUALS(1u, detector.GetTouchesRequired(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSignalReceptionNegative(void)
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

  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Do a tap outside actor's area
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(112.0f, 112.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(112.0f, 112.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSignalReceptionPositive(void)
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

  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Do a tap inside actor's area
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(50.0f, 50.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(50.0f, 50.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTaps, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS( Vector2(50.0f, 50.0f), data.receivedGesture.localPoint, 0.1, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSignalReceptionDetach(void)
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

  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start tap within the actor's area
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(20.0f, 20.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTaps, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS( Vector2(20.0f, 20.0f), data.receivedGesture.localPoint, 0.1, TEST_LOCATION);

  // repeat the tap within the actor's area - we should still receive the signal
  data.Reset();
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(50.0f, 50.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(50.0f, 50.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTaps, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS( Vector2(50.0f, 50.0f), data.receivedGesture.localPoint, 0.1, TEST_LOCATION);

  // Detach actor
  detector.DetachAll();

  // Ensure we are no longer signalled
  data.Reset();
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(20.0f, 20.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(50.0f, 50.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(50.0f, 50.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSignalReceptionActorDestroyedWhileTapping(void)
{
  TestApplication application;

  SignalData data;
  GestureReceivedFunctor functor(data);

  TapGestureDetector detector = TapGestureDetector::New();
  detector.DetectedSignal().Connect(&application, functor);

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

    // Start tap within the actor's area
    application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(20.0f, 20.0f)));
    application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(20.0f, 20.0f)));
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

    // Remove the actor from stage and reset the data
    Stage::GetCurrent().Remove(actor);

    // Render and notify
    application.SendNotification();
    application.Render();
  }

  // Actor should now have been destroyed

  data.Reset();
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(20.0f, 20.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSignalReceptionRotatedActor(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetRotation(Dali::Degree(90.0f), Vector3::ZAXIS);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Do tap, only check finished value
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(5.0f, 5.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(5.0f, 5.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTaps, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS( Vector2(5.0f, 5.0f), data.receivedGesture.screenPoint, 0.1, TEST_LOCATION);

  // Rotate actor again and render
  actor.SetRotation(Dali::Degree(180.0f), Vector3::ZAXIS);
  application.SendNotification();
  application.Render();

  // Do tap, should still receive event
  data.Reset();
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(5.0f, 5.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(5.0f, 5.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTaps, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS( Vector2(5.0f, 5.0f), data.receivedGesture.screenPoint, 0.1, TEST_LOCATION);

  // Rotate actor again and render
  actor.SetRotation(Dali::Degree(90.0f), Vector3::YAXIS);
  application.SendNotification();
  application.Render();

  // Do tap, inside the actor's area (area if it is not rotated), Should not receive the event
  data.Reset();
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(70.0f, 70.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(70.0f, 70.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSignalReceptionChildHit(void)
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
  child.SetRotation(Dali::Degree(90.0f), Vector3::ZAXIS);
  parent.Add(child);

  TouchEventFunctor touchFunctor;
  child.TouchedSignal().Connect(&application, touchFunctor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(parent);
  detector.DetectedSignal().Connect(&application, functor);

  // Do tap - hits child area but parent should still receive it
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(50.0f, 50.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(50.0f, 50.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, parent == data.tappedActor, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(50.0f, 50.0f), data.receivedGesture.screenPoint, 0.01f, TEST_LOCATION);

  // Attach child and generate same touch points
  // (Also proves that you can detach and then re-attach another actor)
  detector.Attach(child);
  detector.Detach(parent);

  // Do an entire tap, only check finished value
  data.Reset();
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(51.0f, 51.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(51.0f, 51.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, child == data.tappedActor, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(51.0f, 51.0f), data.receivedGesture.screenPoint, 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSignalReceptionAttachDetachMany(void)
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

  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(first);
  detector.Attach(second);
  detector.DetectedSignal().Connect(&application, functor);

  // Tap within second actor's area
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(120.0f, 10.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(120.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, second == data.tappedActor, TEST_LOCATION);

  // Tap within first actor's area
  data.Reset();
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(20.0f, 10.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, first == data.tappedActor, TEST_LOCATION);

  // Detach the second actor
  detector.Detach(second);

  // second actor shouldn't receive event
  data.Reset();
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(120.0f, 10.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(120.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // first actor should continue receiving event
  data.Reset();
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(20.0f, 10.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSignalReceptionActorBecomesUntouchable(void)
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

  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Tap in actor's area
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(50.0f, 10.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Actor become invisible - actor should not receive the next pan
  actor.SetVisible(false);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap in the same area, shouldn't receive event
  data.Reset();
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(50.0f, 10.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSignalReceptionMultipleGestureDetectors(void)
{
  TestApplication application;
  Dali::TestGestureManager& gestureManager = application.GetGestureManager();

  Actor first = Actor::New();
  first.SetSize(100.0f, 100.0f);
  first.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(first);

  Actor second = Actor::New();
  second.SetSize(100.0f, 100.0f);
  second.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  second.SetX(100.0f);
  first.Add(second);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  TapGestureDetector firstDetector = TapGestureDetector::New();
  firstDetector.Attach(first);
  firstDetector.DetectedSignal().Connect(&application, functor);

  // secondDetector is scoped
  {
    // Reset gestureManager statistics
    gestureManager.Initialize();

    TapGestureDetector secondDetector = TapGestureDetector::New();
    secondDetector.SetTapsRequired(2);
    secondDetector.SetTouchesRequired(2);
    secondDetector.Attach(second);
    secondDetector.DetectedSignal().Connect(&application, functor);

    DALI_TEST_EQUALS(true, gestureManager.WasCalled(TestGestureManager::UpdateType), TEST_LOCATION);
    DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::RegisterType), TEST_LOCATION);
    DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);

    // Tap within second actor's area
    application.ProcessEvent(GenerateTap(Gesture::Possible, 2u, 2u, Vector2(150.0f, 10.0f)));
    application.ProcessEvent(GenerateTap(Gesture::Started, 2u, 2u, Vector2(150.0f, 10.0f)));
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_EQUALS(true, second == data.tappedActor, TEST_LOCATION);

    // Tap continues as single touch gesture - we should not receive any gesture
    data.Reset();
    application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(150.0f, 10.0f)));
    application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(150.0f, 10.0f)));
    DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

    // Single touch tap starts - first actor should be panned
    data.Reset();
    application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(50.0f, 10.0f)));
    application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(50.0f, 10.0f)));
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_EQUALS(true, first == data.tappedActor, TEST_LOCATION);

    // Pan changes to double-touch - we shouldn't receive event
    data.Reset();
    application.ProcessEvent(GenerateTap(Gesture::Possible, 2u, 2u, Vector2(50.0f, 10.0f)));
    application.ProcessEvent(GenerateTap(Gesture::Started, 2u, 2u, Vector2(50.0f, 10.0f)));
    DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

    // Reset gesture manager statistics
    gestureManager.Initialize();
  }

  // secondDetector has now been deleted.  Gesture detection should have been updated only
  DALI_TEST_EQUALS(true, gestureManager.WasCalled(TestGestureManager::UpdateType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::RegisterType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSignalReceptionMultipleDetectorsOnActor(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Attach actor to one detector
  SignalData firstData;
  GestureReceivedFunctor firstFunctor(firstData);
  TapGestureDetector firstDetector = TapGestureDetector::New();
  firstDetector.Attach(actor);
  firstDetector.DetectedSignal().Connect(&application, firstFunctor);

  // Attach actor to another detector
  SignalData secondData;
  GestureReceivedFunctor secondFunctor(secondData);
  TapGestureDetector secondDetector = TapGestureDetector::New();
  secondDetector.Attach(actor);
  secondDetector.DetectedSignal().Connect(&application, secondFunctor);

  // Tap in actor's area - both detector's functors should be called
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(50.0f, 10.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(true, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSignalReceptionDifferentPossible(void)
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
  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Gesture possible in actor's area.
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Move actor somewhere else
  actor.SetPosition( 100.0f, 100.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit Started event, we should not receive the long press.
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // LongPress possible in empty area.
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Move actor in to the long press position.
  actor.SetPosition( 0.0f, 0.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit Started event, we should not receive the long press.
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Normal long press in actor's area for completeness.
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(50.0f, 10.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureEmitIncorrectStateClear(void)
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
  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Try a Clear state
  try
  {
    application.ProcessEvent(GenerateTap(Gesture::Clear, 1u, 1u, Vector2(50.0f, 10.0f)));
    tet_result(TET_FAIL);
  }
  catch ( Dali::DaliException& e )
  {
    DALI_TEST_ASSERT( e, "false", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliTapGestureEmitIncorrectStateContinuing(void)
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
  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Try a Continuing state
  try
  {
    application.ProcessEvent(GenerateTap(Gesture::Continuing, 1u, 1u, Vector2(50.0f, 10.0f)));
    tet_result(TET_FAIL);
  }
  catch ( Dali::DaliException& e )
  {
    DALI_TEST_ASSERT( e, "false", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliTapGestureEmitIncorrectStateFinished(void)
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
  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Try a Finished state
  try
  {
    application.ProcessEvent(GenerateTap(Gesture::Finished, 1u, 1u, Vector2(50.0f, 10.0f)));
    tet_result(TET_FAIL);
  }
  catch ( Dali::DaliException& e )
  {
    DALI_TEST_ASSERT( e, "false", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliTapGestureActorUnstaged(void)
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
  UnstageActorFunctor functor( data );
  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Tap in Actor's area, actor removed in signal handler, should be handled gracefully.
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(50.0f, 10.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  tet_result( TET_PASS ); // If we get here, then the actor removal on signal handler was handled gracefully.
  END_TEST;
}

int UtcDaliTapGestureRepeatedState(void)
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
  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Emit two possibles
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(50.0f, 10.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Send a couple of Started states, only first one should be received.
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Send a couple of cancelled states, no reception
  application.ProcessEvent(GenerateTap(Gesture::Cancelled, 1u, 1u, Vector2(50.0f, 10.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Cancelled, 1u, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGesturePossibleCancelled(void)
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
  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Emit a possible and then a cancelled, no reception
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(50.0f, 10.0f)));
  application.ProcessEvent(GenerateTap(Gesture::Cancelled, 1u, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureDetectorRemovedWhilePossible(void)
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
  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Emit a possible
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(50.0f, 10.0f)));

  // Detach actor and send a Started state, no signal.
  detector.DetachAll();
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureActorRemovedWhilePossible(void)
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
  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Emit a possible
  application.ProcessEvent(GenerateTap(Gesture::Possible, 1u, 1u, Vector2(50.0f, 10.0f)));

  // Remove, render and delete actor
  Stage::GetCurrent().Remove(actor);
  application.SendNotification();
  application.Render();
  actor = NULL;

  // Send a Started state, no signal.
  application.ProcessEvent(GenerateTap(Gesture::Started, 1u, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSystemOverlay(void)
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

  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  Vector2 screenCoords( 50.0f, 50.0f );

  // Do a tap inside actor's area
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, screenCoords ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, screenCoords ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  END_TEST;
}

int UtcDaliTapGestureBehindTouchableSystemOverlay(void)
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

  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(stageActor);
  detector.DetectedSignal().Connect(&application, functor);

  Vector2 screenCoords( 50.0f, 50.0f );

  // Do a tap inside both actors' area
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, screenCoords ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, screenCoords ) );
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

int UtcDaliTapGestureTouchBehindGesturedSystemOverlay(void)
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

  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(systemOverlayActor);
  detector.DetectedSignal().Connect(&application, functor);

  Vector2 screenCoords( 50.0f, 50.0f );

  // Do a tap inside both actors' area
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, screenCoords ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, screenCoords ) );
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

int UtcDaliTapGestureLayerConsumesTouch(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Add a detector
  SignalData data;
  GestureReceivedFunctor functor(data);
  TapGestureDetector detector = TapGestureDetector::New();
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

  // Emit signals, should receive
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, screenCoords ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, screenCoords ) );
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Set layer to consume all touch
  layer.SetTouchConsumed( true );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit the same signals again, should not receive
  application.ProcessEvent( GenerateTap( Gesture::Possible, 1u, 1u, screenCoords ) );
  application.ProcessEvent( GenerateTap( Gesture::Started, 1u, 1u, screenCoords ) );
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  END_TEST;
}
