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
#include <dali/internal/event/events/geometry-touch-event-processor.h>

// EXTERNAL INCLUDES
#include <algorithm>
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
#include <dali/internal/event/events/touch-recipient-dispatcher.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/signals/callback.h>

namespace DALI_NAMESPACE::Internal
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
  ProcessTouchEventVariables(GeometryTouchEventProcessor& eventProcessor, bool streamEnding)
  : processor(eventProcessor),
    streamEnding(streamEnding)
  {
  }

  GeometryTouchEventProcessor& processor;                              ///< A handle to the touch-event-processor.
  bool                         streamEnding{false};                    ///< Whether this event ends the routed stream.
  Actor*                       lastPrimaryHitActor{nullptr};           ///< The last primary hit-actor.
  Actor*                       lastConsumedActor{nullptr};             ///< The last consuming actor.
  TouchEventPtr                touchEventImpl;                         ///< The current touch-event-impl.
  Dali::TouchEvent             touchEventHandle;                       ///< The handle to the touch-event-impl.
  RenderTaskPtr                currentRenderTask;                      ///< The current render-task.
  Dali::Actor                  consumedActor;                          ///< The actor that consumed the event.
  Dali::Actor                  primaryHitActor;                        ///< The actor that has been hit by the primary point.
  Integration::Point*          primaryPoint{nullptr};                  ///< The primary point of the hit.
  PointState::Type             primaryPointState{PointState::STARTED}; ///< The state of the primary point.
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

GeometryTouchRecipient* FindRecipient(GeometryTouchStreamState& streamState, const Actor* actor)
{
  auto iter = std::find_if(streamState.recipientsInDispatchOrder.begin(), streamState.recipientsInDispatchOrder.end(), [actor](const GeometryTouchRecipient& recipient)
  { return recipient.actor.Get() == actor; });
  return iter == streamState.recipientsInDispatchOrder.end() ? nullptr : &(*iter);
}

bool PrepareRecipientDispatch(GeometryTouchStreamState& streamState, Actor* actor, bool terminal)
{
  GeometryTouchRecipient* recipient = FindRecipient(streamState, actor);
  if(recipient && recipient->state == GeometryTouchRecipientState::TERMINATED)
  {
    return false;
  }

  if(!recipient)
  {
    streamState.recipientsInDispatchOrder.push_back({ActorPtr(actor), GeometryTouchRecipientState::ACTIVE});
    recipient = &streamState.recipientsInDispatchOrder.back();
  }

  recipient->state = terminal ? GeometryTouchRecipientState::TERMINATED : GeometryTouchRecipientState::ACTIVE;
  return true;
}

void BuildRootToTargetPath(Dali::Actor target, std::list<ActorPtr>& path)
{
  path.clear();
  for(Dali::Actor actor = target; actor; actor = actor.GetParent())
  {
    path.push_front(ActorPtr(&GetImplementation(actor)));
  }
}

Dali::Actor EmitGeoInterceptTouchSignals(const std::list<ActorPtr>& rootToTargetPath,
                                         const Dali::TouchEvent&    touchEvent,
                                         RenderTask*                renderTask,
                                         Actor*                     initialHitActor,
                                         const Actor*               stopBoundary)
{
  const std::list<ActorPtr> pathSnapshot(rootToTargetPath);
  for(const auto& actorPtr : pathSnapshot)
  {
    Actor* actor = actorPtr.Get();
    if(TouchRecipientDispatcher::IsGeometryInterceptDispatchable(*actor, touchEvent))
    {
      DALI_TRACE_SCOPE(gTraceFilter, "DALI_EMIT_INTERCEPT_TOUCH_EVENT_SIGNAL");
      if(TouchRecipientDispatcher::DispatchGeometryIntercept(*actor, renderTask, touchEvent, initialHitActor))
      {
        return Dali::Actor(actor);
      }
    }

    // The owner may consume DOWN before recognizing a gesture through interception.
    // Include its intercept callback so subsequent motion can start that gesture.
    if(actor == stopBoundary)
    {
      break;
    }
  }
  return Dali::Actor();
}

/**
 * Delivers coordinate candidates from front to back and records actual recipients.
 */
