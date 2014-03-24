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

void utc_dali_gesture_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_gesture_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{

// TestGesture Struct (Gesture constructor is protected)
struct TestGesture : public Gesture
{
public:
  TestGesture(Gesture::Type type, Gesture::State state)
  : Gesture(type, state) {}

  virtual ~TestGesture() {}
};

} // anon namespace

int UtcDaliGestureConstructor(void)
{
  TestApplication application; // Reset all test adapter return codes

  TestGesture pan(Gesture::Pan, Gesture::Started);
  DALI_TEST_EQUALS(Gesture::Pan, pan.type, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Started, pan.state, TEST_LOCATION);

  TestGesture pinch(Gesture::Pinch, Gesture::Clear);
  DALI_TEST_EQUALS(Gesture::Pinch, pinch.type, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Clear, pinch.state, TEST_LOCATION);

  // Test copy constructor
  TestGesture pan2(pan);
  DALI_TEST_EQUALS(Gesture::Pan, pan2.type, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Started, pan2.state, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGestureAssignment(void)
{
  // Test Assignment operator
  TestGesture pan(Gesture::Pan, Gesture::Finished);
  DALI_TEST_EQUALS(Gesture::Pan, pan.type, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Finished, pan.state, TEST_LOCATION);

  TestGesture test(Gesture::Pinch, Gesture::Started);
  DALI_TEST_EQUALS(Gesture::Pinch, test.type, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Started, test.state, TEST_LOCATION);

  test = pan;
  DALI_TEST_EQUALS(Gesture::Pan, test.type, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Finished, test.state, TEST_LOCATION);
  END_TEST;
}
