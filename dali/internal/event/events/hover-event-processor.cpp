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

// CLASS HEADER
#include <dali/internal/event/events/hover-event-processor.h>

#if defined(DEBUG_ENABLED)
#include <sstream>
#endif

// INTERNAL INCLUDES
#include <dali/public-api/actors/renderable-actor.h>
#include <dali/public-api/math/vector2.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/events/hover-event-integ.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/events/hit-test-algorithm-impl.h>
#include <dali/internal/event/events/multi-point-event-util.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>

namespace Dali
{

namespace Internal
{

namespace
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_HOVER_PROCESSOR" );

const char * TOUCH_POINT_STATE[TouchPoint::Last] =
{
  "Started",
  "Finished",
  "Motion",
  "Leave",
  "Stationary",
  "Interrupted",
};

#endif // defined(DEBUG_ENABLED)

/**
 *  Recursively deliver events to the actor and its parents, until the event is consumed or the stage is reached.
 */
Dali::Actor EmitHoverSignals( Dali::Actor actor, const HoverEvent& event )
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

/**
 * Changes the state of the primary point to leave and emits the hover signals
 */
Dali::Actor EmitHoverSignals( Actor* actor, RenderTask& renderTask, const HoverEvent& originalEvent, TouchPoint::State state )
{
  HoverEvent hoverEvent( originalEvent );

  DALI_ASSERT_DEBUG( NULL != actor && "NULL actor pointer" );
  if( actor )
  {
    TouchPoint& primaryPoint = hoverEvent.points[0];

    actor->ScreenToLocal( renderTask, primaryPoint.local.x, primaryPoint.local.y, primaryPoint.screen.x, primaryPoint.screen.y );

    primaryPoint.hitActor = Dali::Actor(actor);
    primaryPoint.state = state;
  }

  return EmitHoverSignals( Dali::Actor(actor), hoverEvent );
}

/**
 * Used in the hit-test algorithm to check whether the actor is hoverable.
 */
struct ActorHoverableCheck : public HitTestAlgorithm::HitTestInterface
{
  bool IsActorHittable( Actor* actor )
  {
    return actor->GetHoverRequired() && // Does the Application or derived actor type require a hover event?
           actor->IsHittable();         // Is actor sensitive, visible and on the scene?
  }

  bool DescendActorHierarchy( Actor* actor )
  {
    return actor->IsVisible() && // Actor is visible, if not visible then none of its children are visible.
           actor->IsSensitive(); // Actor is sensitive, if insensitive none of its children should be hittable either.
  }

  bool DoesLayerConsumeHit( Layer* layer )
  {
    return layer->IsHoverConsumed();
  }
};

} // unnamed namespace

HoverEventProcessor::HoverEventProcessor( Stage& stage )
: mStage( stage ),
  mLastPrimaryHitActor(),
  mLastConsumedActor(),
  mHoverStartConsumedActor(),
  mLastRenderTask()
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

  Stage& stage = mStage;

  PRINT_HIERARCHY(gLogFilter);

  // Copy so we can add the results of a hit-test.
  HoverEvent hoverEvent( event.time );

  // 1) Check if it is an interrupted event - we should inform our last primary hit actor about this
  //    and emit the stage signal as well.

