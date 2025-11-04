/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <chrono>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/events/hover-event-integ.h>
#include <dali/integration-api/trace.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/events/hit-test-algorithm-impl.h>
#include <dali/internal/event/events/hover-event-impl.h>
#include <dali/internal/event/events/multi-point-event-util.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/public-api/math/vector2.h>

namespace Dali::Internal
{
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_PERFORMANCE_MARKER, false);
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_HOVER_PROCESSOR");
#endif // defined(DEBUG_ENABLED)

/**
 * Structure for Variables used in the ProcessHoverEvent method.
 */
struct ProcessHoverEventVariables
{
  ProcessHoverEventVariables(bool geometry)
  : isGeometry(geometry)
  {
  }

  const bool          isGeometry;                             ///< Whether it's a geometry or not.
  Actor*              lastPrimaryHitActor{nullptr};           ///< The last primary hit-actor.
  Actor*              lastConsumedActor{nullptr};             ///< The last consuming actor.
  HoverEventPtr       hoverEvent;                             ///< The current hover-event-impl.
  Dali::HoverEvent    hoverEventHandle;                       ///< The handle to the hover-event-impl.
  RenderTaskPtr       currentRenderTask;                      ///< The current render-task.
  Dali::Actor         consumedActor;                          ///< The actor that consumed the event.
  Dali::Actor         primaryHitActor;                        ///< The actor that has been hit by the primary point.
  Integration::Point* primaryPoint{nullptr};                  ///< The primary point of the hit.
  PointState::Type    primaryPointState{PointState::STARTED}; ///< The state of the primary point.
};

const char* TOUCH_POINT_STATE[PointState::INTERRUPTED + 1] =
  {
    "STARTED",
    "FINISHED",
    "MOTION",
    "LEAVE",
    "STATIONARY",
    "INTERRUPTED",
};

bool ShouldEmitHoverEvent(const Actor& actorImpl, const Dali::HoverEvent& event)
{
  PointState::Type state = event.GetState(0);
  return actorImpl.GetHoverRequired() && (state != PointState::MOTION || actorImpl.IsDispatchHoverMotion());
}

/**
 *  Recursively deliver events to the actor and its parents, until the event is consumed or the stage is reached.
 */
Dali::Actor EmitHoverSignals(Dali::Actor actor, const Dali::HoverEvent& event)
{
  Dali::Actor consumedActor;

  if(actor)
  {
    Dali::Actor oldParent(actor.GetParent());

    Actor& actorImpl(GetImplementation(actor));

    bool consumed(false);

    // Only emit the signal if the actor's hover signal has connections (or derived actor implementation requires hover).
    if(ShouldEmitHoverEvent(actorImpl, event))
    {
      DALI_TRACE_SCOPE(gTraceFilter, "DALI_EMIT_HOVER_EVENT_SIGNAL");
      consumed = actorImpl.EmitHoverEventSignal(event);
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
        consumedActor = EmitHoverSignals(parent, event);
      }
    }
  }

  return consumedActor;
}

/**
 *  Recursively deliver events to the actor and its below actor, until the event is consumed or the stage is reached.
 */