Dali::Actor EmitGeoTouchSignalsWithTracking(const std::list<ActorPtr>& actorLists,
                                            GeometryTouchStreamState&  streamState,
                                            const Dali::TouchEvent&    touchEvent,
                                            RenderTask*                renderTask,
                                            Actor*                     initialHitActor,
                                            bool                       streamEnding)
{
  Dali::Actor consumedActor;

  const std::list<ActorPtr> candidateSnapshot(actorLists);
  std::vector<Actor*>       dispatchedCandidates;
  for(auto rIter = candidateSnapshot.rbegin(); rIter != candidateSnapshot.rend(); ++rIter)
  {
    ActorPtr actorPtr  = *rIter;
    Actor*   actorImpl = actorPtr.Get();
    if(std::find(dispatchedCandidates.begin(), dispatchedCandidates.end(), actorImpl) != dispatchedCandidates.end())
    {
      continue;
    }
    dispatchedCandidates.push_back(actorImpl);

    if(TouchRecipientDispatcher::IsGeometryTouchDispatchable(*actorImpl, touchEvent))
    {
      if(!PrepareRecipientDispatch(streamState, actorImpl, streamEnding))
      {
        continue;
      }

      DALI_TRACE_SCOPE(gTraceFilter, "DALI_EMIT_TOUCH_EVENT_SIGNAL");
      if(TouchRecipientDispatcher::DispatchGeometryTouch(*actorImpl,
                                                         renderTask,
                                                         touchEvent,
                                                         initialHitActor,
                                                         touchEvent.GetState(0u)))
      {
        // One of this actor's listeners has consumed the event so set this actor as the consumed actor.
        consumedActor = Dali::Actor(actorImpl);
        break;
      }
      if(streamState.phase == GeometryTouchStreamPhase::FINISHING ||
         streamState.phase == GeometryTouchStreamPhase::FINISHED)
      {
        break;
      }
    }
  }
  return consumedActor;
}

/**
 * Changes the state of the primary point to leave and emits the touch signals
 */
