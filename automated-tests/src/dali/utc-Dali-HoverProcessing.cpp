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
#include <dali/integration-api/events/hover-event-integ.h>
#include <dali/integration-api/system-overlay.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

void utc_dali_hover_processing_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_hover_processing_cleanup(void)
{
  test_return_value = TET_PASS;
}

///////////////////////////////////////////////////////////////////////////////

namespace
{

// Stores data that is populated in the callback and will be read by the TET cases
struct SignalData
{
  SignalData()
  : functorCalled( false ),
    hoverEvent(),
    hoveredActor()
  {
  }

  void Reset()
  {
    functorCalled = false;

    hoverEvent.time = 0u;
    hoverEvent.points.clear();

    hoveredActor.Reset();
  }

  bool functorCalled;
  HoverEvent hoverEvent;
  Actor hoveredActor;
};

// Functor that sets the data when called
struct HoverEventFunctor
{
  /**
   * Constructor.
   * @param[in]  data         Reference to the data to store callback information.
   * @param[in]  returnValue  What the functor should return.
   */
  HoverEventFunctor( SignalData& data, bool returnValue = true )
  : signalData( data ),
    returnValue( returnValue )
  {
  }

  bool operator()( Actor actor, const HoverEvent& hoverEvent )
  {
    signalData.functorCalled = true;
    signalData.hoveredActor = actor;
    signalData.hoverEvent = hoverEvent;

    return returnValue;
  }

  SignalData& signalData;
  bool returnValue;
};

// Functor that removes the actor when called.
struct RemoveActorFunctor : public HoverEventFunctor
{
  /**
   * Constructor.
   * @param[in]  data         Reference to the data to store callback information.
   * @param[in]  returnValue  What the functor should return.
   */
  RemoveActorFunctor( SignalData& data, bool returnValue = true )
  : HoverEventFunctor( data, returnValue )
  {
  }

  bool operator()( Actor actor, const HoverEvent& hoverEvent )
  {
    Actor parent( actor.GetParent() );
    if ( parent )
    {
      parent.Remove( actor );
    }

    return HoverEventFunctor::operator()( actor, hoverEvent );
  }
};

Integration::HoverEvent GenerateSingleHover( TouchPoint::State state, Vector2 screenPosition )
{
  Integration::HoverEvent hoverEvent;
  hoverEvent.points.push_back( TouchPoint ( 0, state, screenPosition.x, screenPosition.y ) );
  return hoverEvent;
}

} // anon namespace

///////////////////////////////////////////////////////////////////////////////

