//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <iostream>

#include <stdlib.h>
#include <tet_api.h>

#include <dali/public-api/dali-core.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/integration-api/events/touch-event-integ.h>

#include <dali-test-suite-utils.h>

using namespace Dali;
using namespace std;

static void Startup();
static void Cleanup();

extern "C"
{
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

enum
{
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

#define MAX_NUMBER_OF_TESTS 10000
extern "C" {
  struct tet_testlist tet_testlist[MAX_NUMBER_OF_TESTS];
}

// Add test functionality for all APIs in the class (Positive and Negative)
TEST_FUNCTION( UtcDaliStageDefaultConstructor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliStageDestructor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliStageGetCurrent, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliStageIsInstalled, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliStageAdd, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliStageRemove, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliStageGetSize, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliStageGetLayerCount, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliStageGetLayer, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliStageGetRootLayer, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliStageSetBackgroundColor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliStageGetBackgroundColor, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliStageGetDpi01, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliStageGetDpi02, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliStageGetDpi03, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliStageInitializeDynamics, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliStageGetDynamicsWorld, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliStageTerminateDynamics, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliStageKeepRendering, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliStageEventProcessingFinished, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliStageSignalKeyEvent, POSITIVE_TC_IDX );
TEST_FUNCTION( UtcDaliStageTouchedSignal, POSITIVE_TC_IDX );

namespace
{

/**
 * Functor for EventProcessingFinished signal
 */
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
  }

  bool functorCalled;
  TouchEvent receivedTouchEvent;
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

bool DummyTouchCallback( Actor actor, const TouchEvent& touch )
{
  return true;
}

} // unnamed namespace

// Called only once before first test is run.
static void Startup()
{
  // THERE IS NO MAINLOOP IN THE TEST APPLICATION
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliStageDefaultConstructor()
{
  TestApplication application;
  Stage stage;

  DALI_TEST_CHECK(!stage);
}

static void UtcDaliStageDestructor()
{
  TestApplication application;
  Stage* stage = new Stage();
  delete stage;
  stage = NULL;

  DALI_TEST_CHECK( true );
}

static void UtcDaliStageGetCurrent()
{
  TestApplication application;
  Stage stage = Stage::GetCurrent();

  DALI_TEST_CHECK(stage);
}

static void UtcDaliStageIsInstalled()
{
  DALI_TEST_CHECK(!Stage::IsInstalled());

  TestApplication application;

  Stage::GetCurrent();

  DALI_TEST_CHECK(Stage::IsInstalled());
}

static void UtcDaliStageAdd()
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  Actor actor = Actor::New();
  DALI_TEST_CHECK(!actor.OnStage());

  stage.Add(actor);
  DALI_TEST_CHECK(actor.OnStage());
}

static void UtcDaliStageRemove()
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  Actor actor = Actor::New();
  DALI_TEST_CHECK(!actor.OnStage());

  stage.Add(actor);
  DALI_TEST_CHECK(actor.OnStage());

  stage.Remove(actor);
  DALI_TEST_CHECK(!actor.OnStage());
}

static void UtcDaliStageGetSize()
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  Vector2 size = stage.GetSize();

  DALI_TEST_EQUALS(size.width,  static_cast<float>(TestApplication::DEFAULT_SURFACE_WIDTH),  TEST_LOCATION);
  DALI_TEST_EQUALS(size.height, static_cast<float>(TestApplication::DEFAULT_SURFACE_HEIGHT), TEST_LOCATION);
}

static void UtcDaliStageGetDpi01()
{
  TestApplication application; // Initializes core DPI to default values

  Stage stage = Stage::GetCurrent();

  // Test the default DPI.
  Vector2 dpi = stage.GetDpi();
  DALI_TEST_EQUALS(dpi.x, static_cast<float>(TestApplication::DEFAULT_HORIZONTAL_DPI), TEST_LOCATION);
  DALI_TEST_EQUALS(dpi.y, static_cast<float>(TestApplication::DEFAULT_VERTICAL_DPI),   TEST_LOCATION);
}

