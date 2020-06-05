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
#include <dali-test-suite-utils.h>
#include <dali/internal/event/events/long-press-gesture/long-press-gesture-impl.h>

using namespace Dali;

void utc_dali_internal_long_press_gesture_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_long_press_gesture_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliLongPressGestureConstructorP(void)
{
  TestApplication application;

  LongPressGesture gesture(new Internal::LongPressGesture( Gesture::Started ));
  DALI_TEST_EQUALS(1u, gesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::LongPress, gesture.GetType(), TEST_LOCATION);

  // Test Copy constructor
  GetImplementation( gesture ).SetNumberOfTouches( 5u );

  LongPressGesture gesture2(gesture);
  DALI_TEST_EQUALS(5u, gesture2.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::LongPress, gesture2.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Started, gesture2.GetState(), TEST_LOCATION);

  // Test move constructor
  const auto refCount = gesture.GetObjectPtr()->ReferenceCount();
  LongPressGesture gesture3( std::move( gesture ) );
  DALI_TEST_EQUALS(gesture, LongPressGesture(), TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::LongPress, gesture3.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(gesture3.GetBaseObject().ReferenceCount(), refCount, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLongPressGestureAssignmentP(void)
{
  // Test Assignment operator
  LongPressGesture gesture(new Internal::LongPressGesture( Gesture::Started ));
  DALI_TEST_EQUALS(1u, gesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::LongPress, gesture.GetType(), TEST_LOCATION);

  GetImplementation( gesture ).SetNumberOfTouches( 5u );

  LongPressGesture gesture2(new Internal::LongPressGesture( Gesture::Finished ));
  DALI_TEST_EQUALS(Gesture::Finished, gesture2.GetState(), TEST_LOCATION);
  gesture2 = gesture;
  DALI_TEST_EQUALS(5u, gesture2.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::LongPress, gesture2.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Started, gesture2.GetState(), TEST_LOCATION);

  // Move assignment
  const auto refCount = gesture.GetObjectPtr()->ReferenceCount();
  LongPressGesture gesture3;
  DALI_TEST_EQUALS(gesture3, Gesture(), TEST_LOCATION);
  gesture3 = std::move(gesture);
  DALI_TEST_EQUALS(gesture, Gesture(), TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::LongPress, gesture3.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(gesture3.GetBaseObject().ReferenceCount(), refCount, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLongPressGestureGetNumberOfTouchesP(void)
{
  TestApplication application;

  LongPressGesture gesture(new Internal::LongPressGesture(Gesture::Started));
  DALI_TEST_EQUALS(gesture.GetNumberOfTouches(), 1, TEST_LOCATION);

  GetImplementation(gesture).SetNumberOfTouches(4);
  DALI_TEST_EQUALS(gesture.GetNumberOfTouches(), 4, TEST_LOCATION);

  END_TEST;
}

int UtcDaliLongPressGestureGetScreenPointP(void)
{
  TestApplication application;

  LongPressGesture gesture(new Internal::LongPressGesture(Gesture::Started));
  DALI_TEST_EQUALS(gesture.GetScreenPoint(), Vector2::ZERO , TEST_LOCATION);
  DALI_TEST_EQUALS(gesture.GetLocalPoint(), Vector2::ZERO , TEST_LOCATION);

  GetImplementation(gesture).SetScreenPoint(Vector2(100.0f, 300.0f));
  DALI_TEST_EQUALS(gesture.GetScreenPoint(), Vector2(100.0f, 300.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(gesture.GetLocalPoint(), Vector2::ZERO , TEST_LOCATION);

  END_TEST;
}

int UtcDaliLongPressGestureGetLocalPointP(void)
{
  TestApplication application;

  LongPressGesture gesture(new Internal::LongPressGesture(Gesture::Started));
  DALI_TEST_EQUALS(gesture.GetLocalPoint(), Vector2::ZERO , TEST_LOCATION);
  DALI_TEST_EQUALS(gesture.GetScreenPoint(), Vector2::ZERO , TEST_LOCATION);

  GetImplementation(gesture).SetLocalPoint(Vector2(100.0f, 300.0f));
  DALI_TEST_EQUALS(gesture.GetLocalPoint(), Vector2(100.0f, 300.0f), TEST_LOCATION);
  DALI_TEST_EQUALS(gesture.GetScreenPoint(), Vector2::ZERO , TEST_LOCATION);

  END_TEST;
}
