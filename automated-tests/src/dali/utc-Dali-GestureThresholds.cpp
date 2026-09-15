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
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/input-options.h>
#include <dali/internal/event/events/touch-event-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <utility>

using namespace Dali;

void utc_dali_gesture_thresholds_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_gesture_thresholds_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
struct SignalData
{
  SignalData()
  : functorCalled(false)
  {
  }

  void Reset()
  {
    functorCalled = false;
  }

  bool functorCalled;
};

template<typename GestureType>
struct GestureReceivedFunctor
{
  explicit GestureReceivedFunctor(SignalData& data)
  : signalData(data)
  {
  }

  void operator()(Actor /*actor*/, GestureType /*gesture*/)
  {
    signalData.functorCalled = true;
  }

  SignalData& signalData;
};

const Dali::String REMOTE_NAME("Pointing Device");

const GestureDeviceSelector POINTER_SELECTOR = GestureDeviceSelector::ByDeviceClass(Device::Class::POINTER);
const GestureDeviceSelector TOUCH_SELECTOR   = GestureDeviceSelector::ByDeviceClass(Device::Class::TOUCH);
const GestureDeviceSelector FINGER_SELECTOR  = GestureDeviceSelector::ByDeviceClassAndSubclass(Device::Class::TOUCH, Device::Subclass::FINGER);

Dali::Integration::Point MakeDevicePoint(PointState::Type state, const Vector2& position, Device::Class::Type deviceClass, Device::Subclass::Type deviceSubclass, int32_t deviceId)
{
  Dali::Integration::Point point;
  point.SetState(state);
  point.SetDeviceId(deviceId);
  point.SetScreenPosition(position);
  point.SetDeviceClass(deviceClass);
  point.SetDeviceSubclass(deviceSubclass);
  point.SetDeviceName(deviceClass == Device::Class::POINTER ? REMOTE_NAME : Dali::String(""));
  point.SetMouseButton(MouseButton::PRIMARY);
  return point;
}

Dali::Integration::TouchEvent SingleTouch(PointState::Type state, const Vector2& position, Device::Class::Type deviceClass, uint32_t time)
{
  Dali::Integration::TouchEvent touchEvent;
  touchEvent.points.push_back(MakeDevicePoint(state, position, deviceClass, deviceClass == Device::Class::POINTER ? Device::Subclass::REMOCON : Device::Subclass::FINGER, 4));
  touchEvent.time = time;
  return touchEvent;
}

Dali::Integration::TouchEvent DoubleTouch(PointState::Type state, const Vector2& a, const Vector2& b, Device::Subclass::Type deviceSubclass, uint32_t time)
{
  Dali::Integration::TouchEvent touchEvent;
  touchEvent.points.push_back(MakeDevicePoint(state, a, Device::Class::TOUCH, deviceSubclass, 4));
  touchEvent.points.push_back(MakeDevicePoint(state, b, Device::Class::TOUCH, deviceSubclass, 7));
  touchEvent.time = time;
  return touchEvent;
}

/**
 * A single-finger pan of the given horizontal distance through the scene, in three motion steps.
 */
void EmitPan(TestApplication& application, Device::Class::Type deviceClass, float distance, uint32_t startTime)
{
  const Vector2 start(20.0f, 20.0f);
  application.ProcessEvent(SingleTouch(PointState::DOWN, start, deviceClass, startTime));
  application.ProcessEvent(SingleTouch(PointState::MOTION, start + Vector2(distance / 3.0f, 0.0f), deviceClass, startTime + 16u));
  application.ProcessEvent(SingleTouch(PointState::MOTION, start + Vector2(distance * 2.0f / 3.0f, 0.0f), deviceClass, startTime + 32u));
  application.ProcessEvent(SingleTouch(PointState::MOTION, start + Vector2(distance, 0.0f), deviceClass, startTime + 48u));
  application.ProcessEvent(SingleTouch(PointState::UP, start + Vector2(distance, 0.0f), deviceClass, startTime + 64u));
  application.SendNotification();
}

void EmitTap(TestApplication& application, Device::Class::Type deviceClass, uint32_t startTime, uint32_t holdMilliseconds)
{
  application.ProcessEvent(SingleTouch(PointState::DOWN, Vector2(50.0f, 50.0f), deviceClass, startTime));
  application.ProcessEvent(SingleTouch(PointState::UP, Vector2(50.0f, 50.0f), deviceClass, startTime + holdMilliseconds));
  application.SendNotification();
}

/**
 * A two-finger pinch through the scene: DOWN, the given number of MOTION events (fingers closing), UP.
 */
void EmitPinch(TestApplication& application, Device::Subclass::Type deviceSubclass, uint32_t motionEvents, uint32_t startTime)
{
  uint32_t time = startTime;
  application.ProcessEvent(DoubleTouch(PointState::DOWN, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), deviceSubclass, time));
  for(uint32_t i = 0u; i < motionEvents; ++i)
  {
    time += 50u;
    application.ProcessEvent(DoubleTouch(PointState::MOTION, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), deviceSubclass, time));
  }
  application.ProcessEvent(DoubleTouch(PointState::UP, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), deviceSubclass, time + 50u));
  application.SendNotification();
}

