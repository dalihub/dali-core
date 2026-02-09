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
#include <dali/integration-api/input-options.h>
#include <dali/integration-api/render-task-list-integ.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>

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

    pinchedActor.Reset();
  }

  bool         functorCalled;
  bool         voidFunctorCalled;
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

int UtcDaliPinchGestureRecognizerBasicNoAction(void)
{
  TestApplication application;

  PinchGestureDetector detector = PinchGestureDetector::New();

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

int UtcDaliPinchGestureRecognizerBasic(void)
{
  TestApplication application;

  PinchGestureDetector detector = PinchGestureDetector::New();

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

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), PointState::DOWN, Vector2(20.0f, 90.0f), 150));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 28.0f), PointState::MOTION, Vector2(20.0f, 82.0f), 160));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 37.0f), PointState::MOTION, Vector2(20.0f, 74.0f), 170));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 46.0f), PointState::MOTION, Vector2(20.0f, 66.0f), 180));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 55.0f), PointState::MOTION, Vector2(20.0f, 58.0f), 190));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPinchGestureRecognizerEndEarly01(void)
{
  TestApplication application;

  PinchGestureDetector detector = PinchGestureDetector::New();

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

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), PointState::DOWN, Vector2(20.0f, 90.0f), 150));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 28.0f), PointState::MOTION, Vector2(20.0f, 82.0f), 160));
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 29.0f), 165));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 37.0f), PointState::MOTION, Vector2(20.0f, 74.0f), 170));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 46.0f), PointState::MOTION, Vector2(20.0f, 66.0f), 180));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 55.0f), PointState::MOTION, Vector2(20.0f, 58.0f), 190));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPinchGestureRecognizerEndEarly02(void)
{
  TestApplication application;

  PinchGestureDetector detector = PinchGestureDetector::New();

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

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), PointState::DOWN, Vector2(20.0f, 90.0f), 150));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 28.0f), PointState::MOTION, Vector2(20.0f, 82.0f), 160));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 37.0f), PointState::MOTION, Vector2(20.0f, 74.0f), 170));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 37.0f), PointState::UP, Vector2(20.0f, 74.0f), 173));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 37.0f), PointState::DOWN, Vector2(20.0f, 74.0f), 178));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 46.0f), PointState::MOTION, Vector2(20.0f, 66.0f), 180));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 55.0f), PointState::MOTION, Vector2(20.0f, 58.0f), 190));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPinchGestureRecognizerRealistic01(void)
{
  TestApplication application;

  PinchGestureDetector detector = PinchGestureDetector::New();

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
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 56.0f), PointState::UP, Vector2(20.0f, 60.0f), 150));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 56.0f), 155));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPinchGestureRecognizerRealistic02(void)
{
  TestApplication application;

  PinchGestureDetector detector = PinchGestureDetector::New();

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

int UtcDaliPinchGestureRecognizerRealistic03(void)
{
  TestApplication application;

  PinchGestureDetector detector = PinchGestureDetector::New();

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
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 37.0f), PointState::MOTION, Vector2(20.0f, 77.0f), 127));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 40.0f), PointState::MOTION, Vector2(20.0f, 75.0f), 130));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 47.0f), PointState::MOTION, Vector2(20.0f, 73.0f), 133));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 45.0f), PointState::MOTION, Vector2(20.0f, 70.0f), 135));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 47.0f), PointState::MOTION, Vector2(20.0f, 67.0f), 137));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 50.0f), PointState::MOTION, Vector2(20.0f, 65.0f), 140));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 53.0f), PointState::MOTION, Vector2(20.0f, 63.0f), 143));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 55.0f), PointState::MOTION, Vector2(20.0f, 60.0f), 145));

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 56.0f), 155));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPinchGestureRecognizerMultipleDetectors(void)
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

  PinchGestureDetector detector = PinchGestureDetector::New();
  detector.Attach(actor);

  PinchGestureDetector detector2 = PinchGestureDetector::New();
  detector2.Attach(actor2);

  SignalData             data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  SignalData             data2;
  GestureReceivedFunctor functor2(data2);
  detector2.DetectedSignal().Connect(&application, functor2);

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(120.0f, 20.0f), PointState::DOWN, Vector2(120.0f, 90.0f), 150));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(120.0f, 28.0f), PointState::MOTION, Vector2(120.0f, 82.0f), 160));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(120.0f, 37.0f), PointState::MOTION, Vector2(120.0f, 74.0f), 170));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(120.0f, 46.0f), PointState::MOTION, Vector2(120.0f, 66.0f), 180));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(120.0f, 55.0f), PointState::MOTION, Vector2(120.0f, 58.0f), 190));
  application.ProcessEvent(GenerateDoubleTouch(PointState::UP, Vector2(120.0f, 55.0f), PointState::UP, Vector2(120.0f, 58.0f), 200));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, data2.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, actor2 == data2.pinchedActor, TEST_LOCATION);
  data2.Reset();

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), PointState::DOWN, Vector2(20.0f, 90.0f), 250));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 28.0f), PointState::MOTION, Vector2(20.0f, 82.0f), 260));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 37.0f), PointState::MOTION, Vector2(20.0f, 74.0f), 270));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 46.0f), PointState::MOTION, Vector2(20.0f, 66.0f), 280));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 55.0f), PointState::MOTION, Vector2(20.0f, 58.0f), 290));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, actor == data.pinchedActor, TEST_LOCATION);
  DALI_TEST_EQUALS(false, data2.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPinchGestureRecognizerShortDistance01(void)
{
  TestApplication application;

  Dali::Integration::SetPinchGestureMinimumDistance(7.0f);

  PinchGestureDetector detector = PinchGestureDetector::New();

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

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), PointState::DOWN, Vector2(20.0f, 90.0f), 150));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 28.0f), PointState::MOTION, Vector2(20.0f, 82.0f), 160));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 30.0f), PointState::MOTION, Vector2(20.0f, 80.0f), 170));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 32.0f), PointState::MOTION, Vector2(20.0f, 78.0f), 180));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 34.0f), PointState::MOTION, Vector2(20.0f, 76.0f), 190));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPinchGestureRecognizerShortDistance02(void)
{
  TestApplication application;

  PinchGestureDetector detector = PinchGestureDetector::New();

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

  Dali::Integration::SetPinchGestureMinimumDistance(7.0f);

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), PointState::DOWN, Vector2(20.0f, 90.0f), 150));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 28.0f), PointState::MOTION, Vector2(20.0f, 82.0f), 160));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 30.0f), PointState::MOTION, Vector2(20.0f, 80.0f), 170));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 32.0f), PointState::MOTION, Vector2(20.0f, 78.0f), 180));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 34.0f), PointState::MOTION, Vector2(20.0f, 76.0f), 190));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPinchGestureRecognizerLongDistance01(void)
{
  TestApplication application;

  Dali::Integration::SetPinchGestureMinimumDistance(14.0f);

  PinchGestureDetector detector = PinchGestureDetector::New();

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

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), PointState::DOWN, Vector2(20.0f, 90.0f), 150));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 22.0f), PointState::MOTION, Vector2(20.0f, 88.0f), 160));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 24.0f), PointState::MOTION, Vector2(20.0f, 86.0f), 170));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 26.0f), PointState::MOTION, Vector2(20.0f, 84.0f), 180));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPinchGestureRecognizerLongDistance02(void)
{
  TestApplication application;

  Dali::Integration::SetPinchGestureMinimumDistance(14.0f);

  PinchGestureDetector detector = PinchGestureDetector::New();

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

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), PointState::DOWN, Vector2(20.0f, 90.0f), 150));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 38.0f), PointState::MOTION, Vector2(20.0f, 72.0f), 160));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 40.0f), PointState::MOTION, Vector2(20.0f, 70.0f), 170));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 42.0f), PointState::MOTION, Vector2(20.0f, 68.0f), 180));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 44.0f), PointState::MOTION, Vector2(20.0f, 66.0f), 190));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPinchGestureRecognizerLongDistance03(void)
{
  TestApplication application;

  PinchGestureDetector detector = PinchGestureDetector::New();

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

  Dali::Integration::SetPinchGestureMinimumDistance(14.0f);

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), PointState::DOWN, Vector2(20.0f, 90.0f), 150));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 22.0f), PointState::MOTION, Vector2(20.0f, 88.0f), 160));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 24.0f), PointState::MOTION, Vector2(20.0f, 86.0f), 170));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 26.0f), PointState::MOTION, Vector2(20.0f, 84.0f), 180));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPinchGestureRecognizerLongDistance04(void)
{
  TestApplication application;

  PinchGestureDetector detector = PinchGestureDetector::New();

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

  Dali::Integration::SetPinchGestureMinimumDistance(14.0f);

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), PointState::DOWN, Vector2(20.0f, 90.0f), 150));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 38.0f), PointState::MOTION, Vector2(20.0f, 72.0f), 160));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 40.0f), PointState::MOTION, Vector2(20.0f, 70.0f), 170));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 42.0f), PointState::MOTION, Vector2(20.0f, 68.0f), 180));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 44.0f), PointState::MOTION, Vector2(20.0f, 66.0f), 190));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPinchGestureRecognizerMinimumTouchEvents(void)
{
  TestApplication application;

  PinchGestureDetector detector = PinchGestureDetector::New();

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

  // Case 1
  // 2 touch events make a gesture begin
  Dali::Integration::SetPinchGestureMinimumTouchEvents(2);
  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), PointState::DOWN, Vector2(20.0f, 90.0f), 150));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 20.0f), PointState::MOTION, Vector2(90.0f, 90.0f), 160));

  DALI_TEST_EQUALS(GestureState::STARTED, data.receivedGesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Case 2
  // 4 touch events make a gesture begin
  Dali::Integration::SetPinchGestureMinimumTouchEvents(4);
  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), PointState::DOWN, Vector2(20.0f, 90.0f), 150));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 20.0f), PointState::MOTION, Vector2(90.0f, 90.0f), 160));

  // Check the gesture is not detected unlike previous case
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPinchGestureRecognizerMinimumTouchEventsAfterStart(void)
{
  TestApplication application;

  PinchGestureDetector detector = PinchGestureDetector::New();

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

  // Case 1
  // > 2 touch events make a gesture begin
  // > 4 touch events generate gestures after begin
  Dali::Integration::SetPinchGestureMinimumTouchEvents(2);
  Dali::Integration::SetPinchGestureMinimumTouchEventsAfterStart(6);

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