static void UtcDaliStageGetDpi02()
{
  TestApplication application; // Initializes core DPI to default values

  // Test that setting core DPI explicitly also sets up the Stage's DPI.
  application.GetCore().SetDpi(200, 180);

  Stage stage = Stage::GetCurrent();
  Vector2 dpi = stage.GetDpi();
  DALI_TEST_EQUALS(dpi.x, 200.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(dpi.y, 180.0f, TEST_LOCATION);
}


static void UtcDaliStageGetDpi03()
{
  TestApplication application(480, 800, 72.0f, 120.0f); // Initializes core DPI with specific values

  Stage stage = Stage::GetCurrent();

  // Test that setting core DPI explicitly also sets up the Stage's DPI.
  Vector2 dpi = stage.GetDpi();
  DALI_TEST_EQUALS(dpi.x, 72.0f, TEST_LOCATION);
  DALI_TEST_EQUALS(dpi.y, 120.0f, TEST_LOCATION);
}

static void UtcDaliStageInitializeDynamics()
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  DALI_TEST_CHECK( !stage.InitializeDynamics( DynamicsWorldConfig::New() ) );
}

static void UtcDaliStageGetDynamicsWorld()
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  DALI_TEST_CHECK( !stage.GetDynamicsWorld() );
}

static void UtcDaliStageTerminateDynamics()
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  stage.TerminateDynamics();

  DALI_TEST_CHECK( !stage.GetDynamicsWorld() );
}

static void UtcDaliStageGetLayerCount()
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  // Initially we have a default layer
  DALI_TEST_EQUALS(stage.GetLayerCount(), 1u, TEST_LOCATION);

  Layer layer = Layer::New();
  stage.Add(layer);

  DALI_TEST_EQUALS(stage.GetLayerCount(), 2u, TEST_LOCATION);
}

static void UtcDaliStageGetLayer()
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  Layer rootLayer = stage.GetLayer(0);
  DALI_TEST_CHECK(rootLayer);

  Layer layer = Layer::New();
  stage.Add(layer);

  Layer sameLayer = stage.GetLayer(1);
  DALI_TEST_CHECK(layer == sameLayer);
}


static void UtcDaliStageGetRootLayer()
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  Layer rootLayer = stage.GetLayer(0);
  DALI_TEST_CHECK( rootLayer );

  Layer layer = Layer::New();
  stage.Add( layer );
  layer.LowerToBottom();

  DALI_TEST_CHECK( stage.GetRootLayer() == rootLayer );
}

static void UtcDaliStageSetBackgroundColor()
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  Vector4 testColor(0.1f, 0.2f, 0.3f, 1.0f);
  stage.SetBackgroundColor(testColor);

  DALI_TEST_EQUALS(testColor, stage.GetBackgroundColor(), TEST_LOCATION);
}

static void UtcDaliStageGetBackgroundColor()
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  DALI_TEST_EQUALS(Stage::DEFAULT_BACKGROUND_COLOR, stage.GetBackgroundColor(), TEST_LOCATION);
}

static void UtcDaliStageKeepRendering()
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  // Run core until it wants to sleep
  bool keepUpdating( true );
  while ( keepUpdating )
  {
    application.SendNotification();
    keepUpdating = application.Render(1000.0f /*1 second*/);
  }

  // Force rendering for the next 5 seconds
  stage.KeepRendering( 5.0f );

  application.SendNotification();

  // Test that core wants to sleep after 10 seconds
  keepUpdating = application.Render(1000.0f /*1 second*/);
  DALI_TEST_CHECK( keepUpdating );
  keepUpdating = application.Render(1000.0f /*2 seconds*/);
  DALI_TEST_CHECK( keepUpdating );
  keepUpdating = application.Render(1000.0f /*3 seconds*/);
  DALI_TEST_CHECK( keepUpdating );
  keepUpdating = application.Render(1000.0f /*4 seconds*/);
  DALI_TEST_CHECK( keepUpdating );
  keepUpdating = application.Render(1000.0f /*5 seconds*/);
  DALI_TEST_CHECK( !keepUpdating );
}

