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
#include <dali/integration-api/render-task-list-integ.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <chrono>
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

    pannedActor.Reset();
  }

  bool       functorCalled;
  bool       voidFunctorCalled;
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

  void operator()(Actor actor, const PanGesture& pan)
  {
    signalData.functorCalled   = true;
    signalData.receivedGesture = pan;
    signalData.pannedActor     = actor;
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

uint32_t GetMilliSeconds()
{
  // Get the time of a monotonic clock since its epoch.
  auto epoch = std::chrono::steady_clock::now().time_since_epoch();

  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);

  return static_cast<uint32_t>(duration.count());
}
} // namespace

///////////////////////////////////////////////////////////////////////////////

int UtcDaliPanGestureRecognizerBasicNoAction(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

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

int UtcDaliPanGestureRecognizerBasic(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

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
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 40.0f), 151));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 60.0f), 152));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerBasicInterrupted(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

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
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 25.0f), 151));
  application.ProcessEvent(GenerateSingleTouch(PointState::INTERRUPTED, Vector2(20.0f, 30.0f), 152));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerBasicShortest(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

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
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 40.0f), 151));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 40.0f), 155));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerBasicFailToStart(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

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
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 40.0f), 151));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(20.0f, 50.0f), PointState::DOWN, Vector2(40.0f, 40.0f), 153));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerBasicStationary(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

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
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 40.0f), 151));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 50.0f), 152));
  application.ProcessEvent(GenerateSingleTouch(PointState::STATIONARY, Vector2(20.0f, 50.0f), 153));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 55.0f), 154));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerNewParametersFail(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  detector.SetMaximumTouchesRequired(2);
  detector.SetMinimumTouchesRequired(2);

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
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 40.0f), 151));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 60.0f), 152));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerNewParametersSuccess(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  detector.SetMaximumTouchesRequired(2);
  detector.SetMinimumTouchesRequired(2);

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

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 50.0f), PointState::DOWN, Vector2(20.0f, 40.0f), 150));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(40.0f, 50.0f), PointState::MOTION, Vector2(40.0f, 40.0f), 151));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(50.0f, 50.0f), PointState::MOTION, Vector2(50.0f, 40.0f), 152));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerNewParametersEndFewerTouches01(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  detector.SetMaximumTouchesRequired(2);
  detector.SetMinimumTouchesRequired(2);

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

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 50.0f), PointState::DOWN, Vector2(20.0f, 40.0f), 150));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(40.0f, 50.0f), PointState::MOTION, Vector2(40.0f, 40.0f), 151));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(50.0f, 50.0f), PointState::MOTION, Vector2(50.0f, 40.0f), 152));
  application.ProcessEvent(GenerateSingleTouch(PointState::STATIONARY, Vector2(50.0f, 50.0f), 153));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerNewParametersEndFewerTouches02(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  detector.SetMaximumTouchesRequired(2);
  detector.SetMinimumTouchesRequired(2);

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

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 50.0f), PointState::DOWN, Vector2(20.0f, 40.0f), 150));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(40.0f, 50.0f), PointState::MOTION, Vector2(40.0f, 40.0f), 151));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(50.0f, 50.0f), PointState::MOTION, Vector2(50.0f, 40.0f), 152));
  application.ProcessEvent(GenerateDoubleTouch(PointState::STATIONARY, Vector2(50.0f, 50.0f), PointState::UP, Vector2(50.0f, 40.0f), 153));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerNewParametersNoStart(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  detector.SetMaximumTouchesRequired(2);
  detector.SetMinimumTouchesRequired(2);

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

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 50.0f), PointState::DOWN, Vector2(20.0f, 40.0f), 150));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(40.0f, 50.0f), PointState::MOTION, Vector2(40.0f, 40.0f), 151));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(50.0f, 50.0f), 153));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerNewParametersSlowRelease(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  detector.SetMaximumTouchesRequired(2);
  detector.SetMinimumTouchesRequired(2);

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

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 50.0f), PointState::DOWN, Vector2(20.0f, 40.0f), 150));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(40.0f, 50.0f), PointState::MOTION, Vector2(40.0f, 40.0f), 151));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(50.0f, 50.0f), PointState::MOTION, Vector2(50.0f, 40.0f), 152));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(60.0f, 50.0f), PointState::MOTION, Vector2(60.0f, 40.0f), 153));
  application.ProcessEvent(GenerateDoubleTouch(PointState::MOTION, Vector2(70.0f, 50.0f), PointState::MOTION, Vector2(70.0f, 40.0f), 154));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(70.0f, 50.0f), 155));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerNewParamsMaxMotionEventAge(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  detector.SetMaximumMotionEventAge(1000);

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

  uint32_t currentTime = GetMilliSeconds();
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), currentTime));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 40.0f), currentTime));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 60.0f), currentTime));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 70.0f), currentTime));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 80.0f), currentTime));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 90.0f), currentTime));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  data.Reset();

  // Update current time
  currentTime = GetMilliSeconds();

  tet_infoline("Test fast enough motion\n");
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 80.0f), currentTime - 100));

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  data.Reset();

  // Update current time
  currentTime = GetMilliSeconds();

  tet_infoline("Test super heavy motion\n");
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 80.0f), currentTime - 10000));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 70.0f), currentTime - 9000));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 60.0f), currentTime - 8000));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 50.0f), currentTime - 7000));

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerOtherEvent(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

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
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 40.0f), 151));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 60.0f), 152));
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 60.0f), 153)); // Exercise default case in STARTED case. Not sure if realistic
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 65.0f), 154));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerSlowMoving(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

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
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 40.0f), 251));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 60.0f), 352));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 70.0f), 453));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 80.0f), 554));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 90.0f), 655));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerNewParamsMinNum(void)
{
  TestApplication application;

  Integration::SetPanGestureMinimumPanEvents(8);

  PanGestureDetector detector = PanGestureDetector::New();

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
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 40.0f), 251));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 60.0f), 352));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 70.0f), 453));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 80.0f), 554));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 90.0f), 655));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerNewParamsMinDistance(void)
{
  TestApplication application;

  Integration::SetPanGestureMinimumDistance(100);

  PanGestureDetector detector = PanGestureDetector::New();

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
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 40.0f), 251));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 60.0f), 352));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 70.0f), 453));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 80.0f), 554));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 90.0f), 655));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}
