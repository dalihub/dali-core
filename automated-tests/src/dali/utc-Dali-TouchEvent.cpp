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
#include <dali/devel-api/events/touch-event-devel.h>
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

  DALI_TEST_EQUALS(123lu, touchEvent.GetTime(), TEST_LOCATION);

  DevelTouchEvent::SetTime(touchEvent, 200lu);
  DALI_TEST_EQUALS(200lu, touchEvent.GetTime(), TEST_LOCATION);

  END_TEST;
}
