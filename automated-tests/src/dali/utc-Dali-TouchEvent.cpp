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

#include <iostream>
#include <stdlib.h>

#include <dali/public-api/dali-core.h>
#include <dali/integration-api/events/touch-integ.h>
#include <dali-test-suite-utils.h>

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
}

int UtcDaliTouchEventConstructorP(void)
{
  TouchEvent touchEvent;
  DALI_TEST_CHECK( !touchEvent );
  END_TEST;
}

int UtcDaliTouchEventCopyConstructorP(void)
{
  TouchEvent touchEvent = Integration::NewTouchEvent(123u, GenerateTouchPoint());
  DALI_TEST_CHECK( touchEvent );

  const auto refCount = touchEvent.GetBaseObject().ReferenceCount();

  TouchEvent touchEvent2( touchEvent );
  DALI_TEST_CHECK( touchEvent );
  DALI_TEST_CHECK( touchEvent2 );
  DALI_TEST_EQUALS( touchEvent, touchEvent2, TEST_LOCATION );
  DALI_TEST_EQUALS( refCount + 1, touchEvent.GetBaseObject().ReferenceCount(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliTouchEventMoveConstructorP(void)
{
  TouchEvent touchEvent = Integration::NewTouchEvent(123u, GenerateTouchPoint());
  DALI_TEST_CHECK( touchEvent );

  const auto refCount = touchEvent.GetBaseObject().ReferenceCount();

  TouchEvent touchEvent2( std::move(touchEvent) );
  DALI_TEST_CHECK( !touchEvent );
  DALI_TEST_CHECK( touchEvent2 );
  DALI_TEST_EQUALS( refCount, touchEvent2.GetBaseObject().ReferenceCount(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliTouchEventCopyAssignmentP(void)
{
  TouchEvent touchEvent = Integration::NewTouchEvent(123u, GenerateTouchPoint());
  DALI_TEST_CHECK( touchEvent );

  const auto refCount = touchEvent.GetBaseObject().ReferenceCount();

  TouchEvent touchEvent2;
  DALI_TEST_CHECK( !touchEvent2 );

  touchEvent2 = touchEvent;
  DALI_TEST_CHECK( touchEvent );
  DALI_TEST_CHECK( touchEvent2 );
  DALI_TEST_EQUALS( touchEvent, touchEvent2, TEST_LOCATION );
  DALI_TEST_EQUALS( refCount + 1, touchEvent.GetBaseObject().ReferenceCount(), TEST_LOCATION );

  END_TEST;
}

int UtcDaliTouchEventMoveAssignmentP(void)
{
  TouchEvent touchEvent = Integration::NewTouchEvent(123u, GenerateTouchPoint());
  DALI_TEST_CHECK( touchEvent );

  const auto refCount = touchEvent.GetBaseObject().ReferenceCount();

  TouchEvent touchEvent2;
  DALI_TEST_CHECK( !touchEvent2 );

  touchEvent2 = std::move(touchEvent);
  DALI_TEST_CHECK( !touchEvent );
  DALI_TEST_CHECK( touchEvent2 );
  DALI_TEST_EQUALS( refCount, touchEvent2.GetBaseObject().ReferenceCount(), TEST_LOCATION );

  END_TEST;
}