static void UtcDaliStageEventProcessingFinished()
{
  TestApplication application;
  Stage stage = Stage::GetCurrent();

  bool eventProcessingFinished = false;
  EventProcessingFinishedFunctor functor( eventProcessingFinished );
  stage.EventProcessingFinishedSignal().Connect( &application, functor );

  Actor actor( Actor::New() );
  stage.Add( actor );

  application.SendNotification();
  application.Render();

  DALI_TEST_CHECK( eventProcessingFinished );

}

static void UtcDaliStageSignalKeyEvent()
{
  TestApplication application;
  Stage stage = Stage::GetCurrent();
  Dali::Integration::Core& core ( application.GetCore() );

  KeyEventSignalData data;
  KeyEventReceivedFunctor functor( data );
  stage.KeyEventSignal().Connect( &application, functor );

  Integration::KeyEvent event( "i","i", 0, 0, 0, Integration::KeyEvent::Down );
  core.SendEvent( event );

  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( event.keyModifier == data.receivedKeyEvent.keyModifier );
  DALI_TEST_CHECK( event.keyName == data.receivedKeyEvent.keyPressedName );
  DALI_TEST_CHECK( event.keyString == data.receivedKeyEvent.keyPressed );
  DALI_TEST_CHECK( event.state == static_cast<Integration::KeyEvent::State>(data.receivedKeyEvent.state) );

  data.Reset();

  Integration::KeyEvent event2( "i","i", 0, 0, 0, Integration::KeyEvent::Up );
  core.SendEvent( event2 );

  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( event2.keyModifier == data.receivedKeyEvent.keyModifier );
  DALI_TEST_CHECK( event2.keyName == data.receivedKeyEvent.keyPressedName );
  DALI_TEST_CHECK( event2.keyString == data.receivedKeyEvent.keyPressed );
  DALI_TEST_CHECK( event2.state == static_cast<Integration::KeyEvent::State>(data.receivedKeyEvent.state) );

  data.Reset();

  Integration::KeyEvent event3( "a","a", 0, 0, 0, Integration::KeyEvent::Down );
  core.SendEvent( event3 );

  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( event3.keyModifier == data.receivedKeyEvent.keyModifier );
  DALI_TEST_CHECK( event3.keyName == data.receivedKeyEvent.keyPressedName );
  DALI_TEST_CHECK( event3.keyString == data.receivedKeyEvent.keyPressed );
  DALI_TEST_CHECK( event3.state == static_cast<Integration::KeyEvent::State>(data.receivedKeyEvent.state) );

  data.Reset();

  Integration::KeyEvent event4( "a","a", 0, 0, 0, Integration::KeyEvent::Up );
  core.SendEvent( event4 );

  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( event4.keyModifier == data.receivedKeyEvent.keyModifier );
  DALI_TEST_CHECK( event4.keyName == data.receivedKeyEvent.keyPressedName );
  DALI_TEST_CHECK( event4.keyString == data.receivedKeyEvent.keyPressed );
  DALI_TEST_CHECK( event4.state == static_cast<Integration::KeyEvent::State>(data.receivedKeyEvent.state) );
}

