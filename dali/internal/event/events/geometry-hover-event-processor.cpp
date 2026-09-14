/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/events/geometry-hover-event-processor.h>

#if defined(DEBUG_ENABLED)
#include <sstream>
#endif

// EXTERNAL INCLUDES
#include <algorithm>
#include <vector>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/events/hover-event-integ.h>
#include <dali/integration-api/trace.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/events/hit-test-algorithm-impl.h>
#include <dali/internal/event/events/hover-event-impl.h>
#include <dali/internal/event/events/hover-event-processor-common.h>
#include <dali/internal/event/events/multi-point-event-util.h>

namespace DALI_NAMESPACE::Internal
{
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_PERFORMANCE_MARKER, false);
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_HOVER_PROCESSOR");
#endif // defined(DEBUG_ENABLED)

using HoverEventProcessorCommon::ActorHoverableCheck;
using HoverEventProcessorCommon::GetMilliSeconds;
using HoverEventProcessorCommon::POINT_STATE_NAME;

struct ProcessHoverEventVariables
{
  HoverEventPtr         hoverEvent;
  Dali::HoverEvent      hoverEventHandle;
  std::vector<ActorPtr> candidates;
  PointState::Type      primaryPointState{PointState::STARTED};
};

void LogLifecycle(Actor& actor, const HoverEventPtr& event, PointState::Type state, const char* reason)
{
  Dali::Actor primaryHitActor = event->GetHitActor(0u);
  DALI_LOG_RELEASE_INFO("GeoHoverLifecycle: ReceiverActor:(%p), id(%d), name(%s), PrimaryHitActor:(%p), id(%d), name(%s), state(%s), reason(%s)\n",
                        reinterpret_cast<void*>(&actor),
                        actor.GetId(),
                        actor.GetName().data(),
                        primaryHitActor ? reinterpret_cast<void*>(&primaryHitActor.GetBaseObject()) : nullptr,
                        primaryHitActor ? primaryHitActor.GetProperty<int32_t>(Dali::Actor::Property::ID) : -1,
                        primaryHitActor ? primaryHitActor.GetProperty<Dali::String>(Dali::Actor::Property::NAME).CStr() : "",
                        POINT_STATE_NAME[state],
                        reason);
}

void LogDispatchSummary(const ProcessHoverEventVariables& localVars,
                        Actor*                            consumedActor,
                        size_t                            previousActiveCount,
                        size_t                            activeCount,
                        bool                              consumerChanged)
{
  Dali::Actor primaryHitActor = localVars.hoverEvent->GetHitActor(0u);
  DALI_LOG_RELEASE_INFO("GeoHoverDispatch: PrimaryHitActor:(%p), id(%d), name(%s), state(%s), candidates(%zu), previousActive(%zu), active(%zu), ConsumedActor:(%p), id(%d), name(%s), consumerChanged(%d)\n",
                        primaryHitActor ? reinterpret_cast<void*>(&primaryHitActor.GetBaseObject()) : nullptr,
                        primaryHitActor ? primaryHitActor.GetProperty<int32_t>(Dali::Actor::Property::ID) : -1,
                        primaryHitActor ? primaryHitActor.GetProperty<Dali::String>(Dali::Actor::Property::NAME).CStr() : "",
                        POINT_STATE_NAME[localVars.primaryPointState],
                        localVars.candidates.size(),
                        previousActiveCount,
                        activeCount,
                        reinterpret_cast<void*>(consumedActor),
                        consumedActor ? consumedActor->GetId() : -1,
                        consumedActor ? consumedActor->GetName().data() : "",
                        consumerChanged);
}

bool EmitHoverSignal(Actor& actor, const Dali::HoverEvent& event)
{
  DALI_TRACE_SCOPE(gTraceFilter, "DALI_EMIT_HOVER_EVENT_SIGNAL");
  return actor.DispatchHoverEvent(event);
}

bool EmitHoverSignal(Actor& actor, const HoverEventPtr& originalEvent, PointState::Type state)
{
  HoverEventPtr hoverEvent = HoverEvent::Clone(*originalEvent.Get());
  hoverEvent->GetPoint(0).SetState(state);
  return EmitHoverSignal(actor, Dali::HoverEvent(hoverEvent.Get()));
}
} // unnamed namespace

