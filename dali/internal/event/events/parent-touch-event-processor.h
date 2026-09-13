#ifndef DALI_INTERNAL_PARENT_TOUCH_EVENT_PROCESSOR_H
#define DALI_INTERNAL_PARENT_TOUCH_EVENT_PROCESSOR_H

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

#include <dali/internal/event/actors/actor-declarations.h>
#include <dali/internal/event/events/actor-observer.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/public-api/events/point-state.h>

namespace DALI_NAMESPACE
{
namespace Integration
{
struct TouchEvent;
}

namespace Internal
{
class Scene;

/**
 * Processes touch events using TouchPropagationType::PARENT.
 */
class ParentTouchEventProcessor
{
public:
  explicit ParentTouchEventProcessor(Scene& scene);
  ~ParentTouchEventProcessor();

  ParentTouchEventProcessor(const ParentTouchEventProcessor&)            = delete;
  ParentTouchEventProcessor(ParentTouchEventProcessor&&)                 = delete;
  ParentTouchEventProcessor& operator=(const ParentTouchEventProcessor&) = delete;
  ParentTouchEventProcessor& operator=(ParentTouchEventProcessor&&)      = delete;

  bool ProcessTouchEvent(const Integration::TouchEvent& event);

private:
  void OnObservedActorDisconnected(Actor* actor);
  void Clear();

  Scene&           mScene;
  ActorObserver    mLastPrimaryHitActor;
  ActorObserver    mLastConsumedActor;
  ActorObserver    mCapturingTouchActor;
  ActorObserver    mOwnTouchActor;
  ActorObserver    mTouchDownConsumedActor;
  ActorObserver    mInterceptedTouchActor;
  RenderTaskPtr    mLastRenderTask;
  PointState::Type mLastPrimaryPointState;

  struct Impl;
};

} // namespace Internal
} //namespace DALI_NAMESPACE

#endif // DALI_INTERNAL_PARENT_TOUCH_EVENT_PROCESSOR_H
