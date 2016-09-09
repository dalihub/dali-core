/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/system-overlay.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

void utc_dali_touch_data_processing_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_touch_data_processing_cleanup(void)
{
  test_return_value = TET_PASS;
}

///////////////////////////////////////////////////////////////////////////////

namespace
{
struct TestPoint
{
  int32_t deviceId;
  PointState::Type state;
  Actor hitActor;
  Vector2 local;
  Vector2 screen;
  float radius;
  Vector2 ellipseRadius;
  float pressure;
  Degree angle;

  TestPoint()
  : deviceId(-1), state(PointState::FINISHED), radius(0), pressure(0)
  {
  }
  static const TestPoint ZERO;
};

const TestPoint TestPoint::ZERO;


// Stores data that is populated in the callback and will be read by the TET cases
struct SignalData
{
  SignalData()
  : functorCalled( false ),
    touchData(),
    touchedActor()
  {
  }

  struct TestTouchData
  {
    unsigned long time;
    std::vector<TestPoint> points;

    const TestPoint& GetPoint(size_t i)
    {
      if( i < points.size() )
      {
        return points[i];
      }
      return TestPoint::ZERO;
    }
    size_t GetPointCount()
    {
      return points.size();
    }
  };

  void Reset()
  {
    functorCalled = false;

    touchData.time = 0u;
    touchData.points.clear();

    touchedActor.Reset();
  }

  bool functorCalled;
  TestTouchData touchData;
  Actor touchedActor;
};

// Functor that sets the data when called
struct TouchDataFunctor
{
  /**
   * Constructor.
   * @param[in]  data         Reference to the data to store callback information.
   * @param[in]  returnValue  What the functor should return.
   */
  TouchDataFunctor( SignalData& data, bool returnValue = true )
  : signalData( data ),
    returnValue( returnValue )
  {
  }

  bool operator()( Actor actor, const TouchData& touchData )
  {
    signalData.functorCalled = true;
    signalData.touchedActor = actor;

    signalData.touchData.time = touchData.GetTime();
    signalData.touchData.points.clear();

    for( size_t i=0; i<touchData.GetPointCount(); ++i )
    {
      TestPoint p;
      p.deviceId = touchData.GetDeviceId(i);
      p.state = touchData.GetState(i);
      p.hitActor = touchData.GetHitActor(i);
      p.local = touchData.GetLocalPosition(i);
      p.screen = touchData.GetScreenPosition(i);
      p.radius = touchData.GetRadius(i);
      p.ellipseRadius = touchData.GetEllipseRadius(i);
      p.pressure = touchData.GetPressure(i);
      p.angle = touchData.GetAngle(i);
      signalData.touchData.points.push_back(p);
    }

    return returnValue;
  }

  SignalData& signalData;
  bool returnValue;
};

struct HandleData
{
  bool signalReceived;
  TouchData touchData;

  HandleData()
  : signalReceived(false)
  {
  }
};

struct TouchDataHandleFunctor
{
  /**
   * Constructor.
   * @param[in]  data         Reference to the data to store callback information.
   * @param[in]  returnValue  What the functor should return.
   */
  TouchDataHandleFunctor( HandleData& handleData, bool returnValue = true )
  : handleData(handleData),
    returnValue( returnValue )
  {
  }

  bool operator()( Actor actor, const TouchData& someTouchData )
  {
    handleData.signalReceived = true;
    TouchData handle(someTouchData);
    handleData.touchData = handle;
    return returnValue;
  }

  HandleData& handleData;
  bool returnValue;
};


// Functor that removes the actor when called.
struct RemoveActorFunctor : public TouchDataFunctor
{
  /**
   * Constructor.
   * @param[in]  data         Reference to the data to store callback information.
   * @param[in]  returnValue  What the functor should return.
   */
  RemoveActorFunctor( SignalData& data, bool returnValue = true )
  : TouchDataFunctor( data, returnValue )
  {
  }

  bool operator()( Actor actor, const TouchData& touchData )
  {
    Actor parent( actor.GetParent() );
    if ( parent )
    {
      parent.Remove( actor );
    }

    return TouchDataFunctor::operator()( actor, touchData );
  }
};

struct OutOfBoundsData
{
  TestPoint point;
  bool functorCalled;

  OutOfBoundsData()
  :functorCalled(false)
  {
  }
};

// Functor that reads out of bounds data when called
struct OutOfBoundsFunctor
{
  /**
   * Constructor.
   * @param[in]  data         Reference to the data to store callback information.
   * @param[in]  returnValue  What the functor should return.
   */
  OutOfBoundsFunctor( OutOfBoundsData& data, bool returnValue = true )
  : outOfBoundsData ( data ),
    returnValue( returnValue )
  {
  }

