/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/events/pan-gesture-devel.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/touch-integ.h>
#include <dali/integration-api/input-options.h>
#include <dali/integration-api/profiling.h>
#include <dali/integration-api/render-task-list-integ.h>
#include <dali/internal/event/events/touch-event-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>
#include <test-touch-event-utils.h>
#include <limits>
#include <utility>
#include <vector>

#include <cmath>
#include <iostream>

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
const int PAN_GESTURE_UPDATE_COUNT = 50;

// Stores data that is populated in the callback and will be read by the test cases
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
    functorCalled          = false;
    voidFunctorCalled      = false;
    needGesturePropagation = false;
    receivedGesture.Reset();

    pannedActor.Reset();
  }

  bool       functorCalled;
  bool       voidFunctorCalled;
  bool       needGesturePropagation;
  PanGesture receivedGesture;
  Actor      pannedActor;
};

// Functor that sets the data when called
struct GestureReceivedFunctor
{
  GestureReceivedFunctor(SignalData& data)
  : signalData(data)
  {
  }

  void operator()(Actor actor, PanGesture pan)
  {
    signalData.functorCalled   = true;
    signalData.receivedGesture = pan;
    signalData.pannedActor     = actor;
    if(signalData.needGesturePropagation)
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
  UnstageActorFunctor(SignalData& data, GestureState& stateToUnstage, Dali::Integration::Scene scene)
  : GestureReceivedFunctor(data),
    stateToUnstage(stateToUnstage),
    scene(scene)
  {
  }

  void operator()(Actor actor, PanGesture pan)
  {
    GestureReceivedFunctor::operator()(actor, pan);

    if(pan.GetState() == stateToUnstage)
    {
      scene.Remove(actor);
    }
  }

  GestureState&            stateToUnstage;
  Dali::Integration::Scene scene;
};

// Functor that removes the gestured actor from stage
struct PropagationActorFunctor : public GestureReceivedFunctor
{
  PropagationActorFunctor(SignalData& data, bool propagation)
  : GestureReceivedFunctor(data),
    propagation(propagation)
  {
  }

  void operator()(Actor actor, PanGesture pan)
  {
    GestureReceivedFunctor::operator()(actor, pan);
    Dali::DevelActor::SetNeedGesturePropagation(actor, propagation);
  }

  bool propagation;
};

// Data for constraints
struct ConstraintData
{
  ConstraintData()
  : panning(false),
    called(false)
  {
  }

  Vector2 screenPosition;
  Vector2 screenDisplacement;
  Vector2 screenVelocity;
  Vector2 localPosition;
  Vector2 localDisplacement;
  Vector2 localVelocity;
  bool    panning;
  bool    called;

  void Reset()
  {
    screenPosition = screenDisplacement = screenVelocity = localPosition = localDisplacement = localVelocity = Vector2::ZERO;
    panning                                                                                                  = false;
    called                                                                                                   = false;
  }
};

// Constraint used with panning properties
struct PanConstraint
{
  PanConstraint(ConstraintData& data)
  : constraintData(data)
  {
  }

  void operator()(Vector3& current, const PropertyInputContainer& inputs)
  {
    constraintData.screenPosition     = inputs[0]->GetVector2();
    constraintData.screenDisplacement = inputs[1]->GetVector2();
    constraintData.screenVelocity     = inputs[2]->GetVector2();
    constraintData.localPosition      = inputs[3]->GetVector2();
    constraintData.localDisplacement  = inputs[4]->GetVector2();
    constraintData.localVelocity      = inputs[5]->GetVector2();
    constraintData.panning            = inputs[6]->GetBoolean();
    constraintData.called             = true;
    current                           = Vector3::ZERO;
  }

  ConstraintData& constraintData;
};

// Generate a PanGesture
PanGesture GeneratePan(unsigned int time,
                       GestureState state,
                       Vector2      screenPosition,
                       Vector2      localPosition,
                       Vector2      screenDisplacement = Vector2::ONE,
                       Vector2      localDisplacement  = Vector2::ONE,
                       Vector2      screenVelocity     = Vector2::ONE,
                       Vector2      localVelocity      = Vector2::ONE,
                       unsigned int numberOfTouches    = 1)
{
  Dali::PanGesture pan = DevelPanGesture::New(state);

  DevelPanGesture::SetTime(pan, time);

  DevelPanGesture::SetScreenPosition(pan, screenPosition);
  DevelPanGesture::SetPosition(pan, localPosition);

  DevelPanGesture::SetScreenDisplacement(pan, screenDisplacement);
  DevelPanGesture::SetDisplacement(pan, localDisplacement);

  DevelPanGesture::SetScreenVelocity(pan, screenVelocity);
  DevelPanGesture::SetVelocity(pan, localVelocity);

  DevelPanGesture::SetNumberOfTouches(pan, numberOfTouches);

  return pan;
}

Dali::Integration::TouchEvent GenerateSingleTouch(PointState::Type state, const Vector2& screenPosition, uint32_t time)
{
  Dali::Integration::TouchEvent touchEvent;
  Dali::Integration::Point      point;
  point.SetState(state);
  point.SetDeviceId(4);
  point.SetScreenPosition(screenPosition);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);
  touchEvent.time = time;
  return touchEvent;
}
} // namespace

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

  PanGestureDetector copy(detector);
  DALI_TEST_CHECK(detector);
  END_TEST;
}

int UtcDaliPanGestureDetectorAssignmentOperatorP(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  PanGestureDetector assign;
  assign = detector;
  DALI_TEST_CHECK(detector);

  DALI_TEST_CHECK(detector == assign);
  END_TEST;
}

int UtcDaliPanGestureDetectorMoveConstructorP(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();
  DALI_TEST_CHECK(detector);

  PanGestureDetector moved = std::move(detector);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!detector);
  END_TEST;
}

