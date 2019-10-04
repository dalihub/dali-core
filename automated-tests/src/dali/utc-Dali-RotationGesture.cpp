/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/events/gesture-devel.h>
#include <dali/devel-api/events/rotation-gesture.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

void utc_dali_rotation_gesture_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_rotation_gesture_cleanup(void)
{
  test_return_value = TET_PASS;
}

// Positive test case for a method
int UtcDaliRotationGestureConstructor(void)
{
  TestApplication application; // Reset all test adapter return codes

  RotationGesture gesture(Gesture::Started);
  DALI_TEST_EQUALS(Gesture::Started, gesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture.rotation.radian, TEST_LOCATION);
  DALI_TEST_EQUALS(DevelGesture::Rotation, static_cast< DevelGesture::Type >( gesture.type ), TEST_LOCATION);

  RotationGesture gesture2(Gesture::Continuing);
  DALI_TEST_EQUALS(Gesture::Continuing, gesture2.state, TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture2.rotation.radian, TEST_LOCATION);
  DALI_TEST_EQUALS(DevelGesture::Rotation, static_cast< DevelGesture::Type >( gesture2.type ), TEST_LOCATION);

  RotationGesture gesture3(Gesture::Finished);
  DALI_TEST_EQUALS(Gesture::Finished, gesture3.state, TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture3.rotation.radian, TEST_LOCATION);
  DALI_TEST_EQUALS(DevelGesture::Rotation, static_cast< DevelGesture::Type >( gesture3.type ), TEST_LOCATION);

  // Test copy constructor
  gesture3.rotation = 3.0f;

  RotationGesture rotation(gesture3);
  DALI_TEST_EQUALS(Gesture::Finished, rotation.state, TEST_LOCATION);
  DALI_TEST_EQUALS(3.0f, rotation.rotation.radian, TEST_LOCATION);
DALI_TEST_EQUALS(DevelGesture::Rotation, static_cast< DevelGesture::Type >( rotation.type ), TEST_LOCATION);
  END_TEST;
}

int UtcDaliRotationGestureAssignment(void)
{
  // Test Assignment operator
  RotationGesture gesture(Gesture::Started);
  DALI_TEST_EQUALS(Gesture::Started, gesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture.rotation.radian, TEST_LOCATION);
  DALI_TEST_EQUALS(DevelGesture::Rotation, static_cast< DevelGesture::Type >( gesture.type ), TEST_LOCATION);

  RotationGesture gesture2(Gesture::Continuing);
  DALI_TEST_EQUALS(Gesture::Continuing, gesture2.state, TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture2.rotation.radian, TEST_LOCATION);
  DALI_TEST_EQUALS(DevelGesture::Rotation, static_cast< DevelGesture::Type >( gesture2.type ), TEST_LOCATION);

  gesture2.rotation.radian = 3.0f;

  gesture = gesture2;
  DALI_TEST_EQUALS(Gesture::Continuing, gesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(3.0f, gesture.rotation.radian, TEST_LOCATION);
  DALI_TEST_EQUALS(DevelGesture::Rotation, static_cast< DevelGesture::Type >( gesture.type ), TEST_LOCATION);
  END_TEST;
}

int UtcDaliRotationGestureDynamicAllocation(void)
{
  RotationGesture* gesture = new RotationGesture( Gesture::Started );
  DALI_TEST_EQUALS(Gesture::Started, gesture->state, TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture->rotation.radian, TEST_LOCATION);
  DALI_TEST_EQUALS(DevelGesture::Rotation, static_cast< DevelGesture::Type >( gesture->type ), TEST_LOCATION);
  delete gesture;

  END_TEST;
}
