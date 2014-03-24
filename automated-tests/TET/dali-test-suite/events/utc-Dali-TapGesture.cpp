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
TEST_FUNCTION( UtcDaliTapGestureConstructor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliTapGestureAssignment, POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}


// Positive test case for a method
static void UtcDaliTapGestureConstructor()
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
}

static void UtcDaliTapGestureAssignment()
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
}