  bool operator()( Actor actor, const TouchData& touchData )
  {
    outOfBoundsData.functorCalled = true;
    size_t count = touchData.GetPointCount();

    // Read out of bounds data
    outOfBoundsData.point.deviceId = touchData.GetDeviceId(count+1);
    outOfBoundsData.point.state = touchData.GetState(count+1);
    outOfBoundsData.point.hitActor = touchData.GetHitActor(count+1);
    outOfBoundsData.point.local = touchData.GetLocalPosition(count+1);
    outOfBoundsData.point.screen = touchData.GetScreenPosition(count+1);

    return returnValue;
  }

  OutOfBoundsData& outOfBoundsData;
  bool returnValue;
};

struct TouchEventFunctor
{
  /**
   * Constructor.
   * @param[in]  functorCalled  Reference to a boolean which is set to true if the touch event functor is called.
   */
  TouchEventFunctor( bool& functorCalled )
  : functorCalled( functorCalled )
  {
  }

  bool operator()( Actor actor, const TouchEvent& touch )
  {
    functorCalled = true;
    return true;
  }

  bool& functorCalled;
};

Integration::TouchEvent GenerateSingleTouch( PointState::Type state, const Vector2& screenPosition )
{
  Integration::TouchEvent touchEvent;
  Integration::Point point;
  point.SetState( state );
  point.SetScreenPosition( screenPosition );
  touchEvent.points.push_back( point );
  return touchEvent;
}

} // anon namespace

///////////////////////////////////////////////////////////////////////////////

int UtcDaliTouchDataNormalProcessing01(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data );
  actor.TouchSignal().Connect( &application, functor );

  Vector2 screenCoordinates( 10.0f, 10.0f );
  Vector2 localCoordinates;
  actor.ScreenToLocal( localCoordinates.x, localCoordinates.y, screenCoordinates.x, screenCoordinates.y );

  // Emit a down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, screenCoordinates ) );
  const TestPoint *point1 = &data.touchData.GetPoint(0);
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, data.touchData.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, point1->state, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, point1->screen, TEST_LOCATION );
  DALI_TEST_EQUALS( localCoordinates, point1->local, 0.1f, TEST_LOCATION );
  data.Reset();

  // Emit a motion signal
  screenCoordinates.x = screenCoordinates.y = 11.0f;
  actor.ScreenToLocal( localCoordinates.x, localCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, screenCoordinates ) );
  const TestPoint *point2 = &data.touchData.GetPoint(0);
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, data.touchData.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::MOTION, point2->state, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, point2->screen, TEST_LOCATION );
  DALI_TEST_EQUALS( localCoordinates, point2->local, 0.1f, TEST_LOCATION );
  data.Reset();

  // Emit an up signal
  screenCoordinates.x = screenCoordinates.y = 12.0f;
  actor.ScreenToLocal( localCoordinates.x, localCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  application.ProcessEvent( GenerateSingleTouch( PointState::UP, screenCoordinates ) );
  const TestPoint *point3 = &data.touchData.GetPoint(0);
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, data.touchData.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::UP, point3->state, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, point3->screen, TEST_LOCATION );
  DALI_TEST_EQUALS( localCoordinates, point3->local, 0.1f, TEST_LOCATION );
  data.Reset();

  // Emit a down signal where the actor is not present
  screenCoordinates.x = screenCoordinates.y = 200.0f;
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, screenCoordinates ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  END_TEST;
}


int UtcDaliTouchDataNormalProcessing02(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  HandleData handleData;
  TouchDataHandleFunctor functor( handleData );
  actor.TouchSignal().Connect( &application, functor );

  Vector2 screenCoordinates( 10.0f, 10.0f );
  Vector2 localCoordinates;
  actor.ScreenToLocal( localCoordinates.x, localCoordinates.y, screenCoordinates.x, screenCoordinates.y );

  // Emit a down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, screenCoordinates ) );
  DALI_TEST_EQUALS( true, handleData.signalReceived, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, handleData.touchData.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, handleData.touchData.GetState(0), TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, handleData.touchData.GetScreenPosition(0), TEST_LOCATION );
  DALI_TEST_EQUALS( localCoordinates, handleData.touchData.GetLocalPosition(0), 0.1f, TEST_LOCATION );

  END_TEST;
}


int UtcDaliTouchDataAPINegative(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  OutOfBoundsData data;
  OutOfBoundsFunctor functor( data, true );
  actor.TouchSignal().Connect( &application, functor );

  Vector2 screenCoordinates( 10.0f, 10.0f );
  Vector2 localCoordinates;
  actor.ScreenToLocal( localCoordinates.x, localCoordinates.y, screenCoordinates.x, screenCoordinates.y );

  // Emit a down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, screenCoordinates ) );

  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( -1, data.point.deviceId, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::FINISHED, data.point.state, TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2::ZERO, data.point.screen, TEST_LOCATION );
  DALI_TEST_EQUALS( Vector2::ZERO, data.point.local, 0.1f, TEST_LOCATION );
  DALI_TEST_CHECK( ! data.point.hitActor );

  END_TEST;
}