Dali::Actor EmitGeoHoverSignals(std::list<Dali::Internal::Actor*>& actorLists, const Dali::HoverEvent& hoverEvent)
{
  Dali::Actor consumedActor;

  std::list<Dali::Internal::Actor*>::reverse_iterator rIter = actorLists.rbegin();
  for(; rIter != actorLists.rend(); rIter++)
  {
    Actor* actorImpl(*rIter);
    // Only emit the signal if the actor's hover signal has connections (or derived actor implementation requires hover).
    if(actorImpl->GetHoverRequired())
    {
      DALI_TRACE_SCOPE(gTraceFilter, "DALI_EMIT_HOVER_EVENT_SIGNAL");
      PointState::Type currentState = actorImpl->GetHoverState();
      actorImpl->SetHoverState(hoverEvent.GetState(0));
      // If hover event is newly entering the actor, update it to the started state.
      if(hoverEvent.GetState(0) == PointState::MOTION &&
         (currentState == PointState::FINISHED || currentState == PointState::INTERRUPTED || currentState == PointState::LEAVE))
      {
        HoverEventPtr newHoverEvent = HoverEvent::Clone(GetImplementation(hoverEvent));
        newHoverEvent->GetPoint(0).SetState(PointState::STARTED);
        actorImpl->SetHoverState(PointState::STARTED); //update state
        if(actorImpl->EmitHoverEventSignal(Dali::HoverEvent(newHoverEvent.Get())))
        {
          // One of this actor's listeners has consumed the event so set this actor as the consumed actor.
          consumedActor = Dali::Actor(actorImpl);
          break;
        }
      }
      else if(hoverEvent.GetState(0) != PointState::MOTION || actorImpl->IsDispatchHoverMotion())
      {
        if(actorImpl->EmitHoverEventSignal(hoverEvent))
        {
          // One of this actor's listeners has consumed the event so set this actor as the consumed actor.
          consumedActor = Dali::Actor(actorImpl);
          break;
        }
      }
    }
  }
  return consumedActor;
}

Dali::Actor AllocAndEmitHoverSignals(unsigned long time, Dali::Actor actor, const Integration::Point& point)
{
  HoverEventPtr    hoverEvent(new HoverEvent(time));
  Dali::HoverEvent hoverEventHandle(hoverEvent.Get());

  hoverEvent->AddPoint(point);

  return EmitHoverSignals(actor, hoverEventHandle);
}

Dali::Actor GeoAllocAndEmitHoverSignals(std::list<Dali::Internal::Actor*>& actorLists, unsigned long time, const Integration::Point& point)
{
  HoverEventPtr    hoverEvent(new HoverEvent(time));
  Dali::HoverEvent hoverEventHandle(hoverEvent.Get());

  hoverEvent->AddPoint(point);

  return EmitGeoHoverSignals(actorLists, hoverEventHandle);
}

/**
 * Changes the state of the primary point to leave and emits the hover signals
 */
Dali::Actor EmitHoverSignals(Actor* actor, RenderTask& renderTask, const HoverEventPtr& originalEvent, PointState::Type state, bool isGeometry)
{
  HoverEventPtr hoverEvent = HoverEvent::Clone(*originalEvent.Get());

  DALI_ASSERT_DEBUG(NULL != actor && "NULL actor pointer");
  Dali::Actor consumingActor;
  if(actor)
  {
    Integration::Point& primaryPoint = hoverEvent->GetPoint(0);

    const Vector2& screenPosition = primaryPoint.GetScreenPosition();
    Vector2        localPosition;
    actor->ScreenToLocal(renderTask, localPosition.x, localPosition.y, screenPosition.x, screenPosition.y);

    primaryPoint.SetLocalPosition(localPosition);
    primaryPoint.SetHitActor(Dali::Actor(actor));
    primaryPoint.SetState(state);
  }

  if(isGeometry)
  {
    std::list<Dali::Internal::Actor*> actorLists;
    actorLists.push_back(actor);
    consumingActor = EmitGeoHoverSignals(actorLists, Dali::HoverEvent(hoverEvent.Get()));
  }
  else
  {
    consumingActor = EmitHoverSignals(Dali::Actor(actor), Dali::HoverEvent(hoverEvent.Get()));
  }
  return consumingActor;
}

/**
 * Clear hover start consumed actor if it matches the given actor
 */
void ClearHoverStartConsumedActorIfNeeded(ActorObserver& hoverStartConsumedActor, Actor* actor)
{
  if(actor)
  {
    Actor* consumedActor = hoverStartConsumedActor.GetActor();
    if(consumedActor && actor == consumedActor)
    {
      hoverStartConsumedActor.SetActor(nullptr);
    }
  }
}

/**
 * Used in the hit-test algorithm to check whether the actor is hoverable.
 */
struct ActorHoverableCheck : public HitTestAlgorithm::HitTestInterface
{
  bool IsActorHittable(Actor* actor) override
  {
    return actor->GetHoverRequired() && // Does the Application or derived actor type require a hover event?
           actor->IsHittable();         // Is actor sensitive, visible and on the scene?
  }

