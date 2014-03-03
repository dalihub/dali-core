//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <iostream>

#include <stdlib.h>
#include <tet_api.h>

#include <dali/public-api/dali-core.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/pinch-gesture-event.h>
#include <dali/integration-api/system-overlay.h>

#include <dali-test-suite-utils.h>

using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliPinchGestureDetectorConstructor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPinchGestureDetectorNew, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPinchGestureDetectorDownCast, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPinchGestureSignalReceptionNegative, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPinchGestureSignalReceptionDownMotionLeave, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPinchGestureSignalReceptionDownMotionUp, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPinchGestureSignalReceptionCancelled, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPinchGestureSignalReceptionDetach, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPinchGestureSignalReceptionDetachWhilePinching, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPinchGestureSignalReceptionActorDestroyedWhilePinching, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPinchGestureSignalReceptionRotatedActor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPinchGestureSignalReceptionChildHit, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPinchGestureSignalReceptionAttachDetachMany, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPinchGestureSignalReceptionActorBecomesUntouchable, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPinchGestureSignalReceptionMultipleDetectorsOnActor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPinchGestureSignalReceptionMultipleStarted, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPinchGestureSignalReceptionEnsureCorrectSignalling, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPinchGestureEmitIncorrectStateClear, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPinchGestureEmitIncorrectStatePossible, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPinchGestureDetectorTypeRegistry, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPinchGestureActorUnstaged, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPinchGestureActorStagedAndDestroyed, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPinchGestureSystemOverlay, POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

///////////////////////////////////////////////////////////////////////////////

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

    pinchedActor = NULL;
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

  void operator()(Actor actor, PinchGesture pinch)
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

  void operator()( Actor actor, PinchGesture pinch )
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

///////////////////////////////////////////////////////////////////////////////

static void UtcDaliPinchGestureDetectorConstructor()
{
  TestApplication application;

  PinchGestureDetector detector;
  DALI_TEST_CHECK(!detector);
}

static void UtcDaliPinchGestureDetectorNew()
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
  application.GetCore().SendEvent(touchEvent);

  TouchPoint point2(1, TouchPoint::Down, 20.0f, 20.0f, 20.0f, 20.0f);
  touchEvent.AddPoint(point2);
  application.GetCore().SendEvent(touchEvent);
}

static void UtcDaliPinchGestureDetectorDownCast()
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
}

// Negative test case for a method
static void UtcDaliPinchGestureSignalReceptionNegative()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

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
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 45.0f, Vector2(112.0f, 112.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Continue pinch into actor's area - we should still not receive the signal
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Continuing, 4.5f, 95.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Stop pinching - we should still not receive the signal
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Finished, 10.0f, 50.0f, Vector2(12.0f, 12.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
}

static void UtcDaliPinchGestureSignalReceptionDownMotionLeave()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

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
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Started, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(10.0f, data.receivedGesture.scale, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(50.0f, data.receivedGesture.speed, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.screenCenterPoint, 0.01f, TEST_LOCATION);

  // Continue the pan within the actor's area - we should still receive the signal
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Continuing, 5.0f, 90.0f, Vector2(21.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Continuing, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(5.0f, data.receivedGesture.scale, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(90.0f, data.receivedGesture.speed, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(21.0f, 20.0f), data.receivedGesture.screenCenterPoint, 0.01f, TEST_LOCATION);

  // Pan Gesture leaves actor's area - we should still receive the signal
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Continuing, 10.0f, 15.5f, Vector2(320.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Continuing, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(10.0f, data.receivedGesture.scale, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(15.5f, data.receivedGesture.speed, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(320.0f, 10.0f), data.receivedGesture.screenCenterPoint, 0.01f, TEST_LOCATION);

  // Gesture ends - we would receive a finished state
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Finished, 15.2f, 12.1f, Vector2(310.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Finished, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(15.2f, data.receivedGesture.scale, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(12.1f, data.receivedGesture.speed, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(310.0f, 10.0f), data.receivedGesture.screenCenterPoint, 0.01f, TEST_LOCATION);
}

static void UtcDaliPinchGestureSignalReceptionDownMotionUp()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

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
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Started, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(10.0f, data.receivedGesture.scale, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(50.0f, data.receivedGesture.speed, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.screenCenterPoint, 0.01f, TEST_LOCATION);

  // Continue the pinch within the actor's area - we should still receive the signal
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Continuing, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Continuing, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(5.0f, data.receivedGesture.scale, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(25.0f, data.receivedGesture.speed, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.screenCenterPoint, 0.01f, TEST_LOCATION);

  // Gesture ends within actor's area - we would receive a finished state
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Finished, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Finished, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(5.0f, data.receivedGesture.scale, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(25.0f, data.receivedGesture.speed, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 20.0f), data.receivedGesture.screenCenterPoint, 0.01f, TEST_LOCATION);
}

static void UtcDaliPinchGestureSignalReceptionCancelled()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

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
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Started, data.receivedGesture.state, TEST_LOCATION);


  // Continue the pinch within the actor's area - we should still receive the signal
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Continuing, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Continuing, data.receivedGesture.state, TEST_LOCATION);

  // The gesture is cancelled
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Cancelled, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Cancelled, data.receivedGesture.state, TEST_LOCATION);
}

static void UtcDaliPinchGestureSignalReceptionDetach()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

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
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Started, data.receivedGesture.state, TEST_LOCATION);


  // Continue the pinch within the actor's area - we should still receive the signal
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Continuing, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Continuing, data.receivedGesture.state, TEST_LOCATION);

  // Gesture ends within actor's area
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Finished, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Finished, data.receivedGesture.state, TEST_LOCATION);

  // Detach actor
  detector.DetachAll();

  // Ensure we are no longer signalled
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 20.0f)));
  core.SendEvent(GeneratePinch(Gesture::Continuing, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
  core.SendEvent(GeneratePinch(Gesture::Finished, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
}

static void UtcDaliPinchGestureSignalReceptionDetachWhilePinching()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

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
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Started, data.receivedGesture.state, TEST_LOCATION);

  // Continue the pinch within the actor's area - we should still receive the signal
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Continuing, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Continuing, data.receivedGesture.state, TEST_LOCATION);

  // Detach actor during the pinch, we should not receive the next event
  detector.DetachAll();

  // Gesture ends within actor's area
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Finished, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
}

