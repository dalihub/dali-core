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
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/input-options.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>

using namespace Dali;

void utc_dali_rotation_gesture_recognizer_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_rotation_gesture_recognizer_cleanup(void)
{
  test_return_value = TET_PASS;
}

///////////////////////////////////////////////////////////////////////////////
namespace
{
struct SignalData
{
  SignalData()
  : functorCalled(false),
    voidFunctorCalled(false),
    receivedGesture()
  {
  }

  void Reset()
  {
    functorCalled     = false;
    voidFunctorCalled = false;

    receivedGesture.Reset();

    rotatedActor.Reset();
  }

  bool            functorCalled;
  bool            voidFunctorCalled;
  RotationGesture receivedGesture;
  Actor           rotatedActor;
};

// Functor that sets the data when called
struct GestureReceivedFunctor
{
  GestureReceivedFunctor(SignalData& data)
  : signalData(data)
  {
  }

  void operator()(Actor actor, const RotationGesture& rotation)
  {
    signalData.functorCalled   = true;
    signalData.receivedGesture = rotation;
    signalData.rotatedActor    = actor;
  }

  void operator()()
  {
    signalData.voidFunctorCalled = true;
  }

  SignalData& signalData;
};

Integration::TouchEvent GenerateSingleTouch(PointState::Type state, const Vector2& screenPosition, uint32_t time)
{
  Integration::TouchEvent touchEvent;
  Integration::Point      point;
  point.SetState(state);
  point.SetScreenPosition(screenPosition);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);
  touchEvent.time = time;
  return touchEvent;
}

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
int UtcDaliRotationGestureRecognizerRealistic(void)
{
  TestApplication application;

  RotationGestureDetector detector = RotationGestureDetector::New();

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  detector.Attach(actor);

  SignalData             data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 100));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 20.0f), 105));
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 110));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 25.0f), PointState::DOWN, Vector2(20.0f, 90.0f), 115));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 30.0f), PointState::MOTION, Vector2(20.0f, 85.0f), 120));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 35.0f), PointState::MOTION, Vector2(20.0f, 80.0f), 125));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 40.0f), PointState::MOTION, Vector2(20.0f, 75.0f), 130));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 45.0f), PointState::MOTION, Vector2(20.0f, 70.0f), 135));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 50.0f), PointState::MOTION, Vector2(20.0f, 65.0f), 140));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 55.0f), PointState::MOTION, Vector2(20.0f, 60.0f), 145));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 56.0f), 155));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRotationGestureRecognizerBasicInterrupted(void)
{
  TestApplication application;

  RotationGestureDetector detector = RotationGestureDetector::New();

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  detector.Attach(actor);

  SignalData             data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  application.ProcessEvent(GenerateSingleTouch(PointState::INTERRUPTED, Vector2(20.0f, 30.0f), 152));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRotationGestureRecognizerMinimumTouchEvents(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  RotationGestureDetector detector = RotationGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Case 1
  // 2 touch events make a gesture begin
  Integration::SetRotationGestureMinimumTouchEvents(2);
  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), PointState::DOWN, Vector2(20.0f, 90.0f), 150));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 20.0f), PointState::MOTION, Vector2(90.0f, 90.0f), 160));

  DALI_TEST_EQUALS(GestureState::STARTED, data.receivedGesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Case 2
  // 4 touch events make a gesture begin
  Integration::SetRotationGestureMinimumTouchEvents(4);
  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), PointState::DOWN, Vector2(20.0f, 90.0f), 150));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 20.0f), PointState::MOTION, Vector2(90.0f, 90.0f), 160));

  // Check the gesture is not detected unlike previous case
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRotationGestureRecognizerMinimumTouchEventsAfterStart(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  application.SendNotification();
  application.Render();

  SignalData             data;
  GestureReceivedFunctor functor(data);

  RotationGestureDetector detector = RotationGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Case 1
  // > 2 touch events make a gesture begin
  // > 4 touch events generate gestures after begin
  Integration::SetRotationGestureMinimumTouchEvents(2);
  Integration::SetRotationGestureMinimumTouchEventsAfterStart(6);

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), PointState::DOWN, Vector2(20.0f, 90.0f), 150));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 20.0f), PointState::MOTION, Vector2(90.0f, 90.0f), 160));

  DALI_TEST_EQUALS(GestureState::STARTED, data.receivedGesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 20.0f), PointState::MOTION, Vector2(20.0f, 90.0f), 170));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 20.0f), PointState::MOTION, Vector2(20.0f, 90.0f), 180));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 20.0f), PointState::MOTION, Vector2(20.0f, 90.0f), 190));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 20.0f), PointState::MOTION, Vector2(20.0f, 90.0f), 200));
  // > Test : not enough touch events to make the gesture state "CONTINUING"
  DALI_TEST_EQUALS(GestureState::STARTED, data.receivedGesture.GetState(), TEST_LOCATION);

  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 20.0f), PointState::MOTION, Vector2(20.0f, 90.0f), 210));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 20.0f), PointState::MOTION, Vector2(20.0f, 90.0f), 220));
  // > Test : 6 touch events after start make the gesture state "CONTINUING"
  DALI_TEST_EQUALS(GestureState::CONTINUING, data.receivedGesture.GetState(), TEST_LOCATION);

  END_TEST;
}