int UtcDaliPanGestureDetectorMoveAssignmentOperatorP(void)
{
  TestApplication application;

  PanGestureDetector detector;
  detector = PanGestureDetector::New();
  DALI_TEST_CHECK(detector);

  PanGestureDetector moved;
  moved = std::move(detector);
  DALI_TEST_CHECK(moved);
  DALI_TEST_CHECK(!detector);
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
  DALI_TEST_EQUALS(std::numeric_limits<uint32_t>::max(), detector.GetMaximumMotionEventAge(), TEST_LOCATION);

  // Attach an actor and emit a touch event on the actor to ensure complete line coverage
  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  detector.Attach(actor);

  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Use long press function for touch event
  TestStartLongPress(application);

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

  PanGestureDetector detector3 = DownCast<PanGestureDetector>(object);
  DALI_TEST_CHECK(detector3);

  BaseHandle         unInitializedObject;
  PanGestureDetector detector4 = PanGestureDetector::DownCast(unInitializedObject);
  DALI_TEST_CHECK(!detector4);

  PanGestureDetector detector5 = DownCast<PanGestureDetector>(unInitializedObject);
  DALI_TEST_CHECK(!detector5);

  GestureDetector    detector6 = PanGestureDetector::New();
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
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
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
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  detector.SetMaximumTouchesRequired(4);

  DALI_TEST_EQUALS(4, detector.GetMaximumTouchesRequired(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureSetMaximumMotionEventAge(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  uint32_t minTime = 20;

  DALI_TEST_CHECK(minTime != detector.GetMaximumMotionEventAge());

  detector.SetMaximumMotionEventAge(minTime);

  DALI_TEST_EQUALS(minTime, detector.GetMaximumMotionEventAge(), TEST_LOCATION);

  // Attach an actor and change the maximum touches

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  detector.SetMaximumMotionEventAge(minTime * 2);

  DALI_TEST_EQUALS(minTime * 2, detector.GetMaximumMotionEventAge(), TEST_LOCATION);

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

int UtcDaliPanGestureGetMaximumMotionEventAge(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();
  DALI_TEST_EQUALS(std::numeric_limits<uint32_t>::max(), detector.GetMaximumMotionEventAge(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSignalReceptionNegative(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Do a pan outside actor's area
  uint32_t time = 100;
  TestStartPan(application, Vector2(110.0f, 110.0f), Vector2(121.0f, 121.0f), time);

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Continue pan into actor's area - we should still not receive the signal
  data.Reset();
  TestMovePan(application, Vector2(20.0f, 20.0f), time);

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Stop panning - we should still not receive the signal
  data.Reset();
  TestEndPan(application, Vector2(12.0f, 12.0f), time);

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSignalReceptionDownMotionLeave(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pan within the actor's area
  uint32_t time = 100;
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::STARTED, data.receivedGesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(16.0f, 0.0f), data.receivedGesture.GetDisplacement(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(0.5f, 0.0f), data.receivedGesture.GetVelocity(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(16.0f, data.receivedGesture.GetDistance(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(0.5f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);

  // Continue the pan within the actor's area - we should still receive the signal
  data.Reset();

  TestMovePan(application, Vector2(26.0f, 4.0f), time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::CONTINUING, data.receivedGesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(0.0f, -16.0f), data.receivedGesture.GetDisplacement(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(0.0f, -1.0f), data.receivedGesture.GetVelocity(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(16.0f, data.receivedGesture.GetDistance(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(1.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);

  // Pan Gesture leaves actor's area - we should still receive the signal
  data.Reset();

  TestMovePan(application, Vector2(346.0f, 4.0f), time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::CONTINUING, data.receivedGesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(320.0f, 0.0f), data.receivedGesture.GetDisplacement(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(20.0f, 0.0f), data.receivedGesture.GetVelocity(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(320.0f, data.receivedGesture.GetDistance(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(20.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);

  // Gesture ends - we would receive a finished state
  data.Reset();

  TestEndPan(application, Vector2(314.0f, 4.0f), time);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::FINISHED, data.receivedGesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(-32.0f, 0.0f), data.receivedGesture.GetDisplacement(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(-2.0f, 0.0f), data.receivedGesture.GetVelocity(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(32.0f, data.receivedGesture.GetDistance(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(2.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSignalReceptionDownMotionUp(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pan within the actor's area
  uint32_t time = 100;
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::STARTED, data.receivedGesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(16.0f, 0.0f), data.receivedGesture.GetDisplacement(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(0.5f, 0.0f), data.receivedGesture.GetVelocity(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(16.0f, data.receivedGesture.GetDistance(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(0.5f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);

  // Continue the pan within the actor's area - we should still receive the signal
  data.Reset();

  TestMovePan(application, Vector2(26.0f, 4.0f), time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::CONTINUING, data.receivedGesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(0.0f, -16.0f), data.receivedGesture.GetDisplacement(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(0.0f, -1.0f), data.receivedGesture.GetVelocity(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(16.0f, data.receivedGesture.GetDistance(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(1.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);

  // Gesture ends within actor's area - we would receive a finished state
  data.Reset();

  TestEndPan(application, Vector2(10.0f, 4.0f), time);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::FINISHED, data.receivedGesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, data.receivedGesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(-16.0f, 0.0f), data.receivedGesture.GetDisplacement(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(-1.0f, 0.0f), data.receivedGesture.GetVelocity(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(16.0f, data.receivedGesture.GetDistance(), 0.01f, TEST_LOCATION);
  DALI_TEST_EQUALS(1.0f, data.receivedGesture.GetSpeed(), 0.01f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSignalReceptionDetach(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pan within the actor's area
  uint32_t time = 100;
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Continue the pan within the actor's area - we should still receive the signal
  data.Reset();

  TestMovePan(application, Vector2(26.0f, 4.0f), time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Gesture ends within actor's area
  data.Reset();

  TestEndPan(application, Vector2(10.0f, 4.0f), time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Detach actor
  detector.DetachAll();

  // Ensure we are no longer signalled
  data.Reset();

  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);
  TestMovePan(application, Vector2(26.0f, 4.0f), time);
  time += TestGetFrameInterval();
  TestEndPan(application, Vector2(10.0f, 4.0f), time);

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSignalReceptionDetachWhilePanning(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pan within the actor's area
  uint32_t time = 100;
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);
  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Continue the pan within the actor's area - we should still receive the signal
  data.Reset();

  TestMovePan(application, Vector2(26.0f, 4.0f), time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Detach actor during the pan, we should not receive the next event
  detector.DetachAll();

  // Gesture ends within actor's area
  data.Reset();

  TestEndPan(application, Vector2(10.0f, 4.0f), time);

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSignalReceptionActorDestroyedWhilePanning(void)
{
  TestApplication application;

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.DetectedSignal().Connect(&application, functor);

  // Attach a temporary actor to stop detector being removed from PanGestureProcessor when main actor
  // is destroyed.
  Actor tempActor = Actor::New();
  tempActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  tempActor.SetProperty(Actor::Property::PIVOT, Pivot::BOTTOM_RIGHT);
  application.GetScene().Add(tempActor);
  detector.Attach(tempActor);

  uint32_t time = 100;

  // Actor lifetime is scoped
  {
    Actor actor = Actor::New();
    actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
    actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
    application.GetScene().Add(actor);

    // Render and notify
    application.SendNotification();
    application.Render();

    detector.Attach(actor);

    // Start pan within the actor's area
    TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);

    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

    // Continue the pan within the actor's area - we should still receive the signal
    data.Reset();

    TestMovePan(application, Vector2(26.0f, 4.0f), time);
    time += TestGetFrameInterval();

    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

    // Remove the actor from stage and reset the data
    application.GetScene().Remove(actor);

    // Render and notify
    application.SendNotification();
    application.Render();
  }

  // Actor should now have been destroyed

  // Gesture ends within the area where the actor used to be
  data.Reset();

  TestEndPan(application, Vector2(10.0f, 4.0f), time);

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSignalReceptionActorBecomesInvisible(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pan within the actor's area
  uint32_t time = 100;
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);
  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::STARTED, data.receivedGesture.GetState(), TEST_LOCATION);

  // Continue the pan within the actor's area
  data.Reset();
  TestMovePan(application, Vector2(26.0f, 4.0f), time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::CONTINUING, data.receivedGesture.GetState(), TEST_LOCATION);

  // Make the actor invisible, this should emit CANCELLED gesture during the next pan event
  data.Reset();
  actor.SetProperty(Actor::Property::VISIBLE, false);
  application.SendNotification();
  application.Render();

  // Next move should trigger the gesture state as CANCELLED, and no more gestures
  TestMovePan(application, Vector2(10.0f, 4.0f), time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::CANCELLED, data.receivedGesture.GetState(), TEST_LOCATION);

  // Try again to finish the pan - we should not receive it since it was cancelled
  data.Reset();
  TestEndPan(application, Vector2(5.0f, 4.0f), time);

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSignalReceptionRotatedActor(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ORIENTATION, Quaternion(Dali::Degree(90.0f), Vector3::ZAXIS));
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Do an entire pan, only check finished value
  uint32_t time = 100;
  TestStartPan(application, Vector2(11.0f, 12.0f), Vector2(27.0f, 12.0f), time);

  data.Reset();

  TestEndPan(application, Vector2(25.0f, 28.0f), time);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(16.0f, 2.0f), data.receivedGesture.GetDisplacement(), 0.01f, TEST_LOCATION); // Actor relative

  // Rotate actor again and render a couple of times
  actor.SetProperty(Actor::Property::ORIENTATION, Quaternion(Dali::Degree(180.0f), Vector3::ZAXIS));
  application.SendNotification();
  application.Render();

  // Do an entire pan, only check finished value
  TestStartPan(application, Vector2(11.0f, 12.0f), Vector2(27.0f, 12.0f), time);

  data.Reset();

  TestEndPan(application, Vector2(25.0f, 28.0f), time);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(2.0f, -16.0f), data.receivedGesture.GetDisplacement(), 0.01f, TEST_LOCATION); // Actor relative

  // Rotate actor again and render a couple of times
  actor.SetProperty(Actor::Property::ORIENTATION, Quaternion(Dali::Degree(270.0f), Vector3::ZAXIS));
  application.SendNotification();
  application.Render();

  // Do an entire pan, only check finished value
  TestStartPan(application, Vector2(11.0f, 12.0f), Vector2(27.0f, 12.0f), time);

  data.Reset();

  TestEndPan(application, Vector2(25.0f, 28.0f), time);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(-16.0f, -2.0f), data.receivedGesture.GetDisplacement(), 0.01f, TEST_LOCATION); // Actor relative
  END_TEST;
}

int UtcDaliPanGestureSignalReceptionChildHit(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  parent.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  parent.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(parent);

  // Set child to completely cover parent.
  // Change rotation of child to be different from parent so that we can check if our local coordinate
  // conversion of the parent actor is correct.
  Actor child = Actor::New();
  child.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  child.SetProperty(Actor::Property::PIVOT, Pivot::CENTER);
  child.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  child.SetProperty(Actor::Property::ORIENTATION, Quaternion(Dali::Degree(90.0f), Vector3::ZAXIS));
  parent.Add(child);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(parent);
  detector.DetectedSignal().Connect(&application, functor);

  // Do an entire pan, only check finished value - hits child area but parent should still receive it
  uint32_t time = 100;
  TestStartPan(application, Vector2(11.0f, 12.0f), Vector2(27.0f, 12.0f), time);

  data.Reset();

  TestEndPan(application, Vector2(25.0f, 28.0f), time);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, parent == data.pannedActor, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(-2.0f, 16.0f), data.receivedGesture.GetDisplacement(), 0.01f, TEST_LOCATION); // Actor relative

  // Attach child and generate same touch points to yield a different displacement
  // (Also proves that you can detach and then re-attach another actor)
  detector.Attach(child);
  detector.Detach(parent);

  // Do an entire pan, only check finished value
  TestStartPan(application, Vector2(11.0f, 12.0f), Vector2(27.0f, 12.0f), time);

  data.Reset();

  TestEndPan(application, Vector2(25.0f, 28.0f), time);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, child == data.pannedActor, TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(16.0f, 2.0f), data.receivedGesture.GetDisplacement(), 0.01f, TEST_LOCATION); // Actor relative
  END_TEST;
}

int UtcDaliPanGestureSignalReceptionAttachDetachMany(void)
{
  TestApplication application;

  Actor first = Actor::New();
  first.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  first.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(first);

  Actor second = Actor::New();
  second.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  second.SetProperty(Actor::Property::POSITION_X, 100.0f);
  second.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(second);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(first);
  detector.Attach(second);
  detector.DetectedSignal().Connect(&application, functor);

  DALI_TEST_EQUALS(application.GetScene().GetRootLayer(), first.GetParent(), TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetScene().GetRootLayer(), second.GetParent(), TEST_LOCATION);

  // Start pan within second actor's area
  uint32_t time = 100;
  TestStartPan(application, Vector2(110.0f, 20.0f), Vector2(126.0f, 20.0f), time);

  DALI_TEST_EQUALS(application.GetScene().GetRootLayer(), first.GetParent(), TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetScene().GetRootLayer(), second.GetParent(), TEST_LOCATION);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, second == data.pannedActor, TEST_LOCATION);

  // Pan moves into first actor's area - second actor should receive the pan
  data.Reset();

  TestMovePan(application, Vector2(126.0f, 20.0f), time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, second == data.pannedActor, TEST_LOCATION);

  // Detach the second actor during the pan, we should not receive the next event
  detector.Detach(second);

  // Gesture ends within actor's area
  data.Reset();

  TestMovePan(application, Vector2(26.0f, 20.0f), time);
  TestEndPan(application, Vector2(10.0f, 4.0f), time);

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureSignalReceptionActorBecomesUntouchable(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pan in actor's area
  uint32_t time = 100;
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Pan continues within actor's area
  data.Reset();

  TestMovePan(application, Vector2(26.0f, 4.0f), time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Actor become invisible - actor should not receive the next pan
  actor.SetProperty(Actor::Property::VISIBLE, false);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Gesture ends within actor's area
  data.Reset();

  TestEndPan(application, Vector2(10.0f, 4.0f), time);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::CANCELLED, data.receivedGesture.GetState(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSignalReceptionMultipleDetectorsOnActor(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  Actor actor2 = Actor::New();
  actor2.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor2.SetProperty(Actor::Property::PIVOT, Pivot::BOTTOM_RIGHT);
  application.GetScene().Add(actor2);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Attach actor to one detector
  SignalData             firstData;
  GestureReceivedFunctor firstFunctor(firstData);
  PanGestureDetector     firstDetector = PanGestureDetector::New();
  firstDetector.Attach(actor);
  firstDetector.DetectedSignal().Connect(&application, firstFunctor);

  // Attach actor to another detector
  SignalData             secondData;
  GestureReceivedFunctor secondFunctor(secondData);
  PanGestureDetector     secondDetector = PanGestureDetector::New();
  secondDetector.Attach(actor);
  secondDetector.DetectedSignal().Connect(&application, secondFunctor);

  // Add second actor to second detector, when we remove the actor, this will make sure that this
  // gesture detector is not removed from the GestureDetectorProcessor.  In this scenario, the
  // functor should still not be called (which is what we're also testing).
  secondDetector.Attach(actor2);

  // Pan in actor's area - both detector's functors should be called
  uint32_t time = 100;
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);

  DALI_TEST_EQUALS(true, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // Pan continues in actor's area - both detector's functors should be called
  firstData.Reset();
  secondData.Reset();

  TestMovePan(application, Vector2(10.0f, 20.0f), time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // Detach actor from firstDetector and emit pan on actor, only secondDetector's functor should be called.
  firstDetector.Detach(actor);
  firstData.Reset();
  secondData.Reset();

  TestEndPan(application, Vector2(10.0f, 4.0f), time);

  DALI_TEST_EQUALS(false, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // New pan on actor, only secondDetector has actor attached
  firstData.Reset();
  secondData.Reset();

  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);

  DALI_TEST_EQUALS(false, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, secondData.functorCalled, TEST_LOCATION);

  // Detach actor from secondDetector
  secondDetector.Detach(actor);
  firstData.Reset();
  secondData.Reset();

  TestMovePan(application, Vector2(10.0f, 20.0f), time);

  DALI_TEST_EQUALS(false, firstData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, secondData.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureSignalReceptionEnsureCorrectSignalling(void)
{
  TestApplication application;

  Actor actor1 = Actor::New();
  actor1.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor1.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor1);
  SignalData             data1;
  GestureReceivedFunctor functor1(data1);
  PanGestureDetector     detector1 = PanGestureDetector::New();
  detector1.Attach(actor1);
  detector1.DetectedSignal().Connect(&application, functor1);

  Actor actor2 = Actor::New();
  actor2.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor2.SetProperty(Actor::Property::PIVOT, Pivot::BOTTOM_RIGHT);
  actor2.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_RIGHT);
  application.GetScene().Add(actor2);
  SignalData             data2;
  GestureReceivedFunctor functor2(data2);
  PanGestureDetector     detector2 = PanGestureDetector::New();
  detector2.Attach(actor2);
  detector2.DetectedSignal().Connect(&application, functor2);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Start pan in actor1's area, only data1 should be set
  uint32_t time = 100;
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);

  DALI_TEST_EQUALS(true, data1.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, data2.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSignalReceptionAttachActorAfterDown(void)
{
  // This test checks to ensure a pan is possible after attaching an actor after a down (possible) event

  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Gesture possible in actor's area (using long-press)
  uint32_t time = 100;
  TestStartLongPress(application, 10.0f, 20.0f, time);
  time += TestGetFrameInterval();

  // Attach actor to detector
  SignalData             data;
  GestureReceivedFunctor functor(data);
  PanGestureDetector     detector = PanGestureDetector::New();
  detector.DetectedSignal().Connect(&application, functor);
  detector.Attach(actor);

  // Start a pan, initially it'll only be possible, we shouldn't receive it
  TestMovePan(application, Vector2(10.0f, 20.0f), time);
  time += TestGetFrameInterval();
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Now the pan truly starts, we should receive a signal
  TestMovePan(application, Vector2(26.0f, 20.0f), time);
  time += TestGetFrameInterval();
  TestMovePan(application, Vector2(32.0f, 32.0f), time);
  time += TestGetFrameInterval();
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Finish the pan, we should still receive a signal
  data.Reset();
  TestEndPan(application, Vector2(32.0f, 32.0f), time);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureSignalReceptionAttachActorAfterDownAfterInitialPanToAnotherActor(void)
{
  // This test checks to ensure a pan is possible after attaching an actor after a down (possible) event even if another
  // pan actor was there before (parent)

  TestApplication application;

  Actor parent = Actor::New();
  parent.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  parent.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(parent);

  Actor child = Actor::New();
  child.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  child.SetProperty(Actor::Property::PIVOT, Pivot::CENTER);
  child.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  parent.Add(child);

  // Create detector for parent and attach
  SignalData             parentData;
  GestureReceivedFunctor parentFunctor(parentData);
  PanGestureDetector     parentDetector = PanGestureDetector::New();
  parentDetector.DetectedSignal().Connect(&application, parentFunctor);
  parentDetector.Attach(parent);

  // Create detector for child but do not attach
  SignalData             childData;
  GestureReceivedFunctor childFunctor(childData);
  PanGestureDetector     childDetector = PanGestureDetector::New();
  childDetector.DetectedSignal().Connect(&application, childFunctor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Do a full pan in both actors' area, only the parent's functor should be called
  uint32_t time = 100;
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);
  DALI_TEST_EQUALS(parentData.functorCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(childData.functorCalled, false, TEST_LOCATION);
  parentData.Reset();
  childData.Reset();
  TestEndPan(application, Vector2(26.0f, 20.0f), time);
  DALI_TEST_EQUALS(parentData.functorCalled, true, TEST_LOCATION);
  DALI_TEST_EQUALS(childData.functorCalled, false, TEST_LOCATION);
  parentData.Reset();
  childData.Reset();

  // Gesture possible in both actors' area (using long-press), no functors called
  TestStartLongPress(application, 10.0f, 20.0f, time);
  time += TestGetFrameInterval();
  DALI_TEST_EQUALS(parentData.functorCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(childData.functorCalled, false, TEST_LOCATION);

  // Attach the child as well now
  childDetector.Attach(child);

  // Now the pan truly starts, we should receive a signal for the child only
  TestMovePan(application, Vector2(26.0f, 20.0f), time);
  time += TestGetFrameInterval();
  TestMovePan(application, Vector2(32.0f, 32.0f), time);
  time += TestGetFrameInterval();
  DALI_TEST_EQUALS(parentData.functorCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(childData.functorCalled, true, TEST_LOCATION);
  parentData.Reset();
  childData.Reset();

  // Finish the pan, again only the child should still receive a signal
  TestEndPan(application, Vector2(32.0f, 32.0f), time);
  DALI_TEST_EQUALS(parentData.functorCalled, false, TEST_LOCATION);
  DALI_TEST_EQUALS(childData.functorCalled, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureSignalReceptionDifferentPossible(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Attach actor to detector
  SignalData             data;
  GestureReceivedFunctor functor(data);
  PanGestureDetector     detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Gesture possible in actor's area.
  uint32_t time = 100;
  TestStartLongPress(application, 10.0f, 20.0f, time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Move actor somewhere else
  actor.SetProperty(Actor::Property::POSITION, Vector2(100.0f, 100.0f));

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit STARTED event, we should not receive the pan.
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);
  TestEndPan(application, Vector2(26.0f, 20.0f), time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // LONG_PRESS possible in empty area.
  TestStartLongPress(application, 10.0f, 20.0f, time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Move actor in to the long press position.
  actor.SetProperty(Actor::Property::POSITION, Vector2(0.0f, 0.0f));

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit STARTED event, we should be receiving the pan now.
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);
  TestEndPan(application, Vector2(26.0f, 20.0f), time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Normal pan in actor's area for completeness.
  data.Reset();
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);
  TestEndPan(application, Vector2(26.0f, 20.0f), time);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureActorUnstaged(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // State to remove actor in.
  GestureState stateToUnstage(GestureState::STARTED);

  // Attach actor to detector
  SignalData          data;
  UnstageActorFunctor functor(data, stateToUnstage, application.GetScene());
  PanGestureDetector  detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Emit signals
  uint32_t time = 100;
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  TestEndPan(application, Vector2(26.0f, 20.0f), time);

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
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  TestMovePan(application, Vector2(26.0f, 4.0f), time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  data.Reset();

  TestEndPan(application, Vector2(10.0f, 4.0f), time);

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

  // Change state to GestureState::FINISHED to remove
  stateToUnstage = GestureState::FINISHED;

  // Emit signals
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  TestMovePan(application, Vector2(26.0f, 4.0f), time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  TestEndPan(application, Vector2(10.0f, 4.0f), time);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  tet_result(TET_PASS); // If we get here then we have handled actor stage removal gracefully.
  END_TEST;
}

int UtcDaliPanGestureActorStagedAndDestroyed(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Create and add a second actor so that GestureDetector destruction does not come into play.
  Actor dummyActor(Actor::New());
  dummyActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  dummyActor.SetProperty(Actor::Property::POSITION, Vector2(100.0f, 100.0f));
  dummyActor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(dummyActor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // State to remove actor in.
  GestureState stateToUnstage(GestureState::STARTED);

  // Attach actor to detector
  SignalData          data;
  UnstageActorFunctor functor(data, stateToUnstage, application.GetScene());
  PanGestureDetector  detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.Attach(dummyActor);
  detector.DetectedSignal().Connect(&application, functor);

  // Here we are testing a STARTED actor which is removed in the STARTED callback, but then added back
  // before we get a continuing state.  As we were removed from the stage, even if we're at the same
  // position, we should still not be signalled.

  // Emit signals
  uint32_t time = 100;
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);

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
  TestMovePan(application, Vector2(26.0f, 4.0f), time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  TestEndPan(application, Vector2(10.0f, 4.0f), time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Here we delete an actor in started, we should not receive any subsequent signalling.

  // Emit signals
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);

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
  TestMovePan(application, Vector2(26.0f, 4.0f), time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  TestEndPan(application, Vector2(10.0f, 4.0f), time);

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureAngleHandling(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();
  DALI_TEST_EQUALS(detector.GetAngleCount(), 0u, TEST_LOCATION);

  detector.AddAngle(PanGestureDetector::DIRECTION_LEFT, Radian(Math::PI * 0.25));
  DALI_TEST_EQUALS(detector.GetAngleCount(), 1u, TEST_LOCATION);
  bool found = false;
  for(uint32_t i = 0; i < detector.GetAngleCount(); i++)
  {
    if(detector.GetAngle(i).first == PanGestureDetector::DIRECTION_LEFT)
    {
      tet_result(TET_PASS);
      found = true;
      break;
    }
  }

  if(!found)
  {
    tet_printf("%s, angle not added\n", TEST_LOCATION);
    tet_result(TET_FAIL);
  }

  detector.AddAngle(PanGestureDetector::DIRECTION_RIGHT, Radian(Math::PI * 0.25));
  DALI_TEST_EQUALS(detector.GetAngleCount(), 2u, TEST_LOCATION);

  // Remove something not in the container.
  detector.RemoveAngle(PanGestureDetector::DIRECTION_UP);
  DALI_TEST_EQUALS(detector.GetAngleCount(), 2u, TEST_LOCATION);

  detector.RemoveAngle(PanGestureDetector::DIRECTION_RIGHT);
  DALI_TEST_EQUALS(detector.GetAngleCount(), 1u, TEST_LOCATION);
  for(uint32_t i = 0; i < detector.GetAngleCount(); i++)
  {
    if(detector.GetAngle(i).first == PanGestureDetector::DIRECTION_RIGHT)
    {
      tet_printf("%s, angle not removed\n", TEST_LOCATION);
      tet_result(TET_FAIL);
      break;
    }
  }

  detector.ClearAngles();
  DALI_TEST_EQUALS(detector.GetAngleCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureGetAngle(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();
  DALI_TEST_EQUALS(detector.GetAngleCount(), 0, TEST_LOCATION);

  detector.AddAngle(PanGestureDetector::DIRECTION_LEFT);
  DALI_TEST_EQUALS(detector.GetAngleCount(), 1, TEST_LOCATION);

  detector.AddAngle(PanGestureDetector::DIRECTION_RIGHT);
  DALI_TEST_EQUALS(detector.GetAngleCount(), 2, TEST_LOCATION);

  detector.AddAngle(PanGestureDetector::DIRECTION_UP);
  DALI_TEST_EQUALS(detector.GetAngleCount(), 3, TEST_LOCATION);

  detector.AddAngle(PanGestureDetector::DIRECTION_DOWN);
  DALI_TEST_EQUALS(detector.GetAngleCount(), 4, TEST_LOCATION);

  DALI_TEST_EQUALS(detector.GetAngle(0).first, PanGestureDetector::DIRECTION_LEFT, TEST_LOCATION);
  DALI_TEST_EQUALS(detector.GetAngle(1).first, PanGestureDetector::DIRECTION_RIGHT, TEST_LOCATION);
  DALI_TEST_EQUALS(detector.GetAngle(2).first, PanGestureDetector::DIRECTION_UP, TEST_LOCATION);
  DALI_TEST_EQUALS(detector.GetAngle(3).first, PanGestureDetector::DIRECTION_DOWN, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureAngleOutOfRange(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();
  DALI_TEST_EQUALS(detector.GetAngleCount(), 0u, TEST_LOCATION);

  //
  // Angle
  //

  detector.AddAngle(Degree(180.0f));
  DALI_TEST_EQUALS(detector.GetAngle(0).first, Radian(Degree(-180.0f)), 0.000001f, TEST_LOCATION);
  detector.ClearAngles();

  detector.AddAngle(Degree(190.0f));
  DALI_TEST_EQUALS(detector.GetAngle(0).first, Radian(Degree(-170.0f)), 0.000001f, TEST_LOCATION);
  detector.ClearAngles();

  detector.AddAngle(Degree(-190.0f));
  DALI_TEST_EQUALS(detector.GetAngle(0).first, Radian(Degree(170.0f)), 0.000001f, TEST_LOCATION);
  detector.ClearAngles();

  detector.AddAngle(Degree(350.0f));
  DALI_TEST_EQUALS(detector.GetAngle(0).first, Radian(Degree(-10.0f)), 0.000001f, TEST_LOCATION);
  detector.ClearAngles();

  detector.AddAngle(Degree(-350.0f));
  DALI_TEST_EQUALS(detector.GetAngle(0).first, Radian(Degree(10.0f)), 0.000001f, TEST_LOCATION);
  detector.ClearAngles();

  detector.AddAngle(Degree(370.0f));
  DALI_TEST_EQUALS(detector.GetAngle(0).first, Radian(Degree(10.0f)), 0.000001f, TEST_LOCATION);
  detector.ClearAngles();

  detector.AddAngle(Degree(-370.0f));
  DALI_TEST_EQUALS(detector.GetAngle(0).first, Radian(Degree(-10.0f)), 0.000001f, TEST_LOCATION);
  detector.ClearAngles();

  //
  // Threshold
  //

  detector.AddAngle(PanGestureDetector::DIRECTION_RIGHT, Degree(0.0f));
  DALI_TEST_EQUALS(detector.GetAngle(0).second, Radian(Degree(0.0f)), 0.000001f, TEST_LOCATION);
  detector.ClearAngles();

  detector.AddAngle(PanGestureDetector::DIRECTION_RIGHT, Degree(-10.0f));
  DALI_TEST_EQUALS(detector.GetAngle(0).second, Radian(Degree(10.0f)), 0.000001f, TEST_LOCATION);
  detector.ClearAngles();

  detector.AddAngle(PanGestureDetector::DIRECTION_RIGHT, Degree(-181.0f));
  DALI_TEST_EQUALS(detector.GetAngle(0).second, Radian(Degree(180.0f)), 0.000001f, TEST_LOCATION);
  detector.ClearAngles();

  detector.AddAngle(PanGestureDetector::DIRECTION_RIGHT, Degree(181.0f));
  DALI_TEST_EQUALS(detector.GetAngle(0).second, Radian(Degree(180.0f)), 0.000001f, TEST_LOCATION);
  detector.ClearAngles();
  END_TEST;
}

int UtcDaliPanGestureAngleProcessing(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  parent.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  parent.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(parent);

  Actor child = Actor::New();
  child.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  child.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  parent.Add(child);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Parent detector only requires up pans
  PanGestureDetector parentDetector = PanGestureDetector::New();
  parentDetector.Attach(parent);
  parentDetector.AddAngle(PanGestureDetector::DIRECTION_UP, Degree(30.0f));
  SignalData             parentData;
  GestureReceivedFunctor parentFunctor(parentData);
  parentDetector.DetectedSignal().Connect(&application, parentFunctor);

  // Child detector only requires right pans
  PanGestureDetector childDetector = PanGestureDetector::New();
  childDetector.Attach(child);
  childDetector.AddAngle(PanGestureDetector::DIRECTION_RIGHT, Degree(30.0f));
  SignalData             childData;
  GestureReceivedFunctor childFunctor(childData);
  childDetector.DetectedSignal().Connect(&application, childFunctor);

  // Generate an Up pan gesture, only parent should receive it.
  uint32_t time = 100;
  TestStartPan(application, Vector2(20.0f, 20.0f), Vector2(20.0f, 4.0f), time);

  DALI_TEST_EQUALS(true, parentData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, childData.functorCalled, TEST_LOCATION);

  TestEndPan(application, Vector2(20.0f, 4.0f), time);
  time += TestGetFrameInterval();
  parentData.Reset();
  childData.Reset();

  // Generate a Right pan gesture, only child should receive it.
  TestStartPan(application, Vector2(20.0f, 20.0f), Vector2(36.0f, 20.0f), time);

  DALI_TEST_EQUALS(false, parentData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, childData.functorCalled, TEST_LOCATION);

  TestEndPan(application, Vector2(4.0f, 20.0f), time);
  time += TestGetFrameInterval();
  parentData.Reset();
  childData.Reset();

  // Generate a Down pan gesture, no one should receive it.
  TestStartPan(application, Vector2(20.0f, 20.0f), Vector2(20.0f, 36.0f), time);

  DALI_TEST_EQUALS(false, parentData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, childData.functorCalled, TEST_LOCATION);

  TestEndPan(application, Vector2(20.0f, 36.0f), time);
  time += TestGetFrameInterval();
  parentData.Reset();
  childData.Reset();

  // Generate a Left pan gesture, no one should receive it.
  TestStartPan(application, Vector2(20.0f, 20.0f), Vector2(4.0f, 20.0f), time);

  DALI_TEST_EQUALS(false, parentData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, childData.functorCalled, TEST_LOCATION);

  TestEndPan(application, Vector2(4.0f, 20.0f), time);
  parentData.Reset();
  childData.Reset();
  END_TEST;
}

int UtcDaliPanGestureDirectionHandling(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();
  DALI_TEST_EQUALS(detector.GetAngleCount(), 0u, TEST_LOCATION);

  detector.AddDirection(PanGestureDetector::DIRECTION_LEFT, Radian(Math::PI * 0.25));
  DALI_TEST_EQUALS(detector.GetAngleCount(), 2u, TEST_LOCATION);
  bool found = false;
  for(uint32_t i = 0; i < detector.GetAngleCount(); i++)
  {
    if(detector.GetAngle(i).first == PanGestureDetector::DIRECTION_LEFT)
    {
      tet_result(TET_PASS);
      found = true;
      break;
    }
  }

  if(!found)
  {
    tet_printf("%s, angle not added\n", TEST_LOCATION);
    tet_result(TET_FAIL);
  }

  found = false;
  for(uint32_t i = 0; i < detector.GetAngleCount(); i++)
  {
    if(detector.GetAngle(i).first == PanGestureDetector::DIRECTION_RIGHT)
    {
      tet_result(TET_PASS);
      found = true;
      break;
    }
  }

  if(!found)
  {
    tet_printf("%s, angle not added\n", TEST_LOCATION);
    tet_result(TET_FAIL);
  }

  // Remove something not in the container.
  detector.RemoveDirection(PanGestureDetector::DIRECTION_UP);
  DALI_TEST_EQUALS(detector.GetAngleCount(), 2u, TEST_LOCATION);

  detector.RemoveDirection(PanGestureDetector::DIRECTION_RIGHT);
  DALI_TEST_EQUALS(detector.GetAngleCount(), 0u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureDirectionProcessing(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  parent.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  parent.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(parent);

  Actor child = Actor::New();
  child.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  child.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  parent.Add(child);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Parent detector only requires vertical panning
  PanGestureDetector parentDetector = PanGestureDetector::New();
  parentDetector.Attach(parent);
  parentDetector.AddDirection(PanGestureDetector::DIRECTION_VERTICAL, Degree(30.0f));
  SignalData             parentData;
  GestureReceivedFunctor parentFunctor(parentData);
  parentDetector.DetectedSignal().Connect(&application, parentFunctor);

  // Child detector only requires horizontal panning
  PanGestureDetector childDetector = PanGestureDetector::New();
  childDetector.Attach(child);
  childDetector.AddDirection(PanGestureDetector::DIRECTION_HORIZONTAL, Degree(30.0f));
  SignalData             childData;
  GestureReceivedFunctor childFunctor(childData);
  childDetector.DetectedSignal().Connect(&application, childFunctor);

  // Generate an Up pan gesture, only parent should receive it.
  uint32_t time = 100;
  TestStartPan(application, Vector2(20.0f, 20.0f), Vector2(20.0f, 4.0f), time);

  DALI_TEST_EQUALS(true, parentData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, childData.functorCalled, TEST_LOCATION);

  TestEndPan(application, Vector2(20.0f, 20.0f), time);
  time += TestGetFrameInterval();
  parentData.Reset();
  childData.Reset();

  // Generate a Right pan gesture, only child should receive it.
  TestStartPan(application, Vector2(20.0f, 20.0f), Vector2(36.0f, 20.0f), time);

  DALI_TEST_EQUALS(false, parentData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, childData.functorCalled, TEST_LOCATION);

  TestEndPan(application, Vector2(20.0f, 20.0f), time);
  time += TestGetFrameInterval();
  parentData.Reset();
  childData.Reset();

  // Generate a Down pan gesture, only parent should receive it.
  TestStartPan(application, Vector2(20.0f, 20.0f), Vector2(20.0f, 36.0f), time);

  DALI_TEST_EQUALS(true, parentData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, childData.functorCalled, TEST_LOCATION);

  TestEndPan(application, Vector2(20.0f, 20.0f), time);
  time += TestGetFrameInterval();
  parentData.Reset();
  childData.Reset();

  // Generate a Left pan gesture, only child should receive it.
  TestStartPan(application, Vector2(20.0f, 20.0f), Vector2(4.0f, 20.0f), time);

  DALI_TEST_EQUALS(false, parentData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, childData.functorCalled, TEST_LOCATION);

  TestEndPan(application, Vector2(20.0f, 20.0f), time);
  time += TestGetFrameInterval();
  parentData.Reset();
  childData.Reset();

  // Generate a pan at -45 degrees, no one should receive it.
  TestStartPan(application, Vector2(20.0f, 20.0f), Vector2(9.0f, 31.0f), time);

  DALI_TEST_EQUALS(false, parentData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, childData.functorCalled, TEST_LOCATION);

  TestEndPan(application, Vector2(20.0f, 20.0f), time);
  time += TestGetFrameInterval();
  parentData.Reset();
  childData.Reset();

  // Generate a pan at 45 degrees, no one should receive it.
  TestStartPan(application, Vector2(20.0f, 20.0f), Vector2(31.0f, 31.0f), time);

  DALI_TEST_EQUALS(false, parentData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, childData.functorCalled, TEST_LOCATION);

  TestEndPan(application, Vector2(20.0f, 20.0f), time);
  time += TestGetFrameInterval();
  parentData.Reset();
  childData.Reset();

  // Generate a pan at -135 degrees, no one should receive it.
  TestStartPan(application, Vector2(20.0f, 20.0f), Vector2(4.0f, 4.0f), time);

  DALI_TEST_EQUALS(false, parentData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, childData.functorCalled, TEST_LOCATION);

  TestEndPan(application, Vector2(20.0f, 20.0f), time);
  parentData.Reset();
  childData.Reset();
  END_TEST;
}

int UtcDaliPanGestureNoPredictionNoSmoothing(void)
{
  TestApplication application;
  Dali::Integration::SetPanGesturePredictionMode(0);
  Dali::Integration::SetPanGestureSmoothingMode(0);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  SignalData             data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  Property::Index property = actor.RegisterProperty("Dummy Property", Vector3::ZERO);

  ConstraintData constraintData;
  Constraint     constraint = Constraint::New<Vector3>(actor, property, PanConstraint(constraintData));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_POSITION));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_DISPLACEMENT));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_VELOCITY));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_POSITION));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_DISPLACEMENT));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_VELOCITY));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::PANNING));
  constraint.Apply();

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector2  startPosition(1.0f, 1.0f);
  Vector2  position(-14.0f, 1.0f);
  Vector2  direction(Vector2::XAXIS * -5.0f);
  uint32_t time = 100;

  TestStartPan(application, startPosition, position, time);

  for(int i = 0; i < 47; i++)
  {
    position += direction;
    TestMovePan(application, position, time);
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  TestEndPan(application, position, time);
  application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.called, true, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.screenPosition, startPosition + (direction * PAN_GESTURE_UPDATE_COUNT), 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.localPosition, startPosition + (direction * PAN_GESTURE_UPDATE_COUNT), 0.1f, TEST_LOCATION);

  constraintData.Reset();
  END_TEST;
}

int UtcDaliPanGestureNoPredictionSmoothing(void)
{
  TestApplication application;
  Dali::Integration::SetPanGesturePredictionMode(0);
  Dali::Integration::SetPanGestureSmoothingMode(1);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  SignalData             data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  Property::Index property = actor.RegisterProperty("Dummy Property", Vector3::ZERO);

  ConstraintData constraintData;
  Constraint     constraint = Constraint::New<Vector3>(actor, property, PanConstraint(constraintData));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_POSITION));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_DISPLACEMENT));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_VELOCITY));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_POSITION));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_DISPLACEMENT));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_VELOCITY));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::PANNING));
  constraint.Apply();

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector2  startPosition(1.0f, 1.0f);
  Vector2  position(-14.0f, 1.0f);
  Vector2  direction(Vector2::XAXIS * -5.0f);
  uint32_t time = 100;

  TestStartPan(application, startPosition, position, time);

  for(int i = 0; i < 47; i++)
  {
    position += direction;
    TestMovePan(application, position, time);
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  TestEndPan(application, position, time);
  application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.called, true, TEST_LOCATION);
  // Take into account resampling done when prediction is off.
  DALI_TEST_EQUALS(constraintData.screenPosition, startPosition + (direction * (PAN_GESTURE_UPDATE_COUNT - 0.25f)), 0.15f, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.localPosition, startPosition + (direction * (PAN_GESTURE_UPDATE_COUNT - 0.25f)), 0.15f, TEST_LOCATION);

  constraintData.Reset();
  END_TEST;
}

int UtcDaliPanGesturePredictionNoSmoothing(void)
{
  TestApplication application;
  Dali::Integration::SetPanGesturePredictionMode(1);
  Dali::Integration::SetPanGestureSmoothingMode(0);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  SignalData             data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  Property::Index property = actor.RegisterProperty("Dummy Property", Vector3::ZERO);

  ConstraintData constraintData;
  Constraint     constraint = Constraint::New<Vector3>(actor, property, PanConstraint(constraintData));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_POSITION));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_DISPLACEMENT));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_VELOCITY));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_POSITION));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_DISPLACEMENT));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_VELOCITY));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::PANNING));
  constraint.Apply();

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector2  startPosition(1.0f, 1.0f);
  Vector2  position(-1.0f, 1.0f);
  Vector2  direction(Vector2::XAXIS * -1.0f);
  uint32_t time = 100;

  TestStartPan(application, startPosition, position, time);

  for(int i = 0; i < 47; i++)
  {
    position += direction;
    TestMovePan(application, position, time);
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  TestEndPan(application, position, time);
  application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.called, true, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.screenPosition, Vector2(1.0f, 1.0f) + (direction * PAN_GESTURE_UPDATE_COUNT), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.localPosition, Vector2(1.0f, 1.0f) + (direction * PAN_GESTURE_UPDATE_COUNT), 10.0f, TEST_LOCATION);

  constraintData.Reset();
  END_TEST;
}

int UtcDaliPanGesturePredictionSmoothing01(void)
{
  TestApplication application;
  Dali::Integration::SetPanGesturePredictionMode(1);
  Dali::Integration::SetPanGestureSmoothingMode(1);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  SignalData             data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  Property::Index property = actor.RegisterProperty("Dummy Property", Vector3::ZERO);

  ConstraintData constraintData;
  Constraint     constraint = Constraint::New<Vector3>(actor, property, PanConstraint(constraintData));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_POSITION));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_DISPLACEMENT));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_VELOCITY));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_POSITION));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_DISPLACEMENT));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_VELOCITY));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::PANNING));
  constraint.Apply();

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector2  startPosition(1.0f, 1.0f);
  Vector2  position(-1.0f, 1.0f);
  Vector2  direction(Vector2::XAXIS * -1.0f);
  uint32_t time = 100;

  TestStartPan(application, startPosition, position, time);

  for(int i = 0; i < 47; i++)
  {
    position += direction;
    TestMovePan(application, position, time);
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  TestEndPan(application, position, time);
  application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.called, true, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.screenPosition, Vector2(1.0f, 1.0f) + (direction * PAN_GESTURE_UPDATE_COUNT), 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.localPosition, Vector2(1.0f, 1.0f) + (direction * PAN_GESTURE_UPDATE_COUNT), 10.0f, TEST_LOCATION);

  constraintData.Reset();
  END_TEST;
}

int UtcDaliPanGesturePredictionSmoothing02(void)
{
  TestApplication application;
  Dali::Integration::SetPanGesturePredictionMode(1);
  Dali::Integration::SetPanGestureMaximumPredictionAmount(1);
  Dali::Integration::SetPanGesturePredictionAmountAdjustment(2);
  Dali::Integration::SetPanGestureSmoothingMode(1);
  Dali::Integration::SetPanGestureSmoothingAmount(0.25f);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  SignalData             data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  Property::Index property = actor.RegisterProperty("Dummy Property", Vector3::ZERO);

  ConstraintData constraintData;
  Constraint     constraint = Constraint::New<Vector3>(actor, property, PanConstraint(constraintData));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_POSITION));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_DISPLACEMENT));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_VELOCITY));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_POSITION));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_DISPLACEMENT));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_VELOCITY));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::PANNING));
  constraint.Apply();

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector2  startPosition(2.0f, 2.0f);
  Vector2  position(4.0f, 2.0f);
  Vector2  directionX(Vector2::XAXIS);
  Vector2  directionY(Vector2::YAXIS);
  uint32_t time = 100;

  TestStartPan(application, startPosition, position, time);

  for(int i = 0; i < 7; i++)
  {
    position += directionX;
    TestMovePan(application, position, time);
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  position += directionX * 10.0f;
  TestMovePan(application, position, time);
  time += TestGetFrameInterval();
  application.SendNotification();
  application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);

  for(int i = 0; i < 2; i++)
  {
    position += (directionX * -1.0f);
    TestMovePan(application, position, time);
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  for(int i = 0; i < 10; i++)
  {
    position += directionX;
    TestMovePan(application, position, time);
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  for(int i = 0; i < 10; i++)
  {
    position += directionY;
    TestMovePan(application, position, time);
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  TestEndPan(application, position, time);
  application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.called, true, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.screenPosition, Vector2(2.0f, 2.0f) + position, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.localPosition, Vector2(2.0f, 2.0f) + position, 10.0f, TEST_LOCATION);

  constraintData.Reset();
  END_TEST;
}

int UtcDaliPanGesturePrediction2SmoothingMultiTap01(void)
{
  TestApplication application;

  Dali::Integration::SetPanGesturePredictionMode(2);
  Dali::Integration::SetPanGesturePredictionAmount(57);
  Dali::Integration::SetPanGestureSmoothingMode(2);
  Dali::Integration::SetPanGestureUseActualTimes(false);
  Dali::Integration::SetPanGestureInterpolationTimeRange(10);
  Dali::Integration::SetPanGestureScalarOnlyPredictionEnabled(false);
  Dali::Integration::SetPanGestureTwoPointPredictionEnabled(true);
  Dali::Integration::SetPanGestureTwoPointInterpolatePastTime(42);
  Dali::Integration::SetPanGestureTwoPointVelocityBias(0.35f);
  Dali::Integration::SetPanGestureTwoPointAccelerationBias(0.10f);
  Dali::Integration::SetPanGestureMultitapSmoothingRange(34);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  SignalData             data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  Property::Index property = actor.RegisterProperty("Dummy Property", Vector3::ZERO);

  ConstraintData constraintData;
  Constraint     constraint = Constraint::New<Vector3>(actor, property, PanConstraint(constraintData));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_POSITION));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_DISPLACEMENT));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_VELOCITY));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_POSITION));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_DISPLACEMENT));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_VELOCITY));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::PANNING));
  constraint.Apply();

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector2  startPosition(2.0f, 2.0f);
  Vector2  position(-1.0f, 2.0f);
  Vector2  direction(Vector2::XAXIS * -1.0f);
  uint32_t time = 100;

  TestStartPan(application, startPosition, position, time);

  for(int i = 0; i < 27; i++)
  {
    position += direction;
    TestMovePan(application, position, time);
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  TestEndPan(application, position, time);
  application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.called, true, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.screenPosition, Vector2(2.0f, 2.0f) + position, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.localPosition, Vector2(2.0f, 2.0f) + position, 10.0f, TEST_LOCATION);

  constraintData.Reset();
  END_TEST;
}