  bool DescendActorHierarchy(Actor* actor) override
  {
    return (!actor->IsIgnored()) &&
           actor->IsVisible() && // Actor is visible, if not visible then none of its children are visible.
           actor->IsSensitive(); // Actor is sensitive, if insensitive none of its children should be hittable either.
  }

  bool DoesLayerConsumeHit(Layer* layer) override
  {
    return layer->IsHoverConsumed();
  }

  bool ActorRequiresHitResultCheck(Actor* actor, Vector2 hitPointLocal) override
  {
    // Hover event is always hit.
    return true;
  }
};

uint32_t GetMilliSeconds()
{
  // Get the time of a monotonic clock since its epoch.
  auto epoch = std::chrono::steady_clock::now().time_since_epoch();

  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);

  return static_cast<uint32_t>(duration.count());
}

} // unnamed namespace

struct HoverEventProcessor::Impl
{
  /**
   * Emits an interrupted event while processing the latest hover event.
   * @param[in/out]  processor  The hover-event-processor
   * @param[in]  isGeometry  Whether it's a geometry or not
   * @param[in]  event  The hover event that has occurred
   */
  static inline void EmitInterruptedEvent(HoverEventProcessor& processor, const bool isGeometry, const Integration::HoverEvent& event)
  {
    Dali::Actor        consumingActor;
    Integration::Point currentPoint(event.points[0]);

    Actor* lastPrimaryHitActor(processor.mLastPrimaryHitActor.GetActor());
    if(lastPrimaryHitActor)
    {
      Dali::Actor lastPrimaryHitActorHandle(lastPrimaryHitActor);
      currentPoint.SetHitActor(lastPrimaryHitActorHandle);
      if(isGeometry)
      {
        consumingActor = GeoAllocAndEmitHoverSignals(processor.mCandidateActorLists, event.time, currentPoint);
      }
      else
      {
        consumingActor = AllocAndEmitHoverSignals(event.time, lastPrimaryHitActorHandle, currentPoint);
      }
    }

    // If the last consumed actor was different to the primary hit actor then inform it as well (if it has not already been informed).
    Actor* lastConsumedActor(processor.mLastConsumedActor.GetActor());
    if(lastConsumedActor &&
       lastConsumedActor != lastPrimaryHitActor &&
       lastConsumedActor != consumingActor)
    {
      Dali::Actor lastConsumedActorHandle(lastConsumedActor);
      currentPoint.SetHitActor(lastConsumedActorHandle);
      if(isGeometry)
      {
        std::list<Dali::Internal::Actor*> actorLists;
        actorLists.push_back(lastConsumedActor);
        GeoAllocAndEmitHoverSignals(actorLists, event.time, currentPoint);
      }
      else
      {
        AllocAndEmitHoverSignals(event.time, lastConsumedActorHandle, currentPoint);
      }
    }

    // Tell the hover-start consuming actor as well, if required
    Actor* hoverStartConsumedActor(processor.mHoverStartConsumedActor.GetActor());
    if(hoverStartConsumedActor &&
       hoverStartConsumedActor != lastPrimaryHitActor &&
       hoverStartConsumedActor != lastConsumedActor &&
       hoverStartConsumedActor != consumingActor)
    {
      Dali::Actor hoverStartConsumedActorHandle(hoverStartConsumedActor);
      currentPoint.SetHitActor(hoverStartConsumedActorHandle);
      if(isGeometry)
      {
        std::list<Dali::Internal::Actor*> actorLists;
        actorLists.push_back(hoverStartConsumedActor);
        GeoAllocAndEmitHoverSignals(actorLists, event.time, currentPoint);
      }
      else
      {
        AllocAndEmitHoverSignals(event.time, hoverStartConsumedActorHandle, currentPoint);
      }
    }

    processor.Clear();
    processor.mHoverStartConsumedActor.SetActor(nullptr);
  }

