/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <iostream>

#include <stdlib.h>
#include <dali/integration-api/scene.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/public-api/events/key-event.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/wheel-event-integ.h>

#include <dali-test-suite-utils.h>

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
  EventProcessingFinishedFunctor( bool& eventProcessingFinished )
  : mEventProcessingFinished( eventProcessingFinished )
  {}

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
  {}

  void Reset()
  {
    functorCalled = false;

    receivedKeyEvent.keyModifier = 0;
    receivedKeyEvent.keyPressedName.clear();
    receivedKeyEvent.keyPressed.clear();
  }

  bool functorCalled;
  KeyEvent receivedKeyEvent;
};

// Functor that sets the data when called
struct KeyEventReceivedFunctor
{
  KeyEventReceivedFunctor( KeyEventSignalData& data ) : signalData( data ) { }

  bool operator()( const KeyEvent& keyEvent )
  {
    signalData.functorCalled = true;
    signalData.receivedKeyEvent = keyEvent;

    return true;
  }

  KeyEventSignalData& signalData;
};

// Stores data that is populated in the touched signal callback and will be read by the TET cases
struct TouchedSignalData
{
  TouchedSignalData()
  : functorCalled(false)
  {}

  void Reset()
  {
    functorCalled = false;

    receivedTouchEvent.points.clear();
    receivedTouchEvent.time = 0;

    receivedTouchData.Reset();
  }

  bool functorCalled;
  TouchEvent receivedTouchEvent;
  TouchData receivedTouchData;
};

// Functor that sets the data when touched signal is received
struct TouchedFunctor
{
  TouchedFunctor( TouchedSignalData& data ) : signalData( data ) { }

  void operator()( const TouchEvent& touch )
  {
    signalData.functorCalled = true;
    signalData.receivedTouchEvent = touch;
  }

  TouchedSignalData& signalData;
};

// Functor that sets the data when touched signal is received
struct TouchFunctor
{
  TouchFunctor( TouchedSignalData& data ) : signalData( data ) { }

  void operator()( const TouchData& touch )
  {
    signalData.functorCalled = true;
    signalData.receivedTouchData = touch;
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
  {}

  void Reset()
  {
    functorCalled = false;
  }

  bool functorCalled;
  WheelEvent receivedWheelEvent;
};

// Functor that sets the data when wheel-event signal is received
struct WheelEventReceivedFunctor
{
  WheelEventReceivedFunctor( WheelEventSignalData& data ) : signalData( data ) { }

  bool operator()( const WheelEvent& wheelEvent )
  {
    signalData.functorCalled = true;
    signalData.receivedWheelEvent = wheelEvent;

    return true;
  }

  WheelEventSignalData& signalData;
};

void GenerateTouch( TestApplication& application, PointState::Type state, const Vector2& screenPosition )
{
  Integration::TouchEvent touchEvent;
  Integration::Point point;
  point.SetState( state );
  point.SetScreenPosition( screenPosition );
  touchEvent.points.push_back( point );
  application.ProcessEvent( touchEvent );
}

bool DummyTouchCallback( Actor actor, const TouchEvent& touch )
{
  return true;
}

} // unnamed namespace

int UtcDaliSceneAdd(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Scene::Add");

  Dali::Integration::Scene scene = application.GetScene();

  Actor actor = Actor::New();
  DALI_TEST_CHECK( !actor.OnStage() );

  scene.Add( actor );
  DALI_TEST_CHECK( actor.OnStage() );

  END_TEST;
}

int UtcDaliSceneRemove(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Scene::Remove");

  Dali::Integration::Scene scene = application.GetScene();

  Actor actor = Actor::New();
  DALI_TEST_CHECK( !actor.OnStage() );

  scene.Add( actor );
  DALI_TEST_CHECK( actor.OnStage() );

  scene.Remove(actor);
  DALI_TEST_CHECK( !actor.OnStage() );

  END_TEST;
}