int UtcDaliPanGesturePrediction2SmoothingMultiTap02(void)
{
  TestApplication application;

  Dali::Integration::SetPanGesturePredictionMode(2);
  Dali::Integration::SetPanGestureSmoothingMode(2);
  Dali::Integration::SetPanGestureUseActualTimes(true);
  Dali::Integration::SetPanGestureInterpolationTimeRange(10);
  Dali::Integration::SetPanGestureScalarOnlyPredictionEnabled(true);
  Dali::Integration::SetPanGestureTwoPointPredictionEnabled(true);
  Dali::Integration::SetPanGestureTwoPointInterpolatePastTime(42);
  Dali::Integration::SetPanGestureTwoPointVelocityBias(0.35f);
  Dali::Integration::SetPanGestureTwoPointAccelerationBias(0.10f);
  Dali::Integration::SetPanGestureMultitapSmoothingRange(34);

  Dali::Integration::EnableProfiling(Dali::Integration::PROFILING_TYPE_PAN_GESTURE);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  SignalData             data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  Property::Index property = actor.RegisterProperty("Dummy Property", Vector3::ZERO);

  ConstraintData constraintData;
  Constraint     constraint = Constraint::New<Vector3>(actor, property, PanConstraint(constraintData));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_POSITION));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_DISPLACEMENT));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_VELOCITY));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_POSITION));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_DISPLACEMENT));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_VELOCITY));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::PANNING));
  constraint.Apply();

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector2  startPosition(2.0f, 2.0f);
  Vector2  position(17.0f, 2.0f);
  Vector2  direction(Vector2::XAXIS * -1.0f);
  uint32_t time = 100;

  TestStartPan(application, startPosition, position, time);

  for(int i = 0; i < 10; i++)
  {
    position += direction;
    TestMovePan(application, position, time);
    time += TestGetFrameInterval();

    position += direction;
    TestMovePan(application, position, time);
    time += TestGetFrameInterval();

    position += direction;
    TestMovePan(application, position, time);
    time += TestGetFrameInterval();

    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  for(int i = 0; i < 10; i++)
  {
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  for(int i = 0; i < 10; i++)
  {
    position += direction;
    TestMovePan(application, position, time);
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  for(int i = 0; i < 10; i++)
  {
    position += direction;
    TestMovePan(application, position, time);
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  TestEndPan(application, position, time);
  application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.called, true, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.screenPosition, Vector2(2.0f, 2.0f) + position, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.localPosition, Vector2(2.0f, 2.0f) + position, 10.0f, TEST_LOCATION);

  constraintData.Reset();
  END_TEST;
}

int UtcDaliPanGesturePrediction2Smoothing(void)
{
  TestApplication application;

  Dali::Integration::SetPanGesturePredictionMode(2);
  Dali::Integration::SetPanGesturePredictionAmount(57);
  Dali::Integration::SetPanGestureSmoothingMode(1);
  Dali::Integration::SetPanGestureUseActualTimes(false);
  Dali::Integration::SetPanGestureInterpolationTimeRange(10);
  Dali::Integration::SetPanGestureScalarOnlyPredictionEnabled(true);
  Dali::Integration::SetPanGestureTwoPointPredictionEnabled(true);
  Dali::Integration::SetPanGestureTwoPointInterpolatePastTime(42);
  Dali::Integration::SetPanGestureTwoPointVelocityBias(0.35f);
  Dali::Integration::SetPanGestureTwoPointAccelerationBias(0.10f);
  Dali::Integration::SetPanGestureMultitapSmoothingRange(34);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  SignalData             data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  Property::Index property = actor.RegisterProperty("Dummy Property", Vector3::ZERO);

  ConstraintData constraintData;
  Constraint     constraint = Constraint::New<Vector3>(actor, property, PanConstraint(constraintData));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_POSITION));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_DISPLACEMENT));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_VELOCITY));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_POSITION));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_DISPLACEMENT));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_VELOCITY));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::PANNING));
  constraint.Apply();

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector2  startPosition(2.0f, 2.0f);
  Vector2  position(17.0f, 2.0f);
  Vector2  direction(Vector2::XAXIS * -1.0f);
  uint32_t time = 100;

  TestStartPan(application, startPosition, position, time);

  for(int i = 0; i < 10; i++)
  {
    position += direction;
    TestMovePan(application, position, time);
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  position += direction;
  TestMovePan(application, position, time);
  time += TestGetFrameInterval();

  position += direction;
  TestMovePan(application, position, time);
  time += TestGetFrameInterval();

  position += direction;
  TestMovePan(application, position, time);
  time += TestGetFrameInterval();

  application.SendNotification();
  application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);

  for(int i = 0; i < 5; i++)
  {
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  for(int i = 0; i < 10; i++)
  {
    position += direction;
    TestMovePan(application, position, time);
    time += TestGetFrameInterval();
    application.SendNotification();
    application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);
  }

  TestEndPan(application, position, time);
  application.Render(TestApplication::DEFAULT_RENDER_INTERVAL);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.called, true, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.screenPosition, Vector2(2.0f, 2.0f) + position, 10.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.localPosition, Vector2(2.0f, 2.0f) + position, 10.0f, TEST_LOCATION);

  constraintData.Reset();
  END_TEST;
}