struct GeometryHoverEventProcessor::Impl
{
  static inline void HitTest(GeometryHoverEventProcessor& processor, ProcessHoverEventVariables& localVars, const Integration::HoverEvent& event)
  {
    bool firstPointParsed = false;
    for(auto&& currentPoint : event.points)
    {
      HitTestAlgorithm::Results hitTestResults;
      hitTestResults.eventTime = event.time;
      ActorHoverableCheck actorHoverableCheck;
      HitTestAlgorithm::HitTest(processor.mScene.GetSize(), processor.mScene.GetRenderTaskList(), processor.mScene.GetLayerList(), currentPoint.GetScreenPosition(), hitTestResults, actorHoverableCheck, Integration::Scene::TouchPropagationType::GEOMETRY);

      Integration::Point newPoint(currentPoint);
      newPoint.SetHitActor(hitTestResults.actor);
      newPoint.SetLocalPosition(hitTestResults.actorCoordinates);
      localVars.hoverEvent->AddPoint(newPoint);

      DALI_LOG_INFO(gLogFilter,
                    Debug::General,
                    "  State(%s), Screen(%.0f, %.0f), HitActor(%p, %s), Local(%.2f, %.2f)\n",
                    POINT_STATE_NAME[currentPoint.GetState()],
                    currentPoint.GetScreenPosition().x,
                    currentPoint.GetScreenPosition().y,
                    (hitTestResults.actor ? reinterpret_cast<void*>(&hitTestResults.actor.GetBaseObject()) : nullptr),
                    (hitTestResults.actor ? hitTestResults.actor.GetProperty<Dali::String>(Dali::Actor::Property::NAME).CStr() : ""),
                    hitTestResults.actorCoordinates.x,
                    hitTestResults.actorCoordinates.y);

      if(!firstPointParsed)
      {
        firstPointParsed = true;

        // Every point is copied to the HoverEvent, but only the primary point
        // determines which Actors receive this input.
        // GEOMETRY hit results are stored back-to-front. Keep a strong, de-duplicated
        // snapshot in front-to-back dispatch order so callbacks may mutate the scene safely.
        for(auto candidateIter = hitTestResults.actorLists.rbegin(); candidateIter != hitTestResults.actorLists.rend(); ++candidateIter)
        {
          Actor* candidate = *candidateIter;
          auto   found     = std::find_if(localVars.candidates.begin(), localVars.candidates.end(), [candidate](const ActorPtr& actor)
                { return actor.Get() == candidate; });
          if(found == localVars.candidates.end())
          {
            localVars.candidates.emplace_back(candidate);
          }
        }
      }
    }
  }

  static inline bool IsTargetInList(const std::list<ActorObserver>& targets, Actor* actor)
  {
    return std::any_of(targets.begin(), targets.end(), [actor](const ActorObserver& observer)
    { return observer.GetActor() == actor; });
  }

  static inline bool IsTarget(GeometryHoverEventProcessor& processor, Actor* actor)
  {
    return IsTargetInList(processor.mHoverTargets, actor) ||
           IsTargetInList(processor.mPreviousHoverTargets, actor);
  }

  static inline bool IsTargetInvalidated(GeometryHoverEventProcessor& processor, Actor* actor)
  {
    return std::any_of(processor.mInvalidatedTargets.begin(), processor.mInvalidatedTargets.end(), [actor](const ActorPtr& invalidated)
    { return invalidated.Get() == actor; });
  }

  static inline void QueueInterrupt(GeometryHoverEventProcessor& processor, Actor* actor)
  {
    bool alreadyTerminalized = std::find(processor.mTerminalizedTargets.begin(), processor.mTerminalizedTargets.end(), actor) != processor.mTerminalizedTargets.end();
    if(!actor || alreadyTerminalized || IsTargetInvalidated(processor, actor))
    {
      return;
    }

    // Keep the disconnected Actor alive until the current callback stack unwinds,
    // then deliver one INTERRUPTED without removing its observer re-entrantly.
    processor.mInvalidatedTargets.emplace_back(actor);
    processor.mPendingInterrupts.emplace_back(actor);
    if(processor.mLastConsumedActor == actor)
    {
      processor.mLastConsumedActor = nullptr;
    }
  }

  static inline void RemoveInvalidatedObservers(GeometryHoverEventProcessor& processor)
  {
    auto removeInvalidated = [&processor](std::list<ActorObserver>& targets)
    {
      targets.remove_if([&processor](const ActorObserver& observer)
      {
        Actor* actor = observer.GetActor();
        return !actor || IsTargetInvalidated(processor, actor);
      });
    };

    removeInvalidated(processor.mHoverTargets);
    removeInvalidated(processor.mPreviousHoverTargets);
  }