/**
 * A two-finger rotation through the scene: DOWN, the given number of MOTION events (quarter turn), UP.
 */
void EmitRotation(TestApplication& application, Device::Subclass::Type deviceSubclass, uint32_t motionEvents, uint32_t startTime)
{
  uint32_t time = startTime;
  application.ProcessEvent(DoubleTouch(PointState::DOWN, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), deviceSubclass, time));
  for(uint32_t i = 0u; i < motionEvents; ++i)
  {
    time += 50u;
    application.ProcessEvent(DoubleTouch(PointState::MOTION, Vector2(20.0f, 2.0f), Vector2(20.0f, 38.0f), deviceSubclass, time));
  }
  application.ProcessEvent(DoubleTouch(PointState::UP, Vector2(20.0f, 2.0f), Vector2(20.0f, 38.0f), deviceSubclass, time + 50u));
  application.SendNotification();
}

Actor AddActor(TestApplication& application)
{
  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 200.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  application.GetScene().Add(actor);
  application.SendNotification();
  application.Render();
  return actor;
}
} // namespace

int UtcDaliGestureThresholdsValueTypesP(void)
{
  TestApplication application;

  // Default-constructed values are the DALi built-in defaults.
  GestureThresholds::PanThresholds pan;
  DALI_TEST_EQUALS(pan.GetMinimumDistance(), Integration::DEFAULT_PAN_GESTURE_MINIMUM_DISTANCE, TEST_LOCATION);
  DALI_TEST_EQUALS(pan.GetMinimumPanEvents(), Integration::DEFAULT_PAN_GESTURE_MINIMUM_PAN_EVENTS, TEST_LOCATION);
  pan.SetMinimumDistance(30);
  pan.SetMinimumPanEvents(7);
  DALI_TEST_EQUALS(pan.GetMinimumDistance(), 30, TEST_LOCATION);
  DALI_TEST_EQUALS(pan.GetMinimumPanEvents(), 7, TEST_LOCATION);

  GestureThresholds::TapThresholds tap;
  DALI_TEST_EQUALS(tap.GetMaximumMultiTapInterval(), Integration::DEFAULT_TAP_GESTURE_MAXIMUM_MULTI_TAP_INTERVAL, TEST_LOCATION);
  DALI_TEST_EQUALS(tap.GetMaximumHoldingTime(), Integration::DEFAULT_TAP_GESTURE_MAXIMUM_HOLDING_TIME, TEST_LOCATION);
  DALI_TEST_EQUALS(tap.GetMaximumMotionDistance(), Integration::DEFAULT_TAP_GESTURE_MAXIMUM_MOTION_DISTANCE, TEST_LOCATION);
  tap.SetMaximumMultiTapInterval(200u);
  tap.SetMaximumHoldingTime(100u);
  tap.SetMaximumMotionDistance(30.0f);
  DALI_TEST_EQUALS(tap.GetMaximumMultiTapInterval(), 200u, TEST_LOCATION);
  DALI_TEST_EQUALS(tap.GetMaximumHoldingTime(), 100u, TEST_LOCATION);
  DALI_TEST_EQUALS(tap.GetMaximumMotionDistance(), 30.0f, TEST_LOCATION);

  GestureThresholds::LongPressThresholds longPress;
  DALI_TEST_EQUALS(longPress.GetMinimumHoldingTime(), Integration::DEFAULT_LONG_PRESS_GESTURE_MINIMUM_HOLDING_TIME, TEST_LOCATION);
  longPress.SetMinimumHoldingTime(350u);
  DALI_TEST_EQUALS(longPress.GetMinimumHoldingTime(), 350u, TEST_LOCATION);

  GestureThresholds::PinchThresholds pinch;
  DALI_TEST_EQUALS(pinch.GetMinimumDistance(), Integration::DEFAULT_PINCH_GESTURE_MINIMUM_DISTANCE, TEST_LOCATION);
  DALI_TEST_EQUALS(pinch.GetMinimumTouchEvents(), Integration::DEFAULT_PINCH_GESTURE_MINIMUM_TOUCH_EVENTS, TEST_LOCATION);
  DALI_TEST_EQUALS(pinch.GetMinimumTouchEventsAfterStart(), Integration::DEFAULT_PINCH_GESTURE_MINIMUM_TOUCH_EVENTS_AFTER_START, TEST_LOCATION);
  pinch.SetMinimumDistance(5.0f);
  pinch.SetMinimumTouchEvents(2u);
  pinch.SetMinimumTouchEventsAfterStart(3u);
  DALI_TEST_EQUALS(pinch.GetMinimumDistance(), 5.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(pinch.GetMinimumTouchEvents(), 2u, TEST_LOCATION);
  DALI_TEST_EQUALS(pinch.GetMinimumTouchEventsAfterStart(), 3u, TEST_LOCATION);

  GestureThresholds::RotationThresholds rotation;
  DALI_TEST_EQUALS(rotation.GetMinimumTouchEvents(), Integration::DEFAULT_ROTATION_GESTURE_MINIMUM_TOUCH_EVENTS, TEST_LOCATION);
  DALI_TEST_EQUALS(rotation.GetMinimumTouchEventsAfterStart(), Integration::DEFAULT_ROTATION_GESTURE_MINIMUM_TOUCH_EVENTS_AFTER_START, TEST_LOCATION);
  rotation.SetMinimumTouchEvents(2u);
  rotation.SetMinimumTouchEventsAfterStart(2u);
  DALI_TEST_EQUALS(rotation.GetMinimumTouchEvents(), 2u, TEST_LOCATION);

  // Copies are independent; moved-from objects assert.
  GestureThresholds::PanThresholds copied(pan);
  copied.SetMinimumDistance(1);
  DALI_TEST_EQUALS(pan.GetMinimumDistance(), 30, TEST_LOCATION);
  GestureThresholds::PanThresholds moved(std::move(copied));
  DALI_TEST_EQUALS(moved.GetMinimumDistance(), 1, TEST_LOCATION);
  DALI_TEST_ASSERTION(copied.GetMinimumDistance(), "moved-from GestureThresholds::PanThresholds");

  END_TEST;
}

int UtcDaliGestureThresholdsGetDefaultReflectsGlobalsP(void)
{
  TestApplication application;

  // The defaults are the values currently in effect, not the compile-time constants.
  Integration::SetPanGestureMinimumDistance(20);
  Integration::SetPanGestureMinimumPanEvents(7);
  GestureThresholds::PanThresholds pan = GestureThresholds::GetDefaultPanThresholds();
  DALI_TEST_EQUALS(pan.GetMinimumDistance(), 20, TEST_LOCATION);
  DALI_TEST_EQUALS(pan.GetMinimumPanEvents(), 7, TEST_LOCATION);

  Integration::SetTapGestureMaximumHoldingTime(100u);
  DALI_TEST_EQUALS(GestureThresholds::GetDefaultTapThresholds().GetMaximumHoldingTime(), 100u, TEST_LOCATION);

  Integration::SetLongPressGestureMinimumHoldingTime(350u);
  DALI_TEST_EQUALS(GestureThresholds::GetDefaultLongPressThresholds().GetMinimumHoldingTime(), 350u, TEST_LOCATION);

  Integration::SetPinchGestureMinimumTouchEvents(2u);
  DALI_TEST_EQUALS(GestureThresholds::GetDefaultPinchThresholds().GetMinimumTouchEvents(), 2u, TEST_LOCATION);

  Integration::SetRotationGestureMinimumTouchEventsAfterStart(3u);
  DALI_TEST_EQUALS(GestureThresholds::GetDefaultRotationThresholds().GetMinimumTouchEventsAfterStart(), 3u, TEST_LOCATION);

  Integration::SetPanGestureMinimumDistance(Integration::DEFAULT_PAN_GESTURE_MINIMUM_DISTANCE);
  Integration::SetPanGestureMinimumPanEvents(Integration::DEFAULT_PAN_GESTURE_MINIMUM_PAN_EVENTS);
  Integration::SetTapGestureMaximumHoldingTime(Integration::DEFAULT_TAP_GESTURE_MAXIMUM_HOLDING_TIME);
  Integration::SetLongPressGestureMinimumHoldingTime(Integration::DEFAULT_LONG_PRESS_GESTURE_MINIMUM_HOLDING_TIME);
  Integration::SetPinchGestureMinimumTouchEvents(Integration::DEFAULT_PINCH_GESTURE_MINIMUM_TOUCH_EVENTS);
  Integration::SetRotationGestureMinimumTouchEventsAfterStart(Integration::DEFAULT_ROTATION_GESTURE_MINIMUM_TOUCH_EVENTS_AFTER_START);

  END_TEST;
}

int UtcDaliGestureThresholdsSetGetClearP(void)
{
  TestApplication application;

  GestureThresholds::PanThresholds queried;
  DALI_TEST_CHECK(!GestureThresholds::GetPanThresholds(POINTER_SELECTOR, queried));

  GestureThresholds::PanThresholds remote = GestureThresholds::GetDefaultPanThresholds();
  remote.SetMinimumDistance(30);
  GestureThresholds::SetPanThresholds(POINTER_SELECTOR, remote);

  DALI_TEST_CHECK(GestureThresholds::GetPanThresholds(POINTER_SELECTOR, queried));
  DALI_TEST_EQUALS(queried.GetMinimumDistance(), 30, TEST_LOCATION);

  // Exact lookup only; a miss leaves the output untouched.
  queried.SetMinimumDistance(99);
  DALI_TEST_CHECK(!GestureThresholds::GetPanThresholds(TOUCH_SELECTOR, queried));
  DALI_TEST_EQUALS(queried.GetMinimumDistance(), 99, TEST_LOCATION);

  // The application-wide values are untouched.
  DALI_TEST_EQUALS(GestureThresholds::GetDefaultPanThresholds().GetMinimumDistance(), Integration::DEFAULT_PAN_GESTURE_MINIMUM_DISTANCE, TEST_LOCATION);

  remote.SetMinimumDistance(40);
  GestureThresholds::SetPanThresholds(POINTER_SELECTOR, remote); // replaces
  DALI_TEST_CHECK(GestureThresholds::GetPanThresholds(POINTER_SELECTOR, queried));
  DALI_TEST_EQUALS(queried.GetMinimumDistance(), 40, TEST_LOCATION);

  GestureThresholds::ClearPanThresholds(POINTER_SELECTOR);
  DALI_TEST_CHECK(!GestureThresholds::GetPanThresholds(POINTER_SELECTOR, queried));
  GestureThresholds::ClearPanThresholds(POINTER_SELECTOR); // no-op

  // The other four gestures follow the same contract.
  GestureThresholds::LongPressThresholds longPress;
  longPress.SetMinimumHoldingTime(350u);
  GestureThresholds::SetLongPressThresholds(POINTER_SELECTOR, longPress);
  GestureThresholds::LongPressThresholds queriedLongPress;
  DALI_TEST_CHECK(GestureThresholds::GetLongPressThresholds(POINTER_SELECTOR, queriedLongPress));
  DALI_TEST_EQUALS(queriedLongPress.GetMinimumHoldingTime(), 350u, TEST_LOCATION);
  GestureThresholds::ClearLongPressThresholds(POINTER_SELECTOR);
  DALI_TEST_CHECK(!GestureThresholds::GetLongPressThresholds(POINTER_SELECTOR, queriedLongPress));

  END_TEST;
}

int UtcDaliGestureThresholdsInvalidN(void)
{
  TestApplication application;

  GestureThresholds::PanThresholds pan;
  pan.SetMinimumDistance(-1);
  DALI_TEST_ASSERTION(GestureThresholds::SetPanThresholds(POINTER_SELECTOR, pan), "minimum distance >= 0");
  pan.SetMinimumDistance(15);
  pan.SetMinimumPanEvents(0);
  DALI_TEST_ASSERTION(GestureThresholds::SetPanThresholds(POINTER_SELECTOR, pan), "minimum pan events >= 1");

  GestureThresholds::TapThresholds tap;
  tap.SetMaximumMultiTapInterval(0u);
  DALI_TEST_ASSERTION(GestureThresholds::SetTapThresholds(POINTER_SELECTOR, tap), "multi tap interval > 0");

  GestureThresholds::LongPressThresholds longPress;
  longPress.SetMinimumHoldingTime(0u);
  DALI_TEST_ASSERTION(GestureThresholds::SetLongPressThresholds(POINTER_SELECTOR, longPress), "holding time > 0");

  GestureThresholds::PinchThresholds pinch;
  pinch.SetMinimumTouchEvents(1u);
  DALI_TEST_ASSERTION(GestureThresholds::SetPinchThresholds(FINGER_SELECTOR, pinch), "minimum touch events > 1");

  GestureThresholds::RotationThresholds rotation;
  rotation.SetMinimumTouchEventsAfterStart(1u);
  DALI_TEST_ASSERTION(GestureThresholds::SetRotationThresholds(FINGER_SELECTOR, rotation), "minimum touch events after start > 1");

  // Nothing was registered by the rejected calls.
  GestureThresholds::PanThresholds queried;
  DALI_TEST_CHECK(!GestureThresholds::GetPanThresholds(POINTER_SELECTOR, queried));

  END_TEST;
}

int UtcDaliGestureThresholdsPanMinimumDistancePerDevice(void)
{
  TestApplication application;
  Actor           actor = AddActor(application);

  SignalData                         data;
  GestureReceivedFunctor<PanGesture> functor(data);
  PanGestureDetector                 detector = PanGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // A 20px pan passes the default 15px minimum distance for any device.
  EmitPan(application, Device::Class::POINTER, 20.0f, 100u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Pointing devices must move 30px before a pan starts.
  GestureThresholds::PanThresholds remote = GestureThresholds::GetDefaultPanThresholds();
  remote.SetMinimumDistance(30);
  GestureThresholds::SetPanThresholds(POINTER_SELECTOR, remote);

  EmitPan(application, Device::Class::POINTER, 20.0f, 5000u);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // Touch screens keep the default.
  EmitPan(application, Device::Class::TOUCH, 20.0f, 10000u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // A longer pointer pan passes.
  EmitPan(application, Device::Class::POINTER, 40.0f, 15000u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Clearing the entry restores the default for pointing devices.
  GestureThresholds::ClearPanThresholds(POINTER_SELECTOR);
  EmitPan(application, Device::Class::POINTER, 20.0f, 20000u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGestureThresholdsPanMinimumDistancePerDeviceHandleEvent(void)
{
  TestApplication          application;
  Dali::Integration::Scene scene = application.GetScene();
  Dali::RenderTask         task  = scene.GetRenderTaskList().GetTask(0);
  Actor                    actor = AddActor(application);

  SignalData                         data;
  GestureReceivedFunctor<PanGesture> functor(data);
  PanGestureDetector                 detector = PanGestureDetector::New();
  detector.DetectedSignal().Connect(&application, functor);

  auto feedPan = [&](Device::Class::Type deviceClass, float distance, uint32_t startTime)
  {
    const Vector2 start(50.0f, 50.0f);
    auto          feed = [&](PointState::Type state, const Vector2& position, uint32_t time)
    {
      Dali::Integration::TouchEvent tp = SingleTouch(state, position, deviceClass, time);
      Internal::TouchEventPtr       touchEventImpl(new Internal::TouchEvent(time));
      touchEventImpl->AddPoint(tp.GetPoint(0));
      touchEventImpl->SetRenderTask(task);
      Dali::TouchEvent touchEventHandle(touchEventImpl.Get());
      detector.HandleEvent(actor, touchEventHandle);
    };
    feed(PointState::DOWN, start, startTime);
    feed(PointState::MOTION, start + Vector2(distance / 3.0f, 0.0f), startTime + 50u);
    feed(PointState::MOTION, start + Vector2(distance * 2.0f / 3.0f, 0.0f), startTime + 100u);
    feed(PointState::MOTION, start + Vector2(distance, 0.0f), startTime + 150u);
    feed(PointState::UP, start + Vector2(distance, 0.0f), startTime + 200u);
  };

  // The detector-owned recognizer is created here with the default thresholds.
  feedPan(Device::Class::POINTER, 20.0f, 100u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Registering thresholds afterwards must reach the existing recognizer.
  GestureThresholds::PanThresholds remote = GestureThresholds::GetDefaultPanThresholds();
  remote.SetMinimumDistance(30);
  GestureThresholds::SetPanThresholds(POINTER_SELECTOR, remote);

  feedPan(Device::Class::POINTER, 20.0f, 5000u);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  feedPan(Device::Class::TOUCH, 20.0f, 10000u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  GestureThresholds::ClearPanThresholds(POINTER_SELECTOR);
  feedPan(Device::Class::POINTER, 20.0f, 15000u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGestureThresholdsTapMaximumHoldingTimePerDevice(void)
{
  TestApplication application;
  Actor           actor = AddActor(application);

  SignalData                         data;
  GestureReceivedFunctor<TapGesture> functor(data);
  TapGestureDetector                 detector = TapGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Pointing devices may only hold 100ms for a tap; the default is 330ms.
  GestureThresholds::TapThresholds remote = GestureThresholds::GetDefaultTapThresholds();
  remote.SetMaximumHoldingTime(100u);
  GestureThresholds::SetTapThresholds(POINTER_SELECTOR, remote);

  EmitTap(application, Device::Class::POINTER, 100u, 200u);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  EmitTap(application, Device::Class::TOUCH, 5000u, 200u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  EmitTap(application, Device::Class::POINTER, 10000u, 50u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  GestureThresholds::ClearTapThresholds(POINTER_SELECTOR);
  EmitTap(application, Device::Class::POINTER, 15000u, 200u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGestureThresholdsPinchMinimumTouchEventsPerDevice(void)
{
  TestApplication application;
  Actor           actor = AddActor(application);

  SignalData                           data;
  GestureReceivedFunctor<PinchGesture> functor(data);
  PinchGestureDetector                 detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // Fingers may start a pinch after 2 touch events; the default (also used by palms here) needs 4.
  GestureThresholds::PinchThresholds finger = GestureThresholds::GetDefaultPinchThresholds();
  finger.SetMinimumTouchEvents(2u);
  GestureThresholds::SetPinchThresholds(FINGER_SELECTOR, finger);

  EmitPinch(application, Device::Subclass::FINGER, 1u, 100u); // DOWN + 1 MOTION = 2 events
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  EmitPinch(application, Device::Subclass::PALM, 1u, 5000u);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  GestureThresholds::ClearPinchThresholds(FINGER_SELECTOR);
  EmitPinch(application, Device::Subclass::FINGER, 1u, 10000u);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGestureThresholdsRotationMinimumTouchEventsPerDevice(void)
{
  TestApplication application;
  Actor           actor = AddActor(application);

  SignalData                              data;
  GestureReceivedFunctor<RotationGesture> functor(data);
  RotationGestureDetector                 detector = RotationGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  GestureThresholds::RotationThresholds finger = GestureThresholds::GetDefaultRotationThresholds();
  finger.SetMinimumTouchEvents(2u);
  GestureThresholds::SetRotationThresholds(FINGER_SELECTOR, finger);

  EmitRotation(application, Device::Subclass::FINGER, 1u, 100u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  EmitRotation(application, Device::Subclass::PALM, 1u, 5000u);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  GestureThresholds::ClearRotationThresholds(FINGER_SELECTOR);
  EmitRotation(application, Device::Subclass::FINGER, 1u, 10000u);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

namespace
{
/**
 * Records the time stamped on the STARTED long press; the recognizer stamps touch-down time + the holding time it used.
 */
struct LongPressStartedData
{
  void Reset()
  {
    started     = false;
    startedTime = 0u;
  }

  bool     started{false};
  uint32_t startedTime{0u};
};

// Signal connections copy the functor, so the state lives outside it.
struct LongPressStartedFunctor
{
  explicit LongPressStartedFunctor(LongPressStartedData& data)
  : startedData(data)
  {
  }

  void operator()(Actor /*actor*/, LongPressGesture gesture)
  {
    if(gesture.GetState() == GestureState::STARTED)
    {
      startedData.started     = true;
      startedData.startedTime = gesture.GetTime();
    }
  }

  LongPressStartedData& startedData;
};

Internal::TouchEventPtr MakeInternalTouchEvent(const Dali::Integration::TouchEvent& touchEvent, Dali::RenderTask task)
{
  Internal::TouchEventPtr touchEventImpl(new Internal::TouchEvent(touchEvent.time));
  for(std::size_t i = 0u; i < touchEvent.GetPointCount(); ++i)
  {
    touchEventImpl->AddPoint(touchEvent.GetPoint(i));
  }
  touchEventImpl->SetRenderTask(task);
  return touchEventImpl;
}
} // namespace

int UtcDaliGestureThresholdsLongPressMinimumHoldingTimePerDevice(void)
{
  TestApplication application;
  Actor           actor = AddActor(application);

  LongPressStartedData     startedData;
  LongPressStartedFunctor  functor(startedData);
  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  const uint32_t defaultHoldingTime = GestureThresholds::GetDefaultLongPressThresholds().GetMinimumHoldingTime();

  // Pointing devices need 250ms instead of the default holding time.
  GestureThresholds::LongPressThresholds remote = GestureThresholds::GetDefaultLongPressThresholds();
  remote.SetMinimumHoldingTime(250u);
  GestureThresholds::SetLongPressThresholds(POINTER_SELECTOR, remote);

  auto pressAndRelease = [&](Device::Class::Type deviceClass, uint32_t downTime)
  {
    application.ProcessEvent(SingleTouch(PointState::DOWN, Vector2(50.0f, 50.0f), deviceClass, downTime));
    TestTriggerLongPress(application);
    application.ProcessEvent(SingleTouch(PointState::UP, Vector2(50.0f, 50.0f), deviceClass, downTime + 1000u));
    application.SendNotification();
  };

  pressAndRelease(Device::Class::POINTER, 100u);
  DALI_TEST_EQUALS(true, startedData.started, TEST_LOCATION);
  DALI_TEST_EQUALS(100u + 250u, startedData.startedTime, TEST_LOCATION);
  startedData.Reset();

  pressAndRelease(Device::Class::TOUCH, 5000u);
  DALI_TEST_EQUALS(true, startedData.started, TEST_LOCATION);
  DALI_TEST_EQUALS(5000u + defaultHoldingTime, startedData.startedTime, TEST_LOCATION);
  startedData.Reset();

  GestureThresholds::ClearLongPressThresholds(POINTER_SELECTOR);
  pressAndRelease(Device::Class::POINTER, 10000u);
  DALI_TEST_EQUALS(true, startedData.started, TEST_LOCATION);
  DALI_TEST_EQUALS(10000u + defaultHoldingTime, startedData.startedTime, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGestureThresholdsLongPressMinimumHoldingTimePerDeviceHandleEvent(void)
{
  TestApplication          application;
  Dali::Integration::Scene scene = application.GetScene();
  Dali::RenderTask         task  = scene.GetRenderTaskList().GetTask(0);
  Actor                    actor = AddActor(application);

  LongPressStartedData     startedData;
  LongPressStartedFunctor  functor(startedData);
  LongPressGestureDetector detector = LongPressGestureDetector::New();
  detector.DetectedSignal().Connect(&application, functor);

  const uint32_t defaultHoldingTime = GestureThresholds::GetDefaultLongPressThresholds().GetMinimumHoldingTime();

  auto pressAndRelease = [&](Device::Class::Type deviceClass, uint32_t downTime)
  {
    auto feed = [&](PointState::Type state, uint32_t time)
    {
      Internal::TouchEventPtr touchEventImpl = MakeInternalTouchEvent(SingleTouch(state, Vector2(50.0f, 50.0f), deviceClass, time), task);
      Dali::TouchEvent        touchEventHandle(touchEventImpl.Get());
      detector.HandleEvent(actor, touchEventHandle);
    };
    feed(PointState::DOWN, downTime);
    TestTriggerLongPress(application);
    feed(PointState::UP, downTime + 1000u);
  };

  // The detector-owned recognizer is created here with the default holding time.
  pressAndRelease(Device::Class::POINTER, 100u);
  DALI_TEST_EQUALS(true, startedData.started, TEST_LOCATION);
  DALI_TEST_EQUALS(100u + defaultHoldingTime, startedData.startedTime, TEST_LOCATION);
  startedData.Reset();

  // Registering afterwards must reach the existing recognizer.
  GestureThresholds::LongPressThresholds remote = GestureThresholds::GetDefaultLongPressThresholds();
  remote.SetMinimumHoldingTime(250u);
  GestureThresholds::SetLongPressThresholds(POINTER_SELECTOR, remote);

  pressAndRelease(Device::Class::POINTER, 5000u);
  DALI_TEST_EQUALS(true, startedData.started, TEST_LOCATION);
  DALI_TEST_EQUALS(5000u + 250u, startedData.startedTime, TEST_LOCATION);
  startedData.Reset();

  pressAndRelease(Device::Class::TOUCH, 10000u);
  DALI_TEST_EQUALS(true, startedData.started, TEST_LOCATION);
  DALI_TEST_EQUALS(10000u + defaultHoldingTime, startedData.startedTime, TEST_LOCATION);
  startedData.Reset();

  GestureThresholds::ClearLongPressThresholds(POINTER_SELECTOR);
  pressAndRelease(Device::Class::POINTER, 15000u);
  DALI_TEST_EQUALS(true, startedData.started, TEST_LOCATION);
  DALI_TEST_EQUALS(15000u + defaultHoldingTime, startedData.startedTime, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGestureThresholdsTapMaximumHoldingTimePerDeviceHandleEvent(void)
{
  TestApplication          application;
  Dali::Integration::Scene scene = application.GetScene();
  Dali::RenderTask         task  = scene.GetRenderTaskList().GetTask(0);
  Actor                    actor = AddActor(application);

  SignalData                         data;
  GestureReceivedFunctor<TapGesture> functor(data);
  TapGestureDetector                 detector = TapGestureDetector::New();
  detector.DetectedSignal().Connect(&application, functor);

  auto tap = [&](Device::Class::Type deviceClass, uint32_t downTime, uint32_t holdMilliseconds)
  {
    auto feed = [&](PointState::Type state, uint32_t time)
    {
      Internal::TouchEventPtr touchEventImpl = MakeInternalTouchEvent(SingleTouch(state, Vector2(50.0f, 50.0f), deviceClass, time), task);
      Dali::TouchEvent        touchEventHandle(touchEventImpl.Get());
      detector.HandleEvent(actor, touchEventHandle);
    };
    feed(PointState::DOWN, downTime);
    feed(PointState::UP, downTime + holdMilliseconds);
  };

  // Recognizer created with the default 330ms; a 200ms hold is a tap for any device.
  tap(Device::Class::POINTER, 100u, 200u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  GestureThresholds::TapThresholds remote = GestureThresholds::GetDefaultTapThresholds();
  remote.SetMaximumHoldingTime(100u);
  GestureThresholds::SetTapThresholds(POINTER_SELECTOR, remote);

  tap(Device::Class::POINTER, 5000u, 200u);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  tap(Device::Class::TOUCH, 10000u, 200u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  tap(Device::Class::POINTER, 15000u, 50u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  GestureThresholds::ClearTapThresholds(POINTER_SELECTOR);
  tap(Device::Class::POINTER, 20000u, 200u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGestureThresholdsPinchMinimumDistancePerDevice(void)
{
  TestApplication application;
  Actor           actor = AddActor(application);

  SignalData                           data;
  GestureReceivedFunctor<PinchGesture> functor(data);
  PinchGestureDetector                 detector = PinchGestureDetector::New();
  detector.Attach(actor);
  detector.DetectedSignal().Connect(&application, functor);

  // The built-in default is the DPI-derived distance, expressed as a negative value.
  GestureThresholds::PinchThresholds defaults = GestureThresholds::GetDefaultPinchThresholds();
  DALI_TEST_CHECK(defaults.GetMinimumDistance() < 0.0f);

  // EmitPinch closes the fingers by 16px: enough for the DPI default (about 10px at the test DPI).
  EmitPinch(application, Device::Subclass::FINGER, 4u, 100u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // Fingers must close by 20px.
  GestureThresholds::PinchThresholds finger = GestureThresholds::GetDefaultPinchThresholds();
  finger.SetMinimumDistance(20.0f);
  GestureThresholds::SetPinchThresholds(FINGER_SELECTOR, finger);

  EmitPinch(application, Device::Subclass::FINGER, 4u, 5000u);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  EmitPinch(application, Device::Subclass::PALM, 4u, 10000u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  // A negative per-device value selects the DPI default again, and the copy-modify-set round trip keeps it.
  finger.SetMinimumDistance(-1.0f);
  GestureThresholds::SetPinchThresholds(FINGER_SELECTOR, finger);
  GestureThresholds::PinchThresholds stored;
  DALI_TEST_CHECK(GestureThresholds::GetPinchThresholds(FINGER_SELECTOR, stored));
  DALI_TEST_EQUALS(-1.0f, stored.GetMinimumDistance(), TEST_LOCATION);

  EmitPinch(application, Device::Subclass::FINGER, 4u, 15000u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  GestureThresholds::ClearPinchThresholds(FINGER_SELECTOR);
  EmitPinch(application, Device::Subclass::FINGER, 4u, 20000u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGestureThresholdsPinchMinimumTouchEventsPerDeviceHandleEvent(void)
{
  TestApplication          application;
  Dali::Integration::Scene scene = application.GetScene();
  Dali::RenderTask         task  = scene.GetRenderTaskList().GetTask(0);
  Actor                    actor = AddActor(application);

  SignalData                           data;
  GestureReceivedFunctor<PinchGesture> functor(data);
  PinchGestureDetector                 detector = PinchGestureDetector::New();
  detector.DetectedSignal().Connect(&application, functor);

  auto pinch = [&](Device::Subclass::Type deviceSubclass, uint32_t motionEvents, uint32_t startTime)
  {
    auto feed = [&](PointState::Type state, const Vector2& a, const Vector2& b, uint32_t time)
    {
      Internal::TouchEventPtr touchEventImpl = MakeInternalTouchEvent(DoubleTouch(state, a, b, deviceSubclass, time), task);
      Dali::TouchEvent        touchEventHandle(touchEventImpl.Get());
      detector.HandleEvent(actor, touchEventHandle);
    };
    uint32_t time = startTime;
    feed(PointState::DOWN, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), time);
    for(uint32_t i = 0u; i < motionEvents; ++i)
    {
      time += 50u;
      feed(PointState::MOTION, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), time);
    }
    feed(PointState::UP, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), time + 50u);
  };

  // Recognizer created with the default 4 touch events: DOWN + 1 MOTION is not enough.
  pinch(Device::Subclass::FINGER, 1u, 100u);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  GestureThresholds::PinchThresholds finger = GestureThresholds::GetDefaultPinchThresholds();
  finger.SetMinimumTouchEvents(2u);
  GestureThresholds::SetPinchThresholds(FINGER_SELECTOR, finger);

  pinch(Device::Subclass::FINGER, 1u, 5000u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  pinch(Device::Subclass::PALM, 1u, 10000u);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  GestureThresholds::ClearPinchThresholds(FINGER_SELECTOR);
  pinch(Device::Subclass::FINGER, 1u, 15000u);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGestureThresholdsRotationMinimumTouchEventsPerDeviceHandleEvent(void)
{
  TestApplication          application;
  Dali::Integration::Scene scene = application.GetScene();
  Dali::RenderTask         task  = scene.GetRenderTaskList().GetTask(0);
  Actor                    actor = AddActor(application);

  SignalData                              data;
  GestureReceivedFunctor<RotationGesture> functor(data);
  RotationGestureDetector                 detector = RotationGestureDetector::New();
  detector.DetectedSignal().Connect(&application, functor);

  auto rotate = [&](Device::Subclass::Type deviceSubclass, uint32_t motionEvents, uint32_t startTime)
  {
    auto feed = [&](PointState::Type state, const Vector2& a, const Vector2& b, uint32_t time)
    {
      Internal::TouchEventPtr touchEventImpl = MakeInternalTouchEvent(DoubleTouch(state, a, b, deviceSubclass, time), task);
      Dali::TouchEvent        touchEventHandle(touchEventImpl.Get());
      detector.HandleEvent(actor, touchEventHandle);
    };
    uint32_t time = startTime;
    feed(PointState::DOWN, Vector2(2.0f, 20.0f), Vector2(38.0f, 20.0f), time);
    for(uint32_t i = 0u; i < motionEvents; ++i)
    {
      time += 50u;
      feed(PointState::MOTION, Vector2(20.0f, 2.0f), Vector2(20.0f, 38.0f), time);
    }
    feed(PointState::UP, Vector2(20.0f, 2.0f), Vector2(20.0f, 38.0f), time + 50u);
  };

  rotate(Device::Subclass::FINGER, 1u, 100u);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  GestureThresholds::RotationThresholds finger = GestureThresholds::GetDefaultRotationThresholds();
  finger.SetMinimumTouchEvents(2u);
  GestureThresholds::SetRotationThresholds(FINGER_SELECTOR, finger);

  rotate(Device::Subclass::FINGER, 1u, 5000u);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  data.Reset();

  rotate(Device::Subclass::PALM, 1u, 10000u);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  GestureThresholds::ClearRotationThresholds(FINGER_SELECTOR);
  rotate(Device::Subclass::FINGER, 1u, 15000u);
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}
