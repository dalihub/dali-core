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
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/render-task-list-integ.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <chrono>
#include <iostream>
#include <thread>

using namespace Dali;

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

    pressedActor.Reset();
  }

  bool             functorCalled;
  bool             voidFunctorCalled;
  LongPressGesture receivedGesture;
  Actor            pressedActor;
};

// Functor that sets the data when called
struct GestureReceivedFunctor
{
  GestureReceivedFunctor(SignalData& data)
  : signalData(data)
  {
  }

  void operator()(Actor actor, const LongPressGesture& pan)
  {
    signalData.functorCalled   = true;
    signalData.receivedGesture = pan;
    signalData.pressedActor    = actor;
  }

  void operator()()
  {
    signalData.voidFunctorCalled = true;
  }

  SignalData& signalData;
};

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

Dali::Integration::TouchEvent GenerateDoubleTouch(PointState::Type stateA, const Vector2& screenPositionA, PointState::Type stateB, const Vector2& screenPositionB, uint32_t time)
{
  Dali::Integration::TouchEvent touchEvent;
  Dali::Integration::Point      point;
  point.SetState(stateA);
  point.SetDeviceId(4);
  point.SetScreenPosition(screenPositionA);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);
  point.SetScreenPosition(screenPositionB);
  point.SetState(stateB);
  point.SetDeviceId(7);
  touchEvent.points.push_back(point);
  touchEvent.time = time;
  return touchEvent;
}

} // namespace

///////////////////////////////////////////////////////////////////////////////

int UtcDaliLongPressGestureRecognizerBasicNoAction(void)
{
  TestApplication application;

  LongPressGestureDetector detector = LongPressGestureDetector::New();

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

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 150));

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 20.0f), 200));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLongPressGestureRecognizerBasic(void)
{
  TestApplication application;

  LongPressGestureDetector detector = LongPressGestureDetector::New();

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

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 150));

  application.GetPlatform().TriggerTimer();

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLongPressGestureRecognizerTooShortWait(void)
{
  TestApplication application;

  LongPressGestureDetector detector = LongPressGestureDetector::New();

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

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 150));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLongPressGestureRecognizerTooFewPoints(void)
{
  TestApplication application;

  LongPressGestureDetector detector = LongPressGestureDetector::New();

  detector.SetTouchesRequired(2, 2);

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

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 150));

  // There should be no function to call
  application.GetPlatform().TriggerTimer();

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLongPressGestureRecognizerTooManyPoints(void)
{
  TestApplication application;

  LongPressGestureDetector detector = LongPressGestureDetector::New();

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

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 150));
  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), PointState::DOWN, Vector2(20.0f, 90.0f), 151));

  // There should be no function to call as the double touch should have cancelled it
  application.GetPlatform().TriggerTimer();

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLongPressGestureRecognizerMultiplePointsMoving(void)
{
  TestApplication application;

  LongPressGestureDetector detector = LongPressGestureDetector::New();

  detector.SetTouchesRequired(2, 2);

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

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 0.0f), PointState::DOWN, Vector2(20.0f, 90.0f), 151));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 10.0f), PointState::MOTION, Vector2(20.0f, 80.0f), 153));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 20.0f), PointState::MOTION, Vector2(20.0f, 70.0f), 155));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 30.0f), PointState::MOTION, Vector2(20.0f, 60.0f), 157));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 40.0f), PointState::MOTION, Vector2(20.0f, 50.0f), 159));
  application.ProcessEvent(GenerateDoubleTouch(PointState::STATIONARY, Vector2(20.0f, 40.0f), PointState::UP, Vector2(20.0f, 50.0f), 160));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 40.0f), 161));

  application.GetPlatform().TriggerTimer();

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLongPressGestureRecognizerMultiplePointsLongPress(void)
{
  TestApplication application;

  LongPressGestureDetector detector = LongPressGestureDetector::New();

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  detector.Attach(actor);
  detector.SetTouchesRequired(2, 2); // Set after we've attached forcing us to change things internally

  SignalData             data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 40.0f), 140));
  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 40.0f), PointState::DOWN, Vector2(20.0f, 90.0f), 150));

  application.GetPlatform().TriggerTimer();

  application.ProcessEvent(GenerateDoubleTouch(PointState::STATIONARY, Vector2(20.0f, 20.0f), PointState::UP, Vector2(20.0f, 90.0f), 760));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 20.0f), 761));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLongPressGestureRecognizerMultipleDetectors(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  Actor actor2 = Actor::New();
  actor2.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor2.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor2.SetProperty(Actor::Property::POSITION_X, 100.0f);
  application.GetScene().Add(actor2);

  // Render and notify
  application.SendNotification();
  application.Render();

  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);

  LongPressGestureDetector detector2 = LongPressGestureDetector::New(2);
  detector2.Attach(actor2);

  SignalData             data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  SignalData             data2;
  GestureReceivedFunctor functor2(data2);
  detector2.DetectedSignal().Connect(&application, functor2);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 150));

  application.GetPlatform().TriggerTimer();

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 20.0f), 700));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, actor == data.pressedActor, TEST_LOCATION);
  data.Reset();
  DALI_TEST_EQUALS(false, data2.functorCalled, TEST_LOCATION);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(120.0f, 40.0f), 800));
  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(120.0f, 40.0f), PointState::DOWN, Vector2(120.0f, 90.0f), 805));

  application.GetPlatform().TriggerTimer();

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, data2.functorCalled, TEST_LOCATION);

  END_TEST;
}
