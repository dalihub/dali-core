#ifndef DALI_INTERNAL_HOVER_EVENT_PROCESSOR_H
#define DALI_INTERNAL_HOVER_EVENT_PROCESSOR_H

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

// INTERNAL INCLUDES
#include <dali/internal/event/events/actor-observer.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>

namespace Dali
{
class Actor;
struct Vector2;
struct Vector4;

namespace Integration
{
struct HoverEvent;
}

namespace Internal
{
struct ActorObserver;
class Scene;

/**
 * <h3>Multi-Hover Event Processing:</h3>
 *
 * The HoverEventProcessor processes hover events and emits the Hovered signal on the hit actor (and
 * its parents).
 *
 * - Hit Testing & Hover Event Delivery are described in Dali::Actor.
 */
class HoverEventProcessor
{
public:
  /**
   * Create an event processor.
   * @param[in] scene The scene the event processor belongs to.
   */
  HoverEventProcessor(Scene& scene);

  /**
   * Non-virtual destructor; HoverEventProcessor is not a base class
   */
  ~HoverEventProcessor();

  /**
   * This function is called by the event processor whenever a hover event occurs.
   * @param[in] event The hover event that has occurred.
   */
  void ProcessHoverEvent(const Integration::HoverEvent& event);

  /**
   * This function is called when sending a interrupted hover event to a specific actor.
   * @param[in] actor The actor on which the hover event should occur.
   */
  void SendInterruptedHoverEvent(Dali::Internal::Actor* actor);

  // Movable but not copyable
  HoverEventProcessor(const HoverEventProcessor&)            = delete;
  HoverEventProcessor(HoverEventProcessor&&)                 = default;
  HoverEventProcessor& operator=(const HoverEventProcessor&) = delete;
  HoverEventProcessor& operator=(HoverEventProcessor&&)      = default;

private:
  /**
   * Clears the value.
   */
  void Clear();

  /**
   * Called by some actor-observers when the observed actor is disconnected.
   * @param[in]  actor  The actor that has been disconnected.
   */
  void OnObservedActorDisconnected(Dali::Internal::Actor* actor);

  Scene&                            mScene;                   ///< Reference to the scene
  ActorObserver                     mLastPrimaryHitActor;     ///< Stores the last primary point hit actor
  ActorObserver                     mLastConsumedActor;       ///< Stores the last consumed actor
  ActorObserver                     mHoverStartConsumedActor; ///< Stores the hover-start consumed actor
  RenderTaskPtr                     mLastRenderTask;          ///< The RenderTask used for the last hit actor
  std::list<Dali::Internal::Actor*> mCandidateActorLists;     ///< Stores a list of actors that can be touched, from leaf actor to root.
  std::list<Dali::Internal::Actor*> mLastActorLists;          ///< Stores a list of actors that can be touched, from leaf actor to root.

  struct Impl;
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_HOVER_EVENT_PROCESSOR_H
