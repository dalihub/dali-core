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
  RenderTask                    renderTask;
};

struct TouchTrace
{
  void Record(const char* receiver, CallbackKind kind, TouchEvent touch)
  {
    TraceEntry entry;
    entry.receiver   = receiver;
    entry.kind       = kind;
    entry.renderTask = touch.GetRenderTask();
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

struct PanTrace
{
  uint32_t Count(GestureState state) const
  {
    return static_cast<uint32_t>(std::count(states.begin(), states.end(), state));
  }

  std::vector<GestureState> states;
};

struct PanTraceFunctor
{
  explicit PanTraceFunctor(PanTrace& trace)
  : trace(trace)
  {
  }

  void operator()(Actor, PanGesture pan)
  {
    trace.states.push_back(pan.GetState());
  }

  PanTrace& trace;
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

struct RemoveActorOnInterruptedFunctor
{
  RemoveActorOnInterruptedFunctor(TouchTrace& trace, const char* receiver, Actor actorToRemove)
  : trace(trace),
    receiver(receiver),
    actorToRemove(actorToRemove)
  {
  }

  bool operator()(Actor, TouchEvent touch)
  {
    trace.Record(receiver, CallbackKind::TOUCH, touch);
    if(touch.GetState(0u) == PointState::INTERRUPTED)
    {
      actorToRemove.Unparent();
    }
    return false;
  }

  TouchTrace& trace;
  const char* receiver;
  Actor       actorToRemove;
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

int UtcDaliGeoTouchStreamLateOwnerBindsBeforeTerminationCallbacks(void)
{
  TestApplication application;
  TouchTrace      trace;

  Actor root(application.GetScene().GetRootLayer());
  Actor parent = CreateTouchableActor("parent");
  Actor child  = CreateTouchableActor("child");
  parent.Add(child);
  application.GetScene().Add(parent);

  bool                            parentConsumes = false;
  TouchTraceFunctor               childObserver(trace, "child", false);
  MutableTouchTraceFunctor        parentObserver(trace, "parent", parentConsumes);
  RemoveActorOnInterruptedFunctor rootObserver(trace, "root", child);
  child.TouchEventSignal().Connect(&application, childObserver);
  parent.TouchEventSignal().Connect(&application, parentObserver);
  root.TouchEventSignal().Connect(&application, rootObserver);
  PrepareScene(application);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  trace.Clear();

  // The parent becomes owner on this motion. Terminating the root removes the initial hit actor
  // reentrantly, after the owner route should already have replaced its observers.
  parentConsumes = true;
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(20.0f, 20.0f)));
  DALI_TEST_EQUALS(1u, trace.Count("root", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, trace.Count("parent", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  trace.Clear();

  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(200.0f, 200.0f)));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(200.0f, 200.0f)));
  DALI_TEST_EQUALS(1u, trace.Count("parent", CallbackKind::TOUCH, PointState::MOTION), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("parent", CallbackKind::TOUCH, PointState::UP), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamPrimaryRemovalKeepsOwnerActive(void)
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

  // The child is the primary hit actor, but the parent owns the stream.
  child.Unparent();

  DALI_TEST_EQUALS(0u, trace.Count("child", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, trace.Count("parent", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);

  // No actor is hit here. The established owner still receives the remainder of the stream.
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(200.0f, 200.0f)));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(200.0f, 200.0f)));

