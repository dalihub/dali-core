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
#include <cmath>
#include <dali/public-api/dali-core.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/render-task-list-integ.h>
#include <dali/integration-api/profiling.h>
#include <dali/integration-api/input-options.h>
#include <dali/internal/event/events/pan-gesture-event.h>
#include <dali-test-suite-utils.h>
#include <test-touch-utils.h>

using namespace Dali;

void utc_dali_pan_gesture_detector_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_pan_gesture_detector_cleanup(void)
{
  test_return_value = TET_PASS;
}

///////////////////////////////////////////////////////////////////////////////
namespace
{
const int PAN_EVENT_TIME_DELTA = 8;
const int PAN_GESTURE_UPDATE_COUNT = 50;

// Stores data that is populated in the callback and will be read by the test cases
struct SignalData
{
  SignalData()
  : functorCalled(false),
    voidFunctorCalled(false),
    receivedGesture(Gesture::Clear)
  {}

  void Reset()
  {
    functorCalled = false;
    voidFunctorCalled = false;

    receivedGesture.state = Gesture::Clear;
    receivedGesture.velocity = Vector2(0.0f, 0.0f);
    receivedGesture.displacement = Vector2(0.0f, 0.0f);
    receivedGesture.position = Vector2(0.0f, 0.0f);
    receivedGesture.screenPosition = Vector2(0.0f, 0.0f);
    receivedGesture.numberOfTouches = 0;

    pannedActor.Reset();
  }

  bool functorCalled;
  bool voidFunctorCalled;
  PanGesture receivedGesture;
  Actor pannedActor;
};

// Functor that sets the data when called
struct GestureReceivedFunctor
{
  GestureReceivedFunctor(SignalData& data) : signalData(data) { }

