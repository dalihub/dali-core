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

namespace Dali::Internal
{
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_PERFORMANCE_MARKER, false);
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_TOUCH_PROCESSOR");
#endif // defined(DEBUG_ENABLED)

/**
 * Structure for Variables used in the ProcessTouchEvent method.
 */
struct ProcessTouchEventVariables
{
  ProcessTouchEventVariables(TouchEventProcessor& eventProcessor, bool geometry)
  : processor(eventProcessor),
    isGeometry(geometry)
  {
  }

  TouchEventProcessor& processor;                              ///< A handle to the touch-event-processor.
  const bool           isGeometry;                             ///< Whether it's a geometry or not.
  Actor*               lastPrimaryHitActor{nullptr};           ///< The last primary hit-actor.
  Actor*               lastConsumedActor{nullptr};             ///< The last consuming actor.
  TouchEventPtr        touchEventImpl;                         ///< The current touch-event-impl.
  Dali::TouchEvent     touchEventHandle;                       ///< The handle to the touch-event-impl.
  RenderTaskPtr        currentRenderTask;                      ///< The current render-task.
  Dali::Actor          consumedActor;                          ///< The actor that consumed the event.
  Dali::Actor          primaryHitActor;                        ///< The actor that has been hit by the primary point.
  Integration::Point*  primaryPoint{nullptr};                  ///< The primary point of the hit.
  PointState::Type     primaryPointState{PointState::STARTED}; ///< The state of the primary point.
};

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
  return actorImpl.GetInterceptTouchRequired() && (actorImpl.IsHittable() || state == PointState::INTERRUPTED) && (state != PointState::MOTION || actorImpl.IsDispatchTouchMotion());
}

bool ShouldEmitTouchEvent(const Actor& actorImpl, const Dali::TouchEvent& event)
{
  PointState::Type state = event.GetState(0);
  return actorImpl.GetTouchRequired() && (actorImpl.IsHittable() || state == PointState::INTERRUPTED) && (state != PointState::MOTION || actorImpl.IsDispatchTouchMotion());
}

// child -> parent
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