  /**
   * Performs a hit-test and sets the variables in processor and localVars appropriately.
   * @param[in/out]  processor  The hover-event-processor
   * @param[in/out]  localVars  The struct of stack variables used by ProcessHoverEvent
   * @param[in]  event  The hover event that has occurred
   */
  static inline void HitTest(HoverEventProcessor& processor, ProcessHoverEventVariables& localVars, const Integration::HoverEvent& event)
  {
    bool firstPointParsed = false;
    for(auto&& currentPoint : event.points)
    {
      HitTestAlgorithm::Results hitTestResults;
      hitTestResults.eventTime = event.time;
      ActorHoverableCheck actorHoverableCheck;
      HitTestAlgorithm::HitTest(processor.mScene.GetSize(), processor.mScene.GetRenderTaskList(), processor.mScene.GetLayerList(), currentPoint.GetScreenPosition(), hitTestResults, actorHoverableCheck, localVars.isGeometry ? Integration::Scene::TouchPropagationType::GEOMETRY : Integration::Scene::TouchPropagationType::PARENT);

      Integration::Point newPoint(currentPoint);
      newPoint.SetHitActor(hitTestResults.actor);
      newPoint.SetLocalPosition(hitTestResults.actorCoordinates);

      localVars.hoverEvent->AddPoint(newPoint);

      DALI_LOG_INFO(gLogFilter,
                    Debug::General,
                    "  State(%s), Screen(%.0f, %.0f), HitActor(%p, %s), Local(%.2f, %.2f)\n",
                    TOUCH_POINT_STATE[currentPoint.GetState()],
                    currentPoint.GetScreenPosition().x,
                    currentPoint.GetScreenPosition().y,
                    (hitTestResults.actor ? reinterpret_cast<void*>(&hitTestResults.actor.GetBaseObject()) : NULL),
                    (hitTestResults.actor ? hitTestResults.actor.GetProperty<std::string>(Dali::Actor::Property::NAME).c_str() : ""),
                    hitTestResults.actorCoordinates.x,
                    hitTestResults.actorCoordinates.y);

      // Only set the currentRenderTask for the primary hit actor.
      if(!firstPointParsed)
      {
        firstPointParsed               = true;
        localVars.currentRenderTask    = hitTestResults.renderTask;
        processor.mCandidateActorLists = hitTestResults.actorLists;
      }
    }
  }