int UtcDaliHoverNormalProcessing(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal
  SignalData data;
  HoverEventFunctor functor( data );
  actor.HoveredSignal().Connect( &application, functor );

  Vector2 screenCoordinates( 10.0f, 10.0f );
  Vector2 localCoordinates;
  actor.ScreenToLocal( localCoordinates.x, localCoordinates.y, screenCoordinates.x, screenCoordinates.y );

  // Emit a started signal
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, data.hoverEvent.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Started, data.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, data.hoverEvent.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( localCoordinates, data.hoverEvent.points[0].local, 0.1f, TEST_LOCATION );

  TouchPoint point = data.hoverEvent.GetPoint(0);
  DALI_TEST_EQUALS( TouchPoint::Started, point.state, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, point.screen, TEST_LOCATION );
  DALI_TEST_EQUALS( localCoordinates, point.local, 0.1f, TEST_LOCATION );

  data.Reset();

  // Emit a motion signal
  screenCoordinates.x = screenCoordinates.y = 11.0f;
  actor.ScreenToLocal( localCoordinates.x, localCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, data.hoverEvent.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Motion, data.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, data.hoverEvent.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( localCoordinates, data.hoverEvent.points[0].local, 0.1f, TEST_LOCATION );
  data.Reset();

  // Emit a finished signal
  screenCoordinates.x = screenCoordinates.y = 12.0f;
  actor.ScreenToLocal( localCoordinates.x, localCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Finished, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, data.hoverEvent.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Finished, data.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, data.hoverEvent.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( localCoordinates, data.hoverEvent.points[0].local, 0.1f, TEST_LOCATION );
  data.Reset();

  // Emit a started signal where the actor is not present
  screenCoordinates.x = screenCoordinates.y = 200.0f;
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, screenCoordinates ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  END_TEST;
}

int UtcDaliHoverOutsideCameraNearFarPlanes(void)
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

  // Connect to actor's hovered signal
  SignalData data;
  HoverEventFunctor functor( data );
  actor.HoveredSignal().Connect( &application, functor );

  Vector2 screenCoordinates( stageSize.x * 0.5f, stageSize.y * 0.5f );

  // Emit a started signal
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Emit a started signal where actor is just at the camera's near plane
  actor.SetZ(distance - nearPlane);

  // Render and notify
  application.SendNotification();
  application.Render();

  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Emit a started signal where actor is closer than the camera's near plane
  actor.SetZ((distance - nearPlane) + 1.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, screenCoordinates ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Emit a started signal where actor is just at the camera's far plane
  actor.SetZ(distance - farPlane);

  // Render and notify
  application.SendNotification();
  application.Render();

  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Emit a started signal where actor is further than the camera's far plane
  actor.SetZ((distance - farPlane) - 1.0f);

  // Render and notify
  application.SendNotification();
  application.Render();

  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, screenCoordinates ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();
  END_TEST;
}

int UtcDaliHoverEmitEmpty(void)
{
  TestApplication application;

  try
  {
    // Emit an empty HoverEvent
    Integration::HoverEvent event;
    application.ProcessEvent( event );
    tet_result( TET_FAIL );
  }
  catch ( Dali::DaliException& e )
  {
    DALI_TEST_ASSERT( e, "!event.points.empty()", TEST_LOCATION );
  }
  END_TEST;
}

int UtcDaliHoverInterrupted(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal
  SignalData data;
  HoverEventFunctor functor( data );
  actor.HoveredSignal().Connect( &application, functor );

  // Emit a started signal
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Started, data.hoverEvent.points[0].state, TEST_LOCATION );
  data.Reset();

  // Emit an interrupted signal, we should be signalled regardless of whether there is a hit or not.
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Interrupted, Vector2( 200.0f, 200.0f /* Outside actor */ ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Interrupted, data.hoverEvent.points[0].state, TEST_LOCATION );
  data.Reset();

  // Emit another interrupted signal, our signal handler should not be called.
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Interrupted, Vector2( 200.0f, 200.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  END_TEST;
}

int UtcDaliHoverParentConsumer(void)
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

  // Connect to actor's hovered signal
  SignalData data;
  HoverEventFunctor functor( data, false );
  actor.HoveredSignal().Connect( &application, functor );

  // Connect to root actor's hovered signal
  SignalData rootData;
  HoverEventFunctor rootFunctor( rootData ); // Consumes signal
  rootActor.HoveredSignal().Connect( &application, rootFunctor );

  Vector2 screenCoordinates( 10.0f, 10.0f );
  Vector2 actorCoordinates, rootCoordinates;
  actor.ScreenToLocal( actorCoordinates.x, actorCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  rootActor.ScreenToLocal( rootCoordinates.x, rootCoordinates.y, screenCoordinates.x, screenCoordinates.y );

  // Emit a started signal
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, data.hoverEvent.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, rootData.hoverEvent.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Started, data.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Started, rootData.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, data.hoverEvent.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, rootData.hoverEvent.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( actorCoordinates, data.hoverEvent.points[0].local, 0.1f, TEST_LOCATION );
  DALI_TEST_EQUALS( rootCoordinates, rootData.hoverEvent.points[0].local, 0.1f, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.hoverEvent.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.hoverEvent.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Emit a motion signal
  screenCoordinates.x = screenCoordinates.y = 11.0f;
  actor.ScreenToLocal( actorCoordinates.x, actorCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  rootActor.ScreenToLocal( rootCoordinates.x, rootCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, data.hoverEvent.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, rootData.hoverEvent.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Motion, data.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Motion, rootData.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, data.hoverEvent.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, rootData.hoverEvent.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( actorCoordinates, data.hoverEvent.points[0].local, 0.1f, TEST_LOCATION );
  DALI_TEST_EQUALS( rootCoordinates, rootData.hoverEvent.points[0].local, 0.1f, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.hoverEvent.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.hoverEvent.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Emit a finished signal
  screenCoordinates.x = screenCoordinates.y = 12.0f;
  actor.ScreenToLocal( actorCoordinates.x, actorCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  rootActor.ScreenToLocal( rootCoordinates.x, rootCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Finished, screenCoordinates ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, data.hoverEvent.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, rootData.hoverEvent.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Finished, data.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Finished, rootData.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, data.hoverEvent.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, rootData.hoverEvent.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( actorCoordinates, data.hoverEvent.points[0].local, 0.1f, TEST_LOCATION );
  DALI_TEST_EQUALS( rootCoordinates, rootData.hoverEvent.points[0].local, 0.1f, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.hoverEvent.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.hoverEvent.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Emit a started signal where the actor is not present, will hit the root actor though
  screenCoordinates.x = screenCoordinates.y = 200.0f;
  rootActor.ScreenToLocal( rootCoordinates.x, rootCoordinates.y, screenCoordinates.x, screenCoordinates.y );
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, screenCoordinates ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( 1u, rootData.hoverEvent.GetPointCount(), TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Started, rootData.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( screenCoordinates, rootData.hoverEvent.points[0].screen, TEST_LOCATION );
  DALI_TEST_EQUALS( rootCoordinates, rootData.hoverEvent.points[0].local, 0.1f, TEST_LOCATION );
  DALI_TEST_CHECK( rootActor == rootData.hoverEvent.points[0].hitActor );
  END_TEST;
}

int UtcDaliHoverInterruptedParentConsumer(void)
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

  // Connect to actor's hovered signal
  SignalData data;
  HoverEventFunctor functor( data, false );
  actor.HoveredSignal().Connect( &application, functor );

  // Connect to root actor's hovered signal
  SignalData rootData;
  HoverEventFunctor rootFunctor( rootData ); // Consumes signal
  rootActor.HoveredSignal().Connect( &application, rootFunctor );

  // Emit a started signal
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Started, data.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Started, rootData.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.hoverEvent.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.hoverEvent.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Emit an interrupted signal
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Interrupted, Vector2( 200.0f, 200.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Interrupted, data.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Interrupted, rootData.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.hoverEvent.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.hoverEvent.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Emit another started signal
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Started, data.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Started, rootData.hoverEvent.points[0].state, TEST_LOCATION );
  data.Reset();
  rootData.Reset();

  // Remove actor from Stage
  Stage::GetCurrent().Remove( actor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit an interrupted signal, only root actor's signal should be called.
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Interrupted, Vector2( 200.0f, 200.0f /* Outside actor */ ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Interrupted, rootData.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( rootActor == rootData.hoverEvent.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Emit another interrupted state, none of the signal's should be called.
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Interrupted, Vector2( 200.0f, 200.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, rootData.functorCalled, TEST_LOCATION );
  END_TEST;
}

int UtcDaliHoverLeave(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal
  SignalData data;
  HoverEventFunctor functor( data );
  actor.HoveredSignal().Connect( &application, functor );

  // Set actor to require leave events
  actor.SetLeaveRequired( true );

  // Emit a started signal
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Started, data.hoverEvent.points[0].state, TEST_LOCATION );
  data.Reset();

  // Emit a motion signal outside of actor, should be signalled with a Leave
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, Vector2 ( 200.0f, 200.0f )) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Leave, data.hoverEvent.points[0].state, TEST_LOCATION );
  data.Reset();

  // Another motion outside of actor, no signalling
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, Vector2 ( 201.0f, 201.0f )) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Another motion event inside actor, signalled with motion
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, Vector2 ( 10.0f, 10.0f )) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Motion, data.hoverEvent.points[0].state, TEST_LOCATION );
  data.Reset();

  // We do not want to listen to leave events anymore
  actor.SetLeaveRequired( false );

  // Another motion event outside of actor, no signalling
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, Vector2 ( 200.0f, 200.0f )) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();
  END_TEST;
}

int UtcDaliHoverLeaveParentConsumer(void)
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

  // Connect to actor's hovered signal
  SignalData data;
  HoverEventFunctor functor( data, false );
  actor.HoveredSignal().Connect( &application, functor );

  // Connect to root actor's hovered signal
  SignalData rootData;
  HoverEventFunctor rootFunctor( rootData ); // Consumes signal
  rootActor.HoveredSignal().Connect( &application, rootFunctor );

  // Set actor to require leave events
  actor.SetLeaveRequired( true );
  rootActor.SetLeaveRequired( true );

  // Emit a started signal
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Started, data.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Started, rootData.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.hoverEvent.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.hoverEvent.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Emit a motion signal outside of actor, should be signalled with a Leave
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, Vector2 ( 200.0f, 200.0f )) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Leave, data.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Leave, rootData.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.hoverEvent.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.hoverEvent.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Another motion outside of actor, only rootActor signalled
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, Vector2 ( 201.0f, 201.0f )) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Motion, rootData.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( rootActor == rootData.hoverEvent.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Another motion event inside actor, signalled with motion
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, Vector2 ( 10.0f, 10.0f )) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Motion, data.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Motion, rootData.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.hoverEvent.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.hoverEvent.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // We do not want to listen to leave events of actor anymore
  actor.SetLeaveRequired( false );

  // Another motion event outside of root actor, only root signalled
  Vector2 stageSize( Stage::GetCurrent().GetSize() );
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, Vector2 ( stageSize.width + 10.0f, stageSize.height + 10.0f )) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Leave, rootData.hoverEvent.points[0].state, TEST_LOCATION );
  END_TEST;
}

