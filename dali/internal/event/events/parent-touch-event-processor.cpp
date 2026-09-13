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

#include <dali/internal/event/events/parent-touch-event-processor.h>

#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/events/hit-test-algorithm-impl.h>
#include <dali/internal/event/events/multi-point-event-util.h>
#include <dali/internal/event/events/touch-event-impl.h>
#include <dali/internal/event/events/touch-recipient-dispatcher.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/signals/callback.h>

namespace DALI_NAMESPACE::Internal
{
namespace
{
struct ProcessState
{
  explicit ProcessState(ParentTouchEventProcessor& eventProcessor)
  : processor(eventProcessor)
  {
  }

  ParentTouchEventProcessor& processor;
  Actor*                     lastPrimaryHitActor{nullptr};
  Actor*                     lastConsumedActor{nullptr};
  TouchEventPtr              touchEventImpl;
  Dali::TouchEvent           touchEventHandle;
  RenderTaskPtr              currentRenderTask;
  Dali::Actor                consumedActor;
  Dali::Actor                primaryHitActor;
  Integration::Point*        primaryPoint{nullptr};
  PointState::Type           primaryPointState{PointState::STARTED};
};

Dali::Actor DispatchInterceptToHierarchy(Dali::Actor actor, const Dali::TouchEvent& touchEvent)
{
  Dali::Actor interceptedActor;
  if(actor)
  {
    Dali::Actor parent = actor.GetParent();
    if(parent)
    {
      interceptedActor = DispatchInterceptToHierarchy(parent, touchEvent);
    }

    if(!interceptedActor)
    {
      Actor& actorImpl = GetImplementation(actor);
      if(TouchRecipientDispatcher::DispatchParentIntercept(actorImpl, touchEvent))
      {
        interceptedActor = Dali::Actor(&actorImpl);
      }
    }
  }
  return interceptedActor;
}

Dali::Actor DispatchTouchToHierarchy(Dali::Actor actor, const Dali::TouchEvent& touchEvent)
{
  Dali::Actor consumedActor;
  if(actor)
  {
    Dali::Actor oldParent(actor.GetParent());
    Actor&      actorImpl = GetImplementation(actor);

    if(TouchRecipientDispatcher::DispatchParentTouch(actorImpl, touchEvent))
    {
      consumedActor = Dali::Actor(&actorImpl);
    }
    else
    {
      Dali::Actor parent = actor.GetParent();
      if(parent && parent == oldParent)
      {
        consumedActor = DispatchTouchToHierarchy(parent, touchEvent);
      }
    }
  }
  return consumedActor;
}

Dali::Actor AllocateAndDispatchTouch(unsigned long time, Dali::Actor actor, const Integration::Point& point, RenderTaskPtr renderTask)
{
  TouchEventPtr    touchEvent(new TouchEvent(time));
  Dali::TouchEvent touchEventHandle(touchEvent.Get());
  touchEvent->AddPoint(point);
  touchEvent->SetRenderTask(Dali::RenderTask(renderTask.Get()));
  return DispatchTouchToHierarchy(actor, touchEventHandle);
}

Dali::Actor DispatchSyntheticTouch(Actor* actor, RenderTask& renderTask, const TouchEventPtr& originalTouchEvent, PointState::Type state)
{
  if(!actor)
  {
    return Dali::Actor();
  }

  TouchEventPtr touchEvent = TouchEvent::Clone(*originalTouchEvent.Get());
  touchEvent->SetRenderTask(Dali::RenderTask(&renderTask));

  Integration::Point& primaryPoint   = touchEvent->GetPoint(0);
  const Vector2&      screenPosition = primaryPoint.GetScreenPosition();
  Vector2             localPosition;
  actor->ScreenToLocal(renderTask, localPosition.x, localPosition.y, screenPosition.x, screenPosition.y);
  primaryPoint.SetLocalPosition(localPosition);
  primaryPoint.SetHitActor(Dali::Actor(actor));
  primaryPoint.SetState(state);

  return DispatchTouchToHierarchy(Dali::Actor(actor), Dali::TouchEvent(touchEvent.Get()));
}

void ParsePrimaryPoint(HitTestAlgorithm::Results& hitTestResults,
                       ActorObserver&             capturingTouchActorObserver,
                       ActorObserver&             ownTouchActorObserver,
                       const RenderTaskPtr&       lastRenderTask,
                       const Integration::Point&  currentPoint,
                       const Scene&               scene)
{
  Actor* capturingTouchActor = capturingTouchActorObserver.GetActor();
  if(capturingTouchActor && lastRenderTask)
  {
    hitTestResults.actor          = Dali::Actor(capturingTouchActor);
    hitTestResults.renderTask     = lastRenderTask;
    const Vector2& screenPosition = currentPoint.GetScreenPosition();
    capturingTouchActor->ScreenToLocal(*lastRenderTask, hitTestResults.actorCoordinates.x, hitTestResults.actorCoordinates.y, screenPosition.x, screenPosition.y);
    return;
  }

  Actor* ownTouchActor = ownTouchActorObserver.GetActor();
  HitTestAlgorithm::HitTest(scene.GetSize(), scene.GetRenderTaskList(), scene.GetLayerList(), currentPoint.GetScreenPosition(), hitTestResults, ownTouchActor, Integration::Scene::TouchPropagationType::PARENT);
  if(currentPoint.GetState() == PointState::STARTED && hitTestResults.actor)
  {
    Actor* hitActor = &GetImplementation(hitTestResults.actor);
    if(hitActor->CapturesAllTouchAfterStart())
    {
      capturingTouchActorObserver.SetActor(hitActor);
    }
    if(hitActor->IsAllowSelfInitiatedTouchOnlyEnabled())
    {
      ownTouchActorObserver.SetActor(hitActor);
    }
  }
}
} // unnamed namespace

struct ParentTouchEventProcessor::Impl
{
  static void EmitInterruptedEvent(ProcessState& state, const Integration::TouchEvent& event)
  {
    Dali::Actor        eventConsumer;
    Integration::Point currentPoint(event.points[0]);

    Actor* lastPrimaryHitActor = state.processor.mLastPrimaryHitActor.GetActor();
    if(lastPrimaryHitActor)
    {
      Dali::Actor lastPrimaryHitActorHandle(lastPrimaryHitActor);
      currentPoint.SetHitActor(lastPrimaryHitActorHandle);
      eventConsumer = AllocateAndDispatchTouch(event.time, lastPrimaryHitActorHandle, currentPoint, state.processor.mLastRenderTask);
    }

    Actor* lastConsumedActor = state.processor.mLastConsumedActor.GetActor();
    if(lastConsumedActor &&
       lastConsumedActor != lastPrimaryHitActor &&
       lastConsumedActor != eventConsumer)
    {
      Dali::Actor lastConsumedActorHandle(lastConsumedActor);
      currentPoint.SetHitActor(lastConsumedActorHandle);
      AllocateAndDispatchTouch(event.time, lastConsumedActorHandle, currentPoint, state.processor.mLastRenderTask);
    }

    Actor* touchDownConsumedActor = state.processor.mTouchDownConsumedActor.GetActor();
    if(touchDownConsumedActor &&
       touchDownConsumedActor != lastPrimaryHitActor &&
       touchDownConsumedActor != lastConsumedActor &&
       touchDownConsumedActor != eventConsumer)
    {
      Dali::Actor touchDownConsumedActorHandle(touchDownConsumedActor);
      currentPoint.SetHitActor(touchDownConsumedActorHandle);
      AllocateAndDispatchTouch(event.time, touchDownConsumedActorHandle, currentPoint, state.processor.mLastRenderTask);
    }

    state.processor.Clear();
    state.processor.mTouchDownConsumedActor.SetActor(nullptr);

    currentPoint.SetHitActor(Dali::Actor());
    TouchEventPtr    touchEvent(new TouchEvent(event.time));
    Dali::TouchEvent touchEventHandle(touchEvent.Get());
    touchEvent->AddPoint(currentPoint);
    state.processor.mScene.EmitTouchEventSignal(touchEventHandle);
  }

