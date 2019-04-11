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

// CLASS HEADER
#include <dali/internal/event/events/touch-event-processor.h>

#if defined(DEBUG_ENABLED)
#include <sstream>
#endif

// INTERNAL INCLUDES
#include <dali/public-api/events/touch-data.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/signals/callback.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/events/hit-test-algorithm-impl.h>
#include <dali/internal/event/events/multi-point-event-util.h>
#include <dali/internal/event/events/touch-data-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>

namespace Dali
{

namespace Internal
{

namespace
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_TOUCH_PROCESSOR" );

const char * TOUCH_POINT_STATE[ 6 ] =
{
  "DOWN",
  "UP",
  "MOTION",
  "LEAVE",
  "STATIONARY",
  "INTERRUPTED",
};

#endif // defined(DEBUG_ENABLED)


/**
 *  Recursively deliver events to the actor and its parents, until the event is consumed or the stage is reached.
 */
Dali::Actor EmitTouchSignals( Dali::Actor actor, const TouchEvent& event, const Dali::TouchData& touchData )
{
  Dali::Actor consumedActor;

  if ( actor )
  {
    Dali::Actor oldParent( actor.GetParent() );

    Actor& actorImpl( GetImplementation(actor) );

    bool consumed( false );

    // Only emit the signal if the actor's touch signal has connections (or derived actor implementation requires touch).
    if ( actorImpl.GetTouchRequired() )
    {
      consumed = actorImpl.EmitTouchEventSignal( event, touchData );
    }

    if ( consumed )
    {
      // One of this actor's listeners has consumed the event so set this actor as the consumed actor.
      consumedActor = Dali::Actor( &actorImpl );
    }
    else
    {
      // The actor may have been removed/reparented during the signal callbacks.
      Dali::Actor parent = actor.GetParent();

      if ( parent &&
           (parent == oldParent) )
      {
        // One of the actor's parents may consumed the event and they should be set as the consumed actor.
        consumedActor = EmitTouchSignals( parent, event, touchData );
      }
    }
  }

  return consumedActor;
}

Dali::Actor AllocAndEmitTouchSignals( unsigned long time,  Dali::Actor actor, const Integration::Point& point )
{
  TouchEvent touchEvent( time );
  TouchDataPtr touchData( new TouchData( time ) );
  Dali::TouchData touchDataHandle( touchData.Get() );

  touchEvent.points.push_back( point.GetTouchPoint() );
  touchData->AddPoint( point );

  return EmitTouchSignals( actor, touchEvent, touchDataHandle );
}


/**
 * Changes the state of the primary point to leave and emits the touch signals
 */
Dali::Actor EmitTouchSignals( Actor* actor, RenderTask& renderTask, const TouchEvent& originalEvent, const TouchDataPtr& originalTouchData, PointState::Type state )
{
  Dali::Actor consumingActor;

  if( actor )
  {
    TouchDataPtr touchData = TouchData::Clone( *originalTouchData.Get() );

    Integration::Point& primaryPoint = touchData->GetPoint( 0 );

    const Vector2& screenPosition = primaryPoint.GetScreenPosition();
    Vector2 localPosition;
    actor->ScreenToLocal( renderTask, localPosition.x, localPosition.y, screenPosition.x, screenPosition.y );

    primaryPoint.SetLocalPosition( localPosition );
    primaryPoint.SetHitActor( Dali::Actor( actor ) );
    primaryPoint.SetState( state );

    TouchEvent touchEvent( originalEvent );
    touchEvent.points[0] = primaryPoint.GetTouchPoint();

    consumingActor = EmitTouchSignals( Dali::Actor(actor), touchEvent, Dali::TouchData( touchData.Get() ) );
  }

  return consumingActor;
}

} // unnamed namespace

TouchEventProcessor::TouchEventProcessor( Scene& scene )
: mScene( scene ),
  mLastPrimaryHitActor( MakeCallback( this, &TouchEventProcessor::OnObservedActorDisconnected ) ),
  mLastConsumedActor(),
  mTouchDownConsumedActor(),
  mLastRenderTask()
{
  DALI_LOG_TRACE_METHOD( gLogFilter );
}

TouchEventProcessor::~TouchEventProcessor()
{
  DALI_LOG_TRACE_METHOD( gLogFilter );
}

void TouchEventProcessor::ProcessTouchEvent( const Integration::TouchEvent& event )
{
  DALI_LOG_TRACE_METHOD( gLogFilter );
  DALI_ASSERT_ALWAYS( !event.points.empty() && "Empty TouchEvent sent from Integration\n" );

  PRINT_HIERARCHY(gLogFilter);

  // 1) Check if it is an interrupted event - we should inform our last primary hit actor about this
  //    and emit the stage signal as well.

  if ( event.points[0].GetState() == PointState::INTERRUPTED )
  {
    Dali::Actor consumingActor;
    Integration::Point currentPoint( event.points[0] );

    Actor* lastPrimaryHitActor( mLastPrimaryHitActor.GetActor() );
    if ( lastPrimaryHitActor )
    {
      Dali::Actor lastPrimaryHitActorHandle( lastPrimaryHitActor );
      currentPoint.SetHitActor( lastPrimaryHitActorHandle );

      consumingActor = AllocAndEmitTouchSignals( event.time, lastPrimaryHitActorHandle, currentPoint );
    }

    // If the last consumed actor was different to the primary hit actor then inform it as well (if it has not already been informed).
    Actor* lastConsumedActor( mLastConsumedActor.GetActor() );
    if ( lastConsumedActor &&
         lastConsumedActor != lastPrimaryHitActor &&
         lastConsumedActor != consumingActor )
    {
      Dali::Actor lastConsumedActorHandle( lastConsumedActor );
      currentPoint.SetHitActor( lastConsumedActorHandle );
      AllocAndEmitTouchSignals( event.time, lastConsumedActorHandle, currentPoint );
    }

    // Tell the touch-down consuming actor as well, if required
    Actor* touchDownConsumedActor( mTouchDownConsumedActor.GetActor() );
    if ( touchDownConsumedActor &&
         touchDownConsumedActor != lastPrimaryHitActor &&
         touchDownConsumedActor != lastConsumedActor &&
         touchDownConsumedActor != consumingActor )
    {
      Dali::Actor touchDownConsumedActorHandle( touchDownConsumedActor );

      currentPoint.SetHitActor( touchDownConsumedActorHandle );
      AllocAndEmitTouchSignals( event.time, touchDownConsumedActorHandle, currentPoint );
    }

    mLastPrimaryHitActor.SetActor( NULL );
    mLastConsumedActor.SetActor( NULL );
    mTouchDownConsumedActor.SetActor( NULL );
    mLastRenderTask.Reset();

    currentPoint.SetHitActor( Dali::Actor() );

    TouchEvent touchEvent( event.time );
    TouchDataPtr touchData( new TouchData( event.time ) );
    Dali::TouchData touchDataHandle( touchData.Get() );

    touchEvent.points.push_back( currentPoint.GetTouchPoint() );
    touchData->AddPoint( currentPoint );

    mScene.EmitTouchedSignal( touchEvent, touchDataHandle );
    return; // No need for hit testing
  }

  // 2) Hit Testing.
  TouchEvent touchEvent( event.time );
  TouchDataPtr touchData( new TouchData( event.time ) );
  Dali::TouchData touchDataHandle( touchData.Get() );

  DALI_LOG_INFO( gLogFilter, Debug::Concise, "\n" );
  DALI_LOG_INFO( gLogFilter, Debug::General, "Point(s): %d\n", event.GetPointCount() );

  RenderTaskPtr currentRenderTask;

  for ( Integration::PointContainerConstIterator iter = event.points.begin(), beginIter = event.points.begin(), endIter = event.points.end(); iter != endIter; ++iter )
  {
    HitTestAlgorithm::Results hitTestResults;
    HitTestAlgorithm::HitTest( mScene.GetSize(), mScene.GetRenderTaskList(), mScene.GetLayerList(), iter->GetScreenPosition(), hitTestResults );

    Integration::Point newPoint( *iter );
    newPoint.SetHitActor( hitTestResults.actor );
    newPoint.SetLocalPosition( hitTestResults.actorCoordinates );

    touchEvent.points.push_back( newPoint.GetTouchPoint() );
    touchData->AddPoint( newPoint );

    DALI_LOG_INFO( gLogFilter, Debug::General, "  State(%s), Screen(%.0f, %.0f), HitActor(%p, %s), Local(%.2f, %.2f)\n",
                   TOUCH_POINT_STATE[iter->GetState()], iter->GetScreenPosition().x, iter->GetScreenPosition().y,
                   ( hitTestResults.actor ? reinterpret_cast< void* >( &hitTestResults.actor.GetBaseObject() ) : NULL ),
                   ( hitTestResults.actor ? hitTestResults.actor.GetName().c_str() : "" ),
                   hitTestResults.actorCoordinates.x, hitTestResults.actorCoordinates.y );

    // Only set the currentRenderTask for the primary hit actor.
    if ( iter == beginIter && hitTestResults.renderTask )
    {
      currentRenderTask = hitTestResults.renderTask;
    }
  }

  // 3) Recursively deliver events to the actor and its parents, until the event is consumed or the stage is reached.

  // Emit the touch signal
  Dali::Actor consumedActor;
  if ( currentRenderTask )
  {
    consumedActor = EmitTouchSignals( touchData->GetPoint( 0 ).GetHitActor(), touchEvent, touchDataHandle );
  }

  Integration::Point& primaryPoint = touchData->GetPoint( 0 );
  Dali::Actor primaryHitActor = primaryPoint.GetHitActor();
  PointState::Type primaryPointState = primaryPoint.GetState();

  DALI_LOG_INFO( gLogFilter, Debug::Concise, "PrimaryHitActor:     (%p) %s\n", primaryHitActor ? reinterpret_cast< void* >( &primaryHitActor.GetBaseObject() ) : NULL, primaryHitActor ? primaryHitActor.GetName().c_str() : "" );
  DALI_LOG_INFO( gLogFilter, Debug::Concise, "ConsumedActor:       (%p) %s\n", consumedActor ? reinterpret_cast< void* >( &consumedActor.GetBaseObject() ) : NULL, consumedActor ? consumedActor.GetName().c_str() : "" );

  if ( ( primaryPointState == PointState::DOWN ) &&
       ( touchEvent.GetPointCount() == 1 ) &&
       ( consumedActor && consumedActor.OnStage() ) )
  {
    mTouchDownConsumedActor.SetActor( &GetImplementation( consumedActor ) );
  }

  // 4) Check if the last primary hit actor requires a leave event and if it was different to the current primary
  //    hit actor.  Also process the last consumed actor in the same manner.

  Actor* lastPrimaryHitActor( mLastPrimaryHitActor.GetActor() );
  Actor* lastConsumedActor( mLastConsumedActor.GetActor() );
  if( ( primaryPointState == PointState::MOTION ) || ( primaryPointState == PointState::UP ) || ( primaryPointState == PointState::STATIONARY ) )
  {
    if( mLastRenderTask )
    {
      Dali::Actor leaveEventConsumer;
      RenderTask& lastRenderTaskImpl = *mLastRenderTask.Get();

      if( lastPrimaryHitActor &&
          lastPrimaryHitActor != primaryHitActor &&
          lastPrimaryHitActor != consumedActor )
      {
        if( lastPrimaryHitActor->IsHittable() && IsActuallySensitive( lastPrimaryHitActor ) )
        {
          if ( lastPrimaryHitActor->GetLeaveRequired() )
          {
            DALI_LOG_INFO( gLogFilter, Debug::Concise, "LeaveActor(Hit):     (%p) %s\n", reinterpret_cast< void* >( lastPrimaryHitActor ), lastPrimaryHitActor->GetName().c_str() );
            leaveEventConsumer = EmitTouchSignals( mLastPrimaryHitActor.GetActor(), lastRenderTaskImpl, touchEvent, touchData, PointState::LEAVE );
          }
        }
        else
        {
          // At this point mLastPrimaryHitActor was touchable and sensitive in the previous touch event process but is not in the current one.
          // An interrupted event is send to allow some actors to go back to their original state (i.e. Button controls)
          DALI_LOG_INFO( gLogFilter, Debug::Concise, "InterruptedActor(Hit):     (%p) %s\n", reinterpret_cast< void* >( lastPrimaryHitActor ), lastPrimaryHitActor->GetName().c_str() );
          leaveEventConsumer = EmitTouchSignals( mLastPrimaryHitActor.GetActor(), lastRenderTaskImpl, touchEvent, touchData, PointState::INTERRUPTED );
        }
      }

      // Check if the motion event has been consumed by another actor's listener.  In this case, the previously
      // consumed actor's listeners may need to be informed (through a leave event).
      // Further checks here to ensure we do not signal the same actor twice for the same event.
      if ( lastConsumedActor &&
           lastConsumedActor != consumedActor &&
           lastConsumedActor != lastPrimaryHitActor &&
           lastConsumedActor != primaryHitActor &&
           lastConsumedActor != leaveEventConsumer )
      {
        if( lastConsumedActor->IsHittable() && IsActuallySensitive( lastConsumedActor ) )
        {
          if( lastConsumedActor->GetLeaveRequired() )
          {
            DALI_LOG_INFO( gLogFilter, Debug::Concise, "LeaveActor(Consume): (%p) %s\n", reinterpret_cast< void* >( lastConsumedActor ), lastConsumedActor->GetName().c_str() );
            EmitTouchSignals( lastConsumedActor, lastRenderTaskImpl, touchEvent, touchData, PointState::LEAVE );
          }
        }
        else
        {
          // At this point mLastConsumedActor was touchable and sensitive in the previous touch event process but is not in the current one.
          // An interrupted event is send to allow some actors to go back to their original state (i.e. Button controls)
          DALI_LOG_INFO( gLogFilter, Debug::Concise, "InterruptedActor(Consume):     (%p) %s\n", reinterpret_cast< void* >( lastConsumedActor ), lastConsumedActor->GetName().c_str() );
          EmitTouchSignals( mLastConsumedActor.GetActor(), lastRenderTaskImpl, touchEvent, touchData, PointState::INTERRUPTED );
        }
      }
    }
  }

  // 5) If our primary point is an Up event, then the primary point (in multi-touch) will change next
  //    time so set our last primary actor to NULL.  Do the same to the last consumed actor as well.

  if ( primaryPointState == PointState::UP )
  {
    mLastPrimaryHitActor.SetActor( NULL );
    mLastConsumedActor.SetActor( NULL );
    mLastRenderTask.Reset();
  }
  else
  {
    // The primaryHitActor may have been removed from the stage so ensure it is still on the stage before setting members.
    if ( primaryHitActor && primaryHitActor.OnStage() )
    {
      mLastPrimaryHitActor.SetActor( &GetImplementation( primaryHitActor ) );

      // Only observe the consumed actor if we have a primaryHitActor (check if it is still on stage).
      if ( consumedActor && consumedActor.OnStage() )
      {
        mLastConsumedActor.SetActor( &GetImplementation( consumedActor ) );
      }
      else
      {
        mLastConsumedActor.SetActor( NULL );
      }

      mLastRenderTask = currentRenderTask;
    }
    else
    {
      mLastPrimaryHitActor.SetActor( NULL );
      mLastConsumedActor.SetActor( NULL );
      mLastRenderTask.Reset();
    }
  }

  // 6) Emit an interrupted event to the touch-down actor if it hasn't consumed the up and
  //    emit the stage touched event if required.

  if ( touchEvent.GetPointCount() == 1 ) // Only want the first touch and the last release
  {
    switch ( primaryPointState )
    {
      case PointState::UP:
      {
        Actor* touchDownConsumedActor( mTouchDownConsumedActor.GetActor() );
        if ( touchDownConsumedActor &&
             touchDownConsumedActor != consumedActor &&
             touchDownConsumedActor != lastPrimaryHitActor &&
             touchDownConsumedActor != lastConsumedActor )
        {
          Dali::Actor touchDownConsumedActorHandle( touchDownConsumedActor );

          Integration::Point currentPoint = touchData->GetPoint( 0 );
          currentPoint.SetHitActor( touchDownConsumedActorHandle );
          currentPoint.SetState( PointState::INTERRUPTED );

          AllocAndEmitTouchSignals( event.time, touchDownConsumedActorHandle, currentPoint );
        }

        mTouchDownConsumedActor.SetActor( NULL );
      }
      // No break, Fallthrough

      case PointState::DOWN:
      {
        mScene.EmitTouchedSignal( touchEvent, touchDataHandle );
        break;
      }

      case PointState::MOTION:
      case PointState::LEAVE:
      case PointState::STATIONARY:
      case PointState::INTERRUPTED:
      {
        // Ignore
        break;
      }
    }
  }
}

void TouchEventProcessor::OnObservedActorDisconnected( Actor* actor )
{
  if ( actor == mLastPrimaryHitActor.GetActor() )
  {
    Dali::Actor handle( actor );

    Integration::Point point;
    point.SetState( PointState::INTERRUPTED );
    point.SetHitActor( handle );

    TouchDataPtr touchData( new TouchData );
    touchData->AddPoint( point );
    Dali::TouchData touchDataHandle( touchData.Get() );

    TouchEvent touchEvent( 0 );
    touchEvent.points.push_back( point.GetTouchPoint() );

    Dali::Actor eventConsumer = EmitTouchSignals( handle, touchEvent, touchDataHandle );

    if ( mLastConsumedActor.GetActor() != eventConsumer )
    {
      EmitTouchSignals( Dali::Actor( mLastConsumedActor.GetActor() ), touchEvent, touchDataHandle );
    }

    // Do not set mLastPrimaryHitActor to NULL we may be iterating through its observers

    mLastConsumedActor.SetActor( NULL );
    mLastRenderTask.Reset();
  }
}

} // namespace Internal

} // namespace Dali
