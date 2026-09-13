#ifndef DALI_INTERNAL_GEOMETRY_HOVER_EVENT_PROCESSOR_H
#define DALI_INTERNAL_GEOMETRY_HOVER_EVENT_PROCESSOR_H

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

// EXTERNAL INCLUDES
#include <list>

// INTERNAL INCLUDES
#include <dali/internal/event/actors/actor-declarations.h>
#include <dali/internal/event/events/actor-observer.h>

namespace DALI_NAMESPACE
{
namespace Integration
{
struct HoverEvent;
}

namespace Internal
{
class Scene;

/**
 * Processes front-to-back GEOMETRY hover candidate dispatch.
 */
class GeometryHoverEventProcessor
{
public:
  explicit GeometryHoverEventProcessor(Scene& scene);
  ~GeometryHoverEventProcessor();

  void ProcessHoverEvent(const Integration::HoverEvent& event);
  void SendInterruptedHoverEvent(Actor* actor);

  /**
   * @return true if this processor owns an active hover lifecycle.
   */
  bool HasActiveHover() const;

private:
  GeometryHoverEventProcessor(const GeometryHoverEventProcessor&)            = delete;
  GeometryHoverEventProcessor(GeometryHoverEventProcessor&&)                 = delete;
  GeometryHoverEventProcessor& operator=(const GeometryHoverEventProcessor&) = delete;
  GeometryHoverEventProcessor& operator=(GeometryHoverEventProcessor&&)      = delete;

  void ClearHoverTargets();
  void OnObservedActorDisconnected(Actor* actor);

private:
  Scene&                   mScene;
  std::list<ActorObserver> mHoverTargets;
  std::list<ActorObserver> mPreviousHoverTargets;
  std::list<ActorPtr>      mPendingInterrupts;
  std::list<ActorPtr>      mInvalidatedTargets;
  std::list<Actor*>        mTerminalizedTargets;
  Actor*                   mLastConsumedActor{nullptr};
  bool                     mProcessingHover{false};

  struct Impl;
};
} // namespace Internal
} //namespace DALI_NAMESPACE

#endif // DALI_INTERNAL_GEOMETRY_HOVER_EVENT_PROCESSOR_H
