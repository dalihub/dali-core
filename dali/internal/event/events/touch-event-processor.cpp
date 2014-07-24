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
#include <dali/internal/event/events/touch-event-processor.h>

#if defined(DEBUG_ENABLED)
#include <sstream>
#endif

// INTERNAL INCLUDES
#include <dali/public-api/actors/renderable-actor.h>
#include <dali/public-api/math/vector2.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/events/hit-test-algorithm-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>

namespace Dali
{

namespace Internal
{

namespace
{

bool IsActuallySensitive( Actor* actor );

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_TOUCH_PROCESSOR" );

const char * TOUCH_POINT_STATE[TouchPoint::Last] =
{
  "Down",
  "Up",
  "Motion",
  "Leave",
  "Stationary",
  "Interrupted",
};

static const Debug::LogLevel HIERARCHY_DEBUG_LOG_LEVEL( Debug::Verbose );

static bool HIERARCHY_GEOMETRY( false );
static bool HIERARCHY_SENSITIVITY( false );
static bool HIERARCHY_TOUCH_REQUIRED( false );
static bool HIERARCHY_HITTABLE( false );

/**
 * Prints out all the children of the given actor when debug is enabled.
 *
 * @param[in]  actor  The actor whose children to print.
 * @param[in]  level  The number of " | " to put in front of the children.
 */
void PrintChildren( Dali::Actor actor, int level )
{
  std::ostringstream output;

  for ( int t = 0; t < level; ++t )
  {
    output << " | ";
  }

  output << actor.GetName() << "(" << actor.GetTypeName() << ", " << actor.GetObjectPtr() << ")";

  if ( HIERARCHY_GEOMETRY )
  {
    output << " Pos: " << actor.GetCurrentWorldPosition() << " Size: " << actor.GetCurrentSize() << " Scale: " << actor.GetCurrentWorldScale();
  }

  if ( HIERARCHY_SENSITIVITY )
  {
    output << " Sensitivity: " << ( IsActuallySensitive( &GetImplementation( actor ) ) ? "True " : "False " );
  }

  if ( HIERARCHY_TOUCH_REQUIRED )
  {
    output << " TouchRequired: " << ( GetImplementation(actor).GetTouchRequired() ? "True " : "False " );
  }

  if ( HIERARCHY_HITTABLE )
  {
    output << " Hittable: " << ( GetImplementation(actor).IsHittable() ? "True " : "False " );
  }

  output << std::endl;

  DALI_LOG_INFO( gLogFilter, HIERARCHY_DEBUG_LOG_LEVEL, output.str().c_str() );

  ++level;
  unsigned int numChildren=actor.GetChildCount();
  for ( unsigned int i=0; i<numChildren; ++i )
  {
    PrintChildren( actor.GetChildAt(i), level );
  }
  --level;
}

/**
 * Prints the entire hierarchy of the scene.
 */
void PrintHierarchy()
{
  if ( gLogFilter->IsEnabledFor( HIERARCHY_DEBUG_LOG_LEVEL ) )
  {
    PrintChildren( Dali::Stage().GetCurrent().GetRootLayer(), 0 );
  }
}

#define PRINT_HIERARCHY PrintHierarchy()

#else // defined(DEBUG_ENABLED)

#define PRINT_HIERARCHY

#endif // defined(DEBUG_ENABLED)

/**
 *  Recursively deliver events to the actor and its parents, until the event is consumed or the stage is reached.
 */
Dali::Actor EmitTouchSignals( Dali::Actor actor, const TouchEvent& event )
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
      consumed = actorImpl.EmitTouchEventSignal( event );
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
        consumedActor = EmitTouchSignals( parent, event );
      }
    }
  }

  return consumedActor;
}

/**
 * Changes the state of the primary point to leave and emits the touch signals
 */
Dali::Actor EmitTouchSignals( Actor* actor, RenderTask& renderTask, const TouchEvent& originalEvent, TouchPoint::State state )
{
  TouchEvent touchEvent( originalEvent );
  TouchPoint& primaryPoint = touchEvent.points[0];

  actor->ScreenToLocal( renderTask, primaryPoint.local.x, primaryPoint.local.y, primaryPoint.screen.x, primaryPoint.screen.y );

  primaryPoint.hitActor = actor;
  primaryPoint.state = state;

  return EmitTouchSignals( Dali::Actor(actor), touchEvent );
}

/**
 * In the hit test algorithm above we do not descend actor tree if it is insensitive, so here, we
 * should also check if any of the actor's parents has become insensitive since we last processed
 * it.
 */
bool IsActuallySensitive( Actor* actor )
{
  bool sensitive = true;

  while ( actor && sensitive )
  {
    sensitive = actor->IsSensitive();
    actor = actor->GetParent();
  }

  return sensitive;
}

} // unnamed namespace

