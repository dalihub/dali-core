/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/events/touch-integ.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>

void utc_dali_touch_event_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_touch_event_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
TouchPoint GenerateTouchPoint()
{
  return TouchPoint(1, PointState::STARTED, 100.0f, 200.0f);
}
} // namespace

int UtcDaliTouchEventConstructorP(void)
{
  TouchEvent touchEvent;
  DALI_TEST_CHECK(!touchEvent);
  END_TEST;
}

int UtcDaliTouchEventCopyConstructorP(void)
{
  TouchEvent touchEvent = Dali::Integration::NewTouchEvent(123u, GenerateTouchPoint());
  DALI_TEST_CHECK(touchEvent);

  const auto refCount = touchEvent.GetBaseObject().ReferenceCount();

  TouchEvent touchEvent2(touchEvent);
  DALI_TEST_CHECK(touchEvent);
  DALI_TEST_CHECK(touchEvent2);
  DALI_TEST_EQUALS(touchEvent, touchEvent2, TEST_LOCATION);
  DALI_TEST_EQUALS(refCount + 1, touchEvent.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTouchEventMoveConstructorP(void)
{
  TouchEvent touchEvent = Dali::Integration::NewTouchEvent(123u, GenerateTouchPoint());
  DALI_TEST_CHECK(touchEvent);

  const auto refCount = touchEvent.GetBaseObject().ReferenceCount();

  TouchEvent touchEvent2(std::move(touchEvent));
  DALI_TEST_CHECK(!touchEvent);
  DALI_TEST_CHECK(touchEvent2);
  DALI_TEST_EQUALS(refCount, touchEvent2.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTouchEventCopyAssignmentP(void)
{
  TouchEvent touchEvent = Dali::Integration::NewTouchEvent(123u, GenerateTouchPoint());
  DALI_TEST_CHECK(touchEvent);

  const auto refCount = touchEvent.GetBaseObject().ReferenceCount();

  TouchEvent touchEvent2;
  DALI_TEST_CHECK(!touchEvent2);

  touchEvent2 = touchEvent;
  DALI_TEST_CHECK(touchEvent);
  DALI_TEST_CHECK(touchEvent2);
  DALI_TEST_EQUALS(touchEvent, touchEvent2, TEST_LOCATION);
  DALI_TEST_EQUALS(refCount + 1, touchEvent.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTouchEventMoveAssignmentP(void)
{
  TouchEvent touchEvent = Dali::Integration::NewTouchEvent(123u, GenerateTouchPoint());
  DALI_TEST_CHECK(touchEvent);

  const auto refCount = touchEvent.GetBaseObject().ReferenceCount();

  TouchEvent touchEvent2;
  DALI_TEST_CHECK(!touchEvent2);

  touchEvent2 = std::move(touchEvent);
  DALI_TEST_CHECK(!touchEvent);
  DALI_TEST_CHECK(touchEvent2);
  DALI_TEST_EQUALS(refCount, touchEvent2.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTouchEventCopyConstructorWithPointP(void)
{
  Dali::Integration::Point point;

  Vector2 touchPoint(10.0, 20.0);
  point.SetDeviceId(1);
  point.SetState(PointState::DOWN);
  point.SetScreenPosition(Vector2(touchPoint.x, touchPoint.y));

  TouchEvent touchEvent = Dali::Integration::NewTouchEvent(123u, point);
  DALI_TEST_CHECK(touchEvent);

  const auto refCount = touchEvent.GetBaseObject().ReferenceCount();

  TouchEvent touchEvent2(touchEvent);
  DALI_TEST_CHECK(touchEvent);
  DALI_TEST_CHECK(touchEvent2);
  DALI_TEST_EQUALS(touchEvent, touchEvent2, TEST_LOCATION);
  DALI_TEST_EQUALS(refCount + 1, touchEvent.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTouchEventMoveConstructorWithPointP(void)
{
  Dali::Integration::Point point;

  Vector2 touchPoint(10.0, 20.0);
  point.SetDeviceId(1);
  point.SetState(PointState::DOWN);
  point.SetScreenPosition(Vector2(touchPoint.x, touchPoint.y));

  TouchEvent touchEvent = Dali::Integration::NewTouchEvent(123u, point);
  DALI_TEST_CHECK(touchEvent);

  const auto refCount = touchEvent.GetBaseObject().ReferenceCount();

  TouchEvent touchEvent2(std::move(touchEvent));
  DALI_TEST_CHECK(!touchEvent);
  DALI_TEST_CHECK(touchEvent2);
  DALI_TEST_EQUALS(refCount, touchEvent2.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTouchEventSetTime(void)
{
  TestApplication application;

  TouchEvent touchEvent = Dali::Integration::NewTouchEvent(123u, GenerateTouchPoint());
  DALI_TEST_CHECK(touchEvent);

  DALI_TEST_EQUALS(123u, touchEvent.GetTime(), TEST_LOCATION);

  touchEvent.SetTime(200u);
  DALI_TEST_EQUALS(200u, touchEvent.GetTime(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTouchEventNew(void)
{
  TestApplication application;

  TouchEvent touchEvent = TouchEvent::New(123u);
  DALI_TEST_CHECK(touchEvent);

  DALI_TEST_EQUALS(123u, touchEvent.GetTime(), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, touchEvent.GetPointCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTouchEventAddPoint(void)
{
  TestApplication application;

  TouchEvent touchEvent = TouchEvent::New(123u);
  DALI_TEST_CHECK(touchEvent);

  touchEvent.AddPoint(1, PointState::DOWN, Vector2(10.0f, 20.0f));

  DALI_TEST_EQUALS(1u, touchEvent.GetPointCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(1, touchEvent.GetDeviceId(0), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::DOWN, touchEvent.GetState(0), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(10.0f, 20.0f), touchEvent.GetScreenPosition(0), TEST_LOCATION);
  DALI_TEST_EQUALS(Device::Class::NONE, touchEvent.GetDeviceClass(0), TEST_LOCATION);
  DALI_TEST_EQUALS(Device::Subclass::NONE, touchEvent.GetDeviceSubclass(0), TEST_LOCATION);
  DALI_TEST_EQUALS(MouseButton::INVALID, touchEvent.GetMouseButton(0), TEST_LOCATION);
  DALI_TEST_EQUALS("", touchEvent.GetDeviceName(0), TEST_LOCATION);

  // The hit-actor and the local position are filled in when the event is processed
  DALI_TEST_CHECK(!touchEvent.GetHitActor(0));
  DALI_TEST_EQUALS(Vector2::ZERO, touchEvent.GetLocalPosition(0), TEST_LOCATION);

  // Add a second point with the device information
  touchEvent.AddPoint(2, PointState::MOTION, Vector2(30.0f, 40.0f), Device::Class::MOUSE, Device::Subclass::NONE, "mouse-0", MouseButton::SECONDARY);

  DALI_TEST_EQUALS(2u, touchEvent.GetPointCount(), TEST_LOCATION);
  DALI_TEST_EQUALS(2, touchEvent.GetDeviceId(1), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::MOTION, touchEvent.GetState(1), TEST_LOCATION);
  DALI_TEST_EQUALS(Vector2(30.0f, 40.0f), touchEvent.GetScreenPosition(1), TEST_LOCATION);
  DALI_TEST_EQUALS(Device::Class::MOUSE, touchEvent.GetDeviceClass(1), TEST_LOCATION);
  DALI_TEST_EQUALS(Device::Subclass::NONE, touchEvent.GetDeviceSubclass(1), TEST_LOCATION);
  DALI_TEST_EQUALS(MouseButton::SECONDARY, touchEvent.GetMouseButton(1), TEST_LOCATION);
  DALI_TEST_EQUALS("mouse-0", touchEvent.GetDeviceName(1), TEST_LOCATION);

  END_TEST;
}