  if ( event.points[0].state == TouchPoint::Interrupted )
  {
    Dali::Actor consumingActor;
    hoverEvent.points.push_back(event.points[0]);

    Actor* lastPrimaryHitActor( mLastPrimaryHitActor.GetActor() );
    if ( lastPrimaryHitActor )
    {
      Dali::Actor lastPrimaryHitActorHandle( lastPrimaryHitActor );
      hoverEvent.points[0].hitActor = lastPrimaryHitActorHandle;
      consumingActor = EmitHoverSignals( lastPrimaryHitActorHandle, hoverEvent );
    }

    // If the last consumed actor was different to the primary hit actor then inform it as well (if it has not already been informed).
    Actor* lastConsumedActor( mLastConsumedActor.GetActor() );
    if ( lastConsumedActor &&
         lastConsumedActor != lastPrimaryHitActor &&
         lastConsumedActor != consumingActor )
    {
      Dali::Actor lastConsumedActorHandle( lastConsumedActor );
      hoverEvent.points[0].hitActor = lastConsumedActorHandle;
      EmitHoverSignals( lastConsumedActorHandle, hoverEvent );
    }

    // Tell the hover-start consuming actor as well, if required
    Actor* hoverStartConsumedActor( mHoverStartConsumedActor.GetActor() );
    if ( hoverStartConsumedActor &&
         hoverStartConsumedActor != lastPrimaryHitActor &&
         hoverStartConsumedActor != lastConsumedActor &&
         hoverStartConsumedActor != consumingActor )
    {
      Dali::Actor hoverStartConsumedActorHandle( hoverStartConsumedActor );
      hoverEvent.points[0].hitActor = hoverStartConsumedActorHandle;
      EmitHoverSignals( hoverStartConsumedActorHandle, hoverEvent );
    }

    mLastPrimaryHitActor.SetActor( NULL );
    mLastConsumedActor.SetActor( NULL );
    mHoverStartConsumedActor.SetActor( NULL );
    mLastRenderTask.Reset();

    hoverEvent.points[0].hitActor.Reset();

    return; // No need for hit testing
  }

  // 2) Hit Testing.

  DALI_LOG_INFO( gLogFilter, Debug::Concise, "\n" );
  DALI_LOG_INFO( gLogFilter, Debug::General, "Point(s): %d\n", event.GetPointCount() );

  Dali::RenderTask currentRenderTask;

  for ( TouchPointContainerConstIterator iter = event.points.begin(), beginIter = event.points.begin(), endIter = event.points.end(); iter != endIter; ++iter )
  {
    HitTestAlgorithm::Results hitTestResults;
    ActorHoverableCheck actorHoverableCheck;
    HitTestAlgorithm::HitTest( stage, iter->screen, hitTestResults, actorHoverableCheck );

    TouchPoint newPoint( iter->deviceId, iter->state, iter->screen.x, iter->screen.y );
    newPoint.hitActor = hitTestResults.actor;
    newPoint.local = hitTestResults.actorCoordinates;

    hoverEvent.points.push_back( newPoint );

    DALI_LOG_INFO( gLogFilter, Debug::General, "  State(%s), Screen(%.0f, %.0f), HitActor(%p, %s), Local(%.2f, %.2f)\n",
                   TOUCH_POINT_STATE[iter->state], iter->screen.x, iter->screen.y,
                   ( hitTestResults.actor ? (void*)&hitTestResults.actor.GetBaseObject() : NULL ),
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
    consumedActor = EmitHoverSignals( hoverEvent.points[0].hitActor, hoverEvent );
  }

  TouchPoint& primaryPoint = hoverEvent.points[0];
  Dali::Actor primaryHitActor = primaryPoint.hitActor;
  TouchPoint::State primaryPointState = primaryPoint.state;

  DALI_LOG_INFO( gLogFilter, Debug::Concise, "PrimaryHitActor:     (%p) %s\n", primaryPoint.hitActor ? (void*)&primaryPoint.hitActor.GetBaseObject() : NULL, primaryPoint.hitActor ? primaryPoint.hitActor.GetName().c_str() : "" );
  DALI_LOG_INFO( gLogFilter, Debug::Concise, "ConsumedActor:       (%p) %s\n", consumedActor ? (void*)&consumedActor.GetBaseObject() : NULL, consumedActor ? consumedActor.GetName().c_str() : "" );

  if ( ( primaryPointState == TouchPoint::Started ) &&
       ( hoverEvent.GetPointCount() == 1 ) &&
       ( consumedActor && consumedActor.OnStage() ) )
  {
    mHoverStartConsumedActor.SetActor( &GetImplementation( consumedActor ) );
  }

  // 4) Check if the last primary hit actor requires a leave event and if it was different to the current primary
  //    hit actor.  Also process the last consumed actor in the same manner.

  Actor* lastPrimaryHitActor( mLastPrimaryHitActor.GetActor() );
  Actor* lastConsumedActor( mLastConsumedActor.GetActor() );
  if( (primaryPointState == TouchPoint::Motion) || (primaryPointState == TouchPoint::Finished) || (primaryPointState == TouchPoint::Stationary) )
  {
    if ( mLastRenderTask )
    {
      Dali::Actor leaveEventConsumer;
      RenderTask& lastRenderTaskImpl( GetImplementation( mLastRenderTask ) );

      if( lastPrimaryHitActor &&
          lastPrimaryHitActor != primaryHitActor &&
          lastPrimaryHitActor != consumedActor )
      {
        if( lastPrimaryHitActor->IsHittable() && IsActuallySensitive( lastPrimaryHitActor ) )
        {
          if ( lastPrimaryHitActor->GetLeaveRequired() )
          {
            DALI_LOG_INFO( gLogFilter, Debug::Concise, "LeaveActor(Hit):     (%p) %s\n", (void*)lastPrimaryHitActor, lastPrimaryHitActor->GetName().c_str() );
            leaveEventConsumer = EmitHoverSignals( mLastPrimaryHitActor.GetActor(), lastRenderTaskImpl, hoverEvent, TouchPoint::Leave );
          }
        }
        else
        {
          // At this point mLastPrimaryHitActor was touchable and sensitive in the previous touch event process but is not in the current one.
          // An interrupted event is send to allow some actors to go back to their original state (i.e. Button controls)
          DALI_LOG_INFO( gLogFilter, Debug::Concise, "InterruptedActor(Hit):     (%p) %s\n", (void*)lastPrimaryHitActor, lastPrimaryHitActor->GetName().c_str() );
          leaveEventConsumer = EmitHoverSignals( mLastPrimaryHitActor.GetActor(), lastRenderTaskImpl, hoverEvent, TouchPoint::Interrupted );
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
            DALI_LOG_INFO( gLogFilter, Debug::Concise, "LeaveActor(Consume): (%p) %s\n", (void*)lastConsumedActor, lastConsumedActor->GetName().c_str() );
            EmitHoverSignals( lastConsumedActor, lastRenderTaskImpl, hoverEvent, TouchPoint::Leave );
          }
        }
        else
        {
          // At this point mLastConsumedActor was touchable and sensitive in the previous touch event process but is not in the current one.
          // An interrupted event is send to allow some actors to go back to their original state (i.e. Button controls)
          DALI_LOG_INFO( gLogFilter, Debug::Concise, "InterruptedActor(Consume):     (%p) %s\n", (void*)lastConsumedActor, lastConsumedActor->GetName().c_str() );
          EmitHoverSignals( mLastConsumedActor.GetActor(), lastRenderTaskImpl, hoverEvent, TouchPoint::Interrupted );
        }
      }
    }
  }