int UtcDaliPanGestureSetProperties(void)
{
  TestApplication       application;
  TestRenderController& renderController(application.GetRenderController());
  Dali::Integration::SetPanGesturePredictionMode(0);
  Dali::Integration::SetPanGestureSmoothingMode(0);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  SignalData             data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  Property::Index property = actor.RegisterProperty("Dummy Property", Vector3::ZERO);

  ConstraintData constraintData;
  Constraint     constraint = Constraint::New<Vector3>(actor, property, PanConstraint(constraintData));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_POSITION));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_DISPLACEMENT));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_VELOCITY));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_POSITION));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_DISPLACEMENT));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_VELOCITY));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::PANNING));
  constraint.Apply();

  // Render and notify
  application.SendNotification();
  application.Render();

  renderController.Initialize();
  DALI_TEST_EQUALS(renderController.WasCalled(TestRenderController::RequestUpdateFunc), false, TEST_LOCATION);

  Vector2 screenPosition(20.0f, 20.0f);
  Vector2 screenDisplacement(1.0f, 1.0f);
  Vector2 screenVelocity(1.3f, 4.0f);
  Vector2 localPosition(21.0f, 21.0f);
  Vector2 localDisplacement(0.5f, 0.5f);
  Vector2 localVelocity(1.5f, 2.5f);

  PanGestureDetector::SetPanGestureProperties(GeneratePan(1u, GestureState::STARTED, screenPosition, localPosition, screenDisplacement, localDisplacement, screenVelocity, localVelocity));
  DALI_TEST_EQUALS(renderController.WasCalled(TestRenderController::RequestUpdateFunc), true, TEST_LOCATION);

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(constraintData.called, true, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.screenPosition, screenPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.localPosition, localPosition, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.screenDisplacement, screenDisplacement, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.localDisplacement, localDisplacement, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.screenVelocity, screenVelocity, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.localVelocity, localVelocity, TEST_LOCATION);
  constraintData.Reset();
  END_TEST;
}