  /**
   * Recursively deliver events to the actor and its parents, until the event is consumed or the stage is reached.
   * @param[in/out]  processor  The hover-event-processor
   * @param[in/out]  localVars  The struct of stack variables used by ProcessHoverEvent
   */
  static inline void DeliverEventsToActorAndParents(HoverEventProcessor& processor, ProcessHoverEventVariables& localVars)
  {
    // Emit the touch signal
    if(localVars.currentRenderTask)
    {
      Dali::Actor hitActor = localVars.hoverEvent->GetHitActor(0);

      if(localVars.isGeometry)
      {
        localVars.consumedActor = EmitGeoHoverSignals(processor.mCandidateActorLists, localVars.hoverEventHandle);
      }
      else
      {
        // If the actor is hit first, the hover is started.
        if(hitActor && processor.mLastPrimaryHitActor.GetActor() != hitActor &&
           localVars.primaryPointState == PointState::MOTION && GetImplementation(hitActor).GetLeaveRequired())
        {
          // A leave event is sent to the previous actor first.
          localVars.lastPrimaryHitActor = processor.mLastPrimaryHitActor.GetActor();
          localVars.lastConsumedActor   = processor.mLastConsumedActor.GetActor();
          Impl::DeliverLeaveEvent(processor, localVars);

          localVars.hoverEvent->GetPoint(0).SetState(PointState::STARTED);
          localVars.primaryPointState = PointState::STARTED;

          // It sends a started event and updates information.
          localVars.consumedActor = EmitHoverSignals(hitActor, localVars.hoverEventHandle);
          UpdateMembersWithCurrentHitInformation(processor, localVars);
        }
        else
        {
          localVars.consumedActor = EmitHoverSignals(hitActor, localVars.hoverEventHandle);
          // If IsDispatchHoverMotion is false, the move event was not dispatched. So consumedActor should keep the previous LastConsumedActor value.
          if(!localVars.consumedActor && localVars.primaryPointState == PointState::MOTION && !GetImplementation(hitActor).IsDispatchHoverMotion())
          {
            localVars.consumedActor = Dali::Actor(processor.mLastConsumedActor.GetActor());
          }
        }
      }

      if(localVars.hoverEvent->GetPoint(0).GetState() != PointState::MOTION)
      {
        DALI_LOG_RELEASE_INFO("PrimaryHitActor:(%p), id(%d), name(%s), state(%s)\n",
                              localVars.primaryHitActor ? reinterpret_cast<void*>(&localVars.primaryHitActor.GetBaseObject()) : NULL,
                              localVars.primaryHitActor ? localVars.primaryHitActor.GetProperty<int32_t>(Dali::Actor::Property::ID) : -1,
                              localVars.primaryHitActor ? localVars.primaryHitActor.GetProperty<std::string>(Dali::Actor::Property::NAME).c_str() : "",
                              TOUCH_POINT_STATE[localVars.hoverEvent->GetPoint(0).GetState()]);
        DALI_LOG_RELEASE_INFO("ConsumedActor:  (%p), id(%d), name(%s), state(%s)\n",
                              localVars.consumedActor ? reinterpret_cast<void*>(&localVars.consumedActor.GetBaseObject()) : NULL,
                              localVars.consumedActor ? localVars.consumedActor.GetProperty<int32_t>(Dali::Actor::Property::ID) : -1,
                              localVars.consumedActor ? localVars.consumedActor.GetProperty<std::string>(Dali::Actor::Property::NAME).c_str() : "",
                              TOUCH_POINT_STATE[localVars.hoverEvent->GetPoint(0).GetState()]);
      }
    }

    if((localVars.primaryPointState == PointState::STARTED) &&
       (localVars.hoverEvent->GetPointCount() == 1) &&
       (localVars.consumedActor && GetImplementation(localVars.consumedActor).OnScene()))
    {
      processor.mHoverStartConsumedActor.SetActor(&GetImplementation(localVars.consumedActor));
    }
  }

