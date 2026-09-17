#ifndef DALI_INTERNAL_GEOMETRY_TOUCH_STREAM_ROUTER_H
#define DALI_INTERNAL_GEOMETRY_TOUCH_STREAM_ROUTER_H

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

#include <dali/integration-api/events/point.h>
#include <dali/internal/event/events/geometry-touch-event-processor.h>

#include <cstdint>
#include <list>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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
 * Routes each device to the stream created from its initial geometry hit.
 *
 * The initial routing group of a point is the nearest hit candidate requiring a multi-touch
 * gesture (pinch or rotation), or the hit actor itself when there is none. Points with the same
 * initial routing group share a stream, while independent initial groups remain separate even
 * if they later select the same owner. A shared stream is delivered along the hit candidates
 * of its first point only. Device routes are detached individually on terminal points. Scene
 * touch boundaries are coordinated once from the original event after all routed actor
 * callbacks have completed.
 */
class GeometryTouchStreamRouter
{
public:
  explicit GeometryTouchStreamRouter(Scene& scene);
  ~GeometryTouchStreamRouter();

  GeometryTouchStreamRouter(const GeometryTouchStreamRouter&)            = delete;
  GeometryTouchStreamRouter(GeometryTouchStreamRouter&&)                 = delete;
  GeometryTouchStreamRouter& operator=(const GeometryTouchStreamRouter&) = delete;
  GeometryTouchStreamRouter& operator=(GeometryTouchStreamRouter&&)      = delete;

  void ProcessTouchEvent(const Integration::TouchEvent& touchEvent);
  void Clear();

private:
  using TouchStreamId = uint64_t;
  using DeviceId      = int32_t;
  using TouchPoints   = std::list<Integration::Point>;

  struct StreamEntry
  {
    std::unique_ptr<GeometryTouchEventProcessor> processor;
    std::unique_ptr<GeometryTouchInitialHit>     pendingInitialHit;
    ActorPtr                                     initialHitActor;
    uint32_t                                     initialRoutingGroup{0u};
    std::unordered_set<DeviceId>                 activeDeviceIds;
  };

  using TouchStreams        = std::unordered_map<TouchStreamId, StreamEntry>;
  using StreamTouchPoints   = std::unordered_map<TouchStreamId, TouchPoints>;
  using DeviceRoutes        = std::unordered_map<DeviceId, TouchStreamId>;
  using InitialStreamRoutes = std::unordered_map<uint32_t, TouchStreamId>;

  TouchStreamId              FindOrCreateStream(uint32_t initialRoutingGroup, std::unique_ptr<GeometryTouchInitialHit> initialHit);
  void                       CancelStream(TouchStreamId streamId, const Integration::Point& sourcePoint, unsigned long time);
  void                       RemoveStream(TouchStreamId streamId);
  bool                       WillFinishStream(const StreamEntry& stream, const TouchPoints& points) const;
  void                       DetachTerminalDevices(TouchStreamId streamId, const TouchPoints& points);
  void                       EmitSceneTouchEvent(const Integration::TouchEvent&                touchEvent,
                                                 const std::unordered_map<DeviceId, ActorPtr>& hitActors);
  std::vector<TouchStreamId> GetOrderedStreamIds() const;

  Scene&                       mScene;
  TouchStreamId                mNextStreamId;
  TouchStreams                 mTouchStreams;
  StreamTouchPoints            mStreamTouchPoints;
  DeviceRoutes                 mDeviceRoutes;
  InitialStreamRoutes          mInitialStreamRoutes;
  std::unordered_set<DeviceId> mSceneActiveDeviceIds;
};

} // namespace Internal
} //namespace DALI_NAMESPACE

#endif // DALI_INTERNAL_GEOMETRY_TOUCH_STREAM_ROUTER_H
