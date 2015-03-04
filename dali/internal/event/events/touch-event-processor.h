#ifndef __DALI_INTERNAL_TOUCH_EVENT_PROCESSOR_H__
#define __DALI_INTERNAL_TOUCH_EVENT_PROCESSOR_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/internal/event/events/actor-observer.h>

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
class Stage;
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
   * @param[in] stage The stage.
   */
  TouchEventProcessor( Stage& stage );

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

  /**
   * Called by some actor-observers when the observed actor is disconnected.
   *
   * @param[in]  actor  The actor that has been disconnected.
   */
  void OnObservedActorDisconnected( Actor* actor );

private:

  Stage& mStage; ///< Used to deliver touch events
  ActorObserver mLastPrimaryHitActor; ///< Stores the last primary point hit actor
  ActorObserver mLastConsumedActor; ///< Stores the last consumed actor
  ActorObserver mTouchDownConsumedActor; ///< Stores the touch-down consumed actor
  Dali::RenderTask mLastRenderTask; ///< The RenderTask used for the last hit actor
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_TOUCH_EVENT_PROCESSOR_H__
