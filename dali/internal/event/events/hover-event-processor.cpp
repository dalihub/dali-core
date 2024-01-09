/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

namespace Dali
{
namespace Internal
{
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_PERFORMANCE_MARKER, false);
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_HOVER_PROCESSOR");
#endif // defined(DEBUG_ENABLED)

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
  return actorImpl.GetHoverRequired() && (state!= PointState::MOTION || actorImpl.IsDispatchHoverMotion());
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
  for (; rIter != actorLists.rend(); rIter++)
  {
    Actor* actorImpl(*rIter);
    // Only emit the signal if the actor's hover signal has connections (or derived actor implementation requires hover).
    if(ShouldEmitHoverEvent(*actorImpl, hoverEvent))
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
      else if(actorImpl->EmitHoverEventSignal(hoverEvent))
      {
        // One of this actor's listeners has consumed the event so set this actor as the consumed actor.
        consumedActor = Dali::Actor(actorImpl);
        break;
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
    return actor->IsVisible() && // Actor is visible, if not visible then none of its children are visible.
           actor->IsSensitive(); // Actor is sensitive, if insensitive none of its children should be hittable either.
  }

  bool DoesLayerConsumeHit(Layer* layer) override
  {
    return layer->IsHoverConsumed();
  }

  bool ActorRequiresHitResultCheck(Actor* actor, Integration::Point point, Vector2 hitPointLocal, uint32_t timeStamp, bool isGeometry) override
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
    if(mScene.IsGeometryHittestEnabled())
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

  PointState::Type state = static_cast<PointState::Type>(event.points[0].GetState());

  PRINT_HIERARCHY(gLogFilter);

  DALI_TRACE_SCOPE(gTraceFilter, "DALI_PROCESS_HOVER_EVENT");

  bool isGeometry = mScene.IsGeometryHittestEnabled();

  // Copy so we can add the results of a hit-test.
  HoverEventPtr hoverEvent(new HoverEvent(event.time));

  // 1) Check if it is an interrupted event - we should inform our last primary hit actor about this
  //    and emit the stage signal as well.

  if(state == PointState::INTERRUPTED)
  {
    Dali::Actor        consumingActor;
    Integration::Point currentPoint(event.points[0]);

    Actor* lastPrimaryHitActor(mLastPrimaryHitActor.GetActor());
    if(lastPrimaryHitActor)
    {
      Dali::Actor lastPrimaryHitActorHandle(lastPrimaryHitActor);
      currentPoint.SetHitActor(lastPrimaryHitActorHandle);
      if(isGeometry)
      {
        consumingActor = GeoAllocAndEmitHoverSignals(mCandidateActorLists, event.time, currentPoint);
      }
      else
      {
        consumingActor = AllocAndEmitHoverSignals(event.time, lastPrimaryHitActorHandle, currentPoint);
      }
    }

    // If the last consumed actor was different to the primary hit actor then inform it as well (if it has not already been informed).
    Actor* lastConsumedActor(mLastConsumedActor.GetActor());
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
    Actor* hoverStartConsumedActor(mHoverStartConsumedActor.GetActor());
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

    Clear();
    mHoverStartConsumedActor.SetActor(nullptr);
    return; // No need for hit testing
  }

  // 2) Hit Testing.

  Dali::HoverEvent hoverEventHandle(hoverEvent.Get());

  DALI_LOG_INFO(gLogFilter, Debug::Concise, "\n");
  DALI_LOG_INFO(gLogFilter, Debug::General, "Point(s): %d\n", event.GetPointCount());

  RenderTaskPtr currentRenderTask;
  bool          firstPointParsed = false;

  for(auto&& currentPoint : event.points)
  {
    HitTestAlgorithm::Results hitTestResults;
    hitTestResults.eventTime = event.time;
    ActorHoverableCheck       actorHoverableCheck;
    HitTestAlgorithm::HitTest(mScene.GetSize(), mScene.GetRenderTaskList(), mScene.GetLayerList(), currentPoint.GetScreenPosition(), hitTestResults, actorHoverableCheck, isGeometry);

    Integration::Point newPoint(currentPoint);
    newPoint.SetHitActor(hitTestResults.actor);
    newPoint.SetLocalPosition(hitTestResults.actorCoordinates);

    hoverEvent->AddPoint(newPoint);

    DALI_LOG_INFO(gLogFilter, Debug::General, "  State(%s), Screen(%.0f, %.0f), HitActor(%p, %s), Local(%.2f, %.2f)\n", TOUCH_POINT_STATE[currentPoint.GetState()], currentPoint.GetScreenPosition().x, currentPoint.GetScreenPosition().y, (hitTestResults.actor ? reinterpret_cast<void*>(&hitTestResults.actor.GetBaseObject()) : NULL), (hitTestResults.actor ? hitTestResults.actor.GetProperty<std::string>(Dali::Actor::Property::NAME).c_str() : ""), hitTestResults.actorCoordinates.x, hitTestResults.actorCoordinates.y);

    // Only set the currentRenderTask for the primary hit actor.
    if(!firstPointParsed)
    {
      firstPointParsed  = true;
      currentRenderTask = hitTestResults.renderTask;
      mCandidateActorLists = hitTestResults.actorLists;
    }
  }

  // 3) Recursively deliver events to the actor and its parents, until the event is consumed or the stage is reached.

  Integration::Point primaryPoint      = hoverEvent->GetPoint(0);
  Dali::Actor        primaryHitActor   = primaryPoint.GetHitActor();
  PointState::Type   primaryPointState = primaryPoint.GetState();

  // Emit the touch signal
  Dali::Actor consumedActor;
  if(currentRenderTask)
  {
    Dali::Actor hitActor = hoverEvent->GetHitActor(0);

    if(isGeometry)
    {
      consumedActor = EmitGeoHoverSignals(mCandidateActorLists, hoverEventHandle);
    }
    else
    {
      // If the actor is hit first, the hover is started.
      if(hitActor &&
        mLastPrimaryHitActor.GetActor() != hitActor &&
        state == PointState::MOTION)
      {
        Actor* hitActorImpl = &GetImplementation(hitActor);
        if(hitActorImpl->GetLeaveRequired())
        {
          hoverEvent->GetPoint(0).SetState(PointState::STARTED);
        }
      }
      consumedActor = EmitHoverSignals(hitActor, hoverEventHandle);
    }

    if(hoverEvent->GetPoint(0).GetState() != PointState::MOTION)
    {
      DALI_LOG_RELEASE_INFO("PrimaryHitActor:(%p), id(%d), name(%s), state(%s)\n", primaryHitActor ? reinterpret_cast<void*>(&primaryHitActor.GetBaseObject()) : NULL, primaryHitActor ? primaryHitActor.GetProperty<int32_t>(Dali::Actor::Property::ID) : -1, primaryHitActor ? primaryHitActor.GetProperty<std::string>(Dali::Actor::Property::NAME).c_str() : "", TOUCH_POINT_STATE[hoverEvent->GetPoint(0).GetState()]);
      DALI_LOG_RELEASE_INFO("ConsumedActor:  (%p), id(%d), name(%s), state(%s)\n", consumedActor ? reinterpret_cast<void*>(&consumedActor.GetBaseObject()) : NULL, consumedActor ? consumedActor.GetProperty<int32_t>(Dali::Actor::Property::ID) : -1, consumedActor ? consumedActor.GetProperty<std::string>(Dali::Actor::Property::NAME).c_str() : "", TOUCH_POINT_STATE[hoverEvent->GetPoint(0).GetState()]);
    }
  }

  if((primaryPointState == PointState::STARTED) &&
     (hoverEvent->GetPointCount() == 1) &&
     (consumedActor && GetImplementation(consumedActor).OnScene()))
  {
    mHoverStartConsumedActor.SetActor(&GetImplementation(consumedActor));
  }

  // 4) Check if the last primary hit actor requires a leave event and if it was different to the current primary
  //    hit actor.  Also process the last consumed actor in the same manner.

  Actor* lastPrimaryHitActor(mLastPrimaryHitActor.GetActor());
  Actor* lastConsumedActor(mLastConsumedActor.GetActor());
  if((primaryPointState == PointState::STARTED) || (primaryPointState == PointState::MOTION) || (primaryPointState == PointState::FINISHED) || (primaryPointState == PointState::STATIONARY))
  {
    if(mLastRenderTask)
    {
      Dali::Actor leaveEventConsumer;
      RenderTask& lastRenderTaskImpl = *mLastRenderTask.Get();

      if(lastPrimaryHitActor &&
        lastPrimaryHitActor != primaryHitActor &&
        lastPrimaryHitActor != consumedActor)
      {
        if(lastPrimaryHitActor->IsHittable() && IsActuallySensitive(lastPrimaryHitActor))
        {
          if(isGeometry)
          {
            // This is a situation where actors who received a hover event must leave.
            // Compare the lastActorList that received the hover event and the CandidateActorList that can receive the new hover event
            // If the hover event can no longer be received, Leave is sent.
            std::list<Dali::Internal::Actor*>::reverse_iterator rLastIter = mLastActorLists.rbegin();
            for(; rLastIter != mLastActorLists.rend(); rLastIter++)
            {
              bool find = false;
              std::list<Dali::Internal::Actor*>::reverse_iterator rCandidateIter = mCandidateActorLists.rbegin();
              for(; rCandidateIter != mCandidateActorLists.rend(); rCandidateIter++)
              {
                if(*rCandidateIter == *rLastIter)
                {
                  find = true;
                  break;
                }
              }
              if(!find)
              {
                DALI_LOG_RELEASE_INFO("LeaveActor(Hit): (%p) %d %s\n", reinterpret_cast<void*>(*rLastIter), (*rLastIter)->GetId(), (*rLastIter)->GetName().data());
                leaveEventConsumer = EmitHoverSignals(*rLastIter, lastRenderTaskImpl, hoverEvent, PointState::LEAVE, isGeometry);
              }
              // If the actor has been consumed, you do not need to proceed.
              if(*rLastIter == lastConsumedActor)
              {
                break;
              }
            }
          }
          else if(lastPrimaryHitActor->GetLeaveRequired())
          {
            // In the case of isGeometry, it is not propagated but only sent to actors who are not hittable.
            DALI_LOG_RELEASE_INFO("LeaveActor(Hit): (%p) %d %s\n", reinterpret_cast<void*>(lastPrimaryHitActor), lastPrimaryHitActor->GetId(), lastPrimaryHitActor->GetName().data());
            leaveEventConsumer = EmitHoverSignals(mLastPrimaryHitActor.GetActor(), lastRenderTaskImpl, hoverEvent, PointState::LEAVE, isGeometry);
          }
        }
        else if(primaryPointState != PointState::STARTED)
        {
          // At this point mLastPrimaryHitActor was touchable and sensitive in the previous touch event process but is not in the current one.
          // An interrupted event is send to allow some actors to go back to their original state (i.e. Button controls)
          DALI_LOG_RELEASE_INFO("InterruptedActor(Hit): (%p) %d %s\n", reinterpret_cast<void*>(lastPrimaryHitActor), lastPrimaryHitActor->GetId(), lastPrimaryHitActor->GetName().data());
          leaveEventConsumer = EmitHoverSignals(mLastPrimaryHitActor.GetActor(), lastRenderTaskImpl, hoverEvent, PointState::INTERRUPTED, isGeometry);
        }
      }

      // Check if the motion event has been consumed by another actor's listener.  In this case, the previously
      // consumed actor's listeners may need to be informed (through a leave event).
      // Further checks here to ensure we do not signal the same actor twice for the same event.
      if(lastConsumedActor &&
        lastConsumedActor != consumedActor &&
        lastConsumedActor != lastPrimaryHitActor &&
        lastConsumedActor != primaryHitActor &&
        lastConsumedActor != leaveEventConsumer)
      {
        if(lastConsumedActor->IsHittable() && IsActuallySensitive(lastConsumedActor))
        {
          if(lastConsumedActor->GetLeaveRequired() && !isGeometry) // For geometry, we have already sent leave. There is no need to send leave repeatedly.
          {
            DALI_LOG_RELEASE_INFO("LeaveActor(Consume): (%p) %d %s\n", reinterpret_cast<void*>(lastConsumedActor), lastConsumedActor->GetId(), lastConsumedActor->GetName().data());
            EmitHoverSignals(lastConsumedActor, lastRenderTaskImpl, hoverEvent, PointState::LEAVE, isGeometry);
          }
        }
        else if(primaryPointState != PointState::STARTED)
        {
          // At this point mLastConsumedActor was touchable and sensitive in the previous touch event process but is not in the current one.
          // An interrupted event is send to allow some actors to go back to their original state (i.e. Button controls)
          DALI_LOG_RELEASE_INFO("InterruptedActor(Consume): (%p) %d %s\n", reinterpret_cast<void*>(lastConsumedActor), lastConsumedActor->GetId(), lastConsumedActor->GetName().data());
          EmitHoverSignals(mLastConsumedActor.GetActor(), lastRenderTaskImpl, hoverEvent, PointState::INTERRUPTED, isGeometry);
        }
      }
    }
  }

  // 5) If our primary point is a FINISHED event, then the primary point (in multi-touch) will change next
  //    time so set our last primary actor to NULL.  Do the same to the last consumed actor as well.

  if(primaryPointState == PointState::FINISHED)
  {
    Clear();
  }
  else
  {
    // The primaryHitActor may have been removed from the scene so ensure it is still on the scene before setting members.
    if(primaryHitActor && GetImplementation(primaryHitActor).OnScene())
    {
      mLastPrimaryHitActor.SetActor(&GetImplementation(primaryHitActor));
      // Only observe the consumed actor if we have a primaryHitActor (check if it is still on the scene).
      if(consumedActor && GetImplementation(consumedActor).OnScene())
      {
        mLastConsumedActor.SetActor(&GetImplementation(consumedActor));
      }
      else
      {
        mLastConsumedActor.SetActor(nullptr);
      }

      mLastRenderTask = currentRenderTask;
      mLastActorLists = mCandidateActorLists;
    }
    else
    {
      Clear();
    }
  }

  // 6) Emit an interrupted event to the hover-started actor if it hasn't consumed the FINISHED.

  if(hoverEvent->GetPointCount() == 1) // Only want the first hover started
  {
    switch(primaryPointState)
    {
      case PointState::FINISHED:
      {
        Actor* hoverStartConsumedActor(mHoverStartConsumedActor.GetActor());
        if(hoverStartConsumedActor &&
           hoverStartConsumedActor != consumedActor &&
           hoverStartConsumedActor != lastPrimaryHitActor &&
           hoverStartConsumedActor != lastConsumedActor)
        {
          Dali::Actor        hoverStartConsumedActorHandle(hoverStartConsumedActor);
          Integration::Point primaryPoint = hoverEvent->GetPoint(0);
          primaryPoint.SetHitActor(hoverStartConsumedActorHandle);
          primaryPoint.SetState(PointState::INTERRUPTED);
          if(isGeometry)
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
          primaryPoint.SetHitActor(primaryHitActor);
          primaryPoint.SetState(primaryPointState);
        }

        mHoverStartConsumedActor.SetActor(nullptr);
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

} // namespace Internal

} // namespace Dali
