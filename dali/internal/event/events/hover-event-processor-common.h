#ifndef DALI_INTERNAL_HOVER_EVENT_PROCESSOR_COMMON_H
#define DALI_INTERNAL_HOVER_EVENT_PROCESSOR_COMMON_H

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
#include <chrono>

// INTERNAL INCLUDES
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/events/hit-test-algorithm-impl.h>

namespace Dali::Internal::HoverEventProcessorCommon
{
inline constexpr const char* POINT_STATE_NAME[PointState::INTERRUPTED + 1] =
  {
    "STARTED",
    "FINISHED",
    "MOTION",
    "LEAVE",
    "STATIONARY",
    "INTERRUPTED",
};

/**
 * Used in the hit-test algorithm to check whether the actor is hoverable.
 */
struct ActorHoverableCheck : public HitTestAlgorithm::HitTestInterface
{
  bool IsActorHittable(Actor* actor) override
  {
    return actor->GetHoverRequired() && // Does the application or derived actor type require a hover event?
           actor->IsHittable();         // Is actor sensitive, visible and on the scene?
  }

  bool DescendActorHierarchy(Actor* actor) override
  {
    return (!actor->IsIgnored()) &&
           actor->IsVisible() && // Actor is visible, if not visible then none of its children are visible.
           actor->IsSensitive(); // Actor is sensitive, if insensitive none of its children should be hittable either.
  }

  bool DoesLayerConsumeHit(Layer* layer) override
  {
    return layer->IsHoverConsumed();
  }

  bool ActorRequiresHitResultCheck(Actor* actor, Vector2 hitPointLocal) override
  {
    // Hover event is always hit.
    return true;
  }
};

inline uint32_t GetMilliSeconds()
{
  // Get the time of a monotonic clock since its epoch.
  auto epoch = std::chrono::steady_clock::now().time_since_epoch();

  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);

  return static_cast<uint32_t>(duration.count());
}
} // namespace Dali::Internal::HoverEventProcessorCommon

#endif // DALI_INTERNAL_HOVER_EVENT_PROCESSOR_COMMON_H