int UtcDaliPanGestureSetPropertiesAlreadyPanning(void)
{
  TestApplication application;
  Dali::Integration::SetPanGesturePredictionMode(0);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  SignalData             data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  Property::Index property = actor.RegisterProperty("Dummy Property", Vector3::ZERO);

  ConstraintData constraintData;
  Constraint     constraint = Constraint::New<Vector3>(actor, property, PanConstraint(constraintData));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_POSITION));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_DISPLACEMENT));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::SCREEN_VELOCITY));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_POSITION));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_DISPLACEMENT));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::LOCAL_VELOCITY));
  constraint.AddSource(Source(detector, PanGestureDetector::Property::PANNING));
  constraint.Apply();

  // Render and notify
  application.SendNotification();
  application.Render();

  Vector2  currentPosition(20.0f, 4.0f);
  uint32_t time = 100;
  TestStartPan(application, Vector2(20.0f, 20.0f), currentPosition, time);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  Vector2 screenPosition(100.0f, 20.0f);
  Vector2 localPosition(110.0f, 110.0f);

  PanGestureDetector::SetPanGestureProperties(GeneratePan(1u, GestureState::STARTED, screenPosition, localPosition));

  // Render and notify
  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(constraintData.called, true, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.screenPosition, currentPosition, 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(constraintData.localPosition, currentPosition, 0.1f, TEST_LOCATION);
  constraintData.Reset();
  END_TEST;
}

int UtcDaliPanGesturePropertyIndices(void)
{
  TestApplication    application;
  PanGestureDetector detector = PanGestureDetector::New();

  Property::IndexContainer indices;
  detector.GetPropertyIndices(indices);
  DALI_TEST_CHECK(indices.Size());
  DALI_TEST_EQUALS(indices.Size(), detector.GetPropertyCount(), TEST_LOCATION);
  END_TEST;
}

namespace
{
struct PropertyStringIndex
{
  const char* const     name;
  const Property::Index index;
  const Property::Type  type;
  const Property::Value value;
};

const PropertyStringIndex PROPERTY_TABLE[] =
  {
    {"screenPosition", PanGestureDetector::Property::SCREEN_POSITION, Property::VECTOR2, Vector2::ZERO},
    {"screenDisplacement", PanGestureDetector::Property::SCREEN_DISPLACEMENT, Property::VECTOR2, Vector2::ZERO},
    {"screenVelocity", PanGestureDetector::Property::SCREEN_VELOCITY, Property::VECTOR2, Vector2::ZERO},
    {"localPosition", PanGestureDetector::Property::LOCAL_POSITION, Property::VECTOR2, Vector2::ZERO},
    {"localDisplacement", PanGestureDetector::Property::LOCAL_DISPLACEMENT, Property::VECTOR2, Vector2::ZERO},
    {"localVelocity", PanGestureDetector::Property::LOCAL_VELOCITY, Property::VECTOR2, Vector2::ZERO},
    {"panning", PanGestureDetector::Property::PANNING, Property::BOOLEAN, false},
};
const unsigned int PROPERTY_TABLE_COUNT = sizeof(PROPERTY_TABLE) / sizeof(PROPERTY_TABLE[0]);
} // unnamed namespace

int UtcDaliPanGestureProperties(void)
{
  TestApplication    application;
  PanGestureDetector detector = PanGestureDetector::New();

  for(unsigned int i = 0; i < PROPERTY_TABLE_COUNT; ++i)
  {
    DALI_TEST_EQUALS(detector.GetPropertyName(PROPERTY_TABLE[i].index), std::string(PROPERTY_TABLE[i].name), TEST_LOCATION);
    DALI_TEST_EQUALS(detector.GetPropertyIndex(PROPERTY_TABLE[i].name), PROPERTY_TABLE[i].index, TEST_LOCATION);
    DALI_TEST_EQUALS(detector.GetPropertyType(PROPERTY_TABLE[i].index), PROPERTY_TABLE[i].type, TEST_LOCATION);
    DALI_TEST_EQUALS(detector.IsPropertyWritable(PROPERTY_TABLE[i].index), false, TEST_LOCATION);
    DALI_TEST_EQUALS(detector.IsPropertyAnimatable(PROPERTY_TABLE[i].index), false, TEST_LOCATION);
    DALI_TEST_EQUALS(detector.IsPropertyAConstraintInput(PROPERTY_TABLE[i].index), true, TEST_LOCATION);
    detector.SetProperty(PROPERTY_TABLE[i].index, Property::Value()); // Just for Coverage
  }

  END_TEST;
}

int UtcDaliPanGestureGetProperty(void)
{
  TestApplication    application;
  PanGestureDetector detector = PanGestureDetector::New();

  for(unsigned int i = 0; i < PROPERTY_TABLE_COUNT; ++i)
  {
    if(PROPERTY_TABLE[i].type == Property::VECTOR2)
    {
      bool value = detector.GetProperty(PROPERTY_TABLE[i].index).Get<bool>();
      DALI_TEST_EQUALS(PROPERTY_TABLE[i].value.Get<bool>(), value, TEST_LOCATION);
    }
    else if(PROPERTY_TABLE[i].type == Property::BOOLEAN)
    {
      Vector2 value = detector.GetProperty(PROPERTY_TABLE[i].index).Get<Vector2>();
      DALI_TEST_EQUALS(PROPERTY_TABLE[i].value.Get<Vector2>(), value, TEST_LOCATION);
    }
  }

  END_TEST;
}

int UtcDaliPanGestureGetPropertyWithSceneObject(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  for(unsigned int i = 0; i < PROPERTY_TABLE_COUNT; ++i)
  {
    detector.SetProperty(PROPERTY_TABLE[i].index, Property::Value()); // Just for Coverage

    if(PROPERTY_TABLE[i].type == Property::VECTOR2)
    {
      bool value = detector.GetProperty(PROPERTY_TABLE[i].index).Get<bool>();
      DALI_TEST_EQUALS(PROPERTY_TABLE[i].value.Get<bool>(), value, TEST_LOCATION);
    }
    else if(PROPERTY_TABLE[i].type == Property::BOOLEAN)
    {
      Vector2 value = detector.GetProperty(PROPERTY_TABLE[i].index).Get<Vector2>();
      DALI_TEST_EQUALS(PROPERTY_TABLE[i].value.Get<Vector2>(), value, TEST_LOCATION);
    }
  }

  END_TEST;
}

int UtcDaliPanGestureLayerConsumesTouch(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  SignalData             data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  // Add a layer to overlap the actor
  Layer layer = Layer::New();
  layer.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  layer.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(layer);
  layer.RaiseToTop();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit signals, should receive
  uint32_t time = 100;
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);
  TestEndPan(application, Vector2(26.0f, 20.0f), time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Set layer to consume all touch
  layer.SetProperty(Layer::Property::CONSUMES_TOUCH, true);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit the same signals again, should not receive
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);
  TestEndPan(application, Vector2(26.0f, 20.0f), time);

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  END_TEST;
}

int UtcDaliPanGestureNoTimeDiff(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Add a pan detector
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  SignalData             data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // As normal helper function adds intervals between presses we must generate the sequence
  // using other helper functions
  TestStartLongPress(application, 10.0f, 20.0f, 100); // Used to send a down press event
  TestMovePan(application, Vector2(26.0f, 20.0f), 100);
  TestMovePan(application, Vector2(26.0f, 20.0f), 100); // 2 motions required to trigger
  TestEndPan(application, Vector2(26.0f, 20.0f), 100);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(!std::isinf(data.receivedGesture.GetVelocity().x));
  DALI_TEST_CHECK(!std::isinf(data.receivedGesture.GetVelocity().y));
  DALI_TEST_CHECK(!std::isinf(data.receivedGesture.GetScreenVelocity().x));
  DALI_TEST_CHECK(!std::isinf(data.receivedGesture.GetScreenVelocity().y));
  data.Reset();

  data.Reset();

  END_TEST;
}

int UtcDaliPanGestureDisableDetectionDuringPanN(void)
{
  // Crash occurred when gesture-recognizer was deleted internally during a signal when the attached actor was detached

  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Add a pan detector
  PanGestureDetector detector      = PanGestureDetector::New();
  bool               functorCalled = false;
  detector.Attach(actor);
  detector.DetectedSignal().Connect(
    &application,
    [&detector, &functorCalled](Actor actor, PanGesture pan)
  {
    if(pan.GetState() == GestureState::FINISHED)
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
    uint32_t time = 100;
    TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);
    TestEndPan(application, Vector2(26.0f, 20.0f));

    DALI_TEST_CHECK(true); // No crash, test has passed
    DALI_TEST_EQUALS(functorCalled, true, TEST_LOCATION);
  }
  catch(...)
  {
    DALI_TEST_CHECK(false); // If we crash, the test has failed
  }

  END_TEST;
}