int UtcDaliHoverActorBecomesInsensitive(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal
  SignalData data;
  HoverEventFunctor functor( data );
  actor.HoveredSignal().Connect( &application, functor );

  // Emit a started signal
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Started, data.hoverEvent.points[0].state, TEST_LOCATION );
  data.Reset();

  // Change actor to insensitive
  actor.SetSensitive( false );

  // Emit a motion signal, signalled with an interrupted
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, Vector2 ( 200.0f, 200.0f )) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Interrupted, data.hoverEvent.points[0].state, TEST_LOCATION );
  data.Reset();
  END_TEST;
}

int UtcDaliHoverActorBecomesInsensitiveParentConsumer(void)
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

  // Connect to actor's hovered signal
  SignalData data;
  HoverEventFunctor functor( data, false );
  actor.HoveredSignal().Connect( &application, functor );

  // Connect to root actor's hovered signal
  SignalData rootData;
  HoverEventFunctor rootFunctor( rootData ); // Consumes signal
  rootActor.HoveredSignal().Connect( &application, rootFunctor );

  // Emit a started signal
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Started, data.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Started, rootData.hoverEvent.points[0].state, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.hoverEvent.points[0].hitActor );
  DALI_TEST_CHECK( actor == rootData.hoverEvent.points[0].hitActor );
  data.Reset();
  rootData.Reset();

  // Remove actor from Stage
  Stage::GetCurrent().Remove( actor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Make root actor insensitive
  rootActor.SetSensitive( false );

  // Emit a motion signal, signalled with an interrupted (should get interrupted even if within root actor)
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, Vector2 ( 200.0f, 200.0f )) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( true, rootData.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Interrupted, rootData.hoverEvent.points[0].state, TEST_LOCATION );
  END_TEST;
}

