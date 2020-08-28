/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/input-options.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali-test-suite-utils.h>


using namespace Dali;

void utc_dali_rotation_gesture_recognizer_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_rotation_gesture_recognizer_cleanup(void)
{
  test_return_value = TET_PASS;
}

///////////////////////////////////////////////////////////////////////////////
namespace
{

struct SignalData
{
  SignalData()
  : functorCalled(false),
    voidFunctorCalled(false),
    receivedGesture(Gesture::Started)
  {}

  void Reset()
  {
    functorCalled = false;
    voidFunctorCalled = false;

    receivedGesture.state = Gesture::Started;
    receivedGesture.rotation = 0.0f;
    receivedGesture.screenCenterPoint = Vector2(0.0f, 0.0f);
    receivedGesture.localCenterPoint = Vector2(0.0f, 0.0f);

    rotatedActor.Reset();
  }

  bool functorCalled;
  bool voidFunctorCalled;
  RotationGesture receivedGesture;
  Actor rotatedActor;
};

// Functor that sets the data when called
struct GestureReceivedFunctor
{
  GestureReceivedFunctor(SignalData& data) : signalData(data) { }

  void operator()(Actor actor, const RotationGesture& rotation)
  {
    signalData.functorCalled = true;
    signalData.receivedGesture = rotation;
    signalData.rotatedActor = actor;
  }

  void operator()()
  {
    signalData.voidFunctorCalled = true;
  }

  SignalData& signalData;
};

Integration::TouchEvent GenerateDoubleTouch( PointState::Type stateA, const Vector2& screenPositionA, PointState::Type stateB, const Vector2& screenPositionB, uint32_t time )
{
  Integration::TouchEvent touchEvent;
  Integration::Point point;
  point.SetState( stateA );
  point.SetScreenPosition( screenPositionA );
  point.SetDeviceClass( Device::Class::TOUCH );
  point.SetDeviceSubclass( Device::Subclass::NONE );
  touchEvent.points.push_back( point );
  point.SetScreenPosition( screenPositionB );
  point.SetState( stateB);
  touchEvent.points.push_back( point );
  touchEvent.time = time;
  return touchEvent;
}


} // anon namespace

///////////////////////////////////////////////////////////////////////////////

int UtcDaliRotationGestureRecognizerMinimumTouchEvents(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty( Actor::Property::SIZE, Vector2( 100.0f, 100.0f ) );
  actor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  application.GetScene().Add( actor );

  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor( data );

  RotationGestureDetector detector = RotationGestureDetector::New();
  detector.Attach( actor );
  detector.DetectedSignal().Connect( &application, functor );

  // Case 1
  // 2 touch events make a gesture begin
  Integration::SetRotationGestureMinimumTouchEvents( 2 );
  application.ProcessEvent( GenerateDoubleTouch( PointState::DOWN, Vector2( 20.0f, 20.0f ), PointState::DOWN, Vector2( 20.0f, 90.0f ), 150 ) );
  application.ProcessEvent( GenerateDoubleTouch( PointState::MOTION, Vector2( 20.0f, 20.0f ), PointState::MOTION, Vector2( 90.0f, 90.0f ), 160 ) );

  DALI_TEST_EQUALS( Gesture::Started, data.receivedGesture.state, TEST_LOCATION );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Case 2
  // 4 touch events make a gesture begin
  Integration::SetRotationGestureMinimumTouchEvents( 4 );
  application.ProcessEvent( GenerateDoubleTouch( PointState::DOWN, Vector2( 20.0f, 20.0f ), PointState::DOWN, Vector2( 20.0f, 90.0f ), 150 ) );
  application.ProcessEvent( GenerateDoubleTouch( PointState::MOTION, Vector2( 20.0f, 20.0f ), PointState::MOTION, Vector2( 90.0f, 90.0f ), 160 ) );

  // Check the gesture is not detected unlike previous case
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );

  END_TEST;
}

int UtcDaliRotationGestureRecognizerMinimumTouchEventsAfterStart(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetProperty( Actor::Property::SIZE, Vector2( 100.0f, 100.0f ) );
  actor.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
  application.GetScene().Add( actor );

  application.SendNotification();
  application.Render();

  SignalData data;
  GestureReceivedFunctor functor( data );

  RotationGestureDetector detector = RotationGestureDetector::New();
  detector.Attach( actor );
  detector.DetectedSignal().Connect( &application, functor );

  // Case 1
  // > 2 touch events make a gesture begin
  // > 4 touch events generate gestures after begin
  Integration::SetRotationGestureMinimumTouchEvents(2);
  Integration::SetRotationGestureMinimumTouchEventsAfterStart(6);

  application.ProcessEvent( GenerateDoubleTouch( PointState::DOWN, Vector2( 20.0f, 20.0f ), PointState::DOWN, Vector2( 20.0f, 90.0f ), 150 ) );
  application.ProcessEvent( GenerateDoubleTouch( PointState::MOTION, Vector2( 20.0f, 20.0f ), PointState::MOTION, Vector2( 90.0f, 90.0f ), 160 ) );

  DALI_TEST_EQUALS(Gesture::Started, data.receivedGesture.state, TEST_LOCATION);
  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  application.ProcessEvent( GenerateDoubleTouch( PointState::MOTION, Vector2( 20.0f, 20.0f ), PointState::MOTION, Vector2( 20.0f, 90.0f ), 170 ) );
  application.ProcessEvent( GenerateDoubleTouch( PointState::MOTION, Vector2( 20.0f, 20.0f ), PointState::MOTION, Vector2( 20.0f, 90.0f ), 180 ) );
  application.ProcessEvent( GenerateDoubleTouch( PointState::MOTION, Vector2( 20.0f, 20.0f ), PointState::MOTION, Vector2( 20.0f, 90.0f ), 190 ) );
  application.ProcessEvent( GenerateDoubleTouch( PointState::MOTION, Vector2( 20.0f, 20.0f ), PointState::MOTION, Vector2( 20.0f, 90.0f ), 200 ) );
  // > Test : not enough touch events to make the gesture state "Continuing"
  DALI_TEST_EQUALS(Gesture::Started, data.receivedGesture.state, TEST_LOCATION);

  application.ProcessEvent( GenerateDoubleTouch( PointState::MOTION, Vector2( 20.0f, 20.0f ), PointState::MOTION, Vector2( 20.0f, 90.0f ), 210 ) );
  application.ProcessEvent( GenerateDoubleTouch( PointState::MOTION, Vector2( 20.0f, 20.0f ), PointState::MOTION, Vector2( 20.0f, 90.0f ), 220 ) );
  // > Test : 6 touch events after start make the gesture state "Continuing"
  DALI_TEST_EQUALS(Gesture::Continuing, data.receivedGesture.state, TEST_LOCATION);

  END_TEST;
}
