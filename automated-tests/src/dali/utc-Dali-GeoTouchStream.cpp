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

#include <dali-test-suite-utils.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/touch-integ.h>
#include <dali/integration-api/scene.h>
#include <dali/public-api/dali-core.h>

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

using namespace Dali;

void utc_dali_geo_touch_stream_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_geo_touch_stream_cleanup(void)
{
  test_return_value = TET_PASS;
}

namespace
{
enum class CallbackKind
{
  INTERCEPT,
  TOUCH,
  SCENE,
};

struct TraceEntry
{
  std::string                   receiver;
  CallbackKind                  kind{CallbackKind::TOUCH};
  std::vector<int32_t>          deviceIds;
  std::vector<PointState::Type> states;
  std::vector<int32_t>          hitActorIds;
  std::vector<Vector2>          localPositions;
  std::vector<Vector2>          screenPositions;
};

struct TouchTrace
{
  void Record(const char* receiver, CallbackKind kind, TouchEvent touch)
  {
    TraceEntry entry;
    entry.receiver = receiver;
    entry.kind     = kind;
    for(uint32_t i = 0u; i < touch.GetPointCount(); ++i)
    {
      Actor hitActor = touch.GetHitActor(i);
      entry.deviceIds.push_back(touch.GetDeviceId(i));
      entry.states.push_back(touch.GetState(i));
      entry.hitActorIds.push_back(hitActor ? hitActor.GetProperty<int32_t>(Actor::Property::ID) : -1);
      entry.localPositions.push_back(touch.GetLocalPosition(i));
      entry.screenPositions.push_back(touch.GetScreenPosition(i));
    }
    entries.push_back(std::move(entry));
  }

  uint32_t Count(const char* receiver, CallbackKind kind, PointState::Type state) const
  {
    uint32_t count = 0u;
    for(const auto& entry : entries)
    {
      if(entry.receiver == receiver && entry.kind == kind && !entry.states.empty() && entry.states[0] == state)
      {
        ++count;
      }
    }
    return count;
  }

  size_t FirstIndexOf(const char* receiver, CallbackKind kind, PointState::Type state) const
  {
    for(size_t i = 0u; i < entries.size(); ++i)
    {
      const auto& entry = entries[i];
      if(entry.receiver == receiver && entry.kind == kind && !entry.states.empty() && entry.states[0] == state)
      {
        return i;
      }
    }
    return entries.size();
  }

  const TraceEntry* Find(const char* receiver, CallbackKind kind, PointState::Type state) const
  {
    const size_t index = FirstIndexOf(receiver, kind, state);
    return index == entries.size() ? nullptr : &entries[index];
  }

  uint32_t CountDevice(const char* receiver, CallbackKind kind, PointState::Type state, int32_t deviceId) const
  {
    uint32_t count = 0u;
    for(const auto& entry : entries)
    {
      if(entry.receiver != receiver || entry.kind != kind)
      {
        continue;
      }
      for(size_t i = 0u; i < entry.deviceIds.size(); ++i)
      {
        if(entry.deviceIds[i] == deviceId && entry.states[i] == state)
        {
          ++count;
          break;
        }
      }
    }
    return count;
  }

  void Clear()
  {
    entries.clear();
  }

  std::vector<TraceEntry> entries;
};

struct TouchTraceFunctor
{
  TouchTraceFunctor(TouchTrace& trace, const char* receiver, bool consume, CallbackKind kind = CallbackKind::TOUCH)
  : trace(trace),
    receiver(receiver),
    consume(consume),
    kind(kind)
  {
  }

  bool operator()(Actor, TouchEvent touch)
  {
    trace.Record(receiver, kind, touch);
    return consume;
  }

  TouchTrace&  trace;
  const char*  receiver;
  bool         consume;
  CallbackKind kind;
};

struct MutableTouchTraceFunctor
{
  MutableTouchTraceFunctor(TouchTrace& trace, const char* receiver, bool& consume)
  : trace(trace),
    receiver(receiver),
    consume(consume)
  {
  }

  bool operator()(Actor, TouchEvent touch)
  {
    trace.Record(receiver, CallbackKind::TOUCH, touch);
    return consume;
  }

  TouchTrace& trace;
  const char* receiver;
  bool&       consume;
};

struct RemoveSelfOnMotionFunctor
{
  RemoveSelfOnMotionFunctor(TouchTrace& trace, const char* receiver)
  : trace(trace),
    receiver(receiver)
  {
  }