int UtcDaliHoverMultipleLayers(void)
{
  TestApplication application;
  Actor rootActor( Stage::GetCurrent().GetRootLayer() );

  // Connect to actor's hovered signal
  SignalData data;
  HoverEventFunctor functor( data );

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
  layer1.HoveredSignal().Connect( &application, functor );
  actor1.HoveredSignal().Connect( &application, functor );

  // Hit in hittable area, actor1 should be hit
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( data.hoveredActor == actor1 );
  data.Reset();

  // Make layer1 insensitive, nothing should be hit
  layer1.SetSensitive( false );
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Make layer1 sensitive again, again actor1 will be hit
  layer1.SetSensitive( true );
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( data.hoveredActor == actor1 );
  data.Reset();

  // Make rootActor insensitive, nothing should be hit
  rootActor.SetSensitive( false );
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
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
  layer2.HoveredSignal().Connect( &application, functor );
  actor2.HoveredSignal().Connect( &application, functor );

  // Emit an event, should hit layer2
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  //DALI_TEST_CHECK( data.hoveredActor == layer2 ); // TODO: Uncomment this after removing renderable hack!
  data.Reset();

  // Make layer2 insensitive, should hit actor1
  layer2.SetSensitive( false );
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( data.hoveredActor == actor1 );
  data.Reset();

  // Make layer2 sensitive again, should hit layer2
  layer2.SetSensitive( true );
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  //DALI_TEST_CHECK( data.hoveredActor == layer2 ); // TODO: Uncomment this after removing renderable hack!
  data.Reset();

  // Make layer2 invisible, render and notify
  layer2.SetVisible( false );
  application.SendNotification();
  application.Render();

  // Should hit actor1
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( data.hoveredActor == actor1 );
  data.Reset();

  // Make rootActor invisible, render and notify
  rootActor.SetVisible( false );
  application.SendNotification();
  application.Render();

  // Should not hit anything
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();
  END_TEST;
}