int UtcDaliPanGestureWhenGesturePropargation(void)
{
  TestApplication application;

  Actor parentActor = Actor::New();
  parentActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  parentActor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);

  Actor childActor = Actor::New();
  childActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  childActor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);

  parentActor.Add(childActor);
  application.GetScene().Add(parentActor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             pData;
  GestureReceivedFunctor pFunctor(pData);

  PanGestureDetector parentDetector = PanGestureDetector::New();
  parentDetector.Attach(parentActor);
  parentDetector.DetectedSignal().Connect(&application, pFunctor);

  SignalData             cData;
  GestureReceivedFunctor cFunctor(cData);

  PanGestureDetector childDetector = PanGestureDetector::New();
  childDetector.Attach(childActor);
  childDetector.DetectedSignal().Connect(&application, cFunctor);

  // Start gesture within the actor's area, we receive the gesture not parent actor but child actor.
  uint32_t time = 100;
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);

  DALI_TEST_EQUALS(true, cData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, pData.functorCalled, TEST_LOCATION);
  cData.Reset();
  pData.Reset();

  TestMovePan(application, Vector2(26.0f, 4.0f), time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, cData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, pData.functorCalled, TEST_LOCATION);
  cData.Reset();
  pData.Reset();

  TestEndPan(application, Vector2(26.0f, 20.0f), time);

  DALI_TEST_EQUALS(true, cData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, pData.functorCalled, TEST_LOCATION);
  cData.Reset();
  pData.Reset();

  // If GesturePropargation is set, a gesture event is to pass over to the parent.
  cData.needGesturePropagation = true;

  // So now the parent got the gesture event.
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);
  DALI_TEST_EQUALS(true, cData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, pData.functorCalled, TEST_LOCATION);
  cData.Reset();
  pData.Reset();

  TestMovePan(application, Vector2(26.0f, 4.0f), time);
  time += TestGetFrameInterval();

  // child does not receive gestures. This is because we have passed the permission of the gesture to the parent.
  DALI_TEST_EQUALS(false, cData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, pData.functorCalled, TEST_LOCATION);
  cData.Reset();
  pData.Reset();

  TestEndPan(application, Vector2(26.0f, 20.0f), time);
  DALI_TEST_EQUALS(false, cData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, pData.functorCalled, TEST_LOCATION);
  cData.Reset();
  pData.Reset();

  END_TEST;
}

int UtcDaliPanGestureHandleEvent(void)
{
  TestApplication          application;
  Dali::Integration::Scene scene    = application.GetScene();
  RenderTaskList           taskList = scene.GetRenderTaskList();
  Dali::RenderTask         task     = taskList.GetTask(0);

  Actor parentActor = Actor::New();
  parentActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  parentActor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);

  Actor childActor = Actor::New();
  childActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  childActor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);

  parentActor.Add(childActor);
  application.GetScene().Add(parentActor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             pData;
  GestureReceivedFunctor pFunctor(pData);

  PanGestureDetector parentDetector = PanGestureDetector::New();
  parentDetector.DetectedSignal().Connect(&application, pFunctor);

  Dali::Integration::TouchEvent tp = GenerateSingleTouch(PointState::DOWN, Vector2(50.0f, 50.0f), 100);
  Internal::TouchEventPtr       touchEventImpl(new Internal::TouchEvent(100));
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->SetRenderTask(task);
  Dali::TouchEvent touchEventHandle(touchEventImpl.Get());
  parentDetector.HandleEvent(parentActor, touchEventHandle);

  tp             = GenerateSingleTouch(PointState::MOTION, Vector2(70.0f, 70.0f), 150);
  touchEventImpl = new Internal::TouchEvent(150);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  parentDetector.HandleEvent(parentActor, touchEventHandle);

  tp             = GenerateSingleTouch(PointState::MOTION, Vector2(90.0f, 90.0f), 200);
  touchEventImpl = new Internal::TouchEvent(200);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  parentDetector.HandleEvent(parentActor, touchEventHandle);

  tp             = GenerateSingleTouch(PointState::UP, Vector2(100.0f, 100.0f), 250);
  touchEventImpl = new Internal::TouchEvent(250);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  parentDetector.HandleEvent(parentActor, touchEventHandle);

  DALI_TEST_EQUALS(true, pData.functorCalled, TEST_LOCATION);
  pData.Reset();

  END_TEST;
}

