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
#include <dali/integration-api/input-options.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/render-task-list-integ.h>
#include <dali/internal/event/events/pan-gesture-event.h>
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

    pannedActor.Reset();
  }

  bool functorCalled;
  bool voidFunctorCalled;
  PanGesture receivedGesture;
  Actor pannedActor;
};

// Functor that sets the data when called
struct GestureReceivedFunctor
{
  GestureReceivedFunctor(SignalData& data) : signalData(data) { }

  void operator()(Actor actor, const PanGesture& pan)
  {
    signalData.functorCalled = true;
    signalData.receivedGesture = pan;
    signalData.pannedActor = actor;
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


int UtcDaliPanGestureRecognizerBasicNoAction(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

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

  END_TEST;
}

int UtcDaliPanGestureRecognizerBasic(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

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
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 40.0f ), 151 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 60.0f ), 152 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerBasicInterrupted(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

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
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 25.0f ), 151 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::INTERRUPTED, Vector2( 20.0f, 30.0f ), 152 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerBasicShortest(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

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
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 40.0f ), 151 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::UP, Vector2( 20.0f, 40.0f ), 155 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerBasicFailToStart(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

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
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 40.0f ), 151 ) );
  application.ProcessEvent( GenerateDoubleTouch( PointState::MOTION, Vector2( 20.0f, 50.0f ), PointState::DOWN, Vector2( 40.0f, 40.0f ), 153 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerBasicStationary(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

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
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 40.0f ), 151 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 50.0f ), 152 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::STATIONARY, Vector2( 20.0f, 50.0f ), 153 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 55.0f ), 154 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerNewParametersFail(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  detector.SetMaximumTouchesRequired(2);
  detector.SetMinimumTouchesRequired(2);

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
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 40.0f ), 151 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 60.0f ), 152 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerNewParametersSuccess(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  detector.SetMaximumTouchesRequired(2);
  detector.SetMinimumTouchesRequired(2);

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

  application.ProcessEvent( GenerateDoubleTouch( PointState::DOWN, Vector2( 20.0f, 50.0f ), PointState::DOWN, Vector2( 20.0f, 40.0f ), 150 ) );
  application.ProcessEvent( GenerateDoubleTouch( PointState::MOTION, Vector2( 40.0f, 50.0f ), PointState::MOTION, Vector2( 40.0f, 40.0f ), 151 ) );
  application.ProcessEvent( GenerateDoubleTouch( PointState::MOTION, Vector2( 50.0f, 50.0f ), PointState::MOTION, Vector2( 50.0f, 40.0f ), 152 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerNewParametersEndFewerTouches01(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  detector.SetMaximumTouchesRequired(2);
  detector.SetMinimumTouchesRequired(2);

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

  application.ProcessEvent( GenerateDoubleTouch( PointState::DOWN, Vector2( 20.0f, 50.0f ), PointState::DOWN, Vector2( 20.0f, 40.0f ), 150 ) );
  application.ProcessEvent( GenerateDoubleTouch( PointState::MOTION, Vector2( 40.0f, 50.0f ), PointState::MOTION, Vector2( 40.0f, 40.0f ), 151 ) );
  application.ProcessEvent( GenerateDoubleTouch( PointState::MOTION, Vector2( 50.0f, 50.0f ), PointState::MOTION, Vector2( 50.0f, 40.0f ), 152 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::STATIONARY, Vector2( 50.0f, 50.0f ), 153 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerNewParametersEndFewerTouches02(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  detector.SetMaximumTouchesRequired(2);
  detector.SetMinimumTouchesRequired(2);

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

  application.ProcessEvent( GenerateDoubleTouch( PointState::DOWN, Vector2( 20.0f, 50.0f ), PointState::DOWN, Vector2( 20.0f, 40.0f ), 150 ) );
  application.ProcessEvent( GenerateDoubleTouch( PointState::MOTION, Vector2( 40.0f, 50.0f ), PointState::MOTION, Vector2( 40.0f, 40.0f ), 151 ) );
  application.ProcessEvent( GenerateDoubleTouch( PointState::MOTION, Vector2( 50.0f, 50.0f ), PointState::MOTION, Vector2( 50.0f, 40.0f ), 152 ) );
  application.ProcessEvent( GenerateDoubleTouch( PointState::STATIONARY, Vector2( 50.0f, 50.0f ), PointState::UP, Vector2( 50.0f, 40.0f ), 153 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerNewParametersNoStart(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  detector.SetMaximumTouchesRequired(2);
  detector.SetMinimumTouchesRequired(2);

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

  application.ProcessEvent( GenerateDoubleTouch( PointState::DOWN, Vector2( 20.0f, 50.0f ), PointState::DOWN, Vector2( 20.0f, 40.0f ), 150 ) );
  application.ProcessEvent( GenerateDoubleTouch( PointState::MOTION, Vector2( 40.0f, 50.0f ), PointState::MOTION, Vector2( 40.0f, 40.0f ), 151 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::UP, Vector2( 50.0f, 50.0f ), 153 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerNewParametersSlowRelease(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

  detector.SetMaximumTouchesRequired(2);
  detector.SetMinimumTouchesRequired(2);

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

  application.ProcessEvent( GenerateDoubleTouch( PointState::DOWN, Vector2( 20.0f, 50.0f ), PointState::DOWN, Vector2( 20.0f, 40.0f ), 150 ) );
  application.ProcessEvent( GenerateDoubleTouch( PointState::MOTION, Vector2( 40.0f, 50.0f ), PointState::MOTION, Vector2( 40.0f, 40.0f ), 151 ) );
  application.ProcessEvent( GenerateDoubleTouch( PointState::MOTION, Vector2( 50.0f, 50.0f ), PointState::MOTION, Vector2( 50.0f, 40.0f ), 152 ) );
  application.ProcessEvent( GenerateDoubleTouch( PointState::MOTION, Vector2( 60.0f, 50.0f ), PointState::MOTION, Vector2( 60.0f, 40.0f ), 153 ) );
  application.ProcessEvent( GenerateDoubleTouch( PointState::MOTION, Vector2( 70.0f, 50.0f ), PointState::MOTION, Vector2( 70.0f, 40.0f ), 154 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::UP, Vector2( 70.0f, 50.0f ), 155 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerOtherEvent(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

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
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 40.0f ), 151 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 60.0f ), 152 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 20.0f, 60.0f ), 153 ) );      // Exercise default case in Started case. Not sure if realistic
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 65.0f ), 154 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerSlowMoving(void)
{
  TestApplication application;

  PanGestureDetector detector = PanGestureDetector::New();

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
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 40.0f ), 251 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 60.0f ), 352 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 70.0f ), 453 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 80.0f ), 554 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 90.0f ), 655 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerNewParamsMinNum(void)
{
  TestApplication application;

  Integration::SetPanGestureMinimumPanEvents(8);


  PanGestureDetector detector = PanGestureDetector::New();

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
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 40.0f ), 251 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 60.0f ), 352 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 70.0f ), 453 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 80.0f ), 554 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 90.0f ), 655 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliPanGestureRecognizerNewParamsMinDistance(void)
{
  TestApplication application;

  Integration::SetPanGestureMinimumDistance(100);

  PanGestureDetector detector = PanGestureDetector::New();

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
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 40.0f ), 251 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 60.0f ), 352 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 70.0f ), 453 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 80.0f ), 554 ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 20.0f, 90.0f ), 655 ) );

  application.SendNotification();

  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}