  static bool DeliverToHierarchy(ProcessState& state)
  {
    if(!state.currentRenderTask)
    {
      return false;
    }

    Actor* interceptedTouchActor = state.processor.mInterceptedTouchActor.GetActor();
    if(interceptedTouchActor)
    {
      state.consumedActor = DispatchTouchToHierarchy(Dali::Actor(interceptedTouchActor), state.touchEventHandle);
    }
    else
    {
      Dali::Actor interceptedActor = DispatchInterceptToHierarchy(state.primaryHitActor, state.touchEventHandle);
      if(interceptedActor)
      {
        state.processor.mInterceptedTouchActor.SetActor(&GetImplementation(interceptedActor));
        if(state.processor.mLastPrimaryHitActor.GetActor() &&
           state.processor.mLastPrimaryHitActor.GetActor() != interceptedActor &&
           state.processor.mLastRenderTask &&
           state.processor.mLastPrimaryPointState != PointState::FINISHED)
        {
          DispatchSyntheticTouch(state.processor.mLastPrimaryHitActor.GetActor(), *state.processor.mLastRenderTask, state.touchEventImpl, PointState::INTERRUPTED);
          state.processor.mTouchDownConsumedActor.SetActor(nullptr);
        }
        state.consumedActor = DispatchTouchToHierarchy(interceptedActor, state.touchEventHandle);
      }
      else
      {
        state.consumedActor = DispatchTouchToHierarchy(state.primaryHitActor, state.touchEventHandle);
      }
    }

    if(state.primaryPointState == PointState::DOWN &&
       state.touchEventImpl->GetPointCount() == 1u &&
       state.consumedActor &&
       state.consumedActor.GetProperty<bool>(Dali::Actor::Property::CONNECTED_TO_SCENE))
    {
      state.processor.mTouchDownConsumedActor.SetActor(&GetImplementation(state.consumedActor));
    }
    return static_cast<bool>(state.consumedActor);
  }

