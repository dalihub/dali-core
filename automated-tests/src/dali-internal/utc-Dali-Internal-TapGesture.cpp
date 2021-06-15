/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/events/tap-gesture-devel.h>
#include <dali/internal/event/events/tap-gesture/tap-gesture-impl.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>

using namespace Dali;

void utc_dali_internal_tap_gesture_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_tap_gesture_cleanup(void)
{
  test_return_value = TET_PASS;
}

// Positive test case for a method
int UtcDaliTapGestureConstructor(void)
{
  TestApplication application; // Reset all test adapter return codes

  TapGesture gesture = DevelTapGesture::New(GestureState::STARTED);
  DALI_TEST_EQUALS(1u, gesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, gesture.GetNumberOfTaps(), TEST_LOCATION);
  DALI_TEST_EQUALS(GestureType::TAP, gesture.GetType(), TEST_LOCATION);

  // Test Copy constructor
  GetImplementation(gesture).SetNumberOfTouches(5u);
  GetImplementation(gesture).SetNumberOfTaps(2u);

  TapGesture gesture2(gesture);
  DALI_TEST_EQUALS(5u, gesture2.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(2u, gesture2.GetNumberOfTaps(), TEST_LOCATION);
  DALI_TEST_EQUALS(GestureType::TAP, gesture2.GetType(), TEST_LOCATION);

  // Test move constructor
  const auto refCount = gesture.GetObjectPtr()->ReferenceCount();
  TapGesture gesture3(std::move(gesture));
  DALI_TEST_CHECK(!gesture);
  DALI_TEST_EQUALS(GestureType::TAP, gesture3.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(gesture3.GetBaseObject().ReferenceCount(), refCount, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureAssignment(void)
{
  // Test Assignment operator
  TapGesture gesture = DevelTapGesture::New(GestureState::STARTED);
  DALI_TEST_EQUALS(1u, gesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, gesture.GetNumberOfTaps(), TEST_LOCATION);
  DALI_TEST_EQUALS(GestureType::TAP, gesture.GetType(), TEST_LOCATION);

  GetImplementation(gesture).SetNumberOfTouches(5u);
  GetImplementation(gesture).SetNumberOfTaps(2u);

  TapGesture gesture2;
  gesture2 = gesture;
  DALI_TEST_EQUALS(5u, gesture2.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(2u, gesture2.GetNumberOfTaps(), TEST_LOCATION);
  DALI_TEST_EQUALS(GestureType::TAP, gesture2.GetType(), TEST_LOCATION);

  // Move assignment
  const auto refCount = gesture.GetObjectPtr()->ReferenceCount();
  TapGesture gesture3;
  DALI_TEST_EQUALS(gesture3, Gesture(), TEST_LOCATION);
  gesture3 = std::move(gesture);
  DALI_TEST_CHECK(!gesture);
  DALI_TEST_EQUALS(GestureType::TAP, gesture3.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(gesture3.GetBaseObject().ReferenceCount(), refCount, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureSetGetNumberOfTapsP(void)
{
  TapGesture gesture = DevelTapGesture::New(GestureState::STARTED);
  DALI_TEST_EQUALS(gesture.GetNumberOfTaps(), 1u, TEST_LOCATION);

  GetImplementation(gesture).SetNumberOfTaps(123u);
  DALI_TEST_EQUALS(gesture.GetNumberOfTaps(), 123u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureSetGetNumberOfTouchesP(void)
{
  TapGesture gesture = DevelTapGesture::New(GestureState::STARTED);
  DALI_TEST_EQUALS(gesture.GetNumberOfTouches(), 1u, TEST_LOCATION);

  GetImplementation(gesture).SetNumberOfTouches(321u);
  DALI_TEST_EQUALS(gesture.GetNumberOfTouches(), 321u, TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureSetGetScreenPointP(void)
{
  TapGesture gesture = DevelTapGesture::New(GestureState::STARTED);
  DALI_TEST_EQUALS(gesture.GetScreenPoint(), Vector2::ZERO, TEST_LOCATION);

  GetImplementation(gesture).SetScreenPoint(Vector2(123.0f, 321.0f));
  DALI_TEST_EQUALS(gesture.GetScreenPoint(), Vector2(123.0f, 321.0f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureSetGetLocalPointP(void)
{
  TapGesture gesture = DevelTapGesture::New(GestureState::STARTED);
  DALI_TEST_EQUALS(gesture.GetLocalPoint(), Vector2::ZERO, TEST_LOCATION);

  GetImplementation(gesture).SetLocalPoint(Vector2(123.0f, 321.0f));
  DALI_TEST_EQUALS(gesture.GetLocalPoint(), Vector2(123.0f, 321.0f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliTapGestureSetGetSourceTypeP(void)
{
  TapGesture gesture = DevelTapGesture::New(GestureState::STARTED);
  DALI_TEST_EQUALS(gesture.GetSourceType(), GestureSourceType::INVALID, TEST_LOCATION);

  GetImplementation(gesture).SetGestureSourceType(GestureSourceType::PRIMARY);
  DALI_TEST_EQUALS(gesture.GetSourceType(), GestureSourceType::PRIMARY, TEST_LOCATION);

  GetImplementation(gesture).SetGestureSourceType(GestureSourceType::SECONDARY);
  DALI_TEST_EQUALS(gesture.GetSourceType(), GestureSourceType::SECONDARY, TEST_LOCATION);

  GetImplementation(gesture).SetGestureSourceType(GestureSourceType::TERTIARY);
  DALI_TEST_EQUALS(gesture.GetSourceType(), GestureSourceType::TERTIARY, TEST_LOCATION);

  END_TEST;
}