static void UtcDaliPinchGestureSignalReceptionActorDestroyedWhilePinching()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

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
    core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(10.0f, 10.0f)));
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_EQUALS(Gesture::Started, data.receivedGesture.state, TEST_LOCATION);

    // Continue the pinch within the actor's area - we should still receive the signal
    data.Reset();
    core.SendEvent(GeneratePinch(Gesture::Continuing, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
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
  core.SendEvent(GeneratePinch(Gesture::Finished, 5.0f, 25.0f, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
}

static void UtcDaliPinchGestureSignalReceptionRotatedActor()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetRotation(Dali::Degree(90.0f), Vector3::ZAXIS);
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
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(10.0f, 10.0f)));
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Finished, 10.0f, 50.0f, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(10.0f, data.receivedGesture.scale, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(50.0f, data.receivedGesture.speed, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(10.0f, 10.0f), data.receivedGesture.screenCenterPoint, 0.01f, TEST_LOCATION);

  // Rotate actor again and render and notify
  actor.SetRotation(Dali::Degree(180.0f), Vector3::ZAXIS);
  application.SendNotification();
  application.Render();

  // Do an entire pinch, only check finished value
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(10.0f, 10.0f)));
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Finished, 10.0f, 50.0f, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(10.0f, data.receivedGesture.scale, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(50.0f, data.receivedGesture.speed, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(10.0f, 10.0f), data.receivedGesture.screenCenterPoint, 0.01f, TEST_LOCATION);

  // Rotate actor again and render and notify
  actor.SetRotation(Dali::Degree(270.0f), Vector3::ZAXIS);
  application.SendNotification();
  application.Render();

  // Do an entire pinch, only check finished value
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(10.0f, 10.0f)));
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Finished, 10.0f, 50.0f, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(10.0f, data.receivedGesture.scale, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(50.0f, data.receivedGesture.speed, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(10.0f, 10.0f), data.receivedGesture.screenCenterPoint, 0.01f, TEST_LOCATION);
}

static void UtcDaliPinchGestureSignalReceptionChildHit()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

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

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(parent);
  detector.DetectedSignal().Connect(&application, functor);

  // Do an entire pan, only check finished value - hits child area but parent should still receive it
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(10.0f, 10.0f)));
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Finished, 5.0f, 50.0f, Vector2(10.0f, 10.0f)));
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
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(10.0f, 10.0f)));
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Finished, 5.0f, 50.0f, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, child == data.pinchedActor, TEST_LOCATION);
  DALI_TEST_EQUALS(5.0f, data.receivedGesture.scale, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(50.0f, data.receivedGesture.speed, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(10.0f, 10.0f), data.receivedGesture.screenCenterPoint, 0.01f, TEST_LOCATION);
}