int UtcDaliSceneGetSize(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Scene::GetSize");

  Dali::Integration::Scene scene = application.GetScene();
  Size size = scene.GetSize();
  DALI_TEST_EQUALS( TestApplication::DEFAULT_SURFACE_WIDTH, size.width, TEST_LOCATION );
  DALI_TEST_EQUALS( TestApplication::DEFAULT_SURFACE_HEIGHT, size.height, TEST_LOCATION );

  END_TEST;
}

int UtcDaliSceneGetDpi(void)
{
  TestApplication application; // Initializes core DPI to default values

  // Test that setting core DPI explicitly also sets up the scene's DPI.
  Dali::Integration::Scene scene = application.GetScene();
  scene.SetDpi( Vector2(200.0f, 180.0f) );
  Vector2 dpi = scene.GetDpi();
  DALI_TEST_EQUALS( dpi.x, 200.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( dpi.y, 180.0f, TEST_LOCATION );
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
  DALI_TEST_EQUALS( tasks.GetTaskCount(), 1u, TEST_LOCATION );

  // RenderTaskList has it's own UTC tests.
  // But we can confirm that GetRenderTaskList in Stage retrieves the same RenderTaskList each time.
  RenderTask newTask = scene.GetRenderTaskList().CreateTask();

  DALI_TEST_EQUALS( scene.GetRenderTaskList().GetTask( 1 ), newTask, TEST_LOCATION );

  END_TEST;
}

int UtcDaliSceneGetRootLayer(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Scene::GetRootLayer");

  Dali::Integration::Scene scene = application.GetScene();
  Layer layer = scene.GetLayer( 0 );
  DALI_TEST_CHECK( layer );

  // Check that GetRootLayer() correctly retreived layer 0.
  DALI_TEST_CHECK( scene.GetRootLayer() == layer );

  END_TEST;
}

int UtcDaliSceneGetLayerCount(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Scene::GetLayerCount");

  Dali::Integration::Scene scene = application.GetScene();
  // Initially we have a default layer
  DALI_TEST_EQUALS( scene.GetLayerCount(), 1u, TEST_LOCATION );

  Layer layer = Layer::New();
  scene.Add( layer );

  DALI_TEST_EQUALS( scene.GetLayerCount(), 2u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliSceneGetLayer(void)
{
  TestApplication application;
  tet_infoline("Testing Dali::Integration::Scene::GetLayer");

  Dali::Integration::Scene scene = application.GetScene();

  Layer rootLayer = scene.GetLayer( 0 );
  DALI_TEST_CHECK( rootLayer );

  Layer layer = Layer::New();
  scene.Add( layer );

  Layer sameLayer = scene.GetLayer( 1 );
  DALI_TEST_CHECK( layer == sameLayer );

  END_TEST;
}

int UtcDaliSceneEventProcessingFinishedP(void)
{
  TestApplication application;
  Dali::Integration::Scene scene = application.GetScene();

  bool eventProcessingFinished = false;
  EventProcessingFinishedFunctor functor( eventProcessingFinished );
  scene.EventProcessingFinishedSignal().Connect( &application, functor );

  Actor actor( Actor::New() );
  scene.Add( actor );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( eventProcessingFinished );

  END_TEST;
}

int UtcDaliSceneEventProcessingFinishedN(void)
{
  TestApplication application;
  Dali::Integration::Scene scene = application.GetScene();

  bool eventProcessingFinished = false;
  EventProcessingFinishedFunctor functor( eventProcessingFinished );
  scene.EventProcessingFinishedSignal().Connect( &application, functor );

  Actor actor( Actor::New() );
  scene.Add( actor );

  // Do not complete event processing and confirm the signal has not been emitted.
  DALI_TEST_CHECK( !eventProcessingFinished );

  END_TEST;
}

int UtcDaliSceneSignalKeyEventP(void)
{
  TestApplication application;
  Dali::Integration::Scene scene = application.GetScene();

  KeyEventSignalData data;
  KeyEventReceivedFunctor functor( data );
  scene.KeyEventSignal().Connect( &application, functor );

  Integration::KeyEvent event( "i", "", "i", 0, 0, 0, Integration::KeyEvent::Down, "i", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE );
  application.ProcessEvent( event );

  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( event.keyModifier == data.receivedKeyEvent.keyModifier );
  DALI_TEST_CHECK( event.keyName == data.receivedKeyEvent.keyPressedName );
  DALI_TEST_CHECK( event.keyString == data.receivedKeyEvent.keyPressed );
  DALI_TEST_CHECK( event.state == static_cast<Integration::KeyEvent::State>( data.receivedKeyEvent.state ) );

  data.Reset();

  Integration::KeyEvent event2( "i", "", "i", 0, 0, 0, Integration::KeyEvent::Up, "i", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE );
  application.ProcessEvent( event2 );

  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( event2.keyModifier == data.receivedKeyEvent.keyModifier );
  DALI_TEST_CHECK( event2.keyName == data.receivedKeyEvent.keyPressedName );
  DALI_TEST_CHECK( event2.keyString == data.receivedKeyEvent.keyPressed );
  DALI_TEST_CHECK( event2.state == static_cast<Integration::KeyEvent::State>( data.receivedKeyEvent.state ) );

  data.Reset();

  Integration::KeyEvent event3( "a", "", "a", 0, 0, 0, Integration::KeyEvent::Down, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE );
  application.ProcessEvent( event3 );

  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( event3.keyModifier == data.receivedKeyEvent.keyModifier );
  DALI_TEST_CHECK( event3.keyName == data.receivedKeyEvent.keyPressedName );
  DALI_TEST_CHECK( event3.keyString == data.receivedKeyEvent.keyPressed );
  DALI_TEST_CHECK( event3.state == static_cast<Integration::KeyEvent::State>( data.receivedKeyEvent.state ) );

  data.Reset();

  Integration::KeyEvent event4( "a", "", "a", 0, 0, 0, Integration::KeyEvent::Up, "a", DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE );
  application.ProcessEvent( event4 );

  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( event4.keyModifier == data.receivedKeyEvent.keyModifier );
  DALI_TEST_CHECK( event4.keyName == data.receivedKeyEvent.keyPressedName );
  DALI_TEST_CHECK( event4.keyString == data.receivedKeyEvent.keyPressed );
  DALI_TEST_CHECK( event4.state == static_cast<Integration::KeyEvent::State>( data.receivedKeyEvent.state ) );
  END_TEST;
}

int UtcDaliSceneSignalKeyEventN(void)
{
  TestApplication application;
  Dali::Integration::Scene scene = application.GetScene();

  KeyEventSignalData data;
  KeyEventReceivedFunctor functor( data );
  scene.KeyEventSignal().Connect( &application, functor );

  // Check that a non-pressed key events data is not modified.
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );

  END_TEST;
}

int UtcDaliSceneTouchSignalP(void)
{
  TestApplication application;
  Dali::Integration::Scene scene = application.GetScene();

  TouchedSignalData data;
  TouchFunctor functor( data );
  scene.TouchSignal().Connect( &application, functor );

  // Render and notify.
  application.SendNotification();
  application.Render();

  // Basic test: No actors, single touch (down then up).
  {
    GenerateTouch( application, PointState::DOWN, Vector2( 10.0f, 10.0f ) );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchData.GetPointCount() != 0u );
    DALI_TEST_CHECK( !data.receivedTouchData.GetHitActor(0) );
    data.Reset();

    GenerateTouch( application, PointState::UP, Vector2( 10.0f, 10.0f ) );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchData.GetPointCount() != 0u );
    DALI_TEST_CHECK( !data.receivedTouchData.GetHitActor(0) );
    data.Reset();
  }

  // Add an actor to the scene.
  Actor actor = Actor::New();
  actor.SetSize( 100.0f, 100.0f );
  actor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  actor.SetParentOrigin( ParentOrigin::TOP_LEFT );
  actor.TouchedSignal().Connect( &DummyTouchCallback );
  scene.Add( actor );

  // Render and notify.
  application.SendNotification();
  application.Render();

  // Actor on scene, single touch, down in actor, motion, then up outside actor.
  {
    GenerateTouch( application, PointState::DOWN, Vector2( 10.0f, 10.0f ) );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchData.GetPointCount() != 0u );
    DALI_TEST_CHECK( data.receivedTouchData.GetHitActor(0) == actor );
    data.Reset();

    GenerateTouch( application, PointState::MOTION, Vector2( 150.0f, 10.0f ) ); // Some motion

    DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
    data.Reset();

    GenerateTouch( application, PointState::UP, Vector2( 150.0f, 10.0f ) ); // Some motion

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchData.GetPointCount() != 0u );
    DALI_TEST_CHECK( !data.receivedTouchData.GetHitActor(0) );
    data.Reset();
  }

  // Multiple touch. Should only receive a touch on first down and last up.
  {
    Integration::TouchEvent touchEvent;
    Integration::Point point;

    // 1st point
    point.SetState( PointState::DOWN );
    point.SetScreenPosition( Vector2( 10.0f, 10.0f ) );
    touchEvent.points.push_back( point );
    application.ProcessEvent( touchEvent );
    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_EQUALS( data.receivedTouchData.GetPointCount(), 1u, TEST_LOCATION );
    data.Reset();

    // 2nd point
    touchEvent.points[0].SetState( PointState::STATIONARY );
    point.SetDeviceId( 1 );
    point.SetScreenPosition( Vector2( 50.0f, 50.0f ) );
    touchEvent.points.push_back( point );
    application.ProcessEvent( touchEvent );
    DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
    data.Reset();

    // Primary point is up
    touchEvent.points[0].SetState( PointState::UP );
    touchEvent.points[1].SetState( PointState::STATIONARY );
    application.ProcessEvent( touchEvent );
    DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
    data.Reset();

    // Remove 1st point now, 2nd point is now in motion
    touchEvent.points.erase( touchEvent.points.begin() );
    touchEvent.points[0].SetState( PointState::MOTION );
    touchEvent.points[0].SetScreenPosition( Vector2( 150.0f, 50.0f ) );
    application.ProcessEvent( touchEvent );
    DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
    data.Reset();

    // Final point Up
    touchEvent.points[0].SetState( PointState::UP );
    application.ProcessEvent( touchEvent );
    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_EQUALS( data.receivedTouchData.GetPointCount(), 1u, TEST_LOCATION );
    data.Reset();
  }
  END_TEST;
}

