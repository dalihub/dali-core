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

  PanGesture gesture;
  DALI_TEST_EQUALS(Gesture::Clear, gesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, gesture.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pan, gesture.type, TEST_LOCATION);

  PanGesture gesture2(Gesture::Started);
  DALI_TEST_EQUALS(Gesture::Started, gesture2.state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, gesture2.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pan, gesture2.type, TEST_LOCATION);

  PanGesture gesture3(Gesture::Continuing);
  DALI_TEST_EQUALS(Gesture::Continuing, gesture3.state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, gesture3.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pan, gesture3.type, TEST_LOCATION);

  PanGesture gesture4(Gesture::Finished);
  DALI_TEST_EQUALS(Gesture::Finished, gesture4.state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, gesture4.numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pan, gesture4.type, TEST_LOCATION);

  // Test copy constructor
  gesture4.numberOfTouches = 3u;

  PanGesture pan(gesture4);
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

int UtcDaliPanGestureDynamicAllocation(void)
{
  PanGesture* gesture = new PanGesture( Gesture::Started );
  DALI_TEST_EQUALS(Gesture::Started, gesture->state, TEST_LOCATION);
  DALI_TEST_EQUALS(1u, gesture->numberOfTouches, TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pan, gesture->type, TEST_LOCATION);
  delete gesture;

  END_TEST;
}

int UtcDaliPanGestureDetectorRegisterProperty(void)
{
  TestApplication application;

  GestureDetector detector = PanGestureDetector::New();

  Property::Index index = detector.RegisterProperty( "sceneProperty", 0 );
  DALI_TEST_EQUALS( index, (Property::Index)PROPERTY_CUSTOM_START_INDEX, TEST_LOCATION );
  DALI_TEST_EQUALS( detector.GetProperty< int32_t >( index ), 0, TEST_LOCATION );
  detector.SetProperty( index, -99 );

  using Dali::Animation;
  Animation animation = Animation::New( 1.0f );
  animation.AnimateTo( Property( detector, index ), 99 );
  DALI_TEST_EQUALS( detector.GetProperty< int32_t >( index ), -99, TEST_LOCATION );

  // create another pan gesture
  GestureDetector detector2 = PanGestureDetector::New();
  DALI_TEST_EQUALS( detector2.GetProperty< int32_t >( index ), 0, TEST_LOCATION );

  // Start the animation
  animation.Play();
  application.SendNotification();
  application.Render( 500 /* 50% progress */);
  DALI_TEST_EQUALS( detector.GetCurrentProperty< int32_t >( index ), 0 /*half way*/, TEST_LOCATION );

  // register another pan gesture value
  Property::Index index2 = detector2.RegisterProperty( "sceneProperty2", 12 );
  DALI_TEST_EQUALS( index2, (Property::Index)PROPERTY_CUSTOM_START_INDEX, TEST_LOCATION );
  DALI_TEST_EQUALS( detector2.GetProperty< int32_t >( index2 ), 12, TEST_LOCATION );
  DALI_TEST_EQUALS( detector2.GetCurrentProperty< int32_t >( index2 ), 12, TEST_LOCATION );

  DALI_TEST_EQUALS( detector.GetProperty< int32_t >( index ), 99 /*target*/, TEST_LOCATION );
  DALI_TEST_EQUALS( detector.GetCurrentProperty< int32_t >( index ), 0, TEST_LOCATION );

  Animation animation2 = Animation::New( 1.0f );
  animation2.AnimateTo( Property( detector2, index2 ), -99 );
  // Start the animation
  animation2.Play();
  application.SendNotification();
  application.Render( 1000 /* 100% more progress */);

  DALI_TEST_EQUALS( detector2.GetProperty< int32_t >( index2 ), -99, TEST_LOCATION );
  DALI_TEST_EQUALS( detector2.GetCurrentProperty< int32_t >( index2 ), -99, TEST_LOCATION );

  DALI_TEST_EQUALS( detector.GetProperty< int32_t >( index ), 99, TEST_LOCATION );
  DALI_TEST_EQUALS( detector.GetCurrentProperty< int32_t >( index ), 99, TEST_LOCATION );

  END_TEST;
}

