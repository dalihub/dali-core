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

void utc_dali_pan_gesture_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_pan_gesture_cleanup(void)
{
  test_return_value = TET_PASS;
}



int UtcDaliPanGestureConstructor(void)
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
  END_TEST;
}

int UtcDaliPanGestureAssignment(void)
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
  END_TEST;
}

int UtcDaliPanGestureGetSpeed(void)
{
  PanGesture gesture(Gesture::Started);
  DALI_TEST_EQUALS(0.0f, gesture.GetSpeed(), TEST_LOCATION);

  gesture.velocity = Vector2(3.0f, -4.0f);

  DALI_TEST_EQUALS(5.0f, gesture.GetSpeed(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureGetDistance(void)
{
  PanGesture gesture(Gesture::Started);
  DALI_TEST_EQUALS(0.0f, gesture.GetDistance(), TEST_LOCATION);

  gesture.displacement = Vector2(-30.0f, -40.0f);

  DALI_TEST_EQUALS(50.0f, gesture.GetDistance(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureGetScreenSpeed(void)
{
  PanGesture gesture(Gesture::Started);
  DALI_TEST_EQUALS(0.0f, gesture.GetScreenSpeed(), TEST_LOCATION);

  gesture.screenVelocity = Vector2(3.0f, -4.0f);

  DALI_TEST_EQUALS(5.0f, gesture.GetScreenSpeed(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureGetScreenDistance(void)
{
  PanGesture gesture(Gesture::Started);
  DALI_TEST_EQUALS(0.0f, gesture.GetScreenDistance(), TEST_LOCATION);

  gesture.screenDisplacement = Vector2(-30.0f, -40.0f);

  DALI_TEST_EQUALS(50.0f, gesture.GetScreenDistance(), TEST_LOCATION);
  END_TEST;
}
