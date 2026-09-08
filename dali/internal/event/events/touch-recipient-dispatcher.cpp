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

#include <dali/internal/event/events/touch-recipient-dispatcher.h>

#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/events/touch-event-impl.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/events/point-state.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/render-tasks/render-task.h>

namespace DALI_NAMESPACE::Internal
{
namespace
{
/**
 * The state a point has once it reaches the recipient. Only the primary point can be overridden.
 */
PointState::Type EffectiveState(const Dali::TouchEvent& event, uint32_t index, PointState::Type primaryState)
{
  return index == 0u ? primaryState : event.GetState(index);
}

bool HasPointState(const Dali::TouchEvent& event, PointState::Type targetState, PointState::Type primaryState)
{
  const uint32_t pointCount = event.GetPointCount();
  for(uint32_t i = 0u; i < pointCount; ++i)
  {
    if(EffectiveState(event, i, primaryState) == targetState)
    {
      return true;
    }
  }
  return false;
}

bool HasPointState(const Dali::TouchEvent& event, PointState::Type targetState)
{
  return event.GetPointCount() > 0u && HasPointState(event, targetState, event.GetState(0u));
}

bool ShouldDispatchParent(const Actor& actor, const Dali::TouchEvent& event, bool intercept)
{
  const bool required = intercept ? actor.GetInterceptTouchRequired() : actor.GetTouchRequired();
  return required &&
         (actor.IsHittable() || HasPointState(event, PointState::INTERRUPTED)) &&
         (actor.IsDispatchTouchMotionEnabled() || !HasPointState(event, PointState::MOTION));
}

/**
 * Decides dispatchability from the point states alone, so that it can be evaluated on the source
 * event before a recipient event is built. primaryState is the state the primary point will carry
 * once it reaches the recipient, which is not always the state it has in the source event.
 */
bool ShouldDispatchGeometry(const Actor& actor, const Dali::TouchEvent& event, PointState::Type primaryState, bool intercept)
{
  const bool required = intercept ? actor.GetInterceptTouchRequired() : actor.GetTouchRequired();
  const bool terminal = HasPointState(event, PointState::UP, primaryState) || HasPointState(event, PointState::INTERRUPTED, primaryState);
  return required &&
         (actor.IsHittable() || HasPointState(event, PointState::INTERRUPTED, primaryState)) &&
         (actor.IsDispatchTouchMotionEnabled() || !HasPointState(event, PointState::MOTION, primaryState) || terminal);
}

Dali::TouchEvent CreateRecipientEvent(Actor&                  actor,
                                      RenderTask*             renderTask,
                                      const Dali::TouchEvent& sourceEvent,
                                      Actor*                  initialHitActor,
                                      PointState::Type        primaryState)
{
  TouchEventPtr recipientEvent = TouchEvent::Clone(GetImplementation(sourceEvent));
  if(renderTask)
  {
    recipientEvent->SetRenderTask(Dali::RenderTask(renderTask));
  }

  const uint32_t pointCount = recipientEvent->GetPointCount();
  for(uint32_t i = 0u; i < pointCount; ++i)
  {
    Integration::Point& point          = recipientEvent->GetPoint(i);
    const Vector2&      screenPosition = point.GetScreenPosition();
    Vector2             localPosition;
    if(renderTask)
    {
      actor.ScreenToLocal(*renderTask, localPosition.x, localPosition.y, screenPosition.x, screenPosition.y);
    }
    else
    {
      actor.ScreenToLocal(localPosition.x, localPosition.y, screenPosition.x, screenPosition.y);
    }
    point.SetLocalPosition(localPosition);
    point.SetHitActor(Dali::Actor(initialHitActor));
  }

  if(pointCount > 0u)
  {
    recipientEvent->GetPoint(0u).SetState(primaryState);
  }
  return Dali::TouchEvent(recipientEvent.Get());
}
} // unnamed namespace

bool TouchRecipientDispatcher::DispatchParentTouch(Actor& actor, const Dali::TouchEvent& touchEvent)
{
  return ShouldDispatchParent(actor, touchEvent, false) && actor.DispatchTouchEvent(touchEvent);
}

bool TouchRecipientDispatcher::DispatchParentIntercept(Actor& actor, const Dali::TouchEvent& touchEvent)
{
  return ShouldDispatchParent(actor, touchEvent, true) && actor.EmitInterceptTouchEventSignal(touchEvent);
}

bool TouchRecipientDispatcher::DispatchGeometryTouch(Actor&                  actor,
                                                     RenderTask*             renderTask,
                                                     const Dali::TouchEvent& sourceEvent,
                                                     Actor*                  initialHitActor,
                                                     PointState::Type        primaryState)
{
  // Decide before building the recipient event, so that a rejected candidate costs no clone and no
  // per-point coordinate conversion.
  if(!IsGeometryTouchDispatchable(actor, sourceEvent, primaryState))
  {
    return false;
  }
  return actor.DispatchTouchEvent(CreateRecipientEvent(actor, renderTask, sourceEvent, initialHitActor, primaryState));
}

bool TouchRecipientDispatcher::DispatchGeometryIntercept(Actor&                  actor,
                                                         RenderTask*             renderTask,
                                                         const Dali::TouchEvent& sourceEvent,
                                                         Actor*                  initialHitActor)
{
  const PointState::Type primaryState = sourceEvent.GetState(0u);
  if(!IsGeometryInterceptDispatchable(actor, sourceEvent))
  {
    return false;
  }
  return actor.EmitInterceptTouchEventSignal(CreateRecipientEvent(actor, renderTask, sourceEvent, initialHitActor, primaryState));
}

bool TouchRecipientDispatcher::IsGeometryTouchDispatchable(const Actor& actor, const Dali::TouchEvent& touchEvent)
{
  return touchEvent.GetPointCount() > 0u && IsGeometryTouchDispatchable(actor, touchEvent, touchEvent.GetState(0u));
}

bool TouchRecipientDispatcher::IsGeometryTouchDispatchable(const Actor& actor, const Dali::TouchEvent& touchEvent, PointState::Type primaryState)
{
  return ShouldDispatchGeometry(actor, touchEvent, primaryState, false);
}

bool TouchRecipientDispatcher::IsGeometryInterceptDispatchable(const Actor& actor, const Dali::TouchEvent& touchEvent)
{
  return touchEvent.GetPointCount() > 0u && ShouldDispatchGeometry(actor, touchEvent, touchEvent.GetState(0u), true);
}

} //namespace DALI_NAMESPACE::Internal
