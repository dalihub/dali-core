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
 * See the License for the specific language governing permissions an
 * d
 * limitations under the License.
 *
 */

#include <iostream>

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>
#include <dali/devel-api/events/pan-gesture-devel.h>

using namespace Dali;

void utc_dali_pan_gesture_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_pan_gesture_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliPanGestureConstructorP(void)
{
  TestApplication application; // Reset all test adapter return codes

  PanGesture gesture = DevelPanGesture::New( Gesture::Clear );

  DALI_TEST_EQUALS(Gesture::Clear, gesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, gesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pan, gesture.GetType(), TEST_LOCATION);

  PanGesture gesture2 = DevelPanGesture::New( Gesture::Started );
  DALI_TEST_EQUALS(Gesture::Started, gesture2.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, gesture2.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pan, gesture2.GetType(), TEST_LOCATION);

  PanGesture gesture3 = DevelPanGesture::New( Gesture::Continuing );
  DALI_TEST_EQUALS(Gesture::Continuing, gesture3.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, gesture3.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pan, gesture3.GetType(), TEST_LOCATION);

  PanGesture gesture4 = DevelPanGesture::New( Gesture::Finished );

  DALI_TEST_EQUALS(Gesture::Finished, gesture4.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, gesture4.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pan, gesture4.GetType(), TEST_LOCATION);

  // Test copy constructor
  DevelPanGesture::SetNumberOfTouches( gesture4, 3u );

  PanGesture pan(gesture4);
  DALI_TEST_EQUALS(Gesture::Finished, pan.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(3u, pan.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pan, pan.GetType(), TEST_LOCATION);

  // Test move constructor
  const auto refCount = gesture.GetObjectPtr()->ReferenceCount();
  PanGesture gesture5( std::move( gesture ) );
  DALI_TEST_CHECK(!gesture);
  DALI_TEST_EQUALS(Gesture::Pan, gesture5.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(gesture5.GetBaseObject().ReferenceCount(), refCount, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureAssignmentP(void)
{
  // Test Assignment operator
  PanGesture gesture = DevelPanGesture::New( Gesture::Started );
  DALI_TEST_EQUALS(Gesture::Started, gesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, gesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pan, gesture.GetType(), TEST_LOCATION);

  PanGesture gesture2 = DevelPanGesture::New( Gesture::Continuing );

  DALI_TEST_EQUALS(Gesture::Continuing, gesture2.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, gesture2.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pan, gesture2.GetType(), TEST_LOCATION);

  DevelPanGesture::SetNumberOfTouches( gesture2, 3u );

  gesture = gesture2;
  DALI_TEST_EQUALS(Gesture::Continuing, gesture.GetState(), TEST_LOCATION);
  DALI_TEST_EQUALS(3u, gesture.GetNumberOfTouches(), TEST_LOCATION);
  DALI_TEST_EQUALS(Gesture::Pan, gesture.GetType(), TEST_LOCATION);

  // Move assignment
  const auto refCount = gesture.GetObjectPtr()->ReferenceCount();
  PanGesture gesture3;
  DALI_TEST_EQUALS(gesture3, Gesture(), TEST_LOCATION);
  gesture3 = std::move(gesture);
  DALI_TEST_CHECK(!gesture);
  DALI_TEST_EQUALS(Gesture::Pan, gesture3.GetType(), TEST_LOCATION);
  DALI_TEST_EQUALS(gesture3.GetBaseObject().ReferenceCount(), refCount, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureGetSpeedP(void)
{
  PanGesture gesture = DevelPanGesture::New( Gesture::Started );
  DALI_TEST_EQUALS(0.0f, gesture.GetSpeed(), TEST_LOCATION);

  DevelPanGesture::SetVelocity( gesture, Vector2( 3.0f, -4.0f ) );

  DALI_TEST_EQUALS(5.0f, gesture.GetSpeed(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureGetDistanceP(void)
{
  PanGesture gesture = DevelPanGesture::New( Gesture::Started );
  DALI_TEST_EQUALS(0.0f, gesture.GetDistance(), TEST_LOCATION);

  DevelPanGesture::SetDisplacement( gesture, Vector2( -30.0f, -40.0f ) );

  DALI_TEST_EQUALS(50.0f, gesture.GetDistance(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureGetScreenSpeedP(void)
{
  PanGesture gesture = DevelPanGesture::New( Gesture::Started );
  DALI_TEST_EQUALS(0.0f, gesture.GetScreenSpeed(), TEST_LOCATION);

  DevelPanGesture::SetScreenVelocity( gesture, Vector2( 3.0f, -4.0f ) );

  DALI_TEST_EQUALS(5.0f, gesture.GetScreenSpeed(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureGetScreenDistanceP(void)
{
  PanGesture gesture = DevelPanGesture::New( Gesture::Started );
  DALI_TEST_EQUALS(0.0f, gesture.GetScreenDistance(), TEST_LOCATION);

  DevelPanGesture::SetScreenDisplacement( gesture, Vector2( -30.0f, -40.0f ) );

  DALI_TEST_EQUALS(50.0f, gesture.GetScreenDistance(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureDetectorRegisterPropertyP(void)
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

int UtcDaliPanGestureSetGetTimeP(void)
{
  PanGesture gesture = DevelPanGesture::New( Gesture::Started );
  DALI_TEST_EQUALS(0, gesture.GetTime(), TEST_LOCATION);

  DevelPanGesture::SetTime( gesture, 123123 );

  DALI_TEST_EQUALS(123123, gesture.GetTime(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSetGetVelocityP(void)
{
  PanGesture gesture = DevelPanGesture::New( Gesture::Started );
  DALI_TEST_EQUALS(Vector2::ZERO, gesture.GetVelocity(), TEST_LOCATION);

  DevelPanGesture::SetVelocity( gesture, Vector2(123.0f, 321.0f) );

  DALI_TEST_EQUALS(Vector2(123.0f, 321.0f), gesture.GetVelocity(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSetGetDisplacementP(void)
{
  PanGesture gesture = DevelPanGesture::New( Gesture::Started );
  DALI_TEST_EQUALS(Vector2::ZERO, gesture.GetDisplacement(), TEST_LOCATION);

  DevelPanGesture::SetDisplacement( gesture, Vector2(123.0f, 321.0f) );

  DALI_TEST_EQUALS(Vector2(123.0f, 321.0f), gesture.GetDisplacement(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSetGetPositionP(void)
{
  PanGesture gesture = DevelPanGesture::New( Gesture::Started );
  DALI_TEST_EQUALS(Vector2::ZERO, gesture.GetPosition(), TEST_LOCATION);

  DevelPanGesture::SetPosition( gesture, Vector2(123.0f, 321.0f) );

  DALI_TEST_EQUALS(Vector2(123.0f, 321.0f), gesture.GetPosition(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSetGetScreenVelocityP(void)
{
  PanGesture gesture = DevelPanGesture::New( Gesture::Started );
  DALI_TEST_EQUALS(Vector2::ZERO, gesture.GetScreenVelocity(), TEST_LOCATION);

  DevelPanGesture::SetScreenVelocity( gesture, Vector2(123.0f, 321.0f) );

  DALI_TEST_EQUALS(Vector2(123.0f, 321.0f), gesture.GetScreenVelocity(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSetGetScreenDisplacementP(void)
{
  PanGesture gesture = DevelPanGesture::New( Gesture::Started );
  DALI_TEST_EQUALS(Vector2::ZERO, gesture.GetScreenDisplacement(), TEST_LOCATION);

  DevelPanGesture::SetScreenDisplacement( gesture, Vector2(123.0f, 321.0f) );

  DALI_TEST_EQUALS(Vector2(123.0f, 321.0f), gesture.GetScreenDisplacement(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSetGetScreenPositionP(void)
{
  PanGesture gesture = DevelPanGesture::New( Gesture::Started );
  DALI_TEST_EQUALS(Vector2::ZERO, gesture.GetScreenPosition(), TEST_LOCATION);

  DevelPanGesture::SetScreenPosition( gesture, Vector2(123.0f, 321.0f) );

  DALI_TEST_EQUALS(Vector2(123.0f, 321.0f), gesture.GetScreenPosition(), TEST_LOCATION);
  END_TEST;
}

int UtcDaliPanGestureSetGetNumberOfTouchesP(void)
{
  PanGesture gesture = DevelPanGesture::New( Gesture::Started );
  DALI_TEST_EQUALS(1u, gesture.GetNumberOfTouches(), TEST_LOCATION);

  DevelPanGesture::SetNumberOfTouches( gesture, 3123 );

  DALI_TEST_EQUALS(3123, gesture.GetNumberOfTouches(), TEST_LOCATION);
  END_TEST;
}