  /**
   * Deliver Leave event to last hit or consuming actor if required.
   * @param[in/out]  processor  The hover-event-processor
   * @param[in/out]  localVars  The struct of stack variables used by ProcessHoverEvent
   */
  static inline void DeliverLeaveEvent(HoverEventProcessor& processor, ProcessHoverEventVariables& localVars)
  {
    if((localVars.primaryPointState == PointState::STARTED) ||
       (localVars.primaryPointState == PointState::MOTION) ||
       (localVars.primaryPointState == PointState::FINISHED) ||
       (localVars.primaryPointState == PointState::STATIONARY))
    {
      if(processor.mLastRenderTask)
      {
        Dali::Actor leaveEventConsumer;
        RenderTask& lastRenderTaskImpl = *processor.mLastRenderTask.Get();

        if(localVars.lastPrimaryHitActor &&
           localVars.lastPrimaryHitActor != localVars.primaryHitActor &&
           localVars.lastPrimaryHitActor != localVars.consumedActor)
        {
          if(localVars.lastPrimaryHitActor->IsHittable() && IsActuallySensitive(localVars.lastPrimaryHitActor))
          {
            if(localVars.isGeometry)
            {
              // This is a situation where actors who received a hover event must leave.
              // Compare the lastActorList that received the hover event and the CandidateActorList that can receive the new hover event
              // If the hover event can no longer be received, Leave is sent.
              std::list<Dali::Internal::Actor*>::reverse_iterator rLastIter = processor.mLastActorLists.rbegin();
              for(; rLastIter != processor.mLastActorLists.rend(); rLastIter++)
              {
                bool foundInCandidates = std::any_of(
                  processor.mCandidateActorLists.rbegin(),
                  processor.mCandidateActorLists.rend(),
                  [rLastIter](Actor* candidate) { return candidate == *rLastIter; }
                );

                if(!foundInCandidates)
                {
                  DALI_LOG_RELEASE_INFO("LeaveActor(Hit): (%p) %d %s\n", reinterpret_cast<void*>(*rLastIter), (*rLastIter)->GetId(), (*rLastIter)->GetName().data());
                  leaveEventConsumer = EmitHoverSignals(*rLastIter, lastRenderTaskImpl, localVars.hoverEvent, PointState::LEAVE, localVars.isGeometry);
                  ClearHoverStartConsumedActorIfNeeded(processor.mHoverStartConsumedActor, *rLastIter);
                }
                // If the actor has been consumed, you do not need to proceed.
                if(*rLastIter == localVars.lastConsumedActor)
                {
                  break;
                }
              }
            }
            else if(localVars.lastPrimaryHitActor->GetLeaveRequired())
            {
              // In the case of isGeometry, it is not propagated but only sent to actors who are not hittable.
              DALI_LOG_RELEASE_INFO("LeaveActor(Hit): (%p) %d %s\n", reinterpret_cast<void*>(localVars.lastPrimaryHitActor), localVars.lastPrimaryHitActor->GetId(), localVars.lastPrimaryHitActor->GetName().data());
              leaveEventConsumer = EmitHoverSignals(processor.mLastPrimaryHitActor.GetActor(), lastRenderTaskImpl, localVars.hoverEvent, PointState::LEAVE, localVars.isGeometry);
              ClearHoverStartConsumedActorIfNeeded(processor.mHoverStartConsumedActor, processor.mLastPrimaryHitActor.GetActor());
            }
          }
          else if(localVars.primaryPointState != PointState::STARTED)
          {
            // At this point mLastPrimaryHitActor was touchable and sensitive in the previous touch event process but is not in the current one.
            // An interrupted event is sent to allow some actors to go back to their original state (i.e. Button controls)
            DALI_LOG_RELEASE_INFO("InterruptedActor(Hit): (%p) %d %s\n", reinterpret_cast<void*>(localVars.lastPrimaryHitActor), localVars.lastPrimaryHitActor->GetId(), localVars.lastPrimaryHitActor->GetName().data());
            leaveEventConsumer = EmitHoverSignals(processor.mLastPrimaryHitActor.GetActor(), lastRenderTaskImpl, localVars.hoverEvent, PointState::INTERRUPTED, localVars.isGeometry);
            ClearHoverStartConsumedActorIfNeeded(processor.mHoverStartConsumedActor, processor.mLastPrimaryHitActor.GetActor());
          }
        }

        // Check if the motion event has been consumed by another actor's listener.  In this case, the previously
        // consumed actor's listeners may need to be informed (through a leave event).
        // Further checks here to ensure we do not signal the same actor twice for the same event.
        if(localVars.lastConsumedActor &&
           localVars.lastConsumedActor != localVars.consumedActor &&
           localVars.lastConsumedActor != localVars.lastPrimaryHitActor &&
           localVars.lastConsumedActor != localVars.primaryHitActor &&
           localVars.lastConsumedActor != leaveEventConsumer)
        {
          if(localVars.lastConsumedActor->IsHittable() && IsActuallySensitive(localVars.lastConsumedActor))
          {
            if(localVars.lastConsumedActor->GetLeaveRequired() && !localVars.isGeometry) // For geometry, we have already sent leave. There is no need to send leave repeatedly.
            {
              DALI_LOG_RELEASE_INFO("LeaveActor(Consume): (%p) %d %s\n", reinterpret_cast<void*>(localVars.lastConsumedActor), localVars.lastConsumedActor->GetId(), localVars.lastConsumedActor->GetName().data());
              EmitHoverSignals(localVars.lastConsumedActor, lastRenderTaskImpl, localVars.hoverEvent, PointState::LEAVE, localVars.isGeometry);
              ClearHoverStartConsumedActorIfNeeded(processor.mHoverStartConsumedActor, localVars.lastConsumedActor);
            }
          }
          else if(localVars.primaryPointState != PointState::STARTED)
          {
            // At this point mLastConsumedActor was touchable and sensitive in the previous touch event process but is not in the current one.
            // An interrupted event is send to allow some actors to go back to their original state (i.e. Button controls)
            DALI_LOG_RELEASE_INFO("InterruptedActor(Consume): (%p) %d %s\n", reinterpret_cast<void*>(localVars.lastConsumedActor), localVars.lastConsumedActor->GetId(), localVars.lastConsumedActor->GetName().data());
            EmitHoverSignals(processor.mLastConsumedActor.GetActor(), lastRenderTaskImpl, localVars.hoverEvent, PointState::INTERRUPTED, localVars.isGeometry);
            ClearHoverStartConsumedActorIfNeeded(processor.mHoverStartConsumedActor, processor.mLastConsumedActor.GetActor());
          }
        }
      }
    }
  }