TouchEventProcessor::TouchEventProcessor( Stage& stage )
: mStage( stage ),
  mLastPrimaryHitActor(),
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

  Stage& stage = mStage;

  PRINT_HIERARCHY;

  // Copy so we can add the results of a hit-test.
  TouchEvent touchEvent( event.time );

  // 1) Check if it is an interrupted event - we should inform our last primary hit actor about this
  //    and emit the stage signal as well.

  if ( event.points[0].state == TouchPoint::Interrupted )
  {
    Dali::Actor consumingActor;
    touchEvent.points.push_back(event.points[0]);

    Actor* lastPrimaryHitActor( mLastPrimaryHitActor.GetActor() );
    if ( lastPrimaryHitActor )
    {
      Dali::Actor lastPrimaryHitActorHandle( lastPrimaryHitActor );
      touchEvent.points[0].hitActor = lastPrimaryHitActorHandle;
      consumingActor = EmitTouchSignals( lastPrimaryHitActorHandle, touchEvent );
    }

    // If the last consumed actor was different to the primary hit actor then inform it as well (if it has not already been informed).
    Actor* lastConsumedActor( mLastConsumedActor.GetActor() );
    if ( lastConsumedActor &&
         lastConsumedActor != lastPrimaryHitActor &&
         lastConsumedActor != consumingActor )
    {
      Dali::Actor lastConsumedActorHandle( lastConsumedActor );
      touchEvent.points[0].hitActor = lastConsumedActorHandle;
      EmitTouchSignals( lastConsumedActorHandle, touchEvent );
    }

    // Tell the touch-down consuming actor as well, if required
    Actor* touchDownConsumedActor( mTouchDownConsumedActor.GetActor() );
    if ( touchDownConsumedActor &&
         touchDownConsumedActor != lastPrimaryHitActor &&
         touchDownConsumedActor != lastConsumedActor &&
         touchDownConsumedActor != consumingActor )
    {
      Dali::Actor touchDownConsumedActorHandle( touchDownConsumedActor );
      touchEvent.points[0].hitActor = touchDownConsumedActorHandle;
      EmitTouchSignals( touchDownConsumedActorHandle, touchEvent );
    }

    mLastPrimaryHitActor.SetActor( NULL );
    mLastConsumedActor.SetActor( NULL );
    mTouchDownConsumedActor.SetActor( NULL );
    mLastRenderTask.Reset();

    touchEvent.points[0].hitActor = NULL;
    mStage.EmitTouchedSignal( touchEvent );

    return; // No need for hit testing
  }

  // 2) Hit Testing.

  DALI_LOG_INFO( gLogFilter, Debug::Concise, "\n" );
  DALI_LOG_INFO( gLogFilter, Debug::General, "Point(s): %d\n", event.GetPointCount() );

  Dali::RenderTask currentRenderTask;

  for ( TouchPointContainerConstIterator iter = event.points.begin(), beginIter = event.points.begin(), endIter = event.points.end(); iter != endIter; ++iter )
  {
    HitTestAlgorithm::Results hitTestResults;
    HitTestAlgorithm::HitTest( stage, iter->screen, hitTestResults );

    TouchPoint newPoint( iter->deviceId, iter->state, iter->screen.x, iter->screen.y );
    newPoint.hitActor = hitTestResults.actor;
    newPoint.local = hitTestResults.actorCoordinates;

    touchEvent.points.push_back( newPoint );

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
    consumedActor = EmitTouchSignals( touchEvent.points[0].hitActor, touchEvent );
  }

  TouchPoint& primaryPoint = touchEvent.points[0];
  Dali::Actor primaryHitActor = primaryPoint.hitActor;
  TouchPoint::State primaryPointState = primaryPoint.state;

  DALI_LOG_INFO( gLogFilter, Debug::Concise, "PrimaryHitActor:     (%p) %s\n", primaryPoint.hitActor ? (void*)&primaryPoint.hitActor.GetBaseObject() : NULL, primaryPoint.hitActor ? primaryPoint.hitActor.GetName().c_str() : "" );
  DALI_LOG_INFO( gLogFilter, Debug::Concise, "ConsumedActor:       (%p) %s\n", consumedActor ? (void*)&consumedActor.GetBaseObject() : NULL, consumedActor ? consumedActor.GetName().c_str() : "" );

  if ( ( primaryPointState == TouchPoint::Down ) &&
       ( touchEvent.GetPointCount() == 1 ) &&
       ( consumedActor && consumedActor.OnStage() ) )
  {
    mTouchDownConsumedActor.SetActor( &GetImplementation( consumedActor ) );
  }

  // 4) Check if the last primary hit actor requires a leave event and if it was different to the current primary
  //    hit actor.  Also process the last consumed actor in the same manner.

  Actor* lastPrimaryHitActor( mLastPrimaryHitActor.GetActor() );
  Actor* lastConsumedActor( mLastConsumedActor.GetActor() );
  if( (primaryPointState == TouchPoint::Motion) || (primaryPointState == TouchPoint::Up) || (primaryPointState == TouchPoint::Stationary) )
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
            leaveEventConsumer = EmitTouchSignals( mLastPrimaryHitActor.GetActor(), lastRenderTaskImpl, touchEvent, TouchPoint::Leave );
          }
        }
        else
        {
          // At this point mLastPrimaryHitActor was touchable and sensitive in the previous touch event process but is not in the current one.
          // An interrupted event is send to allow some actors to go back to their original state (i.e. Button controls)
          DALI_LOG_INFO( gLogFilter, Debug::Concise, "InterruptedActor(Hit):     (%p) %s\n", (void*)lastPrimaryHitActor, lastPrimaryHitActor->GetName().c_str() );
          leaveEventConsumer = EmitTouchSignals( mLastPrimaryHitActor.GetActor(), lastRenderTaskImpl, touchEvent, TouchPoint::Interrupted );
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
            EmitTouchSignals( lastConsumedActor, lastRenderTaskImpl, touchEvent, TouchPoint::Leave );
          }
        }
        else
        {
          // At this point mLastConsumedActor was touchable and sensitive in the previous touch event process but is not in the current one.
          // An interrupted event is send to allow some actors to go back to their original state (i.e. Button controls)
          DALI_LOG_INFO( gLogFilter, Debug::Concise, "InterruptedActor(Consume):     (%p) %s\n", (void*)lastConsumedActor, lastConsumedActor->GetName().c_str() );
          EmitTouchSignals( mLastConsumedActor.GetActor(), lastRenderTaskImpl, touchEvent, TouchPoint::Interrupted );
        }
      }
    }
  }

  // 5) If our primary point is an Up event, then the primary point (in multi-touch) will change next
  //    time so set our last primary actor to NULL.  Do the same to the last consumed actor as well.

  if ( primaryPointState == TouchPoint::Up )
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
      case TouchPoint::Up:
      {
        Actor* touchDownConsumedActor( mTouchDownConsumedActor.GetActor() );
        if ( touchDownConsumedActor &&
             touchDownConsumedActor != consumedActor &&
             touchDownConsumedActor != lastPrimaryHitActor &&
             touchDownConsumedActor != lastConsumedActor )
        {
          Dali::Actor touchDownConsumedActorHandle( touchDownConsumedActor );
          touchEvent.points[0].hitActor = touchDownConsumedActorHandle;
          touchEvent.points[0].state = TouchPoint::Interrupted;
          EmitTouchSignals( touchDownConsumedActorHandle, touchEvent );

          // Restore touch-event to original state
          touchEvent.points[0].hitActor = primaryHitActor;
          touchEvent.points[0].state = primaryPointState;
        }

        mTouchDownConsumedActor.SetActor( NULL );
      }
      // No break, Fallthrough

      case TouchPoint::Down:
      {
        mStage.EmitTouchedSignal( touchEvent );
        break;
      }

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