  void operator()(Actor actor, const PanGesture& pan)
  {
    signalData.functorCalled = true;
    signalData.receivedGesture = pan;
    signalData.pannedActor = actor;
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

  void operator()( Actor actor, const PanGesture& pan )
  {
    GestureReceivedFunctor::operator()( actor, pan );

    if ( pan.state == stateToUnstage )
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

// Data for constraints
struct ConstraintData
{
  ConstraintData()
  : panning( false ),
    called( false )
  {
  }

  Vector2 screenPosition;
  Vector2 screenDisplacement;
  Vector2 screenVelocity;
  Vector2 localPosition;
  Vector2 localDisplacement;
  Vector2 localVelocity;
  bool panning;
  bool called;

  void Reset()
  {
    screenPosition = screenDisplacement = screenVelocity = localPosition = localDisplacement = localVelocity = Vector2::ZERO;
    panning = false;
    called = false;
  }
};

// Constraint used with panning properties
struct PanConstraint
{
  PanConstraint( ConstraintData& data ) : constraintData(data) { }

  void operator()( Vector3& current, const PropertyInputContainer& inputs )
  {
    constraintData.screenPosition = inputs[0]->GetVector2();
    constraintData.screenDisplacement = inputs[1]->GetVector2();
    constraintData.screenVelocity = inputs[2]->GetVector2();
    constraintData.localPosition = inputs[3]->GetVector2();
    constraintData.localDisplacement = inputs[4]->GetVector2();
    constraintData.localVelocity = inputs[5]->GetVector2();
    constraintData.panning = inputs[6]->GetBoolean();
    constraintData.called = true;
    current = Vector3::ZERO;
  }

  ConstraintData& constraintData;
};

// Generate a PanGesture
PanGesture GeneratePan( unsigned int time,
                        Gesture::State state,
                        Vector2 screenPosition,
                        Vector2 localPosition,
                        Vector2 screenDisplacement = Vector2::ONE,
                        Vector2 localDisplacement = Vector2::ONE,
                        Vector2 screenVelocity = Vector2::ONE,
                        Vector2 localVelocity = Vector2::ONE,
                        unsigned int numberOfTouches = 1 )
{
  PanGesture pan( state );

  pan.time = time;

  pan.screenPosition = screenPosition;
  pan.position = localPosition;

  pan.screenDisplacement = screenDisplacement;
  pan.displacement = localDisplacement;

  pan.screenVelocity = screenVelocity;
  pan.velocity = localVelocity;

  pan.numberOfTouches = numberOfTouches;

  return pan;
}

} // anon namespace

///////////////////////////////////////////////////////////////////////////////

// Positive test case for a method
int UtcDaliPanGestureDetectorConstructor(void)
{
  TestApplication application;

  PanGestureDetector detector;
  DALI_TEST_CHECK(!detector);
  END_TEST;
}

int UtcDaliPanGestureDetectorCopyConstructorP(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  PanGestureDetector copy( detector );
  DALI_TEST_CHECK( detector );
  END_TEST;
}

int UtcDaliPanGestureDetectorAssignmentOperatorP(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  PanGestureDetector assign;
  assign = detector;
  DALI_TEST_CHECK( detector );

  DALI_TEST_CHECK( detector == assign );
  END_TEST;
}

// Negative test case for a method
int UtcDaliPanGestureDetectorNew(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  DALI_TEST_CHECK(detector);

  DALI_TEST_EQUALS(1u, detector.GetMinimumTouchesRequired(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, detector.GetMaximumTouchesRequired(), TEST_LOCATION);

  // Attach an actor and emit a touch event on the actor to ensure complete line coverage
  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  detector.Attach(actor);

  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Use long press function for touch event
  TestStartLongPress( application );

  END_TEST;
}

int UtcDaliPanGestureDetectorDownCast(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::GestureDetector::DownCast()");

  PanGestureDetector detector = PanGestureDetector::New();

  BaseHandle object(detector);

  PanGestureDetector detector2 = PanGestureDetector::DownCast(object);
  DALI_TEST_CHECK(detector2);

  PanGestureDetector detector3 = DownCast< PanGestureDetector >(object);
  DALI_TEST_CHECK(detector3);

  BaseHandle unInitializedObject;
  PanGestureDetector detector4 = PanGestureDetector::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!detector4);

  PanGestureDetector detector5 = DownCast< PanGestureDetector >(unInitializedObject);
  DALI_TEST_CHECK(!detector5);

  GestureDetector detector6 = PanGestureDetector::New();
  PanGestureDetector detector7 = PanGestureDetector::DownCast(detector6);
  DALI_TEST_CHECK(detector7);
  END_TEST;
}

int UtcDaliPanGestureSetMinimumTouchesRequired(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  unsigned int min = 2;

  DALI_TEST_CHECK(min != detector.GetMinimumTouchesRequired());

  detector.SetMinimumTouchesRequired(min);

  DALI_TEST_EQUALS(min, detector.GetMinimumTouchesRequired(), TEST_LOCATION);

  // Attach an actor and change the minimum touches

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

  detector.SetMinimumTouchesRequired(3);

  // Create a second gesture detector that requires even less minimum touches
  PanGestureDetector secondDetector = PanGestureDetector::New();
  secondDetector.Attach(actor);

  DALI_TEST_EQUALS(3, detector.GetMinimumTouchesRequired(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureSetMaximumTouchesRequired(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  unsigned int max = 3;

  DALI_TEST_CHECK(max != detector.GetMaximumTouchesRequired());

  detector.SetMaximumTouchesRequired(max);

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

  detector.SetMaximumTouchesRequired(4);

  DALI_TEST_EQUALS(4, detector.GetMaximumTouchesRequired(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureGetMinimumTouchesRequired(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();
  DALI_TEST_EQUALS(1u, detector.GetMinimumTouchesRequired(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureGetMaximumTouchesRequired(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();
  DALI_TEST_EQUALS(1u, detector.GetMaximumTouchesRequired(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSignalReceptionNegative(void)
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

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Do a pan outside actor's area
  uint32_t time = 100;
  TestStartPan( application, Vector2(110.0f, 110.0f), Vector2(121.0f, 121.0f), time );

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Continue pan into actor's area - we should still not receive the signal
  data.Reset();
  TestMovePan( application, Vector2(20.0f, 20.0f), time );

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Stop panning - we should still not receive the signal
  data.Reset();
  TestEndPan( application, Vector2(12.0f, 12.0f), time);

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSignalReceptionDownMotionLeave(void)
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

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pan within the actor's area
  uint32_t time = 100;
  TestStartPan( application, Vector2( 10.0f, 20.0f ),  Vector2( 26.0f, 20.0f ), time );

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Started, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(16.0f, 0.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(0.5f, 0.0f), data.receivedGesture.velocity, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(16.0f, data.receivedGesture.GetDistance(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(0.5f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);

  // Continue the pan within the actor's area - we should still receive the signal
  data.Reset();

  TestMovePan( application, Vector2(26.0f, 4.0f), time );
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Continuing, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(0.0f, -16.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(0.0f, -1.0f), data.receivedGesture.velocity, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(16.0f, data.receivedGesture.GetDistance(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(1.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);

  // Pan Gesture leaves actor's area - we should still receive the signal
  data.Reset();

  TestMovePan( application, Vector2(346.0f, 4.0f), time );
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Continuing, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(320.0f, 0.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 0.0f), data.receivedGesture.velocity, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(320.0f, data.receivedGesture.GetDistance(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(20.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);

  // Gesture ends - we would receive a finished state
  data.Reset();

  TestEndPan( application, Vector2(314.0f, 4.0f), time );

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Finished, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(-32.0f, 0.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(-2.0f, 0.0f), data.receivedGesture.velocity, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(32.0f, data.receivedGesture.GetDistance(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(2.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSignalReceptionDownMotionUp(void)
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

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pan within the actor's area
  uint32_t time = 100;
  TestStartPan( application, Vector2( 10.0f, 20.0f ),  Vector2( 26.0f, 20.0f ), time );

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Started, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(16.0f, 0.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(0.5f, 0.0f), data.receivedGesture.velocity, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(16.0f, data.receivedGesture.GetDistance(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(0.5f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);

  // Continue the pan within the actor's area - we should still receive the signal
  data.Reset();

  TestMovePan( application, Vector2(26.0f, 4.0f), time );
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Continuing, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(0.0f, -16.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(0.0f, -1.0f), data.receivedGesture.velocity, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(16.0f, data.receivedGesture.GetDistance(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(1.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);

  // Gesture ends within actor's area - we would receive a finished state
  data.Reset();

  TestEndPan( application, Vector2(10.0f, 4.0f), time );

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Finished, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(-16.0f, 0.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(-1.0f, 0.0f), data.receivedGesture.velocity, 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(16.0f, data.receivedGesture.GetDistance(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(1.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSignalReceptionDetach(void)
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

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pan within the actor's area
  uint32_t time = 100;
  TestStartPan( application, Vector2( 10.0f, 20.0f ),  Vector2( 26.0f, 20.0f ), time );
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Continue the pan within the actor's area - we should still receive the signal
  data.Reset();

  TestMovePan( application, Vector2(26.0f, 4.0f), time );
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Gesture ends within actor's area
  data.Reset();

  TestEndPan( application, Vector2(10.0f, 4.0f), time );
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Detach actor
  detector.DetachAll();

  // Ensure we are no longer signalled
  data.Reset();

  TestStartPan( application, Vector2( 10.0f, 20.0f ),  Vector2( 26.0f, 20.0f ), time );
  TestMovePan( application, Vector2(26.0f, 4.0f), time );
  time += TestGetFrameInterval();
  TestEndPan( application, Vector2(10.0f, 4.0f), time );

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSignalReceptionDetachWhilePanning(void)
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

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pan within the actor's area
  uint32_t time = 100;
  TestStartPan( application, Vector2( 10.0f, 20.0f ),  Vector2( 26.0f, 20.0f ), time );
  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Continue the pan within the actor's area - we should still receive the signal
  data.Reset();

  TestMovePan( application, Vector2(26.0f, 4.0f), time );
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Detach actor during the pan, we should not receive the next event
  detector.DetachAll();

  // Gesture ends within actor's area
  data.Reset();

  TestEndPan( application, Vector2(10.0f, 4.0f), time );

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSignalReceptionActorDestroyedWhilePanning(void)
{
  TestApplication application;

  SignalData data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.DetectedSignal().Connect(&application, functor);

  // Attach a temporary actor to stop detector being removed from PanGestureProcessor when main actor
  // is destroyed.
  Actor tempActor = Actor::New();
  tempActor.SetSize(100.0f, 100.0f);
  tempActor.SetAnchorPoint(AnchorPoint::BOTTOM_RIGHT);
  Stage::GetCurrent().Add(tempActor);
  detector.Attach(tempActor);

  uint32_t time = 100;

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

    // Start pan within the actor's area
    TestStartPan( application, Vector2( 10.0f, 20.0f ),  Vector2( 26.0f, 20.0f ), time );

    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

    // Continue the pan within the actor's area - we should still receive the signal
    data.Reset();

    TestMovePan( application, Vector2(26.0f, 4.0f), time );
    time += TestGetFrameInterval();

    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

    // Remove the actor from stage and reset the data
    Stage::GetCurrent().Remove(actor);

    // Render and notify
    application.SendNotification();
    application.Render();
  }

  // Actor should now have been destroyed

  // Gesture ends within the area where the actor used to be
  data.Reset();

  TestEndPan( application, Vector2(10.0f, 4.0f), time );

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSignalReceptionRotatedActor(void)
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

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Do an entire pan, only check finished value
  uint32_t time = 100;
  TestStartPan( application, Vector2( 11.0f, 12.0f ),  Vector2( 27.0f, 12.0f ), time );

  data.Reset();

  TestEndPan( application, Vector2(25.0f, 28.0f), time );

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(16.0f, 2.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION); // Actor relative

  // Rotate actor again and render a couple of times
  actor.SetOrientation(Dali::Degree(180.0f), Vector3::ZAXIS);
  application.SendNotification();
  application.Render();

  // Do an entire pan, only check finished value
  TestStartPan( application, Vector2( 11.0f, 12.0f ),  Vector2( 27.0f, 12.0f ), time );

  data.Reset();

  TestEndPan( application, Vector2(25.0f, 28.0f), time );

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(2.0f, -16.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION); // Actor relative

  // Rotate actor again and render a couple of times
  actor.SetOrientation(Dali::Degree(270.0f), Vector3::ZAXIS);
  application.SendNotification();
  application.Render();

  // Do an entire pan, only check finished value
  TestStartPan( application, Vector2( 11.0f, 12.0f ),  Vector2( 27.0f, 12.0f ), time );

  data.Reset();

  TestEndPan( application, Vector2(25.0f, 28.0f), time );

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(-16.0f, -2.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION); // Actor relative
  END_TEST;
}

int UtcDaliPanGestureSignalReceptionChildHit(void)
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

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(parent);
  detector.DetectedSignal().Connect(&application, functor);

  // Do an entire pan, only check finished value - hits child area but parent should still receive it
  uint32_t time = 100;
  TestStartPan( application, Vector2( 11.0f, 12.0f ),  Vector2( 27.0f, 12.0f ), time );

  data.Reset();

  TestEndPan( application, Vector2(25.0f, 28.0f), time );

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, parent == data.pannedActor, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(-2.0f, 16.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION); // Actor relative

  // Attach child and generate same touch points to yield a different displacement
  // (Also proves that you can detach and then re-attach another actor)
  detector.Attach(child);
  detector.Detach(parent);

  // Do an entire pan, only check finished value
  TestStartPan( application, Vector2( 11.0f, 12.0f ),  Vector2( 27.0f, 12.0f ), time );

  data.Reset();

  TestEndPan( application, Vector2(25.0f, 28.0f), time );

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, child == data.pannedActor, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(16.0f, 2.0f), data.receivedGesture.displacement, 0.01f, TEST_LOCATION); // Actor relative
  END_TEST;
}

int UtcDaliPanGestureSignalReceptionAttachDetachMany(void)
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

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(first);
  detector.Attach(second);
  detector.DetectedSignal().Connect(&application, functor);

  DALI_TEST_EQUALS(Stage::GetCurrent().GetRootLayer(), first.GetParent(), TEST_LOCATION);
  DALI_TEST_EQUALS(Stage::GetCurrent().GetRootLayer(), second.GetParent(), TEST_LOCATION);

  // Start pan within second actor's area
  uint32_t time = 100;
  TestStartPan( application, Vector2( 110.0f, 20.0f ),  Vector2( 126.0f, 20.0f ), time );

  DALI_TEST_EQUALS(Stage::GetCurrent().GetRootLayer(), first.GetParent(), TEST_LOCATION);
  DALI_TEST_EQUALS(Stage::GetCurrent().GetRootLayer(), second.GetParent(), TEST_LOCATION);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, second == data.pannedActor, TEST_LOCATION);

  // Pan moves into first actor's area - second actor should receive the pan
  data.Reset();

  TestMovePan( application, Vector2(126.0f, 20.0f), time );
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, second == data.pannedActor, TEST_LOCATION);

  // Detach the second actor during the pan, we should not receive the next event
  detector.Detach(second);

  // Gesture ends within actor's area
  data.Reset();

  TestMovePan( application, Vector2(26.0f, 20.0f), time );
  TestEndPan( application, Vector2(10.0f, 4.0f), time );

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureSignalReceptionActorBecomesUntouchable(void)
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

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pan in actor's area
  uint32_t time = 100;
  TestStartPan( application, Vector2( 10.0f, 20.0f ),  Vector2( 26.0f, 20.0f ), time );

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Pan continues within actor's area
  data.Reset();

  TestMovePan( application, Vector2(26.0f, 4.0f), time );
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Actor become invisible - actor should not receive the next pan
  actor.SetVisible(false);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Gesture ends within actor's area
  data.Reset();

  TestEndPan( application, Vector2(10.0f, 4.0f), time );

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSignalReceptionMultipleDetectorsOnActor(void)
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
  PanGestureDetector firstDetector = PanGestureDetector::New();
  firstDetector.Attach(actor);
  firstDetector.DetectedSignal().Connect(&application, firstFunctor);

  // Attach actor to another detector
  SignalData secondData;
  GestureReceivedFunctor secondFunctor(secondData);
  PanGestureDetector secondDetector = PanGestureDetector::New();
  secondDetector.Attach(actor);
  secondDetector.DetectedSignal().Connect(&application, secondFunctor);

  // Add second actor to second detector, when we remove the actor, this will make sure that this
  // gesture detector is not removed from the GestureDetectorProcessor.  In this scenario, the
  // functor should still not be called (which is what we're also testing).
  secondDetector.Attach(actor2);

  // Pan in actor's area - both detector's functors should be called
  uint32_t time = 100;
  TestStartPan( application, Vector2( 10.0f, 20.0f ),  Vector2( 26.0f, 20.0f ), time );

  DALI_TEST_EQUALS(true, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // Pan continues in actor's area - both detector's functors should be called
  firstData.Reset();
  secondData.Reset();

  TestMovePan( application, Vector2(10.0f, 20.0f), time );
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // Detach actor from firstDetector and emit pan on actor, only secondDetector's functor should be called.
  firstDetector.Detach(actor);
  firstData.Reset();
  secondData.Reset();

  TestEndPan( application, Vector2(10.0f, 4.0f), time );

  DALI_TEST_EQUALS(false, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // New pan on actor, only secondDetector has actor attached
  firstData.Reset();
  secondData.Reset();

  TestStartPan( application, Vector2( 10.0f, 20.0f ),  Vector2( 26.0f, 20.0f ), time );

  DALI_TEST_EQUALS(false, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // Detach actor from secondDetector
  secondDetector.Detach(actor);
  firstData.Reset();
  secondData.Reset();

  TestMovePan( application, Vector2(10.0f, 20.0f), time );

  DALI_TEST_EQUALS(false, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, secondData.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureSignalReceptionEnsureCorrectSignalling(void)
{
  TestApplication application;

  Actor actor1 = Actor::New();
  actor1.SetSize(100.0f, 100.0f);
  actor1.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor1);
  SignalData data1;
  GestureReceivedFunctor functor1(data1);
  PanGestureDetector detector1 = PanGestureDetector::New();
  detector1.Attach(actor1);
  detector1.DetectedSignal().Connect(&application, functor1);

  Actor actor2 = Actor::New();
  actor2.SetSize(100.0f, 100.0f);
  actor2.SetAnchorPoint(AnchorPoint::BOTTOM_RIGHT);
  actor2.SetParentOrigin(ParentOrigin::BOTTOM_RIGHT);
  Stage::GetCurrent().Add(actor2);
  SignalData data2;
  GestureReceivedFunctor functor2(data2);
  PanGestureDetector detector2 = PanGestureDetector::New();
  detector2.Attach(actor2);
  detector2.DetectedSignal().Connect(&application, functor2);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Start pan in actor1's area, only data1 should be set
  uint32_t time = 100;
  TestStartPan( application, Vector2( 10.0f, 20.0f ),  Vector2( 26.0f, 20.0f ), time );

  DALI_TEST_EQUALS(true, data1.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, data2.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSignalReceptionDifferentPossible(void)
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
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Gesture possible in actor's area.
//  application.ProcessEvent(GeneratePan(Gesture::Possible, Vector2(10.0f, 20.0f), Vector2(20.0f, 20.0f), 10));
  uint32_t time = 100;
  TestStartLongPress( application, 10.0f, 20.0f, time );
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Move actor somewhere else
  actor.SetPosition( 100.0f, 100.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit Started event, we should not receive the long press.
  TestStartPan( application, Vector2( 10.0f, 20.0f ),  Vector2( 26.0f, 20.0f ), time );
  TestEndPan( application, Vector2( 26.0f, 20.0f ), time );
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // LongPress possible in empty area.
  TestStartLongPress( application, 10.0f, 20.0f, time );
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Move actor in to the long press position.
  actor.SetPosition( 0.0f, 0.0f );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit Started event, we should not receive the long press.
  TestStartPan( application, Vector2( 10.0f, 20.0f ),  Vector2( 26.0f, 20.0f ), time );
  TestEndPan( application, Vector2( 26.0f, 20.0f ), time );
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Normal long press in actor's area for completeness.
  TestStartPan( application, Vector2( 10.0f, 20.0f ),  Vector2( 26.0f, 20.0f ), time );
  TestEndPan( application, Vector2( 26.0f, 20.0f ), time );
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureActorUnstaged(void)
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
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect( &application, functor );

  // Emit signals
  uint32_t time = 100;
  TestStartPan( application, Vector2( 10.0f, 20.0f ),  Vector2( 26.0f, 20.0f ), time );

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  TestEndPan( application, Vector2(26.0f, 20.0f), time );

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
  TestStartPan( application, Vector2( 10.0f, 20.0f ),  Vector2( 26.0f, 20.0f ), time );

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  TestMovePan( application, Vector2(26.0f, 4.0f), time );
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  data.Reset();

  TestEndPan( application, Vector2(10.0f, 4.0f), time );

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

  // Change state to Gesture::Finished to remove
  stateToUnstage = Gesture::Finished;

  // Emit signals
  TestStartPan( application, Vector2( 10.0f, 20.0f ),  Vector2( 26.0f, 20.0f ), time );

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  TestMovePan( application, Vector2(26.0f, 4.0f), time );
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  TestEndPan( application, Vector2(10.0f, 4.0f), time );

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  tet_result( TET_PASS ); // If we get here then we have handled actor stage removal gracefully.
  END_TEST;
}

int UtcDaliPanGestureActorStagedAndDestroyed(void)
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
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.Attach(dummyActor);
  detector.DetectedSignal().Connect( &application, functor );

  // Here we are testing a Started actor which is removed in the Started callback, but then added back
  // before we get a continuing state.  As we were removed from the stage, even if we're at the same
  // position, we should still not be signalled.

  // Emit signals
  uint32_t time = 100;
  TestStartPan( application, Vector2( 10.0f, 20.0f ),  Vector2( 26.0f, 20.0f ), time );

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
  TestMovePan( application, Vector2(26.0f, 4.0f), time );
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  TestEndPan( application, Vector2(10.0f, 4.0f), time );
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Here we delete an actor in started, we should not receive any subsequent signalling.

  // Emit signals
  TestStartPan( application, Vector2( 10.0f, 20.0f ),  Vector2( 26.0f, 20.0f ), time );

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
  TestMovePan( application, Vector2(26.0f, 4.0f), time );
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  TestEndPan( application, Vector2(10.0f, 4.0f), time );

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureAngleHandling(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();
  DALI_TEST_EQUALS( detector.GetAngleCount(), 0u, TEST_LOCATION );

  detector.AddAngle( PanGestureDetector::DIRECTION_LEFT, Radian( Math::PI * 0.25 ) );
  DALI_TEST_EQUALS( detector.GetAngleCount(), 1u, TEST_LOCATION );
  bool found = false;
  for( size_t i = 0; i < detector.GetAngleCount(); i++)
  {
    if( detector.GetAngle(i).first == PanGestureDetector::DIRECTION_LEFT )
    {
      tet_result( TET_PASS );
      found = true;
      break;
    }
  }

  if(!found )
  {
    tet_printf("%s, angle not added\n", TEST_LOCATION );
    tet_result( TET_FAIL );
  }

  detector.AddAngle( PanGestureDetector::DIRECTION_RIGHT, Radian( Math::PI * 0.25 ) );
  DALI_TEST_EQUALS( detector.GetAngleCount(), 2u, TEST_LOCATION );

  // Remove something not in the container.
  detector.RemoveAngle( PanGestureDetector::DIRECTION_UP );
  DALI_TEST_EQUALS( detector.GetAngleCount(), 2u, TEST_LOCATION );

  detector.RemoveAngle( PanGestureDetector::DIRECTION_RIGHT );
  DALI_TEST_EQUALS( detector.GetAngleCount(), 1u, TEST_LOCATION );
  for ( size_t i = 0; i < detector.GetAngleCount(); i++)
  {
    if ( detector.GetAngle(i).first == PanGestureDetector::DIRECTION_RIGHT )
    {
      tet_printf("%s, angle not removed\n", TEST_LOCATION );
      tet_result( TET_FAIL );
      break;
    }
  }

  detector.ClearAngles();
  DALI_TEST_EQUALS( detector.GetAngleCount(), 0u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliPanGestureGetAngle(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();
  DALI_TEST_EQUALS( detector.GetAngleCount(), 0, TEST_LOCATION );

  detector.AddAngle( PanGestureDetector::DIRECTION_LEFT );
  DALI_TEST_EQUALS( detector.GetAngleCount(), 1, TEST_LOCATION );

  detector.AddAngle( PanGestureDetector::DIRECTION_RIGHT );
  DALI_TEST_EQUALS( detector.GetAngleCount(), 2, TEST_LOCATION );

  detector.AddAngle( PanGestureDetector::DIRECTION_UP );
  DALI_TEST_EQUALS( detector.GetAngleCount(), 3, TEST_LOCATION );

  detector.AddAngle( PanGestureDetector::DIRECTION_DOWN );
  DALI_TEST_EQUALS( detector.GetAngleCount(), 4, TEST_LOCATION );

  DALI_TEST_EQUALS( detector.GetAngle(0).first,  PanGestureDetector::DIRECTION_LEFT, TEST_LOCATION );
  DALI_TEST_EQUALS( detector.GetAngle(1).first,  PanGestureDetector::DIRECTION_RIGHT, TEST_LOCATION );
  DALI_TEST_EQUALS( detector.GetAngle(2).first,  PanGestureDetector::DIRECTION_UP, TEST_LOCATION );
  DALI_TEST_EQUALS( detector.GetAngle(3).first,  PanGestureDetector::DIRECTION_DOWN, TEST_LOCATION );

  END_TEST;
}

inline float RadiansToDegrees( float radian )
{
  return radian * 180.0f / Math::PI;
}

int UtcDaliPanGestureAngleOutOfRange(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();
  DALI_TEST_EQUALS( detector.GetAngleCount(), 0u, TEST_LOCATION );

  //
  // Angle
  //

  detector.AddAngle( Degree(180.0f) );
  DALI_TEST_EQUALS( detector.GetAngle(0).first, Radian( Degree(-180.0f) ), 0.000001, TEST_LOCATION );
  detector.ClearAngles();

  detector.AddAngle( Degree(190.0f) );
  DALI_TEST_EQUALS( detector.GetAngle(0).first, Radian( Degree(-170.0f) ), 0.000001, TEST_LOCATION );
  detector.ClearAngles();

  detector.AddAngle( Degree(-190.0f) );
  DALI_TEST_EQUALS( detector.GetAngle(0).first, Radian( Degree(170.0f) ), 0.000001, TEST_LOCATION );
  detector.ClearAngles();

  detector.AddAngle( Degree(350.0f) );
  DALI_TEST_EQUALS( detector.GetAngle(0).first, Radian( Degree(-10.0f) ), 0.000001, TEST_LOCATION );
  detector.ClearAngles();

  detector.AddAngle( Degree(-350.0f) );
  DALI_TEST_EQUALS( detector.GetAngle(0).first, Radian( Degree(10.0f) ), 0.000001, TEST_LOCATION );
  detector.ClearAngles();

  detector.AddAngle( Degree(370.0f) );
  DALI_TEST_EQUALS( detector.GetAngle(0).first, Radian( Degree(10.0f) ), 0.000001, TEST_LOCATION );
  detector.ClearAngles();

  detector.AddAngle( Degree(-370.0f) );
  DALI_TEST_EQUALS( detector.GetAngle(0).first, Radian( Degree(-10.0f) ), 0.000001, TEST_LOCATION );
  detector.ClearAngles();

  //
  // Threshold
  //

  detector.AddAngle( PanGestureDetector::DIRECTION_RIGHT, Degree( 0.0f ) );
  DALI_TEST_EQUALS( detector.GetAngle(0).second, Radian( Degree(0.0f) ), 0.000001, TEST_LOCATION );
  detector.ClearAngles();

  detector.AddAngle( PanGestureDetector::DIRECTION_RIGHT, Degree( -10.0f ) );
  DALI_TEST_EQUALS( detector.GetAngle(0).second, Radian( Degree(10.0f) ), 0.000001, TEST_LOCATION );
  detector.ClearAngles();

  detector.AddAngle( PanGestureDetector::DIRECTION_RIGHT, Degree( -181.0f ) );
  DALI_TEST_EQUALS( detector.GetAngle(0).second, Radian( Degree(180.0f) ), 0.000001, TEST_LOCATION );
  detector.ClearAngles();

  detector.AddAngle( PanGestureDetector::DIRECTION_RIGHT, Degree( 181.0f ) );
  DALI_TEST_EQUALS( detector.GetAngle(0).second, Radian( Degree(180.0f) ), 0.000001, TEST_LOCATION );
  detector.ClearAngles();
  END_TEST;
}

int UtcDaliPanGestureAngleProcessing(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  parent.SetSize(100.0f, 100.0f);
  parent.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(parent);

  Actor child = Actor::New();
  child.SetSize(100.0f, 100.0f);
  child.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  parent.Add(child);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Parent detector only requires up pans
  PanGestureDetector parentDetector = PanGestureDetector::New();
  parentDetector.Attach( parent );
  parentDetector.AddAngle( PanGestureDetector::DIRECTION_UP, Degree( 30.0f ) );
  SignalData parentData;
  GestureReceivedFunctor parentFunctor(parentData);
  parentDetector.DetectedSignal().Connect(&application, parentFunctor);

  // Child detector only requires right pans
  PanGestureDetector childDetector = PanGestureDetector::New();
  childDetector.Attach( child );
  childDetector.AddAngle( PanGestureDetector::DIRECTION_RIGHT, Degree( 30.0f ) );
  SignalData childData;
  GestureReceivedFunctor childFunctor(childData);
  childDetector.DetectedSignal().Connect(&application, childFunctor);

  // Generate an Up pan gesture, only parent should receive it.
  uint32_t time = 100;
  TestStartPan( application, Vector2( 20.0f, 20.0f ),  Vector2( 20.0f, 4.0f ), time );

  DALI_TEST_EQUALS( true,  parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, childData.functorCalled,  TEST_LOCATION );

  TestEndPan( application, Vector2(20.0f, 4.0f), time );
  time += TestGetFrameInterval();
  parentData.Reset();
  childData.Reset();

  // Generate a Right pan gesture, only child should receive it.
  TestStartPan( application, Vector2( 20.0f, 20.0f ),  Vector2( 36.0f, 20.0f ), time );

  DALI_TEST_EQUALS( false, parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true,  childData.functorCalled,  TEST_LOCATION );

  TestEndPan( application, Vector2(4.0f, 20.0f), time );
  time += TestGetFrameInterval();
  parentData.Reset();
  childData.Reset();

  // Generate a Down pan gesture, no one should receive it.
  TestStartPan( application, Vector2( 20.0f, 20.0f ),  Vector2( 20.0f, 36.0f ), time );

  DALI_TEST_EQUALS( false, parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, childData.functorCalled,  TEST_LOCATION );

  TestEndPan( application, Vector2(20.0f, 36.0f), time );
  time += TestGetFrameInterval();
  parentData.Reset();
  childData.Reset();

  // Generate a Left pan gesture, no one should receive it.
  TestStartPan( application, Vector2( 20.0f, 20.0f ),  Vector2( 4.0f, 20.0f ), time );

  DALI_TEST_EQUALS( false, parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, childData.functorCalled,  TEST_LOCATION );

  TestEndPan( application, Vector2(4.0f, 20.0f), time );
  parentData.Reset();
  childData.Reset();
  END_TEST;
}

int UtcDaliPanGestureDirectionHandling(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();
  DALI_TEST_EQUALS( detector.GetAngleCount(), 0u, TEST_LOCATION );

  detector.AddDirection( PanGestureDetector::DIRECTION_LEFT, Radian( Math::PI * 0.25 ) );
  DALI_TEST_EQUALS( detector.GetAngleCount(), 2u, TEST_LOCATION );
  bool found = false;
  for ( size_t i = 0; detector.GetAngleCount(); i++)
  {
    if ( detector.GetAngle(i).first == PanGestureDetector::DIRECTION_LEFT )
    {
      tet_result( TET_PASS );
      found = true;
      break;
    }

  }

  if (!found )
  {
    tet_printf("%s, angle not added\n", TEST_LOCATION );
    tet_result( TET_FAIL );
  }

  found = false;
  for( size_t i = 0; i < detector.GetAngleCount(); i++)
  {
    if( detector.GetAngle(i).first == PanGestureDetector::DIRECTION_RIGHT )
    {
      tet_result( TET_PASS );
      found = true;
      break;
    }
  }

  if(!found )
  {
    tet_printf("%s, angle not added\n", TEST_LOCATION );
    tet_result( TET_FAIL );
  }

  // Remove something not in the container.
  detector.RemoveDirection( PanGestureDetector::DIRECTION_UP );
  DALI_TEST_EQUALS( detector.GetAngleCount(), 2u, TEST_LOCATION );

  detector.RemoveDirection( PanGestureDetector::DIRECTION_RIGHT );
  DALI_TEST_EQUALS( detector.GetAngleCount(), 0u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliPanGestureDirectionProcessing(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  parent.SetSize(100.0f, 100.0f);
  parent.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(parent);

  Actor child = Actor::New();
  child.SetSize(100.0f, 100.0f);
  child.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  parent.Add(child);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Parent detector only requires vertical panning
  PanGestureDetector parentDetector = PanGestureDetector::New();
  parentDetector.Attach( parent );
  parentDetector.AddDirection( PanGestureDetector::DIRECTION_VERTICAL, Degree( 30.0f ) );
  SignalData parentData;
  GestureReceivedFunctor parentFunctor(parentData);
  parentDetector.DetectedSignal().Connect(&application, parentFunctor);

  // Child detector only requires horizontal panning
  PanGestureDetector childDetector = PanGestureDetector::New();
  childDetector.Attach( child );
  childDetector.AddDirection( PanGestureDetector::DIRECTION_HORIZONTAL, Degree( 30.0f ) );
  SignalData childData;
  GestureReceivedFunctor childFunctor(childData);
  childDetector.DetectedSignal().Connect(&application, childFunctor);

  // Generate an Up pan gesture, only parent should receive it.
  uint32_t time = 100;
  TestStartPan( application, Vector2( 20.0f, 20.0f ),  Vector2( 20.0f, 4.0f ), time );

  DALI_TEST_EQUALS( true,  parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, childData.functorCalled,  TEST_LOCATION );

  TestEndPan( application, Vector2(20.0f, 20.0f), time );
  time += TestGetFrameInterval();
  parentData.Reset();
  childData.Reset();

  // Generate a Right pan gesture, only child should receive it.
  TestStartPan( application, Vector2( 20.0f, 20.0f ),  Vector2( 36.0f, 20.0f ), time );

  DALI_TEST_EQUALS( false, parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true,  childData.functorCalled,  TEST_LOCATION );

  TestEndPan( application, Vector2(20.0f, 20.0f), time );
  time += TestGetFrameInterval();
  parentData.Reset();
  childData.Reset();

  // Generate a Down pan gesture, only parent should receive it.
  TestStartPan( application, Vector2( 20.0f, 20.0f ),  Vector2( 20.0f, 36.0f ), time );

  DALI_TEST_EQUALS( true,  parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, childData.functorCalled,  TEST_LOCATION );

  TestEndPan( application, Vector2(20.0f, 20.0f), time );
  time += TestGetFrameInterval();
  parentData.Reset();
  childData.Reset();

  // Generate a Left pan gesture, only child should receive it.
  TestStartPan( application, Vector2( 20.0f, 20.0f ),  Vector2( 4.0f, 20.0f ), time );

  DALI_TEST_EQUALS( false, parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true,  childData.functorCalled,  TEST_LOCATION );

  TestEndPan( application, Vector2(20.0f, 20.0f), time );
  time += TestGetFrameInterval();
  parentData.Reset();
  childData.Reset();

  // Generate a pan at -45 degrees, no one should receive it.
  TestStartPan( application, Vector2( 20.0f, 20.0f ),  Vector2( 9.0f, 31.0f ), time );

  DALI_TEST_EQUALS( false, parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, childData.functorCalled,  TEST_LOCATION );

  TestEndPan( application, Vector2(20.0f, 20.0f), time );
  time += TestGetFrameInterval();
  parentData.Reset();
  childData.Reset();

  // Generate a pan at 45 degrees, no one should receive it.
  TestStartPan( application, Vector2( 20.0f, 20.0f ),  Vector2( 31.0f, 31.0f ), time );

  DALI_TEST_EQUALS( false, parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, childData.functorCalled,  TEST_LOCATION );

  TestEndPan( application, Vector2(20.0f, 20.0f), time );
  time += TestGetFrameInterval();
  parentData.Reset();
  childData.Reset();

  // Generate a pan at -135 degrees, no one should receive it.
  TestStartPan( application, Vector2( 20.0f, 20.0f ),  Vector2( 4.0f, 4.0f ), time );

  DALI_TEST_EQUALS( false, parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, childData.functorCalled,  TEST_LOCATION );

  TestEndPan( application, Vector2(20.0f, 20.0f), time );
  parentData.Reset();
  childData.Reset();
  END_TEST;
}

int UtcDaliPanGestureNoPredictionNoSmoothing(void)
{
  TestApplication application;
  Integration::SetPanGesturePredictionMode(0);
  Integration::SetPanGestureSmoothingMode(0);

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach( actor );
  SignalData data;
  GestureReceivedFunctor functor( data );
  detector.DetectedSignal().Connect( &application, functor );

  Property::Index property = actor.RegisterProperty( "Dummy Property", Vector3::ZERO );

  ConstraintData constraintData;
  Constraint constraint = Constraint::New<Vector3>( actor, property, PanConstraint( constraintData ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_POSITION ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_DISPLACEMENT ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_VELOCITY ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_POSITION ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_DISPLACEMENT ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_VELOCITY ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::PANNING ) );
  constraint.Apply();

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector2 startPosition( 1.0f, 1.0f );
  Vector2 position( -14.0f, 1.0f );
  Vector2 direction(Vector2::XAXIS * -5.0f);
  uint32_t time = 100;

  TestStartPan( application, startPosition, position, time );

  for(int i = 0; i < 47; i++ )
  {
    position += direction;
    TestMovePan( application, position, time );
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  TestEndPan( application, position, time );
  application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);

  DALI_TEST_EQUALS( true,  data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.called, true, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.screenPosition, startPosition + (direction * PAN_GESTURE_UPDATE_COUNT), 0.1f, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.localPosition,  startPosition + (direction * PAN_GESTURE_UPDATE_COUNT), 0.1f, TEST_LOCATION );

  constraintData.Reset();
  END_TEST;
}

int UtcDaliPanGestureNoPredictionSmoothing(void)
{
  TestApplication application;
  Integration::SetPanGesturePredictionMode(0);
  Integration::SetPanGestureSmoothingMode(1);

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach( actor );
  SignalData data;
  GestureReceivedFunctor functor( data );
  detector.DetectedSignal().Connect( &application, functor );

  Property::Index property = actor.RegisterProperty( "Dummy Property", Vector3::ZERO );

  ConstraintData constraintData;
  Constraint constraint = Constraint::New<Vector3>( actor, property, PanConstraint( constraintData ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_POSITION ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_DISPLACEMENT ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_VELOCITY ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_POSITION ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_DISPLACEMENT ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_VELOCITY ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::PANNING ) );
  constraint.Apply();

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector2 startPosition( 1.0f, 1.0f );
  Vector2 position( -14.0f, 1.0f );
  Vector2 direction(Vector2::XAXIS * -5.0f);
  uint32_t time = 100;

  TestStartPan( application, startPosition, position, time );

  for(int i = 0; i < 47; i++ )
  {
    position += direction;
    TestMovePan( application, position, time );
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  TestEndPan( application, position, time );
  application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);

  DALI_TEST_EQUALS( true,  data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.called, true, TEST_LOCATION );
  // Take into account resampling done when prediction is off.
  DALI_TEST_EQUALS( constraintData.screenPosition, startPosition + (direction * (PAN_GESTURE_UPDATE_COUNT - 0.25f)), 0.15f, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.localPosition,  startPosition + (direction * (PAN_GESTURE_UPDATE_COUNT - 0.25f)), 0.15f, TEST_LOCATION );

  constraintData.Reset();
  END_TEST;
}

int UtcDaliPanGesturePredictionNoSmoothing(void)
{
  TestApplication application;
  Integration::SetPanGesturePredictionMode(1);
  Integration::SetPanGestureSmoothingMode(0);

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach( actor );
  SignalData data;
  GestureReceivedFunctor functor( data );
  detector.DetectedSignal().Connect( &application, functor );

  Property::Index property = actor.RegisterProperty( "Dummy Property", Vector3::ZERO );

  ConstraintData constraintData;
  Constraint constraint = Constraint::New<Vector3>( actor, property, PanConstraint( constraintData ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_POSITION ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_DISPLACEMENT ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_VELOCITY ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_POSITION ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_DISPLACEMENT ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_VELOCITY ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::PANNING ) );
  constraint.Apply();

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector2 startPosition( 1.0f, 1.0f );
  Vector2 position( -1.0f, 1.0f );
  Vector2 direction(Vector2::XAXIS * -1.0f);
  uint32_t time = 100;

  TestStartPan( application, startPosition, position, time );

  for(int i = 0; i < 47; i++ )
  {
    position += direction;
    TestMovePan( application, position, time );
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  TestEndPan( application, position, time );
  application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);

  DALI_TEST_EQUALS( true,  data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.called, true, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.screenPosition, Vector2(1.0f, 1.0f) + (direction * PAN_GESTURE_UPDATE_COUNT), 10.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.localPosition,  Vector2(1.0f, 1.0f) + (direction * PAN_GESTURE_UPDATE_COUNT), 10.0f, TEST_LOCATION );

  constraintData.Reset();
  END_TEST;
}

int UtcDaliPanGesturePredictionSmoothing01(void)
{
  TestApplication application;
  Integration::SetPanGesturePredictionMode(1);
  Integration::SetPanGestureSmoothingMode(1);

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach( actor );
  SignalData data;
  GestureReceivedFunctor functor( data );
  detector.DetectedSignal().Connect( &application, functor );

  Property::Index property = actor.RegisterProperty( "Dummy Property", Vector3::ZERO );

  ConstraintData constraintData;
  Constraint constraint = Constraint::New<Vector3>( actor, property, PanConstraint( constraintData ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_POSITION ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_DISPLACEMENT ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_VELOCITY ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_POSITION ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_DISPLACEMENT ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_VELOCITY ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::PANNING ) );
  constraint.Apply();

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector2 startPosition( 1.0f, 1.0f );
  Vector2 position( -1.0f, 1.0f );
  Vector2 direction(Vector2::XAXIS * -1.0f);
  uint32_t time = 100;

  TestStartPan( application, startPosition, position, time );

  for(int i = 0; i < 47; i++ )
  {
    position += direction;
    TestMovePan( application, position, time );
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  TestEndPan( application, position, time );
  application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);

  DALI_TEST_EQUALS( true,  data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.called, true, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.screenPosition, Vector2(1.0f, 1.0f) + (direction * PAN_GESTURE_UPDATE_COUNT), 10.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.localPosition,  Vector2(1.0f, 1.0f) + (direction * PAN_GESTURE_UPDATE_COUNT), 10.0f, TEST_LOCATION );

  constraintData.Reset();
  END_TEST;
}

int UtcDaliPanGesturePredictionSmoothing02(void)
{
  TestApplication application;
  Integration::SetPanGesturePredictionMode( 1 );
  Integration::SetPanGestureMaximumPredictionAmount( 1 );
  Integration::SetPanGesturePredictionAmountAdjustment( 2 );
  Integration::SetPanGestureSmoothingMode( 1 );
  Integration::SetPanGestureSmoothingAmount( 0.25f );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach( actor );
  SignalData data;
  GestureReceivedFunctor functor( data );
  detector.DetectedSignal().Connect( &application, functor );

  Property::Index property = actor.RegisterProperty( "Dummy Property", Vector3::ZERO );

  ConstraintData constraintData;
  Constraint constraint = Constraint::New<Vector3>( actor, property, PanConstraint( constraintData ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_POSITION ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_DISPLACEMENT ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_VELOCITY ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_POSITION ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_DISPLACEMENT ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_VELOCITY ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::PANNING ) );
  constraint.Apply();

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector2 startPosition( 2.0f, 2.0f );
  Vector2 position( 4.0f, 2.0f );
  Vector2 directionX(Vector2::XAXIS);
  Vector2 directionY(Vector2::YAXIS);
  uint32_t time = 100;

  TestStartPan( application, startPosition, position, time );

  for(int i = 0; i < 7; i++ )
  {
    position += directionX;
    TestMovePan( application, position, time );
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  position += directionX * 10.0f;
  TestMovePan( application, position, time );
  time += TestGetFrameInterval();
  application.SendNotification();
  application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);

  for(int i = 0; i < 2; i++ )
  {
    position += ( directionX * -1.0f );
    TestMovePan( application, position, time );
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  for(int i = 0; i < 10; i++ )
  {
    position += directionX;
    TestMovePan( application, position, time );
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  for(int i = 0; i < 10; i++ )
  {
    position += directionY;
    TestMovePan( application, position, time );
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  TestEndPan( application, position, time );
  application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);

  DALI_TEST_EQUALS( true,  data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.called, true, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.screenPosition, Vector2(2.0f, 2.0f) + position, 10.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.localPosition,  Vector2(2.0f, 2.0f) + position, 10.0f, TEST_LOCATION );

  constraintData.Reset();
  END_TEST;
}

int UtcDaliPanGesturePrediction2SmoothingMultiTap01(void)
{
  TestApplication application;

  Integration::SetPanGesturePredictionMode( 2 );
  Integration::SetPanGesturePredictionAmount( 57 );
  Integration::SetPanGestureSmoothingMode( 2 );
  Integration::SetPanGestureUseActualTimes( false );
  Integration::SetPanGestureInterpolationTimeRange( 10 );
  Integration::SetPanGestureScalarOnlyPredictionEnabled( false );
  Integration::SetPanGestureTwoPointPredictionEnabled( true );
  Integration::SetPanGestureTwoPointInterpolatePastTime( 42 );
  Integration::SetPanGestureTwoPointVelocityBias( 0.35f );
  Integration::SetPanGestureTwoPointAccelerationBias( 0.10f );
  Integration::SetPanGestureMultitapSmoothingRange( 34 );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach( actor );
  SignalData data;
  GestureReceivedFunctor functor( data );
  detector.DetectedSignal().Connect( &application, functor );

  Property::Index property = actor.RegisterProperty( "Dummy Property", Vector3::ZERO );

  ConstraintData constraintData;
  Constraint constraint = Constraint::New<Vector3>( actor, property, PanConstraint( constraintData ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_POSITION ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_DISPLACEMENT ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_VELOCITY ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_POSITION ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_DISPLACEMENT ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_VELOCITY ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::PANNING ) );
  constraint.Apply();

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector2 startPosition( 2.0f, 2.0f );
  Vector2 position( -1.0f, 2.0f );
  Vector2 direction(Vector2::XAXIS * -1.0f);
  uint32_t time = 100;

  TestStartPan( application, startPosition, position, time );

  for(int i = 0; i < 27; i++ )
  {
    position += direction;
    TestMovePan( application, position, time );
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  TestEndPan( application, position, time );
  application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);

  DALI_TEST_EQUALS( true,  data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.called, true, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.screenPosition, Vector2(2.0f, 2.0f) + position, 10.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.localPosition,  Vector2(2.0f, 2.0f) + position, 10.0f, TEST_LOCATION );

  constraintData.Reset();
  END_TEST;
}

int UtcDaliPanGesturePrediction2SmoothingMultiTap02(void)
{
  TestApplication application;

  Integration::SetPanGesturePredictionMode( 2 );
  Integration::SetPanGestureSmoothingMode( 2 );
  Integration::SetPanGestureUseActualTimes( true );
  Integration::SetPanGestureInterpolationTimeRange( 10 );
  Integration::SetPanGestureScalarOnlyPredictionEnabled( true );
  Integration::SetPanGestureTwoPointPredictionEnabled( true );
  Integration::SetPanGestureTwoPointInterpolatePastTime( 42 );
  Integration::SetPanGestureTwoPointVelocityBias( 0.35f );
  Integration::SetPanGestureTwoPointAccelerationBias( 0.10f );
  Integration::SetPanGestureMultitapSmoothingRange( 34 );

  Integration::EnableProfiling( Integration::PROFILING_TYPE_PAN_GESTURE );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach( actor );
  SignalData data;
  GestureReceivedFunctor functor( data );
  detector.DetectedSignal().Connect( &application, functor );

  Property::Index property = actor.RegisterProperty( "Dummy Property", Vector3::ZERO );

  ConstraintData constraintData;
  Constraint constraint = Constraint::New<Vector3>( actor, property, PanConstraint( constraintData ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_POSITION ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_DISPLACEMENT ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_VELOCITY ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_POSITION ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_DISPLACEMENT ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_VELOCITY ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::PANNING ) );
  constraint.Apply();

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector2 startPosition( 2.0f, 2.0f );
  Vector2 position( 17.0f, 2.0f );
  Vector2 direction(Vector2::XAXIS * -1.0f);
  uint32_t time = 100;

  TestStartPan( application, startPosition, position, time );

  for(int i = 0; i < 10; i++ )
  {
    position += direction;
    TestMovePan( application, position, time );
    time += TestGetFrameInterval();

    position += direction;
    TestMovePan( application, position, time );
    time += TestGetFrameInterval();

    position += direction;
    TestMovePan( application, position, time );
    time += TestGetFrameInterval();

    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  for(int i = 0; i < 10; i++ )
  {
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  for(int i = 0; i < 10; i++ )
  {
    position += direction;
    TestMovePan( application, position, time );
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  for(int i = 0; i < 10; i++ )
  {
    position += direction;
    TestMovePan( application, position, time );
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  TestEndPan( application, position, time );
  application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);

  DALI_TEST_EQUALS( true,  data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.called, true, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.screenPosition, Vector2(2.0f, 2.0f) + position, 10.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.localPosition,  Vector2(2.0f, 2.0f) + position, 10.0f, TEST_LOCATION );

  constraintData.Reset();
  END_TEST;
}

int UtcDaliPanGesturePrediction2Smoothing(void)
{
  TestApplication application;

  Integration::SetPanGesturePredictionMode( 2 );
  Integration::SetPanGesturePredictionAmount( 57 );
  Integration::SetPanGestureSmoothingMode( 1 );
  Integration::SetPanGestureUseActualTimes( false );
  Integration::SetPanGestureInterpolationTimeRange( 10 );
  Integration::SetPanGestureScalarOnlyPredictionEnabled( true );
  Integration::SetPanGestureTwoPointPredictionEnabled( true );
  Integration::SetPanGestureTwoPointInterpolatePastTime( 42 );
  Integration::SetPanGestureTwoPointVelocityBias( 0.35f );
  Integration::SetPanGestureTwoPointAccelerationBias( 0.10f );
  Integration::SetPanGestureMultitapSmoothingRange( 34 );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach( actor );
  SignalData data;
  GestureReceivedFunctor functor( data );
  detector.DetectedSignal().Connect( &application, functor );

  Property::Index property = actor.RegisterProperty( "Dummy Property", Vector3::ZERO );

  ConstraintData constraintData;
  Constraint constraint = Constraint::New<Vector3>( actor, property, PanConstraint( constraintData ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_POSITION ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_DISPLACEMENT ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_VELOCITY ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_POSITION ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_DISPLACEMENT ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_VELOCITY ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::PANNING ) );
  constraint.Apply();

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector2 startPosition( 2.0f, 2.0f );
  Vector2 position( 17.0f, 2.0f );
  Vector2 direction(Vector2::XAXIS * -1.0f);
  uint32_t time = 100;

  TestStartPan( application, startPosition, position, time );

  for(int i = 0; i < 10; i++ )
  {
    position += direction;
    TestMovePan( application, position, time );
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }


  position += direction;
  TestMovePan( application, position, time );
  time += TestGetFrameInterval();

  position += direction;
  TestMovePan( application, position, time );
  time += TestGetFrameInterval();

  position += direction;
  TestMovePan( application, position, time );
  time += TestGetFrameInterval();

  application.SendNotification();
  application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);

  for(int i = 0; i < 5; i++ )
  {
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  for(int i = 0; i < 10; i++ )
  {
    position += direction;
    TestMovePan( application, position, time );
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  TestEndPan( application, position, time );
  application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);

  DALI_TEST_EQUALS( true,  data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.called, true, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.screenPosition, Vector2(2.0f, 2.0f) + position, 10.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.localPosition,  Vector2(2.0f, 2.0f) + position, 10.0f, TEST_LOCATION );

  constraintData.Reset();
  END_TEST;
}

int UtcDaliPanGestureSetProperties(void)
{
  TestApplication application;
  TestRenderController& renderController( application.GetRenderController() );
  Integration::SetPanGesturePredictionMode(0);
  Integration::SetPanGestureSmoothingMode(0);

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach( actor );
  SignalData data;
  GestureReceivedFunctor functor( data );
  detector.DetectedSignal().Connect( &application, functor );

  Property::Index property = actor.RegisterProperty( "Dummy Property", Vector3::ZERO );
  Property::Index animatableGestureProperty = detector.RegisterProperty( "Dummy Property", Vector3::ZERO ); // For code coverage

  ConstraintData constraintData;
  Constraint constraint = Constraint::New<Vector3>( actor, property, PanConstraint( constraintData ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_POSITION ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_DISPLACEMENT ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_VELOCITY ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_POSITION ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_DISPLACEMENT ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_VELOCITY ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::PANNING ) );
  constraint.AddSource( Source( detector, animatableGestureProperty ) );
  constraint.Apply();

  // Render and notify
  application.SendNotification();
  application.Render();

  renderController.Initialize();
  DALI_TEST_EQUALS( renderController.WasCalled( TestRenderController::RequestUpdateFunc ), false, TEST_LOCATION );

  Vector2 screenPosition( 20.0f, 20.0f );
  Vector2 screenDisplacement( 1.0f, 1.0f );
  Vector2 screenVelocity( 1.3f, 4.0f );
  Vector2 localPosition( 21.0f, 21.0f );
  Vector2 localDisplacement( 0.5f, 0.5f );
  Vector2 localVelocity( 1.5f, 2.5f );

  PanGestureDetector::SetPanGestureProperties( GeneratePan( 1u, Gesture::Started, screenPosition, localPosition, screenDisplacement, localDisplacement, screenVelocity, localVelocity ) );
  DALI_TEST_EQUALS( renderController.WasCalled( TestRenderController::RequestUpdateFunc ), true, TEST_LOCATION );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( constraintData.called, true, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.screenPosition,     screenPosition,     TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.localPosition,      localPosition,      TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.screenDisplacement, screenDisplacement, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.localDisplacement,  localDisplacement,  TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.screenVelocity,     screenVelocity,     TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.localVelocity,      localVelocity,      TEST_LOCATION );
  constraintData.Reset();
  END_TEST;
}

int UtcDaliPanGestureSetPropertiesAlreadyPanning(void)
{
  TestApplication application;
  Integration::SetPanGesturePredictionMode(0);

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach( actor );
  SignalData data;
  GestureReceivedFunctor functor( data );
  detector.DetectedSignal().Connect( &application, functor );

  Property::Index property = actor.RegisterProperty( "Dummy Property", Vector3::ZERO );

  ConstraintData constraintData;
  Constraint constraint = Constraint::New<Vector3>( actor, property, PanConstraint( constraintData ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_POSITION ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_DISPLACEMENT ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::SCREEN_VELOCITY ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_POSITION ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_DISPLACEMENT ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::LOCAL_VELOCITY ) );
  constraint.AddSource( Source( detector, PanGestureDetector::Property::PANNING ) );
  constraint.Apply();

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector2 currentPosition( 20.0f, 4.0f );
  uint32_t time = 100;
  TestStartPan( application, Vector2( 20.0f, 20.0f ),  currentPosition, time );
  DALI_TEST_EQUALS( true,  data.functorCalled, TEST_LOCATION );

  Vector2 screenPosition( 100.0f, 20.0f );
  Vector2 localPosition( 110.0f, 110.0f );

  PanGestureDetector::SetPanGestureProperties( GeneratePan( 1u, Gesture::Started, screenPosition, localPosition ) );

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS( constraintData.called, true, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.screenPosition, currentPosition, 0.1, TEST_LOCATION );
  DALI_TEST_EQUALS( constraintData.localPosition,  currentPosition, 0.1f, TEST_LOCATION );
  constraintData.Reset();
  END_TEST;
}

int UtcDaliPanGesturePropertyIndices(void)
{
  TestApplication application;
  PanGestureDetector detector = PanGestureDetector::New();

  Property::IndexContainer indices;
  detector.GetPropertyIndices( indices );
  DALI_TEST_CHECK( indices.Size() );
  DALI_TEST_EQUALS( indices.Size(), detector.GetPropertyCount(), TEST_LOCATION );
  END_TEST;
}

namespace
{
struct PropertyStringIndex
{
  const char * const name;
  const Property::Index index;
  const Property::Type type;
  const Property::Value value;
};

const PropertyStringIndex PROPERTY_TABLE[] =
{
  { "screenPosition",      PanGestureDetector::Property::SCREEN_POSITION,     Property::VECTOR2, Vector2::ZERO },
  { "screenDisplacement",  PanGestureDetector::Property::SCREEN_DISPLACEMENT, Property::VECTOR2, Vector2::ZERO },
  { "screenVelocity",      PanGestureDetector::Property::SCREEN_VELOCITY,     Property::VECTOR2, Vector2::ZERO },
  { "localPosition",       PanGestureDetector::Property::LOCAL_POSITION,      Property::VECTOR2, Vector2::ZERO },
  { "localDisplacement",   PanGestureDetector::Property::LOCAL_DISPLACEMENT,  Property::VECTOR2, Vector2::ZERO },
  { "localVelocity",       PanGestureDetector::Property::LOCAL_VELOCITY,      Property::VECTOR2, Vector2::ZERO },
  { "panning",             PanGestureDetector::Property::PANNING,             Property::BOOLEAN, false         },
};
const unsigned int PROPERTY_TABLE_COUNT = sizeof( PROPERTY_TABLE ) / sizeof( PROPERTY_TABLE[0] );
} // unnamed namespace


int UtcDaliPanGestureProperties(void)
{
  TestApplication application;
  PanGestureDetector detector = PanGestureDetector::New();

  for( unsigned int i = 0; i < PROPERTY_TABLE_COUNT; ++i )
  {
    DALI_TEST_EQUALS( detector.GetPropertyName( PROPERTY_TABLE[ i ].index ), std::string( PROPERTY_TABLE[ i ].name ), TEST_LOCATION );
    DALI_TEST_EQUALS( detector.GetPropertyIndex( PROPERTY_TABLE[ i ].name ), PROPERTY_TABLE[ i ].index, TEST_LOCATION );
    DALI_TEST_EQUALS( detector.GetPropertyType( PROPERTY_TABLE[ i ].index ), PROPERTY_TABLE[ i ].type, TEST_LOCATION );
    DALI_TEST_EQUALS( detector.IsPropertyWritable( PROPERTY_TABLE[ i ].index ), false, TEST_LOCATION );
    DALI_TEST_EQUALS( detector.IsPropertyAnimatable( PROPERTY_TABLE[ i ].index ), false, TEST_LOCATION );
    DALI_TEST_EQUALS( detector.IsPropertyAConstraintInput( PROPERTY_TABLE[ i ].index ), true, TEST_LOCATION );
    detector.SetProperty( PROPERTY_TABLE[ i ].index, Property::Value() ); // Just for Coverage
  }

  END_TEST;
}

int UtcDaliPanGestureGetProperty(void)
{
  TestApplication application;
  PanGestureDetector detector = PanGestureDetector::New();

  for( unsigned int i = 0; i < PROPERTY_TABLE_COUNT; ++i )
  {
    if( PROPERTY_TABLE[ i ].type == Property::VECTOR2 )
    {
      bool value = detector.GetProperty( PROPERTY_TABLE[ i ].index ).Get< bool >();
      DALI_TEST_EQUALS( PROPERTY_TABLE[ i ].value.Get< bool >(), value, TEST_LOCATION );
    }
    else if( PROPERTY_TABLE[ i ].type == Property::BOOLEAN )
    {
      Vector2 value = detector.GetProperty( PROPERTY_TABLE[ i ].index ).Get< Vector2 >();
      DALI_TEST_EQUALS( PROPERTY_TABLE[ i ].value.Get< Vector2 >(), value, TEST_LOCATION );
    }
  }

  END_TEST;
}

int UtcDaliPanGestureGetPropertyWithSceneObject(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach( actor );

  // Render and notify
  application.SendNotification();
  application.Render();

  for( unsigned int i = 0; i < PROPERTY_TABLE_COUNT; ++i )
  {
    detector.SetProperty( PROPERTY_TABLE[ i ].index, Property::Value() ); // Just for Coverage

    if( PROPERTY_TABLE[ i ].type == Property::VECTOR2 )
    {
      bool value = detector.GetProperty( PROPERTY_TABLE[ i ].index ).Get< bool >();
      DALI_TEST_EQUALS( PROPERTY_TABLE[ i ].value.Get< bool >(), value, TEST_LOCATION );
    }
    else if( PROPERTY_TABLE[ i ].type == Property::BOOLEAN )
    {
      Vector2 value = detector.GetProperty( PROPERTY_TABLE[ i ].index ).Get< Vector2 >();
      DALI_TEST_EQUALS( PROPERTY_TABLE[ i ].value.Get< Vector2 >(), value, TEST_LOCATION );
    }
  }

  END_TEST;
}

int UtcDaliPanGestureLayerConsumesTouch(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach( actor );
  SignalData data;
  GestureReceivedFunctor functor( data );
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
  uint32_t time = 100;
  TestStartPan( application, Vector2( 10.0f, 20.0f ),  Vector2( 26.0f, 20.0f ), time );
  TestEndPan( application, Vector2(26.0f, 20.0f), time );
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Set layer to consume all touch
  layer.SetTouchConsumed( true );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit the same signals again, should not receive
  TestStartPan( application, Vector2( 10.0f, 20.0f ),  Vector2( 26.0f, 20.0f ), time );
  TestEndPan( application, Vector2(26.0f, 20.0f), time );

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  END_TEST;
}

int UtcDaliPanGestureNoTimeDiff(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach( actor );
  SignalData data;
  GestureReceivedFunctor functor( data );
  detector.DetectedSignal().Connect( &application, functor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // As normal helper function adds intervals between presses we must generate the sequence
  // using other helper functions
  TestStartLongPress( application, 10.0f, 20.0f, 100 );   // Used to send a down press event
  TestMovePan( application, Vector2(26.0f, 20.0f), 100 );
  TestMovePan( application, Vector2(26.0f, 20.0f), 100 ); // 2 motions required to trigger
  TestEndPan( application, Vector2(26.0f, 20.0f), 100 );

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK( !std::isinf( data.receivedGesture.velocity.x ) );
  DALI_TEST_CHECK( !std::isinf( data.receivedGesture.velocity.y ) );
  DALI_TEST_CHECK( !std::isinf( data.receivedGesture.screenVelocity.x ) );
  DALI_TEST_CHECK( !std::isinf( data.receivedGesture.screenVelocity.y ) );
  data.Reset();

  data.Reset();

  END_TEST;
}
