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
TEST_FUNCTION( UtcDaliPanGestureConstructor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureAssignment, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureGetSpeed, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureGetDistance, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureGetScreenSpeed, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliPanGestureGetScreenDistance, POSITIVE_TC_IDX );

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliPanGestureConstructor()
{
  TestApplication application; // Reset all test adapter return codes

  PanGesture gesture(Gesture::Started);
  DALI_TEST_EQUALS(Gesture::Started, gesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, gesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pan, gesture.type, TEST_LOCATION);

  PanGesture gesture2(Gesture::Continuing);
  DALI_TEST_EQUALS(Gesture::Continuing, gesture2.state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, gesture2.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pan, gesture2.type, TEST_LOCATION);

  PanGesture gesture3(Gesture::Finished);
  DALI_TEST_EQUALS(Gesture::Finished, gesture3.state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, gesture3.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pan, gesture3.type, TEST_LOCATION);

  // Test copy constructor
  gesture3.numberOfTouches = 3u;

  PanGesture pan(gesture3);
  DALI_TEST_EQUALS(Gesture::Finished, pan.state, TEST_LOCATION);
  DALI_TEST_EQUALS(3u, pan.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pan, pan.type, TEST_LOCATION);
}

static void UtcDaliPanGestureAssignment()
{
  // Test Assignment operator
  PanGesture gesture(Gesture::Started);
  DALI_TEST_EQUALS(Gesture::Started, gesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, gesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pan, gesture.type, TEST_LOCATION);

  PanGesture gesture2(Gesture::Continuing);
  DALI_TEST_EQUALS(Gesture::Continuing, gesture2.state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, gesture2.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pan, gesture2.type, TEST_LOCATION);

  gesture2.numberOfTouches = 3u;

  gesture = gesture2;
  DALI_TEST_EQUALS(Gesture::Continuing, gesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(3u, gesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pan, gesture.type, TEST_LOCATION);
}

static void UtcDaliPanGestureGetSpeed()
{
  PanGesture gesture(Gesture::Started);
  DALI_TEST_EQUALS(0.0f, gesture.GetSpeed(), TEST_LOCATION);

  gesture.velocity = Vector2(3.0f, -4.0f);

  DALI_TEST_EQUALS(5.0f, gesture.GetSpeed(), TEST_LOCATION);
}

static void UtcDaliPanGestureGetDistance()
{
  PanGesture gesture(Gesture::Started);
  DALI_TEST_EQUALS(0.0f, gesture.GetDistance(), TEST_LOCATION);

  gesture.displacement = Vector2(-30.0f, -40.0f);

  DALI_TEST_EQUALS(50.0f, gesture.GetDistance(), TEST_LOCATION);
}

static void UtcDaliPanGestureGetScreenSpeed()
{
  PanGesture gesture(Gesture::Started);
  DALI_TEST_EQUALS(0.0f, gesture.GetScreenSpeed(), TEST_LOCATION);

  gesture.screenVelocity = Vector2(3.0f, -4.0f);

  DALI_TEST_EQUALS(5.0f, gesture.GetScreenSpeed(), TEST_LOCATION);
}

static void UtcDaliPanGestureGetScreenDistance()
{
  PanGesture gesture(Gesture::Started);
  DALI_TEST_EQUALS(0.0f, gesture.GetScreenDistance(), TEST_LOCATION);

  gesture.screenDisplacement = Vector2(-30.0f, -40.0f);

  DALI_TEST_EQUALS(50.0f, gesture.GetScreenDistance(), TEST_LOCATION);
}