static void UtcDaliPinchGestureSignalReceptionAttachDetachMany()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

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
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(120.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, second == data.pinchedActor, TEST_LOCATION);

  // Pinch moves into first actor's area - second actor should receive the pinch
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Continuing, 10.0f, 50.0f, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, second == data.pinchedActor, TEST_LOCATION);

  // Detach the second actor during the pinch, we should not receive the next event
  detector.Detach(second);

  // Gesture ends within actor's area
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Finished, 10.0f, 50.0f, Vector2(120.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
}

static void UtcDaliPinchGestureSignalReceptionActorBecomesUntouchable()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

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
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Pan continues within actor's area
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Started, 5.0f, 50.0f, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Actor become invisible - actor should not receive the next pinch
  actor.SetVisible(false);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Gesture ends within actor's area
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Finished, 5.0f, 50.0f, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
}

void UtcDaliPinchGestureSignalReceptionMultipleDetectorsOnActor()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

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
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // Pinch continues in actor's area - both detector's functors should be called
  firstData.Reset();
  secondData.Reset();
  core.SendEvent(GeneratePinch(Gesture::Continuing, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // Detach actor from firstDetector and emit pinch on actor, only secondDetector's functor should be called.
  firstDetector.Detach(actor);
  firstData.Reset();
  secondData.Reset();
  core.SendEvent(GeneratePinch(Gesture::Finished, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(false, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // New pinch on actor, only secondDetector has actor attached
  firstData.Reset();
  secondData.Reset();
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(false, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // Detach actor from secondDetector
  secondDetector.Detach(actor);
  firstData.Reset();
  secondData.Reset();
  core.SendEvent(GeneratePinch(Gesture::Continuing, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(false, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, secondData.functorCalled, TEST_LOCATION);
}

void UtcDaliPinchGestureSignalReceptionMultipleStarted()
{
  // Should handle two started events gracefully.

  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

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
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Send another start in actor's area
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
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
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Send another start in actor's area
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
}

void UtcDaliPinchGestureSignalReceptionEnsureCorrectSignalling()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

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
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data1.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, data2.functorCalled, TEST_LOCATION);
}

void UtcDaliPinchGestureEmitIncorrectStateClear()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

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
    core.SendEvent(GeneratePinch(Gesture::Clear, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
    tet_result(TET_FAIL);
  }
  catch ( Dali::DaliException& e )
  {
    DALI_TEST_ASSERT( e, "false", TEST_LOCATION );
  }
}

void UtcDaliPinchGestureEmitIncorrectStatePossible()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

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
    core.SendEvent(GeneratePinch(Gesture::Possible, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
    tet_result(TET_FAIL);
  }
  catch ( Dali::DaliException& e )
  {
    DALI_TEST_ASSERT( e, "false", TEST_LOCATION );
  }
}

void UtcDaliPinchGestureDetectorTypeRegistry()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo( "PinchGestureDetector" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );
  PinchGestureDetector detector = PinchGestureDetector::DownCast( handle );
  DALI_TEST_CHECK( detector );

  // Attach actor to detector
  SignalData data;
  GestureReceivedFunctor functor( data );
  detector.Attach(actor);

  // Connect to signal through type
  handle.ConnectSignal( &application, PinchGestureDetector::SIGNAL_PINCH_DETECTED, functor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit gesture
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.voidFunctorCalled, TEST_LOCATION);
}

void UtcDaliPinchGestureActorUnstaged()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

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
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Finished, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
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
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Continuing, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Finished, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
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
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Continuing, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Finished, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  tet_result( TET_PASS ); // If we get here then we have handled actor stage removal gracefully.
}

void UtcDaliPinchGestureActorStagedAndDestroyed()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

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
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
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
  core.SendEvent(GeneratePinch(Gesture::Continuing, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Finished, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Here we delete an actor in started, we should not receive any subsequent signalling.

  // Emit signals
  core.SendEvent(GeneratePinch(Gesture::Started, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Delete actor as well
  actor = NULL;

  // Render and notify
  application.SendNotification();
  application.Render();

  // Continue signal emission
  core.SendEvent(GeneratePinch(Gesture::Continuing, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();
  core.SendEvent(GeneratePinch(Gesture::Finished, 10.0f, 50.0f, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
}

void UtcDaliPinchGestureSystemOverlay()
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
  core.SendEvent( GeneratePinch( Gesture::Started, scale, speed, screenCoords ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
}