int UtcDaliSceneTouchSignalN(void)
{
  TestApplication application;
  Dali::Integration::Scene scene = application.GetScene();

  TouchedSignalData data;
  TouchFunctor functor( data );
  scene.TouchSignal().Connect( &application, functor );

  // Render and notify.
  application.SendNotification();
  application.Render();

  // Confirm functor not called before there has been any touch event.
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );

  // No actors, single touch, down, motion then up.
  {
    GenerateTouch( application, PointState::DOWN, Vector2( 10.0f, 10.0f ) );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchData.GetPointCount() != 0u );
    DALI_TEST_CHECK( !data.receivedTouchData.GetHitActor(0));

    data.Reset();

    // Confirm there is no signal when the touchpoint is only moved.
    GenerateTouch( application, PointState::MOTION, Vector2( 1200.0f, 10.0f ) ); // Some motion

    DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
    data.Reset();

    // Confirm a following up event generates a signal.
    GenerateTouch( application, PointState::UP, Vector2( 1200.0f, 10.0f ) );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchData.GetPointCount() != 0u );
    DALI_TEST_CHECK( !data.receivedTouchData.GetHitActor(0));
    data.Reset();
  }

  // Add an actor to the scene.
  Actor actor = Actor::New();
  actor.SetSize( 100.0f, 100.0f );
  actor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  actor.SetParentOrigin( ParentOrigin::TOP_LEFT );
  actor.TouchedSignal().Connect( &DummyTouchCallback );
  scene.Add( actor );

  // Render and notify.
  application.SendNotification();
  application.Render();

  // Actor on scene. Interrupted before down and interrupted after down.
  {
    GenerateTouch( application, PointState::INTERRUPTED, Vector2( 10.0f, 10.0f ) );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchData.GetPointCount() != 0u );
    DALI_TEST_CHECK( !data.receivedTouchData.GetHitActor(0) );
    DALI_TEST_CHECK( data.receivedTouchData.GetState(0) == PointState::INTERRUPTED );
    data.Reset();

    GenerateTouch( application, PointState::DOWN, Vector2( 10.0f, 10.0f ) );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchData.GetPointCount() != 0u );
    DALI_TEST_CHECK( data.receivedTouchData.GetHitActor(0) == actor );
    DALI_TEST_CHECK( data.receivedTouchData.GetState(0) == PointState::DOWN );
    data.Reset();

    GenerateTouch( application, PointState::INTERRUPTED, Vector2( 10.0f, 10.0f ) );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchData.GetPointCount() != 0u );
    DALI_TEST_CHECK( !data.receivedTouchData.GetHitActor(0) );
    DALI_TEST_CHECK( data.receivedTouchData.GetState(0) == PointState::INTERRUPTED );

    DALI_TEST_EQUALS( data.receivedTouchData.GetPointCount(), 1u, TEST_LOCATION );

    // Check that getting info about a non-existent point returns an empty handle
    Actor actor = data.receivedTouchData.GetHitActor( 1 );
    DALI_TEST_CHECK( !actor );

    data.Reset();
  }

  END_TEST;
}