  static bool DeliverLeaveEvent(ProcessState& state)
  {
    if(state.primaryPointState != PointState::MOTION &&
       state.primaryPointState != PointState::UP &&
       state.primaryPointState != PointState::STATIONARY)
    {
      return false;
    }
    if(!state.processor.mLastRenderTask)
    {
      return false;
    }

    Dali::Actor leaveEventConsumer;
    RenderTask& lastRenderTask = *state.processor.mLastRenderTask;
    if(state.lastPrimaryHitActor &&
       state.lastPrimaryHitActor != state.primaryHitActor &&
       state.lastPrimaryHitActor != state.consumedActor)
    {
      if(state.lastPrimaryHitActor->IsHittable() && IsActuallySensitive(state.lastPrimaryHitActor))
      {
        if(state.lastPrimaryHitActor->GetLeaveRequired())
        {
          leaveEventConsumer = DispatchSyntheticTouch(state.lastPrimaryHitActor, lastRenderTask, state.touchEventImpl, PointState::LEAVE);
        }
      }
      else
      {
        leaveEventConsumer = DispatchSyntheticTouch(state.lastPrimaryHitActor, lastRenderTask, state.touchEventImpl, PointState::INTERRUPTED);
      }
    }

    if(state.lastConsumedActor &&
       state.lastConsumedActor != state.consumedActor &&
       state.lastConsumedActor != state.lastPrimaryHitActor &&
       state.lastConsumedActor != state.primaryHitActor &&
       state.lastConsumedActor != leaveEventConsumer)
    {
      if(state.lastConsumedActor->IsHittable() && IsActuallySensitive(state.lastConsumedActor))
      {
        if(state.lastConsumedActor->GetLeaveRequired())
        {
          DispatchSyntheticTouch(state.lastConsumedActor, lastRenderTask, state.touchEventImpl, PointState::LEAVE);
        }
      }
      else
      {
        DispatchSyntheticTouch(state.lastConsumedActor, lastRenderTask, state.touchEventImpl, PointState::INTERRUPTED);
      }
    }
    return static_cast<bool>(leaveEventConsumer);
  }

