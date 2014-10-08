#ifndef __DALI_INTERNAL_HOVER_EVENT_PROCESSOR_H__
#define __DALI_INTERNAL_HOVER_EVENT_PROCESSOR_H__

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
struct HoverEvent;
}

namespace Internal
{

class ActorObserver;
class Stage;

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
   * @param[in] stage The stage.
   */
  HoverEventProcessor( Stage& stage );

  /**
   * Non-virtual destructor; HoverEventProcessor is not a base class
   */
  ~HoverEventProcessor();

  /**
   * This function is called by the event processor whenever a hover event occurs.
   * @param[in] event The hover event that has occurred.
   */
  void ProcessHoverEvent( const Integration::HoverEvent& event );

private:

  // Undefined
  HoverEventProcessor(const HoverEventProcessor&);

  // Undefined
  HoverEventProcessor& operator=(const HoverEventProcessor& rhs);

  Stage& mStage; ///< Used to deliver touch events
  ActorObserver mLastPrimaryHitActor; ///< Stores the last primary point hit actor
  ActorObserver mLastConsumedActor; ///< Stores the last consumed actor
  ActorObserver mHoverStartConsumedActor; ///< Stores the hover-start consumed actor
  Dali::RenderTask mLastRenderTask; ///< The RenderTask used for the last hit actor
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_HOVER_EVENT_PROCESSOR_H__