// geometry
// child -> parent
Dali::Actor EmitGeoInterceptTouchSignals(Dali::Actor             actor,
                                         const Dali::TouchEvent& touchEvent,
                                         ActorObserver&          lastConsumedActor)
{
  Dali::Actor interceptedActor;

  if(actor)
  {
    Dali::Actor parent = actor.GetParent();
    if(parent)
    {
      // Recursively deliver events to the actor and its parents for intercept touch event.
      interceptedActor = EmitGeoInterceptTouchSignals(parent, touchEvent, lastConsumedActor);
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
      // If there is a consumed actor, the intercept is sent only up to the moment before the consumed actor.
      if(lastConsumedActor.GetActor() == actor)
      {
        return interceptedActor;
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

/**
 *  Recursively deliver events to the actor and its below actor, until the event is consumed or the stage is reached.
 */
Dali::Actor EmitGeoTouchSignalsWithTracking(std::list<Dali::Internal::Actor*>& actorLists, std::list<Dali::Internal::Actor*>& trackingLists, const Dali::TouchEvent& touchEvent)
{
  Dali::Actor consumedActor;

  std::list<Dali::Internal::Actor*>::reverse_iterator rIter = actorLists.rbegin();
  for(; rIter != actorLists.rend(); rIter++)
  {
    Actor* actorImpl(*rIter);
    // Only emit the signal if the actor's touch signal has connections (or derived actor implementation requires touch).
    if(ShouldEmitTouchEvent(*actorImpl, touchEvent))
    {
      // Check if actor is already in tracking list to avoid duplicates
      if(std::find(trackingLists.begin(), trackingLists.end(), actorImpl) == trackingLists.end())
      {
        trackingLists.push_back(actorImpl);
      }

      DALI_TRACE_SCOPE(gTraceFilter, "DALI_EMIT_TOUCH_EVENT_SIGNAL");
      if(actorImpl->EmitTouchEventSignal(touchEvent))
      {
        // One of this actor's listeners has consumed the event so set this actor as the consumed actor.
        consumedActor = Dali::Actor(actorImpl);
        break;
      }
    }
  }
  return consumedActor;
}

/**
 *  Recursively deliver events to the actor and its below actor, until the event is consumed or the stage is reached.
 */
Dali::Actor EmitGeoTouchSignals(std::list<Dali::Internal::Actor*>& actorLists, const Dali::TouchEvent& touchEvent)
{
  Dali::Actor consumedActor;

  std::list<Dali::Internal::Actor*>::reverse_iterator rIter = actorLists.rbegin();
  for(; rIter != actorLists.rend(); rIter++)
  {
    Actor* actorImpl(*rIter);
    // Only emit the signal if the actor's touch signal has connections (or derived actor implementation requires touch).
    if(ShouldEmitTouchEvent(*actorImpl, touchEvent))
    {
      DALI_TRACE_SCOPE(gTraceFilter, "DALI_EMIT_TOUCH_EVENT_SIGNAL");
      if(actorImpl->EmitTouchEventSignal(touchEvent))
      {
        // One of this actor's listeners has consumed the event so set this actor as the consumed actor.
        consumedActor = Dali::Actor(actorImpl);
        break;
      }
    }
  }
  return consumedActor;
}

Dali::Actor AllocAndEmitTouchSignals(unsigned long time, Dali::Actor actor, const Integration::Point& point, RenderTaskPtr renderTask)
{
  TouchEventPtr    touchEvent(new TouchEvent(time));
  Dali::TouchEvent touchEventHandle(touchEvent.Get());

  touchEvent->AddPoint(point);
  touchEvent->SetRenderTask(Dali::RenderTask(renderTask.Get()));

  return EmitTouchSignals(actor, touchEventHandle);
}

Dali::Actor GeoAllocAndEmitTouchSignals(std::list<Dali::Internal::Actor*>& actorLists, unsigned long time, const Integration::Point& point, RenderTaskPtr renderTask)
{
  TouchEventPtr    touchEvent(new TouchEvent(time));
  Dali::TouchEvent touchEventHandle(touchEvent.Get());

  touchEvent->AddPoint(point);
  touchEvent->SetRenderTask(Dali::RenderTask(renderTask.Get()));

  return EmitGeoTouchSignals(actorLists, touchEventHandle);
}

/**
 * Changes the state of the primary point to leave and emits the touch signals
 */
Dali::Actor EmitTouchSignals(Actor* actor, RenderTask& renderTask, const TouchEventPtr& originalTouchEvent, PointState::Type state, bool isGeometry)
{
  Dali::Actor consumingActor;

  if(actor)
  {
    TouchEventPtr touchEventImpl = TouchEvent::Clone(*originalTouchEvent.Get());
    touchEventImpl->SetRenderTask(Dali::RenderTask(&renderTask));

    Integration::Point& primaryPoint = touchEventImpl->GetPoint(0);

    const Vector2& screenPosition = primaryPoint.GetScreenPosition();
    Vector2        localPosition;
    actor->ScreenToLocal(renderTask, localPosition.x, localPosition.y, screenPosition.x, screenPosition.y);

    primaryPoint.SetLocalPosition(localPosition);
    primaryPoint.SetHitActor(Dali::Actor(actor));
    primaryPoint.SetState(state);

    if(isGeometry)
    {
      std::list<Dali::Internal::Actor*> actorLists;
      actorLists.push_back(actor);
      consumingActor = EmitGeoTouchSignals(actorLists, Dali::TouchEvent(touchEventImpl.Get()));
    }
    else
    {
      consumingActor = EmitTouchSignals(Dali::Actor(actor), Dali::TouchEvent(touchEventImpl.Get()));
    }
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
 * @param[in] actorLists The list of actors that can be touched, from leaf actor to root.
 */
void ParsePrimaryTouchPoint(
  HitTestAlgorithm::Results&         hitTestResults,
  ActorObserver&                     capturingTouchActorObserver,
  ActorObserver&                     ownTouchActorObserver,
  const RenderTaskPtr&               lastRenderTask,
  const Integration::Point&          currentPoint,
  const Internal::Scene&             scene,
  std::list<Dali::Internal::Actor*>& actorLists)
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
    HitTestAlgorithm::HitTest(scene.GetSize(), scene.GetRenderTaskList(), scene.GetLayerList(), currentPoint.GetScreenPosition(), hitTestResults, ownTouchActor, scene.GetTouchPropagationType());

    if(currentPoint.GetState() == PointState::STARTED && hitTestResults.actor)
    {
      bool isGeometry = scene.IsGeometryHittestEnabled();
      // If we've just started touch, then check whether the actor has requested to capture all touch events
      Actor* hitActor = &GetImplementation(hitTestResults.actor);
      if(hitActor->CapturesAllTouchAfterStart() || isGeometry)
      {
        capturingTouchActorObserver.SetActor(hitActor);
      }
      if(hitActor->IsAllowedOnlyOwnTouch() || isGeometry)
      {
        ownTouchActorObserver.SetActor(hitActor);
      }
      if(isGeometry)
      {
        actorLists = hitTestResults.actorLists;
      }
    }
  }
}

} // unnamed namespace

struct TouchEventProcessor::Impl
{
  /**
   * Emits an interrupted event.
   * @param[in/out] localVars The struct of stack variables used by ProcessTouchEvent
   * @param[in] event The touch event that has occurred
   */
  static inline void EmitInterruptedEvent(ProcessTouchEventVariables& localVars, const Integration::TouchEvent& event)
  {
    Dali::Actor        consumingActor;
    Integration::Point currentPoint(event.points[0]);

    if(localVars.isGeometry)
    {
      // Since the geometry way only receives touch events from the consumed actor,
      // it first searches for whether there is a consumed actor and then sends the event
      Actor* touchConsumedActor(localVars.processor.mLastConsumedActor.GetActor());
      Actor* touchDownConsumedActor(localVars.processor.mTouchDownConsumedActor.GetActor());
      Actor* lastPrimaryHitActor(localVars.processor.mLastPrimaryHitActor.GetActor());
      if(touchConsumedActor)
      {
        Dali::Actor touchConsumedActorHandle(touchConsumedActor);
        currentPoint.SetHitActor(touchConsumedActorHandle);
        std::list<Dali::Internal::Actor*> actorLists;
        actorLists.push_back(touchConsumedActor);
        GeoAllocAndEmitTouchSignals(actorLists, event.time, currentPoint, localVars.processor.mLastRenderTask);
      }
      else if(touchDownConsumedActor)
      {
        Dali::Actor touchDownConsumedActorHandle(touchDownConsumedActor);
        currentPoint.SetHitActor(touchDownConsumedActorHandle);
        std::list<Dali::Internal::Actor*> actorLists;
        actorLists.push_back(touchDownConsumedActor);
        GeoAllocAndEmitTouchSignals(actorLists, event.time, currentPoint, localVars.processor.mLastRenderTask);
      }
      else if(lastPrimaryHitActor)
      {
        Dali::Actor lastPrimaryHitActorHandle(lastPrimaryHitActor);
        currentPoint.SetHitActor(lastPrimaryHitActorHandle);

        GeoAllocAndEmitTouchSignals(localVars.processor.mCandidateActorLists, event.time, currentPoint, localVars.processor.mLastRenderTask);
      }
    }
    else
    {
      Actor* lastPrimaryHitActor(localVars.processor.mLastPrimaryHitActor.GetActor());
      if(lastPrimaryHitActor)
      {
        Dali::Actor lastPrimaryHitActorHandle(lastPrimaryHitActor);
        currentPoint.SetHitActor(lastPrimaryHitActorHandle);

        consumingActor = AllocAndEmitTouchSignals(event.time, lastPrimaryHitActorHandle, currentPoint, localVars.processor.mLastRenderTask);
      }

      // If the last consumed actor was different to the primary hit actor then inform it as well (if it has not already been informed).
      Actor* lastConsumedActor(localVars.processor.mLastConsumedActor.GetActor());
      if(lastConsumedActor &&
         lastConsumedActor != lastPrimaryHitActor &&
         lastConsumedActor != consumingActor)
      {
        Dali::Actor lastConsumedActorHandle(lastConsumedActor);
        currentPoint.SetHitActor(lastConsumedActorHandle);
        AllocAndEmitTouchSignals(event.time, lastConsumedActorHandle, currentPoint, localVars.processor.mLastRenderTask);
      }

      // Tell the touch-down consuming actor as well, if required
      Actor* touchDownConsumedActor(localVars.processor.mTouchDownConsumedActor.GetActor());
      if(touchDownConsumedActor &&
         touchDownConsumedActor != lastPrimaryHitActor &&
         touchDownConsumedActor != lastConsumedActor &&
         touchDownConsumedActor != consumingActor)
      {
        Dali::Actor touchDownConsumedActorHandle(touchDownConsumedActor);

        currentPoint.SetHitActor(touchDownConsumedActorHandle);
        AllocAndEmitTouchSignals(event.time, touchDownConsumedActorHandle, currentPoint, localVars.processor.mLastRenderTask);
      }
    }

    localVars.processor.Clear();
    localVars.processor.mTouchDownConsumedActor.SetActor(nullptr);

    currentPoint.SetHitActor(Dali::Actor());

    TouchEventPtr    touchEventImpl(new TouchEvent(event.time));
    Dali::TouchEvent touchEventHandle(touchEventImpl.Get());

    touchEventImpl->AddPoint(currentPoint);

    localVars.processor.mScene.EmitTouchedSignal(touchEventHandle);
  }

  /**
   * Recursively deliver events to the actor and its parents, until the event is consumed or the stage is reached.
   * @param[in/out] localVars The struct of stack variables used by ProcessTouchEvent
   * @return True if consumed, false otherwise.
   */
  static inline bool DeliverEventsToActorAndParents(ProcessTouchEventVariables& localVars)
  {
    bool                 consumed  = false;
    TouchEventProcessor& processor = localVars.processor;
    if(localVars.currentRenderTask)
    {
      if(localVars.isGeometry)
      {
        Actor* interceptedTouchActor(processor.mInterceptedTouchActor.GetActor());
        if(interceptedTouchActor)
        {
          Actor* touchConsumedActor(processor.mLastConsumedActor.GetActor());
          if(touchConsumedActor) // If there is a consumed actor, send events only to the consumed actor.
          {
            RenderTask& currentRenderTaskImpl = *localVars.currentRenderTask.Get();
            localVars.consumedActor           = EmitTouchSignals(touchConsumedActor, currentRenderTaskImpl, localVars.touchEventImpl, localVars.primaryPointState, localVars.isGeometry);
          }
          else // If there is an intercepted actor, send a touch event starting from the intercepted actor.
          {
            Dali::Actor                       interceptedTouchActorHandle(interceptedTouchActor);
            std::list<Dali::Internal::Actor*> interceptActorLists = localVars.processor.mInterceptedActorLists;
            localVars.consumedActor                               = EmitGeoTouchSignals(interceptActorLists, localVars.touchEventHandle);
          }
        }
        else
        {
          Dali::Actor interceptedActor;
          // Let's find out if there is an intercept actor.
          interceptedActor = EmitGeoInterceptTouchSignals(localVars.primaryHitActor, localVars.touchEventHandle, processor.mLastConsumedActor);
          if(interceptedActor)
          {
            processor.mInterceptedTouchActor.SetActor(&GetImplementation(interceptedActor));

            processor.mInterceptedActorLists.clear();
            // Create a list of intercepted actors from the candidate list.
            for(auto& actorImpl : processor.mCandidateActorLists)
            {
              processor.mInterceptedActorLists.push_back(actorImpl);
              if(actorImpl == interceptedActor)
              {
                break;
              }
            }

            // If there is an interception, send an interrupted event to the last consumed actor or to the actor that hit previously.
            if(processor.mLastConsumedActor.GetActor() &&
               processor.mLastConsumedActor.GetActor() != interceptedActor &&
               processor.mLastRenderTask &&
               processor.mLastPrimaryPointState != PointState::FINISHED)
            {
              auto it = std::find(processor.mTrackingActorLists.begin(), processor.mTrackingActorLists.end(), processor.mLastConsumedActor.GetActor());
              if(it != processor.mTrackingActorLists.end())
              {
                processor.mTrackingActorLists.erase(it);
              }
              EmitTouchSignals(processor.mLastConsumedActor.GetActor(), *processor.mLastRenderTask.Get(), localVars.touchEventImpl, PointState::INTERRUPTED, localVars.isGeometry);
              processor.mTouchDownConsumedActor.SetActor(nullptr);
              processor.mLastConsumedActor.SetActor(nullptr);
            }
            else if(processor.mLastPrimaryHitActor.GetActor() &&
                    processor.mLastPrimaryHitActor.GetActor() != interceptedActor &&
                    processor.mLastRenderTask &&
                    processor.mLastPrimaryPointState != PointState::FINISHED)
            {
              std::list<Dali::Internal::Actor*>::reverse_iterator rIter = processor.mCandidateActorLists.rbegin();
              for(; rIter != processor.mCandidateActorLists.rend(); rIter++)
              {
                Actor* actorImpl(*rIter);
                if(actorImpl == interceptedActor)
                {
                  break;
                }
                auto it = std::find(processor.mTrackingActorLists.begin(), processor.mTrackingActorLists.end(), actorImpl);
                if(it != processor.mTrackingActorLists.end())
                {
                  processor.mTrackingActorLists.erase(it);
                }
                EmitTouchSignals(actorImpl, *processor.mLastRenderTask.Get(), localVars.touchEventImpl, PointState::INTERRUPTED, localVars.isGeometry);
              }
            }
          }

          Actor* touchConsumedActor(processor.mLastConsumedActor.GetActor());
          if(touchConsumedActor) // If there is a consumed actor, send events only to the consumed actor.
          {
            RenderTask& currentRenderTaskImpl = *localVars.currentRenderTask.Get();
            localVars.consumedActor           = EmitTouchSignals(touchConsumedActor, currentRenderTaskImpl, localVars.touchEventImpl, localVars.primaryPointState, localVars.isGeometry);
          }
          else
          {
            // Let's propagate touch events from the intercepted actor or start propagating touch events from the leaf actor.
            localVars.consumedActor = EmitGeoTouchSignalsWithTracking(interceptedActor ? processor.mInterceptedActorLists : processor.mCandidateActorLists, processor.mTrackingActorLists, localVars.touchEventHandle);

            if(localVars.consumedActor)
            {
              // If consumed, the actors who previously received the touch are interrupted, indicating that the touch has been consumed by another actor.
              if(processor.mLastRenderTask && localVars.primaryPointState != PointState::DOWN)
              {
                // backward
                std::list<Dali::Internal::Actor*>::reverse_iterator rIter = std::find(processor.mCandidateActorLists.rbegin(), processor.mCandidateActorLists.rend(), localVars.consumedActor);
                if(rIter != processor.mCandidateActorLists.rend())
                {
                  for(++rIter; rIter != processor.mCandidateActorLists.rend(); ++rIter)
                  {
                    Actor* actorImpl(*rIter);
                    auto   it = std::find(processor.mTrackingActorLists.begin(), processor.mTrackingActorLists.end(), actorImpl);
                    if(it != processor.mTrackingActorLists.end())
                    {
                      processor.mTrackingActorLists.erase(it);
                      EmitTouchSignals(actorImpl, *processor.mLastRenderTask.Get(), localVars.touchEventImpl, PointState::INTERRUPTED, localVars.isGeometry);
                    }
                  }
                }
              }
              //forward
              std::list<Dali::Internal::Actor*>::iterator iter = std::find(processor.mCandidateActorLists.begin(), processor.mCandidateActorLists.end(), localVars.consumedActor);
              if(iter != processor.mCandidateActorLists.end())
              {
                for(++iter; iter != processor.mCandidateActorLists.end(); ++iter)
                {
                  Actor* actorImpl(*iter);
                  auto   it = std::find(processor.mTrackingActorLists.begin(), processor.mTrackingActorLists.end(), actorImpl);
                  if(it != processor.mTrackingActorLists.end())
                  {
                    processor.mTrackingActorLists.erase(it);
                    EmitTouchSignals(actorImpl, *localVars.currentRenderTask.Get(), localVars.touchEventImpl, PointState::INTERRUPTED, localVars.isGeometry);
                  }
                }
              }
            }
          }
        }
      }
      else
      {
        Actor* interceptedTouchActor(processor.mInterceptedTouchActor.GetActor());
        if(interceptedTouchActor)
        {
          Dali::Actor interceptedTouchActorHandle(interceptedTouchActor);
          localVars.consumedActor = EmitTouchSignals(interceptedTouchActorHandle, localVars.touchEventHandle);
        }
        else
        {
          // Emit the intercept touch signal
          Dali::Actor interceptedActor = EmitInterceptTouchSignals(localVars.primaryHitActor, localVars.touchEventHandle);
          if(interceptedActor)
          {
            processor.mInterceptedTouchActor.SetActor(&GetImplementation(interceptedActor));
            // If the child is being touched, INTERRUPTED is sent.
            if(processor.mLastPrimaryHitActor.GetActor() &&
               processor.mLastPrimaryHitActor.GetActor() != interceptedActor &&
               processor.mLastRenderTask &&
               processor.mLastPrimaryPointState != PointState::FINISHED)
            {
              EmitTouchSignals(processor.mLastPrimaryHitActor.GetActor(), *processor.mLastRenderTask.Get(), localVars.touchEventImpl, PointState::INTERRUPTED, localVars.isGeometry);
              processor.mTouchDownConsumedActor.SetActor(nullptr);
            }
            localVars.consumedActor = EmitTouchSignals(interceptedActor, localVars.touchEventHandle);
          }
          else
          {
            localVars.consumedActor = EmitTouchSignals(localVars.primaryHitActor, localVars.touchEventHandle);
          }
        }
      }

      consumed = localVars.consumedActor ? true : false;

      if(localVars.primaryPointState == PointState::MOTION)
      {
        DALI_LOG_INFO(gLogFilter,
                      Debug::Concise,
                      "PrimaryHitActor: (%p) id(%d), name(%s), state(%s), screenPosition(%f, %f), isGeo : %d \n",
                      localVars.primaryHitActor ? reinterpret_cast<void*>(&localVars.primaryHitActor.GetBaseObject()) : NULL,
                      localVars.primaryHitActor ? localVars.primaryHitActor.GetProperty<int32_t>(Dali::Actor::Property::ID) : -1,
                      localVars.primaryHitActor ? localVars.primaryHitActor.GetProperty<std::string>(Dali::Actor::Property::NAME).c_str() : "",
                      TOUCH_POINT_STATE[localVars.primaryPointState],
                      localVars.primaryPoint->GetScreenPosition().x,
                      localVars.primaryPoint->GetScreenPosition().y,
                      localVars.isGeometry);
        DALI_LOG_INFO(gLogFilter,
                      Debug::Concise,
                      "ConsumedActor:   (%p) id(%d), name(%s), state(%s), isGeo : %d \n",
                      localVars.consumedActor ? reinterpret_cast<void*>(&localVars.consumedActor.GetBaseObject()) : NULL,
                      localVars.consumedActor ? localVars.consumedActor.GetProperty<int32_t>(Dali::Actor::Property::ID) : -1,
                      localVars.consumedActor ? localVars.consumedActor.GetProperty<std::string>(Dali::Actor::Property::NAME).c_str() : "",
                      TOUCH_POINT_STATE[localVars.primaryPointState],
                      localVars.isGeometry);
      }
      else
      {
        DALI_LOG_RELEASE_INFO("PrimaryHitActor:(%p), id(%d), name(%s), state(%s), screenPosition(%f, %f), isGeo : %d \n",
                              localVars.primaryHitActor ? reinterpret_cast<void*>(&localVars.primaryHitActor.GetBaseObject()) : NULL,
                              localVars.primaryHitActor ? localVars.primaryHitActor.GetProperty<int32_t>(Dali::Actor::Property::ID) : -1,
                              localVars.primaryHitActor ? localVars.primaryHitActor.GetProperty<std::string>(Dali::Actor::Property::NAME).c_str() : "",
                              TOUCH_POINT_STATE[localVars.primaryPointState],
                              localVars.primaryPoint->GetScreenPosition().x,
                              localVars.primaryPoint->GetScreenPosition().y,
                              localVars.isGeometry);
        DALI_LOG_RELEASE_INFO("ConsumedActor:  (%p), id(%d), name(%s), state(%s), isGeo : %d \n",
                              localVars.consumedActor ? reinterpret_cast<void*>(&localVars.consumedActor.GetBaseObject()) : NULL,
                              localVars.consumedActor ? localVars.consumedActor.GetProperty<int32_t>(Dali::Actor::Property::ID) : -1,
                              localVars.consumedActor ? localVars.consumedActor.GetProperty<std::string>(Dali::Actor::Property::NAME).c_str() : "",
                              TOUCH_POINT_STATE[localVars.primaryPointState],
                              localVars.isGeometry);
      }
    }

    if((localVars.primaryPointState == PointState::DOWN) &&
       (localVars.touchEventImpl->GetPointCount() == 1) &&
       (localVars.consumedActor && localVars.consumedActor.GetProperty<bool>(Dali::Actor::Property::CONNECTED_TO_SCENE)))
    {
      processor.mTouchDownConsumedActor.SetActor(&GetImplementation(localVars.consumedActor));
    }

    return consumed;
  }

  /**
   * Deliver Leave event to last hit or consuming actor if required.
   * @param[in/out] localVars The struct of stack variables used by ProcessTouchEvent
   * @return True if consumed, false otherwise.
   */
  static inline bool DeliverLeaveEvent(ProcessTouchEventVariables& localVars)
  {
    bool                 consumed = false;
    TouchEventProcessor& processor(localVars.processor);
    if((localVars.primaryPointState == PointState::MOTION) || (localVars.primaryPointState == PointState::UP) || (localVars.primaryPointState == PointState::STATIONARY))
    {
      if(processor.mLastRenderTask)
      {
        Dali::Actor leaveEventConsumer;
        RenderTask& lastRenderTaskImpl = *processor.mLastRenderTask.Get();

        if(localVars.isGeometry)
        {
          if(localVars.lastPrimaryHitActor)
          {
            if(!localVars.lastPrimaryHitActor->IsHittable() || !IsActuallySensitive(localVars.lastPrimaryHitActor))
            {
              auto it = std::find(processor.mTrackingActorLists.begin(), processor.mTrackingActorLists.end(), localVars.lastPrimaryHitActor);
              if(it != processor.mTrackingActorLists.end())
              {
                // At this point mLastPrimaryHitActor was touchable and sensitive in the previous touch event process but is not in the current one.
                // An interrupted event is send to allow some actors to go back to their original state (i.e. Button controls)
                DALI_LOG_RELEASE_INFO("InterruptedActor(Hit):     (%p) %s\n", reinterpret_cast<void*>(localVars.lastPrimaryHitActor), localVars.lastPrimaryHitActor->GetName().data());
                leaveEventConsumer = EmitTouchSignals(localVars.lastPrimaryHitActor, lastRenderTaskImpl, localVars.touchEventImpl, PointState::INTERRUPTED, localVars.isGeometry);
              }
            }
          }

          consumed = leaveEventConsumer ? true : false;

          // Check if the motion event has been consumed by another actor's listener.  In this case, the previously
          // consumed actor's listeners may need to be informed (through a leave event).
          // Further checks here to ensure we do not signal the same actor twice for the same event.
          if(localVars.lastConsumedActor &&
             localVars.lastConsumedActor != localVars.lastPrimaryHitActor &&
             localVars.lastConsumedActor != leaveEventConsumer)
          {
            if(!localVars.lastConsumedActor->IsHittable() || !IsActuallySensitive(localVars.lastConsumedActor))
            {
              // At this point mLastConsumedActor was touchable and sensitive in the previous touch event process but is not in the current one.
              // An interrupted event is send to allow some actors to go back to their original state (i.e. Button controls)
              DALI_LOG_RELEASE_INFO("InterruptedActor(Consume):     (%p) %s\n", reinterpret_cast<void*>(localVars.lastConsumedActor), localVars.lastConsumedActor->GetName().data());
              EmitTouchSignals(processor.mLastConsumedActor.GetActor(), lastRenderTaskImpl, localVars.touchEventImpl, PointState::INTERRUPTED, localVars.isGeometry);
            }
          }
        }
        else
        {
          if(localVars.lastPrimaryHitActor &&
             localVars.lastPrimaryHitActor != localVars.primaryHitActor &&
             localVars.lastPrimaryHitActor != localVars.consumedActor)
          {
            if(localVars.lastPrimaryHitActor->IsHittable() && IsActuallySensitive(localVars.lastPrimaryHitActor))
            {
              if(localVars.lastPrimaryHitActor->GetLeaveRequired())
              {
                DALI_LOG_RELEASE_INFO("LeaveActor(Hit): (%p) %d %s\n", reinterpret_cast<void*>(localVars.lastPrimaryHitActor), localVars.lastPrimaryHitActor->GetId(), localVars.lastPrimaryHitActor->GetName().data());
                leaveEventConsumer = EmitTouchSignals(localVars.lastPrimaryHitActor, lastRenderTaskImpl, localVars.touchEventImpl, PointState::LEAVE, localVars.isGeometry);
              }
            }
            else
            {
              // At this point mLastPrimaryHitActor was touchable and sensitive in the previous touch event process but is not in the current one.
              // An interrupted event is send to allow some actors to go back to their original state (i.e. Button controls)
              DALI_LOG_RELEASE_INFO("InterruptedActor(Hit): (%p) %d %s\n", reinterpret_cast<void*>(localVars.lastPrimaryHitActor), localVars.lastPrimaryHitActor->GetId(), localVars.lastPrimaryHitActor->GetName().data());
              leaveEventConsumer = EmitTouchSignals(localVars.lastPrimaryHitActor, lastRenderTaskImpl, localVars.touchEventImpl, PointState::INTERRUPTED, localVars.isGeometry);
            }
          }

          consumed |= leaveEventConsumer ? true : false;

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
              if(localVars.lastConsumedActor->GetLeaveRequired())
              {
                DALI_LOG_RELEASE_INFO("LeaveActor(Consume): (%p) %d %s\n", reinterpret_cast<void*>(localVars.lastConsumedActor), localVars.lastConsumedActor->GetId(), localVars.lastConsumedActor->GetName().data());
                EmitTouchSignals(localVars.lastConsumedActor, lastRenderTaskImpl, localVars.touchEventImpl, PointState::LEAVE, localVars.isGeometry);
              }
            }
            else
            {
              // At this point mLastConsumedActor was touchable and sensitive in the previous touch event process but is not in the current one.
              // An interrupted event is send to allow some actors to go back to their original state (i.e. Button controls)
              DALI_LOG_RELEASE_INFO("InterruptedActor(Consume): (%p) %d %s\n", reinterpret_cast<void*>(localVars.lastConsumedActor), localVars.lastConsumedActor->GetId(), localVars.lastConsumedActor->GetName().data());
              EmitTouchSignals(processor.mLastConsumedActor.GetActor(), lastRenderTaskImpl, localVars.touchEventImpl, PointState::INTERRUPTED, localVars.isGeometry);
            }
          }
        }
      }
    }
    return consumed;
  }

  /**
   * Update the processor member appropriately by handling the final up event, and setting the last hit/consumed events etc.
   * @param[in/out] localVars The struct of stack variables used by ProcessTouchEvent
   */
  static inline void UpdateMembersWithCurrentHitInformation(ProcessTouchEventVariables& localVars)
  {
    // If our primary point is an Up event, then the primary point (in multi-touch) will change next
    // time so set our last primary actor to NULL.  Do the same to the last consumed actor as well.

    TouchEventProcessor& processor(localVars.processor);
    if(localVars.primaryPointState == PointState::UP)
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
          if(localVars.isGeometry)
          {
            if(localVars.lastConsumedActor && !localVars.lastConsumedActor->OnScene())
            {
              processor.mLastConsumedActor.SetActor(nullptr);
            }
          }
          else
          {
            processor.mLastConsumedActor.SetActor(nullptr);
          }
        }

        processor.mLastRenderTask        = localVars.currentRenderTask;
        processor.mLastPrimaryPointState = localVars.primaryPointState;
      }
      else
      {
        processor.Clear();
      }
    }
  }

  /**
   * Deliver an event to the touch-down actor and emit from the scene if required.
   * @param[in/out] localVars The struct of stack variables used by ProcessTouchEvent
   * @param[in] event The touch event that has occurred
   */
  static inline void DeliverEventToTouchDownActorAndScene(ProcessTouchEventVariables& localVars, const Integration::TouchEvent& event)
  {
    TouchEventProcessor& processor(localVars.processor);
    if(localVars.touchEventImpl->GetPointCount() == 1) // Only want the first touch and the last release
    {
      switch(localVars.primaryPointState)
      {
        case PointState::UP:
        {
          Actor* touchDownConsumedActor(processor.mTouchDownConsumedActor.GetActor());
          if(touchDownConsumedActor &&
             touchDownConsumedActor != localVars.consumedActor &&
             touchDownConsumedActor != localVars.lastPrimaryHitActor &&
             touchDownConsumedActor != localVars.lastConsumedActor)
          {
            Dali::Actor touchDownConsumedActorHandle(touchDownConsumedActor);

            Integration::Point currentPoint = localVars.touchEventImpl->GetPoint(0);
            currentPoint.SetHitActor(touchDownConsumedActorHandle);
            currentPoint.SetState(PointState::INTERRUPTED);

            if(localVars.isGeometry)
            {
              std::list<Dali::Internal::Actor*> actorLists;
              actorLists.push_back(touchDownConsumedActor);
              GeoAllocAndEmitTouchSignals(actorLists, event.time, currentPoint, nullptr /* Not Required for this use case */);
            }
            else
            {
              AllocAndEmitTouchSignals(event.time, touchDownConsumedActorHandle, currentPoint, nullptr /* Not Required for this use case */);
            }
          }

          processor.mTouchDownConsumedActor.SetActor(nullptr);
          processor.mInterceptedTouchActor.SetActor(nullptr);

          DALI_FALLTHROUGH;
        }

        case PointState::DOWN:
        {
          processor.mScene.EmitTouchedSignal(localVars.touchEventHandle);
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
  }
};

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

bool TouchEventProcessor::ProcessTouchEvent(const Integration::TouchEvent& event)
{
  DALI_LOG_TRACE_METHOD(gLogFilter);
  DALI_ASSERT_ALWAYS(!event.points.empty() && "Empty TouchEvent sent from Integration\n");

  PRINT_HIERARCHY(gLogFilter);

  DALI_TRACE_SCOPE(gTraceFilter, "DALI_PROCESS_TOUCH_EVENT");

  ProcessTouchEventVariables localVars(*this, mScene.IsGeometryHittestEnabled());

  // 1) Check if it is an interrupted event - we should inform our last primary hit actor about this
  //    and emit the stage signal as well.

  bool isInterrupted = false;
  if(event.points[0].GetState() == PointState::INTERRUPTED)
  {
    if(!localVars.isGeometry)
    {
      DALI_LOG_RELEASE_INFO("INTERRUPTED");
      Impl::EmitInterruptedEvent(localVars, event);
      return false; // No need for hit testing & already an interrupted event so just return false
    }
    isInterrupted = true;
  }

  // 2) Hit Testing.
  localVars.touchEventImpl   = new TouchEvent(event.time);
  localVars.touchEventHandle = Dali::TouchEvent(localVars.touchEventImpl.Get());

  DALI_LOG_INFO(gLogFilter, Debug::Concise, "\n");
  DALI_LOG_INFO(gLogFilter, Debug::General, "Point(s): %d\n", event.GetPointCount());

  bool firstPointParsed = false;
  for(auto&& currentPoint : event.points)
  {
    HitTestAlgorithm::Results hitTestResults;
    hitTestResults.point     = currentPoint;
    hitTestResults.eventTime = event.time;
    if(!firstPointParsed)
    {
      firstPointParsed = true;
      ParsePrimaryTouchPoint(hitTestResults, mCapturingTouchActor, mOwnTouchActor, mLastRenderTask, currentPoint, mScene, mCandidateActorLists);

      // Only set the currentRenderTask for the primary hit actor.
      localVars.currentRenderTask = hitTestResults.renderTask;
      localVars.touchEventImpl->SetRenderTask(Dali::RenderTask(localVars.currentRenderTask.Get()));
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
        HitTestAlgorithm::HitTest(mScene.GetSize(), mScene.GetRenderTaskList(), mScene.GetLayerList(), currentPoint.GetScreenPosition(), hitTestResults, nullptr, localVars.isGeometry ? Integration::Scene::TouchPropagationType::GEOMETRY : Integration::Scene::TouchPropagationType::PARENT);
      }
    }

    Integration::Point newPoint(currentPoint);
    newPoint.SetHitActor(hitTestResults.actor);
    newPoint.SetLocalPosition(hitTestResults.actorCoordinates);

    localVars.touchEventImpl->AddPoint(newPoint);

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
  }

  // 3) Recursively deliver events to the actor and its parents, until the event is consumed or the stage is reached.
  localVars.primaryPoint      = &localVars.touchEventImpl->GetPoint(0);
  localVars.primaryHitActor   = localVars.primaryPoint->GetHitActor();
  localVars.primaryPointState = localVars.primaryPoint->GetState();

  bool consumed = Impl::DeliverEventsToActorAndParents(localVars);

  // 4) Check if the last primary hit actor requires a leave event and if it was different to the current primary
  //    hit actor.  Also process the last consumed actor in the same manner.
  localVars.lastPrimaryHitActor = mInterceptedTouchActor.GetActor() ? mInterceptedTouchActor.GetActor() : mLastPrimaryHitActor.GetActor();
  localVars.lastConsumedActor   = mLastConsumedActor.GetActor();
  consumed |= Impl::DeliverLeaveEvent(localVars);

  // 5) Update the processor member appropriately.
  Impl::UpdateMembersWithCurrentHitInformation(localVars);

  // 6) Emit an interrupted event to the touch-down actor if it hasn't consumed the up and
  //    emit the stage touched event if required.
  Impl::DeliverEventToTouchDownActorAndScene(localVars, event);

  // If no events are consumed or intercepted in the Geometry mode, perform EmitInterruptedEvent.
  if(localVars.isGeometry && isInterrupted && !consumed && !mInterceptedTouchActor.GetActor())
  {
    DALI_LOG_RELEASE_INFO("INTERRUPTED");
    Impl::EmitInterruptedEvent(localVars, event);
  }

  return consumed;
}

void TouchEventProcessor::OnObservedActorDisconnected(Actor* actor)
{
  if(mScene.IsGeometryHittestEnabled() && (actor == mLastConsumedActor.GetActor() || actor == mLastPrimaryHitActor.GetActor()))
  {
    Dali::Actor        actorHandle(actor);
    Integration::Point point;
    point.SetState(PointState::INTERRUPTED);
    point.SetHitActor(actorHandle);
    if(actor == mLastConsumedActor.GetActor())
    {
      std::list<Dali::Internal::Actor*> actorLists;
      actorLists.push_back(mLastConsumedActor.GetActor());
      GeoAllocAndEmitTouchSignals(actorLists, 0, point, mLastRenderTask);
    }
    else if(!mLastConsumedActor.GetActor())
    {
      GeoAllocAndEmitTouchSignals(mCandidateActorLists, 0, point, mLastRenderTask);
    }
    // Do not set mLastPrimaryHitActor to NULL we may be iterating through its observers
    mLastConsumedActor.SetActor(nullptr);
    mLastRenderTask.Reset();
    mLastPrimaryPointState = PointState::FINISHED;
  }
  else
  {
    if(actor == mLastPrimaryHitActor.GetActor())
    {
      Dali::Actor        actorHandle(actor);
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
}

void TouchEventProcessor::Clear()
{
  mLastPrimaryHitActor.SetActor(nullptr);
  mLastConsumedActor.SetActor(nullptr);
  mCapturingTouchActor.SetActor(nullptr);
  mOwnTouchActor.SetActor(nullptr);
  mInterceptedTouchActor.SetActor(nullptr);
  mLastRenderTask.Reset();
  mTrackingActorLists.clear();
  mLastPrimaryPointState = PointState::FINISHED;
}

} // namespace Dali::Internal
