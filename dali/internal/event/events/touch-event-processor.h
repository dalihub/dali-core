#ifndef DALI_INTERNAL_TOUCH_EVENT_PROCESSOR_H
#define DALI_INTERNAL_TOUCH_EVENT_PROCESSOR_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
struct TouchEvent;
}

namespace Internal
{

class Actor;
class Scene;
struct ActorObserver;

/**
 * <h3>Multi-Touch Event Processing:</h3>
 *
 * The TouchEventProcessor processes touch events and emits the Touched signal on the hit actor (and
 * its parents).
 *
 * - Hit Testing & Touch Event Delivery are described in Dali::Actor.
 */
class TouchEventProcessor
{
public:

  /**
   * Create an event processor.
   * @param[in] scene The scene the event processor belongs to.
   */
  TouchEventProcessor( Scene& scene );

  /**
   * Non-virtual destructor; TouchEventProcessor is not a base class
   */
  ~TouchEventProcessor();

  /**
   * This function is called by the event processor whenever a touch event occurs.
   * @param[in] event The touch event that has occurred.
   */
  void ProcessTouchEvent( const Integration::TouchEvent& event );

private:

  // Undefined
  TouchEventProcessor(const TouchEventProcessor&);

  // Undefined
  TouchEventProcessor& operator=(const TouchEventProcessor& rhs);

private:

  Scene& mScene; ///< Used to deliver touch events

  /**
   * Called by some actor-observers when the observed actor is disconnected.
   *
   * @param[in]  actor  The actor that has been disconnected.
   */
  void OnObservedActorDisconnected( Actor* actor );

  ActorObserver mLastPrimaryHitActor; ///< Stores the last primary point hit actor
  ActorObserver mLastConsumedActor; ///< Stores the last consumed actor
  ActorObserver mTouchDownConsumedActor; ///< Stores the touch-down consumed actor
  RenderTaskPtr mLastRenderTask; ///< The RenderTask used for the last hit actor
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_TOUCH_EVENT_PROCESSOR_H