  bool operator()(Actor actor, TouchEvent touch)
  {
    trace.Record(receiver, CallbackKind::TOUCH, touch);
    if(touch.GetState(0u) == PointState::MOTION)
    {
      actor.Unparent();
    }
    return touch.GetState(0u) == PointState::DOWN;
  }

  TouchTrace& trace;
  const char* receiver;
};

struct RemoveSelfOnDownFunctor
{
  RemoveSelfOnDownFunctor(TouchTrace& trace, const char* receiver)
  : trace(trace),
    receiver(receiver)
  {
  }

  bool operator()(Actor actor, TouchEvent touch)
  {
    trace.Record(receiver, CallbackKind::TOUCH, touch);
    if(touch.GetState(0u) == PointState::DOWN)
    {
      actor.Unparent();
    }
    return false;
  }

  TouchTrace& trace;
  const char* receiver;
};

struct SceneTraceFunctor
{
  SceneTraceFunctor(TouchTrace& trace, const char* receiver)
  : trace(trace),
    receiver(receiver)
  {
  }

  void operator()(TouchEvent touch)
  {
    trace.Record(receiver, CallbackKind::SCENE, touch);
  }

  TouchTrace& trace;
  const char* receiver;
};

Actor CreateTouchableActor(const char* name)
{
  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::NAME, name);
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  return actor;
}

Integration::TouchEvent GenerateSingleTouch(PointState::Type state, const Vector2& screenPosition, int32_t deviceId = 1)
{
  Integration::TouchEvent touchEvent;
  Integration::Point      point;
  point.SetState(state);
  point.SetDeviceId(deviceId);
  point.SetScreenPosition(screenPosition);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);
  return touchEvent;
}

Integration::TouchEvent GenerateDoubleTouch(PointState::Type state, const Vector2& screenPosition)
{
  Integration::TouchEvent touchEvent;
  Integration::Point      point;
  point.SetState(state);
  point.SetDeviceId(4);
  point.SetScreenPosition(screenPosition);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);
  point.SetDeviceId(7);
  touchEvent.points.push_back(point);
  return touchEvent;
}

Integration::TouchEvent GenerateTwoTouches(PointState::Type firstState,
                                           const Vector2&   firstPosition,
                                           int32_t          firstDeviceId,
                                           PointState::Type secondState,
                                           const Vector2&   secondPosition,
                                           int32_t          secondDeviceId)
{
  Integration::TouchEvent touchEvent;
  Integration::Point      point;
  point.SetState(firstState);
  point.SetDeviceId(firstDeviceId);
  point.SetScreenPosition(firstPosition);
  point.SetDeviceClass(Device::Class::TOUCH);
  point.SetDeviceSubclass(Device::Subclass::NONE);
  touchEvent.points.push_back(point);
  point.SetState(secondState);
  point.SetDeviceId(secondDeviceId);
  point.SetScreenPosition(secondPosition);
  touchEvent.points.push_back(point);
  return touchEvent;
}

void PrepareScene(TestApplication& application)
{
  application.GetScene().SetGeometryHittestEnabled(true);
  application.SendNotification();
  application.Render();
}
} // namespace

