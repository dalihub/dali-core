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
#include <dali/internal/event/events/touch-event-processor.h>

#if defined(DEBUG_ENABLED)
#include <sstream>
#endif

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/trace.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/events/hit-test-algorithm-impl.h>
#include <dali/internal/event/events/multi-point-event-util.h>
#include <dali/internal/event/events/touch-event-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/signals/callback.h>

namespace Dali
{
namespace Internal
{
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_PERFORMANCE_MARKER, false);
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_TOUCH_PROCESSOR");
#endif // defined(DEBUG_ENABLED)

const char* TOUCH_POINT_STATE[6] =
  {
    "DOWN",
    "UP",
    "MOTION",
    "LEAVE",
    "STATIONARY",
    "INTERRUPTED",
};

bool ShouldEmitInterceptTouchEvent(const Actor& actorImpl, const Dali::TouchEvent& event)
{
  PointState::Type state = event.GetState(0);
  return actorImpl.GetInterceptTouchRequired() && (state!= PointState::MOTION || actorImpl.IsDispatchTouchMotion());
}

bool ShouldEmitTouchEvent(const Actor& actorImpl, const Dali::TouchEvent& event)
{
  PointState::Type state = event.GetState(0);
  return actorImpl.GetTouchRequired() && (state!= PointState::MOTION || actorImpl.IsDispatchTouchMotion());
}

Dali::Actor EmitInterceptTouchSignals(Dali::Actor actor, const Dali::TouchEvent& touchEvent)
{
  Dali::Actor interceptedActor;

  if(actor)
  {
    Dali::Actor parent = actor.GetParent();
    if(parent)
    {
      // Recursively deliver events to the actor and its parents for intercept touch event.
      interceptedActor = EmitInterceptTouchSignals(parent, touchEvent);
    }

    if(!interceptedActor)
    {
      bool   intercepted = false;
      Actor& actorImpl(GetImplementation(actor));
      if(ShouldEmitInterceptTouchEvent(actorImpl, touchEvent))
      {
        DALI_TRACE_SCOPE(gTraceFilter, "DALI_EMIT_INTERCEPT_TOUCH_EVENT_SIGNAL");
        intercepted = actorImpl.EmitInterceptTouchEventSignal(touchEvent);
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
Dali::Actor EmitTouchSignals(Dali::Actor actor, const Dali::TouchEvent& touchEvent)
{
  Dali::Actor consumedActor;

  if(actor)
  {
    Dali::Actor oldParent(actor.GetParent());

    Actor& actorImpl(GetImplementation(actor));

    bool consumed(false);

    // Only emit the signal if the actor's touch signal has connections (or derived actor implementation requires touch).
    if(ShouldEmitTouchEvent(actorImpl, touchEvent))
    {
      DALI_TRACE_SCOPE(gTraceFilter, "DALI_EMIT_TOUCH_EVENT_SIGNAL");
      consumed = actorImpl.EmitTouchEventSignal(touchEvent);
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
        consumedActor = EmitTouchSignals(parent, touchEvent);
      }
    }
  }

  return consumedActor;
}

Dali::Actor AllocAndEmitTouchSignals(unsigned long time, Dali::Actor actor, const Integration::Point& point)
{
  TouchEventPtr    touchEvent(new TouchEvent(time));
  Dali::TouchEvent touchEventHandle(touchEvent.Get());

  touchEvent->AddPoint(point);

  return EmitTouchSignals(actor, touchEventHandle);
}

/**
 * Changes the state of the primary point to leave and emits the touch signals
 */
Dali::Actor EmitTouchSignals(Actor* actor, RenderTask& renderTask, const TouchEventPtr& originalTouchEvent, PointState::Type state)
{
  Dali::Actor consumingActor;

  if(actor)
  {
    TouchEventPtr touchEventImpl = TouchEvent::Clone(*originalTouchEvent.Get());

    Integration::Point& primaryPoint = touchEventImpl->GetPoint(0);

    const Vector2& screenPosition = primaryPoint.GetScreenPosition();
    Vector2        localPosition;
    actor->ScreenToLocal(renderTask, localPosition.x, localPosition.y, screenPosition.x, screenPosition.y);

    primaryPoint.SetLocalPosition(localPosition);
    primaryPoint.SetHitActor(Dali::Actor(actor));
    primaryPoint.SetState(state);

    consumingActor = EmitTouchSignals(Dali::Actor(actor), Dali::TouchEvent(touchEventImpl.Get()));
  }

  return consumingActor;
}

/**
 * @brief Parses the primary touch point by performing a hit-test if necessary
 *
 * @param[out] hitTestResults The hit test results are put into this variable
 * @param[in/out] capturingTouchActorObserver The observer for the capturing touch actor member
 * @param[in] lastRenderTask The last render task member
 * @param[in] currentPoint The current point information
 * @param[in] scene The scene that this touch is related to
 */
void ParsePrimaryTouchPoint(
  HitTestAlgorithm::Results& hitTestResults,
  ActorObserver&             capturingTouchActorObserver,
  ActorObserver&             ownTouchActorObserver,
  const RenderTaskPtr&       lastRenderTask,
  const Integration::Point&  currentPoint,
  const Internal::Scene&     scene)
{
  Actor* capturingTouchActor = capturingTouchActorObserver.GetActor();

  // We only set the capturing touch actor when the first touch-started actor captures all touch so if it's set, just use it
  if(capturingTouchActor && lastRenderTask)
  {
    hitTestResults.actor          = Dali::Actor(capturingTouchActor);
    hitTestResults.renderTask     = lastRenderTask;
    const Vector2& screenPosition = currentPoint.GetScreenPosition();
    capturingTouchActor->ScreenToLocal(*lastRenderTask, hitTestResults.actorCoordinates.x, hitTestResults.actorCoordinates.y, screenPosition.x, screenPosition.y);
  }
  else
  {
    Actor* ownTouchActor = ownTouchActorObserver.GetActor();
    HitTestAlgorithm::HitTest(scene.GetSize(), scene.GetRenderTaskList(), scene.GetLayerList(), currentPoint.GetScreenPosition(), hitTestResults, ownTouchActor);

    if(currentPoint.GetState() == PointState::STARTED && hitTestResults.actor)
    {
      // If we've just started touch, then check whether the actor has requested to capture all touch events
      Actor* hitActor = &GetImplementation(hitTestResults.actor);
      if(hitActor->CapturesAllTouchAfterStart())
      {
        capturingTouchActorObserver.SetActor(hitActor);
      }
      if(hitActor->IsAllowedOnlyOwnTouch())
      {
        ownTouchActorObserver.SetActor(hitActor);
      }
    }
  }
}

} // unnamed namespace

TouchEventProcessor::TouchEventProcessor(Scene& scene)
: mScene(scene),
  mLastPrimaryHitActor(MakeCallback(this, &TouchEventProcessor::OnObservedActorDisconnected)),
  mLastConsumedActor(),
  mCapturingTouchActor(),
  mOwnTouchActor(),
  mTouchDownConsumedActor(),
  mInterceptedTouchActor(),
  mLastRenderTask(),
  mLastPrimaryPointState(PointState::FINISHED)
{
  DALI_LOG_TRACE_METHOD(gLogFilter);
}

TouchEventProcessor::~TouchEventProcessor()
{
  DALI_LOG_TRACE_METHOD(gLogFilter);
}

void TouchEventProcessor::Clear()
{
  mLastPrimaryHitActor.SetActor(nullptr);
  mLastConsumedActor.SetActor(nullptr);
  mCapturingTouchActor.SetActor(nullptr);
  mOwnTouchActor.SetActor(nullptr);
  mInterceptedTouchActor.SetActor(nullptr);
  mLastRenderTask.Reset();
  mLastPrimaryPointState = PointState::FINISHED;
}

bool TouchEventProcessor::ProcessTouchEvent(const Integration::TouchEvent& event)
{
  DALI_LOG_TRACE_METHOD(gLogFilter);
  DALI_ASSERT_ALWAYS(!event.points.empty() && "Empty TouchEvent sent from Integration\n");

  PRINT_HIERARCHY(gLogFilter);

  DALI_TRACE_SCOPE(gTraceFilter, "DALI_PROCESS_TOUCH_EVENT");

  // 1) Check if it is an interrupted event - we should inform our last primary hit actor about this
  //    and emit the stage signal as well.

  if(event.points[0].GetState() == PointState::INTERRUPTED)
  {
    Dali::Actor        consumingActor;
    Integration::Point currentPoint(event.points[0]);

    Actor* lastPrimaryHitActor(mLastPrimaryHitActor.GetActor());
    if(lastPrimaryHitActor)
    {
      Dali::Actor lastPrimaryHitActorHandle(lastPrimaryHitActor);
      currentPoint.SetHitActor(lastPrimaryHitActorHandle);

      consumingActor = AllocAndEmitTouchSignals(event.time, lastPrimaryHitActorHandle, currentPoint);
    }

    // If the last consumed actor was different to the primary hit actor then inform it as well (if it has not already been informed).
    Actor* lastConsumedActor(mLastConsumedActor.GetActor());
    if(lastConsumedActor &&
       lastConsumedActor != lastPrimaryHitActor &&
       lastConsumedActor != consumingActor)
    {
      Dali::Actor lastConsumedActorHandle(lastConsumedActor);
      currentPoint.SetHitActor(lastConsumedActorHandle);
      AllocAndEmitTouchSignals(event.time, lastConsumedActorHandle, currentPoint);
    }

    // Tell the touch-down consuming actor as well, if required
    Actor* touchDownConsumedActor(mTouchDownConsumedActor.GetActor());
    if(touchDownConsumedActor &&
       touchDownConsumedActor != lastPrimaryHitActor &&
       touchDownConsumedActor != lastConsumedActor &&
       touchDownConsumedActor != consumingActor)
    {
      Dali::Actor touchDownConsumedActorHandle(touchDownConsumedActor);

      currentPoint.SetHitActor(touchDownConsumedActorHandle);
      AllocAndEmitTouchSignals(event.time, touchDownConsumedActorHandle, currentPoint);
    }

    Clear();
    mTouchDownConsumedActor.SetActor(nullptr);

    currentPoint.SetHitActor(Dali::Actor());

    TouchEventPtr    touchEventImpl(new TouchEvent(event.time));
    Dali::TouchEvent touchEventHandle(touchEventImpl.Get());

    touchEventImpl->AddPoint(currentPoint);

    mScene.EmitTouchedSignal(touchEventHandle);
    return false; // No need for hit testing & already an interrupted event so just return false
  }

  // 2) Hit Testing.
  TouchEventPtr    touchEventImpl(new TouchEvent(event.time));
  Dali::TouchEvent touchEventHandle(touchEventImpl.Get());

  DALI_LOG_INFO(gLogFilter, Debug::Concise, "\n");
  DALI_LOG_INFO(gLogFilter, Debug::General, "Point(s): %d\n", event.GetPointCount());

  RenderTaskPtr currentRenderTask;
  bool          firstPointParsed = false;

  for(auto&& currentPoint : event.points)
  {
    HitTestAlgorithm::Results hitTestResults;
    hitTestResults.point     = currentPoint;
    hitTestResults.eventTime = event.time;
    if(!firstPointParsed)
    {
      firstPointParsed = true;
      ParsePrimaryTouchPoint(hitTestResults, mCapturingTouchActor, mOwnTouchActor, mLastRenderTask, currentPoint, mScene);

      // Only set the currentRenderTask for the primary hit actor.
      currentRenderTask = hitTestResults.renderTask;
    }
    else
    {
      HitTestAlgorithm::HitTest(mScene.GetSize(), mScene.GetRenderTaskList(), mScene.GetLayerList(), currentPoint.GetScreenPosition(), hitTestResults);
    }

    Integration::Point newPoint(currentPoint);
    newPoint.SetHitActor(hitTestResults.actor);
    newPoint.SetLocalPosition(hitTestResults.actorCoordinates);

    touchEventImpl->AddPoint(newPoint);

    DALI_LOG_INFO(gLogFilter, Debug::General, "  State(%s), Screen(%.0f, %.0f), HitActor(%p, %s), Local(%.2f, %.2f)\n", TOUCH_POINT_STATE[currentPoint.GetState()], currentPoint.GetScreenPosition().x, currentPoint.GetScreenPosition().y, (hitTestResults.actor ? reinterpret_cast<void*>(&hitTestResults.actor.GetBaseObject()) : NULL), (hitTestResults.actor ? hitTestResults.actor.GetProperty<std::string>(Dali::Actor::Property::NAME).c_str() : ""), hitTestResults.actorCoordinates.x, hitTestResults.actorCoordinates.y);
  }

  // 3) Recursively deliver events to the actor and its parents, until the event is consumed or the stage is reached.

  bool consumed = false;

  // Emit the touch signal
  Dali::Actor consumedActor;

  Integration::Point& primaryPoint      = touchEventImpl->GetPoint(0);
  Dali::Actor         primaryHitActor   = primaryPoint.GetHitActor();
  PointState::Type    primaryPointState = primaryPoint.GetState();

  if(currentRenderTask)
  {
    Actor* interceptedTouchActor(mInterceptedTouchActor.GetActor());
    if(interceptedTouchActor)
    {
      Dali::Actor interceptedTouchActorHandle(interceptedTouchActor);
      consumedActor = EmitTouchSignals(interceptedTouchActorHandle, touchEventHandle);
    }
    else
    {
      // Emit the intercept touch signal
      Dali::Actor interceptedActor = EmitInterceptTouchSignals(primaryHitActor, touchEventHandle);
      if(interceptedActor)
      {
        mInterceptedTouchActor.SetActor(&GetImplementation(interceptedActor));
        // If the child is being touched, INTERRUPTED is sent.
        if(mLastPrimaryHitActor.GetActor() &&
          mLastPrimaryHitActor.GetActor() != interceptedActor &&
          mLastRenderTask &&
          mLastPrimaryPointState != PointState::FINISHED)
        {
          EmitTouchSignals(mLastPrimaryHitActor.GetActor(), *mLastRenderTask.Get(), touchEventImpl, PointState::INTERRUPTED);
          mTouchDownConsumedActor.SetActor(nullptr);
        }
        consumedActor = EmitTouchSignals(interceptedActor, touchEventHandle);
      }
      else
      {
        consumedActor = EmitTouchSignals(primaryHitActor, touchEventHandle);
      }
    }
    consumed = consumedActor ? true : false;

    if(primaryPointState == PointState::MOTION)
    {
      DALI_LOG_INFO(gLogFilter, Debug::Concise, "PrimaryHitActor: (%p) id(%d), name(%s), state(%s), screenPosition(%f, %f)\n", primaryHitActor ? reinterpret_cast<void*>(&primaryHitActor.GetBaseObject()) : NULL, primaryHitActor ? primaryHitActor.GetProperty<int32_t>(Dali::Actor::Property::ID) : -1, primaryHitActor ? primaryHitActor.GetProperty<std::string>(Dali::Actor::Property::NAME).c_str() : "", TOUCH_POINT_STATE[primaryPointState], primaryPoint.GetScreenPosition().x, primaryPoint.GetScreenPosition().y);
      DALI_LOG_INFO(gLogFilter, Debug::Concise, "ConsumedActor:   (%p) id(%d), name(%s), state(%s)\n", consumedActor ? reinterpret_cast<void*>(&consumedActor.GetBaseObject()) : NULL, consumedActor ? consumedActor.GetProperty<int32_t>(Dali::Actor::Property::ID) : -1, consumedActor ? consumedActor.GetProperty<std::string>(Dali::Actor::Property::NAME).c_str() : "", TOUCH_POINT_STATE[primaryPointState]);
    }
    else
    {
      DALI_LOG_RELEASE_INFO("PrimaryHitActor:(%p), id(%d), name(%s), state(%s)\n", primaryHitActor ? reinterpret_cast<void*>(&primaryHitActor.GetBaseObject()) : NULL, primaryHitActor ? primaryHitActor.GetProperty<int32_t>(Dali::Actor::Property::ID) : -1, primaryHitActor ? primaryHitActor.GetProperty<std::string>(Dali::Actor::Property::NAME).c_str() : "", TOUCH_POINT_STATE[primaryPointState]);
      DALI_LOG_RELEASE_INFO("ConsumedActor:  (%p), id(%d), name(%s), state(%s)\n", consumedActor ? reinterpret_cast<void*>(&consumedActor.GetBaseObject()) : NULL, consumedActor ? consumedActor.GetProperty<int32_t>(Dali::Actor::Property::ID) : -1, consumedActor ? consumedActor.GetProperty<std::string>(Dali::Actor::Property::NAME).c_str() : "", TOUCH_POINT_STATE[primaryPointState]);
    }
  }

  if((primaryPointState == PointState::DOWN) &&
     (touchEventImpl->GetPointCount() == 1) &&
     (consumedActor && consumedActor.GetProperty<bool>(Dali::Actor::Property::CONNECTED_TO_SCENE)))
  {
    mTouchDownConsumedActor.SetActor(&GetImplementation(consumedActor));
  }

  // 4) Check if the last primary hit actor requires a leave event and if it was different to the current primary
  //    hit actor.  Also process the last consumed actor in the same manner.
  Actor* lastPrimaryHitActor(nullptr);
  if(mInterceptedTouchActor.GetActor())
  {
    lastPrimaryHitActor = mInterceptedTouchActor.GetActor();
  }
  else
  {
    lastPrimaryHitActor = mLastPrimaryHitActor.GetActor();
  }
  Actor* lastConsumedActor(mLastConsumedActor.GetActor());
  if((primaryPointState == PointState::MOTION) || (primaryPointState == PointState::UP) || (primaryPointState == PointState::STATIONARY))
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
          if(lastPrimaryHitActor->GetLeaveRequired())
          {
            DALI_LOG_RELEASE_INFO("LeaveActor(Hit): (%p) %d %s\n", reinterpret_cast<void*>(lastPrimaryHitActor), lastPrimaryHitActor->GetId(), lastPrimaryHitActor->GetName().data());
            leaveEventConsumer = EmitTouchSignals(lastPrimaryHitActor, lastRenderTaskImpl, touchEventImpl, PointState::LEAVE);
          }
        }
        else
        {
          // At this point mLastPrimaryHitActor was touchable and sensitive in the previous touch event process but is not in the current one.
          // An interrupted event is send to allow some actors to go back to their original state (i.e. Button controls)
          DALI_LOG_RELEASE_INFO("InterruptedActor(Hit): (%p) %d %s\n", reinterpret_cast<void*>(lastPrimaryHitActor), lastPrimaryHitActor->GetId(), lastPrimaryHitActor->GetName().data());
          leaveEventConsumer = EmitTouchSignals(lastPrimaryHitActor, lastRenderTaskImpl, touchEventImpl, PointState::INTERRUPTED);
        }
      }

      consumed |= leaveEventConsumer ? true : false;

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
          if(lastConsumedActor->GetLeaveRequired())
          {
            DALI_LOG_RELEASE_INFO("LeaveActor(Consume): (%p) %d %s\n", reinterpret_cast<void*>(lastConsumedActor), lastConsumedActor->GetId(), lastConsumedActor->GetName().data());
            EmitTouchSignals(lastConsumedActor, lastRenderTaskImpl, touchEventImpl, PointState::LEAVE);
          }
        }
        else
        {
          // At this point mLastConsumedActor was touchable and sensitive in the previous touch event process but is not in the current one.
          // An interrupted event is send to allow some actors to go back to their original state (i.e. Button controls)
          DALI_LOG_RELEASE_INFO("InterruptedActor(Consume): (%p) %d %s\n", reinterpret_cast<void*>(lastConsumedActor), lastConsumedActor->GetId(), lastConsumedActor->GetName().data());
          EmitTouchSignals(mLastConsumedActor.GetActor(), lastRenderTaskImpl, touchEventImpl, PointState::INTERRUPTED);
        }
      }
    }
  }

  // 5) If our primary point is an Up event, then the primary point (in multi-touch) will change next
  //    time so set our last primary actor to NULL.  Do the same to the last consumed actor as well.
  if(primaryPointState == PointState::UP)
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

      mLastRenderTask        = currentRenderTask;
      mLastPrimaryPointState = primaryPointState;
    }
    else
    {
      Clear();
    }
  }

  // 6) Emit an interrupted event to the touch-down actor if it hasn't consumed the up and
  //    emit the stage touched event if required.

  if(touchEventImpl->GetPointCount() == 1) // Only want the first touch and the last release
  {
    switch(primaryPointState)
    {
      case PointState::UP:
      {
        Actor* touchDownConsumedActor(mTouchDownConsumedActor.GetActor());
        if(touchDownConsumedActor &&
           touchDownConsumedActor != consumedActor &&
           touchDownConsumedActor != lastPrimaryHitActor &&
           touchDownConsumedActor != lastConsumedActor)
        {
          Dali::Actor touchDownConsumedActorHandle(touchDownConsumedActor);

          Integration::Point currentPoint = touchEventImpl->GetPoint(0);
          currentPoint.SetHitActor(touchDownConsumedActorHandle);
          currentPoint.SetState(PointState::INTERRUPTED);

          AllocAndEmitTouchSignals(event.time, touchDownConsumedActorHandle, currentPoint);
        }

        mTouchDownConsumedActor.SetActor(nullptr);
        mInterceptedTouchActor.SetActor(nullptr);

        DALI_FALLTHROUGH;
      }

      case PointState::DOWN:
      {
        mScene.EmitTouchedSignal(touchEventHandle);
        break;
      }

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

  return consumed;
}

void TouchEventProcessor::OnObservedActorDisconnected(Actor* actor)
{
  if(actor == mLastPrimaryHitActor.GetActor())
  {
    Dali::Actor actorHandle(actor);

    Integration::Point point;
    point.SetState(PointState::INTERRUPTED);
    point.SetHitActor(actorHandle);

    TouchEventPtr touchEventImpl(new TouchEvent);
    touchEventImpl->AddPoint(point);
    Dali::TouchEvent touchEventHandle(touchEventImpl.Get());

    Dali::Actor eventConsumer = EmitTouchSignals(actorHandle, touchEventHandle);

    if(mLastConsumedActor.GetActor() != eventConsumer)
    {
      EmitTouchSignals(Dali::Actor(mLastConsumedActor.GetActor()), touchEventHandle);
    }

    // Do not set mLastPrimaryHitActor to NULL we may be iterating through its observers

    mLastConsumedActor.SetActor(nullptr);
    mLastRenderTask.Reset();
    mLastPrimaryPointState = PointState::FINISHED;
  }
}

} // namespace Internal

} // namespace Dali
