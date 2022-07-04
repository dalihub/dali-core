/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/common/stage.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/wheel-event-integ.h>
#include <dali/integration-api/scene.h>
#include <dali/public-api/events/key-event.h>
#include <mesh-builder.h>
#include <stdlib.h>

#include <iostream>

// Internal headers are allowed here

namespace
{
const std::string DEFAULT_DEVICE_NAME("hwKeyboard");

// Functor for EventProcessingFinished signal
struct EventProcessingFinishedFunctor
{
  /**
   * @param[in] eventProcessingFinished reference to a boolean variable used to check if signal has been called.
   */
  EventProcessingFinishedFunctor(bool& eventProcessingFinished)
  : mEventProcessingFinished(eventProcessingFinished)
  {
  }

  void operator()()
  {
    mEventProcessingFinished = true;
  }

  bool& mEventProcessingFinished;
};

// Stores data that is populated in the key-event callback and will be read by the TET cases
struct KeyEventSignalData
{
  KeyEventSignalData()
  : functorCalled(false)
  {
  }

  void Reset()
  {
    functorCalled = false;

    receivedKeyEvent.Reset();
  }

  bool     functorCalled;
  KeyEvent receivedKeyEvent;
};

// Functor that sets the data when called
struct KeyEventReceivedFunctor
{
  KeyEventReceivedFunctor(KeyEventSignalData& data)
  : signalData(data)
  {
  }

  bool operator()(const KeyEvent& keyEvent)
  {
    signalData.functorCalled    = true;
    signalData.receivedKeyEvent = keyEvent;

    return true;
  }

  KeyEventSignalData& signalData;
};

// Stores data that is populated in the touched signal callback and will be read by the TET cases
struct TouchedSignalData
{
  TouchedSignalData()
  : functorCalled(false),
    createNewScene(false),
    newSceneCreated(false)
  {
  }

  void Reset()
  {
    functorCalled   = false;
    createNewScene  = false;
    newSceneCreated = false;
    receivedTouchEvent.Reset();
  }

  bool       functorCalled;
  bool       createNewScene;
  bool       newSceneCreated;
  TouchEvent receivedTouchEvent;
};

// Functor that sets the data when touched signal is received
struct TouchFunctor
{
  TouchFunctor(TouchedSignalData& data)
  : signalData(data)
  {
  }

  void operator()(const TouchEvent& touch)
  {
    signalData.functorCalled      = true;
    signalData.receivedTouchEvent = touch;

    if(signalData.createNewScene)
    {
      Dali::Integration::Scene scene = Dali::Integration::Scene::New(Size(480.0f, 800.0f));
      DALI_TEST_CHECK(scene);

      signalData.newSceneCreated = true;
    }
  }

  void operator()()
  {
    signalData.functorCalled = true;
  }

  TouchedSignalData& signalData;
};

// Stores data that is populated in the wheel-event callback and will be read by the TET cases
struct WheelEventSignalData
{
  WheelEventSignalData()
  : functorCalled(false)
  {
  }

  void Reset()
  {
    functorCalled = false;
  }

  bool       functorCalled;
  WheelEvent receivedWheelEvent;
};

// Functor that sets the data when wheel-event signal is received
struct WheelEventReceivedFunctor
{
  WheelEventReceivedFunctor(WheelEventSignalData& data)
  : signalData(data)
  {
  }

  bool operator()(const WheelEvent& wheelEvent)
  {
    signalData.functorCalled      = true;
    signalData.receivedWheelEvent = wheelEvent;

    return true;
  }

  WheelEventSignalData& signalData;
};

// Stores data that is populated in the KeyEventGeneratedSignal callback and will be read by the TET cases
struct KeyEventGeneratedSignalData
{
  KeyEventGeneratedSignalData()
  : functorCalled(false)
  {
  }

  void Reset()
  {
    functorCalled = false;

    receivedKeyEvent.Reset();
  }

  bool     functorCalled;
  KeyEvent receivedKeyEvent;
};

// Functor that sets the data when called
struct KeyEventGeneratedReceivedFunctor
{
  KeyEventGeneratedReceivedFunctor(KeyEventGeneratedSignalData& data)
  : signalData(data)
  {
  }

  bool operator()(const KeyEvent& keyEvent)
  {
    signalData.functorCalled    = true;
    signalData.receivedKeyEvent = keyEvent;

    return true;
  }

  bool operator()()
  {
    signalData.functorCalled = true;
    return true;
  }

  KeyEventGeneratedSignalData& signalData;
};

// Stores data that is populated in the WheelEventGeneratedSignal callback and will be read by the TET cases
struct WheelEventGeneratedSignalData
{
  WheelEventGeneratedSignalData()
  : functorCalled(false)
  {
  }

  void Reset()
  {
    functorCalled = false;

    receivedWheelEvent.Reset();
  }

  bool       functorCalled;
  WheelEvent receivedWheelEvent;
};

// Functor that sets the data when called
struct WheelEventGeneratedReceivedFunctor
{
  WheelEventGeneratedReceivedFunctor(WheelEventGeneratedSignalData& data)
  : signalData(data)
  {
  }

  bool operator()(const WheelEvent& wheelEvent)
  {
    signalData.functorCalled      = true;
    signalData.receivedWheelEvent = wheelEvent;

    return true;
  }

  bool operator()()
  {
    signalData.functorCalled = true;
    return true;
  }

  WheelEventGeneratedSignalData& signalData;
};

void GenerateTouch(TestApplication& application, PointState::Type state, const Vector2& screenPosition)
{
  Integration::TouchEvent touchEvent;
  Integration::Point      point;
  point.SetState(state);
  point.SetScreenPosition(screenPosition);
  touchEvent.points.push_back(point);
  application.ProcessEvent(touchEvent);
}

bool DummyTouchCallback(Actor actor, const TouchEvent& touch)
{
  return true;
}

void FrameCallback(int frameId)
{
}

} // unnamed namespace

int UtcDaliSceneAdd(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Scene::Add");

  Dali::Integration::Scene scene = application.GetScene();

  Actor actor = Actor::New();
  DALI_TEST_CHECK(!actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));

  scene.Add(actor);
  DALI_TEST_CHECK(actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));

  END_TEST;
}

int UtcDaliSceneRemove(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Scene::Remove");

  Dali::Integration::Scene scene = application.GetScene();

  Actor actor = Actor::New();
  DALI_TEST_CHECK(!actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));

  scene.Add(actor);
  DALI_TEST_CHECK(actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));

  scene.Remove(actor);
  DALI_TEST_CHECK(!actor.GetProperty<bool>(Actor::Property::CONNECTED_TO_SCENE));

  END_TEST;
}