int UtcDaliGeoTouchStreamInterruptedConsumedCharacterization(void)
{
  TestApplication application;
  TouchTrace      trace;

  Actor actor = CreateTouchableActor("actor");
  application.GetScene().Add(actor);
  TouchTraceFunctor actorFunctor(trace, "actor", true);
  SceneTraceFunctor sceneFunctor(trace, "scene");
  actor.TouchEventSignal().Connect(&application, actorFunctor);
  application.GetScene().TouchEventSignal().Connect(&application, sceneFunctor);
  PrepareScene(application);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  trace.Clear();
  application.ProcessEvent(GenerateSingleTouch(PointState::INTERRUPTED, Vector2(10.0f, 10.0f)));

  DALI_TEST_EQUALS(1u, trace.Count("actor", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("scene", CallbackKind::SCENE, PointState::INTERRUPTED), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamInterruptedUnconsumedCharacterization(void)
{
  TestApplication application;
  TouchTrace      trace;

  Actor actor = CreateTouchableActor("actor");
  application.GetScene().Add(actor);
  TouchTraceFunctor actorFunctor(trace, "actor", false);
  SceneTraceFunctor sceneFunctor(trace, "scene");
  actor.TouchEventSignal().Connect(&application, actorFunctor);
  application.GetScene().TouchEventSignal().Connect(&application, sceneFunctor);
  PrepareScene(application);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  trace.Clear();
  application.ProcessEvent(GenerateSingleTouch(PointState::INTERRUPTED, Vector2(10.0f, 10.0f)));

  DALI_TEST_EQUALS(1u, trace.Count("actor", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("scene", CallbackKind::SCENE, PointState::INTERRUPTED), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamInterruptedInterceptPrecedesRecipientTermination(void)
{
  TestApplication application;
  TouchTrace      trace;

  Actor root  = application.GetScene().GetRootLayer();
  Actor actor = CreateTouchableActor("actor");
  application.GetScene().Add(actor);

  TouchTraceFunctor actorFunctor(trace, "actor", false);
  actor.TouchEventSignal().Connect(&application, actorFunctor);
  PrepareScene(application);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  trace.Clear();

  TouchTraceFunctor rootIntercept(trace, "root-intercept", true, CallbackKind::INTERCEPT);
  root.InterceptTouchEventSignal().Connect(&application, rootIntercept);
  application.ProcessEvent(GenerateSingleTouch(PointState::INTERRUPTED, Vector2(200.0f, 200.0f)));

  DALI_TEST_EQUALS(1u, trace.Count("root-intercept", CallbackKind::INTERCEPT, PointState::INTERRUPTED), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("actor", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  DALI_TEST_CHECK(trace.FirstIndexOf("root-intercept", CallbackKind::INTERCEPT, PointState::INTERRUPTED) <
                  trace.FirstIndexOf("actor", CallbackKind::TOUCH, PointState::INTERRUPTED));

  trace.Clear();
  application.ProcessEvent(GenerateSingleTouch(PointState::INTERRUPTED, Vector2(200.0f, 200.0f)));
  DALI_TEST_EQUALS(0u, trace.Count("root-intercept", CallbackKind::INTERCEPT, PointState::INTERRUPTED), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, trace.Count("actor", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamSameEventConsumerTrace(void)
{
  TestApplication application;
  TouchTrace      trace;

  Actor parent = CreateTouchableActor("parent");
  Actor child  = CreateTouchableActor("child");
  parent.Add(child);
  application.GetScene().Add(parent);

  TouchTraceFunctor childFunctor(trace, "child", false);
  TouchTraceFunctor parentFunctor(trace, "parent", true);
  child.TouchEventSignal().Connect(&application, childFunctor);
  parent.TouchEventSignal().Connect(&application, parentFunctor);
  PrepareScene(application);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));

  DALI_TEST_EQUALS(1u, trace.Count("child", CallbackKind::TOUCH, PointState::DOWN), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("parent", CallbackKind::TOUCH, PointState::DOWN), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("child", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  DALI_TEST_CHECK(trace.FirstIndexOf("child", CallbackKind::TOUCH, PointState::DOWN) < trace.FirstIndexOf("parent", CallbackKind::TOUCH, PointState::DOWN));
  DALI_TEST_CHECK(trace.FirstIndexOf("parent", CallbackKind::TOUCH, PointState::DOWN) < trace.FirstIndexOf("child", CallbackKind::TOUCH, PointState::INTERRUPTED));
  END_TEST;
}

int UtcDaliGeoTouchStreamLateConsumerTrace(void)
{
  TestApplication application;
  TouchTrace      trace;

  Actor root(application.GetScene().GetRootLayer());
  Actor parent = CreateTouchableActor("parent");
  Actor child  = CreateTouchableActor("child");
  parent.Add(child);
  application.GetScene().Add(parent);

  TouchTraceFunctor childObserver(trace, "child-observer", false);
  TouchTraceFunctor parentObserver(trace, "parent", false);
  TouchTraceFunctor rootObserver(trace, "root", false);
  child.TouchEventSignal().Connect(&application, childObserver);
  parent.TouchEventSignal().Connect(&application, parentObserver);
  root.TouchEventSignal().Connect(&application, rootObserver);
  PrepareScene(application);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  trace.Clear();

  TouchTraceFunctor childConsumer(trace, "child-consumer", true);
  child.TouchEventSignal().Connect(&application, childConsumer);
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 20.0f)));

  DALI_TEST_EQUALS(1u, trace.Count("child-observer", CallbackKind::TOUCH, PointState::MOTION), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("child-consumer", CallbackKind::TOUCH, PointState::MOTION), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("parent", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("root", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  DALI_TEST_CHECK(trace.FirstIndexOf("child-consumer", CallbackKind::TOUCH, PointState::MOTION) < trace.FirstIndexOf("parent", CallbackKind::TOUCH, PointState::INTERRUPTED));
  END_TEST;
}

int UtcDaliGeoTouchStreamOwnerRemovalCharacterization(void)
{
  TestApplication application;
  TouchTrace      trace;

  Actor parent = CreateTouchableActor("parent");
  Actor child  = CreateTouchableActor("child");
  parent.Add(child);
  application.GetScene().Add(parent);

  TouchTraceFunctor childFunctor(trace, "child", false);
  TouchTraceFunctor parentFunctor(trace, "parent", true);
  child.TouchEventSignal().Connect(&application, childFunctor);
  parent.TouchEventSignal().Connect(&application, parentFunctor);
  PrepareScene(application);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(1u, trace.Count("child", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  trace.Clear();

  child.Unparent();

  DALI_TEST_EQUALS(0u, trace.Count("child", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("parent", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamTwoPointUpEndsStream(void)
{
  TestApplication application;
  TouchTrace      trace;

  Actor actor = CreateTouchableActor("actor");
  application.GetScene().Add(actor);
  TouchTraceFunctor actorFunctor(trace, "actor", true);
  actor.TouchEventSignal().Connect(&application, actorFunctor);
  PrepareScene(application);

  // Both devices go up within one event, so the stream ends and its route is detached. A later raw
  // interruption has nothing left to terminate.
  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  application.ProcessEvent(GenerateDoubleTouch(PointState::UP, Vector2(10.0f, 10.0f)));
  trace.Clear();

  application.ProcessEvent(GenerateSingleTouch(PointState::INTERRUPTED, Vector2(10.0f, 10.0f), 4));

  DALI_TEST_EQUALS(0u, trace.Count("actor", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamInsensitiveRecipientInterruptedExactlyOnce(void)
{
  TestApplication application;
  TouchTrace      trace;

  Actor actor = CreateTouchableActor("actor");
  application.GetScene().Add(actor);
  TouchTraceFunctor actorFunctor(trace, "actor", false);
  actor.TouchEventSignal().Connect(&application, actorFunctor);
  PrepareScene(application);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  trace.Clear();

  actor.SetProperty(Actor::Property::SENSITIVE, false);
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 20.0f)));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(30.0f, 30.0f)));

  DALI_TEST_EQUALS(1u, trace.Count("actor", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamDoesNotInterruptTerminatedRecipientAgain(void)
{
  TestApplication application;
  TouchTrace      trace;

  Actor parent = CreateTouchableActor("parent");
  Actor child  = CreateTouchableActor("child");
  parent.Add(child);
  application.GetScene().Add(parent);

  TouchTraceFunctor childFunctor(trace, "child", false);
  TouchTraceFunctor parentFunctor(trace, "parent", true);
  child.TouchEventSignal().Connect(&application, childFunctor);
  parent.TouchEventSignal().Connect(&application, parentFunctor);
  PrepareScene(application);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(1u, trace.Count("child", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  trace.Clear();

  application.ProcessEvent(GenerateSingleTouch(PointState::INTERRUPTED, Vector2(20.0f, 20.0f)));

  DALI_TEST_EQUALS(0u, trace.Count("child", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("parent", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamOwnerRemainsStableWhenCallbackReturnsFalse(void)
{
  TestApplication application;
  TouchTrace      trace;

  Actor actor = CreateTouchableActor("actor");
  application.GetScene().Add(actor);
  bool                     consume = true;
  MutableTouchTraceFunctor actorFunctor(trace, "actor", consume);
  actor.TouchEventSignal().Connect(&application, actorFunctor);
  PrepareScene(application);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  trace.Clear();

  consume = false;
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 20.0f)));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(30.0f, 30.0f)));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(30.0f, 30.0f)));

  DALI_TEST_EQUALS(2u, trace.Count("actor", CallbackKind::TOUCH, PointState::MOTION), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("actor", CallbackKind::TOUCH, PointState::UP), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamInterceptStopsBeforeCurrentOwner(void)
{
  TestApplication application;
  TouchTrace      trace;

  Actor root(application.GetScene().GetRootLayer());
  Actor parent = CreateTouchableActor("parent");
  Actor child  = CreateTouchableActor("child");
  parent.Add(child);
  application.GetScene().Add(parent);

  TouchTraceFunctor childTouch(trace, "child-touch", true);
  TouchTraceFunctor rootIntercept(trace, "root-intercept", false, CallbackKind::INTERCEPT);
  TouchTraceFunctor parentIntercept(trace, "parent-intercept", false, CallbackKind::INTERCEPT);
  TouchTraceFunctor childIntercept(trace, "child-intercept", false, CallbackKind::INTERCEPT);
  child.TouchEventSignal().Connect(&application, childTouch);
  root.InterceptTouchEventSignal().Connect(&application, rootIntercept);
  parent.InterceptTouchEventSignal().Connect(&application, parentIntercept);
  child.InterceptTouchEventSignal().Connect(&application, childIntercept);
  PrepareScene(application);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  trace.Clear();
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 20.0f)));

  DALI_TEST_EQUALS(1u, trace.Count("root-intercept", CallbackKind::INTERCEPT, PointState::MOTION), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("parent-intercept", CallbackKind::INTERCEPT, PointState::MOTION), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, trace.Count("child-intercept", CallbackKind::INTERCEPT, PointState::MOTION), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("child-touch", CallbackKind::TOUCH, PointState::MOTION), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamCallbackRemovalTerminatesExactlyOnce(void)
{
  TestApplication application;
  TouchTrace      trace;

  Actor actor = CreateTouchableActor("actor");
  application.GetScene().Add(actor);
  RemoveSelfOnMotionFunctor actorFunctor(trace, "actor");
  actor.TouchEventSignal().Connect(&application, actorFunctor);
  PrepareScene(application);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  trace.Clear();

  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 20.0f)));
  application.ProcessEvent(GenerateSingleTouch(PointState::INTERRUPTED, Vector2(20.0f, 20.0f)));

  DALI_TEST_EQUALS(1u, trace.Count("actor", CallbackKind::TOUCH, PointState::MOTION), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("actor", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamDownCallbackRemovalTerminatesRemainingOwner(void)
{
  TestApplication application;
  TouchTrace      trace;

  Actor parent = CreateTouchableActor("parent");
  Actor child  = CreateTouchableActor("child");
  parent.Add(child);
  application.GetScene().Add(parent);

  RemoveSelfOnDownFunctor childFunctor(trace, "child");
  TouchTraceFunctor       parentFunctor(trace, "parent", true);
  child.TouchEventSignal().Connect(&application, childFunctor);
  parent.TouchEventSignal().Connect(&application, parentFunctor);
  PrepareScene(application);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));

  DALI_TEST_EQUALS(1u, trace.Count("child", CallbackKind::TOUCH, PointState::DOWN), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("child", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("parent", CallbackKind::TOUCH, PointState::DOWN), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("parent", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamPartialUpKeepsRemainingDeviceActive(void)
{
  TestApplication application;
  TouchTrace      trace;

  Actor actor = CreateTouchableActor("actor");
  application.GetScene().Add(actor);
  TouchTraceFunctor actorFunctor(trace, "actor", true);
  actor.TouchEventSignal().Connect(&application, actorFunctor);
  PrepareScene(application);

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  trace.Clear();

  application.ProcessEvent(GenerateTwoTouches(PointState::UP,
                                              Vector2(10.0f, 10.0f),
                                              4,
                                              PointState::STATIONARY,
                                              Vector2(20.0f, 20.0f),
                                              7));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(30.0f, 30.0f), 7));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(30.0f, 30.0f), 7));

  DALI_TEST_EQUALS(1u, trace.CountDevice("actor", CallbackKind::TOUCH, PointState::UP, 4), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.CountDevice("actor", CallbackKind::TOUCH, PointState::MOTION, 7), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.CountDevice("actor", CallbackKind::TOUCH, PointState::UP, 7), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamNewDownInterruptsMappedDevice(void)
{
  TestApplication application;
  TouchTrace      trace;

  Actor actor = CreateTouchableActor("actor");
  application.GetScene().Add(actor);
  TouchTraceFunctor actorFunctor(trace, "actor", true);
  actor.TouchEventSignal().Connect(&application, actorFunctor);
  PrepareScene(application);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f), 9));
  trace.Clear();
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 9));

  DALI_TEST_EQUALS(1u, trace.Count("actor", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("actor", CallbackKind::TOUCH, PointState::DOWN), TEST_LOCATION);
  DALI_TEST_CHECK(trace.FirstIndexOf("actor", CallbackKind::TOUCH, PointState::INTERRUPTED) < trace.FirstIndexOf("actor", CallbackKind::TOUCH, PointState::DOWN));
  END_TEST;
}

int UtcDaliGeoTouchStreamSameInitialRouteSharesOneSubset(void)
{
  TestApplication application;
  TouchTrace      trace;

  Actor actor = CreateTouchableActor("actor");
  application.GetScene().Add(actor);
  TouchTraceFunctor actorFunctor(trace, "actor", true);
  actor.TouchEventSignal().Connect(&application, actorFunctor);
  PrepareScene(application);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f), 4));
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 7));
  trace.Clear();

  application.ProcessEvent(GenerateTwoTouches(PointState::MOTION,
                                              Vector2(30.0f, 30.0f),
                                              4,
                                              PointState::STATIONARY,
                                              Vector2(40.0f, 40.0f),
                                              7));

  DALI_TEST_EQUALS(1u, trace.Count("actor", CallbackKind::TOUCH, PointState::MOTION), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.CountDevice("actor", CallbackKind::TOUCH, PointState::MOTION, 4), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.CountDevice("actor", CallbackKind::TOUCH, PointState::STATIONARY, 7), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamDifferentRoutesSameOwnerRemainIndependent(void)
{
  TestApplication application;
  TouchTrace      trace;

  Actor root(application.GetScene().GetRootLayer());
  Actor left  = CreateTouchableActor("left");
  Actor right = CreateTouchableActor("right");
  right.SetProperty(Actor::Property::POSITION, Vector2(120.0f, 0.0f));
  application.GetScene().Add(left);
  application.GetScene().Add(right);

  TouchTraceFunctor leftFunctor(trace, "left", false);
  TouchTraceFunctor rightFunctor(trace, "right", false);
  TouchTraceFunctor rootFunctor(trace, "root", true);
  left.TouchEventSignal().Connect(&application, leftFunctor);
  right.TouchEventSignal().Connect(&application, rightFunctor);
  root.TouchEventSignal().Connect(&application, rootFunctor);
  PrepareScene(application);

  application.ProcessEvent(GenerateTwoTouches(PointState::DOWN,
                                              Vector2(10.0f, 10.0f),
                                              4,
                                              PointState::DOWN,
                                              Vector2(130.0f, 10.0f),
                                              7));
  trace.Clear();

  application.ProcessEvent(GenerateTwoTouches(PointState::MOTION,
                                              Vector2(20.0f, 20.0f),
                                              4,
                                              PointState::MOTION,
                                              Vector2(140.0f, 20.0f),
                                              7));

  DALI_TEST_EQUALS(2u, trace.Count("root", CallbackKind::TOUCH, PointState::MOTION), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.CountDevice("root", CallbackKind::TOUCH, PointState::MOTION, 4), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.CountDevice("root", CallbackKind::TOUCH, PointState::MOTION, 7), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, trace.Count("left", CallbackKind::TOUCH, PointState::MOTION), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, trace.Count("right", CallbackKind::TOUCH, PointState::MOTION), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamInterceptUsesCurrentOwnerAncestry(void)
{
  TestApplication application;
  TouchTrace      trace;

  Actor parentA = CreateTouchableActor("parent-a");
  Actor actorA  = CreateTouchableActor("actor-a");
  parentA.Add(actorA);
  application.GetScene().Add(parentA);

  Actor parentB = CreateTouchableActor("parent-b");
  Actor actorB  = CreateTouchableActor("actor-b");
  parentB.Add(actorB);
  application.GetScene().Add(parentB);

  TouchTraceFunctor actorAFunctor(trace, "actor-a", true);
  TouchTraceFunctor actorBFunctor(trace, "actor-b", false);
  actorA.TouchEventSignal().Connect(&application, actorAFunctor);
  actorB.TouchEventSignal().Connect(&application, actorBFunctor);
  PrepareScene(application);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  DALI_TEST_EQUALS(1u, trace.Count("actor-b", CallbackKind::TOUCH, PointState::DOWN), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("actor-a", CallbackKind::TOUCH, PointState::DOWN), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("actor-b", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);

  TouchTraceFunctor parentAIntercept(trace, "parent-a-intercept", false, CallbackKind::INTERCEPT);
  TouchTraceFunctor parentBIntercept(trace, "parent-b-intercept", false, CallbackKind::INTERCEPT);
  parentA.InterceptTouchEventSignal().Connect(&application, parentAIntercept);
  parentB.InterceptTouchEventSignal().Connect(&application, parentBIntercept);
  trace.Clear();

  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 20.0f)));

  DALI_TEST_EQUALS(1u, trace.Count("parent-a-intercept", CallbackKind::INTERCEPT, PointState::MOTION), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, trace.Count("parent-b-intercept", CallbackKind::INTERCEPT, PointState::MOTION), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("actor-a", CallbackKind::TOUCH, PointState::MOTION), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamAllPointsUseRecipientCoordinatesAndInitialHit(void)
{
  TestApplication application;
  TouchTrace      trace;

  Actor parent = CreateTouchableActor("parent");
  parent.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  parent.SetProperty(Actor::Property::POSITION, Vector2(30.0f, 40.0f));
  Actor child = CreateTouchableActor("child");
  child.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  child.SetProperty(Actor::Property::POSITION, Vector2(20.0f, 15.0f));
  parent.Add(child);
  application.GetScene().Add(parent);

  TouchTraceFunctor childFunctor(trace, "child", false);
  TouchTraceFunctor parentFunctor(trace, "parent", true);
  child.TouchEventSignal().Connect(&application, childFunctor);
  parent.TouchEventSignal().Connect(&application, parentFunctor);
  PrepareScene(application);

  const Vector2 firstPosition(60.0f, 65.0f);
  const Vector2 secondPosition(80.0f, 85.0f);
  application.ProcessEvent(GenerateTwoTouches(PointState::DOWN,
                                              firstPosition,
                                              4,
                                              PointState::DOWN,
                                              secondPosition,
                                              7));

  const TraceEntry* parentDown = trace.Find("parent", CallbackKind::TOUCH, PointState::DOWN);
  DALI_TEST_CHECK(parentDown);
  DALI_TEST_EQUALS(2u, parentDown->localPositions.size(), TEST_LOCATION);
  const int32_t childId = child.GetProperty<int32_t>(Actor::Property::ID);
  Vector2       expectedFirst;
  Vector2       expectedSecond;
  parent.ScreenToLocal(expectedFirst.x, expectedFirst.y, firstPosition.x, firstPosition.y);
  parent.ScreenToLocal(expectedSecond.x, expectedSecond.y, secondPosition.x, secondPosition.y);
  DALI_TEST_EQUALS(expectedFirst, parentDown->localPositions[0], 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedSecond, parentDown->localPositions[1], 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(childId, parentDown->hitActorIds[0], TEST_LOCATION);
  DALI_TEST_EQUALS(childId, parentDown->hitActorIds[1], TEST_LOCATION);

  trace.Clear();
  const Vector2 movedFirst(65.0f, 70.0f);
  const Vector2 movedSecond(85.0f, 90.0f);
  application.ProcessEvent(GenerateTwoTouches(PointState::MOTION,
                                              movedFirst,
                                              4,
                                              PointState::STATIONARY,
                                              movedSecond,
                                              7));
  const TraceEntry* parentMotion = trace.Find("parent", CallbackKind::TOUCH, PointState::MOTION);
  DALI_TEST_CHECK(parentMotion);
  parent.ScreenToLocal(expectedFirst.x, expectedFirst.y, movedFirst.x, movedFirst.y);
  parent.ScreenToLocal(expectedSecond.x, expectedSecond.y, movedSecond.x, movedSecond.y);
  DALI_TEST_EQUALS(expectedFirst, parentMotion->localPositions[0], 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(expectedSecond, parentMotion->localPositions[1], 0.1f, TEST_LOCATION);
  DALI_TEST_EQUALS(childId, parentMotion->hitActorIds[0], TEST_LOCATION);
  DALI_TEST_EQUALS(childId, parentMotion->hitActorIds[1], TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamMixedMotionAndTerminalBypassesMotionFilter(void)
{
  TestApplication application;
  TouchTrace      trace;

  Actor actor = CreateTouchableActor("actor");
  actor.SetProperty(Actor::Property::DISPATCH_TOUCH_MOTION, false);
  application.GetScene().Add(actor);
  TouchTraceFunctor actorFunctor(trace, "actor", true);
  actor.TouchEventSignal().Connect(&application, actorFunctor);
  PrepareScene(application);

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  trace.Clear();

  application.ProcessEvent(GenerateTwoTouches(PointState::MOTION,
                                              Vector2(20.0f, 20.0f),
                                              4,
                                              PointState::UP,
                                              Vector2(25.0f, 25.0f),
                                              7));

  const TraceEntry* mixed = trace.Find("actor", CallbackKind::TOUCH, PointState::MOTION);
  DALI_TEST_CHECK(mixed);
  DALI_TEST_EQUALS(2u, mixed->states.size(), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::UP, mixed->states[1], TEST_LOCATION);

  trace.Clear();
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(30.0f, 30.0f), 4));
  DALI_TEST_EQUALS(0u, trace.Count("actor", CallbackKind::TOUCH, PointState::MOTION), TEST_LOCATION);
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(30.0f, 30.0f), 4));
  DALI_TEST_EQUALS(1u, trace.Count("actor", CallbackKind::TOUCH, PointState::UP), TEST_LOCATION);

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  trace.Clear();
  application.ProcessEvent(GenerateTwoTouches(PointState::INTERRUPTED,
                                              Vector2(10.0f, 10.0f),
                                              4,
                                              PointState::MOTION,
                                              Vector2(20.0f, 20.0f),
                                              7));
  const TraceEntry* interruptedMixed = trace.Find("actor", CallbackKind::TOUCH, PointState::INTERRUPTED);
  DALI_TEST_CHECK(interruptedMixed);
  DALI_TEST_EQUALS(2u, interruptedMixed->states.size(), TEST_LOCATION);
  DALI_TEST_EQUALS(PointState::MOTION, interruptedMixed->states[1], TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamSceneSignalOncePerRawBoundaryEvent(void)
{
  TestApplication application;
  TouchTrace      trace;

  Actor left  = CreateTouchableActor("left");
  Actor right = CreateTouchableActor("right");
  right.SetProperty(Actor::Property::POSITION, Vector2(120.0f, 0.0f));
  application.GetScene().Add(left);
  application.GetScene().Add(right);

  TouchTraceFunctor leftFunctor(trace, "left", true);
  TouchTraceFunctor rightFunctor(trace, "right", true);
  SceneTraceFunctor sceneFunctor(trace, "scene");
  left.TouchEventSignal().Connect(&application, leftFunctor);
  right.TouchEventSignal().Connect(&application, rightFunctor);
  application.GetScene().TouchEventSignal().Connect(&application, sceneFunctor);
  PrepareScene(application);

  application.ProcessEvent(GenerateTwoTouches(PointState::DOWN,
                                              Vector2(10.0f, 10.0f),
                                              4,
                                              PointState::DOWN,
                                              Vector2(130.0f, 10.0f),
                                              7));
  DALI_TEST_EQUALS(1u, trace.Count("scene", CallbackKind::SCENE, PointState::DOWN), TEST_LOCATION);
  const TraceEntry* sceneDown = trace.Find("scene", CallbackKind::SCENE, PointState::DOWN);
  DALI_TEST_CHECK(sceneDown);
  DALI_TEST_EQUALS(2u, sceneDown->states.size(), TEST_LOCATION);
  DALI_TEST_CHECK(trace.FirstIndexOf("left", CallbackKind::TOUCH, PointState::DOWN) < trace.FirstIndexOf("right", CallbackKind::TOUCH, PointState::DOWN));
  DALI_TEST_CHECK(trace.FirstIndexOf("left", CallbackKind::TOUCH, PointState::DOWN) < trace.FirstIndexOf("scene", CallbackKind::SCENE, PointState::DOWN));
  DALI_TEST_CHECK(trace.FirstIndexOf("right", CallbackKind::TOUCH, PointState::DOWN) < trace.FirstIndexOf("scene", CallbackKind::SCENE, PointState::DOWN));

  trace.Clear();
  application.ProcessEvent(GenerateTwoTouches(PointState::UP,
                                              Vector2(10.0f, 10.0f),
                                              4,
                                              PointState::UP,
                                              Vector2(130.0f, 10.0f),
                                              7));
  DALI_TEST_EQUALS(1u, trace.Count("scene", CallbackKind::SCENE, PointState::UP), TEST_LOCATION);
  END_TEST;
}