  /**
   * Update the processor member appropriately by handling the final up event, and setting the last hit/consumed events etc.
   * @param[in/out]  processor  The hover-event-processor
   * @param[in/out] localVars The struct of stack variables used by ProcessHoverEvent
   */
  static inline void UpdateMembersWithCurrentHitInformation(HoverEventProcessor& processor, ProcessHoverEventVariables& localVars)
  {
    // If our primary point is a FINISHED event, then the primary point (in multi-touch) will change next
    // time so set our last primary actor to NULL.  Do the same to the last consumed actor as well.

    if(localVars.primaryPointState == PointState::FINISHED)
    {
      processor.Clear();
    }
    else
    {
      // The primaryHitActor may have been removed from the scene so ensure it is still on the scene before setting members.
      if(localVars.primaryHitActor && GetImplementation(localVars.primaryHitActor).OnScene())
      {
        processor.mLastPrimaryHitActor.SetActor(&GetImplementation(localVars.primaryHitActor));
        // Only observe the consumed actor if we have a primaryHitActor (check if it is still on the scene).
        if(localVars.consumedActor && GetImplementation(localVars.consumedActor).OnScene())
        {
          processor.mLastConsumedActor.SetActor(&GetImplementation(localVars.consumedActor));
        }
        else
        {
          processor.mLastConsumedActor.SetActor(nullptr);
        }

        processor.mLastRenderTask = localVars.currentRenderTask;
        processor.mLastActorLists = processor.mCandidateActorLists;
      }
      else
      {
        processor.Clear();
      }
    }
  }

  /**
   * Deliver an interrupted event to the hover started actor as required.
   * @param[in/out]  processor  The hover-event-processor
   * @param[in/out]  localVars  The struct of stack variables used by ProcessHoverEvent
   * @param[in]  event  The hover event that has occurred
   */
  static inline void DeliverInterruptedEventToHoverStartedActor(HoverEventProcessor& processor, ProcessHoverEventVariables& localVars, const Integration::HoverEvent& event)
  {
    if(localVars.hoverEvent->GetPointCount() == 1 && localVars.primaryPointState == PointState::FINISHED) // Only want the first hover started
    {
      Actor* hoverStartConsumedActor(processor.mHoverStartConsumedActor.GetActor());
      if(hoverStartConsumedActor &&
         hoverStartConsumedActor != localVars.consumedActor &&
         hoverStartConsumedActor != localVars.lastPrimaryHitActor &&
         hoverStartConsumedActor != localVars.lastConsumedActor)
      {
        Dali::Actor        hoverStartConsumedActorHandle(hoverStartConsumedActor);
        Integration::Point primaryPoint = localVars.hoverEvent->GetPoint(0);
        primaryPoint.SetHitActor(hoverStartConsumedActorHandle);
        primaryPoint.SetState(PointState::INTERRUPTED);
        if(localVars.isGeometry)
        {
          std::list<Dali::Internal::Actor*> actorLists;
          actorLists.push_back(hoverStartConsumedActor);
          GeoAllocAndEmitHoverSignals(actorLists, event.time, primaryPoint);
        }
        else
        {
          AllocAndEmitHoverSignals(event.time, hoverStartConsumedActorHandle, primaryPoint);
        }

        // Restore hover-event to original state
        primaryPoint.SetHitActor(localVars.primaryHitActor);
        primaryPoint.SetState(localVars.primaryPointState);
      }

      processor.mHoverStartConsumedActor.SetActor(nullptr);
    }
  }
};