int UtcDaliSceneGetSize(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Scene::GetSize");

  Dali::Integration::Scene scene = application.GetScene();
  Size                     size  = scene.GetSize();
  DALI_TEST_EQUALS(TestApplication::DEFAULT_SURFACE_WIDTH, size.width, TEST_LOCATION);
  DALI_TEST_EQUALS(TestApplication::DEFAULT_SURFACE_HEIGHT, size.height, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneGetDpi(void)
{
  TestApplication application; // Initializes core DPI to default values

  // Test that setting core DPI explicitly also sets up the scene's DPI.
  Dali::Integration::Scene scene = application.GetScene();
  scene.SetDpi(Vector2(200.0f, 180.0f));
  Vector2 dpi = scene.GetDpi();
  DALI_TEST_EQUALS(dpi.x, 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(dpi.y, 180.0f, TEST_LOCATION);
  END_TEST;
}

int UtcDaliSceneGetRenderTaskList(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Scene::GetRenderTaskList");

  Dali::Integration::Scene scene = application.GetScene();

  // Check we get a valid instance.
  const RenderTaskList& tasks = scene.GetRenderTaskList();

  // There should be 1 task by default.
  DALI_TEST_EQUALS(tasks.GetTaskCount(), 1u, TEST_LOCATION);

  // RenderTaskList has it's own UTC tests.
  // But we can confirm that GetRenderTaskList in Stage retrieves the same RenderTaskList each time.
  RenderTask newTask = scene.GetRenderTaskList().CreateTask();

  DALI_TEST_EQUALS(scene.GetRenderTaskList().GetTask(1), newTask, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneGetRootLayer(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Scene::GetRootLayer");

  Dali::Integration::Scene scene = application.GetScene();
  Layer                    layer = scene.GetLayer(0);
  DALI_TEST_CHECK(layer);

  // Check that GetRootLayer() correctly retreived layer 0.
  DALI_TEST_CHECK(scene.GetRootLayer() == layer);

  END_TEST;
}

int UtcDaliSceneGetLayerCount(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Scene::GetLayerCount");

  Dali::Integration::Scene scene = application.GetScene();
  // Initially we have a default layer
  DALI_TEST_EQUALS(scene.GetLayerCount(), 1u, TEST_LOCATION);

  Layer layer = Layer::New();
  scene.Add(layer);

  DALI_TEST_EQUALS(scene.GetLayerCount(), 2u, TEST_LOCATION);
  END_TEST;
}

int UtcDaliSceneGetLayer(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Scene::GetLayer");

  Dali::Integration::Scene scene = application.GetScene();

  Layer rootLayer = scene.GetLayer(0);
  DALI_TEST_CHECK(rootLayer);

  Layer layer = Layer::New();
  scene.Add(layer);

  Layer sameLayer = scene.GetLayer(1);
  DALI_TEST_CHECK(layer == sameLayer);

  END_TEST;
}

int UtcDaliSceneGet(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Scene::Get");

  Dali::Integration::Scene scene = application.GetScene();

  Actor parent = Actor::New();
  Actor child  = Actor::New();

  parent.Add(child);

  // Should be empty scene
  DALI_TEST_CHECK(Dali::Integration::Scene() == Dali::Integration::Scene::Get(parent));
  DALI_TEST_CHECK(Dali::Integration::Scene() == Dali::Integration::Scene::Get(child));

  scene.Add(parent);

  // Should return the valid scene
  DALI_TEST_CHECK(scene == Dali::Integration::Scene::Get(parent));
  DALI_TEST_CHECK(scene == Dali::Integration::Scene::Get(child));

  parent.Unparent();

  // Should be empty scene
  DALI_TEST_CHECK(Dali::Integration::Scene() == Dali::Integration::Scene::Get(parent));
  DALI_TEST_CHECK(Dali::Integration::Scene() == Dali::Integration::Scene::Get(child));

  END_TEST;
}

int UtcDaliSceneDiscard(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Scene::Discard");

  // Create a new Scene
  Dali::Integration::Scene scene = Dali::Integration::Scene::New(Size(480.0f, 800.0f));
  DALI_TEST_CHECK(scene);

  // One reference of scene kept here and the other one kept in the Core
  DALI_TEST_CHECK(scene.GetBaseObject().ReferenceCount() == 2);

  // Render and notify.
  application.SendNotification();
  application.Render(0);

  // Keep the reference of the root layer handle so it will still be alive after the scene is deleted
  Layer rootLayer = scene.GetRootLayer();
  DALI_TEST_CHECK(rootLayer);
  DALI_TEST_CHECK(rootLayer.GetBaseObject().ReferenceCount() == 2);

  // Request to discard the scene from the Core
  scene.Discard();
  DALI_TEST_CHECK(scene.GetBaseObject().ReferenceCount() == 1);

  // Reset the scene handle
  scene.Reset();

  // Render and notify.
  application.SendNotification();
  application.Render(0);

  // At this point, the scene should have been automatically deleted
  // To prove this, the ref count of the root layer handle should be decremented to 1
  DALI_TEST_CHECK(rootLayer.GetBaseObject().ReferenceCount() == 1);

  // Delete the root layer handle
  rootLayer.Reset();

  // Render and notify.
  application.SendNotification();
  application.Render(0);

  END_TEST;
}

int UtcDaliSceneCreateNewSceneDuringCoreEventProcessing(void)
{
  TestApplication application;

  Dali::Integration::Scene scene = application.GetScene();

  TouchedSignalData data;
  data.createNewScene = true;
  TouchFunctor functor(data);
  scene.TouchedSignal().Connect(&application, functor);

  // Render and notify.
  application.SendNotification();
  application.Render();

  GenerateTouch(application, PointState::DOWN, Vector2(10.0f, 10.0f));

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_EQUALS(true, data.createNewScene, TEST_LOCATION);
  DALI_TEST_EQUALS(true, data.newSceneCreated, TEST_LOCATION);
  data.Reset();

  END_TEST;
}

int UtcDaliSceneRootLayerAndSceneAlignment(void)
{
  TestApplication application;

  // Create a Scene
  Dali::Integration::Scene scene = Dali::Integration::Scene::New(Size(480.0f, 800.0f));
  DALI_TEST_CHECK(scene);

  // One reference of scene kept here and the other one kept in the Core
  DALI_TEST_CHECK(scene.GetBaseObject().ReferenceCount() == 2);

  // Add a renderable actor to the scene
  auto actor = CreateRenderableActor();
  scene.Add(actor);

  // Render and notify.
  application.SendNotification();
  application.Render(0);

  // Keep the reference of the root layer handle so it will still be alive after the scene is deleted
  Layer rootLayer = scene.GetRootLayer();
  DALI_TEST_CHECK(rootLayer);
  DALI_TEST_CHECK(rootLayer.GetBaseObject().ReferenceCount() == 2);

  // Request to discard the scene from the Core
  scene.Discard();
  DALI_TEST_CHECK(scene.GetBaseObject().ReferenceCount() == 1);

  // Reset the scene handle
  scene.Reset();

  // Render and notify.
  application.SendNotification();
  application.Render(0);

  // At this point, the scene should have been automatically deleted
  // To prove this, the ref count of the root layer handle should be decremented to 1
  DALI_TEST_CHECK(rootLayer.GetBaseObject().ReferenceCount() == 1);

  // Create a new Scene while the root layer of the deleted scene is still alive
  Dali::Integration::Scene newScene = Dali::Integration::Scene::New(Size(480.0f, 800.0f));
  DALI_TEST_CHECK(newScene);

  // Render and notify.
  application.SendNotification();
  application.Render(0);

  // At this point, we have only one scene but two root layers
  // The root layer of the deleted scene is still alive
  DALI_TEST_CHECK(rootLayer.GetBaseObject().ReferenceCount() == 1);

  // Delete the root layer of the deleted scene
  rootLayer.Reset();

  // Render and notify.
  application.SendNotification();
  application.Render(0);

  END_TEST;
}

int UtcDaliSceneEventProcessingFinishedP(void)
{
  TestApplication          application;
  Dali::Integration::Scene scene = application.GetScene();

  bool                           eventProcessingFinished = false;
  EventProcessingFinishedFunctor functor(eventProcessingFinished);
  scene.EventProcessingFinishedSignal().Connect(&application, functor);

  Actor actor(Actor::New());
  scene.Add(actor);

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK(eventProcessingFinished);

  END_TEST;
}

int UtcDaliSceneEventProcessingFinishedN(void)
{
  TestApplication          application;
  Dali::Integration::Scene scene = application.GetScene();

  bool                           eventProcessingFinished = false;
  EventProcessingFinishedFunctor functor(eventProcessingFinished);
  scene.EventProcessingFinishedSignal().Connect(&application, functor);

  Actor actor(Actor::New());
  scene.Add(actor);

  // Do not complete event processing and confirm the signal has not been emitted.
  DALI_TEST_CHECK(!eventProcessingFinished);

  END_TEST;
}

int UtcDaliSceneSignalKeyEventP(void)
{
  TestApplication          application;
  Dali::Integration::Scene scene = application.GetScene();

  KeyEventSignalData      data;
  KeyEventReceivedFunctor functor(data);
  scene.KeyEventSignal().Connect(&application, functor);

  Integration::KeyEvent event("i", "", "i", 0, 0, 0, Integration::KeyEvent::DOWN, "i", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(event);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(event.keyModifier == data.receivedKeyEvent.GetKeyModifier());
  DALI_TEST_CHECK(event.keyName == data.receivedKeyEvent.GetKeyName());
  DALI_TEST_CHECK(event.keyString == data.receivedKeyEvent.GetKeyString());
  DALI_TEST_CHECK(event.state == static_cast<Integration::KeyEvent::State>(data.receivedKeyEvent.GetState()));

  data.Reset();

  Integration::KeyEvent event2("i", "", "i", 0, 0, 0, Integration::KeyEvent::UP, "i", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(event2);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(event2.keyModifier == data.receivedKeyEvent.GetKeyModifier());
  DALI_TEST_CHECK(event2.keyName == data.receivedKeyEvent.GetKeyName());
  DALI_TEST_CHECK(event2.keyString == data.receivedKeyEvent.GetKeyString());
  DALI_TEST_CHECK(event2.state == static_cast<Integration::KeyEvent::State>(data.receivedKeyEvent.GetState()));

  data.Reset();

  Integration::KeyEvent event3("a", "", "a", 0, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(event3);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(event3.keyModifier == data.receivedKeyEvent.GetKeyModifier());
  DALI_TEST_CHECK(event3.keyName == data.receivedKeyEvent.GetKeyName());
  DALI_TEST_CHECK(event3.keyString == data.receivedKeyEvent.GetKeyString());
  DALI_TEST_CHECK(event3.state == static_cast<Integration::KeyEvent::State>(data.receivedKeyEvent.GetState()));

  data.Reset();

  Integration::KeyEvent event4("a", "", "a", 0, 0, 0, Integration::KeyEvent::UP, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(event4);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(event4.keyModifier == data.receivedKeyEvent.GetKeyModifier());
  DALI_TEST_CHECK(event4.keyName == data.receivedKeyEvent.GetKeyName());
  DALI_TEST_CHECK(event4.keyString == data.receivedKeyEvent.GetKeyString());
  DALI_TEST_CHECK(event4.state == static_cast<Integration::KeyEvent::State>(data.receivedKeyEvent.GetState()));
  END_TEST;
}

int UtcDaliSceneSignalKeyEventN(void)
{
  TestApplication          application;
  Dali::Integration::Scene scene = application.GetScene();

  KeyEventSignalData      data;
  KeyEventReceivedFunctor functor(data);
  scene.KeyEventSignal().Connect(&application, functor);

  // Check that a non-pressed key events data is not modified.
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneTouchedSignalP(void)
{
  TestApplication          application;
  Dali::Integration::Scene scene = application.GetScene();

  TouchedSignalData data;
  TouchFunctor      functor(data);
  scene.TouchedSignal().Connect(&application, functor);

  // Render and notify.
  application.SendNotification();
  application.Render();

  // Basic test: No actors, single touch (down then up).
  {
    GenerateTouch(application, PointState::DOWN, Vector2(10.0f, 10.0f));

    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_CHECK(data.receivedTouchEvent.GetPointCount() != 0u);
    DALI_TEST_CHECK(!data.receivedTouchEvent.GetHitActor(0));
    data.Reset();

    GenerateTouch(application, PointState::UP, Vector2(10.0f, 10.0f));

    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_CHECK(data.receivedTouchEvent.GetPointCount() != 0u);
    DALI_TEST_CHECK(!data.receivedTouchEvent.GetHitActor(0));
    data.Reset();
  }

  // Add an actor to the scene.
  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  actor.TouchedSignal().Connect(&DummyTouchCallback);
  scene.Add(actor);

  // Render and notify.
  application.SendNotification();
  application.Render();

  // Actor on scene, single touch, down in actor, motion, then up outside actor.
  {
    GenerateTouch(application, PointState::DOWN, Vector2(10.0f, 10.0f));

    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_CHECK(data.receivedTouchEvent.GetPointCount() != 0u);
    DALI_TEST_CHECK(data.receivedTouchEvent.GetHitActor(0) == actor);
    data.Reset();

    GenerateTouch(application, PointState::MOTION, Vector2(150.0f, 10.0f)); // Some motion

    DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
    data.Reset();

    GenerateTouch(application, PointState::UP, Vector2(150.0f, 10.0f)); // Some motion

    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_CHECK(data.receivedTouchEvent.GetPointCount() != 0u);
    DALI_TEST_CHECK(!data.receivedTouchEvent.GetHitActor(0));
    data.Reset();
  }

  // Multiple touch. Should only receive a touch on first down and last up.
  {
    Integration::TouchEvent touchEvent;
    Integration::Point      point;

    // 1st point
    point.SetState(PointState::DOWN);
    point.SetScreenPosition(Vector2(10.0f, 10.0f));
    touchEvent.points.push_back(point);
    application.ProcessEvent(touchEvent);
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_EQUALS(data.receivedTouchEvent.GetPointCount(), 1u, TEST_LOCATION);
    data.Reset();

    // 2nd point
    touchEvent.points[0].SetState(PointState::STATIONARY);
    point.SetDeviceId(1);
    point.SetScreenPosition(Vector2(50.0f, 50.0f));
    touchEvent.points.push_back(point);
    application.ProcessEvent(touchEvent);
    DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
    data.Reset();

    // Primary point is up
    touchEvent.points[0].SetState(PointState::UP);
    touchEvent.points[1].SetState(PointState::STATIONARY);
    application.ProcessEvent(touchEvent);
    DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
    data.Reset();

    // Remove 1st point now, 2nd point is now in motion
    touchEvent.points.erase(touchEvent.points.begin());
    touchEvent.points[0].SetState(PointState::MOTION);
    touchEvent.points[0].SetScreenPosition(Vector2(150.0f, 50.0f));
    application.ProcessEvent(touchEvent);
    DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
    data.Reset();

    // Final point Up
    touchEvent.points[0].SetState(PointState::UP);
    application.ProcessEvent(touchEvent);
    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_EQUALS(data.receivedTouchEvent.GetPointCount(), 1u, TEST_LOCATION);
    data.Reset();
  }
  END_TEST;
}

int UtcDaliSceneTouchedSignalN(void)
{
  TestApplication          application;
  Dali::Integration::Scene scene = application.GetScene();

  TouchedSignalData data;
  TouchFunctor      functor(data);
  scene.TouchedSignal().Connect(&application, functor);

  // Render and notify.
  application.SendNotification();
  application.Render();

  // Confirm functor not called before there has been any touch event.
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  // No actors, single touch, down, motion then up.
  {
    GenerateTouch(application, PointState::DOWN, Vector2(10.0f, 10.0f));

    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_CHECK(data.receivedTouchEvent.GetPointCount() != 0u);
    DALI_TEST_CHECK(!data.receivedTouchEvent.GetHitActor(0));

    data.Reset();

    // Confirm there is no signal when the touchpoint is only moved.
    GenerateTouch(application, PointState::MOTION, Vector2(1200.0f, 10.0f)); // Some motion

    DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
    data.Reset();

    // Confirm a following up event generates a signal.
    GenerateTouch(application, PointState::UP, Vector2(1200.0f, 10.0f));

    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_CHECK(data.receivedTouchEvent.GetPointCount() != 0u);
    DALI_TEST_CHECK(!data.receivedTouchEvent.GetHitActor(0));
    data.Reset();
  }

  // Add an actor to the scene.
  Actor actor = Actor::New();
  actor.SetProperty(Actor::Property::SIZE, Vector2(100.0f, 100.0f));
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  actor.TouchedSignal().Connect(&DummyTouchCallback);
  scene.Add(actor);

  // Render and notify.
  application.SendNotification();
  application.Render();

  // Actor on scene. Interrupted before down and interrupted after down.
  {
    GenerateTouch(application, PointState::INTERRUPTED, Vector2(10.0f, 10.0f));

    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_CHECK(data.receivedTouchEvent.GetPointCount() != 0u);
    DALI_TEST_CHECK(!data.receivedTouchEvent.GetHitActor(0));
    DALI_TEST_CHECK(data.receivedTouchEvent.GetState(0) == PointState::INTERRUPTED);
    data.Reset();

    GenerateTouch(application, PointState::DOWN, Vector2(10.0f, 10.0f));

    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_CHECK(data.receivedTouchEvent.GetPointCount() != 0u);
    DALI_TEST_CHECK(data.receivedTouchEvent.GetHitActor(0) == actor);
    DALI_TEST_CHECK(data.receivedTouchEvent.GetState(0) == PointState::DOWN);
    data.Reset();

    GenerateTouch(application, PointState::INTERRUPTED, Vector2(10.0f, 10.0f));

    DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
    DALI_TEST_CHECK(data.receivedTouchEvent.GetPointCount() != 0u);
    DALI_TEST_CHECK(!data.receivedTouchEvent.GetHitActor(0));
    DALI_TEST_CHECK(data.receivedTouchEvent.GetState(0) == PointState::INTERRUPTED);

    DALI_TEST_EQUALS(data.receivedTouchEvent.GetPointCount(), 1u, TEST_LOCATION);

    // Check that getting info about a non-existent point returns an empty handle
    Actor actor = data.receivedTouchEvent.GetHitActor(1);
    DALI_TEST_CHECK(!actor);

    data.Reset();
  }

  END_TEST;
}

int UtcDaliSceneSignalWheelEventP(void)
{
  TestApplication          application;
  Dali::Integration::Scene scene = application.GetScene();

  WheelEventSignalData      data;
  WheelEventReceivedFunctor functor(data);
  scene.WheelEventSignal().Connect(&application, functor);

  Integration::WheelEvent event(Integration::WheelEvent::CUSTOM_WHEEL, 0, 0u, Vector2(0.0f, 0.0f), 1, 1000u);
  application.ProcessEvent(event);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(static_cast<WheelEvent::Type>(event.type) == data.receivedWheelEvent.GetType());
  DALI_TEST_CHECK(event.direction == data.receivedWheelEvent.GetDirection());
  DALI_TEST_CHECK(event.modifiers == data.receivedWheelEvent.GetModifiers());
  DALI_TEST_CHECK(event.point == data.receivedWheelEvent.GetPoint());
  DALI_TEST_CHECK(event.delta == data.receivedWheelEvent.GetDelta());
  DALI_TEST_CHECK(event.timeStamp == data.receivedWheelEvent.GetTime());

  data.Reset();

  Integration::WheelEvent event2(Integration::WheelEvent::CUSTOM_WHEEL, 0, 0u, Vector2(0.0f, 0.0f), -1, 1000u);
  application.ProcessEvent(event2);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(static_cast<WheelEvent::Type>(event2.type) == data.receivedWheelEvent.GetType());
  DALI_TEST_CHECK(event2.direction == data.receivedWheelEvent.GetDirection());
  DALI_TEST_CHECK(event2.modifiers == data.receivedWheelEvent.GetModifiers());
  DALI_TEST_CHECK(event2.point == data.receivedWheelEvent.GetPoint());
  DALI_TEST_CHECK(event2.delta == data.receivedWheelEvent.GetDelta());
  DALI_TEST_CHECK(event2.timeStamp == data.receivedWheelEvent.GetTime());
  END_TEST;
}

int UtcDaliSceneSurfaceResizedDefaultScene(void)
{
  tet_infoline("Ensure resizing of the surface is handled properly");

  TestApplication application;

  auto defaultScene = application.GetScene();
  DALI_TEST_CHECK(defaultScene);

  // Ensure stage size matches the scene size
  auto stage = Stage::GetCurrent();
  DALI_TEST_EQUALS(stage.GetSize(), defaultScene.GetSize(), TEST_LOCATION);

  // Resize the scene
  Vector2 newSize(1000.0f, 2000.0f);
  DALI_TEST_CHECK(stage.GetSize() != newSize);
  defaultScene.SurfaceResized(newSize.width, newSize.height);

  DALI_TEST_EQUALS(stage.GetSize(), newSize, TEST_LOCATION);
  DALI_TEST_EQUALS(defaultScene.GetSize(), newSize, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneSurfaceResizedDefaultSceneViewport(void)
{
  tet_infoline("Ensure resizing of the surface & viewport is handled properly");

  TestApplication    application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack&    callStack     = glAbstraction.GetViewportTrace();
  glAbstraction.EnableViewportCallTrace(true);

  // Initial scene setup
  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  actor.SetProperty(Actor::Property::SIZE, Vector2(400.0f, 400.0f));
  application.GetScene().Add(actor);

  // Render before resizing surface
  application.SendNotification();
  application.Render(0);
  glAbstraction.ResetViewportCallStack();

  auto defaultScene = application.GetScene();
  DALI_TEST_CHECK(defaultScene);

  // consume the resize flag by first rendering
  defaultScene.IsSurfaceRectChanged();

  // Ensure stage size matches the scene size
  auto stage = Stage::GetCurrent();
  DALI_TEST_EQUALS(stage.GetSize(), defaultScene.GetSize(), TEST_LOCATION);

  Rect<int32_t> surfaceRect = defaultScene.GetCurrentSurfaceRect();

  bool surfaceResized;
  // check resized flag before surface is resized.
  surfaceResized = defaultScene.IsSurfaceRectChanged();
  DALI_TEST_EQUALS(surfaceResized, false, TEST_LOCATION);

  // Resize the scene
  Vector2     newSize(1000.0f, 2000.0f);
  std::string viewportParams("0, 0, 1000, 2000"); // to match newSize
  DALI_TEST_CHECK(stage.GetSize() != newSize);
  defaultScene.SurfaceResized(newSize.width, newSize.height);

  DALI_TEST_EQUALS(stage.GetSize(), newSize, TEST_LOCATION);
  DALI_TEST_EQUALS(defaultScene.GetSize(), newSize, TEST_LOCATION);

  // Check current surface rect
  Rect<int32_t> newSurfaceRect = defaultScene.GetCurrentSurfaceRect();

  // It should not be changed yet.
  DALI_TEST_CHECK(surfaceRect == newSurfaceRect);

  // Render after resizing surface
  application.SendNotification();
  application.Render(0);

  surfaceResized = defaultScene.IsSurfaceRectChanged();
  DALI_TEST_EQUALS(surfaceResized, true, TEST_LOCATION);

  // Check that the viewport is handled properly
  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters("Viewport", viewportParams));

  // Check current surface rect
  newSurfaceRect = defaultScene.GetCurrentSurfaceRect();

  // It should be changed
  DALI_TEST_EQUALS(newSurfaceRect.x, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(newSurfaceRect.y, 0, TEST_LOCATION);
  DALI_TEST_EQUALS(newSurfaceRect.width, 1000, TEST_LOCATION);
  DALI_TEST_EQUALS(newSurfaceRect.height, 2000, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneSurfaceResizedMultipleRenderTasks(void)
{
  tet_infoline("Ensure resizing of the surface & viewport is handled properly");

  TestApplication    application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();
  TraceCallStack&    callStack     = glAbstraction.GetViewportTrace();
  glAbstraction.EnableViewportCallTrace(true);

  // Initial scene setup
  auto scene = application.GetScene();

  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  int testWidth  = 400;
  int testHeight = 400;
  actor.SetProperty(Actor::Property::SIZE, Vector2(testWidth, testHeight));
  scene.Add(actor);

  CameraActor offscreenCameraActor = CameraActor::New(Size(testWidth, testHeight));
  application.GetScene().Add(offscreenCameraActor);

  FrameBuffer newFrameBuffer = FrameBuffer::New(testWidth, testHeight, FrameBuffer::Attachment::NONE);

  RenderTask newTask = scene.GetRenderTaskList().CreateTask();
  newTask.SetCameraActor(offscreenCameraActor);
  newTask.SetSourceActor(actor);
  newTask.SetFrameBuffer(newFrameBuffer);
  newTask.SetViewportPosition(Vector2(0, 0));
  newTask.SetViewportSize(Vector2(testWidth, testHeight));

  // Render before resizing surface
  application.SendNotification();
  application.Render(0);
  glAbstraction.ResetViewportCallStack();

  Rect<int32_t> initialViewport = newTask.GetViewport();
  int           initialWidth    = initialViewport.width;
  int           initialHeight   = initialViewport.height;
  DALI_TEST_EQUALS(initialWidth, testWidth, TEST_LOCATION);
  DALI_TEST_EQUALS(initialHeight, testHeight, TEST_LOCATION);

  auto defaultScene = application.GetScene();
  DALI_TEST_CHECK(defaultScene);

  // Ensure stage size matches the scene size
  auto stage = Stage::GetCurrent();
  DALI_TEST_EQUALS(stage.GetSize(), defaultScene.GetSize(), TEST_LOCATION);

  // Resize the scene
  Vector2     newSize(1000.0f, 2000.0f);
  std::string viewportParams("0, 0, 1000, 2000"); // to match newSize
  DALI_TEST_CHECK(stage.GetSize() != newSize);
  defaultScene.SurfaceResized(newSize.width, newSize.height);

  DALI_TEST_EQUALS(stage.GetSize(), newSize, TEST_LOCATION);
  DALI_TEST_EQUALS(defaultScene.GetSize(), newSize, TEST_LOCATION);

  // Render after resizing surface
  application.SendNotification();
  application.Render(0);

  // Check that the viewport is handled properly
  DALI_TEST_CHECK(callStack.FindMethodAndGetParameters("Viewport", viewportParams));

  // Second render-task should not be affected
  Rect<int32_t> viewport = newTask.GetViewport();
  int           width    = viewport.width;
  int           height   = viewport.height;
  DALI_TEST_EQUALS(width, testWidth, TEST_LOCATION);
  DALI_TEST_EQUALS(height, testHeight, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneSurfaceResizedAdditionalScene(void)
{
  tet_infoline("Ensure resizing of the surface is handled properly on additional scenes");

  TestApplication application;
  Vector2         originalSurfaceSize(500.0f, 1000.0f);

  auto scene = Integration::Scene::New(Size(originalSurfaceSize.width, originalSurfaceSize.height));

  // Ensure stage size does NOT match the surface size
  auto       stage     = Stage::GetCurrent();
  const auto stageSize = stage.GetSize();
  DALI_TEST_CHECK(stageSize != originalSurfaceSize);
  DALI_TEST_EQUALS(originalSurfaceSize, scene.GetSize(), TEST_LOCATION);

  // Resize the surface and inform the scene accordingly
  Vector2 newSize(1000.0f, 2000.0f);
  DALI_TEST_CHECK(stage.GetSize() != newSize);
  scene.SurfaceResized(newSize.width, newSize.height);

  // Ensure the stage hasn't been resized
  DALI_TEST_EQUALS(stage.GetSize(), stageSize, TEST_LOCATION);
  DALI_TEST_EQUALS(scene.GetSize(), newSize, TEST_LOCATION);

  END_TEST;
}

#define CLIPPING_RECT_X (16)
#define CLIPPING_RECT_Y (768)
#define CLIPPING_RECT_WIDTH (32)
#define CLIPPING_RECT_HEIGHT (32)

int UtcDaliSceneSurfaceRotatedWithAngle0(void)
{
  tet_infoline("Ensure rotation of the surface is handled properly with Angle 0");

  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);

  const TestGlAbstraction::ScissorParams& glScissorParams(application.GetGlAbstraction().GetScissorParams());

  std::vector<Rect<int>> damagedRects;
  Rect<int>              clippingRect;
  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);

  DALI_TEST_EQUALS(damagedRects.size(), 0, TEST_LOCATION);

  clippingRect = TestApplication::DEFAULT_SURFACE_RECT;
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::POSITION, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetProperty(Actor::Property::SIZE, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  application.GetScene().Add(actor);

  application.SendNotification();

  damagedRects.clear();
  application.GetScene().SurfaceRotated(TestApplication::DEFAULT_SURFACE_WIDTH,
                                        TestApplication::DEFAULT_SURFACE_HEIGHT,
                                        0);

  // Check current surface orientation
  int32_t orientation = application.GetScene().GetCurrentSurfaceOrientation();

  // It should not be changed yet.
  DALI_TEST_EQUALS(orientation, 0, TEST_LOCATION);

  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  clippingRect = Rect<int>(CLIPPING_RECT_X, CLIPPING_RECT_Y, CLIPPING_RECT_WIDTH, CLIPPING_RECT_HEIGHT); // in screen coordinates, includes 3 last frames updates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  // Check current surface orientation
  orientation = application.GetScene().GetCurrentSurfaceOrientation();

  // It should be changed.
  DALI_TEST_EQUALS(orientation, 0, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneSurfaceRotatedWithAngle90(void)
{
  tet_infoline("Ensure rotation of the surface is handled properly with Angle 90");

  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);

  const TestGlAbstraction::ScissorParams& glScissorParams(application.GetGlAbstraction().GetScissorParams());

  std::vector<Rect<int>> damagedRects;
  Rect<int>              clippingRect;
  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);

  DALI_TEST_EQUALS(damagedRects.size(), 0, TEST_LOCATION);

  clippingRect = TestApplication::DEFAULT_SURFACE_RECT;
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::POSITION, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetProperty(Actor::Property::SIZE, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  application.GetScene().Add(actor);

  application.SendNotification();

  damagedRects.clear();
  application.GetScene().SurfaceRotated(TestApplication::DEFAULT_SURFACE_WIDTH,
                                        TestApplication::DEFAULT_SURFACE_HEIGHT,
                                        90);

  // Check current surface orientation
  int32_t orientation = application.GetScene().GetCurrentSurfaceOrientation();

  // It should not be changed yet.
  DALI_TEST_EQUALS(orientation, 0, TEST_LOCATION);

  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  clippingRect = Rect<int>(CLIPPING_RECT_X, CLIPPING_RECT_Y, CLIPPING_RECT_WIDTH, CLIPPING_RECT_HEIGHT); // in screen coordinates, includes 3 last frames updates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // It is recalculation for glScissor.
  // Because surface is rotated and glScissor is called with recalcurated value.
  clippingRect.x      = TestApplication::DEFAULT_SURFACE_HEIGHT - (CLIPPING_RECT_Y + CLIPPING_RECT_HEIGHT);
  clippingRect.y      = CLIPPING_RECT_X;
  clippingRect.width  = CLIPPING_RECT_HEIGHT;
  clippingRect.height = CLIPPING_RECT_WIDTH;

  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  // Check current surface orientation
  orientation = application.GetScene().GetCurrentSurfaceOrientation();

  // It should be changed.
  DALI_TEST_EQUALS(orientation, 90, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneSurfaceRotatedWithAngle180(void)
{
  tet_infoline("Ensure rotation of the surface is handled properly with Angle 180");

  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);

  const TestGlAbstraction::ScissorParams& glScissorParams(application.GetGlAbstraction().GetScissorParams());

  std::vector<Rect<int>> damagedRects;
  Rect<int>              clippingRect;
  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);

  DALI_TEST_EQUALS(damagedRects.size(), 0, TEST_LOCATION);

  clippingRect = TestApplication::DEFAULT_SURFACE_RECT;
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::POSITION, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetProperty(Actor::Property::SIZE, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  application.GetScene().Add(actor);

  application.SendNotification();

  damagedRects.clear();
  application.GetScene().SurfaceRotated(TestApplication::DEFAULT_SURFACE_WIDTH,
                                        TestApplication::DEFAULT_SURFACE_HEIGHT,
                                        180);

  // Check current surface orientation
  int32_t orientation = application.GetScene().GetCurrentSurfaceOrientation();

  // It should not be changed yet.
  DALI_TEST_EQUALS(orientation, 0, TEST_LOCATION);

  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  clippingRect = Rect<int>(CLIPPING_RECT_X, CLIPPING_RECT_Y, CLIPPING_RECT_WIDTH, CLIPPING_RECT_HEIGHT); // in screen coordinates, includes 3 last frames updates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // It is recalculation for glScissor.
  // Because surface is rotated and glScissor is called with recalcurated value.
  clippingRect.x      = TestApplication::DEFAULT_SURFACE_WIDTH - (CLIPPING_RECT_X + CLIPPING_RECT_WIDTH);
  clippingRect.y      = TestApplication::DEFAULT_SURFACE_HEIGHT - (CLIPPING_RECT_Y + CLIPPING_RECT_HEIGHT);
  clippingRect.width  = CLIPPING_RECT_WIDTH;
  clippingRect.height = CLIPPING_RECT_HEIGHT;

  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  // Check current surface orientation
  orientation = application.GetScene().GetCurrentSurfaceOrientation();

  // It should be changed.
  DALI_TEST_EQUALS(orientation, 180, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneSurfaceRotatedWithAngle270(void)
{
  tet_infoline("Ensure rotation of the surface is handled properly with Angle 270");

  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);

  const TestGlAbstraction::ScissorParams& glScissorParams(application.GetGlAbstraction().GetScissorParams());

  std::vector<Rect<int>> damagedRects;
  Rect<int>              clippingRect;
  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);

  DALI_TEST_EQUALS(damagedRects.size(), 0, TEST_LOCATION);

  clippingRect = TestApplication::DEFAULT_SURFACE_RECT;
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::POSITION, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetProperty(Actor::Property::SIZE, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  application.GetScene().Add(actor);

  application.SendNotification();

  damagedRects.clear();
  application.GetScene().SurfaceRotated(TestApplication::DEFAULT_SURFACE_WIDTH,
                                        TestApplication::DEFAULT_SURFACE_HEIGHT,
                                        270);

  // Check current surface orientation
  int32_t orientation = application.GetScene().GetCurrentSurfaceOrientation();

  // It should not be changed yet.
  DALI_TEST_EQUALS(orientation, 0, TEST_LOCATION);

  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  clippingRect = Rect<int>(CLIPPING_RECT_X, CLIPPING_RECT_Y, CLIPPING_RECT_WIDTH, CLIPPING_RECT_HEIGHT); // in screen coordinates, includes 3 last frames updates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // It is recalculation for glScissor.
  // Because surface is rotated and glScissor is called with recalcurated value.
  clippingRect.x      = CLIPPING_RECT_Y;
  clippingRect.y      = TestApplication::DEFAULT_SURFACE_WIDTH - (CLIPPING_RECT_X + CLIPPING_RECT_WIDTH);
  clippingRect.width  = CLIPPING_RECT_HEIGHT;
  clippingRect.height = CLIPPING_RECT_WIDTH;

  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  // Check current surface orientation
  orientation = application.GetScene().GetCurrentSurfaceOrientation();

  // It should be changed.
  DALI_TEST_EQUALS(orientation, 270, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneSetRotationCompletedAcknowledgementWithAngle90(void)
{
  tet_infoline("Ensure to acknowledge for completing surface 90 angle rotaiton");

  TestApplication application(
    TestApplication::DEFAULT_SURFACE_WIDTH,
    TestApplication::DEFAULT_SURFACE_HEIGHT,
    TestApplication::DEFAULT_HORIZONTAL_DPI,
    TestApplication::DEFAULT_VERTICAL_DPI,
    true,
    true);

  const TestGlAbstraction::ScissorParams& glScissorParams(application.GetGlAbstraction().GetScissorParams());

  std::vector<Rect<int>> damagedRects;
  Rect<int>              clippingRect;
  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);

  DALI_TEST_EQUALS(damagedRects.size(), 0, TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  Actor actor = CreateRenderableActor();
  actor.SetProperty(Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);
  actor.SetProperty(Actor::Property::POSITION, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetProperty(Actor::Property::SIZE, Vector3(16.0f, 16.0f, 0.0f));
  actor.SetResizePolicy(ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  application.GetScene().Add(actor);

  application.SendNotification();

  damagedRects.clear();
  application.GetScene().SurfaceRotated(TestApplication::DEFAULT_SURFACE_WIDTH,
                                        TestApplication::DEFAULT_SURFACE_HEIGHT,
                                        90);

  // Check current surface orientation
  int32_t orientation = application.GetScene().GetCurrentSurfaceOrientation();

  // It should not be changed yet.
  DALI_TEST_EQUALS(orientation, 0, TEST_LOCATION);

  application.GetScene().SetRotationCompletedAcknowledgement();

  application.SendNotification();
  application.PreRenderWithPartialUpdate(TestApplication::RENDER_FRAME_INTERVAL, nullptr, damagedRects);
  DALI_TEST_EQUALS(damagedRects.size(), 1, TEST_LOCATION);

  clippingRect = Rect<int>(CLIPPING_RECT_X, CLIPPING_RECT_Y, CLIPPING_RECT_WIDTH, CLIPPING_RECT_HEIGHT); // in screen coordinates, includes 3 last frames updates
  DALI_TEST_EQUALS<Rect<int>>(clippingRect, damagedRects[0], TEST_LOCATION);
  application.RenderWithPartialUpdate(damagedRects, clippingRect);

  // It is recalculation for glScissor.
  // Because surface is rotated and glScissor is called with recalcurated value.
  clippingRect.x      = TestApplication::DEFAULT_SURFACE_HEIGHT - (CLIPPING_RECT_Y + CLIPPING_RECT_HEIGHT);
  clippingRect.y      = CLIPPING_RECT_X;
  clippingRect.width  = CLIPPING_RECT_HEIGHT;
  clippingRect.height = CLIPPING_RECT_WIDTH;

  DALI_TEST_EQUALS(clippingRect.x, glScissorParams.x, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.y, glScissorParams.y, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.width, glScissorParams.width, TEST_LOCATION);
  DALI_TEST_EQUALS(clippingRect.height, glScissorParams.height, TEST_LOCATION);

  // Check current surface orientation
  orientation = application.GetScene().GetCurrentSurfaceOrientation();

  // It should be changed.
  DALI_TEST_EQUALS(orientation, 90, TEST_LOCATION);

  bool isSetRotationCompletedAcknowledgementSet = application.GetScene().IsRotationCompletedAcknowledgementSet();
  DALI_TEST_EQUALS(isSetRotationCompletedAcknowledgementSet, true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneKeyEventGeneratedSignalP(void)
{
  TestApplication          application;
  Dali::Integration::Scene scene = application.GetScene();

  KeyEventGeneratedSignalData      data;
  KeyEventGeneratedReceivedFunctor functor(data);
  scene.KeyEventGeneratedSignal().Connect(&application, functor);

  Integration::KeyEvent event("a", "", "a", 0, 0, 0, Integration::KeyEvent::UP, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(event);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(event.keyModifier == data.receivedKeyEvent.GetKeyModifier());
  DALI_TEST_CHECK(event.keyName == data.receivedKeyEvent.GetKeyName());
  DALI_TEST_CHECK(event.keyString == data.receivedKeyEvent.GetKeyString());
  DALI_TEST_CHECK(event.state == static_cast<Integration::KeyEvent::State>(data.receivedKeyEvent.GetState()));

  data.Reset();

  Integration::KeyEvent event2("i", "", "i", 0, 0, 0, Integration::KeyEvent::UP, "i", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(event2);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(event2.keyModifier == data.receivedKeyEvent.GetKeyModifier());
  DALI_TEST_CHECK(event2.keyName == data.receivedKeyEvent.GetKeyName());
  DALI_TEST_CHECK(event2.keyString == data.receivedKeyEvent.GetKeyString());
  DALI_TEST_CHECK(event2.state == static_cast<Integration::KeyEvent::State>(data.receivedKeyEvent.GetState()));

  data.Reset();

  Integration::KeyEvent event3("a", "", "a", 0, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(event3);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(event3.keyModifier == data.receivedKeyEvent.GetKeyModifier());
  DALI_TEST_CHECK(event3.keyName == data.receivedKeyEvent.GetKeyName());
  DALI_TEST_CHECK(event3.keyString == data.receivedKeyEvent.GetKeyString());
  DALI_TEST_CHECK(event3.state == static_cast<Integration::KeyEvent::State>(data.receivedKeyEvent.GetState()));

  data.Reset();

  Integration::KeyEvent event4("a", "", "a", 0, 0, 0, Integration::KeyEvent::UP, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(event4);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(event4.keyModifier == data.receivedKeyEvent.GetKeyModifier());
  DALI_TEST_CHECK(event4.keyName == data.receivedKeyEvent.GetKeyName());
  DALI_TEST_CHECK(event4.keyString == data.receivedKeyEvent.GetKeyString());
  DALI_TEST_CHECK(event4.state == static_cast<Integration::KeyEvent::State>(data.receivedKeyEvent.GetState()));
  END_TEST;
}

int UtcDaliSceneEnsureReplacedSurfaceKeepsClearColor(void)
{
  tet_infoline("Ensure we keep background color when the scene surface is replaced ");

  TestApplication application;

  // Create a new scene and set the background color of the main scene
  auto defaultScene = application.GetScene();
  defaultScene.SetBackgroundColor(Color::BLUE);

  // Need to create a renderable as we don't start rendering until we have at least one
  // We don't need to add this to any scene
  auto actor = CreateRenderableActor();

  auto& glAbstraction    = application.GetGlAbstraction();
  auto  clearCountBefore = glAbstraction.GetClearCountCalled();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(glAbstraction.GetClearCountCalled(), clearCountBefore + 1, TEST_LOCATION);
  DALI_TEST_EQUALS(glAbstraction.GetLastClearColor(), Color::BLUE, TEST_LOCATION);

  defaultScene.SurfaceReplaced();

  application.SendNotification();
  application.Render();

  DALI_TEST_EQUALS(glAbstraction.GetClearCountCalled(), clearCountBefore + 2, TEST_LOCATION);
  DALI_TEST_EQUALS(glAbstraction.GetLastClearColor(), Color::BLUE, TEST_LOCATION);

  // Check when the main render task viewport is set the clear color is clipped using scissors
  TraceCallStack& scissorTrace        = glAbstraction.GetScissorTrace();
  TraceCallStack& enabledDisableTrace = glAbstraction.GetEnableDisableTrace();
  scissorTrace.Enable(true);
  enabledDisableTrace.Enable(true);

  defaultScene.GetRenderTaskList().GetTask(0).SetViewport(Viewport(0.0f, 0.0f, 100.0f, 100.0f));

  application.SendNotification();
  application.Render();

  // Check scissor test was enabled.
  std::ostringstream scissor;
  scissor << std::hex << GL_SCISSOR_TEST;
  DALI_TEST_CHECK(enabledDisableTrace.FindMethodAndParams("Enable", scissor.str()));

  // Check the scissor was set, and the coordinates are correct.
  DALI_TEST_CHECK(scissorTrace.FindMethodAndParams("Scissor", "0, 700, 100, 100"));

  DALI_TEST_EQUALS(glAbstraction.GetClearCountCalled(), clearCountBefore + 3, TEST_LOCATION);
  DALI_TEST_EQUALS(glAbstraction.GetLastClearColor(), Color::BLUE, TEST_LOCATION);

  scissorTrace.Enable(false);
  scissorTrace.Reset();

  enabledDisableTrace.Enable(false);
  enabledDisableTrace.Reset();

  END_TEST;
}

int UtcDaliSceneEnsureRenderTargetRecreated(void)
{
  tet_infoline("Ensure render target is re-created when surface replaced ");

  TestApplication application;

  // Create a new scene and set the background color of the main scene
  auto defaultScene = application.GetScene();
  defaultScene.SetBackgroundColor(Color::BLUE);

  auto actor = CreateRenderableActor();
  defaultScene.Add(actor);

  auto& graphicsController = application.GetGraphicsController();

  application.SendNotification();
  application.Render();

  TraceCallStack&                    graphicsCallStack = graphicsController.mCallStack;
  TraceCallStack::NamedParams        empty{};
  const TraceCallStack::NamedParams* matching = graphicsCallStack.FindLastMatch("PresentRenderTarget", empty);
  DALI_TEST_CHECK(matching != nullptr);

  graphicsCallStack.Reset();

  int                              fakeSurface1;
  Graphics::RenderTargetCreateInfo createInfo{};
  createInfo.SetSurface(&fakeSurface1).SetExtent(Graphics::Extent2D{480u, 800u});
  defaultScene.SetSurfaceRenderTarget(createInfo);

  application.SendNotification();
  application.Render();

  TraceCallStack::NamedParams query1;
  query1["surface"] << std::hex << &fakeSurface1;
  const TraceCallStack::NamedParams* matching2 = graphicsCallStack.FindLastMatch("CreateRenderTarget", query1);
  DALI_TEST_CHECK(matching2 != nullptr);

  const TraceCallStack::NamedParams* matching3 = graphicsCallStack.FindLastMatch("PresentRenderTarget", empty);
  DALI_TEST_CHECK(matching3 != nullptr);
  DALI_TEST_EQUALS((*matching3)["surface"].str(), query1["surface"].str(), TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneEmptySceneRendering(void)
{
  tet_infoline("Ensure not rendering before a Renderer is added");

  TestApplication    application;
  TestGlAbstraction& glAbstraction = application.GetGlAbstraction();

  // Render without any renderer
  application.SendNotification();
  application.Render();

  // Check the clear count and the render status
  DALI_TEST_EQUALS(glAbstraction.GetClearCountCalled(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetRenderNeedsPostRender(), false, TEST_LOCATION);

  // Add a Renderer
  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  // Render before adding renderer
  application.SendNotification();
  application.Render();

  // Check the clear count and the render status
  DALI_TEST_EQUALS(glAbstraction.GetClearCountCalled(), 0, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetRenderNeedsPostRender(), false, TEST_LOCATION);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);

  actor.SetProperty(Actor::Property::SIZE, Vector2(400, 400));
  application.GetScene().Add(actor);

  // Render
  application.SendNotification();
  application.Render();

  // Check the clear count and the render status
  DALI_TEST_EQUALS(glAbstraction.GetClearCountCalled(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(application.GetRenderNeedsPostRender(), true, TEST_LOCATION);

  // Remove the Renderer
  application.GetScene().Remove(actor);
  actor.Reset();
  renderer.Reset();

  // Render
  application.SendNotification();
  application.Render();

  // Check the clear count and the render status
  DALI_TEST_EQUALS(glAbstraction.GetClearCountCalled(), 2, TEST_LOCATION); // Should be cleared
  DALI_TEST_EQUALS(application.GetRenderNeedsPostRender(), true, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneFrameRenderedPresentedCallback(void)
{
  tet_infoline("UtcDaliSceneFrameRenderedCallback");

  TestApplication application;

  // Add a Renderer
  Geometry geometry = CreateQuadGeometry();
  Shader   shader   = CreateShader();
  Renderer renderer = Renderer::New(geometry, shader);

  Actor actor = Actor::New();
  actor.AddRenderer(renderer);
  application.GetScene().Add(actor);

  Dali::Integration::Scene scene = application.GetScene();

  int frameId = 1;
  scene.AddFrameRenderedCallback(std::unique_ptr<CallbackBase>(MakeCallback(&FrameCallback)), frameId);
  scene.AddFramePresentedCallback(std::unique_ptr<CallbackBase>(MakeCallback(&FrameCallback)), frameId);

  // Render
  application.SendNotification();
  application.Render();

  Dali::Integration::Scene::FrameCallbackContainer callbackContainer;
  scene.GetFrameRenderedCallback(callbackContainer);

  DALI_TEST_EQUALS(callbackContainer.size(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(callbackContainer[0].second, frameId, TEST_LOCATION);

  callbackContainer.clear();

  scene.GetFramePresentedCallback(callbackContainer);

  DALI_TEST_EQUALS(callbackContainer.size(), 1, TEST_LOCATION);
  DALI_TEST_EQUALS(callbackContainer[0].second, frameId, TEST_LOCATION);

  END_TEST;
}

int UtcDaliSceneWheelEventGeneratedSignalP(void)
{
  TestApplication          application;
  Dali::Integration::Scene scene = application.GetScene();

  WheelEventGeneratedSignalData      data;
  WheelEventGeneratedReceivedFunctor functor(data);
  scene.WheelEventGeneratedSignal().Connect(&application, functor);

  Integration::WheelEvent event(Integration::WheelEvent::CUSTOM_WHEEL, 0, 0u, Vector2(0.0f, 0.0f), 1, 1000u);
  application.ProcessEvent(event);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(static_cast<WheelEvent::Type>(event.type) == data.receivedWheelEvent.GetType());
  DALI_TEST_CHECK(event.direction == data.receivedWheelEvent.GetDirection());
  DALI_TEST_CHECK(event.modifiers == data.receivedWheelEvent.GetModifiers());
  DALI_TEST_CHECK(event.point == data.receivedWheelEvent.GetPoint());
  DALI_TEST_CHECK(event.delta == data.receivedWheelEvent.GetDelta());
  DALI_TEST_CHECK(event.timeStamp == data.receivedWheelEvent.GetTime());

  data.Reset();

  Integration::WheelEvent event2(Integration::WheelEvent::CUSTOM_WHEEL, 0, 0u, Vector2(0.0f, 0.0f), -1, 1000u);
  application.ProcessEvent(event2);

  DALI_TEST_EQUALS(true, data.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(static_cast<WheelEvent::Type>(event2.type) == data.receivedWheelEvent.GetType());
  DALI_TEST_CHECK(event2.direction == data.receivedWheelEvent.GetDirection());
  DALI_TEST_CHECK(event2.modifiers == data.receivedWheelEvent.GetModifiers());
  DALI_TEST_CHECK(event2.point == data.receivedWheelEvent.GetPoint());
  DALI_TEST_CHECK(event2.delta == data.receivedWheelEvent.GetDelta());
  DALI_TEST_CHECK(event2.timeStamp == data.receivedWheelEvent.GetTime());
  END_TEST;
}

int UtcDaliSceneSignalInterceptKeyEventP(void)
{
  TestApplication          application;
  Dali::Integration::Scene scene = application.GetScene();

  KeyEventSignalData      data;
  KeyEventReceivedFunctor functor(data);
  scene.KeyEventSignal().Connect(&application, functor);

  KeyEventGeneratedSignalData      interceptData;
  KeyEventGeneratedReceivedFunctor interceptFunctor(interceptData);
  scene.InterceptKeyEventSignal().Connect(&application, interceptFunctor);

  Integration::KeyEvent event("i", "", "i", 0, 0, 0, Integration::KeyEvent::DOWN, "i", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(event);

  DALI_TEST_EQUALS(true, interceptData.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(event.keyModifier == interceptData.receivedKeyEvent.GetKeyModifier());
  DALI_TEST_CHECK(event.keyName == interceptData.receivedKeyEvent.GetKeyName());
  DALI_TEST_CHECK(event.keyString == interceptData.receivedKeyEvent.GetKeyString());
  DALI_TEST_CHECK(event.state == static_cast<Integration::KeyEvent::State>(interceptData.receivedKeyEvent.GetState()));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  data.Reset();
  interceptData.Reset();

  Integration::KeyEvent event2("i", "", "i", 0, 0, 0, Integration::KeyEvent::UP, "i", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(event2);

  DALI_TEST_EQUALS(true, interceptData.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(event2.keyModifier == interceptData.receivedKeyEvent.GetKeyModifier());
  DALI_TEST_CHECK(event2.keyName == interceptData.receivedKeyEvent.GetKeyName());
  DALI_TEST_CHECK(event2.keyString == interceptData.receivedKeyEvent.GetKeyString());
  DALI_TEST_CHECK(event2.state == static_cast<Integration::KeyEvent::State>(interceptData.receivedKeyEvent.GetState()));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  data.Reset();
  interceptData.Reset();

  Integration::KeyEvent event3("a", "", "a", 0, 0, 0, Integration::KeyEvent::DOWN, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(event3);

  DALI_TEST_EQUALS(true, interceptData.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(event3.keyModifier == interceptData.receivedKeyEvent.GetKeyModifier());
  DALI_TEST_CHECK(event3.keyName == interceptData.receivedKeyEvent.GetKeyName());
  DALI_TEST_CHECK(event3.keyString == interceptData.receivedKeyEvent.GetKeyString());
  DALI_TEST_CHECK(event3.state == static_cast<Integration::KeyEvent::State>(interceptData.receivedKeyEvent.GetState()));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  data.Reset();
  interceptData.Reset();

  Integration::KeyEvent event4("a", "", "a", 0, 0, 0, Integration::KeyEvent::UP, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE);
  application.ProcessEvent(event4);

  DALI_TEST_EQUALS(true, interceptData.functorCalled, TEST_LOCATION);
  DALI_TEST_CHECK(event4.keyModifier == interceptData.receivedKeyEvent.GetKeyModifier());
  DALI_TEST_CHECK(event4.keyName == interceptData.receivedKeyEvent.GetKeyName());
  DALI_TEST_CHECK(event4.keyString == interceptData.receivedKeyEvent.GetKeyString());
  DALI_TEST_CHECK(event4.state == static_cast<Integration::KeyEvent::State>(interceptData.receivedKeyEvent.GetState()));
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);
  END_TEST;
}

int UtcDaliSceneSignalInterceptKeyEventN(void)
{
  TestApplication          application;
  Dali::Integration::Scene scene = application.GetScene();

  KeyEventGeneratedSignalData      data;
  KeyEventGeneratedReceivedFunctor functor(data);
  scene.InterceptKeyEventSignal().Connect(&application, functor);

  // Check that a non-pressed key events data is not modified.
  DALI_TEST_EQUALS(false, data.functorCalled, TEST_LOCATION);

  END_TEST;
}