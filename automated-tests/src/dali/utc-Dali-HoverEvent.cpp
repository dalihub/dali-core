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
#include <dali/integration-api/events/hover-event-integ.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>
#include <utility>

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
HoverEvent GenerateHoverEvent()
{
  HoverEvent hoverEvent = HoverEvent::New(123u);
  hoverEvent.AddPoint(1, PointState::STARTED, Vector2(100.0f, 200.0f));
  return hoverEvent;
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
  HoverEvent hoverEvent = GenerateHoverEvent();
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
  HoverEvent hoverEvent = GenerateHoverEvent();
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
  HoverEvent hoverEvent = GenerateHoverEvent();
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
  HoverEvent hoverEvent = GenerateHoverEvent();
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
  Vector2 touchPoint(10.0, 20.0);

  HoverEvent hoverEvent = HoverEvent::New(123u);
  hoverEvent.AddPoint(1, PointState::DOWN, Vector2(touchPoint.x, touchPoint.y));
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
  Vector2 touchPoint(10.0, 20.0);

  HoverEvent hoverEvent = HoverEvent::New(123u);
  hoverEvent.AddPoint(1, PointState::DOWN, Vector2(touchPoint.x, touchPoint.y));
  DALI_TEST_CHECK(hoverEvent);

  const auto refCount = hoverEvent.GetBaseObject().ReferenceCount();

  HoverEvent hoverEvent2(std::move(hoverEvent));
  DALI_TEST_CHECK(!hoverEvent);
  DALI_TEST_CHECK(hoverEvent2);
  DALI_TEST_EQUALS(refCount, hoverEvent2.GetBaseObject().ReferenceCount(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliIntegrationHoverEventCopyAssignment(void)
{
  Integration::Point point;
  point.SetDeviceId(7);
  Integration::HoverEvent hoverSource(234u);
  hoverSource.AddPoint(point);
  Integration::HoverEvent hoverTarget(567u);
  DALI_TEST_CHECK(&(hoverTarget = hoverSource) == &hoverTarget);
  DALI_TEST_EQUALS(hoverTarget.type, Integration::Event::Hover, TEST_LOCATION);
  DALI_TEST_EQUALS(hoverTarget.time, 234u, TEST_LOCATION);
  DALI_TEST_EQUALS(hoverTarget.GetPointCount(), 1u, TEST_LOCATION);
  DALI_TEST_EQUALS(hoverTarget.GetPoint(0).GetDeviceId(), 7, TEST_LOCATION);

  END_TEST;
}

int UtcDaliIntegrationHoverEventMove(void)
{
  Integration::Point point;
  point.SetDeviceId(11);
  Integration::HoverEvent hoverSource(234u);
  hoverSource.AddPoint(point);
  Integration::HoverEvent hoverMoved(std::move(hoverSource));
  DALI_TEST_EQUALS(hoverMoved.type, Integration::Event::Hover, TEST_LOCATION);
  DALI_TEST_EQUALS(hoverMoved.time, 234u, TEST_LOCATION);
  DALI_TEST_EQUALS(hoverMoved.GetPoint(0).GetDeviceId(), 11, TEST_LOCATION);
  Integration::HoverEvent hoverTarget;
  DALI_TEST_CHECK(&(hoverTarget = std::move(hoverMoved)) == &hoverTarget);
  DALI_TEST_EQUALS(hoverTarget.time, 234u, TEST_LOCATION);
  DALI_TEST_EQUALS(hoverTarget.GetPointCount(), 1u, TEST_LOCATION);

  END_TEST;
}