  static inline void EmitTerminalEvent(GeometryHoverEventProcessor& processor, Actor& actor, const HoverEventPtr& event, PointState::Type state, const char* reason)
  {
    processor.mTerminalizedTargets.push_back(&actor);
    LogLifecycle(actor, event, state, reason);
    EmitHoverSignal(actor, event, state);
  }

  static inline void FlushInterrupts(GeometryHoverEventProcessor& processor, const HoverEventPtr& event)
  {
    while(!processor.mPendingInterrupts.empty())
    {
      ActorPtr actor = std::move(processor.mPendingInterrupts.front());
      processor.mPendingInterrupts.pop_front();
      if(actor)
      {
        EmitTerminalEvent(processor, *actor, event, PointState::INTERRUPTED, "DISCONNECTED");
      }
    }

    RemoveInvalidatedObservers(processor);
  }

  static inline std::list<ActorObserver>::iterator FindPreviousTarget(GeometryHoverEventProcessor& processor, Actor* actor)
  {
    return std::find_if(processor.mPreviousHoverTargets.begin(), processor.mPreviousHoverTargets.end(), [actor](const ActorObserver& observer)
    { return observer.GetActor() == actor; });
  }

  static inline void DeliverTerminalEventToAll(GeometryHoverEventProcessor& processor, const HoverEventPtr& event, PointState::Type state)
  {
    const char* reason = state == PointState::FINISHED ? "STREAM_FINISHED" : "STREAM_INTERRUPTED";

    // FINISHED and explicit INTERRUPTED close every active lifecycle; one
    // target's consume result must not suppress terminal delivery to the rest.
    processor.mProcessingHover = true;
    processor.mTerminalizedTargets.clear();
    processor.mPreviousHoverTargets.splice(processor.mPreviousHoverTargets.end(), processor.mHoverTargets);

    for(auto& observer : processor.mPreviousHoverTargets)
    {
      Actor* actor = observer.GetActor();
      if(actor && !IsTargetInvalidated(processor, actor))
      {
        EmitTerminalEvent(processor, *actor, event, state, reason);
      }
    }

    FlushInterrupts(processor, event);
    processor.ClearHoverTargets();
    processor.mProcessingHover = false;
  }

