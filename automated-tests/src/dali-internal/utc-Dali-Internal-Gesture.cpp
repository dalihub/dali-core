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
#include <dali/internal/event/events/gesture-impl.h>

using namespace Dali;

void utc_dali_internal_gesture_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_internal_gesture_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

// TestGesture Struct (Gesture constructor is protected)
struct TestGesture : public Internal::Gesture
{
public:
  TestGesture(GestureType::Value type, GestureState state)
  : Gesture(type, state) {}

  virtual ~TestGesture() {}
};

} // anon namespace

int UtcDaliGestureConstructorP(void)
{
  TestApplication application; // Reset all test adapter return codes

  Gesture empty;
  DALI_TEST_CHECK( !empty );

  Gesture pan( new TestGesture(GestureType::PAN, GestureState::STARTED) );
  DALI_TEST_EQUALS(GestureType::PAN, pan.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::STARTED, pan.GetState(), TEST_LOCATION);

  Gesture pinch( new TestGesture(GestureType::PINCH, GestureState::CLEAR) );
  DALI_TEST_EQUALS(GestureType::PINCH, pinch.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::CLEAR, pinch.GetState(), TEST_LOCATION);

  // Test copy constructor
  Gesture pan2(pan);
  DALI_TEST_EQUALS(GestureType::PAN, pan2.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::STARTED, pan2.GetState(), TEST_LOCATION);
  END_TEST;

  // Test move constructor
  const auto refCount = pan.GetObjectPtr()->ReferenceCount();
  Gesture pan3( std::move( pan ) );
  DALI_TEST_EQUALS(pan, Gesture(), TEST_LOCATION);
  DALI_TEST_EQUALS(GestureType::PAN, pan3.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(pan3.GetBaseObject().ReferenceCount(), refCount, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGestureAssignmentP(void)
{
  TestApplication application; // Reset all test adapter return codes

  // Test Assignment operator
  Gesture pan( new TestGesture(GestureType::PAN, GestureState::FINISHED) );
  DALI_TEST_EQUALS(GestureType::PAN, pan.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::FINISHED, pan.GetState(), TEST_LOCATION);

  Gesture test( new TestGesture(GestureType::PINCH, GestureState::STARTED) );
  DALI_TEST_EQUALS(GestureType::PINCH, test.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::STARTED, test.GetState(), TEST_LOCATION);

  // Copy assignment
  test = pan;
  DALI_TEST_EQUALS(GestureType::PAN, test.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::FINISHED, test.GetState(), TEST_LOCATION);

  // Move assignment
  const auto refCount = pan.GetObjectPtr()->ReferenceCount();
  Gesture pan3;
  DALI_TEST_EQUALS(pan3, Gesture(), TEST_LOCATION);
  pan3 = std::move(pan);
  DALI_TEST_EQUALS(pan, Gesture(), TEST_LOCATION);
  DALI_TEST_EQUALS(GestureType::PAN, pan3.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(pan3.GetBaseObject().ReferenceCount(), refCount, TEST_LOCATION);

  END_TEST;
}

int UtcDaliGestureGetTypeP(void)
{
  TestApplication application; // Reset all test adapter return codes

  Gesture pan( new TestGesture(GestureType::PAN, GestureState::STARTED) );
  DALI_TEST_EQUALS(GestureType::PAN, pan.GetType(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliGestureGetStateP(void)
{
  TestApplication application; // Reset all test adapter return codes

  Gesture pan( new TestGesture(GestureType::PAN, GestureState::STARTED) );
  DALI_TEST_EQUALS(GestureState::STARTED, pan.GetState(), TEST_LOCATION);

  GetImplementation(pan).SetState(GestureState::FINISHED);
  DALI_TEST_EQUALS(GestureState::FINISHED, pan.GetState(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliGestureGetTimeP(void)
{
  TestApplication application; // Reset all test adapter return codes

  Gesture pan( new TestGesture(GestureType::PAN, GestureState::STARTED) );
  DALI_TEST_EQUALS(0, pan.GetTime(), TEST_LOCATION);

  GetImplementation(pan).SetTime(61282);
  DALI_TEST_EQUALS(61282, pan.GetTime(), TEST_LOCATION);

  END_TEST;
}

