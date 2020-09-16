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

// CLASS HEADER
#include <dali/internal/event/events/hover-event-processor.h>

#if defined(DEBUG_ENABLED)
#include <sstream>
#endif

// INTERNAL INCLUDES
#include <dali/public-api/math/vector2.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/events/hover-event-integ.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/events/hit-test-algorithm-impl.h>
#include <dali/internal/event/events/multi-point-event-util.h>
#include <dali/internal/event/events/hover-event-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>

namespace Dali
{

namespace Internal
{

namespace
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_HOVER_PROCESSOR" );

const char * TOUCH_POINT_STATE[PointState::INTERRUPTED + 1] =
{
  "STARTED",
  "FINISHED",
  "MOTION",
  "LEAVE",
  "STATIONARY",
  "INTERRUPTED",
};

#endif // defined(DEBUG_ENABLED)

/**
 *  Recursively deliver events to the actor and its parents, until the event is consumed or the stage is reached.
 */
Dali::Actor EmitHoverSignals( Dali::Actor actor, const Dali::HoverEvent& event )
{
  Dali::Actor consumedActor;

  if ( actor )
  {
    Dali::Actor oldParent( actor.GetParent() );

    Actor& actorImpl( GetImplementation(actor) );

    bool consumed( false );

    // Only emit the signal if the actor's hover signal has connections (or derived actor implementation requires hover).
    if ( actorImpl.GetHoverRequired() )
    {
      consumed = actorImpl.EmitHoverEventSignal( event );
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
        consumedActor = EmitHoverSignals( parent, event );
      }
    }
  }

  return consumedActor;
}

Dali::Actor AllocAndEmitHoverSignals( unsigned long time,  Dali::Actor actor, const Integration::Point& point )
{
  HoverEventPtr hoverEvent( new HoverEvent( time ) );
  Dali::HoverEvent hoverEventHandle( hoverEvent.Get() );

  hoverEvent->AddPoint( point );

  return EmitHoverSignals( actor, hoverEventHandle );
}

/**
 * Changes the state of the primary point to leave and emits the hover signals
 */
Dali::Actor EmitHoverSignals( Actor* actor, RenderTask& renderTask, const HoverEventPtr& originalEvent, PointState::Type state )
{
  HoverEventPtr hoverEvent = HoverEvent::Clone( *originalEvent.Get() );

  DALI_ASSERT_DEBUG( NULL != actor && "NULL actor pointer" );
  if( actor )
  {
    Integration::Point& primaryPoint = hoverEvent->GetPoint( 0 );

    const Vector2& screenPosition = primaryPoint.GetScreenPosition();
    Vector2 localPosition;
    actor->ScreenToLocal( renderTask, localPosition.x, localPosition.y, screenPosition.x, screenPosition.y );

    primaryPoint.SetLocalPosition( localPosition );
    primaryPoint.SetHitActor( Dali::Actor( actor ) );
    primaryPoint.SetState( state );
  }

  return EmitHoverSignals( Dali::Actor(actor), Dali::HoverEvent( hoverEvent.Get() ) );
}

/**
 * Used in the hit-test algorithm to check whether the actor is hoverable.
 */
struct ActorHoverableCheck : public HitTestAlgorithm::HitTestInterface
{
  bool IsActorHittable( Actor* actor ) override
  {
    return actor->GetHoverRequired() && // Does the Application or derived actor type require a hover event?
           actor->IsHittable();         // Is actor sensitive, visible and on the scene?
  }

  bool DescendActorHierarchy( Actor* actor ) override
  {
    return actor->IsVisible() && // Actor is visible, if not visible then none of its children are visible.
           actor->IsSensitive(); // Actor is sensitive, if insensitive none of its children should be hittable either.
  }

  bool DoesLayerConsumeHit( Layer* layer ) override
  {
    return layer->IsHoverConsumed();
  }
};

} // unnamed namespace

HoverEventProcessor::HoverEventProcessor( Scene& scene )
: mScene( scene )
{
  DALI_LOG_TRACE_METHOD( gLogFilter );
}

HoverEventProcessor::~HoverEventProcessor()
{
  DALI_LOG_TRACE_METHOD( gLogFilter );
}

