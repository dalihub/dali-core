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
#include <dali/internal/event/events/rotation-gesture/rotation-gesture-impl.h>

using namespace Dali;

void utc_dali_internal_rotation_gesture_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_rotation_gesture_cleanup(void)
{
  test_return_value = TET_PASS;
}

// Positive test case for a method
int UtcDaliRotationGestureConstructor(void)
{
  TestApplication application; // Reset all test adapter return codes

  RotationGesture gesture(new Internal::RotationGesture( GestureState::STARTED ));
  DALI_TEST_EQUALS(GestureState::STARTED, gesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture.GetRotation().radian, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureType::ROTATION, gesture.GetType(), TEST_LOCATION);

  RotationGesture gesture2(new Internal::RotationGesture( GestureState::CONTINUING ));
  DALI_TEST_EQUALS(GestureState::CONTINUING, gesture2.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture2.GetRotation().radian, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureType::ROTATION, gesture2.GetType(), TEST_LOCATION);

  RotationGesture gesture3(new Internal::RotationGesture( GestureState::FINISHED ));
  DALI_TEST_EQUALS(GestureState::FINISHED, gesture3.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture3.GetRotation().radian, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureType::ROTATION, gesture3.GetType(), TEST_LOCATION);

  // Test copy constructor
  GetImplementation( gesture3 ).SetRotation( Radian( 3.0f ));

  RotationGesture rotation(gesture3);
  DALI_TEST_EQUALS(GestureState::FINISHED, rotation.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(3.0f, rotation.GetRotation().radian, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureType::ROTATION, rotation.GetType(), TEST_LOCATION);

  // Test move constructor
  const auto refCount = gesture.GetObjectPtr()->ReferenceCount();
  RotationGesture gesture4( std::move( gesture ) );
  DALI_TEST_CHECK(!gesture);
  DALI_TEST_EQUALS(GestureType::ROTATION, gesture4.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(gesture4.GetBaseObject().ReferenceCount(), refCount, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRotationGestureAssignment(void)
{
  // Test Assignment operator
  RotationGesture gesture(new Internal::RotationGesture( GestureState::STARTED ));
  DALI_TEST_EQUALS(GestureState::STARTED, gesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture.GetRotation().radian, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureType::ROTATION, gesture.GetType(), TEST_LOCATION);

  RotationGesture gesture2(new Internal::RotationGesture( GestureState::CONTINUING ));
  DALI_TEST_EQUALS(GestureState::CONTINUING, gesture2.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture2.GetRotation().radian, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureType::ROTATION, gesture2.GetType(), TEST_LOCATION);

  GetImplementation( gesture2 ).SetRotation( Radian( 3.0f ));

  gesture = gesture2;
  DALI_TEST_EQUALS(GestureState::CONTINUING, gesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(3.0f, gesture.GetRotation().radian, TEST_LOCATION);
  DALI_TEST_EQUALS(GestureType::ROTATION, gesture.GetType(), TEST_LOCATION);

  // Move assignment
  const auto refCount = gesture.GetObjectPtr()->ReferenceCount();
  RotationGesture gesture3;
  DALI_TEST_EQUALS(gesture3, Gesture(), TEST_LOCATION);
  gesture3 = std::move(gesture);
  DALI_TEST_CHECK(!gesture);
  DALI_TEST_EQUALS(GestureType::ROTATION, gesture3.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(gesture3.GetBaseObject().ReferenceCount(), refCount, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRotationGestureSetGetRotationP(void)
{
  RotationGesture gesture(new Internal::RotationGesture(GestureState::STARTED));
  DALI_TEST_EQUALS(gesture.GetRotation(), Radian(), TEST_LOCATION);

  GetImplementation(gesture).SetRotation(Dali::ANGLE_270);
  DALI_TEST_EQUALS(gesture.GetRotation(), Dali::ANGLE_270, TEST_LOCATION);

  END_TEST;
}

int UtcDaliRotationGestureSetGetScreenCenterPointP(void)
{
  RotationGesture gesture(new Internal::RotationGesture(GestureState::STARTED));
  DALI_TEST_EQUALS(gesture.GetScreenCenterPoint(), Vector2::ZERO, TEST_LOCATION);

  GetImplementation(gesture).SetScreenCenterPoint(Vector2(123.0f,321.0f));
  DALI_TEST_EQUALS(gesture.GetScreenCenterPoint(), Vector2(123.0f,321.0f), TEST_LOCATION);

  END_TEST;
}

int UtcDaliRotationGestureSetGetLocalCenterPointP(void)
{
  RotationGesture gesture(new Internal::RotationGesture(GestureState::STARTED));
  DALI_TEST_EQUALS(gesture.GetLocalCenterPoint(), Vector2::ZERO, TEST_LOCATION);

  GetImplementation(gesture).SetLocalCenterPoint(Vector2(123.0f,321.0f));
  DALI_TEST_EQUALS(gesture.GetLocalCenterPoint(), Vector2(123.0f,321.0f), TEST_LOCATION);

  END_TEST;
}