  // 5) If our primary point is an Finished event, then the primary point (in multi-touch) will change next
  //    time so set our last primary actor to NULL.  Do the same to the last consumed actor as well.

  if ( primaryPointState == TouchPoint::Finished )
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

  // 6) Emit an interrupted event to the hover-started actor if it hasn't consumed the Finished.

  if ( hoverEvent.GetPointCount() == 1 ) // Only want the first hover started
  {
    switch ( primaryPointState )
    {
      case TouchPoint::Finished:
      {
        Actor* hoverStartConsumedActor( mHoverStartConsumedActor.GetActor() );
        if ( hoverStartConsumedActor &&
             hoverStartConsumedActor != consumedActor &&
             hoverStartConsumedActor != lastPrimaryHitActor &&
             hoverStartConsumedActor != lastConsumedActor )
        {
          Dali::Actor hoverStartConsumedActorHandle( hoverStartConsumedActor );
          hoverEvent.points[0].hitActor = hoverStartConsumedActorHandle;
          hoverEvent.points[0].state = TouchPoint::Interrupted;
          EmitHoverSignals( hoverStartConsumedActorHandle, hoverEvent );

          // Restore hover-event to original state
          hoverEvent.points[0].hitActor = primaryHitActor;
          hoverEvent.points[0].state = primaryPointState;
        }

        mHoverStartConsumedActor.SetActor( NULL );
      }
      // No break, Fallthrough

      case TouchPoint::Started:
      case TouchPoint::Motion:
      case TouchPoint::Leave:
      case TouchPoint::Stationary:
      case TouchPoint::Interrupted:
      case TouchPoint::Last:
      {
        // Ignore
        break;
      }
    }
  }
}

} // namespace Internal

} // namespace Dali
