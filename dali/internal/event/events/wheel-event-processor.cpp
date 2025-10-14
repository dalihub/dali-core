/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/events/wheel-event-processor.h>

// INTERNAL INCLUDES
#include <dali/public-api/events/wheel-event.h>

#include <dali/integration-api/debug.h>
#include <dali/integration-api/events/wheel-event-integ.h>
#include <dali/integration-api/trace.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/events/hit-test-algorithm-impl.h>
#include <dali/internal/event/events/wheel-event-impl.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{
namespace Internal
{
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_PERFORMANCE_MARKER, false);
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_WHEEL_PROCESSOR");
#endif

Dali::Actor EmitInterceptWheelSignals(Dali::Actor actor, const Dali::WheelEvent& wheelEvent)
{
  Dali::Actor interceptedActor;

  if(actor)
  {
    Dali::Actor parent = actor.GetParent();
    if(parent)
    {
      // Recursively deliver events to the actor and its parents for intercept wheel event.
      interceptedActor = EmitInterceptWheelSignals(parent, wheelEvent);
    }

    if(!interceptedActor)
    {
      bool   intercepted = false;
      Actor& actorImpl(GetImplementation(actor));
      if(actorImpl.GetInterceptWheelRequired())
      {
        DALI_TRACE_SCOPE(gTraceFilter, "DALI_EMIT_INTERCEPT_WHEEL_EVENT_SIGNAL");
        intercepted = actorImpl.EmitInterceptWheelEventSignal(wheelEvent);
        if(intercepted)
        {
          interceptedActor = Dali::Actor(&actorImpl);
        }
      }
    }
  }

  return interceptedActor;
}

/**
 *  Recursively deliver events to the actor and its parents, until the event is consumed or the stage is reached.
 */
Dali::Actor EmitWheelSignals(Dali::Actor actor, const Dali::WheelEvent& event)
{
  Dali::Actor consumedActor;

  if(actor)
  {
    Dali::Actor oldParent(actor.GetParent());

    Actor& actorImpl(GetImplementation(actor));

    bool consumed(false);

    // Only do the conversion and emit the signal if the actor's wheel signal has connections.
    if(actorImpl.GetWheelEventRequired())
    {
      // Emit the signal to the parent
      DALI_TRACE_SCOPE(gTraceFilter, "DALI_EMIT_WHEEL_EVENT_SIGNAL");
      consumed = actorImpl.EmitWheelEventSignal(event);
    }

    if(consumed)
    {
      // One of this actor's listeners has consumed the event so set this actor as the consumed actor.
      consumedActor = Dali::Actor(&actorImpl);
    }
    else
    {
      // The actor may have been removed/reparented during the signal callbacks.
      Dali::Actor parent = actor.GetParent();

      if(parent &&
         (parent == oldParent))
      {
        // One of the actor's parents may consumed the event and they should be set as the consumed actor.
        consumedActor = EmitWheelSignals(parent, event);
      }
    }
  }

  return consumedActor;
}

/**
 * The function to be used in the hit-test algorithm to check whether the actor is wheelable.
 */
bool IsActorWheelableFunction(Dali::Actor actor, Dali::HitTestAlgorithm::TraverseType type)
{
  bool hittable = false;

  switch(type)
  {
    case Dali::HitTestAlgorithm::CHECK_ACTOR:
    {
      if((GetImplementation(actor).GetWheelEventRequired() || GetImplementation(actor).GetInterceptWheelRequired()) && // Does the Application or derived actor type require a wheel event?
         GetImplementation(actor).IsHittable())
      {
        hittable = true;
      }
      break;
    }
    case Dali::HitTestAlgorithm::DESCEND_ACTOR_TREE:
    {
      if(actor.GetProperty<bool>(Dali::Actor::Property::VISIBLE) &&       // Actor is visible, if not visible then none of its children are visible.
         actor.GetProperty<bool>(Dali::Actor::Property::SENSITIVE) &&     // Actor is sensitive, if insensitive none of its children should be hittable either.
         (!actor.GetProperty<bool>(Dali::DevelActor::Property::IGNORED))) // Actor is not ignored, if ignored none of its children should be hittable either.
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

WheelEventProcessor::WheelEventProcessor(Scene& scene)
: mScene(scene)
{
}

WheelEventProcessor::~WheelEventProcessor() = default;

void WheelEventProcessor::ProcessWheelEvent(const Integration::WheelEvent& event)
{
  WheelEventPtr    wheelEvent = WheelEvent::New(static_cast<Dali::WheelEvent::Type>(event.type), event.direction, event.modifiers, event.point, event.delta, event.timeStamp);
  Dali::WheelEvent wheelEventHandle(wheelEvent.Get());

  DALI_TRACE_SCOPE(gTraceFilter, "DALI_PROCESS_WHEEL_EVENT");

  if(wheelEvent->GetType() == Dali::WheelEvent::MOUSE_WHEEL)
  {
    Dali::HitTestAlgorithm::Results hitTestResults;
    HitTestAlgorithm::HitTest(mScene.GetSize(), mScene.GetRenderTaskList(), mScene.GetLayerList(), event.point, hitTestResults, IsActorWheelableFunction);

    DALI_LOG_INFO(gLogFilter, Debug::General, "  Screen(%.0f, %.0f), HitActor(%p, %s), Local(%.2f, %.2f)\n", event.point.x, event.point.y, (hitTestResults.actor ? reinterpret_cast<void*>(&hitTestResults.actor.GetBaseObject()) : NULL), (hitTestResults.actor ? hitTestResults.actor.GetProperty<std::string>(Dali::Actor::Property::NAME).c_str() : ""), hitTestResults.actorCoordinates.x, hitTestResults.actorCoordinates.y);

    // Recursively deliver events to the actor and its parents, until the event is consumed or the stage is reached.
    Dali::Actor consumedActor;

    // Emit the intercept wheel event signal
    Dali::Actor interceptedActor = EmitInterceptWheelSignals(hitTestResults.actor, wheelEventHandle);
    if(interceptedActor)
    {
      consumedActor = EmitWheelSignals(interceptedActor, wheelEventHandle);
    }
    else
    {
      consumedActor = EmitWheelSignals(hitTestResults.actor, wheelEventHandle);
    }

    DALI_LOG_RELEASE_INFO("HitActor:      (%p) id(%d) %s\n", hitTestResults.actor ? reinterpret_cast<void*>(&hitTestResults.actor.GetBaseObject()) : NULL, hitTestResults.actor ? hitTestResults.actor.GetProperty<int32_t>(Dali::Actor::Property::ID) : -1, hitTestResults.actor ? hitTestResults.actor.GetProperty<std::string>(Dali::Actor::Property::NAME).c_str() : "");
    DALI_LOG_RELEASE_INFO("ConsumedActor: (%p) id(%d) %s\n", consumedActor ? reinterpret_cast<void*>(&consumedActor.GetBaseObject()) : NULL, consumedActor ? consumedActor.GetProperty<int32_t>(Dali::Actor::Property::ID) : -1, consumedActor ? consumedActor.GetProperty<std::string>(Dali::Actor::Property::NAME).c_str() : "");
  }
  else
  {
    // if CUSTOM_WHEEL, emit the wheel event signal from the scene.
    bool consumed = mScene.EmitWheelEventGeneratedSignal(wheelEventHandle);
    if(!consumed)
    {
      mScene.EmitWheelEventSignal(wheelEventHandle);
    }
  }
}

} // namespace Internal

} // namespace Dali
