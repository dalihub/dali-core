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
#include <tet_api.h>

#include <dali/public-api/dali-core.h>

#include <dali-test-suite-utils.h>

using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliPinchGestureConstructor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPinchGestureAssignment, POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}


// Positive test case for a method
static void UtcDaliPinchGestureConstructor()
{
  TestApplication application; // Reset all test adapter return codes

  PinchGesture gesture(Gesture::Started);
  DALI_TEST_EQUALS(Gesture::Started, gesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture.scale, TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture.speed, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pinch, gesture.type, TEST_LOCATION);

  PinchGesture gesture2(Gesture::Continuing);
  DALI_TEST_EQUALS(Gesture::Continuing, gesture2.state, TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture2.scale, TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture2.speed, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pinch, gesture2.type, TEST_LOCATION);

  PinchGesture gesture3(Gesture::Finished);
  DALI_TEST_EQUALS(Gesture::Finished, gesture3.state, TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture3.scale, TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture3.speed, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pinch, gesture3.type, TEST_LOCATION);

  // Test copy constructor
  gesture3.scale = 3.0f;
  gesture3.speed = 5.0f;

  PinchGesture pinch(gesture3);
  DALI_TEST_EQUALS(Gesture::Finished, pinch.state, TEST_LOCATION);
  DALI_TEST_EQUALS(3.0f, pinch.scale, TEST_LOCATION);
  DALI_TEST_EQUALS(5.0f, pinch.speed, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pinch, pinch.type, TEST_LOCATION);
}

static void UtcDaliPinchGestureAssignment()
{
  // Test Assignment operator
  PinchGesture gesture(Gesture::Started);
  DALI_TEST_EQUALS(Gesture::Started, gesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture.scale, TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture.speed, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pinch, gesture.type, TEST_LOCATION);

  PinchGesture gesture2(Gesture::Continuing);
  DALI_TEST_EQUALS(Gesture::Continuing, gesture2.state, TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture2.scale, TEST_LOCATION);
  DALI_TEST_EQUALS(0.0f, gesture2.speed, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pinch, gesture2.type, TEST_LOCATION);

  gesture2.scale = 3.0f;
  gesture2.speed = 5.0f;

  gesture = gesture2;
  DALI_TEST_EQUALS(Gesture::Continuing, gesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(3.0f, gesture.scale, TEST_LOCATION);
  DALI_TEST_EQUALS(5.0f, gesture.speed, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pinch, gesture.type, TEST_LOCATION);
}