TouchEventProcessor::ActorObserver::ActorObserver()
: mActor ( NULL ),
  mActorDisconnected(false)
{
  DALI_LOG_TRACE_METHOD( gLogFilter );
}

TouchEventProcessor::ActorObserver::~ActorObserver()
{
  DALI_LOG_TRACE_METHOD( gLogFilter );
  SetActor( NULL );
}

Actor* TouchEventProcessor::ActorObserver::GetActor()
{
  return mActorDisconnected ? NULL : mActor;
}

void TouchEventProcessor::ActorObserver::SetActor( Actor* actor )
{
  DALI_LOG_TRACE_METHOD( gLogFilter );

  if ( mActor != actor )
  {
    ResetActor();

    mActor = actor;

    if ( mActor )
    {
      mActor->AddObserver( *this );
      DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Start Observing:            %p\n", mActor);
    }
  }

  // Make sure this flag is unset (as we may have been disconnected if it's the same actor)
  mActorDisconnected = false;
}

void TouchEventProcessor::ActorObserver::ResetActor()
{
  if ( mActor )
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Stop Observing:             %p\n", mActor);
    mActor->RemoveObserver( *this );
    mActor = NULL;
    mActorDisconnected = false;
  }
}

void TouchEventProcessor::ActorObserver::SceneObjectRemoved( ProxyObject& proxy )
{
  DALI_LOG_TRACE_METHOD( gLogFilter );

  if ( mActor == &proxy )
  {
    // do not call proxy.RemoveObserver here, proxy is currently iterating through observers... you wouldnt want to upset proxy now would you?
    mActorDisconnected = true;
  }
}

void TouchEventProcessor::ActorObserver::ProxyDestroyed(ProxyObject& proxy)
{
  DALI_LOG_TRACE_METHOD( gLogFilter );

  if ( mActor == &proxy )
  {
    DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Stop Observing:             %p\n", mActor);
    mActor = NULL;
  }
}

} // namespace Internal

} // namespace Dali