int UtcDaliTouchDataOutsideCameraNearFarPlanes(void)
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();
  Vector2 stageSize = stage.GetSize();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::CENTER);
  actor.SetParentOrigin(ParentOrigin::CENTER);
  stage.Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Get the camera's near and far planes
  RenderTaskList taskList = stage.GetRenderTaskList();
  Dali::RenderTask task = taskList.GetTask(0);
  CameraActor camera = task.GetCameraActor();
  float nearPlane = camera.GetNearClippingPlane();
  float farPlane = camera.GetFarClippingPlane();

  // Calculate the current distance of the actor from the camera
  float tanHalfFov = tanf(camera.GetFieldOfView() * 0.5f);
  float distance = (stageSize.y * 0.5f) / tanHalfFov;

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data );
  actor.TouchSignal().Connect( &application, functor );

  Vector2 screenCoordinates( stageSize.x * 0.5f, stageSize.y * 0.5f );

  // Emit a down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Emit a down signal where actor is just at the camera's near plane
  actor.SetZ(distance - nearPlane);

  // Render and notify
  application.SendNotification();
  application.Render();

  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Emit a down signal where actor is closer than the camera's near plane
  actor.SetZ((distance - nearPlane) + 1.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, screenCoordinates ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Emit a down signal where actor is just at the camera's far plane
  actor.SetZ(distance - farPlane);

  // Render and notify
  application.SendNotification();
  application.Render();

  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Emit a down signal where actor is further than the camera's far plane
  actor.SetZ((distance - farPlane) - 1.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, screenCoordinates ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();
  END_TEST;
}

int UtcDaliTouchDataEmitEmpty(void)
{
  TestApplication application;

  try
  {
    // Emit an empty TouchEvent
    Integration::TouchEvent event;
    application.ProcessEvent( event );
    tet_result( TET_FAIL );
  }
  catch ( Dali::DaliException& e )
  {
    DALI_TEST_ASSERT( e, "!event.points.empty()", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliTouchDataInterrupted(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data );
  actor.TouchSignal().Connect( &application, functor );

  // Emit a down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, data.touchData.points[0].state, TEST_LOCATION );
  data.Reset();

  // Emit an interrupted signal, we should be signalled regardless of whether there is a hit or not.
  application.ProcessEvent( GenerateSingleTouch( PointState::INTERRUPTED, Vector2( 200.0f, 200.0f /* Outside actor */ ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::INTERRUPTED, data.touchData.points[0].state, TEST_LOCATION );
  data.Reset();

  // Emit another interrupted signal, our signal handler should not be called.
  application.ProcessEvent( GenerateSingleTouch( PointState::INTERRUPTED, Vector2( 200.0f, 200.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  END_TEST;
}

int UtcDaliTouchDataParentConsumer(void)
{
  TestApplication application;
  Actor rootActor( Stage::GetCurrent().GetRootLayer() );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data, false );
  actor.TouchSignal().Connect( &application, functor );

  // Connect to root actor's touched signal
  SignalData rootData;
  TouchDataFunctor rootFunctor( rootData ); // Consumes signal
  rootActor.TouchSignal().Connect( &application, rootFunctor );

  Vector2 screenCoordinates( 10.0f, 10.0f );
  Vector2 actorCoordinates, rootCoordinates;
  actor.ScreenToLocal( actorCoordinates.x, actorCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  rootActor.ScreenToLocal( rootCoordinates.x, rootCoordinates.y, screenCoordinates.x, screenCoordinates.y );

  // Emit a down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, data.touchData.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, rootData.touchData.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, data.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, rootData.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, data.touchData.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, rootData.touchData.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( actorCoordinates, data.touchData.points[0].local, 0.1f, TEST_LOCATION );
  DALI_TEST_EQUALS( rootCoordinates, rootData.touchData.points[0].local, 0.1f, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchData.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.touchData.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Emit a motion signal
  screenCoordinates.x = screenCoordinates.y = 11.0f;
  actor.ScreenToLocal( actorCoordinates.x, actorCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  rootActor.ScreenToLocal( rootCoordinates.x, rootCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, data.touchData.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, rootData.touchData.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::MOTION, data.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::MOTION, rootData.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, data.touchData.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, rootData.touchData.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( actorCoordinates, data.touchData.points[0].local, 0.1f, TEST_LOCATION );
  DALI_TEST_EQUALS( rootCoordinates, rootData.touchData.points[0].local, 0.1f, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchData.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.touchData.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Emit an up signal
  screenCoordinates.x = screenCoordinates.y = 12.0f;
  actor.ScreenToLocal( actorCoordinates.x, actorCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  rootActor.ScreenToLocal( rootCoordinates.x, rootCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  application.ProcessEvent( GenerateSingleTouch( PointState::UP, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, data.touchData.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, rootData.touchData.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::UP, data.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::UP, rootData.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, data.touchData.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, rootData.touchData.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( actorCoordinates, data.touchData.points[0].local, 0.1f, TEST_LOCATION );
  DALI_TEST_EQUALS( rootCoordinates, rootData.touchData.points[0].local, 0.1f, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchData.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.touchData.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Emit a down signal where the actor is not present, will hit the root actor though
  screenCoordinates.x = screenCoordinates.y = 200.0f;
  rootActor.ScreenToLocal( rootCoordinates.x, rootCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, screenCoordinates ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, rootData.touchData.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, rootData.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, rootData.touchData.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( rootCoordinates, rootData.touchData.points[0].local, 0.1f, TEST_LOCATION );
  DALI_TEST_CHECK( rootActor == rootData.touchData.points[0].hitActor );
  END_TEST;
}

int UtcDaliTouchDataInterruptedParentConsumer(void)
{
  TestApplication application;
  Actor rootActor( Stage::GetCurrent().GetRootLayer() );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data, false );
  actor.TouchSignal().Connect( &application, functor );

  // Connect to root actor's touched signal
  SignalData rootData;
  TouchDataFunctor rootFunctor( rootData ); // Consumes signal
  rootActor.TouchSignal().Connect( &application, rootFunctor );

  // Emit a down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, data.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, rootData.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchData.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.touchData.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Emit an interrupted signal
  application.ProcessEvent( GenerateSingleTouch( PointState::INTERRUPTED, Vector2( 200.0f, 200.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::INTERRUPTED, data.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::INTERRUPTED, rootData.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchData.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.touchData.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Emit another down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, data.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, rootData.touchData.points[0].state, TEST_LOCATION );
  data.Reset();
  rootData.Reset();

  // Remove actor from Stage
  Stage::GetCurrent().Remove( actor );
  data.Reset();
  rootData.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit an interrupted signal, only root actor's signal should be called.
  application.ProcessEvent( GenerateSingleTouch( PointState::INTERRUPTED, Vector2( 200.0f, 200.0f /* Outside actor */ ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::INTERRUPTED, rootData.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( rootActor == rootData.touchData.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Emit another interrupted state, none of the signal's should be called.
  application.ProcessEvent( GenerateSingleTouch( PointState::INTERRUPTED, Vector2( 200.0f, 200.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, rootData.functorCalled, TEST_LOCATION );
  END_TEST;
}

int UtcDaliTouchDataLeave(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data );
  actor.TouchSignal().Connect( &application, functor );

  // Set actor to require leave events
  actor.SetLeaveRequired( true );

  // Emit a down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, data.touchData.points[0].state, TEST_LOCATION );
  data.Reset();

  // Emit a motion signal outside of actor, should be signalled with a Leave
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2 ( 200.0f, 200.0f )) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::LEAVE, data.touchData.points[0].state, TEST_LOCATION );
  data.Reset();

  // Another motion outside of actor, no signalling
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2 ( 201.0f, 201.0f )) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Another motion event inside actor, signalled with motion
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2 ( 10.0f, 10.0f )) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::MOTION, data.touchData.points[0].state, TEST_LOCATION );
  data.Reset();

  // We do not want to listen to leave events anymore
  actor.SetLeaveRequired( false );

  // Another motion event outside of actor, no signalling
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2 ( 200.0f, 200.0f )) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();
  END_TEST;
}

int UtcDaliTouchDataLeaveParentConsumer(void)
{
  TestApplication application;
  Actor rootActor( Stage::GetCurrent().GetRootLayer() );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data, false );
  actor.TouchSignal().Connect( &application, functor );

  // Connect to root actor's touched signal
  SignalData rootData;
  TouchDataFunctor rootFunctor( rootData ); // Consumes signal
  rootActor.TouchSignal().Connect( &application, rootFunctor );

  // Set actor to require leave events
  actor.SetLeaveRequired( true );
  rootActor.SetLeaveRequired( true );

  // Emit a down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, data.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, rootData.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchData.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.touchData.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Emit a motion signal outside of actor, should be signalled with a Leave
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2 ( 200.0f, 200.0f )) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::LEAVE, data.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::LEAVE, rootData.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchData.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.touchData.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Another motion outside of actor, only rootActor signalled
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2 ( 201.0f, 201.0f )) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::MOTION, rootData.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( rootActor == rootData.touchData.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Another motion event inside actor, signalled with motion
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2 ( 10.0f, 10.0f )) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::MOTION, data.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::MOTION, rootData.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchData.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.touchData.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // We do not want to listen to leave events of actor anymore
  actor.SetLeaveRequired( false );

  // Another motion event outside of root actor, only root signalled
  Vector2 stageSize( Stage::GetCurrent().GetSize() );
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2 ( stageSize.width + 10.0f, stageSize.height + 10.0f )) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::LEAVE, rootData.touchData.points[0].state, TEST_LOCATION );
  END_TEST;
}

int UtcDaliTouchDataActorBecomesInsensitive(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data );
  actor.TouchSignal().Connect( &application, functor );

  // Emit a down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, data.touchData.points[0].state, TEST_LOCATION );
  data.Reset();

  // Change actor to insensitive
  actor.SetSensitive( false );

  // Emit a motion signal, signalled with an interrupted
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2 ( 200.0f, 200.0f )) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::INTERRUPTED, data.touchData.points[0].state, TEST_LOCATION );
  data.Reset();
  END_TEST;
}

int UtcDaliTouchDataActorBecomesInsensitiveParentConsumer(void)
{
  TestApplication application;
  Actor rootActor( Stage::GetCurrent().GetRootLayer() );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data, false );
  actor.TouchSignal().Connect( &application, functor );

  // Connect to root actor's touched signal
  SignalData rootData;
  TouchDataFunctor rootFunctor( rootData ); // Consumes signal
  rootActor.TouchSignal().Connect( &application, rootFunctor );

  // Emit a down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, data.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, rootData.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchData.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.touchData.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Make root actor insensitive
  rootActor.SetSensitive( false );

  // Emit a motion signal, signalled with an interrupted (should get interrupted even if within root actor)
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2 ( 200.0f, 200.0f )) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::INTERRUPTED, data.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::INTERRUPTED, rootData.touchData.points[0].state, TEST_LOCATION );
  END_TEST;
}

int UtcDaliTouchDataMultipleLayers(void)
{
  TestApplication application;
  Actor rootActor( Stage::GetCurrent().GetRootLayer() );

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data );

  Layer layer1 ( Layer::New() );
  layer1.SetSize(100.0f, 100.0f);
  layer1.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add( layer1 );

  Actor actor1 ( Actor::New() );
  actor1.SetSize( 100.0f, 100.0f );
  actor1.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  actor1.SetZ( 1.0f ); // Should hit actor1 in this layer
  layer1.Add( actor1 );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to layer1 and actor1
  layer1.TouchSignal().Connect( &application, functor );
  actor1.TouchSignal().Connect( &application, functor );

  // Hit in hittable area, actor1 should be hit
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( data.touchedActor == actor1 );
  data.Reset();

  // Make layer1 insensitive, nothing should be hit
  layer1.SetSensitive( false );
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Make layer1 sensitive again, again actor1 will be hit
  layer1.SetSensitive( true );
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( data.touchedActor == actor1 );
  data.Reset();

  // Make rootActor insensitive, nothing should be hit
  rootActor.SetSensitive( false );
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Make rootActor sensitive
  rootActor.SetSensitive( true );

  // Add another layer
  Layer layer2 ( Layer::New() );
  layer2.SetSize(100.0f, 100.0f );
  layer2.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  layer2.SetZ( 10.0f ); // Should hit layer2 in this layer rather than actor2
  Stage::GetCurrent().Add( layer2 );

  Actor actor2 ( Actor::New() );
  actor2.SetSize(100.0f, 100.0f);
  actor2.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  layer2.Add( actor2 );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to layer2 and actor2
  layer2.TouchSignal().Connect( &application, functor );
  actor2.TouchSignal().Connect( &application, functor );

  // Emit an event, should hit layer2
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  //DALI_TEST_CHECK( data.touchedActor == layer2 ); // TODO: Uncomment this after removing renderable hack!
  data.Reset();

  // Make layer2 insensitive, should hit actor1
  layer2.SetSensitive( false );
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( data.touchedActor == actor1 );
  data.Reset();

  // Make layer2 sensitive again, should hit layer2
  layer2.SetSensitive( true );
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  //DALI_TEST_CHECK( data.touchedActor == layer2 ); // TODO: Uncomment this after removing renderable hack!
  data.Reset();

  // Make layer2 invisible, render and notify
  layer2.SetVisible( false );
  application.SendNotification();
  application.Render();

  // Should hit actor1
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( data.touchedActor == actor1 );
  data.Reset();

  // Make rootActor invisible, render and notify
  rootActor.SetVisible( false );
  application.SendNotification();
  application.Render();

  // Should not hit anything
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();
  END_TEST;
}

int UtcDaliTouchDataMultipleRenderTasks(void)
{
  TestApplication application;
  Stage stage ( Stage::GetCurrent() );
  Vector2 stageSize ( stage.GetSize() );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  stage.Add(actor);

  // Create render task
  Viewport viewport( stageSize.width * 0.5f, stageSize.height * 0.5f, stageSize.width * 0.5f, stageSize.height * 0.5f );
  RenderTask renderTask ( Stage::GetCurrent().GetRenderTaskList().CreateTask() );
  renderTask.SetViewport( viewport );
  renderTask.SetInputEnabled( true );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data );
  actor.TouchSignal().Connect( &application, functor );

  // Emit a down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Ensure renderTask actor can be hit too.
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( viewport.x + 5.0f, viewport.y + 5.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Disable input on renderTask, should not be hittable
  renderTask.SetInputEnabled( false );
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( viewport.x + 5.0f, viewport.y + 5.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();
  END_TEST;
}

int UtcDaliTouchDataMultipleRenderTasksWithChildLayer(void)
{
  TestApplication application;
  Stage stage ( Stage::GetCurrent() );
  Vector2 stageSize ( stage.GetSize() );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  stage.Add(actor);

  Layer layer = Layer::New();
  layer.SetSize(100.0f, 100.0f);
  layer.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  actor.Add(layer);

  // Create render task
  Viewport viewport( stageSize.width * 0.5f, stageSize.height * 0.5f, stageSize.width * 0.5f, stageSize.height * 0.5f );
  RenderTask renderTask ( Stage::GetCurrent().GetRenderTaskList().CreateTask() );
  renderTask.SetViewport( viewport );
  renderTask.SetInputEnabled( true );
  renderTask.SetSourceActor( actor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to layer's touched signal
  SignalData data;
  TouchDataFunctor functor( data );
  actor.TouchSignal().Connect( &application, functor );
  layer.TouchSignal().Connect( &application, functor );

  // Emit a down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Ensure renderTask actor can be hit too.
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( viewport.x + 5.0f, viewport.y + 5.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Disable input on renderTask, should not be hittable
  renderTask.SetInputEnabled( false );
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( viewport.x + 5.0f, viewport.y + 5.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();
  END_TEST;
}

int UtcDaliTouchDataOffscreenRenderTasks(void)
{
  TestApplication application;
  Stage stage ( Stage::GetCurrent() );
  Vector2 stageSize ( stage.GetSize() );

  // FrameBufferImage for offscreen RenderTask
  FrameBufferImage frameBufferImage( FrameBufferImage::New( stageSize.width, stageSize.height, Pixel::RGBA8888 ) );

  // Create a renderable actor to display the FrameBufferImage
  Actor renderableActor = CreateRenderableActor( frameBufferImage );
  renderableActor.SetParentOrigin(ParentOrigin::CENTER);
  renderableActor.SetSize( stageSize.x, stageSize.y );
  renderableActor.ScaleBy( Vector3(1.0f, -1.0f, 1.0f) ); // FIXME
  stage.Add( renderableActor );

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  stage.Add( actor );
  application.GetGlAbstraction().SetCheckFramebufferStatusResult( GL_FRAMEBUFFER_COMPLETE ); // Ensure framebuffer connects

  stage.GetRenderTaskList().GetTask( 0u ).SetScreenToFrameBufferFunction( RenderTask::FULLSCREEN_FRAMEBUFFER_FUNCTION );

  // Create a RenderTask
  RenderTask renderTask = stage.GetRenderTaskList().CreateTask();
  renderTask.SetSourceActor( actor );
  renderTask.SetTargetFrameBuffer( frameBufferImage );
  renderTask.SetInputEnabled( true );

  // Create another RenderTask
  RenderTask renderTask2( stage.GetRenderTaskList().CreateTask() );
  renderTask2.SetInputEnabled( true );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data );
  actor.TouchSignal().Connect( &application, functor );

  // Emit a down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();
  END_TEST;
}

int UtcDaliTouchDataMultipleRenderableActors(void)
{
  TestApplication application;
  Stage stage ( Stage::GetCurrent() );
  Vector2 stageSize ( stage.GetSize() );

  Actor parent = CreateRenderableActor();
  parent.SetSize(100.0f, 100.0f);
  parent.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  stage.Add(parent);

  Actor actor = CreateRenderableActor();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  parent.Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to layer's touched signal
  SignalData data;
  TouchDataFunctor functor( data );
  parent.TouchSignal().Connect( &application, functor );
  actor.TouchSignal().Connect( &application, functor );

  // Emit a down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchedActor );
  END_TEST;
}

int UtcDaliTouchDataActorRemovedInSignal(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  RemoveActorFunctor functor( data );
  actor.TouchSignal().Connect( &application, functor );

  // Register for leave events
  actor.SetLeaveRequired( true );

  // Emit a down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Re-add, render and notify
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render();

  // Emit another signal outside of actor's area, should not get anything as the scene has changed.
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 210.0f, 210.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Emit a down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit another signal outside of actor's area, should not get anything as the scene has changed.
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 210.0f, 210.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Re-add actor back to stage, render and notify
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render();

  // Emit another down event
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Completely delete the actor
  actor.Reset();

  // Emit event, should not crash and should not receive an event.
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 210.0f, 210.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  END_TEST;
}

int UtcDaliTouchDataActorSignalNotConsumed(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data, false );
  actor.TouchSignal().Connect( &application, functor );

  // Emit a down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  END_TEST;
}

int UtcDaliTouchDataActorUnStaged(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data );
  actor.TouchSignal().Connect( &application, functor );

  // Emit a down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Remove actor from stage
  Stage::GetCurrent().Remove( actor );
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit a move at the same point, we should not be signalled.
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();
  END_TEST;
}

int UtcDaliTouchDataSystemOverlayActor(void)
{
  TestApplication application;
  Dali::Integration::Core& core( application.GetCore() );
  Dali::Integration::SystemOverlay& systemOverlay( core.GetSystemOverlay() );
  systemOverlay.GetOverlayRenderTasks().CreateTask();

  // Create an actor and add it to the system overlay.
  Actor systemActor = Actor::New();
  systemActor.SetSize(100.0f, 100.0f);
  systemActor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  systemOverlay.Add( systemActor );

  // Create an actor and add it to the stage as per normal, same position and size as systemActor
  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Connect to the touch signals.
  SignalData data;
  TouchDataFunctor functor( data );
  systemActor.TouchSignal().Connect( &application, functor );
  actor.TouchSignal().Connect( &application, functor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit a down signal, the system overlay is drawn last so is at the top, should hit the systemActor.
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( systemActor == data.touchedActor );
  END_TEST;
}

int UtcDaliTouchDataLayerConsumesTouch(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data );
  actor.TouchSignal().Connect( &application, functor );

  // Add a layer to overlap the actor
  Layer layer = Layer::New();
  layer.SetSize(100.0f, 100.0f);
  layer.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add( layer );
  layer.RaiseToTop();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit a few touch signals
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::UP, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Set layer to consume all touch
  layer.SetTouchConsumed( true );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit the same signals again, should not receive
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::UP, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  data.Reset();

  END_TEST;
}

int UtcDaliTouchDataLeaveActorReadded(void)
{
  TestApplication application;
  Stage stage = Stage::GetCurrent();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  stage.Add(actor);

  // Set actor to receive touch-events
  actor.SetLeaveRequired( true );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data );
  actor.TouchSignal().Connect( &application, functor );

  // Emit a down and motion
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 11.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Remove actor from stage and add again
  stage.Remove( actor );
  stage.Add( actor );

  // Emit a motion within the actor's bounds
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 12.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Emit a motion outside the actor's bounds
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 200.0f, 200.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::LEAVE, data.touchData.points[0].state, TEST_LOCATION );
  data.Reset();

  END_TEST;
}

int UtcDaliTouchDataClippedActor(void)
{
  TestApplication application;
  Stage stage = Stage::GetCurrent();

  Actor actor = Actor::New();
  actor.SetSize( 100.0f, 100.0f );
  actor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  stage.Add( actor );

  Actor clippingActor = Actor::New();
  clippingActor.SetSize( 50.0f, 50.0f );
  clippingActor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  clippingActor.SetProperty( Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN );
  stage.Add( clippingActor );

  // Render and notify.
  application.SendNotification();
  application.Render();

  // Connect to actor's touch signal.
  SignalData data;
  TouchDataFunctor functor( data );
  actor.TouchSignal().Connect( &application, functor );

  // Emit an event within clipped area.
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Emit an event outside the clipped area but within the actor area, we should have a hit.
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 60.0f, 60.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  END_TEST;
}

int UtcDaliTouchDataActorUnstaged(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data );
  actor.TouchSignal().Connect( &application, functor );

  // Emit a down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, data.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchData.points[0].hitActor );
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Unparent the actor
  actor.Unparent();

  // Should receive an interrupted event
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::INTERRUPTED, data.touchData.points[0].state, TEST_LOCATION );
  END_TEST;
}

int UtcDaliTouchDataParentUnstaged(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  parent.SetSize(100.0f, 100.0f);
  parent.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(parent);

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  parent.Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data );
  actor.TouchSignal().Connect( &application, functor );

  // Emit a down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, data.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchData.points[0].hitActor );
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Unparent the parent of the touchable actor
  parent.Unparent();

  // Should receive an interrupted event
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::INTERRUPTED, data.touchData.points[0].state, TEST_LOCATION );
  END_TEST;
}

int UtcDaliTouchDataActorUnstagedDifferentConsumer(void)
{
  TestApplication application;

  Actor parent = Actor::New();
  parent.SetSize(100.0f, 100.0f);
  parent.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(parent);

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  parent.Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data, false /* Do not consume */ );
  actor.TouchSignal().Connect( &application, functor );

  // Connect to parent's touched signal
  SignalData parentData;
  TouchDataFunctor parentFunctor( parentData );
  parent.TouchSignal().Connect( &application, parentFunctor );

  // Emit a down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, data.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchData.points[0].hitActor );
  DALI_TEST_CHECK( actor == data.touchedActor );
  DALI_TEST_EQUALS( true, parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, parentData.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == parentData.touchData.points[0].hitActor );
  DALI_TEST_CHECK( parent == parentData.touchedActor );
  data.Reset();
  parentData.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Unparent the actor
  actor.Unparent();

  // Should receive an interrupted event for both actor & parent
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::INTERRUPTED, data.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( true, parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::INTERRUPTED, parentData.touchData.points[0].state, TEST_LOCATION );
  data.Reset();
  parentData.Reset();

  // Readd actor to parent
  parent.Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit a motion signal
  application.ProcessEvent( GenerateSingleTouch( PointState::MOTION, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, parentData.functorCalled, TEST_LOCATION );
  data.Reset();
  parentData.Reset();

  // Parent is now consumer, connect again to the touched signal of the actor so that it becomes the consumer
  SignalData secondData;
  TouchDataFunctor secondFunctor( secondData /* Consume */ );
  actor.TouchSignal().Connect( &application, secondFunctor );

  // Unparent the actor
  actor.Unparent();

  // Should receive an interrupted event for both actor functors & the parent as well as it was last consumer
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::INTERRUPTED, data.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( true, parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::INTERRUPTED, parentData.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( true, secondData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::INTERRUPTED, secondData.touchData.points[0].state, TEST_LOCATION );
  data.Reset();
  parentData.Reset();
  secondData.Reset();

  END_TEST;
}

int UtcDaliTouchDataInterruptedDifferentConsumer(void)
{
  TestApplication application;
  Actor rootActor( Stage::GetCurrent().GetRootLayer() );

  Actor parent = Actor::New();
  parent.SetSize(100.0f, 100.0f);
  parent.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(parent);

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  parent.Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data, false /* Do not consume */ );
  actor.TouchSignal().Connect( &application, functor );

  // Connect to parent's touched signal
  SignalData parentData;
  TouchDataFunctor parentFunctor( parentData, false /* Do not consume */ );
  parent.TouchSignal().Connect( &application, parentFunctor );

  // Connect to root's touched signal and consume
  SignalData rootData;
  TouchDataFunctor rootFunctor( rootData );
  rootActor.TouchSignal().Connect( &application, rootFunctor );

  // Emit a down signal
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, data.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.touchData.points[0].hitActor );
  DALI_TEST_CHECK( actor == data.touchedActor );
  DALI_TEST_EQUALS( true, parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, parentData.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == parentData.touchData.points[0].hitActor );
  DALI_TEST_CHECK( parent == parentData.touchedActor );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, rootData.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == rootData.touchData.points[0].hitActor );
  DALI_TEST_CHECK( rootActor == rootData.touchedActor );
  data.Reset();
  parentData.Reset();
  rootData.Reset();

  // Root is now consumer, connect to the touched signal of the parent so that it becomes the consumer
  SignalData secondData;
  TouchDataFunctor secondFunctor( secondData /* Consume */ );
  parent.TouchSignal().Connect( &application, secondFunctor );

  // Emit an interrupted signal, all three should STILL be called
  application.ProcessEvent( GenerateSingleTouch( PointState::INTERRUPTED, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::INTERRUPTED, data.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( true, parentData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::INTERRUPTED, parentData.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::INTERRUPTED, rootData.touchData.points[0].state, TEST_LOCATION );
  data.Reset();
  parentData.Reset();
  rootData.Reset();

  END_TEST;
}

int UtcDaliTouchDataGetRadius(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data );
  actor.TouchSignal().Connect( &application, functor );

  // Emit a down signal with an angle
  Integration::TouchEvent touchEvent = GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) );
  touchEvent.points[ 0 ].SetRadius( 100.0f );
  application.ProcessEvent( touchEvent );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, data.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( 100.0f, data.touchData.points[0].radius, TEST_LOCATION );
  DALI_TEST_EQUALS( 100.0f, data.touchData.points[0].ellipseRadius.x, TEST_LOCATION );
  DALI_TEST_EQUALS( 100.0f, data.touchData.points[0].ellipseRadius.y, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTouchDataGetEllipseRadius(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data );
  actor.TouchSignal().Connect( &application, functor );

  // Emit a down signal with an angle
  Integration::TouchEvent touchEvent = GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) );
  touchEvent.points[ 0 ].SetRadius( 100.0f, Vector2( 20.0f, 10.0f ) );
  application.ProcessEvent( touchEvent );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, data.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( 100.0f, data.touchData.points[0].radius, TEST_LOCATION );
  DALI_TEST_EQUALS( 20.0f, data.touchData.points[0].ellipseRadius.x, TEST_LOCATION );
  DALI_TEST_EQUALS( 10.0f, data.touchData.points[0].ellipseRadius.y, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTouchDataGetAngle(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data );
  actor.TouchSignal().Connect( &application, functor );

  // Emit a down signal with an angle
  Integration::TouchEvent touchEvent = GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) );
  touchEvent.points[ 0 ].SetAngle( Degree( 90.0f ) );
  application.ProcessEvent( touchEvent );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, data.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( Degree( 90.0f ), data.touchData.points[0].angle, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTouchDataGetPressure(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data );
  actor.TouchSignal().Connect( &application, functor );

  // Emit a down signal with an angle
  Integration::TouchEvent touchEvent = GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) );
  touchEvent.points[ 0 ].SetPressure( 10.0f );
  application.ProcessEvent( touchEvent );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( PointState::DOWN, data.touchData.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( 10.0f, data.touchData.points[0].pressure, TEST_LOCATION );

  END_TEST;
}

int UtcDaliTouchDataAndEventUsage(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's touched signal
  SignalData data;
  TouchDataFunctor functor( data );
  actor.TouchSignal().Connect( &application, functor );

  // Connect to actor's touched signal (OLD)
  bool touchEventFunctorCalled = false;
  TouchEventFunctor eventFunctor( touchEventFunctorCalled );
  actor.TouchedSignal().Connect( &application, eventFunctor );

  // Emit a down signal with an angle
  application.ProcessEvent( GenerateSingleTouch( PointState::DOWN, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, touchEventFunctorCalled, TEST_LOCATION );

  END_TEST;


}
