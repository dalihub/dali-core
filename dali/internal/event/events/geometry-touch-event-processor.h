#ifndef DALI_INTERNAL_GEOMETRY_TOUCH_EVENT_PROCESSOR_H
#define DALI_INTERNAL_GEOMETRY_TOUCH_EVENT_PROCESSOR_H

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

// INTERNAL INCLUDES
#include <dali/internal/event/actors/actor-declarations.h>
#include <dali/internal/event/events/actor-observer.h>
#include <dali/internal/event/events/geometry-touch-stream-state.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>
#include <dali/public-api/events/point-state.h>
#include <dali/public-api/math/vector2.h>

#include <list>

namespace Dali
{
class Actor;
struct Vector2;
struct Vector4;

namespace Integration
{
struct TouchEvent;
} // namespace Integration

namespace Internal
{
class Actor;
class Scene;

struct GeometryTouchInitialHit
{
  ActorPtr            actor;
  RenderTaskPtr       renderTask;
  Vector2             actorCoordinates;
  std::list<ActorPtr> candidatesRootToFront;
};

/**
 * <h3>Multi-Touch Event Processing:</h3>
 *
 * The GeometryTouchEventProcessor processes one geometry-routed touch stream.
 *
 * Coordinate candidates remain eligible until one consumes an event. The consumer then
 * becomes the stable owner. Intercept traversal follows the owner's ancestor path, and
 * every previously active recipient displaced by a new owner receives one terminal event.
 * Recipient events retain the stream's initial hit actor while all point coordinates are
 * converted to the coordinate system of the actual recipient.
 *
 * Hit Testing & Touch Event Delivery are described in Dali::Actor.
 */
class GeometryTouchEventProcessor
{
public:
  /**
   * Create an event processor.
   * @param[in] scene The scene the event processor belongs to.
   */
  explicit GeometryTouchEventProcessor(Scene& scene);

  /**
   * Non-virtual destructor; GeometryTouchEventProcessor is not a base class
   */
  ~GeometryTouchEventProcessor();

  /**
   * This function is called by the event processor whenever a touch event occurs.
   * @param[in] event The touch event that has occurred.
   * @return true if consumed
   */
  bool ProcessTouchEvent(const Integration::TouchEvent& event, bool streamEnding, const GeometryTouchInitialHit* initialHit = nullptr);

  bool IsFinished() const;

private:
  // Not movable and not copyable
  GeometryTouchEventProcessor(const GeometryTouchEventProcessor&)            = delete;
  GeometryTouchEventProcessor(GeometryTouchEventProcessor&&)                 = delete;
  GeometryTouchEventProcessor& operator=(const GeometryTouchEventProcessor&) = delete;
  GeometryTouchEventProcessor& operator=(GeometryTouchEventProcessor&&)      = delete;

private:
  /**
   * Called by some actor-observers when the observed actor is disconnected.
   * @param[in]  actor  The actor that has been disconnected.
   */
  void OnObservedActorDisconnected(Actor* actor);

  /**
   * Clears the value.
   */
  void Clear(bool keepLastPrimaryObserver = false);

  Scene&                   mScene;                     ///< Used to deliver touch events
  ActorObserver            mLastPrimaryHitActor;       ///< Stores the last primary point hit actor
  ActorObserver            mLastConsumedActor;         ///< Stores the last consumed actor
  ActorObserver            mCapturingTouchActor;       ///< Stored the actor that captures touch
  ActorObserver            mOwnTouchActor;             ///< Stored the actor that own touch
  ActorObserver            mInterceptedTouchActor;     ///< Stores the intercepted actor
  RenderTaskPtr            mLastRenderTask;            ///< The RenderTask used for the last hit actor
  GeometryTouchStreamState mStreamState;               ///< Ordered state owned by this stream
  bool                     mProcessingTouchEvent;      ///< True while a routed event is being dispatched.
  bool                     mObservedActorDisconnected; ///< Defers terminal dispatch until the current callback returns.

  struct Impl;
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_GEOMETRY_TOUCH_EVENT_PROCESSOR_H
