/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/events/pinch-gesture-devel.h>
#include <dali/internal/event/events/pinch-gesture/pinch-gesture-impl.h>

using namespace Dali;

void utc_dali_internal_pinch_gesture_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_pinch_gesture_cleanup(void)
{
  test_return_value = TET_PASS;
}

// Positive test case for a method
int UtcDaliPinchGestureConstructor(void)
{
  TestApplication application; // Reset all test adapter return codes

  PinchGesture gesture = DevelPinchGesture::New( GestureState::STARTED );
  DALI_TEST_EQUALS(GestureState::STARTED, gesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture.GetScale(), TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture.GetSpeed(), TEST_LOCATION);
  DALI_TEST_EQUALS(GestureType::PINCH, gesture.GetType(), TEST_LOCATION);

  PinchGesture gesture2 = DevelPinchGesture::New( GestureState::CONTINUING );
  DALI_TEST_EQUALS(GestureState::CONTINUING, gesture2.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture2.GetScale(), TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture2.GetSpeed(), TEST_LOCATION);
  DALI_TEST_EQUALS(GestureType::PINCH, gesture2.GetType(), TEST_LOCATION);

  PinchGesture gesture3 = DevelPinchGesture::New( GestureState::FINISHED );
  DALI_TEST_EQUALS(GestureState::FINISHED, gesture3.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture3.GetScale(), TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture3.GetSpeed(), TEST_LOCATION);
  DALI_TEST_EQUALS(GestureType::PINCH, gesture3.GetType(), TEST_LOCATION);

  // Test copy constructor
  GetImplementation( gesture3 ).SetScale( 3.0f );
  GetImplementation( gesture3 ).SetSpeed( 5.0f );

  PinchGesture pinch(gesture3);
  DALI_TEST_EQUALS(GestureState::FINISHED, pinch.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(3.0f, pinch.GetScale(), TEST_LOCATION);
  DALI_TEST_EQUALS(5.0f, pinch.GetSpeed(), TEST_LOCATION);
  DALI_TEST_EQUALS(GestureType::PINCH, pinch.GetType(), TEST_LOCATION);

  // Test move constructor
  const auto refCount = gesture.GetObjectPtr()->ReferenceCount();
  PinchGesture gesture4( std::move( gesture ) );
  DALI_TEST_CHECK(!gesture);
  DALI_TEST_EQUALS(GestureType::PINCH, gesture4.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(gesture4.GetBaseObject().ReferenceCount(), refCount, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPinchGestureAssignment(void)
{
  // Test Assignment operator
  PinchGesture gesture = DevelPinchGesture::New( GestureState::STARTED );
  DALI_TEST_EQUALS(GestureState::STARTED, gesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture.GetScale(), TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture.GetSpeed(), TEST_LOCATION);
  DALI_TEST_EQUALS(GestureType::PINCH, gesture.GetType(), TEST_LOCATION);

  PinchGesture gesture2 = DevelPinchGesture::New( GestureState::CONTINUING );
  DALI_TEST_EQUALS(GestureState::CONTINUING, gesture2.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture2.GetScale(), TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture2.GetSpeed(), TEST_LOCATION);
  DALI_TEST_EQUALS(GestureType::PINCH, gesture2.GetType(), TEST_LOCATION);

  GetImplementation( gesture2 ).SetScale( 3.0f );
  GetImplementation( gesture2 ).SetSpeed( 5.0f );

  gesture = gesture2;
  DALI_TEST_EQUALS(GestureState::CONTINUING, gesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(3.0f, gesture.GetScale(), TEST_LOCATION);
  DALI_TEST_EQUALS(5.0f, gesture.GetSpeed(), TEST_LOCATION);
  DALI_TEST_EQUALS(GestureType::PINCH, gesture.GetType(), TEST_LOCATION);

  // Move assignment
  const auto refCount = gesture.GetObjectPtr()->ReferenceCount();
  PinchGesture gesture3;
  DALI_TEST_EQUALS(gesture3, Gesture(), TEST_LOCATION);
  gesture3 = std::move(gesture);
  DALI_TEST_CHECK(!gesture);
  DALI_TEST_EQUALS(GestureType::PINCH, gesture3.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(gesture3.GetBaseObject().ReferenceCount(), refCount, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPinchGestureSetGetScaleP(void)
{
  PinchGesture gesture = DevelPinchGesture::New( GestureState::STARTED );
  DALI_TEST_EQUALS(gesture.GetScale(), 0.0f, TEST_LOCATION);

  GetImplementation(gesture).SetScale(123.0f);
  DALI_TEST_EQUALS(gesture.GetScale(), 123.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPinchGestureSetGetSpeedP(void)
{
  PinchGesture gesture = DevelPinchGesture::New( GestureState::STARTED );
  DALI_TEST_EQUALS(gesture.GetSpeed(), 0.0f, TEST_LOCATION);

  GetImplementation(gesture).SetSpeed(123.0f);
  DALI_TEST_EQUALS(gesture.GetSpeed(), 123.0f, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPinchGestureSetGetScreenCenterPointP(void)
{
  PinchGesture gesture = DevelPinchGesture::New( GestureState::STARTED );
  DALI_TEST_EQUALS(gesture.GetScreenCenterPoint(), Vector2::ZERO, TEST_LOCATION);

  GetImplementation(gesture).SetScreenCenterPoint(Vector2(123.0f, 321.0f));
  DALI_TEST_EQUALS(gesture.GetScreenCenterPoint(), Vector2(123.0f,321.0f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliPinchGestureSetGetLocalCenterPointP(void)
{
  PinchGesture gesture = DevelPinchGesture::New( GestureState::STARTED );
  DALI_TEST_EQUALS(gesture.GetLocalCenterPoint(), Vector2::ZERO, TEST_LOCATION);

  GetImplementation(gesture).SetLocalCenterPoint(Vector2(123.0f,321.0f));
  DALI_TEST_EQUALS(gesture.GetLocalCenterPoint(), Vector2(123.0f,321.0f), TEST_LOCATION);

  END_TEST;
}
