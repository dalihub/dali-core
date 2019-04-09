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
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/render-task-list-integ.h>
#include <dali/internal/event/events/tap-gesture-event.h>
#include <dali-test-suite-utils.h>
#include <test-touch-utils.h>

using namespace Dali;

///////////////////////////////////////////////////////////////////////////////
namespace
{

struct SignalData
{
  SignalData()
  : functorCalled(false),
    voidFunctorCalled(false),
    receivedGesture()
  {}

  void Reset()
  {
    functorCalled = false;
    voidFunctorCalled = false;

    receivedGesture.state = Gesture::Started;

    tappedActor.Reset();
  }

  bool functorCalled;
  bool voidFunctorCalled;
  TapGesture receivedGesture;
  Actor tappedActor;
};

// Functor that sets the data when called
struct GestureReceivedFunctor
{
  GestureReceivedFunctor(SignalData& data) : signalData(data) { }

  void operator()(Actor actor, const TapGesture& tap)
  {
    signalData.functorCalled = true;
    signalData.receivedGesture = tap;
    signalData.tappedActor = actor;
  }

  void operator()()
  {
    signalData.voidFunctorCalled = true;
  }

  SignalData& signalData;
};


Integration::TouchEvent GenerateSingleTouch( PointState::Type state, const Vector2& screenPosition, uint32_t time )
{
  Integration::TouchEvent touchEvent;
  Integration::Point point;
  point.SetState( state );
  point.SetScreenPosition( screenPosition );
  point.SetDeviceClass( Device::Class::TOUCH );
  point.SetDeviceSubclass( Device::Subclass::NONE );
  touchEvent.points.push_back( point );
  touchEvent.time = time;
  return touchEvent;
}

Integration::TouchEvent GenerateDoubleTouch( PointState::Type state, const Vector2& screenPositionA, const Vector2& screenPositionB, uint32_t time )
{
  Integration::TouchEvent touchEvent;
  Integration::Point point;
  point.SetState( state );
  point.SetScreenPosition( screenPositionA );
  point.SetDeviceClass( Device::Class::TOUCH );
  point.SetDeviceSubclass( Device::Subclass::NONE );
  touchEvent.points.push_back( point );
  point.SetScreenPosition( screenPositionB );
  touchEvent.points.push_back( point );
  touchEvent.time = time;
  return touchEvent;
}


} // anon namespace

///////////////////////////////////////////////////////////////////////////////