void HoverEventProcessor::ProcessHoverEvent( const Integration::HoverEvent& event )
{
  DALI_LOG_TRACE_METHOD( gLogFilter );
  DALI_ASSERT_ALWAYS( !event.points.empty() && "Empty HoverEvent sent from Integration\n" );

  PointState::Type state = static_cast< PointState::Type >( event.points[0].GetState() );

  PRINT_HIERARCHY(gLogFilter);

  // Copy so we can add the results of a hit-test.
  HoverEventPtr hoverEvent( new HoverEvent( event.time ) );

  // 1) Check if it is an interrupted event - we should inform our last primary hit actor about this
  //    and emit the stage signal as well.

  if ( state == PointState::INTERRUPTED )
  {
    Dali::Actor consumingActor;
    Integration::Point currentPoint( event.points[0] );

    Actor* lastPrimaryHitActor( mLastPrimaryHitActor.GetActor() );
    if ( lastPrimaryHitActor )
    {
      Dali::Actor lastPrimaryHitActorHandle( lastPrimaryHitActor );
      currentPoint.SetHitActor( lastPrimaryHitActorHandle );
      consumingActor = AllocAndEmitHoverSignals( event.time, lastPrimaryHitActorHandle, currentPoint );
    }

    // If the last consumed actor was different to the primary hit actor then inform it as well (if it has not already been informed).
    Actor* lastConsumedActor( mLastConsumedActor.GetActor() );
    if ( lastConsumedActor &&
         lastConsumedActor != lastPrimaryHitActor &&
         lastConsumedActor != consumingActor )
    {
      Dali::Actor lastConsumedActorHandle( lastConsumedActor );
      currentPoint.SetHitActor( lastConsumedActorHandle );
      AllocAndEmitHoverSignals( event.time, lastConsumedActorHandle, currentPoint );
    }

    // Tell the hover-start consuming actor as well, if required
    Actor* hoverStartConsumedActor( mHoverStartConsumedActor.GetActor() );
    if ( hoverStartConsumedActor &&
         hoverStartConsumedActor != lastPrimaryHitActor &&
         hoverStartConsumedActor != lastConsumedActor &&
         hoverStartConsumedActor != consumingActor )
    {
      Dali::Actor hoverStartConsumedActorHandle( hoverStartConsumedActor );
      currentPoint.SetHitActor( hoverStartConsumedActorHandle );
      AllocAndEmitHoverSignals( event.time, hoverStartConsumedActorHandle, currentPoint );
    }

    mLastPrimaryHitActor.SetActor( nullptr );
    mLastConsumedActor.SetActor( nullptr );
    mHoverStartConsumedActor.SetActor( nullptr );
    mLastRenderTask.Reset();

    return; // No need for hit testing
  }

  // 2) Hit Testing.

  Dali::HoverEvent hoverEventHandle( hoverEvent.Get() );

  DALI_LOG_INFO( gLogFilter, Debug::Concise, "\n" );
  DALI_LOG_INFO( gLogFilter, Debug::General, "Point(s): %d\n", event.GetPointCount() );

  RenderTaskPtr currentRenderTask;
  bool firstPointParsed = false;

  for ( auto&& currentPoint : event.points )
  {
    HitTestAlgorithm::Results hitTestResults;
    ActorHoverableCheck actorHoverableCheck;
    HitTestAlgorithm::HitTest( mScene.GetSize(), mScene.GetRenderTaskList(), mScene.GetLayerList(), currentPoint.GetScreenPosition(), hitTestResults, actorHoverableCheck );

    Integration::Point newPoint( currentPoint );
    newPoint.SetHitActor( hitTestResults.actor );
    newPoint.SetLocalPosition( hitTestResults.actorCoordinates );

    hoverEvent->AddPoint( newPoint );

    DALI_LOG_INFO( gLogFilter, Debug::General, "  State(%s), Screen(%.0f, %.0f), HitActor(%p, %s), Local(%.2f, %.2f)\n",
                   TOUCH_POINT_STATE[currentPoint.GetState()], currentPoint.GetScreenPosition().x, currentPoint.GetScreenPosition().y,
                   ( hitTestResults.actor ? reinterpret_cast< void* >( &hitTestResults.actor.GetBaseObject() ) : NULL ),
                   ( hitTestResults.actor ? hitTestResults.actor.GetProperty< std::string >( Dali::Actor::Property::NAME ).c_str() : "" ),
                   hitTestResults.actorCoordinates.x, hitTestResults.actorCoordinates.y );

    // Only set the currentRenderTask for the primary hit actor.
    if( !firstPointParsed )
    {
      firstPointParsed = true;
      currentRenderTask = hitTestResults.renderTask;
    }
  }

  // 3) Recursively deliver events to the actor and its parents, until the event is consumed or the stage is reached.

  // Emit the touch signal
  Dali::Actor consumedActor;
  if ( currentRenderTask )
  {
    consumedActor = EmitHoverSignals( hoverEvent->GetHitActor( 0 ), hoverEventHandle );
  }

  Integration::Point primaryPoint = hoverEvent->GetPoint( 0 );
  Dali::Actor primaryHitActor = primaryPoint.GetHitActor();
  PointState::Type primaryPointState = primaryPoint.GetState();

  DALI_LOG_INFO( gLogFilter, Debug::Concise, "PrimaryHitActor:     (%p) %s\n", primaryHitActor ? reinterpret_cast< void* >( &primaryHitActor.GetBaseObject() ) : NULL, primaryHitActor ? primaryHitActor.GetProperty< std::string >( Dali::Actor::Property::NAME ).c_str() : "" );
  DALI_LOG_INFO( gLogFilter, Debug::Concise, "ConsumedActor:       (%p) %s\n", consumedActor ? reinterpret_cast< void* >( &consumedActor.GetBaseObject() ) : NULL, consumedActor ? consumedActor.GetProperty< std::string >( Dali::Actor::Property::NAME ).c_str() : "" );

  if ( ( primaryPointState == PointState::STARTED ) &&
       ( hoverEvent->GetPointCount() == 1 ) &&
       ( consumedActor && GetImplementation( consumedActor ).OnScene() ) )
  {
    mHoverStartConsumedActor.SetActor( &GetImplementation( consumedActor ) );
  }

  // 4) Check if the last primary hit actor requires a leave event and if it was different to the current primary
  //    hit actor.  Also process the last consumed actor in the same manner.

  Actor* lastPrimaryHitActor( mLastPrimaryHitActor.GetActor() );
  Actor* lastConsumedActor( mLastConsumedActor.GetActor() );
  if( (primaryPointState == PointState::MOTION) || (primaryPointState == PointState::FINISHED) || (primaryPointState == PointState::STATIONARY) )
  {
    if ( mLastRenderTask )
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
            leaveEventConsumer = EmitHoverSignals( mLastPrimaryHitActor.GetActor(), lastRenderTaskImpl, hoverEvent, PointState::LEAVE );
          }
        }
        else
        {
          // At this point mLastPrimaryHitActor was touchable and sensitive in the previous touch event process but is not in the current one.
          // An interrupted event is send to allow some actors to go back to their original state (i.e. Button controls)
          DALI_LOG_INFO( gLogFilter, Debug::Concise, "InterruptedActor(Hit):     (%p) %s\n", reinterpret_cast< void* >( lastPrimaryHitActor ), lastPrimaryHitActor->GetName().c_str() );
          leaveEventConsumer = EmitHoverSignals( mLastPrimaryHitActor.GetActor(), lastRenderTaskImpl, hoverEvent, PointState::INTERRUPTED );
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
            EmitHoverSignals( lastConsumedActor, lastRenderTaskImpl, hoverEvent, PointState::LEAVE );
          }
        }
        else
        {
          // At this point mLastConsumedActor was touchable and sensitive in the previous touch event process but is not in the current one.
          // An interrupted event is send to allow some actors to go back to their original state (i.e. Button controls)
          DALI_LOG_INFO( gLogFilter, Debug::Concise, "InterruptedActor(Consume):     (%p) %s\n", reinterpret_cast< void* >( lastConsumedActor ), lastConsumedActor->GetName().c_str() );
          EmitHoverSignals( mLastConsumedActor.GetActor(), lastRenderTaskImpl, hoverEvent, PointState::INTERRUPTED );
        }
      }
    }
  }

  // 5) If our primary point is a FINISHED event, then the primary point (in multi-touch) will change next
  //    time so set our last primary actor to NULL.  Do the same to the last consumed actor as well.

  if ( primaryPointState == PointState::FINISHED )
  {
    mLastPrimaryHitActor.SetActor( nullptr );
    mLastConsumedActor.SetActor( nullptr );
    mLastRenderTask.Reset();
  }
  else
  {
    // The primaryHitActor may have been removed from the scene so ensure it is still on the scene before setting members.
    if ( primaryHitActor && GetImplementation( primaryHitActor ).OnScene() )
    {
      mLastPrimaryHitActor.SetActor( &GetImplementation( primaryHitActor ) );

      // Only observe the consumed actor if we have a primaryHitActor (check if it is still on the scene).
      if ( consumedActor && GetImplementation( consumedActor ).OnScene() )
      {
        mLastConsumedActor.SetActor( &GetImplementation( consumedActor ) );
      }
      else
      {
        mLastConsumedActor.SetActor( nullptr );
      }

      mLastRenderTask = currentRenderTask;
    }
    else
    {
      mLastPrimaryHitActor.SetActor( nullptr );
      mLastConsumedActor.SetActor( nullptr );
      mLastRenderTask.Reset();
    }
  }

  // 6) Emit an interrupted event to the hover-started actor if it hasn't consumed the FINISHED.

  if ( hoverEvent->GetPointCount() == 1 ) // Only want the first hover started
  {
    switch ( primaryPointState )
    {
      case PointState::FINISHED:
      {
        Actor* hoverStartConsumedActor( mHoverStartConsumedActor.GetActor() );
        if ( hoverStartConsumedActor &&
             hoverStartConsumedActor != consumedActor &&
             hoverStartConsumedActor != lastPrimaryHitActor &&
             hoverStartConsumedActor != lastConsumedActor )
        {
          Dali::Actor hoverStartConsumedActorHandle( hoverStartConsumedActor );
          Integration::Point primaryPoint = hoverEvent->GetPoint( 0 );
          primaryPoint.SetHitActor( hoverStartConsumedActorHandle );
          primaryPoint.SetState( PointState::INTERRUPTED );
          AllocAndEmitHoverSignals( event.time, hoverStartConsumedActorHandle, primaryPoint );

          // Restore hover-event to original state
          primaryPoint.SetHitActor( primaryHitActor );
          primaryPoint.SetState( primaryPointState );
        }

        mHoverStartConsumedActor.SetActor( nullptr );
      }
      // No break, Fallthrough

      case PointState::STARTED:
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

} // namespace Internal

} // namespace Dali