  static void UpdateMembers(ProcessState& state)
  {
    bool shouldClear = false;
    if(state.primaryPointState == PointState::UP)
    {
      shouldClear = state.processor.mCapturingTouchActor.GetActor() ? state.touchEventImpl->GetPointCount() == 1u : true;
    }

    if(shouldClear)
    {
      state.processor.Clear();
      return;
    }

    if(state.primaryHitActor && GetImplementation(state.primaryHitActor).OnScene())
    {
      state.processor.mLastPrimaryHitActor.SetActor(&GetImplementation(state.primaryHitActor));
      if(state.consumedActor && GetImplementation(state.consumedActor).OnScene())
      {
        state.processor.mLastConsumedActor.SetActor(&GetImplementation(state.consumedActor));
      }
      else
      {
        state.processor.mLastConsumedActor.SetActor(nullptr);
      }
      state.processor.mLastRenderTask        = state.currentRenderTask;
      state.processor.mLastPrimaryPointState = state.primaryPointState;
    }
    else
    {
      state.processor.Clear();
    }
  }

  static void DeliverToTouchDownActorAndScene(ProcessState& state, const Integration::TouchEvent& event)
  {
    if(state.touchEventImpl->GetPointCount() != 1u)
    {
      return;
    }

    if(state.primaryPointState == PointState::UP)
    {
      Actor* touchDownConsumedActor = state.processor.mTouchDownConsumedActor.GetActor();
      if(touchDownConsumedActor &&
         touchDownConsumedActor != state.consumedActor &&
         touchDownConsumedActor != state.lastPrimaryHitActor &&
         touchDownConsumedActor != state.lastConsumedActor)
      {
        Integration::Point currentPoint = state.touchEventImpl->GetPoint(0);
        currentPoint.SetHitActor(Dali::Actor(touchDownConsumedActor));
        currentPoint.SetState(PointState::INTERRUPTED);
        AllocateAndDispatchTouch(event.time, Dali::Actor(touchDownConsumedActor), currentPoint, nullptr);
      }
      state.processor.mTouchDownConsumedActor.SetActor(nullptr);
      state.processor.mInterceptedTouchActor.SetActor(nullptr);
    }

    if(state.primaryPointState == PointState::DOWN || state.primaryPointState == PointState::UP)
    {
      state.processor.mScene.EmitTouchEventSignal(state.touchEventHandle);
    }
  }
};

ParentTouchEventProcessor::ParentTouchEventProcessor(Scene& scene)
: mScene(scene),
  mLastPrimaryHitActor(MakeCallback(this, &ParentTouchEventProcessor::OnObservedActorDisconnected)),
  mLastConsumedActor(),
  mCapturingTouchActor(),
  mOwnTouchActor(),
  mTouchDownConsumedActor(),
  mInterceptedTouchActor(),
  mLastRenderTask(),
  mLastPrimaryPointState(PointState::FINISHED)
{
}

ParentTouchEventProcessor::~ParentTouchEventProcessor() = default;

bool ParentTouchEventProcessor::ProcessTouchEvent(const Integration::TouchEvent& event)
{
  DALI_ASSERT_ALWAYS(!event.points.empty() && "Empty TouchEvent sent from Integration\n");

  ProcessState state(*this);
  if(event.points[0].GetState() == PointState::INTERRUPTED)
  {
    Impl::EmitInterruptedEvent(state, event);
    return false;
  }

  state.touchEventImpl   = new TouchEvent(event.time);
  state.touchEventHandle = Dali::TouchEvent(state.touchEventImpl.Get());

  bool firstPoint = true;
  for(const auto& currentPoint : event.points)
  {
    HitTestAlgorithm::Results hitTestResults;
    hitTestResults.point     = currentPoint;
    hitTestResults.eventTime = event.time;
    if(firstPoint)
    {
      firstPoint = false;
      ParsePrimaryPoint(hitTestResults, mCapturingTouchActor, mOwnTouchActor, mLastRenderTask, currentPoint, mScene);
      state.currentRenderTask = hitTestResults.renderTask;
      state.touchEventImpl->SetRenderTask(Dali::RenderTask(state.currentRenderTask.Get()));
    }
    else
    {
      Actor* capturingTouchActor = mCapturingTouchActor.GetActor();
      if(capturingTouchActor && mLastRenderTask)
      {
        hitTestResults.actor          = Dali::Actor(capturingTouchActor);
        hitTestResults.renderTask     = mLastRenderTask;
        const Vector2& screenPosition = currentPoint.GetScreenPosition();
        capturingTouchActor->ScreenToLocal(*mLastRenderTask, hitTestResults.actorCoordinates.x, hitTestResults.actorCoordinates.y, screenPosition.x, screenPosition.y);
      }
      else
      {
        HitTestAlgorithm::HitTest(mScene.GetSize(), mScene.GetRenderTaskList(), mScene.GetLayerList(), currentPoint.GetScreenPosition(), hitTestResults, nullptr, Integration::Scene::TouchPropagationType::PARENT);
      }
    }

    Integration::Point newPoint(currentPoint);
    newPoint.SetHitActor(hitTestResults.actor);
    newPoint.SetLocalPosition(hitTestResults.actorCoordinates);
    state.touchEventImpl->AddPoint(newPoint);
  }

  state.primaryPoint      = &state.touchEventImpl->GetPoint(0);
  state.primaryHitActor   = state.primaryPoint->GetHitActor();
  state.primaryPointState = state.primaryPoint->GetState();

  bool consumed             = Impl::DeliverToHierarchy(state);
  state.lastPrimaryHitActor = mInterceptedTouchActor.GetActor() ? mInterceptedTouchActor.GetActor() : mLastPrimaryHitActor.GetActor();
  state.lastConsumedActor   = mLastConsumedActor.GetActor();
  consumed |= Impl::DeliverLeaveEvent(state);
  Impl::UpdateMembers(state);
  Impl::DeliverToTouchDownActorAndScene(state, event);
  return consumed;
}

void ParentTouchEventProcessor::OnObservedActorDisconnected(Actor* actor)
{
  if(actor != mLastPrimaryHitActor.GetActor())
  {
    return;
  }

  Integration::Point point;
  point.SetState(PointState::INTERRUPTED);
  point.SetHitActor(Dali::Actor(actor));

  TouchEventPtr touchEvent(new TouchEvent);
  touchEvent->AddPoint(point);
  Dali::TouchEvent touchEventHandle(touchEvent.Get());

  Dali::Actor eventConsumer = DispatchTouchToHierarchy(Dali::Actor(actor), touchEventHandle);
  if(mLastConsumedActor.GetActor() != eventConsumer)
  {
    DispatchTouchToHierarchy(Dali::Actor(mLastConsumedActor.GetActor()), touchEventHandle);
  }

  mLastConsumedActor.SetActor(nullptr);
  mLastRenderTask.Reset();
  mLastPrimaryPointState = PointState::FINISHED;
}

void ParentTouchEventProcessor::Clear()
{
  mLastPrimaryHitActor.SetActor(nullptr);
  mLastConsumedActor.SetActor(nullptr);
  mCapturingTouchActor.SetActor(nullptr);
  mOwnTouchActor.SetActor(nullptr);
  mInterceptedTouchActor.SetActor(nullptr);
  mLastRenderTask.Reset();
  mLastPrimaryPointState = PointState::FINISHED;
}

} //namespace DALI_NAMESPACE::Internal