  static inline void DeliverEvents(GeometryHoverEventProcessor& processor, ProcessHoverEventVariables& localVars)
  {
    const PointState::Type state = localVars.primaryPointState;
    if(state == PointState::FINISHED || state == PointState::INTERRUPTED)
    {
      DeliverTerminalEventToAll(processor, localVars.hoverEvent, state);
      return;
    }

    processor.mProcessingHover = true;
    processor.mTerminalizedTargets.clear();
    RemoveInvalidatedObservers(processor);
    processor.mInvalidatedTargets.clear();
    const size_t previousActiveCount = processor.mHoverTargets.size();
    Actor*       previousConsumer    = processor.mLastConsumedActor;

    // Rebuild mHoverTargets as this input's visited prefix. Existing targets
    // are spliced back as they are visited; leftovers are terminalized below.
    processor.mPreviousHoverTargets.splice(processor.mPreviousHoverTargets.end(), processor.mHoverTargets);

    Actor* consumedActor = nullptr;
    for(size_t candidateIndex = 0u; candidateIndex < localVars.candidates.size(); ++candidateIndex)
    {
      const ActorPtr& candidate = localVars.candidates[candidateIndex];
      Actor*          actor     = candidate.Get();

      // Earlier callbacks may mutate later entries in the hit-test snapshot.
      if(!actor || !actor->GetHoverRequired() || !actor->IsHittable() || !IsActuallySensitive(actor))
      {
        DALI_LOG_INFO(gLogFilter,
                      Debug::General,
                      "  GeoHoverCandidate[%zu]: Actor(%p), skipped(1)\n",
                      candidateIndex,
                      reinterpret_cast<void*>(actor));
        continue;
      }

      auto previous    = FindPreviousTarget(processor, actor);
      bool isNewTarget = previous == processor.mPreviousHoverTargets.end();
      if(isNewTarget)
      {
        processor.mHoverTargets.emplace_back(MakeCallback(&processor, &GeometryHoverEventProcessor::OnObservedActorDisconnected));
        processor.mHoverTargets.back().SetActor(actor);
      }
      else
      {
        processor.mHoverTargets.splice(processor.mHoverTargets.end(), processor.mPreviousHoverTargets, previous);
      }

      bool consumed = false;
      if(isNewTarget && (state == PointState::MOTION || state == PointState::STATIONARY))
      {
        // A target first discovered mid-stream needs an enter lifecycle before
        // it receives the original MOTION/STATIONARY input.
        LogLifecycle(*actor, localVars.hoverEvent, PointState::STARTED, "SYNTHETIC_ENTER");
        bool startedConsumed = EmitHoverSignal(*actor, localVars.hoverEvent, PointState::STARTED);
        consumed             = startedConsumed;
        FlushInterrupts(processor, localVars.hoverEvent);
      }

      const bool targetStillActive = IsTargetInList(processor.mHoverTargets, actor) &&
                                     !IsTargetInvalidated(processor, actor);
      if(targetStillActive)
      {
        if(state != PointState::MOTION || actor->IsDispatchHoverMotionEnabled())
        {
          if(state == PointState::STARTED)
          {
            LogLifecycle(*actor, localVars.hoverEvent, state, "INPUT_STARTED");
          }
          bool dispatchConsumed = EmitHoverSignal(*actor, localVars.hoverEventHandle);
          consumed              = dispatchConsumed || consumed;
          FlushInterrupts(processor, localVars.hoverEvent);
        }
        else if(!isNewTarget && processor.mLastConsumedActor == actor)
        {
          // Suppressing MOTION callbacks must not suddenly expose candidates
          // behind the Actor that consumed the preceding input.
          consumed = true;
        }
      }

      DALI_LOG_INFO(gLogFilter,
                    Debug::General,
                    "  GeoHoverCandidate[%zu]: Actor(%p, %s), state(%s), new(%d), motionSkipped(%d), active(%d), consumed(%d)\n",
                    candidateIndex,
                    reinterpret_cast<void*>(actor),
                    actor->GetName().data(),
                    POINT_STATE_NAME[state],
                    isNewTarget,
                    targetStillActive && state == PointState::MOTION && !actor->IsDispatchHoverMotionEnabled(),
                    targetStillActive,
                    consumed);

      if(consumed)
      {
        // Only candidates reached before this break belong to the active prefix.
        consumedActor = actor;
        break;
      }
    }

    processor.mLastConsumedActor = consumedActor && IsTargetInList(processor.mHoverTargets, consumedActor) ? consumedActor : nullptr;

    // Anything that was active but not visited in this event has left the visited prefix.
    // Invalid targets receive INTERRUPTED; ordinary prefix/geometry exits receive LEAVE.
    for(auto& observer : processor.mPreviousHoverTargets)
    {
      Actor* actor = observer.GetActor();
      if(actor && !IsTargetInvalidated(processor, actor))
      {
        PointState::Type exitState      = actor->IsHittable() && IsActuallySensitive(actor) ? PointState::LEAVE : PointState::INTERRUPTED;
        const bool       stillCandidate = std::any_of(localVars.candidates.begin(), localVars.candidates.end(), [actor](const ActorPtr& candidate)
              { return candidate.Get() == actor; });
        const char*      reason         = exitState == PointState::INTERRUPTED ? "NOT_HITTABLE" : (stillCandidate ? "PREFIX_CUT_BY_CONSUMER" : "GEOMETRY_EXIT");
        EmitTerminalEvent(processor, *actor, localVars.hoverEvent, exitState, reason);
      }
    }

    FlushInterrupts(processor, localVars.hoverEvent);
    processor.mPreviousHoverTargets.clear();
    processor.mInvalidatedTargets.clear();

    const bool consumerChanged = previousConsumer != processor.mLastConsumedActor;
    if(state != PointState::MOTION || consumerChanged)
    {
      LogDispatchSummary(localVars, processor.mLastConsumedActor, previousActiveCount, processor.mHoverTargets.size(), consumerChanged);
    }
    processor.mProcessingHover = false;
  }
};

GeometryHoverEventProcessor::GeometryHoverEventProcessor(Scene& scene)
: mScene(scene)
{
  DALI_LOG_TRACE_METHOD(gLogFilter);
}

GeometryHoverEventProcessor::~GeometryHoverEventProcessor()
{
  DALI_LOG_TRACE_METHOD(gLogFilter);
}

