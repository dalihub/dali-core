/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/render-task-list-integ.h>
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

    tappedActor.Reset();
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

  void operator()(Actor actor, const TapGesture& tap)
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

  void operator()(Actor actor, const TapGesture& tap)
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

int UtcDaliTapGestureDetectorCopyConstructorP(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();

  TapGestureDetector copy( detector );
  DALI_TEST_CHECK( detector );
  END_TEST;
}

int UtcDaliTapGestureDetectorAssignmentOperatorP(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();;

  TapGestureDetector assign;
  assign = detector;
  DALI_TEST_CHECK( detector );

  DALI_TEST_CHECK( detector == assign );
  END_TEST;
}

int UtcDaliTapGestureDetectorNew(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();
  DALI_TEST_CHECK(detector);
  DALI_TEST_EQUALS(1u, detector.GetMinimumTapsRequired(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, detector.GetMaximumTapsRequired(), TEST_LOCATION);

  TapGestureDetector detector2 = TapGestureDetector::New( 5u );
  DALI_TEST_CHECK(detector2);
  DALI_TEST_EQUALS(5u, detector2.GetMinimumTapsRequired(), TEST_LOCATION);
  DALI_TEST_EQUALS(5u, detector2.GetMaximumTapsRequired(), TEST_LOCATION);

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
  Integration::Point point;
  point.SetDeviceId( 1 );
  point.SetState( PointState::DOWN );
  point.SetScreenPosition( Vector2( 20.0f, 20.0f ) );
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

int UtcDaliTapGestureSetTapsRequiredMinMaxCheck(void)
{
  TestApplication application;

  // Attach an actor and change the required touches

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Set the minimum to be greater than the maximum, should Assert

  try
  {
    TapGestureDetector detector = TapGestureDetector::New();
    detector.SetMinimumTapsRequired( 7u );
    detector.SetMaximumTapsRequired( 3u );
    detector.Attach(actor);
    DALI_TEST_CHECK( false ); // Should not get here
  }
  catch ( DaliException& e )
  {
    DALI_TEST_CHECK( true );
  }

  END_TEST;
}

int UtcDaliTapGestureGetTapsRequired(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();
  DALI_TEST_EQUALS(1u, detector.GetMinimumTapsRequired(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, detector.GetMaximumTapsRequired(), TEST_LOCATION);
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
  TestGenerateTap( application, 112.0f, 112.0f, 100 );
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
  TestGenerateTap( application, 50.0f, 50.0f, 100 );
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
  TestGenerateTap( application, 20.0f, 20.0f, 100 );
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTaps, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS( Vector2(20.0f, 20.0f), data.receivedGesture.localPoint, 0.1, TEST_LOCATION);

  // repeat the tap within the actor's area - we should still receive the signal
  data.Reset();
  TestGenerateTap( application, 50.0f, 50.0f, 700 );
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTaps, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS( Vector2(50.0f, 50.0f), data.receivedGesture.localPoint, 0.1, TEST_LOCATION);

  // Detach actor
  detector.DetachAll();

  // Ensure we are no longer signalled
  data.Reset();
  TestGenerateTap( application, 20.0f, 20.0f, 1300 );
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  TestGenerateTap( application, 50.0f, 50.0f, 1900 );
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
    TestGenerateTap( application, 20.0f, 20.0f, 100 );
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

    // Remove the actor from stage and reset the data
    Stage::GetCurrent().Remove(actor);

    // Render and notify
    application.SendNotification();
    application.Render();
  }

  // Actor should now have been destroyed

  data.Reset();
  TestGenerateTap( application, 20.0f, 20.0f, 700 );
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSignalReceptionRotatedActor(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetOrientation(Dali::Degree(90.0f), Vector3::ZAXIS);
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
  TestGenerateTap( application, 5.0f, 5.0f, 100 );
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTaps, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS( Vector2(5.0f, 5.0f), data.receivedGesture.screenPoint, 0.1, TEST_LOCATION);

  // Rotate actor again and render
  actor.SetOrientation(Dali::Degree(180.0f), Vector3::ZAXIS);
  application.SendNotification();
  application.Render();

  // Do tap, should still receive event
  data.Reset();
  TestGenerateTap( application, 5.0f, 5.0f, 700 );
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTaps, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS( Vector2(5.0f, 5.0f), data.receivedGesture.screenPoint, 0.1, TEST_LOCATION);

  // Rotate actor again and render
  actor.SetOrientation(Dali::Degree(90.0f), Vector3::YAXIS);
  application.SendNotification();
  application.Render();

  // Do tap, inside the actor's area (area if it is not rotated), Should not receive the event
  data.Reset();
  TestGenerateTap( application, 70.0f, 70.0f, 1300 );
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
  child.SetOrientation(Dali::Degree(90.0f), Vector3::ZAXIS);
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
  TestGenerateTap( application, 50.0f, 50.0f, 100 );
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, parent == data.tappedActor, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(50.0f, 50.0f), data.receivedGesture.screenPoint, 0.01f, TEST_LOCATION);

  // Attach child and generate same touch points
  // (Also proves that you can detach and then re-attach another actor)
  detector.Attach(child);
  detector.Detach(parent);

  // Do an entire tap, only check finished value
  data.Reset();
  TestGenerateTap( application, 51.0f, 51.0f, 700 );
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
  TestGenerateTap( application, 120.0f, 10.0f, 100 );
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, second == data.tappedActor, TEST_LOCATION);

  // Tap within first actor's area
  data.Reset();
  TestGenerateTap( application, 20.0f, 10.0f, 700 );
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, first == data.tappedActor, TEST_LOCATION);

  // Detach the second actor
  detector.Detach(second);

  // second actor shouldn't receive event
  data.Reset();
  TestGenerateTap( application, 120.0f, 10.0f, 1300 );
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // first actor should continue receiving event
  data.Reset();
  TestGenerateTap( application, 20.0f, 10.0f, 1900 );
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
  TestGenerateTap( application, 50.0f, 10.0f, 100 );
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Actor become invisible - actor should not receive the next pan
  actor.SetVisible(false);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Tap in the same area, shouldn't receive event
  data.Reset();
  TestGenerateTap( application, 50.0f, 10.0f, 700 );
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureSignalReceptionMultipleGestureDetectors(void)
{
  TestApplication application;

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
    TapGestureDetector secondDetector = TapGestureDetector::New( 2 );
    secondDetector.Attach(second);
    secondDetector.DetectedSignal().Connect(&application, functor);

    // Tap within second actor's area
    TestGenerateTap( application, 150.0f, 10.0f, 100 );
    TestGenerateTap( application, 150.0f, 10.0f, 200 );

    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_EQUALS(true, second == data.tappedActor, TEST_LOCATION);

    // Tap continues as single touch gesture - we should not receive any gesture
    data.Reset();
    TestGenerateTap( application, 150.0f, 10.0f, 800 );
    DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

    // Single touch tap starts - first actor should be panned
    data.Reset();
    TestGenerateTap( application, 50.0f, 10.0f, 1400 );
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_EQUALS(true, first == data.tappedActor, TEST_LOCATION);

    // Pan changes to double-touch - we shouldn't receive event
    data.Reset();

    TestGenerateTwoPointTap( application, 50.0f, 10.0f, 60.0f, 20.0f, 2000 );

    DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  }

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
  TestGenerateTap( application, 50.0f, 10.0f, 100 );
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
  TestStartLongPress( application, 50.0f, 10.0f, 100 );
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Move actor somewhere else
  actor.SetPosition( 100.0f, 100.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit Started event, we should not receive the tap.
  TestEndPan( application, Vector2(50.0f, 10.0f), 120 );
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Tap possible in empty area.
  TestStartLongPress( application, 50.0f, 10.0f, 700 );
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Move actor in to the tap position.
  actor.SetPosition( 0.0f, 0.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit Started event, we should not receive the tap.
  TestEndPan( application, Vector2(50.0f, 10.0f), 720 );
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Normal tap in actor's area for completeness.
  TestGenerateTap( application, 50.0f, 10.0f, 1300 );
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
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

  TestGenerateTap( application, 50.0f, 10.0f, 100 );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  tet_result( TET_PASS ); // If we get here, then the actor removal on signal handler was handled gracefully.
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

  // Emit a possible - Down press, as emitted by long press function
  TestStartLongPress( application, 50.0f, 10.0f, 100 );

  // Detach actor and send a Started state, no signal.
  detector.DetachAll();
  TestEndPan( application, Vector2(50.0f, 10.0f), 120 );
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

  // Emit a possible - Down press, as emitted by long press function
  TestStartLongPress( application, 50.0f, 10.0f, 100 );

  // Remove, render and delete actor
  Stage::GetCurrent().Remove(actor);
  application.SendNotification();
  application.Render();
  actor.Reset();

  // Send a Started state, no signal - Up motion as provided by end pan function
  TestEndPan( application, Vector2(50.0f, 10.0f), 120 );
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
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

  // Emit signals, should receive
  TestGenerateTap( application, 50.0f, 50.0f, 100 );
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Set layer to consume all touch
  layer.SetTouchConsumed( true );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit the same signals again, should not receive
  TestGenerateTap( application, 50.0f, 50.0f, 700 );
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  END_TEST;
}