int UtcDaliHoverMultipleRenderTasks(void)
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

  // Connect to actor's hovered signal
  SignalData data;
  HoverEventFunctor functor( data );
  actor.HoveredSignal().Connect( &application, functor );

  // Emit a started signal
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Ensure renderTask actor can be hit too.
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( viewport.x + 5.0f, viewport.y + 5.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Disable input on renderTask, should not be hittable
  renderTask.SetInputEnabled( false );
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( viewport.x + 5.0f, viewport.y + 5.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();
  END_TEST;
}

int UtcDaliHoverMultipleRenderTasksWithChildLayer(void)
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

  // Connect to layer's hovered signal
  SignalData data;
  HoverEventFunctor functor( data );
  actor.HoveredSignal().Connect( &application, functor );
  layer.HoveredSignal().Connect( &application, functor );

  // Emit a started signal
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Ensure renderTask actor can be hit too.
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( viewport.x + 5.0f, viewport.y + 5.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Disable input on renderTask, should not be hittable
  renderTask.SetInputEnabled( false );
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( viewport.x + 5.0f, viewport.y + 5.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();
  END_TEST;
}

int UtcDaliHoverOffscreenRenderTasks(void)
{
  TestApplication application;
  Stage stage ( Stage::GetCurrent() );
  Vector2 stageSize ( stage.GetSize() );

  // FrameBufferImage for offscreen RenderTask
  FrameBufferImage frameBufferImage( FrameBufferImage::New( stageSize.width, stageSize.height, Pixel::RGBA8888 ) );

  // Create an image actor to display the FrameBufferImage
  ImageActor imageActor ( ImageActor::New( frameBufferImage ) );
  imageActor.SetParentOrigin(ParentOrigin::CENTER);
  imageActor.SetSize( stageSize.x, stageSize.y );
  imageActor.ScaleBy( Vector3(1.0f, -1.0f, 1.0f) ); // FIXME
  stage.Add( imageActor );

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

  // Connect to actor's hovered signal
  SignalData data;
  HoverEventFunctor functor( data );
  actor.HoveredSignal().Connect( &application, functor );

  // Emit a started signal
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();
  END_TEST;
}

int UtcDaliHoverMultipleRenderableActors(void)
{
  TestApplication application;
  Stage stage ( Stage::GetCurrent() );
  Vector2 stageSize ( stage.GetSize() );

  ImageActor parent = ImageActor::New();
  parent.SetSize( 100.0f, 100.0f );
  parent.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  stage.Add(parent);

  ImageActor actor = ImageActor::New();
  actor.SetSize( 100.0f, 100.0f );
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  actor.SetDepthIndex( 1 );
  parent.Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to layer's hovered signal
  SignalData data;
  HoverEventFunctor functor( data );
  parent.HoveredSignal().Connect( &application, functor );
  actor.HoveredSignal().Connect( &application, functor );

  // Emit a started signal
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( actor == data.hoveredActor );
  END_TEST;
}

int UtcDaliHoverActorRemovedInSignal(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal
  SignalData data;
  RemoveActorFunctor functor( data );
  actor.HoveredSignal().Connect( &application, functor );

  // Register for leave events
  actor.SetLeaveRequired( true );

  // Emit a started signal
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Re-add, render and notify
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render();

  // Emit another signal outside of actor's area, should not get anything as the scene has changed.
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, Vector2( 210.0f, 210.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Emit a started signal
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit another signal outside of actor's area, should not get anything as the scene has changed.
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, Vector2( 210.0f, 210.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Re-add actor back to stage, render and notify
  Stage::GetCurrent().Add(actor);
  application.SendNotification();
  application.Render();

  // Emit another started event
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Completely delete the actor
  actor.Reset();

  // Emit event, should not crash and should not receive an event.
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, Vector2( 210.0f, 210.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  END_TEST;
}

int UtcDaliHoverActorSignalNotConsumed(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal
  SignalData data;
  HoverEventFunctor functor( data, false );
  actor.HoveredSignal().Connect( &application, functor );

  // Emit a started signal
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  END_TEST;
}

int UtcDaliHoverActorUnStaged(void)
{
  TestApplication application;

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  Stage::GetCurrent().Add(actor);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal
  SignalData data;
  HoverEventFunctor functor( data );
  actor.HoveredSignal().Connect( &application, functor );

  // Emit a started signal
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Remove actor from stage
  Stage::GetCurrent().Remove( actor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit a move at the same point, we should not be signalled.
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  data.Reset();
  END_TEST;
}

int UtcDaliHoverSystemOverlayActor(void)
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

  // Connect to the hover signals.
  SignalData data;
  HoverEventFunctor functor( data );
  systemActor.HoveredSignal().Connect( &application, functor );
  actor.HoveredSignal().Connect( &application, functor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Emit a started signal, the system overlay is drawn last so is at the top, should hit the systemActor.
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( systemActor == data.hoveredActor );
  END_TEST;
}

int UtcDaliHoverLeaveActorReadded(void)
{
  TestApplication application;
  Stage stage = Stage::GetCurrent();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  stage.Add(actor);

  // Set actor to receive hover-events
  actor.SetLeaveRequired( true );

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal
  SignalData data;
  HoverEventFunctor functor( data );
  actor.HoveredSignal().Connect( &application, functor );

  // Emit a started and motion
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, Vector2( 11.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Remove actor from stage and add again
  stage.Remove( actor );
  stage.Add( actor );

  // Emit a motion within the actor's bounds
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, Vector2( 12.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Emit a motion outside the actor's bounds
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Motion, Vector2( 200.0f, 200.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( TouchPoint::Leave, data.hoverEvent.points[0].state, TEST_LOCATION );
  data.Reset();

  END_TEST;
}


int UtcDaliHoverStencilNonRenderableActor(void)
{
  TestApplication application;
  Stage stage = Stage::GetCurrent();

  Actor actor = Actor::New();
  actor.SetSize(100.0f, 100.0f);
  actor.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  stage.Add(actor);

  Actor stencil = Actor::New();
  stencil.SetSize(50.0f, 50.0f);
  stencil.SetAnchorPoint(AnchorPoint::TOP_LEFT);
  stencil.SetDrawMode( DrawMode::STENCIL );
  stage.Add(stencil);

  // Render and notify
  application.SendNotification();
  application.Render();

  // Connect to actor's hovered signal
  SignalData data;
  HoverEventFunctor functor( data );
  actor.HoveredSignal().Connect( &application, functor );

  // Emit an event within stencil area
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 10.0f, 10.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  // Emit an event outside the stencil area but within the actor area, we should have a hit!
  application.ProcessEvent( GenerateSingleHover( TouchPoint::Started, Vector2( 60.0f, 60.0f ) ) );
  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  data.Reset();

  END_TEST;
}