  DALI_TEST_EQUALS(1u, trace.Count("parent", CallbackKind::TOUCH, PointState::MOTION), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("parent", CallbackKind::TOUCH, PointState::UP), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamOwnerRenderTaskRemainsStable(void)
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

  RenderTask ownerRenderTask = application.GetScene().GetRenderTaskList().GetTask(0u);
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  child.Unparent();

  // A second input-enabled task renders the owner in a displaced viewport. Without a stable owner
  // route, a hit in this viewport replaces the task used to deliver the stream.
  RenderTask alternateRenderTask = application.GetScene().GetRenderTaskList().CreateTask();
  alternateRenderTask.SetViewport(Viewport(200, 0, 100, 100));
  alternateRenderTask.SetInputEnabled(true);
  application.SendNotification();
  application.Render();
  trace.Clear();

  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(210.0f, 10.0f)));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(210.0f, 10.0f)));

  const TraceEntry* parentMotion = trace.Find("parent", CallbackKind::TOUCH, PointState::MOTION);
  const TraceEntry* parentUp     = trace.Find("parent", CallbackKind::TOUCH, PointState::UP);
  DALI_TEST_CHECK(parentMotion);
  DALI_TEST_CHECK(parentUp);
  DALI_TEST_CHECK(parentMotion->renderTask == ownerRenderTask);
  DALI_TEST_CHECK(parentMotion->renderTask != alternateRenderTask);
  DALI_TEST_EQUALS(Vector2(210.0f, 10.0f), parentMotion->localPositions[0], 0.1f, TEST_LOCATION);
  DALI_TEST_CHECK(parentUp->renderTask == ownerRenderTask);
  DALI_TEST_EQUALS(Vector2(210.0f, 10.0f), parentUp->localPositions[0], 0.1f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamRecycledPrimaryDoesNotReplaceOwnerRoute(void)
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
  trace.Clear();

  child.Unparent();
  child.SetProperty(Actor::Property::POSITION, Vector2(150.0f, 0.0f));
  parent.Add(child);
  application.SendNotification();
  application.Render();
  child.Unparent();

  DALI_TEST_EQUALS(0u, trace.Count("parent", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(200.0f, 200.0f)));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(200.0f, 200.0f)));
  DALI_TEST_EQUALS(1u, trace.Count("parent", CallbackKind::TOUCH, PointState::MOTION), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("parent", CallbackKind::TOUCH, PointState::UP), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamOwnerRemovalAfterPrimaryRemovalInterruptsOnce(void)
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
  trace.Clear();

  child.Unparent();
  parent.Unparent();
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(10.0f, 10.0f)));

  DALI_TEST_EQUALS(0u, trace.Count("child", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("parent", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, trace.Count("parent", CallbackKind::TOUCH, PointState::UP), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamInterruptionAfterPrimaryRemovalInterruptsOnce(void)
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
  trace.Clear();

  child.Unparent();
  application.ProcessEvent(GenerateSingleTouch(PointState::INTERRUPTED, Vector2(200.0f, 200.0f)));
  application.ProcessEvent(GenerateSingleTouch(PointState::INTERRUPTED, Vector2(200.0f, 200.0f)));

  DALI_TEST_EQUALS(0u, trace.Count("child", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("parent", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamOwnerRemovalInterruptsStream(void)
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
  trace.Clear();

  parent.Unparent();

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

int UtcDaliGeoTouchStreamInterceptIncludesCurrentOwner(void)
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
  DALI_TEST_EQUALS(1u, trace.Count("child-intercept", CallbackKind::INTERCEPT, PointState::MOTION), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.Count("child-touch", CallbackKind::TOUCH, PointState::MOTION), TEST_LOCATION);
  DALI_TEST_CHECK(trace.FirstIndexOf("child-intercept", CallbackKind::INTERCEPT, PointState::MOTION) <
                  trace.FirstIndexOf("child-touch", CallbackKind::TOUCH, PointState::MOTION));
  END_TEST;
}

int UtcDaliGeoTouchStreamOwnerRecognizesPanThroughIntercept(void)
{
  TestApplication application;
  TouchTrace      touchTrace;
  PanTrace        panTrace;

  Actor actor = CreateTouchableActor("scroll-owner");
  application.GetScene().Add(actor);

  PanGestureDetector detector = PanGestureDetector::New();
  PanTraceFunctor    panFunctor(panTrace);
  detector.DetectedSignal().Connect(&application, panFunctor);

  // Like PickerList/RecyclerView, consume DOWN in the touch callback, recognize the pan
  // through interception, then feed subsequent events through the touch callback.
  bool intercepted = false;
  actor.InterceptTouchEventSignal().Connect(&application, [&](Actor receiver, TouchEvent touch)
  {
    touchTrace.Record("owner-intercept", CallbackKind::INTERCEPT, touch);
    intercepted = detector.HandleEvent(receiver, touch);
    return intercepted;
  });
  actor.TouchEventSignal().Connect(&application, [&](Actor receiver, TouchEvent touch)
  {
    touchTrace.Record("owner-touch", CallbackKind::TOUCH, touch);
    if(intercepted)
    {
      detector.HandleEvent(receiver, touch);
      if(touch.GetState(0u) == PointState::UP || touch.GetState(0u) == PointState::INTERRUPTED)
      {
        intercepted = false;
      }
    }
    return true;
  });
  PrepareScene(application);

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 10.0f), Vector2(10.0f, 30.0f), time);
  DALI_TEST_EQUALS(1u, panTrace.Count(GestureState::STARTED), TEST_LOCATION);

  TestMovePan(application, Vector2(10.0f, 60.0f), time);
  time += TestGetFrameInterval();
  TestEndPan(application, Vector2(10.0f, 80.0f), time);

  DALI_TEST_CHECK(touchTrace.Count("owner-intercept", CallbackKind::INTERCEPT, PointState::MOTION) >= 1u);
  DALI_TEST_EQUALS(3u, touchTrace.Count("owner-touch", CallbackKind::TOUCH, PointState::MOTION), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, touchTrace.Count("owner-touch", CallbackKind::TOUCH, PointState::UP), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, touchTrace.Count("owner-touch", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  DALI_TEST_CHECK(panTrace.Count(GestureState::CONTINUING) >= 1u);
  DALI_TEST_EQUALS(1u, panTrace.Count(GestureState::FINISHED), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, panTrace.Count(GestureState::CANCELLED), TEST_LOCATION);
  DALI_TEST_CHECK(!intercepted);
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

int UtcDaliGeoTouchStreamPrimaryRemovalKeepsRemainingDeviceActive(void)
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

  application.ProcessEvent(GenerateDoubleTouch(PointState::DOWN, Vector2(10.0f, 10.0f)));
  trace.Clear();
  child.Unparent();

  application.ProcessEvent(GenerateTwoTouches(PointState::UP,
                                              Vector2(200.0f, 200.0f),
                                              4,
                                              PointState::STATIONARY,
                                              Vector2(210.0f, 210.0f),
                                              7));
  application.ProcessEvent(GenerateSingleTouch(PointState::MOTION, Vector2(220.0f, 220.0f), 7));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(220.0f, 220.0f), 7));

  DALI_TEST_EQUALS(0u, trace.Count("parent", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.CountDevice("parent", CallbackKind::TOUCH, PointState::UP, 4), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.CountDevice("parent", CallbackKind::TOUCH, PointState::MOTION, 7), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.CountDevice("parent", CallbackKind::TOUCH, PointState::UP, 7), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamPanFinishesAfterPrimaryRemoval(void)
{
  TestApplication application;
  TouchTrace      touchTrace;
  PanTrace        panTrace;

  Actor parent = CreateTouchableActor("parent");
  Actor child  = CreateTouchableActor("child");
  parent.Add(child);
  application.GetScene().Add(parent);

  TouchTraceFunctor childFunctor(touchTrace, "child", false);
  TouchTraceFunctor parentFunctor(touchTrace, "parent", true);
  child.TouchEventSignal().Connect(&application, childFunctor);
  parent.TouchEventSignal().Connect(&application, parentFunctor);
  PrepareScene(application);

  PanGestureDetector detector = PanGestureDetector::New();
  PanTraceFunctor    panFunctor(panTrace);
  detector.Attach(parent);
  detector.DetectedSignal().Connect(&application, panFunctor);

  uint32_t time = 100u;
  TestStartPan(application, Vector2(10.0f, 20.0f), Vector2(30.0f, 20.0f), time);
  DALI_TEST_EQUALS(1u, panTrace.Count(GestureState::STARTED), TEST_LOCATION);

  child.Unparent();
  TestMovePan(application, Vector2(200.0f, 20.0f), time);
  time += TestGetFrameInterval();
  TestEndPan(application, Vector2(210.0f, 20.0f), time);

  DALI_TEST_CHECK(panTrace.states.size() >= 3u);
  DALI_TEST_EQUALS(GestureState::STARTED, panTrace.states.front(), TEST_LOCATION);
  DALI_TEST_CHECK(panTrace.Count(GestureState::CONTINUING) >= 1u);
  DALI_TEST_EQUALS(1u, panTrace.Count(GestureState::FINISHED), TEST_LOCATION);
  DALI_TEST_EQUALS(GestureState::FINISHED, panTrace.states.back(), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, panTrace.Count(GestureState::CANCELLED), TEST_LOCATION);
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

int UtcDaliGeoTouchStreamLaterDownPreservesOwnerAndInitialHit(void)
{
  TestApplication application;
  TouchTrace      trace;

  Actor parent = CreateTouchableActor("parent");
  Actor child  = CreateTouchableActor("child");
  parent.Add(child);
  application.GetScene().Add(parent);

  TouchTraceFunctor childFunctor(trace, "child", false);
  TouchTraceFunctor parentFunctor(trace, "parent", true);
  SceneTraceFunctor sceneFunctor(trace, "scene");
  child.TouchEventSignal().Connect(&application, childFunctor);
  parent.TouchEventSignal().Connect(&application, parentFunctor);
  application.GetScene().TouchEventSignal().Connect(&application, sceneFunctor);
  PrepareScene(application);

  const int32_t    childId         = child.GetProperty<int32_t>(Actor::Property::ID);
  const RenderTask ownerRenderTask = application.GetScene().GetRenderTaskList().GetTask(0u);

  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(10.0f, 10.0f), 4));
  const TraceEntry* sceneInitialDown = trace.Find("scene", CallbackKind::SCENE, PointState::DOWN);
  DALI_TEST_CHECK(sceneInitialDown);
  DALI_TEST_EQUALS(childId, sceneInitialDown->hitActorIds[0], TEST_LOCATION);
  trace.Clear();

  // A later device joins the same routing group with a separate DOWN event.
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(20.0f, 20.0f), 7));

  const TraceEntry* parentDown = trace.Find("parent", CallbackKind::TOUCH, PointState::DOWN);
  DALI_TEST_CHECK(parentDown);
  DALI_TEST_EQUALS(1u, parentDown->hitActorIds.size(), TEST_LOCATION);
  DALI_TEST_EQUALS(childId, parentDown->hitActorIds[0], TEST_LOCATION);
  DALI_TEST_CHECK(parentDown->renderTask == ownerRenderTask);
  trace.Clear();

  child.Unparent();
  DALI_TEST_EQUALS(0u, trace.Count("parent", CallbackKind::TOUCH, PointState::INTERRUPTED), TEST_LOCATION);

  application.ProcessEvent(GenerateTwoTouches(PointState::MOTION,
                                              Vector2(200.0f, 200.0f),
                                              4,
                                              PointState::UP,
                                              Vector2(210.0f, 210.0f),
                                              7));
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(220.0f, 220.0f), 4));

  const TraceEntry* parentMotion = trace.Find("parent", CallbackKind::TOUCH, PointState::MOTION);
  DALI_TEST_CHECK(parentMotion);
  DALI_TEST_EQUALS(2u, parentMotion->hitActorIds.size(), TEST_LOCATION);
  DALI_TEST_EQUALS(childId, parentMotion->hitActorIds[0], TEST_LOCATION);
  DALI_TEST_EQUALS(childId, parentMotion->hitActorIds[1], TEST_LOCATION);
  DALI_TEST_CHECK(parentMotion->renderTask == ownerRenderTask);
  DALI_TEST_EQUALS(1u, trace.CountDevice("parent", CallbackKind::TOUCH, PointState::UP, 7), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, trace.CountDevice("parent", CallbackKind::TOUCH, PointState::UP, 4), TEST_LOCATION);
  const TraceEntry* parentUp = trace.Find("parent", CallbackKind::TOUCH, PointState::UP);
  const TraceEntry* sceneUp  = trace.Find("scene", CallbackKind::SCENE, PointState::UP);
  DALI_TEST_CHECK(parentUp);
  DALI_TEST_CHECK(sceneUp);
  DALI_TEST_EQUALS(childId, parentUp->hitActorIds[0], TEST_LOCATION);
  DALI_TEST_EQUALS(childId, sceneUp->hitActorIds[0], TEST_LOCATION);
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

namespace
{
struct GestureStateTrace
{
  uint32_t Count(GestureState state) const
  {
    return static_cast<uint32_t>(std::count(states.begin(), states.end(), state));
  }

  std::vector<GestureState> states;
};

struct PinchTraceFunctor
{
  explicit PinchTraceFunctor(GestureStateTrace& trace)
  : trace(trace)
  {
  }

  void operator()(Actor, PinchGesture pinch)
  {
    trace.states.push_back(pinch.GetState());
  }

  GestureStateTrace& trace;
};

struct RotationTraceFunctor
{
  explicit RotationTraceFunctor(GestureStateTrace& trace)
  : trace(trace)
  {
  }

  void operator()(Actor, RotationGesture rotation)
  {
    trace.states.push_back(rotation.GetState());
  }

  GestureStateTrace& trace;
};

/**
 * A 200x100 parent with two 100x100 touchable children side by side: "left" covers x 0..100 and
 * "right" covers x 100..200. Every actor records its touch events without consuming them.
 * Geometry hit-test is enabled on construction so that a gesture detector attached to the parent
 * afterwards connects to the parent's touch signal.
 */
struct SideBySideChildrenFixture
{
  SideBySideChildrenFixture(TestApplication& application, TouchTrace& touchTrace)
  : parentFunctor(touchTrace, "parent", false),
    leftFunctor(touchTrace, "left", false),
    rightFunctor(touchTrace, "right", false)
  {
    application.GetScene().SetGeometryHittestEnabled(true);

    parent = CreateTouchableActor("parent");
    parent.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 100.0f));
    left  = CreateTouchableActor("left");
    right = CreateTouchableActor("right");
    right.SetProperty(Actor::Property::POSITION, Vector2(100.0f, 0.0f));
    parent.Add(left);
    parent.Add(right);
    application.GetScene().Add(parent);

    parent.TouchEventSignal().Connect(&application, parentFunctor);
    left.TouchEventSignal().Connect(&application, leftFunctor);
    right.TouchEventSignal().Connect(&application, rightFunctor);
  }

  Actor             parent;
  Actor             left;
  Actor             right;
  TouchTraceFunctor parentFunctor;
  TouchTraceFunctor leftFunctor;
  TouchTraceFunctor rightFunctor;
};

uint32_t PointCountOfFirst(const TouchTrace& trace, const char* receiver, PointState::Type state)
{
  const TraceEntry* entry = trace.Find(receiver, CallbackKind::TOUCH, state);
  return entry ? static_cast<uint32_t>(entry->states.size()) : 0u;
}
} // namespace