int UtcDaliPanGestureSignalReceptionWithGeometryHittest(void)
{
  TestApplication application;
  application.GetScene().SetGeometryHittestEnabled(true);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pan within the actor's area
  uint32_t time = 100;
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Continue the pan within the actor's area - we should still receive the signal
  data.Reset();

  TestMovePan(application, Vector2(26.0f, 4.0f), time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Gesture ends within actor's area
  data.Reset();

  TestEndPan(application, Vector2(10.0f, 4.0f), time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  // Detach actor
  detector.DetachAll();

  // Ensure we are no longer signalled
  data.Reset();

  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);
  TestMovePan(application, Vector2(26.0f, 4.0f), time);
  time += TestGetFrameInterval();
  TestEndPan(application, Vector2(10.0f, 4.0f), time);

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureFeedTouchWhenGesturePropagation(void)
{
  TestApplication          application;
  Dali::Integration::Scene scene    = application.GetScene();
  RenderTaskList           taskList = scene.GetRenderTaskList();
  Dali::RenderTask         task     = taskList.GetTask(0);

  Actor parentActor = Actor::New();
  parentActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  parentActor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);

  Actor childActor = Actor::New();
  childActor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  childActor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);

  parentActor.Add(childActor);
  application.GetScene().Add(parentActor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             pData;
  GestureReceivedFunctor pFunctor(pData);

  PanGestureDetector parentDetector = PanGestureDetector::New();
  parentDetector.DetectedSignal().Connect(&application, pFunctor);

  SignalData             cData;
  GestureReceivedFunctor cFunctor(cData);

  PanGestureDetector childDetector = PanGestureDetector::New();
  childDetector.DetectedSignal().Connect(&application, cFunctor);

  // Start gesture within the actor's area, we receive the gesture not parent actor but child actor.
  Dali::Integration::TouchEvent tp = GenerateSingleTouch(PointState::DOWN, Vector2(50.0f, 50.0f), 100);
  Internal::TouchEventPtr       touchEventImpl(new Internal::TouchEvent(100));
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->SetRenderTask(task);
  Dali::TouchEvent touchEventHandle(touchEventImpl.Get());
  if(!childDetector.HandleEvent(childActor, touchEventHandle))
  {
    parentDetector.HandleEvent(parentActor, touchEventHandle);
  }

  tp             = GenerateSingleTouch(PointState::MOTION, Vector2(60.0f, 60.0f), 150);
  touchEventImpl = new Internal::TouchEvent(150);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  if(!childDetector.HandleEvent(childActor, touchEventHandle))
  {
    parentDetector.HandleEvent(parentActor, touchEventHandle);
  }

  tp             = GenerateSingleTouch(PointState::MOTION, Vector2(70.0f, 70.0f), 200);
  touchEventImpl = new Internal::TouchEvent(200);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  if(!childDetector.HandleEvent(childActor, touchEventHandle))
  {
    parentDetector.HandleEvent(parentActor, touchEventHandle);
  }

  tp             = GenerateSingleTouch(PointState::MOTION, Vector2(80.0f, 80.0f), 250);
  touchEventImpl = new Internal::TouchEvent(200);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  if(!childDetector.HandleEvent(childActor, touchEventHandle))
  {
    parentDetector.HandleEvent(parentActor, touchEventHandle);
  }

  tp             = GenerateSingleTouch(PointState::UP, Vector2(100.0f, 100.0f), 300);
  touchEventImpl = new Internal::TouchEvent(250);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  if(!childDetector.HandleEvent(childActor, touchEventHandle))
  {
    parentDetector.HandleEvent(parentActor, touchEventHandle);
  }

  DALI_TEST_EQUALS(true, cData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(false, pData.functorCalled, TEST_LOCATION);
  cData.Reset();
  pData.Reset();

  // If GesturePropargation is set, a gesture event is to pass over to the parent.
  SignalData              cPData;
  PropagationActorFunctor cPFunctor(cPData, true);
  childDetector.DetectedSignal().Connect(&application, cPFunctor);

  // So now the parent got the gesture event.
  tp             = GenerateSingleTouch(PointState::DOWN, Vector2(50.0f, 50.0f), 100);
  touchEventImpl = new Internal::TouchEvent(100);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  if(!childDetector.HandleEvent(childActor, touchEventHandle))
  {
    parentDetector.HandleEvent(parentActor, touchEventHandle);
  }

  tp             = GenerateSingleTouch(PointState::MOTION, Vector2(60.0f, 60.0f), 150);
  touchEventImpl = new Internal::TouchEvent(150);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  if(!childDetector.HandleEvent(childActor, touchEventHandle))
  {
    parentDetector.HandleEvent(parentActor, touchEventHandle);
  }

  tp             = GenerateSingleTouch(PointState::MOTION, Vector2(70.0f, 70.0f), 200);
  touchEventImpl = new Internal::TouchEvent(200);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  if(!childDetector.HandleEvent(childActor, touchEventHandle))
  {
    parentDetector.HandleEvent(parentActor, touchEventHandle);
  }

  tp             = GenerateSingleTouch(PointState::MOTION, Vector2(80.0f, 80.0f), 250);
  touchEventImpl = new Internal::TouchEvent(250);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  if(!childDetector.HandleEvent(childActor, touchEventHandle))
  {
    parentDetector.HandleEvent(parentActor, touchEventHandle);
  }

  tp             = GenerateSingleTouch(PointState::UP, Vector2(100.0f, 100.0f), 300);
  touchEventImpl = new Internal::TouchEvent(300);
  touchEventImpl->AddPoint(tp.GetPoint(0));
  touchEventImpl->SetRenderTask(task);
  touchEventHandle = Dali::TouchEvent(touchEventImpl.Get());
  if(!childDetector.HandleEvent(childActor, touchEventHandle))
  {
    parentDetector.HandleEvent(parentActor, touchEventHandle);
  }

  DALI_TEST_EQUALS(true, cPData.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, pData.functorCalled, TEST_LOCATION);
  cPData.Reset();
  pData.Reset();

  END_TEST;
}

int UtcDaliPanGestureSignalGetLastGestureState(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);

  Dali::Integration::Scene scene = application.GetScene();
  scene.Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Start pan within the actor's area
  uint32_t time = 100;
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(26.0f, 20.0f), time);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::STARTED, data.receivedGesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::STARTED, scene.GetLastPanGestureState(), TEST_LOCATION);

  // Continue the pan within the actor's area - we should still receive the signal
  data.Reset();

  TestMovePan(application, Vector2(26.0f, 4.0f), time);
  time += TestGetFrameInterval();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::CONTINUING, data.receivedGesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::CONTINUING, scene.GetLastPanGestureState(), TEST_LOCATION);

  // Gesture ends within actor's area - we would receive a finished state
  data.Reset();

  // Remove the actor from stage and reset the data
  application.GetScene().Remove(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  TestEndPan(application, Vector2(10.0f, 4.0f), time);

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::CONTINUING, scene.GetLastPanGestureState(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureGetSourceSubType(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Helper: send a pan sequence where the DOWN event carries the given subclass.
  // source=1 (PRIMARY) triggers the recognizer source update block.
  auto EmitPanWithSubclass = [&](Device::Subclass::Type subclass, uint32_t startTime)
  {
    auto MakeTouch = [&](PointState::Type state, const Vector2& pos, uint32_t time)
    {
      Dali::Integration::TouchEvent ev;
      Dali::Integration::Point      p;
      p.SetState(state);
      p.SetDeviceId(4);
      p.SetScreenPosition(pos);
      p.SetDeviceClass(Device::Class::TOUCH);
      p.SetDeviceSubclass(subclass);
      p.SetMouseButton(MouseButton::PRIMARY);
      ev.points.push_back(p);
      ev.time = time;
      return ev;
    };

    application.ProcessEvent(MakeTouch(PointState::DOWN, Vector2(10.0f, 10.0f), startTime));
    application.ProcessEvent(MakeTouch(PointState::MOTION, Vector2(26.0f, 10.0f), startTime + 16));
    application.ProcessEvent(MakeTouch(PointState::MOTION, Vector2(42.0f, 10.0f), startTime + 32));
    application.ProcessEvent(MakeTouch(PointState::MOTION, Vector2(58.0f, 10.0f), startTime + 48));
    application.SendNotification();
  };

  // Pan with FINGER subclass
  EmitPanWithSubclass(Device::Subclass::FINGER, 100);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(data.receivedGesture.GetDeviceClass(), Device::Class::TOUCH, TEST_LOCATION);
  DALI_TEST_EQUALS(data.receivedGesture.GetDeviceSubclass(), Device::Subclass::FINGER, TEST_LOCATION);
  data.Reset();

  // Finish the previous pan
  {
    Dali::Integration::TouchEvent ev;
    Dali::Integration::Point      p;
    p.SetState(PointState::UP);
    p.SetDeviceId(4);
    p.SetScreenPosition(Vector2(58.0f, 10.0f));
    p.SetDeviceClass(Device::Class::TOUCH);
    p.SetDeviceSubclass(Device::Subclass::FINGER);
    p.SetMouseButton(MouseButton::PRIMARY);
    ev.points.push_back(p);
    ev.time = 200;
    application.ProcessEvent(ev);
    application.SendNotification();
    data.Reset();
  }

  // Pan with PALM subclass
  EmitPanWithSubclass(Device::Subclass::PALM, 1000);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(data.receivedGesture.GetDeviceSubclass(), Device::Subclass::PALM, TEST_LOCATION);
  data.Reset();

  END_TEST;
}

namespace
{
/**
 * Feeds a single-touch pan of the given horizontal distance straight into the detector through
 * HandleEvent(), i.e. the detector-owned recognizer path used by geometry hit-testing.
 * Two MOTION events are sent so the recognizer's minimum pan event count is satisfied.
 */
void FeedHandleEventPan(PanGestureDetector& detector, Actor& actor, Dali::RenderTask& task, float distance, uint32_t startTime)
{
  auto feed = [&](PointState::Type state, const Vector2& position, uint32_t time)
  {
    Dali::Integration::TouchEvent tp = GenerateSingleTouch(state, position, time);
    Internal::TouchEventPtr       touchEventImpl(new Internal::TouchEvent(time));
    touchEventImpl->AddPoint(tp.GetPoint(0));
    touchEventImpl->SetRenderTask(task);
    Dali::TouchEvent touchEventHandle(touchEventImpl.Get());
    detector.HandleEvent(actor, touchEventHandle);
  };

  feed(PointState::DOWN, Vector2(50.0f, 50.0f), startTime);
  feed(PointState::MOTION, Vector2(50.0f + distance * 0.5f, 50.0f), startTime + 50u);
  feed(PointState::MOTION, Vector2(50.0f + distance, 50.0f), startTime + 100u);
  feed(PointState::UP, Vector2(50.0f + distance, 50.0f), startTime + 150u);
}
} // namespace

int UtcDaliPanGestureHandleEventAppliesUpdatedMinimumDistance(void)
{
  TestApplication          application;
  Dali::Integration::Scene scene = application.GetScene();
  Dali::RenderTask         task  = scene.GetRenderTaskList().GetTask(0);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  scene.Add(actor);

  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.DetectedSignal().Connect(&application, functor);

  // A 10px pan is below the default 15px minimum distance. This also creates the detector-owned recognizer.
  FeedHandleEventPan(detector, actor, task, 10.0f, 100u);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Lower the application-wide threshold after the recognizer exists: the same pan must now be recognised.
  Dali::Integration::SetPanGestureMinimumDistance(5);
  FeedHandleEventPan(detector, actor, task, 10.0f, 5000u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Raise it again: the same pan must be rejected again.
  Dali::Integration::SetPanGestureMinimumDistance(30);
  FeedHandleEventPan(detector, actor, task, 10.0f, 10000u);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  Dali::Integration::SetPanGestureMinimumDistance(Dali::Integration::DEFAULT_PAN_GESTURE_MINIMUM_DISTANCE);
  END_TEST;
}

int UtcDaliPanGestureHandleEventAppliesUpdatedTouchesRequired(void)
{
  TestApplication          application;
  Dali::Integration::Scene scene = application.GetScene();
  Dali::RenderTask         task  = scene.GetRenderTaskList().GetTask(0);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  scene.Add(actor);

  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.DetectedSignal().Connect(&application, functor);

  // Single-touch pan with the default requirement of exactly one touch.
  FeedHandleEventPan(detector, actor, task, 40.0f, 100u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Change this detector's own requirement after its recognizer exists: single-touch pans must stop.
  detector.SetMaximumTouchesRequired(2u);
  detector.SetMinimumTouchesRequired(2u);
  FeedHandleEventPan(detector, actor, task, 40.0f, 5000u);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // And back again.
  detector.SetMinimumTouchesRequired(1u);
  detector.SetMaximumTouchesRequired(1u);
  FeedHandleEventPan(detector, actor, task, 40.0f, 10000u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

namespace
{
/**
 * Builds a single-touch event whose point carries the given device name. The mouse button is set so
 * the recognizer records the point as the gesture source.
 */
Dali::Integration::TouchEvent GenerateNamedTouch(PointState::Type state, const Vector2& position, const Dali::String& deviceName, uint32_t time)
{
  Dali::Integration::TouchEvent touchEvent;
  Dali::Integration::Point      point;
  point.SetState(state);
  point.SetDeviceId(4);
  point.SetScreenPosition(position);
  point.SetDeviceClass(Device::Class::POINTER);
  point.SetDeviceSubclass(Device::Subclass::REMOCON);
  point.SetMouseButton(MouseButton::PRIMARY);
  point.SetDeviceName(deviceName);
  touchEvent.points.push_back(point);
  touchEvent.time = time;
  return touchEvent;
}
} // namespace

int UtcDaliPanGestureGetDeviceName(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  const Dali::String deviceName("Pointing Device");
  application.ProcessEvent(GenerateNamedTouch(PointState::DOWN, Vector2(10.0f, 10.0f), deviceName, 100));
  application.ProcessEvent(GenerateNamedTouch(PointState::MOTION, Vector2(26.0f, 10.0f), deviceName, 116));
  application.ProcessEvent(GenerateNamedTouch(PointState::MOTION, Vector2(42.0f, 10.0f), deviceName, 132));
  application.ProcessEvent(GenerateNamedTouch(PointState::MOTION, Vector2(58.0f, 10.0f), deviceName, 148));
  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(data.receivedGesture.GetDeviceName(), deviceName, TEST_LOCATION);
  DALI_TEST_EQUALS(data.receivedGesture.GetDeviceClass(), Device::Class::POINTER, TEST_LOCATION);
  DALI_TEST_EQUALS(data.receivedGesture.GetDeviceSubclass(), Device::Subclass::REMOCON, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureHandleEventPreservesDeviceName(void)
{
  TestApplication          application;
  Dali::Integration::Scene scene = application.GetScene();
  Dali::RenderTask         task  = scene.GetRenderTaskList().GetTask(0);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  scene.Add(actor);

  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.DetectedSignal().Connect(&application, functor);

  const Dali::String deviceName("Pointing Device");
  auto               feed = [&](PointState::Type state, const Vector2& position, uint32_t time)
  {
    Dali::Integration::TouchEvent tp = GenerateNamedTouch(state, position, deviceName, time);
    Internal::TouchEventPtr       touchEventImpl(new Internal::TouchEvent(time));
    touchEventImpl->AddPoint(tp.GetPoint(0));
    touchEventImpl->SetRenderTask(task);
    Dali::TouchEvent touchEventHandle(touchEventImpl.Get());
    detector.HandleEvent(actor, touchEventHandle);
  };

  // The geometry path rebuilds the Integration::Point from the public TouchEvent; the name must survive.
  feed(PointState::DOWN, Vector2(50.0f, 50.0f), 100u);
  feed(PointState::MOTION, Vector2(70.0f, 50.0f), 150u);
  feed(PointState::MOTION, Vector2(90.0f, 50.0f), 200u);
  feed(PointState::UP, Vector2(90.0f, 50.0f), 250u);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(data.receivedGesture.GetDeviceName(), deviceName, TEST_LOCATION);

  END_TEST;
}

namespace
{
/**
 * Builds a touch event whose points all carry the given device metadata. The mouse button is set on
 * every point so the trigger-point snapshot is recorded as well as the sequence source.
 */
Dali::Integration::TouchEvent GenerateDeviceTouch(Device::Class::Type deviceClass, Device::Subclass::Type deviceSubclass, const Dali::String& deviceName, uint32_t time, PointState::Type state, const Vector2& position, PointState::Type secondState = PointState::INTERRUPTED, const Vector2& secondPosition = Vector2::ZERO)
{
  Dali::Integration::TouchEvent touchEvent;
  auto                          addPoint = [&](PointState::Type pointState, const Vector2& pointPosition, int32_t deviceId)
  {
    Dali::Integration::Point point;
    point.SetState(pointState);
    point.SetDeviceId(deviceId);
    point.SetScreenPosition(pointPosition);
    point.SetDeviceClass(deviceClass);
    point.SetDeviceSubclass(deviceSubclass);
    point.SetDeviceName(deviceName);
    point.SetMouseButton(MouseButton::PRIMARY);
    touchEvent.points.push_back(point);
  };
  addPoint(state, position, 4);
  if(secondState != PointState::INTERRUPTED)
  {
    addPoint(secondState, secondPosition, 7);
  }
  touchEvent.time = time;
  return touchEvent;
}

/**
 * Pans horizontally by 40px with one finger of the given device through the scene (classic path).
 */
void EmitDevicePan(TestApplication& application, Device::Class::Type deviceClass, Device::Subclass::Type deviceSubclass, const Dali::String& deviceName, uint32_t startTime, const Vector2& displacement = Vector2(40.0f, 0.0f))
{
  const Vector2 start(20.0f, 20.0f);
  application.ProcessEvent(GenerateDeviceTouch(deviceClass, deviceSubclass, deviceName, startTime, PointState::DOWN, start));
  application.ProcessEvent(GenerateDeviceTouch(deviceClass, deviceSubclass, deviceName, startTime + 16, PointState::MOTION, start + displacement * 0.5f));
  application.ProcessEvent(GenerateDeviceTouch(deviceClass, deviceSubclass, deviceName, startTime + 32, PointState::MOTION, start + displacement));
  application.ProcessEvent(GenerateDeviceTouch(deviceClass, deviceSubclass, deviceName, startTime + 48, PointState::MOTION, start + displacement * 1.5f));
  application.ProcessEvent(GenerateDeviceTouch(deviceClass, deviceSubclass, deviceName, startTime + 64, PointState::UP, start + displacement * 1.5f));
  application.SendNotification();
}

/**
 * Pans horizontally by 40px with two fingers of the given device through the scene (classic path).
 */
void EmitTwoFingerDevicePan(TestApplication& application, Device::Class::Type deviceClass, Device::Subclass::Type deviceSubclass, const Dali::String& deviceName, uint32_t startTime)
{
  const Vector2 a(20.0f, 20.0f);
  const Vector2 b(20.0f, 60.0f);
  const Vector2 step(20.0f, 0.0f);
  application.ProcessEvent(GenerateDeviceTouch(deviceClass, deviceSubclass, deviceName, startTime, PointState::DOWN, a));
  application.ProcessEvent(GenerateDeviceTouch(deviceClass, deviceSubclass, deviceName, startTime + 8, PointState::STATIONARY, a, PointState::DOWN, b));
  application.ProcessEvent(GenerateDeviceTouch(deviceClass, deviceSubclass, deviceName, startTime + 16, PointState::MOTION, a + step, PointState::MOTION, b + step));
  application.ProcessEvent(GenerateDeviceTouch(deviceClass, deviceSubclass, deviceName, startTime + 32, PointState::MOTION, a + step * 2.0f, PointState::MOTION, b + step * 2.0f));
  application.ProcessEvent(GenerateDeviceTouch(deviceClass, deviceSubclass, deviceName, startTime + 48, PointState::MOTION, a + step * 3.0f, PointState::MOTION, b + step * 3.0f));
  application.ProcessEvent(GenerateDeviceTouch(deviceClass, deviceSubclass, deviceName, startTime + 64, PointState::UP, a + step * 3.0f, PointState::UP, b + step * 3.0f));
  application.SendNotification();
}

const Dali::String REMOTE_NAME("Pointing Device");
} // namespace

int UtcDaliPanGestureDetectorOptionsP(void)
{
  TestApplication application;

  PanGestureDetector::Options options;
  DALI_TEST_EQUALS(options.GetMinimumTouchesRequired(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(options.GetMaximumTouchesRequired(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(options.GetMaximumMotionEventAge(), std::numeric_limits<uint32_t>::max(), TEST_LOCATION);
  DALI_TEST_EQUALS(options.GetAngleCount(), 0u, TEST_LOCATION);

  options.SetMinimumTouchesRequired(2u);
  options.SetMaximumTouchesRequired(3u);
  options.SetMaximumMotionEventAge(120u);
  DALI_TEST_EQUALS(options.GetMinimumTouchesRequired(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(options.GetMaximumTouchesRequired(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(options.GetMaximumMotionEventAge(), 120u, TEST_LOCATION);

  // Angles are normalised like the detector's own AddAngle(): threshold made positive and clamped
  // to PI, angle wrapped into [-PI, PI].
  options.AddAngle(Degree(190.0f), Degree(-30.0f));
  DALI_TEST_EQUALS(options.GetAngleCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(options.GetAngle(0).first, Radian(Degree(-170.0f)), 0.0001f, TEST_LOCATION);
  DALI_TEST_EQUALS(options.GetAngle(0).second, Radian(Degree(30.0f)), 0.0001f, TEST_LOCATION);

  options.AddAngle(Degree(0.0f), Radian(10.0f)); // threshold above PI is clamped
  DALI_TEST_EQUALS(options.GetAngle(1).second, Radian(Math::PI), 0.0001f, TEST_LOCATION);

  options.AddDirection(PanGestureDetector::DIRECTION_HORIZONTAL); // adds both directions with the default threshold
  DALI_TEST_EQUALS(options.GetAngleCount(), 4u, TEST_LOCATION);
  DALI_TEST_EQUALS(options.GetAngle(2).second, PanGestureDetector::DEFAULT_THRESHOLD, 0.0001f, TEST_LOCATION);

  options.RemoveDirection(PanGestureDetector::DIRECTION_HORIZONTAL);
  DALI_TEST_EQUALS(options.GetAngleCount(), 2u, TEST_LOCATION);
  options.RemoveAngle(Degree(190.0f)); // removes the wrapped -170 entry
  DALI_TEST_EQUALS(options.GetAngleCount(), 1u, TEST_LOCATION);
  options.ClearAngles();
  DALI_TEST_EQUALS(options.GetAngleCount(), 0u, TEST_LOCATION);

  // Out-of-range index behaves like the detector: zero pair.
  DALI_TEST_EQUALS(options.GetAngle(5).first, Radian(0.0f), TEST_LOCATION);

  // Copies are independent.
  PanGestureDetector::Options copied(options);
  copied.SetMinimumTouchesRequired(1u);
  DALI_TEST_EQUALS(options.GetMinimumTouchesRequired(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(copied.GetMinimumTouchesRequired(), 1u, TEST_LOCATION);

  PanGestureDetector::Options moved(std::move(copied));
  DALI_TEST_EQUALS(moved.GetMinimumTouchesRequired(), 1u, TEST_LOCATION);
  DALI_TEST_ASSERTION(copied.GetMinimumTouchesRequired(), "moved-from PanGestureDetector::Options");

  END_TEST;
}

int UtcDaliPanGestureDetectorGetDefaultOptionsP(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();
  detector.SetMinimumTouchesRequired(2u);
  detector.SetMaximumTouchesRequired(3u);
  detector.SetMaximumMotionEventAge(80u);
  detector.AddDirection(PanGestureDetector::DIRECTION_VERTICAL);

  PanGestureDetector::Options defaults = detector.GetDefaultOptions();
  DALI_TEST_EQUALS(defaults.GetMinimumTouchesRequired(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetMaximumTouchesRequired(), 3u, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetMaximumMotionEventAge(), 80u, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetAngleCount(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(defaults.GetAngle(0).first, detector.GetAngle(0).first, 0.0001f, TEST_LOCATION);

  // The copy does not write back to the detector.
  defaults.SetMinimumTouchesRequired(1u);
  defaults.ClearAngles();
  DALI_TEST_EQUALS(detector.GetMinimumTouchesRequired(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(detector.GetAngleCount(), 2u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureDetectorDeviceOptionsSetGetClearP(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  const GestureDeviceSelector remote = GestureDeviceSelector::ByDeviceClassAndSubclass(Device::Class::POINTER, Device::Subclass::REMOCON);
  const GestureDeviceSelector touch  = GestureDeviceSelector::ByDeviceClass(Device::Class::TOUCH);

  PanGestureDetector::Options queried;
  DALI_TEST_CHECK(!detector.GetDeviceOptions(remote, queried));

  PanGestureDetector::Options remoteOptions = detector.GetDefaultOptions();
  remoteOptions.SetMaximumMotionEventAge(120u);
  detector.SetDeviceOptions(remote, remoteOptions);

  // Exact lookup returns an independent copy of what was registered.
  DALI_TEST_CHECK(detector.GetDeviceOptions(remote, queried));
  DALI_TEST_EQUALS(queried.GetMaximumMotionEventAge(), 120u, TEST_LOCATION);
  remoteOptions.SetMaximumMotionEventAge(999u); // caller's copy changes nothing registered
  DALI_TEST_CHECK(detector.GetDeviceOptions(remote, queried));
  DALI_TEST_EQUALS(queried.GetMaximumMotionEventAge(), 120u, TEST_LOCATION);

  // A different selector is not found and leaves the output untouched.
  queried.SetMaximumMotionEventAge(5u);
  DALI_TEST_CHECK(!detector.GetDeviceOptions(touch, queried));
  DALI_TEST_EQUALS(queried.GetMaximumMotionEventAge(), 5u, TEST_LOCATION);

  // The detector's own defaults are untouched by device options.
  DALI_TEST_EQUALS(detector.GetMaximumMotionEventAge(), std::numeric_limits<uint32_t>::max(), TEST_LOCATION);

  // Registering again replaces; clearing removes; clearing again is a no-op.
  remoteOptions.SetMaximumMotionEventAge(60u);
  detector.SetDeviceOptions(remote, remoteOptions);
  DALI_TEST_CHECK(detector.GetDeviceOptions(remote, queried));
  DALI_TEST_EQUALS(queried.GetMaximumMotionEventAge(), 60u, TEST_LOCATION);
  detector.ClearDeviceOptions(remote);
  DALI_TEST_CHECK(!detector.GetDeviceOptions(remote, queried));
  detector.ClearDeviceOptions(remote);

  END_TEST;
}

int UtcDaliPanGestureDetectorDeviceOptionsInvalidN(void)
{
  TestApplication application;

  PanGestureDetector          detector = PanGestureDetector::New();
  const GestureDeviceSelector touch    = GestureDeviceSelector::ByDeviceClass(Device::Class::TOUCH);

  PanGestureDetector::Options options;
  options.SetMinimumTouchesRequired(0u);
  DALI_TEST_ASSERTION(detector.SetDeviceOptions(touch, options), "positive touch counts");

  options.SetMinimumTouchesRequired(3u);
  options.SetMaximumTouchesRequired(2u);
  DALI_TEST_ASSERTION(detector.SetDeviceOptions(touch, options), "minimum touches <= maximum touches");

  // Nothing was registered by the rejected calls.
  PanGestureDetector::Options queried;
  DALI_TEST_CHECK(!detector.GetDeviceOptions(touch, queried));

  END_TEST;
}

int UtcDaliPanGestureDetectorDeviceOptionsTouchesRequired(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Touch screens must pan with exactly two fingers; everything else keeps the one-finger default.
  PanGestureDetector::Options touchOptions = detector.GetDefaultOptions();
  touchOptions.SetMinimumTouchesRequired(2u);
  touchOptions.SetMaximumTouchesRequired(2u);
  detector.SetDeviceOptions(GestureDeviceSelector::ByDeviceClass(Device::Class::TOUCH), touchOptions);

  // One finger on a touch screen: rejected by the touch profile.
  EmitDevicePan(application, Device::Class::TOUCH, Device::Subclass::FINGER, Dali::String(""), 100u);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // One "finger" from a mouse: no profile, the default (1..1) applies.
  EmitDevicePan(application, Device::Class::MOUSE, Device::Subclass::NONE, Dali::String(""), 5000u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(data.receivedGesture.GetNumberOfTouches(), 1u, TEST_LOCATION);
  data.Reset();

  // Two fingers on a touch screen: accepted by the touch profile.
  EmitTwoFingerDevicePan(application, Device::Class::TOUCH, Device::Subclass::FINGER, Dali::String(""), 10000u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(data.receivedGesture.GetNumberOfTouches(), 2u, TEST_LOCATION);
  data.Reset();

  // Clearing the profile restores the default for touch screens.
  detector.ClearDeviceOptions(GestureDeviceSelector::ByDeviceClass(Device::Class::TOUCH));
  EmitDevicePan(application, Device::Class::TOUCH, Device::Subclass::FINGER, Dali::String(""), 15000u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureDetectorDeviceOptionsAngle(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Pointing devices may only pan horizontally; the default accepts any direction.
  PanGestureDetector::Options pointerOptions = detector.GetDefaultOptions();
  pointerOptions.AddDirection(PanGestureDetector::DIRECTION_HORIZONTAL);
  detector.SetDeviceOptions(GestureDeviceSelector::ByDeviceClass(Device::Class::POINTER), pointerOptions);

  const Vector2 vertical(0.0f, 40.0f);
  EmitDevicePan(application, Device::Class::POINTER, Device::Subclass::REMOCON, REMOTE_NAME, 100u, vertical);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  EmitDevicePan(application, Device::Class::TOUCH, Device::Subclass::FINGER, Dali::String(""), 5000u, vertical);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  EmitDevicePan(application, Device::Class::POINTER, Device::Subclass::REMOCON, REMOTE_NAME, 10000u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureDetectorDeviceOptionsPrecedence(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Class+subclass profile allows any direction; the named model is restricted to horizontal.
  detector.SetDeviceOptions(GestureDeviceSelector::ByDeviceClassAndSubclass(Device::Class::POINTER, Device::Subclass::REMOCON), detector.GetDefaultOptions());
  PanGestureDetector::Options namedOptions = detector.GetDefaultOptions();
  namedOptions.AddDirection(PanGestureDetector::DIRECTION_HORIZONTAL);
  detector.SetDeviceOptions(GestureDeviceSelector::ByDeviceName(REMOTE_NAME), namedOptions);

  const Vector2 vertical(0.0f, 40.0f);

  // Same class and subclass, different name: the class+subclass profile applies.
  EmitDevicePan(application, Device::Class::POINTER, Device::Subclass::REMOCON, Dali::String("Other Remote"), 100u, vertical);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // The named model: the name profile wins over the class+subclass profile.
  EmitDevicePan(application, Device::Class::POINTER, Device::Subclass::REMOCON, REMOTE_NAME, 5000u, vertical);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Remove the name profile: the named model falls back to the class+subclass profile.
  detector.ClearDeviceOptions(GestureDeviceSelector::ByDeviceName(REMOTE_NAME));
  EmitDevicePan(application, Device::Class::POINTER, Device::Subclass::REMOCON, REMOTE_NAME, 10000u, vertical);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureDetectorDeviceOptionsKeptForGestureInProgress(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  const Vector2 start(20.0f, 20.0f);
  const Vector2 step(20.0f, 0.0f);
  auto          touch = [&](uint32_t time, PointState::Type state, const Vector2& position)
  {
    application.ProcessEvent(GenerateDeviceTouch(Device::Class::TOUCH, Device::Subclass::FINGER, Dali::String(""), time, state, position));
    application.SendNotification();
  };

  // Start a one-finger pan with the default profile.
  touch(100u, PointState::DOWN, start);
  touch(116u, PointState::MOTION, start + step);
  touch(132u, PointState::MOTION, start + step * 2.0f);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(data.receivedGesture.GetState(), GestureState::STARTED, TEST_LOCATION);
  data.Reset();

  // Register a touch profile requiring two fingers while the one-finger pan is in progress.
  PanGestureDetector::Options touchOptions = detector.GetDefaultOptions();
  touchOptions.SetMinimumTouchesRequired(2u);
  touchOptions.SetMaximumTouchesRequired(2u);
  detector.SetDeviceOptions(GestureDeviceSelector::ByDeviceClass(Device::Class::TOUCH), touchOptions);

  // The gesture in progress keeps the profile it started with.
  touch(148u, PointState::MOTION, start + step * 3.0f);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(data.receivedGesture.GetState(), GestureState::CONTINUING, TEST_LOCATION);
  data.Reset();
  touch(164u, PointState::UP, start + step * 3.0f);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(data.receivedGesture.GetState(), GestureState::FINISHED, TEST_LOCATION);
  data.Reset();

  // The next one-finger touch pan uses the new profile and is rejected.
  EmitDevicePan(application, Device::Class::TOUCH, Device::Subclass::FINGER, Dali::String(""), 5000u);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureDetectorDeviceOptionsHandleEvent(void)
{
  TestApplication          application;
  Dali::Integration::Scene scene = application.GetScene();
  Dali::RenderTask         task  = scene.GetRenderTaskList().GetTask(0);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  scene.Add(actor);
  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  PanGestureDetector detector = PanGestureDetector::New();
  detector.DetectedSignal().Connect(&application, functor);

  // Geometry path: pointing devices may only pan horizontally.
  PanGestureDetector::Options pointerOptions = detector.GetDefaultOptions();
  pointerOptions.AddDirection(PanGestureDetector::DIRECTION_HORIZONTAL);
  detector.SetDeviceOptions(GestureDeviceSelector::ByDeviceClass(Device::Class::POINTER), pointerOptions);

  auto feedPan = [&](Device::Class::Type deviceClass, uint32_t startTime, const Vector2& displacement)
  {
    const Vector2 start(50.0f, 50.0f);
    auto          feed = [&](PointState::Type state, const Vector2& position, uint32_t time)
    {
      Dali::Integration::TouchEvent tp = GenerateDeviceTouch(deviceClass, Device::Subclass::NONE, Dali::String(""), time, state, position);
      Internal::TouchEventPtr       touchEventImpl(new Internal::TouchEvent(time));
      touchEventImpl->AddPoint(tp.GetPoint(0));
      touchEventImpl->SetRenderTask(task);
      Dali::TouchEvent touchEventHandle(touchEventImpl.Get());
      detector.HandleEvent(actor, touchEventHandle);
    };
    feed(PointState::DOWN, start, startTime);
    feed(PointState::MOTION, start + displacement * 0.5f, startTime + 50u);
    feed(PointState::MOTION, start + displacement, startTime + 100u);
    feed(PointState::UP, start + displacement, startTime + 150u);
  };

  const Vector2 vertical(0.0f, 40.0f);
  feedPan(Device::Class::POINTER, 100u, vertical);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  feedPan(Device::Class::TOUCH, 5000u, vertical);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  feedPan(Device::Class::POINTER, 10000u, Vector2(40.0f, 0.0f));
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

namespace
{
void CheckPanDeviceTouchRange(bool belowMinimum, bool handleEvent)
{
  TestApplication application;
  application.GetScene().SetGeometryHittestEnabled(true);
  Dali::RenderTask task = application.GetScene().GetRenderTaskList().GetTask(0);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render();

  PanGestureDetector detector = PanGestureDetector::New();
  if(!handleEvent)
  {
    detector.Attach(actor);
  }

  // Keep the recognizer's envelope at 1..2, while the active touch profile accepts only one count.
  detector.SetMaximumTouchesRequired(2u);
  PanGestureDetector::Options options = detector.GetDefaultOptions();
  options.SetMinimumTouchesRequired(belowMinimum ? 2u : 1u);
  options.SetMaximumTouchesRequired(belowMinimum ? 2u : 1u);
  detector.SetDeviceOptions(GestureDeviceSelector::ByDeviceClass(Device::Class::TOUCH), options);

  std::vector<PanGesture> gestures;
  detector.DetectedSignal().Connect(&application, [&](Actor, PanGesture pan)
  {
    gestures.push_back(pan);
  });

  uint32_t time = 1000u;
  auto     feed = [&](PointState::Type state, float x, PointState::Type secondState = PointState::INTERRUPTED)
  {
    const Integration::TouchEvent event = GenerateDeviceTouch(Device::Class::TOUCH, Device::Subclass::FINGER, Dali::String(""), time, state, Vector2(x, 20.0f), secondState, Vector2(x, 60.0f));
    time += 16u;
    if(handleEvent)
    {
      Internal::TouchEventPtr touch(new Internal::TouchEvent(event.time));
      for(const Integration::Point& point : event.points)
      {
        touch->AddPoint(point);
      }
      touch->SetRenderTask(task);
      Dali::TouchEvent touchHandle(touch.Get());
      detector.HandleEvent(actor, touchHandle);
    }
    else
    {
      application.ProcessEvent(event);
    }
    application.SendNotification();
  };

  const PointState::Type secondMotion = belowMinimum ? PointState::MOTION : PointState::INTERRUPTED;
  feed(PointState::DOWN, 20.0f);
  if(belowMinimum)
  {
    feed(PointState::STATIONARY, 20.0f, PointState::DOWN);
  }
  feed(PointState::MOTION, 40.0f, secondMotion);
  feed(PointState::MOTION, 60.0f, secondMotion);
  DALI_TEST_EQUALS(gestures.size(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(gestures.back().GetState(), GestureState::STARTED, TEST_LOCATION);

  feed(PointState::STATIONARY, 60.0f, belowMinimum ? PointState::UP : PointState::DOWN);
  feed(PointState::MOTION, 80.0f, belowMinimum ? PointState::INTERRUPTED : PointState::MOTION);
  DALI_TEST_EQUALS(gestures.size(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(gestures.back().GetState(), GestureState::FINISHED, TEST_LOCATION);
  DALI_TEST_EQUALS(gestures.back().GetNumberOfTouches(), belowMinimum ? 1u : 2u, TEST_LOCATION);

  // Returning to the accepted range must not resume this pan or emit another FINISHED on release.
  feed(PointState::STATIONARY, 80.0f, belowMinimum ? PointState::DOWN : PointState::UP);
  feed(PointState::MOTION, 100.0f, secondMotion);
  feed(PointState::UP, 100.0f, belowMinimum ? PointState::UP : PointState::INTERRUPTED);
  DALI_TEST_EQUALS(gestures.size(), 2u, TEST_LOCATION);
}
} // namespace

int UtcDaliPanGestureDetectorDeviceOptionsFinishBelowMinimumTouches(void)
{
  CheckPanDeviceTouchRange(true, false);
  CheckPanDeviceTouchRange(true, true);
  END_TEST;
}

int UtcDaliPanGestureDetectorDeviceOptionsFinishAboveMaximumTouches(void)
{
  CheckPanDeviceTouchRange(false, false);
  CheckPanDeviceTouchRange(false, true);
  END_TEST;
}
