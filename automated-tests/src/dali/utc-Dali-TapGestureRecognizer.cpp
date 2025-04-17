/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

    tappedActor.Reset();
  }

  bool       functorCalled;
  bool       voidFunctorCalled;
  TapGesture receivedGesture;
  Actor      tappedActor;
};

// Functor that sets the data when called
struct GestureReceivedFunctor
{
  GestureReceivedFunctor(SignalData& data)
  : signalData(data)
  {
  }

  void operator()(Actor actor, const TapGesture& tap)
  {
    signalData.functorCalled   = true;
    signalData.receivedGesture = tap;
    signalData.tappedActor     = actor;
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

Integration::TouchEvent GenerateDoubleTouch(PointState::Type state, const Vector2& screenPositionA, const Vector2& screenPositionB, uint32_t time)
{
  Integration::TouchEvent touchEvent;
  Integration::Point      point;
  point.SetState(state);
  point.SetScreenPosition(screenPositionA);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);
  point.SetScreenPosition(screenPositionB);
  touchEvent.points.push_back(point);
  touchEvent.time = time;
  return touchEvent;
}

} // namespace

///////////////////////////////////////////////////////////////////////////////

int UtcDaliTapGestureRecognizerBasic(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();

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

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureRecognizerGapTooLong(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();

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

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 20.0f), 651));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureRecognizerInterrupted(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();

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

  application.ProcessEvent(GenerateSingleTouch(PointState::INTERRUPTED, Vector2(20.0f, 20.0f), 175));

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 20.0f), 200));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureRecognizerMoveTooFar(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();
  detector.SetMaximumTapsRequired(2);

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

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(50.0f, 20.0f), 200));

  application.SendNotification();

  application.GetPlatform().TriggerTimer();
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureRecognizerStartDouble(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();

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

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), Vector2(25.0f, 25.0f), 150));

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 20.0f), 200));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureRecognizerEndDouble(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();

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

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), Vector2(25.0f, 25.0f), 200));

  application.ProcessEvent(GenerateDoubleTouch(PointState::UP, Vector2(20.0f, 20.0f), Vector2(25.0f, 25.0f), 200));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureRecognizerDoubleTap(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New(2);

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

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 250));

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 20.0f), 300));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureRecognizerDoubleTapMoveTooFar(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New(2);

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

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 50.0f), 250));

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 50.0f), 300));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(50.0f, 50.0f), 450));

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(50.0f, 50.0f), 500));

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(50.0f, 50.0f), 550));

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(50.0f, 50.0f), 600));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureRecognizerDoubleTapWaitTooLong(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New(2);

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

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 750));

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 20.0f), 850));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 900));

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 20.0f), 1450));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(50.0f, 50.0f), 1500));

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(50.0f, 50.0f), 1550));

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(50.0f, 50.0f), 1600));

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(50.0f, 50.0f), 1650));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureRecognizerMultipleDetectors(void)
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

  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);

  TapGestureDetector detector2 = TapGestureDetector::New(2);
  detector2.Attach(actor2);

  SignalData             data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  SignalData             data2;
  GestureReceivedFunctor functor2(data2);
  detector2.DetectedSignal().Connect(&application, functor2);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 150));

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 20.0f), 200));

  application.SendNotification();

  application.GetPlatform().TriggerTimer();
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, actor == data.tappedActor, TEST_LOCATION);
  data.Reset();
  DALI_TEST_EQUALS(false, data2.functorCalled, TEST_LOCATION);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(120.0f, 20.0f), 250));

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(120.0f, 20.0f), 300));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data2.functorCalled, TEST_LOCATION);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(120.0f, 20.0f), 350));

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(120.0f, 20.0f), 400));

  application.SendNotification();

  application.GetPlatform().TriggerTimer();
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, data2.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureRecognizerTripleTap(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New(3);

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

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 250));

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 20.0f), 300));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 350));

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 20.0f), 400));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureSetMaximumAllowedTime(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New(2);

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  detector.Attach(actor);

  try
  {
    Integration::SetTapMaximumAllowedTime(0);
  }
  catch(...)
  {
    DALI_TEST_CHECK(false); // Should not get here
  }

  // Reduce the maximum allowable time. 500 -> 100
  Integration::SetTapMaximumAllowedTime(100);

  SignalData             data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 150));

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 20.0f), 200));

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 310));

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 20.0f), 350));

  application.SendNotification();

  // The double tap fails because the maximum allowed time has been exceeded
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // reset maximum allowed time
  Integration::SetTapMaximumAllowedTime(500);

  END_TEST;
}

int UtcDaliTapGestureSetRecognizerTime(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  detector.Attach(actor);

  try
  {
    Integration::SetTapRecognizerTime(0);
  }
  catch(...)
  {
    DALI_TEST_CHECK(false); // Should not get here
  }

  // Reduce the recognizer time. 500 -> 100
  Integration::SetTapRecognizerTime(100);

  SignalData             data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 150));

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 20.0f), 200));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 300));

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(20.0f, 20.0f), 450));

  application.SendNotification();

  // The tap fails because the recognizer time has been exceeded
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // reset recognizer time
  Integration::SetTapRecognizerTime(500);

  END_TEST;
}

int UtcDaliTapGestureSetMaximumMotionAllowedDistance(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();

  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  application.GetScene().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  detector.Attach(actor);

  try
  {
    Integration::SetTapMaximumMotionAllowedDistance(-1.0f);
  }
  catch(...)
  {
    DALI_TEST_CHECK(false); // Should not get here
  }

  // increase the distance. 20 -> 50
  Integration::SetTapMaximumMotionAllowedDistance(50);

  SignalData             data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 150));

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(60.0f, 60.0f), 200));

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 300));

  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(80.0f, 80.0f), 450));

  application.SendNotification();

  // The tap fails because the distance has been exceeded
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // reset distance
  Integration::SetTapMaximumMotionAllowedDistance(20);

  END_TEST;
}
