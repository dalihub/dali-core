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
#include <dali/internal/event/events/parent-hover-event-processor.h>

#if defined(DEBUG_ENABLED)
#include <sstream>
#endif

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
#include <dali/public-api/math/vector2.h>

namespace Dali::Internal
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
  Actor*              lastPrimaryHitActor{nullptr};
  Actor*              lastConsumedActor{nullptr};
  HoverEventPtr       hoverEvent;
  Dali::HoverEvent    hoverEventHandle;
  RenderTaskPtr       currentRenderTask;
  Dali::Actor         consumedActor;
  Dali::Actor         primaryHitActor;
  Integration::Point* primaryPoint{nullptr};
  PointState::Type    primaryPointState{PointState::STARTED};
};

bool ShouldEmitHoverEvent(const Actor& actorImpl, const Dali::HoverEvent& event)
{
  PointState::Type state = event.GetState(0);
  return actorImpl.GetHoverRequired() && (state != PointState::MOTION || actorImpl.IsDispatchHoverMotionEnabled());
}

Dali::Actor EmitHoverSignals(Dali::Actor actor, const Dali::HoverEvent& event)
{
  Dali::Actor consumedActor;

  if(actor)
  {
    Dali::Actor oldParent(actor.GetParent());
    Actor&      actorImpl(GetImplementation(actor));
    bool        consumed(false);

    if(ShouldEmitHoverEvent(actorImpl, event))
    {
      DALI_TRACE_SCOPE(gTraceFilter, "DALI_EMIT_HOVER_EVENT_SIGNAL");
      consumed = actorImpl.DispatchHoverEvent(event);
    }

    if(consumed)
    {
      consumedActor = Dali::Actor(&actorImpl);
    }
    else
    {
      // The actor may have been removed or reparented during the signal callbacks.
      Dali::Actor parent = actor.GetParent();
      if(parent && parent == oldParent)
      {
        consumedActor = EmitHoverSignals(parent, event);
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

Dali::Actor EmitHoverSignals(Actor* actor, RenderTask& renderTask, const HoverEventPtr& originalEvent, PointState::Type state)
{
  HoverEventPtr hoverEvent = HoverEvent::Clone(*originalEvent.Get());

  DALI_ASSERT_DEBUG(nullptr != actor && "NULL actor pointer");
  if(actor)
  {
    Integration::Point& primaryPoint   = hoverEvent->GetPoint(0);
    const Vector2&      screenPosition = primaryPoint.GetScreenPosition();
    Vector2             localPosition;
    actor->ScreenToLocal(renderTask, localPosition.x, localPosition.y, screenPosition.x, screenPosition.y);

    primaryPoint.SetLocalPosition(localPosition);
    primaryPoint.SetHitActor(Dali::Actor(actor));
    primaryPoint.SetState(state);
  }

  return EmitHoverSignals(Dali::Actor(actor), Dali::HoverEvent(hoverEvent.Get()));
}

void ClearHoverStartConsumedActorIfNeeded(ActorObserver& hoverStartConsumedActor, Actor* actor)
{
  if(actor && hoverStartConsumedActor.GetActor() == actor)
  {
    hoverStartConsumedActor.SetActor(nullptr);
  }
}
} // unnamed namespace

struct ParentHoverEventProcessor::Impl
{
  static inline void EmitInterruptedEvent(ParentHoverEventProcessor& processor, const Integration::HoverEvent& event)
  {
    Dali::Actor        consumingActor;
    Integration::Point currentPoint(event.points[0]);

    Actor* lastPrimaryHitActor(processor.mLastPrimaryHitActor.GetActor());
    if(lastPrimaryHitActor)
    {
      Dali::Actor lastPrimaryHitActorHandle(lastPrimaryHitActor);
      currentPoint.SetHitActor(lastPrimaryHitActorHandle);
      consumingActor = AllocAndEmitHoverSignals(event.time, lastPrimaryHitActorHandle, currentPoint);
    }

    Actor* lastConsumedActor(processor.mLastConsumedActor.GetActor());
    if(lastConsumedActor &&
       lastConsumedActor != lastPrimaryHitActor &&
       lastConsumedActor != consumingActor)
    {
      Dali::Actor lastConsumedActorHandle(lastConsumedActor);
      currentPoint.SetHitActor(lastConsumedActorHandle);
      AllocAndEmitHoverSignals(event.time, lastConsumedActorHandle, currentPoint);
    }

    Actor* hoverStartConsumedActor(processor.mHoverStartConsumedActor.GetActor());
    if(hoverStartConsumedActor &&
       hoverStartConsumedActor != lastPrimaryHitActor &&
       hoverStartConsumedActor != lastConsumedActor &&
       hoverStartConsumedActor != consumingActor)
    {
      Dali::Actor hoverStartConsumedActorHandle(hoverStartConsumedActor);
      currentPoint.SetHitActor(hoverStartConsumedActorHandle);
      AllocAndEmitHoverSignals(event.time, hoverStartConsumedActorHandle, currentPoint);
    }

    processor.Clear();
    processor.mHoverStartConsumedActor.SetActor(nullptr);
  }

  static inline void HitTest(ParentHoverEventProcessor& processor, ProcessHoverEventVariables& localVars, const Integration::HoverEvent& event)
  {
    bool firstPointParsed = false;
    for(auto&& currentPoint : event.points)
    {
      HitTestAlgorithm::Results hitTestResults;
      hitTestResults.eventTime = event.time;
      ActorHoverableCheck actorHoverableCheck;
      HitTestAlgorithm::HitTest(processor.mScene.GetSize(), processor.mScene.GetRenderTaskList(), processor.mScene.GetLayerList(), currentPoint.GetScreenPosition(), hitTestResults, actorHoverableCheck, Integration::Scene::TouchPropagationType::PARENT);

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
        firstPointParsed            = true;
        localVars.currentRenderTask = hitTestResults.renderTask;
      }
    }
  }

  static inline void DeliverEventsToActorAndParents(ParentHoverEventProcessor& processor, ProcessHoverEventVariables& localVars)
  {
    if(localVars.currentRenderTask)
    {
      Dali::Actor hitActor = localVars.hoverEvent->GetHitActor(0);

      if(hitActor && processor.mLastPrimaryHitActor.GetActor() != hitActor &&
         localVars.primaryPointState == PointState::MOTION && GetImplementation(hitActor).GetLeaveRequired())
      {
        localVars.lastPrimaryHitActor = processor.mLastPrimaryHitActor.GetActor();
        localVars.lastConsumedActor   = processor.mLastConsumedActor.GetActor();
        DeliverLeaveEvent(processor, localVars);

        localVars.hoverEvent->GetPoint(0).SetState(PointState::STARTED);
        localVars.primaryPointState = PointState::STARTED;

        localVars.consumedActor = EmitHoverSignals(hitActor, localVars.hoverEventHandle);
        UpdateMembersWithCurrentHitInformation(processor, localVars);
      }
      else
      {
        localVars.consumedActor = EmitHoverSignals(hitActor, localVars.hoverEventHandle);
        if(!localVars.consumedActor && localVars.primaryPointState == PointState::MOTION && !GetImplementation(hitActor).IsDispatchHoverMotionEnabled())
        {
          localVars.consumedActor = Dali::Actor(processor.mLastConsumedActor.GetActor());
        }
      }

      if(localVars.hoverEvent->GetPoint(0).GetState() != PointState::MOTION)
      {
        DALI_LOG_RELEASE_INFO("PrimaryHitActor:(%p), id(%d), name(%s), state(%s)\n",
                              localVars.primaryHitActor ? reinterpret_cast<void*>(&localVars.primaryHitActor.GetBaseObject()) : nullptr,
                              localVars.primaryHitActor ? localVars.primaryHitActor.GetProperty<int32_t>(Dali::Actor::Property::ID) : -1,
                              localVars.primaryHitActor ? localVars.primaryHitActor.GetProperty<Dali::String>(Dali::Actor::Property::NAME).CStr() : "",
                              POINT_STATE_NAME[localVars.hoverEvent->GetPoint(0).GetState()]);
        DALI_LOG_RELEASE_INFO("ConsumedActor:  (%p), id(%d), name(%s), state(%s)\n",
                              localVars.consumedActor ? reinterpret_cast<void*>(&localVars.consumedActor.GetBaseObject()) : nullptr,
                              localVars.consumedActor ? localVars.consumedActor.GetProperty<int32_t>(Dali::Actor::Property::ID) : -1,
                              localVars.consumedActor ? localVars.consumedActor.GetProperty<Dali::String>(Dali::Actor::Property::NAME).CStr() : "",
                              POINT_STATE_NAME[localVars.hoverEvent->GetPoint(0).GetState()]);
      }
    }

    if(localVars.primaryPointState == PointState::STARTED &&
       localVars.hoverEvent->GetPointCount() == 1 &&
       localVars.consumedActor && GetImplementation(localVars.consumedActor).OnScene())
    {
      processor.mHoverStartConsumedActor.SetActor(&GetImplementation(localVars.consumedActor));
    }
  }

  static inline void DeliverLeaveEvent(ParentHoverEventProcessor& processor, ProcessHoverEventVariables& localVars)
  {
    if(localVars.primaryPointState == PointState::STARTED ||
       localVars.primaryPointState == PointState::MOTION ||
       localVars.primaryPointState == PointState::FINISHED ||
       localVars.primaryPointState == PointState::STATIONARY)
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
            if(localVars.lastPrimaryHitActor->GetLeaveRequired())
            {
              DALI_LOG_RELEASE_INFO("LeaveActor(Hit): (%p) %d %s\n", reinterpret_cast<void*>(localVars.lastPrimaryHitActor), localVars.lastPrimaryHitActor->GetId(), localVars.lastPrimaryHitActor->GetName().data());
              leaveEventConsumer = EmitHoverSignals(processor.mLastPrimaryHitActor.GetActor(), lastRenderTaskImpl, localVars.hoverEvent, PointState::LEAVE);
              ClearHoverStartConsumedActorIfNeeded(processor.mHoverStartConsumedActor, processor.mLastPrimaryHitActor.GetActor());
            }
          }
          else if(localVars.primaryPointState != PointState::STARTED)
          {
            DALI_LOG_RELEASE_INFO("InterruptedActor(Hit): (%p) %d %s\n", reinterpret_cast<void*>(localVars.lastPrimaryHitActor), localVars.lastPrimaryHitActor->GetId(), localVars.lastPrimaryHitActor->GetName().data());
            leaveEventConsumer = EmitHoverSignals(processor.mLastPrimaryHitActor.GetActor(), lastRenderTaskImpl, localVars.hoverEvent, PointState::INTERRUPTED);
            ClearHoverStartConsumedActorIfNeeded(processor.mHoverStartConsumedActor, processor.mLastPrimaryHitActor.GetActor());
          }
        }

        if(localVars.lastConsumedActor &&
           localVars.lastConsumedActor != localVars.consumedActor &&
           localVars.lastConsumedActor != localVars.lastPrimaryHitActor &&
           localVars.lastConsumedActor != localVars.primaryHitActor &&
           localVars.lastConsumedActor != leaveEventConsumer)
        {
          if(localVars.lastConsumedActor->IsHittable() && IsActuallySensitive(localVars.lastConsumedActor))
          {
            if(localVars.lastConsumedActor->GetLeaveRequired())
            {
              DALI_LOG_RELEASE_INFO("LeaveActor(Consume): (%p) %d %s\n", reinterpret_cast<void*>(localVars.lastConsumedActor), localVars.lastConsumedActor->GetId(), localVars.lastConsumedActor->GetName().data());
              EmitHoverSignals(localVars.lastConsumedActor, lastRenderTaskImpl, localVars.hoverEvent, PointState::LEAVE);
              ClearHoverStartConsumedActorIfNeeded(processor.mHoverStartConsumedActor, localVars.lastConsumedActor);
            }
          }
          else if(localVars.primaryPointState != PointState::STARTED)
          {
            DALI_LOG_RELEASE_INFO("InterruptedActor(Consume): (%p) %d %s\n", reinterpret_cast<void*>(localVars.lastConsumedActor), localVars.lastConsumedActor->GetId(), localVars.lastConsumedActor->GetName().data());
            EmitHoverSignals(processor.mLastConsumedActor.GetActor(), lastRenderTaskImpl, localVars.hoverEvent, PointState::INTERRUPTED);
            ClearHoverStartConsumedActorIfNeeded(processor.mHoverStartConsumedActor, processor.mLastConsumedActor.GetActor());
          }
        }
      }
    }
  }

  static inline void UpdateMembersWithCurrentHitInformation(ParentHoverEventProcessor& processor, ProcessHoverEventVariables& localVars)
  {
    if(localVars.primaryPointState == PointState::FINISHED)
    {
      processor.Clear();
    }
    else
    {
      if(localVars.primaryHitActor && GetImplementation(localVars.primaryHitActor).OnScene())
      {
        processor.mLastPrimaryHitActor.SetActor(&GetImplementation(localVars.primaryHitActor));
        if(localVars.consumedActor && GetImplementation(localVars.consumedActor).OnScene())
        {
          processor.mLastConsumedActor.SetActor(&GetImplementation(localVars.consumedActor));
        }
        else
        {
          processor.mLastConsumedActor.SetActor(nullptr);
        }
        processor.mLastRenderTask = localVars.currentRenderTask;
      }
      else
      {
        processor.Clear();
      }
    }
  }

  static inline void DeliverInterruptedEventToHoverStartedActor(ParentHoverEventProcessor& processor, ProcessHoverEventVariables& localVars, const Integration::HoverEvent& event)
  {
    if(localVars.hoverEvent->GetPointCount() == 1 && localVars.primaryPointState == PointState::FINISHED)
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
        AllocAndEmitHoverSignals(event.time, hoverStartConsumedActorHandle, primaryPoint);
      }

      processor.mHoverStartConsumedActor.SetActor(nullptr);
    }
  }
};