HoverEventProcessor::HoverEventProcessor(Scene& scene)
: mScene(scene),
  mLastPrimaryHitActor(MakeCallback(this, &HoverEventProcessor::OnObservedActorDisconnected))
{
  DALI_LOG_TRACE_METHOD(gLogFilter);
}

HoverEventProcessor::~HoverEventProcessor()
{
  DALI_LOG_TRACE_METHOD(gLogFilter);
}

void HoverEventProcessor::SendInterruptedHoverEvent(Dali::Internal::Actor* actor)
{
  if(actor &&
     (mLastPrimaryHitActor.GetActor() == actor || mLastConsumedActor.GetActor() == actor))
  {
    Integration::Point point;
    point.SetState(PointState::INTERRUPTED);
    point.SetHitActor(Dali::Actor(actor));
    if(mScene.GetTouchPropagationType() == Integration::Scene::TouchPropagationType::GEOMETRY)
    {
      std::list<Dali::Internal::Actor*> actorLists;
      actorLists.push_back(actor);
      GeoAllocAndEmitHoverSignals(actorLists, 0, point);
    }
    else
    {
      AllocAndEmitHoverSignals(GetMilliSeconds(), point.GetHitActor(), point);
    }
    Clear();
  }
}

void HoverEventProcessor::ProcessHoverEvent(const Integration::HoverEvent& event)
{
  DALI_LOG_TRACE_METHOD(gLogFilter);
  DALI_ASSERT_ALWAYS(!event.points.empty() && "Empty HoverEvent sent from Integration\n");

  PRINT_HIERARCHY(gLogFilter);

  DALI_TRACE_SCOPE(gTraceFilter, "DALI_PROCESS_HOVER_EVENT");

  ProcessHoverEventVariables localVars(mScene.IsGeometryHittestEnabled());

  // Copy so we can add the results of a hit-test.
  localVars.hoverEvent = new HoverEvent(event.time);

  // 1) Check if it is an interrupted event - we should inform our last primary hit actor about this
  //    and emit the stage signal as well.
  if(event.points[0].GetState() == PointState::INTERRUPTED)
  {
    Impl::EmitInterruptedEvent(*this, localVars.isGeometry, event);
    return; // No need for hit testing
  }

  // 2) Hit Testing.
  DALI_LOG_INFO(gLogFilter, Debug::Concise, "\n");
  DALI_LOG_INFO(gLogFilter, Debug::General, "Point(s): %d\n", event.GetPointCount());
  localVars.hoverEventHandle = Dali::HoverEvent(localVars.hoverEvent.Get());
  Impl::HitTest(*this, localVars, event);

  // 3) Recursively deliver events to the actor and its parents, until the event is consumed or the stage is reached.
  localVars.primaryPoint      = &localVars.hoverEvent->GetPoint(0);
  localVars.primaryHitActor   = localVars.primaryPoint->GetHitActor();
  localVars.primaryPointState = localVars.primaryPoint->GetState();
  Impl::DeliverEventsToActorAndParents(*this, localVars);

  // 4) Check if the last primary hit actor requires a leave event and if it was different to the current primary
  //    hit actor.  Also process the last consumed actor in the same manner.
  localVars.lastPrimaryHitActor = mLastPrimaryHitActor.GetActor();
  localVars.lastConsumedActor   = mLastConsumedActor.GetActor();
  Impl::DeliverLeaveEvent(*this, localVars);

  // 5) Update the processor member appropriately.
  Impl::UpdateMembersWithCurrentHitInformation(*this, localVars);

  // 6) Emit an interrupted event to the hover-started actor if it hasn't consumed the FINISHED.
  Impl::DeliverInterruptedEventToHoverStartedActor(*this, localVars, event);
}

void HoverEventProcessor::Clear()
{
  mLastPrimaryHitActor.SetActor(nullptr);
  mLastConsumedActor.SetActor(nullptr);
  mLastRenderTask.Reset();
  mLastActorLists.clear();
}

void HoverEventProcessor::OnObservedActorDisconnected(Dali::Internal::Actor* actor)
{
  SendInterruptedHoverEvent(actor);
}

} // namespace Dali::Internal