void UtcDaliStageTouchedSignal()
{
  TestApplication application;
  Stage stage = Stage::GetCurrent();
  Dali::Integration::Core& core ( application.GetCore() );

  TouchedSignalData data;
  TouchedFunctor functor( data );
  stage.TouchedSignal().Connect( &application, functor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // NO ACTORS, SINGLE TOUCH, DOWN, MOTION THEN UP
  {
    Integration::TouchEvent touchEvent;
    touchEvent.points.push_back( TouchPoint( 0, TouchPoint::Down, 10.0f, 10.0f ) );
    core.SendEvent( touchEvent );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchEvent.GetPointCount() != 0 );
    DALI_TEST_CHECK( !data.receivedTouchEvent.points[0].hitActor );
    data.Reset();

    touchEvent.points[0].state = TouchPoint::Motion;
    touchEvent.points[0].screen.x = 12.0f; // Some motion
    core.SendEvent( touchEvent );

    DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
    data.Reset();

    touchEvent.points[0].state = TouchPoint::Up;
    core.SendEvent( touchEvent );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchEvent.GetPointCount() != 0 );
    DALI_TEST_CHECK( !data.receivedTouchEvent.points[0].hitActor );
    data.Reset();
  }

  // Add an actor to the scene

  Actor actor = Actor::New();
  actor.SetSize( 100.0f, 100.0f );
  actor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  actor.SetParentOrigin( ParentOrigin::TOP_LEFT );
  actor.TouchedSignal().Connect( &DummyTouchCallback );
  stage.Add( actor );

  // Render and notify
  application.SendNotification();
  application.Render();

  // ACTOR ON SCENE, SINGLE TOUCH, DOWN IN ACTOR, MOTION THEN UP OUTSIDE ACTOR
  {
    Integration::TouchEvent touchEvent;
    touchEvent.points.push_back( TouchPoint( 0, TouchPoint::Down, 10.0f, 10.0f ) );
    core.SendEvent( touchEvent );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchEvent.GetPointCount() != 0 );
    DALI_TEST_CHECK( data.receivedTouchEvent.points[0].hitActor == actor );
    data.Reset();

    touchEvent.points[0].state = TouchPoint::Motion;
    touchEvent.points[0].screen.x = 150.0f; // Some motion
    core.SendEvent( touchEvent );

    DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
    data.Reset();

    touchEvent.points[0].state = TouchPoint::Up;
    core.SendEvent( touchEvent );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchEvent.GetPointCount() != 0 );
    DALI_TEST_CHECK( !data.receivedTouchEvent.points[0].hitActor );
    data.Reset();
  }

  // INTERRUPTED BEFORE DOWN AND INTERRUPTED AFTER DOWN
  {
    Integration::TouchEvent touchEvent;
    touchEvent.points.push_back( TouchPoint( 0, TouchPoint::Interrupted, 10.0f, 10.0f ) );
    core.SendEvent( touchEvent );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchEvent.GetPointCount() != 0 );
    DALI_TEST_CHECK( !data.receivedTouchEvent.points[0].hitActor );
    DALI_TEST_CHECK( data.receivedTouchEvent.points[0].state == TouchPoint::Interrupted );
    data.Reset();

    touchEvent.points[0].state = TouchPoint::Down;
    core.SendEvent( touchEvent );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchEvent.GetPointCount() != 0 );
    DALI_TEST_CHECK( data.receivedTouchEvent.points[0].hitActor == actor );
    DALI_TEST_CHECK( data.receivedTouchEvent.points[0].state == TouchPoint::Down );
    data.Reset();

    touchEvent.points[0].state = TouchPoint::Interrupted;
    core.SendEvent( touchEvent );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchEvent.GetPointCount() != 0 );
    DALI_TEST_CHECK( !data.receivedTouchEvent.points[0].hitActor );
    DALI_TEST_CHECK( data.receivedTouchEvent.points[0].state == TouchPoint::Interrupted );
    data.Reset();
  }

  // MULTIPLE TOUCH, SHOULD ONLY RECEIVE TOUCH ON FIRST DOWN AND LAST UP
  {
    Integration::TouchEvent touchEvent;

    // 1st point
    touchEvent.points.push_back( TouchPoint( 0, TouchPoint::Down, 10.0f, 10.0f ) );
    core.SendEvent( touchEvent );
    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    data.Reset();

    // 2nd point
    touchEvent.points[0].state = TouchPoint::Stationary;
    touchEvent.points.push_back( TouchPoint( 1, TouchPoint::Down, 50.0f, 50.0f ) );
    core.SendEvent( touchEvent );
    DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
    data.Reset();

    // Primary point is up
    touchEvent.points[0].state = TouchPoint::Up;
    touchEvent.points[1].state = TouchPoint::Stationary;
    core.SendEvent( touchEvent );
    DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
    data.Reset();

    // Remove 1st point now, 2nd point is now in motion
    touchEvent.points.erase( touchEvent.points.begin() );
    touchEvent.points[0].state = TouchPoint::Motion;
    touchEvent.points[0].screen.x = 150.0f;
    core.SendEvent( touchEvent );
    DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
    data.Reset();

    // Final point Up
    touchEvent.points[0].state = TouchPoint::Up;
    core.SendEvent( touchEvent );
    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    data.Reset();
  }
}
