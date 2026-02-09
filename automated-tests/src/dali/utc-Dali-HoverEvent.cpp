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

void utc_dali_hover_event_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_hover_event_cleanup(void)
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

int UtcDaliHoverEventConstructorP(void)
{
  HoverEvent hoverEvent;
  DALI_TEST_CHECK(!hoverEvent);
  END_TEST;
}

int UtcDaliHoverEventCopyConstructorP(void)
{
  HoverEvent hoverEvent = Dali::Integration::NewHoverEvent(123u, GenerateTouchPoint());
  DALI_TEST_CHECK(hoverEvent);

  const auto refCount = hoverEvent.GetBaseObject().ReferenceCount();

  HoverEvent hoverEvent2(hoverEvent);
  DALI_TEST_CHECK(hoverEvent);
  DALI_TEST_CHECK(hoverEvent2);
  DALI_TEST_EQUALS(hoverEvent, hoverEvent2, TEST_LOCATION);
  DALI_TEST_EQUALS(refCount + 1, hoverEvent.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliHoverEventMoveConstructorP(void)
{
  HoverEvent hoverEvent = Dali::Integration::NewHoverEvent(123u, GenerateTouchPoint());
  DALI_TEST_CHECK(hoverEvent);

  const auto refCount = hoverEvent.GetBaseObject().ReferenceCount();

  HoverEvent hoverEvent2(std::move(hoverEvent));
  DALI_TEST_CHECK(!hoverEvent);
  DALI_TEST_CHECK(hoverEvent2);
  DALI_TEST_EQUALS(refCount, hoverEvent2.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliHoverEventCopyAssignmentP(void)
{
  HoverEvent hoverEvent = Dali::Integration::NewHoverEvent(123u, GenerateTouchPoint());
  DALI_TEST_CHECK(hoverEvent);

  const auto refCount = hoverEvent.GetBaseObject().ReferenceCount();

  HoverEvent hoverEvent2;
  DALI_TEST_CHECK(!hoverEvent2);

  hoverEvent2 = hoverEvent;
  DALI_TEST_CHECK(hoverEvent);
  DALI_TEST_CHECK(hoverEvent2);
  DALI_TEST_EQUALS(hoverEvent, hoverEvent2, TEST_LOCATION);
  DALI_TEST_EQUALS(refCount + 1, hoverEvent.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliHoverEventMoveAssignmentP(void)
{
  HoverEvent hoverEvent = Dali::Integration::NewHoverEvent(123u, GenerateTouchPoint());
  DALI_TEST_CHECK(hoverEvent);

  const auto refCount = hoverEvent.GetBaseObject().ReferenceCount();

  HoverEvent hoverEvent2;
  DALI_TEST_CHECK(!hoverEvent2);

  hoverEvent2 = std::move(hoverEvent);
  DALI_TEST_CHECK(!hoverEvent);
  DALI_TEST_CHECK(hoverEvent2);
  DALI_TEST_EQUALS(refCount, hoverEvent2.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliHoverEventCopyConstructorWithPointP(void)
{
  Dali::Integration::Point point;

  Vector2 touchPoint(10.0, 20.0);
  point.SetDeviceId(1);
  point.SetState(PointState::DOWN);
  point.SetScreenPosition(Vector2(touchPoint.x, touchPoint.y));

  HoverEvent hoverEvent = Dali::Integration::NewHoverEvent(123u, point);
  DALI_TEST_CHECK(hoverEvent);

  const auto refCount = hoverEvent.GetBaseObject().ReferenceCount();

  HoverEvent hoverEvent2(hoverEvent);
  DALI_TEST_CHECK(hoverEvent);
  DALI_TEST_CHECK(hoverEvent2);
  DALI_TEST_EQUALS(hoverEvent, hoverEvent2, TEST_LOCATION);
  DALI_TEST_EQUALS(refCount + 1, hoverEvent.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliHoverEventMoveConstructorWithPointP(void)
{
  Dali::Integration::Point point;

  Vector2 touchPoint(10.0, 20.0);
  point.SetDeviceId(1);
  point.SetState(PointState::DOWN);
  point.SetScreenPosition(Vector2(touchPoint.x, touchPoint.y));

  HoverEvent hoverEvent = Dali::Integration::NewHoverEvent(123u, point);
  DALI_TEST_CHECK(hoverEvent);

  const auto refCount = hoverEvent.GetBaseObject().ReferenceCount();

  HoverEvent hoverEvent2(std::move(hoverEvent));
  DALI_TEST_CHECK(!hoverEvent);
  DALI_TEST_CHECK(hoverEvent2);
  DALI_TEST_EQUALS(refCount, hoverEvent2.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  END_TEST;
}
