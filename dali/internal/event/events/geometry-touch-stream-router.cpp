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

#include <dali/internal/event/events/geometry-touch-stream-router.h>

#include <algorithm>

#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/events/hit-test-algorithm-impl.h>
#include <dali/internal/event/events/touch-event-impl.h>
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/events/touch-event.h>

namespace DALI_NAMESPACE::Internal
{
namespace
{
bool IsTerminalPoint(const Integration::Point& point)
{
  return point.GetState() == PointState::UP || point.GetState() == PointState::INTERRUPTED;
}
} // unnamed namespace

GeometryTouchStreamRouter::GeometryTouchStreamRouter(Scene& scene)
: mScene(scene),
  mNextStreamId(1u),
  mTouchStreams(),
  mStreamTouchPoints(),
  mDeviceRoutes(),
  mInitialStreamRoutes(),
  mSceneActiveDeviceIds()
{
}

GeometryTouchStreamRouter::~GeometryTouchStreamRouter()
{
  Clear();
}

GeometryTouchStreamRouter::TouchStreamId GeometryTouchStreamRouter::FindOrCreateStream(uint32_t initialRoutingGroup, std::unique_ptr<GeometryTouchInitialHit> initialHit)
{
  auto initialRoute = mInitialStreamRoutes.find(initialRoutingGroup);
  if(initialRoute != mInitialStreamRoutes.end())
  {
    auto stream = mTouchStreams.find(initialRoute->second);
    if(stream != mTouchStreams.end() && !stream->second.processor->IsFinished())
    {
      return initialRoute->second;
    }
    if(stream != mTouchStreams.end())
    {
      RemoveStream(stream->first);
    }
    else
    {
      mInitialStreamRoutes.erase(initialRoute);
    }
  }

  const TouchStreamId streamId = mNextStreamId++;
  StreamEntry         stream;
  stream.processor           = std::make_unique<GeometryTouchEventProcessor>(mScene);
  stream.initialHitActor     = initialHit->actor;
  stream.pendingInitialHit   = std::move(initialHit);
  stream.initialRoutingGroup = initialRoutingGroup;
  mTouchStreams.emplace(streamId, std::move(stream));
  mInitialStreamRoutes.emplace(initialRoutingGroup, streamId);
  return streamId;
}

void GeometryTouchStreamRouter::CancelStream(TouchStreamId streamId, const Integration::Point& sourcePoint, unsigned long time)
{
  auto stream = mTouchStreams.find(streamId);
  if(stream == mTouchStreams.end())
  {
    return;
  }

  Integration::Point interruptedPoint(sourcePoint);
  interruptedPoint.SetState(PointState::INTERRUPTED);
  Integration::TouchEvent interruptedEvent(time);
  interruptedEvent.AddPoint(interruptedPoint);
  stream->second.processor->ProcessTouchEvent(interruptedEvent, true);
  RemoveStream(streamId);
}

void GeometryTouchStreamRouter::RemoveStream(TouchStreamId streamId)
{
  auto stream = mTouchStreams.find(streamId);
  if(stream == mTouchStreams.end())
  {
    return;
  }

  for(DeviceId deviceId : stream->second.activeDeviceIds)
  {
    auto deviceRoute = mDeviceRoutes.find(deviceId);
    if(deviceRoute != mDeviceRoutes.end() && deviceRoute->second == streamId)
    {
      mDeviceRoutes.erase(deviceRoute);
    }
  }

  auto initialRoute = mInitialStreamRoutes.find(stream->second.initialRoutingGroup);
  if(initialRoute != mInitialStreamRoutes.end() && initialRoute->second == streamId)
  {
    mInitialStreamRoutes.erase(initialRoute);
  }
  mTouchStreams.erase(stream);
}

bool GeometryTouchStreamRouter::WillFinishStream(const StreamEntry& stream, const TouchPoints& points) const
{
  std::unordered_set<DeviceId> remainingDevices(stream.activeDeviceIds);
  for(const auto& point : points)
  {
    if(IsTerminalPoint(point))
    {
      remainingDevices.erase(point.GetDeviceId());
    }
  }
  return !stream.activeDeviceIds.empty() && remainingDevices.empty();
}

void GeometryTouchStreamRouter::DetachTerminalDevices(TouchStreamId streamId, const TouchPoints& points)
{
  auto stream = mTouchStreams.find(streamId);
  if(stream == mTouchStreams.end())
  {
    return;
  }

  for(const auto& point : points)
  {
    if(!IsTerminalPoint(point))
    {
      continue;
    }

    const DeviceId deviceId = point.GetDeviceId();
    stream->second.activeDeviceIds.erase(deviceId);
    auto deviceRoute = mDeviceRoutes.find(deviceId);
    if(deviceRoute != mDeviceRoutes.end() && deviceRoute->second == streamId)
    {
      mDeviceRoutes.erase(deviceRoute);
    }
  }
}

void GeometryTouchStreamRouter::ProcessTouchEvent(const Integration::TouchEvent& touchEvent)
{
  DALI_ASSERT_ALWAYS(!touchEvent.points.empty() && "Empty TouchEvent sent from Integration\n");
  std::unordered_map<DeviceId, ActorPtr> sceneHitActors;
  if(touchEvent.GetPoint(0).GetState() == PointState::INTERRUPTED)
  {
    for(const auto& point : touchEvent.points)
    {
      auto route = mDeviceRoutes.find(point.GetDeviceId());
      if(route != mDeviceRoutes.end())
      {
        auto stream = mTouchStreams.find(route->second);
        if(stream != mTouchStreams.end())
        {
          sceneHitActors.emplace(point.GetDeviceId(), stream->second.initialHitActor);
        }
      }
    }
    for(TouchStreamId streamId : GetOrderedStreamIds())
    {
      auto stream = mTouchStreams.find(streamId);
      if(stream != mTouchStreams.end())
      {
        stream->second.processor->ProcessTouchEvent(touchEvent, true);
      }
    }
    EmitSceneTouchEvent(touchEvent, sceneHitActors);
    Clear();
    return;
  }

  mStreamTouchPoints.clear();
  for(uint32_t i = 0u; i < touchEvent.GetPointCount(); ++i)
  {
    const Integration::Point& point    = touchEvent.GetPoint(i);
    const DeviceId            deviceId = point.GetDeviceId();
    if(point.GetState() == PointState::DOWN)
    {
      auto previousRoute = mDeviceRoutes.find(deviceId);
      if(previousRoute != mDeviceRoutes.end())
      {
        CancelStream(previousRoute->second, point, touchEvent.time);
      }

      HitTestAlgorithm::Results hitTestResults;
      hitTestResults.point     = point;
      hitTestResults.eventTime = touchEvent.time;
      HitTestAlgorithm::HitTest(mScene.GetSize(), mScene.GetRenderTaskList(), mScene.GetLayerList(), point.GetScreenPosition(), hitTestResults, nullptr, Integration::Scene::TouchPropagationType::GEOMETRY);

      if(hitTestResults.actor)
      {
        auto initialHit              = std::make_unique<GeometryTouchInitialHit>();
        initialHit->actor            = ActorPtr(&GetImplementation(hitTestResults.actor));
        initialHit->renderTask       = hitTestResults.renderTask;
        initialHit->actorCoordinates = hitTestResults.actorCoordinates;
        std::vector<Actor*> uniqueCandidates;
        for(auto candidate = hitTestResults.actorLists.rbegin(); candidate != hitTestResults.actorLists.rend(); ++candidate)
        {
          if(std::find(uniqueCandidates.begin(), uniqueCandidates.end(), *candidate) == uniqueCandidates.end())
          {
            uniqueCandidates.push_back(*candidate);
            initialHit->candidatesRootToFront.push_front(ActorPtr(*candidate));
          }
        }

        const uint32_t      initialRoutingGroup = initialHit->actor->GetId();
        const TouchStreamId streamId            = FindOrCreateStream(initialRoutingGroup, std::move(initialHit));
        auto                stream              = mTouchStreams.find(streamId);
        if(DALI_UNLIKELY(stream == mTouchStreams.end()))
        {
          DALI_LOG_ERROR("Failed to find geometry touch stream. streamId(%llu)\n", static_cast<unsigned long long>(streamId));
          continue;
        }

        stream->second.activeDeviceIds.insert(deviceId);
        mDeviceRoutes.insert_or_assign(deviceId, streamId);
      }
    }

    auto route = mDeviceRoutes.find(deviceId);
    if(route != mDeviceRoutes.end())
    {
      auto stream = mTouchStreams.find(route->second);
      if(stream != mTouchStreams.end() && !stream->second.processor->IsFinished())
      {
        mStreamTouchPoints[route->second].push_back(point);
        sceneHitActors.insert_or_assign(deviceId, stream->second.initialHitActor);
      }
      else
      {
        const TouchStreamId finishedStreamId = route->second;
        RemoveStream(finishedStreamId);
      }
    }
  }

  std::vector<TouchStreamId> routedStreamIds;
  routedStreamIds.reserve(mStreamTouchPoints.size());
  for(const auto& streamPoints : mStreamTouchPoints)
  {
    routedStreamIds.push_back(streamPoints.first);
  }
  std::sort(routedStreamIds.begin(), routedStreamIds.end());

  for(TouchStreamId streamId : routedStreamIds)
  {
    const TouchPoints& points = mStreamTouchPoints.find(streamId)->second;
    auto               stream = mTouchStreams.find(streamId);
    if(stream == mTouchStreams.end())
    {
      continue;
    }

    Integration::TouchEvent routedEvent(touchEvent.time);
    for(const auto& point : points)
    {
      routedEvent.AddPoint(point);
    }

    const bool streamEnding = WillFinishStream(stream->second, points);
    stream->second.processor->ProcessTouchEvent(routedEvent, streamEnding, stream->second.pendingInitialHit.get());
    stream->second.pendingInitialHit.reset();

    if(stream->second.processor->IsFinished())
    {
      RemoveStream(streamId);
      continue;
    }

    DetachTerminalDevices(streamId, points);

    stream = mTouchStreams.find(streamId);
    if(stream != mTouchStreams.end() && stream->second.activeDeviceIds.empty())
    {
      RemoveStream(streamId);
    }
  }

  EmitSceneTouchEvent(touchEvent, sceneHitActors);
}

std::vector<GeometryTouchStreamRouter::TouchStreamId> GeometryTouchStreamRouter::GetOrderedStreamIds() const
{
  std::vector<TouchStreamId> streamIds;
  streamIds.reserve(mTouchStreams.size());
  for(const auto& stream : mTouchStreams)
  {
    streamIds.push_back(stream.first);
  }
  std::sort(streamIds.begin(), streamIds.end());
  return streamIds;
}

void GeometryTouchStreamRouter::EmitSceneTouchEvent(const Integration::TouchEvent&                touchEvent,
                                                    const std::unordered_map<DeviceId, ActorPtr>& hitActors)
{
  const bool hadActiveDevices = !mSceneActiveDeviceIds.empty();
  bool       interrupted      = false;
  for(const auto& point : touchEvent.points)
  {
    switch(point.GetState())
    {
      case PointState::DOWN:
        mSceneActiveDeviceIds.insert(point.GetDeviceId());
        break;
      case PointState::UP:
        mSceneActiveDeviceIds.erase(point.GetDeviceId());
        break;
      case PointState::INTERRUPTED:
        interrupted = true;
        break;
      default:
        break;
    }
  }

  if(interrupted)
  {
    mSceneActiveDeviceIds.clear();
  }

  const bool firstDown = !hadActiveDevices && !mSceneActiveDeviceIds.empty();
  const bool lastUp    = hadActiveDevices && mSceneActiveDeviceIds.empty();
  if(!firstDown && !lastUp && !interrupted)
  {
    return;
  }

  TouchEventPtr sceneEvent(new TouchEvent(touchEvent.time));
  for(const auto& rawPoint : touchEvent.points)
  {
    Integration::Point scenePoint(rawPoint);
    auto               hitActor = hitActors.find(rawPoint.GetDeviceId());
    scenePoint.SetHitActor(hitActor == hitActors.end() ? Dali::Actor() : Dali::Actor(hitActor->second.Get()));
    sceneEvent->AddPoint(scenePoint);
  }
  mScene.EmitTouchEventSignal(Dali::TouchEvent(sceneEvent.Get()));
}

void GeometryTouchStreamRouter::Clear()
{
  mStreamTouchPoints.clear();
  mDeviceRoutes.clear();
  mInitialStreamRoutes.clear();
  mTouchStreams.clear();
  mSceneActiveDeviceIds.clear();
}

} //namespace DALI_NAMESPACE::Internal
