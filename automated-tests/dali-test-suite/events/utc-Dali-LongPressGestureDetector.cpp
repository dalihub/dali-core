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
#include <dali/integration-api/events/long-press-gesture-event.h>
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
TEST_FUNCTION( UtcDaliLongPressGestureDetectorConstructor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureDetectorNew, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureDetectorDownCast, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureSetTouchesRequired01, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureSetTouchesRequired02, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureGetMinimumTouchesRequired, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureGetMaximumTouchesRequired, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureSignalReceptionNegative, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureSignalReceptionPositive, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureSignalReceptionDetach, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureSignalReceptionActorDestroyedDuringLongPress, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureSignalReceptionRotatedActor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureSignalReceptionChildHit, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureSignalReceptionAttachDetachMany, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureSignalReceptionActorBecomesUntouchable, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureSignalReceptionMultipleGestureDetectors, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureSignalReceptionMultipleDetectorsOnActor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureSignalReceptionDifferentPossible, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureEmitIncorrectStateClear, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureEmitIncorrectStateContinuing, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureDetectorTypeRegistry, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureRepeatedState, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGesturePossibleCancelled, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureDetachAfterStarted, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureActorUnstaged, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureActorStagedAndDestroyed, NEGATIVE_TC_IDX );
TEST_FUNCTION( UtcDaliLongPressGestureSystemOverlay, POSITIVE_TC_IDX );

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
  : functorCalled( false ),
    voidFunctorCalled( false ),
    receivedGesture( Gesture::Clear ),
    pressedActor()
  {}

  void Reset()
  {
    functorCalled = false;
    voidFunctorCalled = false;

    receivedGesture.numberOfTouches = 0u;
    receivedGesture.screenPoint = Vector2(0.0f, 0.0f);
    receivedGesture.localPoint = Vector2(0.0f, 0.0f);

    pressedActor = NULL;
  }

  bool functorCalled;
  bool voidFunctorCalled;
  LongPressGesture receivedGesture;
  Actor pressedActor;
};

// Functor that sets the data when called
struct GestureReceivedFunctor
{
  GestureReceivedFunctor(SignalData& data) : signalData(data) { }

