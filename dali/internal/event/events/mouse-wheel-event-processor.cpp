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
#include <dali/internal/event/events/mouse-wheel-event-processor.h>

// INTERNAL INCLUDES
#include <dali/public-api/events/mouse-wheel-event.h>
#include <dali/devel-api/events/hit-test-algorithm.h>
#include <dali/public-api/math/vector2.h>
#include <dali/integration-api/events/mouse-wheel-event-integ.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/actors/actor-impl.h>

namespace Dali
{

namespace Internal
{

namespace
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_MOUSE_WHEEL_PROCESSOR");
#endif

/**
 *  Recursively deliver events to the actor and its parents, until the event is consumed or the stage is reached.
 */
Dali::Actor EmitMouseWheelSignals( Dali::Actor actor, const MouseWheelEvent& event )
{
  Dali::Actor consumedActor;

  if ( actor )
  {
    Dali::Actor oldParent( actor.GetParent() );

    Actor& actorImpl( GetImplementation(actor) );

    bool consumed( false );

    // Only do the conversion and emit the signal if the actor's mouse wheel signal has connections.
    if ( actorImpl.GetMouseWheelEventRequired() )
    {
      // Emit the signal to the parent
      consumed = actorImpl.EmitMouseWheelEventSignal( event );
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
        consumedActor = EmitMouseWheelSignals( parent, event );
      }
    }
  }

  return consumedActor;
}

/**
 * The function to be used in the hit-test algorithm to check whether the actor is mouse wheelable.
 */
bool IsActorMouseWheelableFunction(Dali::Actor actor, Dali::HitTestAlgorithm::TraverseType type)
{
  bool hittable = false;

  switch (type)
  {
    case Dali::HitTestAlgorithm::CHECK_ACTOR:
    {
      if( GetImplementation(actor).GetMouseWheelEventRequired() && // Does the Application or derived actor type require a mouse wheel event?
          GetImplementation(actor).IsHittable() )
      {
        hittable = true;
      }
      break;
    }
    case Dali::HitTestAlgorithm::DESCEND_ACTOR_TREE:
    {
      if( actor.IsVisible() ) // Actor is visible, if not visible then none of its children are visible.
      {
        hittable = true;
      }
      break;
    }
    default:
    {
      break;
    }
  }

  return hittable;
};

} // unnamed namespace


MouseWheelEventProcessor::MouseWheelEventProcessor(Stage& stage)
: mStage(stage)
{
}

MouseWheelEventProcessor::~MouseWheelEventProcessor()
{
}

void MouseWheelEventProcessor::ProcessMouseWheelEvent(const Integration::MouseWheelEvent& event)
{
  Stage& stage = mStage;
  MouseWheelEvent mouseWheelEvent( event.direction, event.modifiers, event.point, event.z, event.timeStamp );

  HitTestAlgorithm::Results hitTestResults;
  HitTestAlgorithm::HitTest( Dali::Stage(&stage), event.point, hitTestResults, IsActorMouseWheelableFunction );

  DALI_LOG_INFO( gLogFilter, Debug::General, "  Screen(%.0f, %.0f), HitActor(%p, %s), Local(%.2f, %.2f)\n",
                 event.point.x, event.point.y,
                 ( hitTestResults.actor ? (void*)&hitTestResults.actor.GetBaseObject() : NULL ),
                 ( hitTestResults.actor ? hitTestResults.actor.GetName().c_str() : "" ),
                 hitTestResults.actorCoordinates.x, hitTestResults.actorCoordinates.y );

  // Recursively deliver events to the actor and its parents, until the event is consumed or the stage is reached.
  Dali::Actor consumedActor = EmitMouseWheelSignals( hitTestResults.actor, mouseWheelEvent );

  DALI_LOG_INFO( gLogFilter, Debug::Concise, "HitActor:      (%p) %s\n", hitTestResults.actor ? (void*)&hitTestResults.actor.GetBaseObject() : NULL, hitTestResults.actor ? hitTestResults.actor.GetName().c_str() : "" );
  DALI_LOG_INFO( gLogFilter, Debug::Concise, "ConsumedActor: (%p) %s\n", consumedActor ? (void*)&consumedActor.GetBaseObject() : NULL, consumedActor ? consumedActor.GetName().c_str() : "" );

}

} // namespace Internal

} // namespace Dali