Dali::Actor EmitTouchSignals(Actor*                    actor,
                             RenderTask&               renderTask,
                             const TouchEventPtr&      originalTouchEvent,
                             PointState::Type          state,
                             Actor*                    initialHitActor,
                             GeometryTouchStreamState* streamState  = nullptr,
                             bool                      streamEnding = false)
{
  Dali::Actor consumingActor;

  if(actor)
  {
    Dali::TouchEvent sourceEvent(originalTouchEvent.Get());
    // Ask with the state the primary point will carry at the recipient, so that this guard and the
    // check inside DispatchGeometryTouch() agree. Otherwise a recipient could be marked terminated
    // by PrepareRecipientDispatch() without ever receiving its terminal event.
    if(TouchRecipientDispatcher::IsGeometryTouchDispatchable(*actor, sourceEvent, state) &&
       (!streamState || PrepareRecipientDispatch(*streamState, actor, streamEnding)))
    {
      DALI_TRACE_SCOPE(gTraceFilter, "DALI_EMIT_TOUCH_EVENT_SIGNAL");
      if(TouchRecipientDispatcher::DispatchGeometryTouch(*actor, &renderTask, sourceEvent, initialHitActor, state))
      {
        consumingActor = Dali::Actor(actor);
      }
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
  HitTestAlgorithm::Results&     hitTestResults,
  ActorObserver&                 capturingTouchActorObserver,
  ActorObserver&                 ownTouchActorObserver,
  const RenderTaskPtr&           lastRenderTask,
  const Integration::Point&      currentPoint,
  const Internal::Scene&         scene,
  std::list<ActorPtr>&           actorLists,
  const GeometryTouchInitialHit* initialHit)
{
  Actor* capturingTouchActor = capturingTouchActorObserver.GetActor();

  if(initialHit)
  {
    Actor* hitActor                 = initialHit->actor.Get();
    hitTestResults.actor            = Dali::Actor(hitActor);
    hitTestResults.renderTask       = initialHit->renderTask;
    hitTestResults.actorCoordinates = initialHit->actorCoordinates;
    actorLists                      = initialHit->candidatesRootToFront;
    capturingTouchActorObserver.SetActor(hitActor);
    ownTouchActorObserver.SetActor(hitActor);
  }
  // We only set the capturing touch actor when the first touch-started actor captures all touch so if it's set, just use it
  else if(capturingTouchActor && lastRenderTask)
  {
    hitTestResults.actor          = Dali::Actor(capturingTouchActor);
    hitTestResults.renderTask     = lastRenderTask;
    const Vector2& screenPosition = currentPoint.GetScreenPosition();
    capturingTouchActor->ScreenToLocal(*lastRenderTask, hitTestResults.actorCoordinates.x, hitTestResults.actorCoordinates.y, screenPosition.x, screenPosition.y);
  }
  else
  {
    Actor* ownTouchActor = ownTouchActorObserver.GetActor();
    HitTestAlgorithm::HitTest(scene.GetSize(), scene.GetRenderTaskList(), scene.GetLayerList(), currentPoint.GetScreenPosition(), hitTestResults, ownTouchActor, Integration::Scene::TouchPropagationType::GEOMETRY);

    if(currentPoint.GetState() == PointState::STARTED && hitTestResults.actor)
    {
      // A geometry stream starts from its initial hit. These observers are rebound if a consumer
      // later becomes the stable owner.
      Actor* hitActor = &GetImplementation(hitTestResults.actor);
      capturingTouchActorObserver.SetActor(hitActor);
      ownTouchActorObserver.SetActor(hitActor);

      // Convert raw actor pointers to ActorPtr
      actorLists.clear();
      for(auto* rawActor : hitTestResults.actorLists)
      {
        actorLists.push_back(ActorPtr(rawActor));
      }
    }
  }
}

} // unnamed namespace

struct GeometryTouchEventProcessor::Impl
{
  static Actor* GetEstablishedOwner(GeometryTouchEventProcessor& processor)
  {
    if(processor.mStreamState.phase != GeometryTouchStreamPhase::OWNED || !processor.mLastRenderTask)
    {
      return nullptr;
    }

    Actor* owner = processor.mLastConsumedActor.GetActor();
    if(!owner)
    {
      return nullptr;
    }

    const GeometryTouchRecipient* recipient = FindRecipient(processor.mStreamState, owner);
    return recipient && recipient->state == GeometryTouchRecipientState::ACTIVE ? owner : nullptr;
  }

  static void BindRouteToOwner(GeometryTouchEventProcessor& processor, Actor* owner)
  {
    processor.mLastPrimaryHitActor.SetActor(owner);
    processor.mCapturingTouchActor.SetActor(owner);
    processor.mOwnTouchActor.SetActor(owner);
  }

  static Dali::Actor TerminateRecipient(GeometryTouchEventProcessor& processor,
                                        Actor*                       actor,
                                        const TouchEventPtr&         sourceEvent,
                                        RenderTask*                  renderTask)
  {
    GeometryTouchRecipient* recipient = FindRecipient(processor.mStreamState, actor);
    if(!recipient || recipient->state != GeometryTouchRecipientState::ACTIVE)
    {
      return Dali::Actor();
    }

    ActorPtr recipientActor = recipient->actor;
    recipient->state        = GeometryTouchRecipientState::TERMINATED;
    if(renderTask)
    {
      return EmitTouchSignals(recipientActor.Get(),
                              *renderTask,
                              sourceEvent,
                              PointState::INTERRUPTED,
                              processor.mStreamState.initialHitActor.Get());
    }

    Dali::TouchEvent sourceEventHandle(sourceEvent.Get());
    if(TouchRecipientDispatcher::DispatchGeometryTouch(*recipientActor,
                                                       nullptr,
                                                       sourceEventHandle,
                                                       processor.mStreamState.initialHitActor.Get(),
                                                       PointState::INTERRUPTED))
    {
      return Dali::Actor(recipientActor.Get());
    }
    return Dali::Actor();
  }

  static bool TerminateAllActive(GeometryTouchEventProcessor& processor,
                                 const TouchEventPtr&         sourceEvent,
                                 RenderTask*                  renderTask,
                                 const Actor*                 exceptActor = nullptr)
  {
    std::vector<ActorPtr> activeRecipients;
    activeRecipients.reserve(processor.mStreamState.recipientsInDispatchOrder.size());
    for(const auto& recipient : processor.mStreamState.recipientsInDispatchOrder)
    {
      if(recipient.state == GeometryTouchRecipientState::ACTIVE && recipient.actor.Get() != exceptActor)
      {
        activeRecipients.push_back(recipient.actor);
      }
    }

    bool consumed = false;
    for(const auto& actor : activeRecipients)
    {
      consumed |= static_cast<bool>(TerminateRecipient(processor, actor.Get(), sourceEvent, renderTask));
    }
    return consumed;
  }

  static void SelectOwner(ProcessTouchEventVariables& localVars, Actor* newOwner)
  {
    GeometryTouchEventProcessor& processor = localVars.processor;
    if(!newOwner ||
       processor.mStreamState.phase == GeometryTouchStreamPhase::FINISHING ||
       processor.mStreamState.phase == GeometryTouchStreamPhase::FINISHED ||
       processor.mLastConsumedActor.GetActor() == newOwner)
    {
      return;
    }

    BuildRootToTargetPath(Dali::Actor(newOwner), processor.mStreamState.ownerPathRootToOwner);
    processor.mLastConsumedActor.SetActor(newOwner);
    processor.mStreamState.phase = GeometryTouchStreamPhase::OWNED;

    // A consumer selected after the initial event can take over an already established route
    // before termination callbacks run. The first event binds the route after its render task
    // has been stored.
    if(!localVars.streamEnding && GetEstablishedOwner(processor))
    {
      BindRouteToOwner(processor, newOwner);
    }

    TerminateAllActive(processor, localVars.touchEventImpl, localVars.currentRenderTask.Get(), newOwner);

    if(!localVars.streamEnding && processor.mStreamState.phase == GeometryTouchStreamPhase::OWNED)
    {
      const auto                    activeCount                = std::count_if(processor.mStreamState.recipientsInDispatchOrder.begin(), processor.mStreamState.recipientsInDispatchOrder.end(), [](const GeometryTouchRecipient& recipient)
                                        { return recipient.state == GeometryTouchRecipientState::ACTIVE; });
      const GeometryTouchRecipient* ownerRecipient             = FindRecipient(processor.mStreamState, newOwner);
      const bool                    ownerIsSoleActiveRecipient = activeCount == 1u && ownerRecipient && ownerRecipient->state == GeometryTouchRecipientState::ACTIVE;
      if(DALI_UNLIKELY(!ownerIsSoleActiveRecipient))
      {
        DALI_LOG_ERROR("Geometry owner selection left an invalid active-recipient state. activeCount(%u), ownerActive(%d)\n",
                       static_cast<unsigned int>(activeCount),
                       ownerRecipient && ownerRecipient->state == GeometryTouchRecipientState::ACTIVE);
      }
    }
  }

  static const std::list<ActorPtr>& ResolveInterceptPath(GeometryTouchEventProcessor& processor, Actor*& owner)
  {
    owner = processor.mLastConsumedActor.GetActor();
    if(owner)
    {
      BuildRootToTargetPath(Dali::Actor(owner), processor.mStreamState.ownerPathRootToOwner);
      return processor.mStreamState.ownerPathRootToOwner;
    }
    return processor.mStreamState.initialHitPathRootToTarget;
  }

  static void DeliverInterruptedToInterceptPath(ProcessTouchEventVariables& localVars)
  {
    GeometryTouchEventProcessor& processor = localVars.processor;
    if(!localVars.currentRenderTask || processor.mInterceptedTouchActor.GetActor())
    {
      return;
    }

    Actor*      owner         = nullptr;
    const auto& interceptPath = ResolveInterceptPath(processor, owner);

    // A raw interruption is observable on the intercept path before active touch recipients are terminated.
    // An intercept result only stops this hierarchy traversal; it cannot suppress terminal delivery or create a new owner.
    EmitGeoInterceptTouchSignals(interceptPath,
                                 localVars.touchEventHandle,
                                 localVars.currentRenderTask.Get(),
                                 processor.mStreamState.initialHitActor.Get(),
                                 owner);
  }

  static bool FinishInterrupted(ProcessTouchEventVariables& localVars)
  {
    GeometryTouchEventProcessor& processor = localVars.processor;
    if(processor.mStreamState.phase == GeometryTouchStreamPhase::FINISHING ||
       processor.mStreamState.phase == GeometryTouchStreamPhase::FINISHED)
    {
      return false;
    }

    processor.mStreamState.phase = GeometryTouchStreamPhase::FINISHING;
    DeliverInterruptedToInterceptPath(localVars);
    const bool consumed = TerminateAllActive(processor, localVars.touchEventImpl, localVars.currentRenderTask.Get());

    const bool hasActiveRecipient = std::any_of(processor.mStreamState.recipientsInDispatchOrder.begin(), processor.mStreamState.recipientsInDispatchOrder.end(), [](const GeometryTouchRecipient& recipient)
    { return recipient.state == GeometryTouchRecipientState::ACTIVE; });
    if(DALI_UNLIKELY(hasActiveRecipient))
    {
      DALI_LOG_ERROR("Finished geometry stream still has an active recipient\n");
    }

    processor.Clear();
    return consumed;
  }

  static void FinishDisconnected(GeometryTouchEventProcessor& processor)
  {
    if(processor.mStreamState.phase == GeometryTouchStreamPhase::FINISHING ||
       processor.mStreamState.phase == GeometryTouchStreamPhase::FINISHED)
    {
      return;
    }

    processor.mStreamState.phase = GeometryTouchStreamPhase::FINISHING;

    TouchEventPtr      interruptedEvent(new TouchEvent(0u));
    Integration::Point point;
    point.SetState(PointState::INTERRUPTED);
    interruptedEvent->AddPoint(point);
    interruptedEvent->SetRenderTask(Dali::RenderTask(processor.mLastRenderTask.Get()));

    TerminateAllActive(processor, interruptedEvent, processor.mLastRenderTask.Get());
    const bool hasActiveRecipient = std::any_of(processor.mStreamState.recipientsInDispatchOrder.begin(), processor.mStreamState.recipientsInDispatchOrder.end(), [](const GeometryTouchRecipient& recipient)
    { return recipient.state == GeometryTouchRecipientState::ACTIVE; });
    if(DALI_UNLIKELY(hasActiveRecipient))
    {
      DALI_LOG_ERROR("Disconnected geometry stream still has an active recipient\n");
    }
    processor.Clear(true);
  }

  /**
   * Recursively deliver events to the actor and its parents, until the event is consumed or the stage is reached.
   * @param[in/out] localVars The struct of stack variables used by ProcessTouchEvent
   * @return True if consumed, false otherwise.
   */
  static inline bool DeliverEventsToActorAndParents(ProcessTouchEventVariables& localVars)
  {
    bool                         consumed  = false;
    GeometryTouchEventProcessor& processor = localVars.processor;
    if(localVars.currentRenderTask)
    {
      Dali::Actor interceptedActor;
      Actor*      interceptedTouchActor = processor.mInterceptedTouchActor.GetActor();

      if(!interceptedTouchActor)
      {
        Actor*      owner         = nullptr;
        const auto& interceptPath = ResolveInterceptPath(processor, owner);

        interceptedActor = EmitGeoInterceptTouchSignals(interceptPath,
                                                        localVars.touchEventHandle,
                                                        localVars.currentRenderTask.Get(),
                                                        processor.mStreamState.initialHitActor.Get(),
                                                        owner);
        if(interceptedActor)
        {
          interceptedTouchActor = &GetImplementation(interceptedActor);
          processor.mInterceptedTouchActor.SetActor(interceptedTouchActor);

          processor.mStreamState.interceptedRootToTarget.clear();
          for(const auto& actor : interceptPath)
          {
            processor.mStreamState.interceptedRootToTarget.push_back(actor);
            if(actor.Get() == interceptedTouchActor)
            {
              break;
            }
          }

          TerminateAllActive(processor, localVars.touchEventImpl, localVars.currentRenderTask.Get(), interceptedTouchActor);
          processor.mLastConsumedActor.SetActor(nullptr);
        }
      }

      Actor* touchConsumedActor = processor.mLastConsumedActor.GetActor();
      if(touchConsumedActor)
      {
        localVars.consumedActor = EmitTouchSignals(touchConsumedActor,
                                                   *localVars.currentRenderTask.Get(),
                                                   localVars.touchEventImpl,
                                                   localVars.primaryPointState,
                                                   processor.mStreamState.initialHitActor.Get(),
                                                   &processor.mStreamState,
                                                   localVars.streamEnding);
      }
      else
      {
        const auto& dispatchActors = interceptedTouchActor ? processor.mStreamState.interceptedRootToTarget : processor.mStreamState.candidatesRootToFront;
        localVars.consumedActor    = EmitGeoTouchSignalsWithTracking(dispatchActors,
                                                                     processor.mStreamState,
                                                                     localVars.touchEventHandle,
                                                                     localVars.currentRenderTask.Get(),
                                                                     processor.mStreamState.initialHitActor.Get(),
                                                                     localVars.streamEnding);
        if(localVars.consumedActor)
        {
          SelectOwner(localVars, &GetImplementation(localVars.consumedActor));
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
                      localVars.primaryHitActor ? localVars.primaryHitActor.GetProperty<Dali::String>(Dali::Actor::Property::NAME).CStr() : "",
                      TOUCH_POINT_STATE[localVars.primaryPointState],
                      localVars.primaryPoint->GetScreenPosition().x,
                      localVars.primaryPoint->GetScreenPosition().y,
                      true);
        DALI_LOG_INFO(gLogFilter,
                      Debug::Concise,
                      "ConsumedActor:   (%p) id(%d), name(%s), state(%s), isGeo : %d \n",
                      localVars.consumedActor ? reinterpret_cast<void*>(&localVars.consumedActor.GetBaseObject()) : NULL,
                      localVars.consumedActor ? localVars.consumedActor.GetProperty<int32_t>(Dali::Actor::Property::ID) : -1,
                      localVars.consumedActor ? localVars.consumedActor.GetProperty<Dali::String>(Dali::Actor::Property::NAME).CStr() : "",
                      TOUCH_POINT_STATE[localVars.primaryPointState],
                      true);
      }
      else
      {
        DALI_LOG_RELEASE_INFO("PrimaryHitActor:(%p), id(%d), name(%s), state(%s), screenPosition(%f, %f), isGeo : %d \n",
                              localVars.primaryHitActor ? reinterpret_cast<void*>(&localVars.primaryHitActor.GetBaseObject()) : NULL,
                              localVars.primaryHitActor ? localVars.primaryHitActor.GetProperty<int32_t>(Dali::Actor::Property::ID) : -1,
                              localVars.primaryHitActor ? localVars.primaryHitActor.GetProperty<Dali::String>(Dali::Actor::Property::NAME).CStr() : "",
                              TOUCH_POINT_STATE[localVars.primaryPointState],
                              localVars.primaryPoint->GetScreenPosition().x,
                              localVars.primaryPoint->GetScreenPosition().y,
                              true);
        DALI_LOG_RELEASE_INFO("ConsumedActor:  (%p), id(%d), name(%s), state(%s), isGeo : %d \n",
                              localVars.consumedActor ? reinterpret_cast<void*>(&localVars.consumedActor.GetBaseObject()) : NULL,
                              localVars.consumedActor ? localVars.consumedActor.GetProperty<int32_t>(Dali::Actor::Property::ID) : -1,
                              localVars.consumedActor ? localVars.consumedActor.GetProperty<Dali::String>(Dali::Actor::Property::NAME).CStr() : "",
                              TOUCH_POINT_STATE[localVars.primaryPointState],
                              true);
      }
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
    bool                         consumed = false;
    GeometryTouchEventProcessor& processor(localVars.processor);
    if((localVars.primaryPointState == PointState::MOTION) || (localVars.primaryPointState == PointState::UP) || (localVars.primaryPointState == PointState::STATIONARY))
    {
      if(processor.mLastRenderTask)
      {
        Dali::Actor leaveEventConsumer;
        RenderTask& lastRenderTaskImpl = *processor.mLastRenderTask.Get();

        {
          if(localVars.lastPrimaryHitActor)
          {
            if(!localVars.lastPrimaryHitActor->IsHittable() || !IsActuallySensitive(localVars.lastPrimaryHitActor))
            {
              // The recipient is marked terminal before its callback, so a later event cannot interrupt it again.
              DALI_LOG_RELEASE_INFO("InterruptedActor(Hit):     (%p) %s\n", reinterpret_cast<void*>(localVars.lastPrimaryHitActor), localVars.lastPrimaryHitActor->GetName().data());
              leaveEventConsumer = TerminateRecipient(processor, localVars.lastPrimaryHitActor, localVars.touchEventImpl, &lastRenderTaskImpl);
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
              TerminateRecipient(processor, localVars.lastConsumedActor, localVars.touchEventImpl, &lastRenderTaskImpl);
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

    // A geometry stream always has a capturing touch actor, so the point count of this event cannot
    // tell whether the stream is over: the router owns that answer and passes it as streamEnding,
    // which accounts for the stream's devices that this event does not carry.
    GeometryTouchEventProcessor& processor(localVars.processor);
    const bool                   shouldClear = localVars.streamEnding && (localVars.primaryPointState == PointState::UP);

    if(shouldClear)
    {
      processor.mStreamState.phase = GeometryTouchStreamPhase::FINISHING;
      TerminateAllActive(processor, localVars.touchEventImpl, localVars.currentRenderTask.Get());
      processor.Clear();
    }
    else
    {
      // The primaryHitActor may have been removed from the scene so ensure it is still on the scene before setting members.
      if(localVars.primaryHitActor && GetImplementation(localVars.primaryHitActor).OnScene())
      {
        // Only observe the consumed actor if we have a primaryHitActor (check if it is still on the scene).
        if(localVars.consumedActor && GetImplementation(localVars.consumedActor).OnScene())
        {
          processor.mLastConsumedActor.SetActor(&GetImplementation(localVars.consumedActor));
        }
        else if(localVars.lastConsumedActor && !localVars.lastConsumedActor->OnScene())
        {
          processor.mLastConsumedActor.SetActor(nullptr);
        }

        processor.mLastRenderTask = localVars.currentRenderTask;

        if(Actor* owner = GetEstablishedOwner(processor))
        {
          // Ownership replaces the primary hit as the stable route. The initial hit remains in
          // mStreamState solely as the public hit-actor identity for recipient events.
          BindRouteToOwner(processor, owner);
        }
        else
        {
          processor.mLastPrimaryHitActor.SetActor(&GetImplementation(localVars.primaryHitActor));
        }
      }
      else if(Actor* owner = GetEstablishedOwner(processor))
      {
        // The route was established by an earlier event, so a disappearing primary cannot replace
        // its render task or terminate it. Continue routing through the owner.
        BindRouteToOwner(processor, owner);
      }
      else
      {
        processor.mStreamState.phase = GeometryTouchStreamPhase::FINISHING;
        TerminateAllActive(processor, localVars.touchEventImpl, localVars.currentRenderTask.Get());
        processor.Clear();
      }
    }
  }

  /**
   * Clear per-stream terminal observers after the last release.
   * @param[in/out] localVars The struct of stack variables used by ProcessTouchEvent
   */
  static inline void ClearTerminalObservers(ProcessTouchEventVariables& localVars)
  {
    GeometryTouchEventProcessor& processor(localVars.processor);
    if(localVars.touchEventImpl->GetPointCount() == 1) // Only want the first touch and the last release
    {
      if(localVars.primaryPointState == PointState::UP)
      {
        processor.mInterceptedTouchActor.SetActor(nullptr);
      }
    }
  }
};

GeometryTouchEventProcessor::GeometryTouchEventProcessor(Scene& scene)
: mScene(scene),
  mLastPrimaryHitActor(MakeCallback(this, &GeometryTouchEventProcessor::OnObservedActorDisconnected)),
  mLastConsumedActor(MakeCallback(this, &GeometryTouchEventProcessor::OnObservedActorDisconnected)),
  mCapturingTouchActor(),
  mOwnTouchActor(),
  mInterceptedTouchActor(),
  mLastRenderTask(),
  mStreamState(),
  mProcessingTouchEvent(false),
  mObservedActorDisconnected(false)
{
  DALI_LOG_TRACE_METHOD(gLogFilter);
}

GeometryTouchEventProcessor::~GeometryTouchEventProcessor()
{
  DALI_LOG_TRACE_METHOD(gLogFilter);
}

bool GeometryTouchEventProcessor::ProcessTouchEvent(const Integration::TouchEvent& event, bool streamEnding, const GeometryTouchInitialHit* initialHit)
{
  DALI_LOG_TRACE_METHOD(gLogFilter);
  DALI_ASSERT_ALWAYS(!event.points.empty() && "Empty TouchEvent sent from Integration\n");
  mProcessingTouchEvent      = true;
  mObservedActorDisconnected = false;

  PRINT_HIERARCHY(gLogFilter);

  DALI_TRACE_SCOPE(gTraceFilter, "DALI_PROCESS_TOUCH_EVENT");

  if(event.points[0].GetState() == PointState::DOWN)
  {
    const bool hasActiveRecipient = std::any_of(mStreamState.recipientsInDispatchOrder.begin(), mStreamState.recipientsInDispatchOrder.end(), [](const GeometryTouchRecipient& recipient)
    { return recipient.state == GeometryTouchRecipientState::ACTIVE; });
    if(!hasActiveRecipient && !mStreamState.recipientsInDispatchOrder.empty())
    {
      Clear();
      mStreamState.phase = GeometryTouchStreamPhase::UNOWNED;
    }
  }

  ProcessTouchEventVariables localVars(*this, streamEnding);

  // 1) Hit testing and event construction.
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
      ParsePrimaryTouchPoint(hitTestResults, mCapturingTouchActor, mOwnTouchActor, mLastRenderTask, currentPoint, mScene, mStreamState.candidatesRootToFront, initialHit);

      // The router's pending initial hit starts this processor. A processor whose recipients were
      // all terminated can also be cleared and restarted within the same router stream. Otherwise,
      // a later device joining with another DOWN must preserve the established owner and initial hit.
      const bool initializeStream = initialHit || (mStreamState.initialHitActor.Get() == nullptr);
      if(initializeStream && currentPoint.GetState() == PointState::DOWN && hitTestResults.actor)
      {
        mStreamState.phase           = GeometryTouchStreamPhase::UNOWNED;
        mStreamState.initialHitActor = ActorPtr(&GetImplementation(hitTestResults.actor));
        BuildRootToTargetPath(hitTestResults.actor, mStreamState.initialHitPathRootToTarget);
      }

      // Start with the task of the primary hit actor. An established owner route overrides it below.
      localVars.currentRenderTask = hitTestResults.renderTask;
      if(Impl::GetEstablishedOwner(*this))
      {
        // Once ownership is established, the stream stays in the render task that selected the
        // owner even if a later hit test finds another actor in another render task.
        localVars.currentRenderTask = mLastRenderTask;
      }
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
        HitTestAlgorithm::HitTest(mScene.GetSize(), mScene.GetRenderTaskList(), mScene.GetLayerList(), currentPoint.GetScreenPosition(), hitTestResults, nullptr, Integration::Scene::TouchPropagationType::GEOMETRY);
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
                  (hitTestResults.actor ? hitTestResults.actor.GetProperty<Dali::String>(Dali::Actor::Property::NAME).CStr() : ""),
                  hitTestResults.actorCoordinates.x,
                  hitTestResults.actorCoordinates.y);
  }

  // 2) Resolve the primary point and finish cancellation through the single terminal path.
  localVars.primaryPoint      = &localVars.touchEventImpl->GetPoint(0);
  localVars.primaryHitActor   = localVars.primaryPoint->GetHitActor();
  localVars.primaryPointState = localVars.primaryPoint->GetState();

  if(localVars.primaryPointState == PointState::INTERRUPTED)
  {
    const bool consumed        = Impl::FinishInterrupted(localVars);
    mProcessingTouchEvent      = false;
    mObservedActorDisconnected = false;
    return consumed;
  }

  // 3) Deliver intercept callbacks on the hierarchy path and touch callbacks in coordinate order.
  bool consumed = Impl::DeliverEventsToActorAndParents(localVars);
  if(mObservedActorDisconnected)
  {
    mProcessingTouchEvent      = false;
    mObservedActorDisconnected = false;
    Impl::FinishDisconnected(*this);
    return consumed;
  }

  // 4) Check if the last primary hit actor requires a leave event and if it was different to the current primary
  //    hit actor.  Also process the last consumed actor in the same manner.
  localVars.lastPrimaryHitActor = mInterceptedTouchActor.GetActor() ? mInterceptedTouchActor.GetActor() : mLastPrimaryHitActor.GetActor();
  localVars.lastConsumedActor   = mLastConsumedActor.GetActor();
  consumed |= Impl::DeliverLeaveEvent(localVars);
  if(mObservedActorDisconnected)
  {
    mProcessingTouchEvent      = false;
    mObservedActorDisconnected = false;
    Impl::FinishDisconnected(*this);
    return consumed;
  }

  // 5) Update the processor member appropriately.
  Impl::UpdateMembersWithCurrentHitInformation(localVars);

  // 6) Clear observers whose lifetime ends with the last release.
  Impl::ClearTerminalObservers(localVars);

  mProcessingTouchEvent = false;
  return consumed;
}

bool GeometryTouchEventProcessor::IsFinished() const
{
  return mStreamState.phase == GeometryTouchStreamPhase::FINISHED;
}

void GeometryTouchEventProcessor::OnObservedActorDisconnected(Actor* actor)
{
  Actor* consumedActor    = mLastConsumedActor.GetActor();
  Actor* primaryHitActor  = mLastPrimaryHitActor.GetActor();
  Actor* establishedOwner = Impl::GetEstablishedOwner(*this);

  if(actor == consumedActor || actor == primaryHitActor)
  {
    // Once an actor owns the stream, removing a different primary hit actor must not
    // terminate the owner's stream. The owner will receive the physical terminal event.
    if(establishedOwner && actor == primaryHitActor && actor != establishedOwner)
    {
      return;
    }

    if(mProcessingTouchEvent)
    {
      mObservedActorDisconnected = true;
    }
    else
    {
      Impl::FinishDisconnected(*this);
    }
  }
}

void GeometryTouchEventProcessor::Clear(bool keepLastPrimaryObserver)
{
  if(!keepLastPrimaryObserver)
  {
    mLastPrimaryHitActor.SetActor(nullptr);
  }
  mLastConsumedActor.SetActor(nullptr);
  mCapturingTouchActor.SetActor(nullptr);
  mOwnTouchActor.SetActor(nullptr);
  mInterceptedTouchActor.SetActor(nullptr);
  mLastRenderTask.Reset();
  mStreamState.phase = GeometryTouchStreamPhase::FINISHED;
  mStreamState.initialHitActor.Reset();
  mStreamState.initialHitPathRootToTarget.clear();
  mStreamState.ownerPathRootToOwner.clear();
  mStreamState.interceptedRootToTarget.clear();
  mStreamState.candidatesRootToFront.clear();
  mStreamState.recipientsInDispatchOrder.clear();
  mObservedActorDisconnected = false;
}

} //namespace DALI_NAMESPACE::Internal