int UtcDaliGeoTouchStreamPinchOwnerGroupsPointsOnDifferentChildren(void)
{
  TestApplication           application;
  TouchTrace                touchTrace;
  GestureStateTrace         pinchTrace;
  SideBySideChildrenFixture fixture(application, touchTrace);

  PinchGestureDetector detector = PinchGestureDetector::New();
  PinchTraceFunctor    pinchFunctor(pinchTrace);
  detector.Attach(fixture.parent);
  detector.DetectedSignal().Connect(&application, pinchFunctor);
  PrepareScene(application);

  // One point on "left" and one on "right". The parent requires PINCH, so both points share one stream.
  TestStartPinch(application, Vector2(80.0f, 50.0f), Vector2(120.0f, 50.0f), Vector2(60.0f, 50.0f), Vector2(140.0f, 50.0f), 100u);
  TestEndPinch(application, Vector2(60.0f, 50.0f), Vector2(140.0f, 50.0f), Vector2(50.0f, 50.0f), Vector2(150.0f, 50.0f), 400u);

  DALI_TEST_EQUALS(1u, pinchTrace.Count(GestureState::STARTED), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, pinchTrace.Count(GestureState::FINISHED), TEST_LOCATION);
  DALI_TEST_EQUALS(2u, PointCountOfFirst(touchTrace, "parent", PointState::DOWN), TEST_LOCATION);
  DALI_TEST_EQUALS(2u, PointCountOfFirst(touchTrace, "parent", PointState::MOTION), TEST_LOCATION);

  // The shared stream is delivered along the hit candidates of its first point only.
  DALI_TEST_EQUALS(1u, touchTrace.Count("left", CallbackKind::TOUCH, PointState::DOWN), TEST_LOCATION);
  DALI_TEST_EQUALS(2u, PointCountOfFirst(touchTrace, "left", PointState::DOWN), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, touchTrace.Count("right", CallbackKind::TOUCH, PointState::DOWN), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, touchTrace.Count("right", CallbackKind::TOUCH, PointState::MOTION), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamPinchOwnerKeepsPointsOnOneChildGrouped(void)
{
  TestApplication           application;
  TouchTrace                touchTrace;
  GestureStateTrace         pinchTrace;
  SideBySideChildrenFixture fixture(application, touchTrace);

  PinchGestureDetector detector = PinchGestureDetector::New();
  PinchTraceFunctor    pinchFunctor(pinchTrace);
  detector.Attach(fixture.parent);
  detector.DetectedSignal().Connect(&application, pinchFunctor);
  PrepareScene(application);

  // Both points on "left" already shared a stream before owner grouping; this must keep working.
  TestStartPinch(application, Vector2(20.0f, 50.0f), Vector2(80.0f, 50.0f), Vector2(10.0f, 50.0f), Vector2(90.0f, 50.0f), 100u);
  TestEndPinch(application, Vector2(10.0f, 50.0f), Vector2(90.0f, 50.0f), Vector2(5.0f, 50.0f), Vector2(95.0f, 50.0f), 400u);

  DALI_TEST_EQUALS(1u, pinchTrace.Count(GestureState::STARTED), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, pinchTrace.Count(GestureState::FINISHED), TEST_LOCATION);
  DALI_TEST_EQUALS(2u, PointCountOfFirst(touchTrace, "left", PointState::DOWN), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, touchTrace.Count("right", CallbackKind::TOUCH, PointState::DOWN), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamRotationOwnerGroupsPointsOnDifferentChildren(void)
{
  TestApplication           application;
  TouchTrace                touchTrace;
  GestureStateTrace         rotationTrace;
  SideBySideChildrenFixture fixture(application, touchTrace);

  RotationGestureDetector detector = RotationGestureDetector::New();
  RotationTraceFunctor    rotationFunctor(rotationTrace);
  detector.Attach(fixture.parent);
  detector.DetectedSignal().Connect(&application, rotationFunctor);
  PrepareScene(application);

  // One point on "left" and one on "right". The parent requires ROTATION, so both points share one stream.
  TestStartRotation(application, Vector2(80.0f, 50.0f), Vector2(120.0f, 50.0f), Vector2(80.0f, 30.0f), Vector2(120.0f, 70.0f), 100u);
  TestEndRotation(application, Vector2(80.0f, 30.0f), Vector2(120.0f, 70.0f), Vector2(90.0f, 20.0f), Vector2(110.0f, 80.0f), 400u);

  DALI_TEST_EQUALS(1u, rotationTrace.Count(GestureState::STARTED), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, rotationTrace.Count(GestureState::FINISHED), TEST_LOCATION);
  DALI_TEST_EQUALS(2u, PointCountOfFirst(touchTrace, "parent", PointState::MOTION), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, touchTrace.Count("right", CallbackKind::TOUCH, PointState::DOWN), TEST_LOCATION);
  END_TEST;
}

int UtcDaliGeoTouchStreamSingleTouchGestureOwnerKeepsIndependentGroups(void)
{
  TestApplication           application;
  TouchTrace                touchTrace;
  SideBySideChildrenFixture fixture(application, touchTrace);

  // A pan detector requires a single-touch gesture only, so it must not merge the two groups.
  PanGestureDetector detector = PanGestureDetector::New();
  detector.Attach(fixture.parent);
  PrepareScene(application);

  application.ProcessEvent(GenerateTwoTouches(PointState::DOWN, Vector2(80.0f, 50.0f), 4, PointState::DOWN, Vector2(120.0f, 50.0f), 7));
  application.ProcessEvent(GenerateTwoTouches(PointState::MOTION, Vector2(60.0f, 50.0f), 4, PointState::MOTION, Vector2(140.0f, 50.0f), 7));

  DALI_TEST_EQUALS(1u, PointCountOfFirst(touchTrace, "parent", PointState::MOTION), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, touchTrace.CountDevice("parent", CallbackKind::TOUCH, PointState::MOTION, 4), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, touchTrace.CountDevice("parent", CallbackKind::TOUCH, PointState::MOTION, 7), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, touchTrace.Count("left", CallbackKind::TOUCH, PointState::DOWN), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, touchTrace.Count("right", CallbackKind::TOUCH, PointState::DOWN), TEST_LOCATION);

  application.ProcessEvent(GenerateTwoTouches(PointState::UP, Vector2(60.0f, 50.0f), 4, PointState::UP, Vector2(140.0f, 50.0f), 7));
  END_TEST;
}

int UtcDaliGeoTouchStreamPinchOwnerGroupsPointsAcrossMappingActorEdge(void)
{
  TestApplication          application;
  TouchTrace               touchTrace;
  GestureStateTrace        pinchTrace;
  Dali::Integration::Scene scene = application.GetScene();
  scene.SetGeometryHittestEnabled(true);

  // A 200x100 container owning a pinch detector. Its left half is the mapping actor of an offscreen
  // render task, so a hit there resolves to an actor inside the framebuffer; its right half is a plain child.
  Actor container = CreateTouchableActor("container");
  container.SetProperty(Actor::Property::SIZE, Vector2(200.0f, 100.0f));
  Actor mappingActor = CreateTouchableActor("mapping");
  Actor right        = CreateTouchableActor("right");
  right.SetProperty(Actor::Property::POSITION, Vector2(100.0f, 0.0f));
  container.Add(mappingActor);
  container.Add(right);
  scene.Add(container);

  // The offscreen tree is centred on the scene, where a 100x100 camera looks by default.
  Actor offscreenRoot = Actor::New();
  offscreenRoot.SetProperty(Actor::Property::NAME, "offscreen-root");
  offscreenRoot.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  offscreenRoot.SetProperty(Actor::Property::PIVOT, Pivot::CENTER);
  offscreenRoot.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  Actor offscreenChild = CreateTouchableActor("offscreen-child");
  offscreenRoot.Add(offscreenChild);
  scene.Add(offscreenRoot);

  CameraActor offscreenCamera = CameraActor::New(Vector2(100.0f, 100.0f));
  offscreenCamera.SetProperty(Actor::Property::PIVOT, Pivot::CENTER);
  offscreenCamera.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  scene.Add(offscreenCamera);

  FrameBuffer frameBuffer   = FrameBuffer::New(100u, 100u);
  RenderTask  offscreenTask = scene.GetRenderTaskList().CreateTask();
  offscreenTask.SetExclusive(true);
  offscreenTask.SetInputEnabled(true);
  offscreenTask.SetCameraActor(offscreenCamera);
  offscreenTask.SetSourceActor(offscreenRoot);
  offscreenTask.SetFrameBuffer(frameBuffer);
  offscreenTask.SetScreenToFrameBufferMappingActor(mappingActor);

  TouchTraceFunctor containerFunctor(touchTrace, "container", false);
  TouchTraceFunctor mappingFunctor(touchTrace, "mapping", false);
  TouchTraceFunctor rightFunctor(touchTrace, "right", false);
  TouchTraceFunctor offscreenChildFunctor(touchTrace, "offscreen-child", false);
  container.TouchEventSignal().Connect(&application, containerFunctor);
  mappingActor.TouchEventSignal().Connect(&application, mappingFunctor);
  right.TouchEventSignal().Connect(&application, rightFunctor);
  offscreenChild.TouchEventSignal().Connect(&application, offscreenChildFunctor);

  PinchGestureDetector detector = PinchGestureDetector::New();
  PinchTraceFunctor    pinchFunctor(pinchTrace);
  detector.Attach(container);
  detector.DetectedSignal().Connect(&application, pinchFunctor);
  PrepareScene(application);
  application.SendNotification();
  application.Render();

  // The first point hits the offscreen child through the mapping actor, the second hits "right".
  application.ProcessEvent(GenerateSingleTouch(PointState::DOWN, Vector2(50.0f, 50.0f), 4));
  DALI_TEST_EQUALS(1u, touchTrace.Count("offscreen-child", CallbackKind::TOUCH, PointState::DOWN), TEST_LOCATION);
  application.ProcessEvent(GenerateSingleTouch(PointState::UP, Vector2(50.0f, 50.0f), 4));
  touchTrace.Clear();

  TestStartPinch(application, Vector2(80.0f, 50.0f), Vector2(120.0f, 50.0f), Vector2(60.0f, 50.0f), Vector2(140.0f, 50.0f), 100u);
  TestEndPinch(application, Vector2(60.0f, 50.0f), Vector2(140.0f, 50.0f), Vector2(50.0f, 50.0f), Vector2(150.0f, 50.0f), 400u);

  DALI_TEST_EQUALS(1u, pinchTrace.Count(GestureState::STARTED), TEST_LOCATION);
  DALI_TEST_EQUALS(1u, pinchTrace.Count(GestureState::FINISHED), TEST_LOCATION);
  DALI_TEST_EQUALS(2u, PointCountOfFirst(touchTrace, "container", PointState::MOTION), TEST_LOCATION);
  DALI_TEST_EQUALS(0u, touchTrace.Count("right", CallbackKind::TOUCH, PointState::DOWN), TEST_LOCATION);
  END_TEST;
}