void GeometryHoverEventProcessor::SendInterruptedHoverEvent(Actor* actor)
{
  if(actor && Impl::IsTarget(*this, actor))
  {
    Impl::QueueInterrupt(*this, actor);
    if(!mProcessingHover)
    {
      Integration::Point point;
      point.SetState(PointState::INTERRUPTED);
      point.SetHitActor(Dali::Actor(actor));

      HoverEventPtr hoverEvent(new HoverEvent(GetMilliSeconds()));
      hoverEvent->AddPoint(point);

      mProcessingHover = true;
      Impl::FlushInterrupts(*this, hoverEvent);
      mInvalidatedTargets.clear();
      mTerminalizedTargets.clear();
      mProcessingHover = false;
    }
  }
}

void GeometryHoverEventProcessor::ProcessHoverEvent(const Integration::HoverEvent& event)
{
  DALI_LOG_TRACE_METHOD(gLogFilter);
  DALI_ASSERT_ALWAYS(!event.points.empty() && "Empty HoverEvent sent from Integration\n");

  PRINT_HIERARCHY(gLogFilter);
  DALI_TRACE_SCOPE(gTraceFilter, "DALI_PROCESS_HOVER_EVENT");

  // Observer callbacks defer their own removal until input processing is safely resumed.
  Impl::RemoveInvalidatedObservers(*this);
  mInvalidatedTargets.clear();

  ProcessHoverEventVariables localVars;
  localVars.hoverEvent = new HoverEvent(event.time);

  if(event.points[0].GetState() == PointState::INTERRUPTED)
  {
    // An integration INTERRUPTED terminates the current active prefix without
    // selecting a new target from the pointer position.
    for(const auto& point : event.points)
    {
      localVars.hoverEvent->AddPoint(point);
    }
    for(const auto& observer : mHoverTargets)
    {
      Actor* activeTarget = observer.GetActor();
      if(activeTarget)
      {
        localVars.hoverEvent->GetPoint(0).SetHitActor(Dali::Actor(activeTarget));
        break;
      }
    }
    localVars.hoverEventHandle  = Dali::HoverEvent(localVars.hoverEvent.Get());
    localVars.primaryPointState = PointState::INTERRUPTED;
    Impl::DeliverEvents(*this, localVars);
    return;
  }

  DALI_LOG_INFO(gLogFilter, Debug::Concise, "\n");
  DALI_LOG_INFO(gLogFilter, Debug::General, "Point(s): %d\n", event.GetPointCount());
  localVars.hoverEventHandle = Dali::HoverEvent(localVars.hoverEvent.Get());
  Impl::HitTest(*this, localVars, event);

  localVars.primaryPointState = localVars.hoverEvent->GetPoint(0).GetState();
  Impl::DeliverEvents(*this, localVars);
}

bool GeometryHoverEventProcessor::HasActiveHover() const
{
  auto hasActor = [](const ActorObserver& observer)
  { return observer.GetActor() != nullptr; };

  return std::any_of(mHoverTargets.begin(), mHoverTargets.end(), hasActor) ||
         std::any_of(mPreviousHoverTargets.begin(), mPreviousHoverTargets.end(), hasActor);
}

void GeometryHoverEventProcessor::ClearHoverTargets()
{
  mHoverTargets.clear();
  mPreviousHoverTargets.clear();
  mPendingInterrupts.clear();
  mInvalidatedTargets.clear();
  mTerminalizedTargets.clear();
  mLastConsumedActor = nullptr;
}

void GeometryHoverEventProcessor::OnObservedActorDisconnected(Actor* actor)
{
  if(actor && Impl::IsTarget(*this, actor))
  {
    Impl::QueueInterrupt(*this, actor);
    if(!mProcessingHover)
    {
      Integration::Point point;
      point.SetState(PointState::INTERRUPTED);
      point.SetHitActor(Dali::Actor(actor));

      HoverEventPtr hoverEvent(new HoverEvent(GetMilliSeconds()));
      hoverEvent->AddPoint(point);

      // Do not destroy an ActorObserver from inside its SceneObjectRemoved callback.
      // Invalid observers are removed at the beginning of the next input event.
      mProcessingHover = true;
      while(!mPendingInterrupts.empty())
      {
        ActorPtr interruptedActor = std::move(mPendingInterrupts.front());
        mPendingInterrupts.pop_front();
        if(interruptedActor)
        {
          Impl::EmitTerminalEvent(*this, *interruptedActor, hoverEvent, PointState::INTERRUPTED, "DISCONNECTED");
        }
      }
      mTerminalizedTargets.clear();
      mProcessingHover = false;
    }
  }
}
} //namespace DALI_NAMESPACE::Internal