int UtcDaliSceneSignalWheelEventP(void)
{
  TestApplication application;
  Dali::Integration::Scene scene = application.GetScene();

  WheelEventSignalData data;
  WheelEventReceivedFunctor functor( data );
  scene.WheelEventSignal().Connect( &application, functor );

  Integration::WheelEvent event( Integration::WheelEvent::CUSTOM_WHEEL, 0, 0u, Vector2( 0.0f, 0.0f ), 1, 1000u );
  application.ProcessEvent( event );

  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( static_cast< WheelEvent::Type >(event.type) == data.receivedWheelEvent.type );
  DALI_TEST_CHECK( event.direction == data.receivedWheelEvent.direction );
  DALI_TEST_CHECK( event.modifiers == data.receivedWheelEvent.modifiers );
  DALI_TEST_CHECK( event.point == data.receivedWheelEvent.point );
  DALI_TEST_CHECK( event.z == data.receivedWheelEvent.z );
  DALI_TEST_CHECK( event.timeStamp == data.receivedWheelEvent.timeStamp );

  data.Reset();

  Integration::WheelEvent event2( Integration::WheelEvent::CUSTOM_WHEEL, 0, 0u, Vector2( 0.0f, 0.0f ), -1, 1000u );
  application.ProcessEvent( event2 );

  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( static_cast< WheelEvent::Type >(event2.type) == data.receivedWheelEvent.type );
  DALI_TEST_CHECK( event2.direction == data.receivedWheelEvent.direction );
  DALI_TEST_CHECK( event2.modifiers == data.receivedWheelEvent.modifiers );
  DALI_TEST_CHECK( event2.point == data.receivedWheelEvent.point );
  DALI_TEST_CHECK( event2.z == data.receivedWheelEvent.z );
  DALI_TEST_CHECK( event2.timeStamp == data.receivedWheelEvent.timeStamp );
  END_TEST;
}