  void operator()(Actor actor, LongPressGesture longPress)
  {
    signalData.functorCalled = true;
    signalData.receivedGesture = longPress;
    signalData.pressedActor = actor;
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

  void operator()( Actor actor, LongPressGesture longPress )
  {
    GestureReceivedFunctor::operator()( actor, longPress );

    if ( longPress.state == stateToUnstage )
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

// Generate a LongPressGestureEvent to send to Core
Integration::LongPressGestureEvent GenerateLongPress(
    Gesture::State state,
    unsigned int numberOfTouches,
    Vector2 point)
{
  Integration::LongPressGestureEvent longPress( state );

  longPress.numberOfTouches = numberOfTouches;
  longPress.point = point;

  return longPress;
}

///////////////////////////////////////////////////////////////////////////////


// Positive test case for a method
static void UtcDaliLongPressGestureDetectorConstructor()
{
  TestApplication application;

  LongPressGestureDetector detector;
  DALI_TEST_CHECK(!detector);
}


static void UtcDaliLongPressGestureDetectorNew()
{
  TestApplication application;

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  DALI_TEST_CHECK(detector);
  DALI_TEST_EQUALS(1u, detector.GetMinimumTouchesRequired(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, detector.GetMaximumTouchesRequired(), TEST_LOCATION);

  LongPressGestureDetector detector2 = LongPressGestureDetector::New(5u);
  DALI_TEST_CHECK(detector2);
  DALI_TEST_EQUALS(5u, detector2.GetMinimumTouchesRequired(), TEST_LOCATION);
  DALI_TEST_EQUALS(5u, detector2.GetMaximumTouchesRequired(), TEST_LOCATION);

  LongPressGestureDetector detector3 = LongPressGestureDetector::New(5u, 7u);
  DALI_TEST_CHECK(detector2);
  DALI_TEST_EQUALS(5u, detector3.GetMinimumTouchesRequired(), TEST_LOCATION);
  DALI_TEST_EQUALS(7u, detector3.GetMaximumTouchesRequired(), TEST_LOCATION);

  //Scoped test to test destructor
  {
    LongPressGestureDetector detector4 = LongPressGestureDetector::New();
    DALI_TEST_CHECK(detector4);
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
  actor.TouchedSignal().Connect(&application, touchFunctor);

  Integration::TouchEvent touchEvent(1);
  TouchPoint point(1, TouchPoint::Down, 20.0f, 20.0f);
  touchEvent.AddPoint(point);
  application.GetCore().SendEvent(touchEvent);

  // Render and notify
  application.SendNotification();
  application.Render();

  // For line coverage, Initialise default constructor
  TouchEvent touchEvent2;
}

static void UtcDaliLongPressGestureDetectorDownCast()
{
  TestApplication application;
  tet_infoline("Testing Dali::LongPressGestureDetector::DownCast()");

  LongPressGestureDetector detector = LongPressGestureDetector::New();

  BaseHandle object(detector);

  LongPressGestureDetector detector2 = LongPressGestureDetector::DownCast(object);
  DALI_TEST_CHECK(detector2);

  LongPressGestureDetector detector3 = DownCast< LongPressGestureDetector >(object);
  DALI_TEST_CHECK(detector3);

  BaseHandle unInitializedObject;
  LongPressGestureDetector detector4 = LongPressGestureDetector::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!detector4);

  LongPressGestureDetector detector5 = DownCast< LongPressGestureDetector >(unInitializedObject);
  DALI_TEST_CHECK(!detector5);

  GestureDetector detector6 = LongPressGestureDetector::New();
  LongPressGestureDetector detector7 = LongPressGestureDetector::DownCast(detector6);
  DALI_TEST_CHECK(detector7);
}

static void UtcDaliLongPressGestureSetTouchesRequired01()
{
  TestApplication application;

  LongPressGestureDetector detector = LongPressGestureDetector::New();

  unsigned int touches = 3;

  DALI_TEST_CHECK(touches != detector.GetMinimumTouchesRequired());
  DALI_TEST_CHECK(touches != detector.GetMaximumTouchesRequired());

  detector.SetTouchesRequired(touches);

  DALI_TEST_EQUALS(touches, detector.GetMinimumTouchesRequired(), TEST_LOCATION);
  DALI_TEST_EQUALS(touches, detector.GetMaximumTouchesRequired(), TEST_LOCATION);

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
  detector.DetectedSignal().Connect(&application, functor);

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
  LongPressGestureDetector secondDetector = LongPressGestureDetector::New();
  secondDetector.Attach(actor);

  // Gesture detection should have been updated
  DALI_TEST_EQUALS(true, gestureManager.WasCalled(TestGestureManager::UpdateType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::RegisterType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);
}

static void UtcDaliLongPressGestureSetTouchesRequired02()
{
  TestApplication application;

  LongPressGestureDetector detector = LongPressGestureDetector::New();

  unsigned int min = 3;
  unsigned int max = 5;

  DALI_TEST_CHECK(min != detector.GetMinimumTouchesRequired());
  DALI_TEST_CHECK(max != detector.GetMaximumTouchesRequired());

  detector.SetTouchesRequired(min, max);

  DALI_TEST_EQUALS(min, detector.GetMinimumTouchesRequired(), TEST_LOCATION);
  DALI_TEST_EQUALS(max, detector.GetMaximumTouchesRequired(), TEST_LOCATION);

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
  detector.DetectedSignal().Connect(&application, functor);

  TestGestureManager& gestureManager = application.GetGestureManager();
  gestureManager.Initialize();

  detector.SetTouchesRequired(4, 5);

  // Gesture detection should have been updated only
  DALI_TEST_EQUALS(true, gestureManager.WasCalled(TestGestureManager::UpdateType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::RegisterType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);

  // Reset values
  gestureManager.Initialize();

  // Create a second gesture detector that requires even less maximum touches
  LongPressGestureDetector secondDetector = LongPressGestureDetector::New();
  secondDetector.Attach(actor);

  // Gesture detection should have been updated
  DALI_TEST_EQUALS(true, gestureManager.WasCalled(TestGestureManager::UpdateType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::RegisterType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);
}

static void UtcDaliLongPressGestureGetMinimumTouchesRequired()
{
  TestApplication application;

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  DALI_TEST_EQUALS(1u, detector.GetMinimumTouchesRequired(), TEST_LOCATION);
}

static void UtcDaliLongPressGestureGetMaximumTouchesRequired()
{
  TestApplication application;

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  DALI_TEST_EQUALS(1u, detector.GetMaximumTouchesRequired(), TEST_LOCATION);
}

static void UtcDaliLongPressGestureSignalReceptionNegative()
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

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Do a long press outside actor's area
  core.SendEvent( GenerateLongPress( Gesture::Possible, 1u, Vector2(112.0f, 112.0f ) ) );
  core.SendEvent( GenerateLongPress( Gesture::Started,  1u, Vector2(112.0f, 112.0f ) ) );
  core.SendEvent( GenerateLongPress( Gesture::Finished, 1u, Vector2(112.0f, 112.0f ) ) );
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
}

static void UtcDaliLongPressGestureSignalReceptionPositive()
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

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Do a long press inside actor's area
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(50.0f, 50.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Started,  1u, Vector2(50.0f, 50.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS( Vector2(50.0f, 50.0f), data.receivedGesture.localPoint, 0.1, TEST_LOCATION);
  core.SendEvent(GenerateLongPress(Gesture::Finished,  1u, Vector2(50.0f, 50.0f)));
}

static void UtcDaliLongPressGestureSignalReceptionDetach()
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

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start long press within the actor's area
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(20.0f, 20.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Started,  1u, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS( Vector2(20.0f, 20.0f), data.receivedGesture.localPoint, 0.1, TEST_LOCATION);
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(20.0f, 20.0f)));

  // repeat the long press within the actor's area - we should still receive the signal
  data.Reset();
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(50.0f, 50.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Started,  1u, Vector2(50.0f, 50.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS( Vector2(50.0f, 50.0f), data.receivedGesture.localPoint, 0.1, TEST_LOCATION);
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(50.0f, 50.0f)));

  // Detach actor
  detector.DetachAll();

  // Ensure we are no longer signalled
  data.Reset();
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(20.0f, 20.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Started,  1u, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(50.0f, 50.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
}

static void UtcDaliLongPressGestureSignalReceptionActorDestroyedDuringLongPress()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  SignalData data;
  GestureReceivedFunctor functor(data);

  LongPressGestureDetector detector = LongPressGestureDetector::New();
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

    // Start long press within the actor's area
    core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(20.0f, 20.0f)));
    core.SendEvent(GenerateLongPress(Gesture::Started,  1u, Vector2(20.0f, 20.0f)));
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

    // Remove the actor from stage and reset the data
    Stage::GetCurrent().Remove(actor);

    // Render and notify
    application.SendNotification();
    application.Render();
  }

  // Actor should now have been destroyed

  data.Reset();
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
}

static void UtcDaliLongPressGestureSignalReceptionRotatedActor()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetRotation(Dali::Degree(90.0f), Vector3::ZAXIS);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor(data);

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Do a long press
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(5.0f, 5.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Started,  1u, Vector2(5.0f, 5.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(5.0f, 5.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS( Vector2(5.0f, 5.0f), data.receivedGesture.screenPoint, 0.1, TEST_LOCATION);

  // Rotate actor again and render
  actor.SetRotation(Dali::Degree(180.0f), Vector3::ZAXIS);
  application.SendNotification();
  application.Render();

  // Do another long press, should still receive event
  data.Reset();
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(5.0f, 5.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Started,  1u, Vector2(5.0f, 5.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(5.0f, 5.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS( Vector2(5.0f, 5.0f), data.receivedGesture.screenPoint, 0.1, TEST_LOCATION);

  // Rotate actor again and render
  actor.SetRotation(Dali::Degree(90.0f), Vector3::YAXIS);
  application.SendNotification();
  application.Render();

  // Do a long press, inside where the actor used to be, Should not receive the event
  data.Reset();
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(70.0f, 70.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Started,  1u, Vector2(70.0f, 70.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(70.0f, 70.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
}

static void UtcDaliLongPressGestureSignalReceptionChildHit()
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

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(parent);
  detector.DetectedSignal().Connect(&application, functor);

  // Do long press - hits child area but parent should still receive it
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(50.0f, 50.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Started,  1u, Vector2(50.0f, 50.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(50.0f, 50.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, parent == data.pressedActor, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(50.0f, 50.0f), data.receivedGesture.screenPoint, 0.01f, TEST_LOCATION);

  // Attach child and generate same touch points
  // (Also proves that you can detach and then re-attach another actor)
  detector.Attach(child);
  detector.Detach(parent);

  // Do an entire long press, only check finished value
  data.Reset();
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(51.0f, 51.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Started,  1u, Vector2(51.0f, 51.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(51.0f, 51.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, child == data.pressedActor, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(51.0f, 51.0f), data.receivedGesture.screenPoint, 0.01f, TEST_LOCATION);
}

static void UtcDaliLongPressGestureSignalReceptionAttachDetachMany()
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

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(first);
  detector.Attach(second);
  detector.DetectedSignal().Connect(&application, functor);

  // LongPress within second actor's area
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(120.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Started,  1u, Vector2(120.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(120.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, second == data.pressedActor, TEST_LOCATION);

  // LongPress within first actor's area
  data.Reset();
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(20.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Started,  1u, Vector2(20.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, first == data.pressedActor, TEST_LOCATION);

  // Detach the second actor
  detector.Detach(second);

  // second actor shouldn't receive event
  data.Reset();
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(120.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Started,  1u, Vector2(120.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(120.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // first actor should continue receiving event
  data.Reset();
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(20.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Started,  1u, Vector2(20.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(20.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
}

static void UtcDaliLongPressGestureSignalReceptionActorBecomesUntouchable()
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

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // LongPress in actor's area
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(50.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Started,  1u, Vector2(50.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Actor becomes invisible - actor should not receive the next long press
  actor.SetVisible(false);

  // Render and notify
  application.SendNotification();
  application.Render();

  // LongPress in the same area, shouldn't receive event
  data.Reset();
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(50.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Started,  1u, Vector2(50.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
}

static void UtcDaliLongPressGestureSignalReceptionMultipleGestureDetectors()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();
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

  LongPressGestureDetector firstDetector = LongPressGestureDetector::New();
  firstDetector.Attach(first);
  firstDetector.DetectedSignal().Connect(&application, functor);

  // secondDetector is scoped
  {
    // Reset gestureManager statistics
    gestureManager.Initialize();

    LongPressGestureDetector secondDetector = LongPressGestureDetector::New();
    secondDetector.SetTouchesRequired(2);
    secondDetector.Attach(second);
    secondDetector.DetectedSignal().Connect(&application, functor);

    DALI_TEST_EQUALS(true, gestureManager.WasCalled(TestGestureManager::UpdateType), TEST_LOCATION);
    DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::RegisterType), TEST_LOCATION);
    DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);

    // LongPress within second actor's area
    core.SendEvent(GenerateLongPress(Gesture::Possible, 2u, Vector2(150.0f, 10.0f)));
    core.SendEvent(GenerateLongPress(Gesture::Started,  2u, Vector2(150.0f, 10.0f)));
    core.SendEvent(GenerateLongPress(Gesture::Finished, 2u, Vector2(150.0f, 10.0f)));
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_EQUALS(true, second == data.pressedActor, TEST_LOCATION);

    // LongPress continues as single touch gesture - we should not receive any gesture
    data.Reset();
    core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(150.0f, 10.0f)));
    core.SendEvent(GenerateLongPress(Gesture::Started, 1u, Vector2(150.0f, 10.0f)));
    core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(150.0f, 10.0f)));
    DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

    // Single touch long press starts - first actor should receive gesture
    data.Reset();
    core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(50.0f, 10.0f)));
    core.SendEvent(GenerateLongPress(Gesture::Started,  1u, Vector2(50.0f, 10.0f)));
    core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(50.0f, 10.0f)));
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_EQUALS(true, first == data.pressedActor, TEST_LOCATION);

    // long press changes to double-touch - we shouldn't receive event
    data.Reset();
    core.SendEvent(GenerateLongPress(Gesture::Possible, 2u, Vector2(50.0f, 10.0f)));
    core.SendEvent(GenerateLongPress(Gesture::Started, 2u, Vector2(50.0f, 10.0f)));
    core.SendEvent(GenerateLongPress(Gesture::Finished, 2u, Vector2(50.0f, 10.0f)));
    DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

    // Reset gesture manager statistics
    gestureManager.Initialize();
  }

  // secondDetector has now been deleted.  Gesture detection should have been updated only
  DALI_TEST_EQUALS(true, gestureManager.WasCalled(TestGestureManager::UpdateType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::RegisterType), TEST_LOCATION);
  DALI_TEST_EQUALS(false, gestureManager.WasCalled(TestGestureManager::UnregisterType), TEST_LOCATION);
}

void UtcDaliLongPressGestureSignalReceptionMultipleDetectorsOnActor()
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

  // Attach actor to one detector
  SignalData firstData;
  GestureReceivedFunctor firstFunctor(firstData);
  LongPressGestureDetector firstDetector = LongPressGestureDetector::New();
  firstDetector.Attach(actor);
  firstDetector.DetectedSignal().Connect(&application, firstFunctor);

  // Attach actor to another detector
  SignalData secondData;
  GestureReceivedFunctor secondFunctor(secondData);
  LongPressGestureDetector secondDetector = LongPressGestureDetector::New();
  secondDetector.Attach(actor);
  secondDetector.DetectedSignal().Connect(&application, secondFunctor);

  // LongPress in actor's area - both detector's functors should be called
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(50.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Started, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(true, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);
}

void UtcDaliLongPressGestureSignalReceptionDifferentPossible()
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
  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // LongPress possible in actor's area.
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Move actor somewhere else
  actor.SetPosition( 100.0f, 100.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit Started event, we should not receive the long press.
  core.SendEvent(GenerateLongPress(Gesture::Started, 1u, Vector2(50.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // LongPress possible in empty area.
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Move actor in to the long press position.
  actor.SetPosition( 0.0f, 0.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit Started event, we should not receive the long press.
  core.SendEvent(GenerateLongPress(Gesture::Started, 1u, Vector2(50.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Normal long press in actor's area for completeness.
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(50.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Started, 1u, Vector2(50.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
}

void UtcDaliLongPressGestureEmitIncorrectStateClear()
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
  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Try a Clear state
  try
  {
    core.SendEvent(GenerateLongPress(Gesture::Clear, 1u, Vector2(50.0f, 10.0f)));
    tet_result(TET_FAIL);
  }
  catch ( Dali::DaliException& e )
  {
    DALI_TEST_ASSERT( e, "false", TEST_LOCATION );
  }
}

void UtcDaliLongPressGestureEmitIncorrectStateContinuing()
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
  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Try a Continuing state
  try
  {
    core.SendEvent(GenerateLongPress(Gesture::Continuing, 1u, Vector2(50.0f, 10.0f)));
    tet_result(TET_FAIL);
  }
  catch ( Dali::DaliException& e )
  {
    DALI_TEST_ASSERT( e, "false", TEST_LOCATION );
  }
}

void UtcDaliLongPressGestureDetectorTypeRegistry()
{
  TestApplication application;
  Dali::Integration::Core& core = application.GetCore();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Register Type
  TypeInfo type;
  type = TypeRegistry::Get().GetTypeInfo( "LongPressGestureDetector" );
  DALI_TEST_CHECK( type );
  BaseHandle handle = type.CreateInstance();
  DALI_TEST_CHECK( handle );
  LongPressGestureDetector detector = LongPressGestureDetector::DownCast( handle );
  DALI_TEST_CHECK( detector );

  // Attach actor to detector
  SignalData data;
  GestureReceivedFunctor functor( data );
  detector.Attach(actor);

  // Connect to signal through type
  handle.ConnectSignal( &application, LongPressGestureDetector::SIGNAL_LONG_PRESS_DETECTED, functor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit gesture
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(50.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Started, 1u, Vector2(50.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.voidFunctorCalled, TEST_LOCATION);
}

void UtcDaliLongPressGestureRepeatedState()
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
  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Two possibles
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // ... Send some finished states, still no signal
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Send two Started states, should be signalled
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  core.SendEvent(GenerateLongPress(Gesture::Started, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  core.SendEvent(GenerateLongPress(Gesture::Started, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Send two cancelled states, should not be signalled
  core.SendEvent(GenerateLongPress(Gesture::Cancelled, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  core.SendEvent(GenerateLongPress(Gesture::Cancelled, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
}

void UtcDaliLongPressGesturePossibleCancelled()
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
  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Send a possible followed by a cancel, we should not be signalled
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  core.SendEvent(GenerateLongPress(Gesture::Cancelled, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
}

void UtcDaliLongPressGestureDetachAfterStarted()
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
  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Emit initial signal
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(50.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Started, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Detach actor
  detector.Detach(actor);

  // Emit Finished, no signal
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
}

void UtcDaliLongPressGestureActorUnstaged()
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
  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Emit signals
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(50.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Started, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

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
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(50.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Started, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  tet_result( TET_PASS ); // If we get here then we have handled actor stage removal gracefully.
}

void UtcDaliLongPressGestureActorStagedAndDestroyed()
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
  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.Attach(dummyActor);
  detector.DetectedSignal().Connect( &application, functor );

  // Here we are testing a Started actor which is removed in the Started callback, but then added back
  // before we get a finished state.  As we were removed from the stage, even if we're at the same
  // position, we should still not be signalled.

  // Emit signals
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(50.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Started, 1u, Vector2(50.0f, 10.0f)));
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
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Here we delete an actor in started, we should not receive any subsequent signalling.

  // Emit signals
  core.SendEvent(GenerateLongPress(Gesture::Possible, 1u, Vector2(50.0f, 10.0f)));
  core.SendEvent(GenerateLongPress(Gesture::Started, 1u, Vector2(50.0f, 10.0f)));
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
  core.SendEvent(GenerateLongPress(Gesture::Finished, 1u, Vector2(50.0f, 10.0f)));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
}

void UtcDaliLongPressGestureSystemOverlay()
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

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Do a long press inside actor's area
  Vector2 screenCoords( 50.0f, 50.0f );
  core.SendEvent( GenerateLongPress( Gesture::Possible, 1u, screenCoords ) );
  core.SendEvent( GenerateLongPress( Gesture::Started, 1u, screenCoords ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
}