int UtcDaliTapGestureRecognizerBasic(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  detector.Attach(actor);

  SignalData data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 20.0f, 20.0f ), 150 ) );

  application.ProcessEvent( GenerateSingleTouch( PointState::UP, Vector2( 20.0f, 20.0f ), 200 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}


int UtcDaliTapGestureRecognizerGapTooLong(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  detector.Attach(actor);

  SignalData data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 20.0f, 20.0f ), 150 ) );

  application.ProcessEvent( GenerateSingleTouch( PointState::UP, Vector2( 20.0f, 20.0f ), 651 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}


int UtcDaliTapGestureRecognizerInterrupted(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  detector.Attach(actor);

  SignalData data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 20.0f, 20.0f ), 150 ) );

  application.ProcessEvent( GenerateSingleTouch( PointState::INTERRUPTED, Vector2( 20.0f, 20.0f ), 175 ) );

  application.ProcessEvent( GenerateSingleTouch( PointState::UP, Vector2( 20.0f, 20.0f ), 200 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}


int UtcDaliTapGestureRecognizerMoveTooFar(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  detector.Attach(actor);

  SignalData data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 20.0f, 20.0f ), 150 ) );

  application.ProcessEvent( GenerateSingleTouch( PointState::UP, Vector2( 50.0f, 20.0f ), 200 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}


int UtcDaliTapGestureRecognizerStartDouble(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  detector.Attach(actor);

  SignalData data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  application.ProcessEvent( GenerateDoubleTouch( PointState::DOWN, Vector2( 20.0f, 20.0f ), Vector2( 25.0f, 25.0f ), 150 ) );

  application.ProcessEvent( GenerateSingleTouch( PointState::UP, Vector2( 20.0f, 20.0f ), 200 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}


int UtcDaliTapGestureRecognizerEndDouble(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  detector.Attach(actor);

  SignalData data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 20.0f, 20.0f ), 150 ) );

  application.ProcessEvent( GenerateDoubleTouch( PointState::DOWN, Vector2( 20.0f, 20.0f ), Vector2( 25.0f, 25.0f ), 200 ) );

  application.ProcessEvent( GenerateDoubleTouch( PointState::UP, Vector2( 20.0f, 20.0f ), Vector2( 25.0f, 25.0f ), 200 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}


int UtcDaliTapGestureRecognizerDoubleTap(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New(2);

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  detector.Attach(actor);

  SignalData data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 20.0f, 20.0f ), 150 ) );

  application.ProcessEvent( GenerateSingleTouch( PointState::UP, Vector2( 20.0f, 20.0f ), 200 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 20.0f, 20.0f ), 250 ) );

  application.ProcessEvent( GenerateSingleTouch( PointState::UP, Vector2( 20.0f, 20.0f ), 300 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}


int UtcDaliTapGestureRecognizerDoubleTapMoveTooFar(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New(2);

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  detector.Attach(actor);

  SignalData data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 20.0f, 20.0f ), 150 ) );

  application.ProcessEvent( GenerateSingleTouch( PointState::UP, Vector2( 20.0f, 20.0f ), 200 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 20.0f, 50.0f ), 250 ) );

  application.ProcessEvent( GenerateSingleTouch( PointState::UP, Vector2( 20.0f, 50.0f ), 300 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 50.0f, 50.0f ), 450 ) );

  application.ProcessEvent( GenerateSingleTouch( PointState::UP, Vector2( 50.0f, 50.0f ), 500 ) );

  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 50.0f, 50.0f ), 550 ) );

  application.ProcessEvent( GenerateSingleTouch( PointState::UP, Vector2( 50.0f, 50.0f ), 600 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}


int UtcDaliTapGestureRecognizerDoubleTapWaitTooLong(void)
{
  TestApplication application;

  TapGestureDetector detector = TapGestureDetector::New(2);

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  detector.Attach(actor);

  SignalData data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 20.0f, 20.0f ), 150 ) );

  application.ProcessEvent( GenerateSingleTouch( PointState::UP, Vector2( 20.0f, 20.0f ), 200 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 20.0f, 20.0f ), 650 ) );

  application.ProcessEvent( GenerateSingleTouch( PointState::UP, Vector2( 20.0f, 20.0f ), 750 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 50.0f, 50.0f ), 950 ) );

  application.ProcessEvent( GenerateSingleTouch( PointState::UP, Vector2( 50.0f, 50.0f ), 1000 ) );

  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 50.0f, 50.0f ), 1050 ) );

  application.ProcessEvent( GenerateSingleTouch( PointState::UP, Vector2( 50.0f, 50.0f ), 1000 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}


int UtcDaliTapGestureRecognizerMultipleDetectors(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  Actor actor2 = Actor::New();
  actor2.SetSize(100.0f, 100.0f);
  actor2.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  actor2.SetX(100.0f);
  Stage::GetCurrent().Add(actor2);

  // Render and notify
  application.SendNotification();
  application.Render();

  TapGestureDetector detector = TapGestureDetector::New();
  detector.Attach(actor);

  TapGestureDetector detector2 = TapGestureDetector::New(2);
  detector2.Attach(actor2);

  SignalData data;
  GestureReceivedFunctor functor(data);
  detector.DetectedSignal().Connect(&application, functor);

  SignalData data2;
  GestureReceivedFunctor functor2(data2);
  detector2.DetectedSignal().Connect(&application, functor2);

  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 20.0f, 20.0f ), 150 ) );

  application.ProcessEvent( GenerateSingleTouch( PointState::UP, Vector2( 20.0f, 20.0f ), 200 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, actor == data.tappedActor, TEST_LOCATION);
  data.Reset();
  DALI_TEST_EQUALS(false, data2.functorCalled, TEST_LOCATION);

  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 120.0f, 20.0f ), 250 ) );

  application.ProcessEvent( GenerateSingleTouch( PointState::UP, Vector2( 120.0f, 20.0f ), 300 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(false, data2.functorCalled, TEST_LOCATION);

  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 120.0f, 20.0f ), 350 ) );

  application.ProcessEvent( GenerateSingleTouch( PointState::UP, Vector2( 120.0f, 20.0f ), 400 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, data2.functorCalled, TEST_LOCATION);

  END_TEST;
}
