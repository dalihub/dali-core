//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <iostream>

#include <stdlib.h>
#include <dali/dali.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

void utc_dali_tap_gesture_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_tap_gesture_cleanup(void)
{
  test_return_value = TET_PASS;
}


// Positive test case for a method
int UtcDaliTapGestureConstructor(void)
{
  TestApplication application; // Reset all test adapter return codes

  TapGesture gesture;
  DALI_TEST_EQUALS(1u, gesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, gesture.numberOfTaps, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Tap, gesture.type, TEST_LOCATION);

  // Test Copy constructor
  gesture.numberOfTouches = 5u;
  gesture.numberOfTaps = 2u;

  TapGesture gesture2(gesture);
  DALI_TEST_EQUALS(5u, gesture2.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(2u, gesture2.numberOfTaps, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Tap, gesture2.type, TEST_LOCATION);
  END_TEST;
}

int UtcDaliTapGestureAssignment(void)
{
  // Test Assignment operator
  TapGesture gesture;
  DALI_TEST_EQUALS(1u, gesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, gesture.numberOfTaps, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Tap, gesture.type, TEST_LOCATION);

  gesture.numberOfTouches = 5u;
  gesture.numberOfTaps = 2u;

  TapGesture gesture2;
  gesture2 = gesture;
  DALI_TEST_EQUALS(5u, gesture2.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(2u, gesture2.numberOfTaps, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Tap, gesture2.type, TEST_LOCATION);
  END_TEST;
}