ParentHoverEventProcessor::ParentHoverEventProcessor(Scene& scene)
: mScene(scene),
  mLastPrimaryHitActor(MakeCallback(this, &ParentHoverEventProcessor::OnObservedActorDisconnected))
{
  DALI_LOG_TRACE_METHOD(gLogFilter);
}

ParentHoverEventProcessor::~ParentHoverEventProcessor()
{
  DALI_LOG_TRACE_METHOD(gLogFilter);
}

void ParentHoverEventProcessor::SendInterruptedHoverEvent(Actor* actor)
{
  if(actor && (mLastPrimaryHitActor.GetActor() == actor || mLastConsumedActor.GetActor() == actor))
  {
    Integration::Point point;
    point.SetState(PointState::INTERRUPTED);
    point.SetHitActor(Dali::Actor(actor));
    AllocAndEmitHoverSignals(GetMilliSeconds(), point.GetHitActor(), point);
    Clear();
  }
}

void ParentHoverEventProcessor::ProcessHoverEvent(const Integration::HoverEvent& event)
{
  DALI_LOG_TRACE_METHOD(gLogFilter);
  DALI_ASSERT_ALWAYS(!event.points.empty() && "Empty HoverEvent sent from Integration\n");

  PRINT_HIERARCHY(gLogFilter);
  DALI_TRACE_SCOPE(gTraceFilter, "DALI_PROCESS_HOVER_EVENT");

  ProcessHoverEventVariables localVars;
  localVars.hoverEvent = new HoverEvent(event.time);

  if(event.points[0].GetState() == PointState::INTERRUPTED)
  {
    Impl::EmitInterruptedEvent(*this, event);
    return;
  }

  DALI_LOG_INFO(gLogFilter, Debug::Concise, "\n");
  DALI_LOG_INFO(gLogFilter, Debug::General, "Point(s): %d\n", event.GetPointCount());
  localVars.hoverEventHandle = Dali::HoverEvent(localVars.hoverEvent.Get());
  Impl::HitTest(*this, localVars, event);

  localVars.primaryPoint      = &localVars.hoverEvent->GetPoint(0);
  localVars.primaryHitActor   = localVars.primaryPoint->GetHitActor();
  localVars.primaryPointState = localVars.primaryPoint->GetState();

  Impl::DeliverEventsToActorAndParents(*this, localVars);

  localVars.lastPrimaryHitActor = mLastPrimaryHitActor.GetActor();
  localVars.lastConsumedActor   = mLastConsumedActor.GetActor();
  Impl::DeliverLeaveEvent(*this, localVars);
  Impl::UpdateMembersWithCurrentHitInformation(*this, localVars);
  Impl::DeliverInterruptedEventToHoverStartedActor(*this, localVars, event);
}

bool ParentHoverEventProcessor::HasActiveHover() const
{
  return mLastPrimaryHitActor.GetActor() || mLastConsumedActor.GetActor() || mHoverStartConsumedActor.GetActor();
}

void ParentHoverEventProcessor::Clear()
{
  mLastPrimaryHitActor.SetActor(nullptr);
  mLastConsumedActor.SetActor(nullptr);
  mLastRenderTask.Reset();
}

void ParentHoverEventProcessor::OnObservedActorDisconnected(Actor* actor)
{
  SendInterruptedHoverEvent(actor);
}
} // namespace Dali::Internal
