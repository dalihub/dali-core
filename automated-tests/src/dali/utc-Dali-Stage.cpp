/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/dali-core.h>
#include <dali/devel-api/common/stage-devel.h>
#include <dali/integration-api/context-notifier.h>
#include <dali/integration-api/events/key-event-integ.h>
#include <dali/public-api/events/key-event.h>
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/integration-api/events/wheel-event-integ.h>

#include <dali-test-suite-utils.h>

using namespace Dali;

void stage_test_startup(void)
{
  test_return_value = TET_UNDEF;
}

void stage_test_cleanup(void)
{
  test_return_value = TET_PASS;
}

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

// Stores data that is populated in the KeyEventGeneratedSignal callback and will be read by the TET cases
struct KeyEventGeneratedSignalData
{
  KeyEventGeneratedSignalData()
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
struct KeyEventGeneratedReceivedFunctor
{
  KeyEventGeneratedReceivedFunctor( KeyEventGeneratedSignalData& data )
  : signalData( data )
  {}

  bool operator()( const KeyEvent& keyEvent )
  {
    signalData.functorCalled = true;
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

bool DummyTouchCallback( Actor actor, const TouchEvent& touch )
{
  return true;
}

struct ContextStatusFunctor
{
  ContextStatusFunctor(bool& calledFlag) : mCalledFlag( calledFlag )
  {
    mCalledFlag = false;
  }

  void operator()()
  {
    mCalledFlag = true;
  }
  void Reset()
  {
    mCalledFlag = false;
  }

  bool& mCalledFlag;
};

struct SceneCreatedStatusFunctor
{
  SceneCreatedStatusFunctor(bool& calledFlag) : mCalledFlag( calledFlag )
  {
    mCalledFlag = false;
  }

  void operator()()
  {
    mCalledFlag = true;
  }
  void Reset()
  {
    mCalledFlag = false;
  }

  bool& mCalledFlag;
};

struct ActorCreatedFunctor
{
  ActorCreatedFunctor( bool& signalReceived )
  : mSignalVerified( signalReceived )
  {
  }

  void operator()( BaseHandle object )
  {
    tet_infoline( "Verifying TestActorCallback()" );
    Actor actor = Actor::DownCast( object );
    if( actor )
    {
      mSignalVerified = true;
    }
  }

  bool& mSignalVerified;
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

} // unnamed namespace


int UtcDaliStageDefaultConstructorP(void)
{
  TestApplication application;
  Stage stage;

  DALI_TEST_CHECK( !stage );
  END_TEST;
}

// Note: No negative test for default constructor.

int UtcDaliStageDestructorP(void)
{
  TestApplication application;
  Stage* stage = new Stage();
  delete stage;
  stage = NULL;

  DALI_TEST_CHECK( true );
  END_TEST;
}

// Note: No negative test for default destructor.

int UtcDaliStageGetCurrentP(void)
{
  TestApplication application;
  Stage stage = Stage::GetCurrent();

  DALI_TEST_CHECK( stage );
  END_TEST;
}

int UtcDaliStageGetCurrentN(void)
{
  bool asserted = false;
  try
  {
    Stage stage = Stage::GetCurrent();
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "stage && \"Stage doesn't exist\"", TEST_LOCATION );
    asserted = true;
  }

  DALI_TEST_CHECK( asserted );
  END_TEST;
}

int UtcDaliStageIsInstalledP(void)
{
  TestApplication application;

  Stage::GetCurrent();

  DALI_TEST_CHECK( Stage::IsInstalled() );
  END_TEST;
}

int UtcDaliStageIsInstalledN(void)
{
  DALI_TEST_CHECK( !Stage::IsInstalled() );

  END_TEST;
}

int UtcDaliStageCopyConstructorP(void)
{
  TestApplication application;
  Stage stage = Stage::GetCurrent();

  Stage copyStage( stage );

  DALI_TEST_CHECK( copyStage );
  DALI_TEST_CHECK( copyStage.GetRootLayer() == stage.GetRootLayer() );

  END_TEST;
}

// Note: no negative test for UtcDaliStageCopyConstructor.

int UtcDaliStageAssignmentOperatorP(void)
{
  TestApplication application;
  const Stage stage = Stage::GetCurrent();

  Stage copyStage = stage;

  DALI_TEST_CHECK( copyStage );
  DALI_TEST_CHECK( copyStage.GetRootLayer() == stage.GetRootLayer() );

  END_TEST;
}

// Note: No negative test for UtcDaliStageAssignmentOperator.

int UtcDaliStageAddP(void)
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  Actor actor = Actor::New();
  DALI_TEST_CHECK( !actor.OnStage() );

  stage.Add( actor );
  DALI_TEST_CHECK( actor.OnStage() );
  END_TEST;
}

int UtcDaliStageAddN(void)
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();
  Actor actor;

  bool asserted = false;
  try
  {
    stage.Add( actor );
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "actor && \"Actor handle is empty\"", TEST_LOCATION );
    asserted = true;
  }

  DALI_TEST_CHECK( asserted );

  END_TEST;
}

int UtcDaliStageRemoveP(void)
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  Actor actor = Actor::New();
  DALI_TEST_CHECK( !actor.OnStage() );

  stage.Add( actor );
  DALI_TEST_CHECK( actor.OnStage() );

  stage.Remove(actor);
  DALI_TEST_CHECK( !actor.OnStage() );
  END_TEST;
}

int UtcDaliStageRemoveN1(void)
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();
  Actor actor;

  bool asserted = false;
  try
  {
    // Actor is not valid, confirm a removal attempt does assert.
    stage.Remove( actor );
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "actor && \"Actor handle is empty\"", TEST_LOCATION );
    asserted = true;
  }

  DALI_TEST_CHECK( asserted );
  END_TEST;
}

int UtcDaliStageRemoveN2(void)
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();
  Actor actor = Actor::New();
  DALI_TEST_CHECK( !actor.OnStage() );

  bool asserted = false;
  try
  {
    // Actor is not on stage, confirm a removal attempt does not assert / segfault.
    stage.Remove( actor );
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    asserted = true;
  }

  DALI_TEST_CHECK( !asserted );
  END_TEST;
}

int UtcDaliStageRemoveN3(void)
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  // Initially we have a default layer
  DALI_TEST_EQUALS( stage.GetLayerCount(), 1u, TEST_LOCATION );

  // Check we cannot remove the root layer from the stage.
  Layer layer = stage.GetRootLayer();
  bool asserted = true;
  try
  {
    stage.Remove( layer );
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "this != &child && \"Cannot remove actor from itself\"", TEST_LOCATION );
    asserted = true;
  }

  DALI_TEST_CHECK( asserted );
  DALI_TEST_EQUALS( stage.GetLayerCount(), 1u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliStageGetSizeP(void)
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  Vector2 size = stage.GetSize();

  DALI_TEST_EQUALS( size.width,  static_cast<float>( TestApplication::DEFAULT_SURFACE_WIDTH ),  TEST_LOCATION );
  DALI_TEST_EQUALS( size.height, static_cast<float>( TestApplication::DEFAULT_SURFACE_HEIGHT ), TEST_LOCATION );
  END_TEST;
}

int UtcDaliStageGetSizeN(void)
{
  TestApplication application;

  Stage stage;

  bool asserted = false;
  Vector2 size;
  try
  {
    size = stage.GetSize();
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "stage && \"Stage handle is empty\"", TEST_LOCATION );
    asserted = true;
  }

  DALI_TEST_CHECK( asserted );
  END_TEST;
}

int UtcDaliStageGetDpiP1(void)
{
  TestApplication application; // Initializes core DPI to default values

  Stage stage = Stage::GetCurrent();

  // Test the default DPI.
  Vector2 dpi = stage.GetDpi();
  DALI_TEST_EQUALS( dpi.x, static_cast<float>( TestApplication::DEFAULT_HORIZONTAL_DPI ), TEST_LOCATION );
  DALI_TEST_EQUALS( dpi.y, static_cast<float>( TestApplication::DEFAULT_VERTICAL_DPI ),   TEST_LOCATION );
  END_TEST;
}

int UtcDaliStageGetDpiP2(void)
{
  TestApplication application; // Initializes core DPI to default values

  // Test that setting core DPI explicitly also sets up the Stage's DPI.
  application.GetCore().SetDpi( 200, 180 );

  Stage stage = Stage::GetCurrent();
  Vector2 dpi = stage.GetDpi();
  DALI_TEST_EQUALS( dpi.x, 200.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( dpi.y, 180.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliStageGetDpiP3(void)
{
  TestApplication application( 480, 800, 72.0f, 120.0f ); // Initializes core DPI with specific values

  Stage stage = Stage::GetCurrent();

  // Test that setting core DPI explicitly also sets up the Stage's DPI.
  Vector2 dpi = stage.GetDpi();
  DALI_TEST_EQUALS( dpi.x, 72.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( dpi.y, 120.0f, TEST_LOCATION) ;
  END_TEST;
}

/*
 * This is not a true negative test, we are checking the DPI if it has not been set.
 * A test for setting negative DPI values would be part of the application core utc tests.
 */
int UtcDaliStageGetDpiN(void)
{
  TestApplication application; // Initializes core DPI to default values

  Stage stage = Stage::GetCurrent();
  Vector2 dpi = stage.GetDpi();

  DALI_TEST_EQUALS( dpi.x, 220.0f, TEST_LOCATION );
  DALI_TEST_EQUALS( dpi.y, 217.0f, TEST_LOCATION );
  END_TEST;
}

int UtcDaliStageGetLayerCountP(void)
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  // Initially we have a default layer
  DALI_TEST_EQUALS( stage.GetLayerCount(), 1u, TEST_LOCATION );

  Layer layer = Layer::New();
  stage.Add( layer );

  DALI_TEST_EQUALS( stage.GetLayerCount(), 2u, TEST_LOCATION );
  END_TEST;
}

/*
 * Not a true negative test, but confirms layer count is not affected by an invalid removal.
 */
int UtcDaliStageGetLayerCountN(void)
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  // Initially we have a default layer
  DALI_TEST_EQUALS( stage.GetLayerCount(), 1u, TEST_LOCATION );

  Layer layer = Layer::New();
  stage.Remove( layer );

  // Still have 1 layer.
  DALI_TEST_EQUALS( stage.GetLayerCount(), 1u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliStageGetLayerP(void)
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  Layer rootLayer = stage.GetLayer( 0 );
  DALI_TEST_CHECK( rootLayer );

  Layer layer = Layer::New();
  stage.Add( layer );

  Layer sameLayer = stage.GetLayer( 1 );
  DALI_TEST_CHECK( layer == sameLayer );
  END_TEST;
}

int UtcDaliStageGetLayerN(void)
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  bool asserted = false;
  try
  {
    // Try to get a layer that doesn't exist (note: 0 is the root layer).
    Layer layer = stage.GetLayer( 1 );
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "depth < mLayers.size()", TEST_LOCATION );
    asserted = true;
  }

  DALI_TEST_CHECK( asserted );
  END_TEST;
}

int UtcDaliStageGetRootLayerP(void)
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  Layer layer = stage.GetLayer( 0 );
  DALI_TEST_CHECK( layer );

  // Check that GetRootLayer() correctly retreived layer 0.
  DALI_TEST_CHECK( stage.GetRootLayer() == layer );

  END_TEST;
}

int UtcDaliStageGetRootLayerN(void)
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  Layer rootLayer = stage.GetLayer( 0 );
  DALI_TEST_CHECK( rootLayer );
  DALI_TEST_CHECK( stage.GetRootLayer() == rootLayer );

  // Create a new layer and attempt to lower it below the root layer.
  Layer layer = Layer::New();
  stage.Add( layer );
  layer.LowerToBottom();

  // Check that GetRootLayer still retrieves the same original layer.
  DALI_TEST_CHECK( stage.GetRootLayer() == rootLayer );

  // Check modifying the root layer is also blocked.
  rootLayer.RaiseToTop();
  DALI_TEST_CHECK( stage.GetRootLayer() == rootLayer );

  END_TEST;
}

int UtcDaliStageSetBackgroundColorP(void)
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  Vector4 testColor( 0.1f, 0.2f, 0.3f, 1.0f );
  stage.SetBackgroundColor( testColor );

  DALI_TEST_EQUALS( testColor, stage.GetBackgroundColor(), TEST_LOCATION );
  END_TEST;
}

// Note: No negative test for UtcDaliStageSetBackgroundColor as we do not wish to implement
// range checking for colors due to speed. Colors are clamped with glclampf within GL anyway.

int UtcDaliStageGetBackgroundColorP(void)
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  DALI_TEST_EQUALS( Stage::DEFAULT_BACKGROUND_COLOR, stage.GetBackgroundColor(), TEST_LOCATION );
  END_TEST;
}

// Note: No negative test for UtcDaliStageGetBackgroundColor as this is covered by UtcDaliStageSetBackgroundColorN.

int UtcDaliStageKeepRenderingP(void)
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  // Run core until it wants to sleep
  bool keepUpdating( true );
  while ( keepUpdating )
  {
    application.SendNotification();
    keepUpdating = application.Render( 1000.0f /*1 second*/ );
  }

  // Force rendering for the next 5 seconds
  stage.KeepRendering( 5.0f );

  application.SendNotification();

  // Test that core wants to sleep after 10 seconds
  keepUpdating = application.Render( 1000.0f /*1 second*/ );
  DALI_TEST_CHECK( keepUpdating );
  keepUpdating = application.Render( 1000.0f /*2 seconds*/ );
  DALI_TEST_CHECK( keepUpdating );
  keepUpdating = application.Render( 1000.0f /*3 seconds*/ );
  DALI_TEST_CHECK( keepUpdating );
  keepUpdating = application.Render( 1000.0f /*4 seconds*/ );
  DALI_TEST_CHECK( keepUpdating );
  keepUpdating = application.Render( 1000.0f /*5 seconds*/ );
  DALI_TEST_CHECK( !keepUpdating );
  END_TEST;
}

int UtcDaliStageKeepRenderingN(void)
{
  TestApplication application;

  Stage stage = Stage::GetCurrent();

  // Run core until it wants to sleep
  bool keepUpdating( true );
  while ( keepUpdating )
  {
    application.SendNotification();
    keepUpdating = application.Render( 1000.0f /*1 second*/ );
  }

  // Force rendering for the next 5 seconds
  stage.KeepRendering( -1.0f );

  application.SendNotification();

  // Test that core wants to sleep after 10 seconds
  keepUpdating = application.Render( 1000.0f /*1 second*/ );
  DALI_TEST_CHECK( !keepUpdating );

  END_TEST;
}

int UtcDaliStageEventProcessingFinishedP(void)
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

  END_TEST;
}

int UtcDaliStageEventProcessingFinishedN(void)
{
  TestApplication application;
  Stage stage = Stage::GetCurrent();

  bool eventProcessingFinished = false;
  EventProcessingFinishedFunctor functor( eventProcessingFinished );
  stage.EventProcessingFinishedSignal().Connect( &application, functor );

  Actor actor( Actor::New() );
  stage.Add( actor );

  // Do not complete event processing and confirm the signal has not been emitted.
  DALI_TEST_CHECK( !eventProcessingFinished );

  END_TEST;
}

int UtcDaliStageKeyEventGeneratedSignalP(void)
{
  TestApplication application;
  Stage stage = Stage::GetCurrent();

  KeyEventGeneratedSignalData data;
  KeyEventGeneratedReceivedFunctor functor( data );
  DevelStage::KeyEventGeneratedSignal( stage ).Connect( &application, functor );

  KeyEventGeneratedSignalData data2;
  KeyEventGeneratedReceivedFunctor functor2( data2 );
  GetImplementation( stage ).ConnectSignal( &application, "keyEventGenerated", functor2 );

  Integration::KeyEvent event( "a", "a", 0, 0, 0, Integration::KeyEvent::Up, DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE );
  application.ProcessEvent( event );

  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( event.keyModifier == data.receivedKeyEvent.keyModifier );
  DALI_TEST_CHECK( event.keyName == data.receivedKeyEvent.keyPressedName );
  DALI_TEST_CHECK( event.keyString == data.receivedKeyEvent.keyPressed );
  DALI_TEST_CHECK( event.state == static_cast<Integration::KeyEvent::State>( data.receivedKeyEvent.state ) );

  data.Reset();

  Integration::KeyEvent event2( "i", "i", 0, 0, 0, Integration::KeyEvent::Up, DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE );
  application.ProcessEvent( event2 );

  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( event2.keyModifier == data.receivedKeyEvent.keyModifier );
  DALI_TEST_CHECK( event2.keyName == data.receivedKeyEvent.keyPressedName );
  DALI_TEST_CHECK( event2.keyString == data.receivedKeyEvent.keyPressed );
  DALI_TEST_CHECK( event2.state == static_cast<Integration::KeyEvent::State>( data.receivedKeyEvent.state ) );

  data.Reset();

  Integration::KeyEvent event3( "a", "a", 0, 0, 0, Integration::KeyEvent::Down, DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE );
  application.ProcessEvent( event3 );

  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( event3.keyModifier == data.receivedKeyEvent.keyModifier );
  DALI_TEST_CHECK( event3.keyName == data.receivedKeyEvent.keyPressedName );
  DALI_TEST_CHECK( event3.keyString == data.receivedKeyEvent.keyPressed );
  DALI_TEST_CHECK( event3.state == static_cast<Integration::KeyEvent::State>( data.receivedKeyEvent.state ) );

  data.Reset();

  Integration::KeyEvent event4( "a", "a", 0, 0, 0, Integration::KeyEvent::Up, DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE );
  application.ProcessEvent( event4 );

  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( event4.keyModifier == data.receivedKeyEvent.keyModifier );
  DALI_TEST_CHECK( event4.keyName == data.receivedKeyEvent.keyPressedName );
  DALI_TEST_CHECK( event4.keyString == data.receivedKeyEvent.keyPressed );
  DALI_TEST_CHECK( event4.state == static_cast<Integration::KeyEvent::State>( data.receivedKeyEvent.state ) );
  END_TEST;
}

int UtcDaliStageSignalKeyEventP(void)
{
  TestApplication application;
  Stage stage = Stage::GetCurrent();

  KeyEventSignalData data;
  KeyEventReceivedFunctor functor( data );
  stage.KeyEventSignal().Connect( &application, functor );

  Integration::KeyEvent event( "i", "i", 0, 0, 0, Integration::KeyEvent::Down, DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE );
  application.ProcessEvent( event );

  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( event.keyModifier == data.receivedKeyEvent.keyModifier );
  DALI_TEST_CHECK( event.keyName == data.receivedKeyEvent.keyPressedName );
  DALI_TEST_CHECK( event.keyString == data.receivedKeyEvent.keyPressed );
  DALI_TEST_CHECK( event.state == static_cast<Integration::KeyEvent::State>( data.receivedKeyEvent.state ) );

  data.Reset();

  Integration::KeyEvent event2( "i", "i", 0, 0, 0, Integration::KeyEvent::Up, DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE );
  application.ProcessEvent( event2 );

  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( event2.keyModifier == data.receivedKeyEvent.keyModifier );
  DALI_TEST_CHECK( event2.keyName == data.receivedKeyEvent.keyPressedName );
  DALI_TEST_CHECK( event2.keyString == data.receivedKeyEvent.keyPressed );
  DALI_TEST_CHECK( event2.state == static_cast<Integration::KeyEvent::State>( data.receivedKeyEvent.state ) );

  data.Reset();

  Integration::KeyEvent event3( "a", "a", 0, 0, 0, Integration::KeyEvent::Down, DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE );
  application.ProcessEvent( event3 );

  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( event3.keyModifier == data.receivedKeyEvent.keyModifier );
  DALI_TEST_CHECK( event3.keyName == data.receivedKeyEvent.keyPressedName );
  DALI_TEST_CHECK( event3.keyString == data.receivedKeyEvent.keyPressed );
  DALI_TEST_CHECK( event3.state == static_cast<Integration::KeyEvent::State>( data.receivedKeyEvent.state ) );

  data.Reset();

  Integration::KeyEvent event4( "a", "a", 0, 0, 0, Integration::KeyEvent::Up, DEFAULT_DEVICE_NAME, Device::Class::NONE, Device::Subclass::NONE );
  application.ProcessEvent( event4 );

  DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
  DALI_TEST_CHECK( event4.keyModifier == data.receivedKeyEvent.keyModifier );
  DALI_TEST_CHECK( event4.keyName == data.receivedKeyEvent.keyPressedName );
  DALI_TEST_CHECK( event4.keyString == data.receivedKeyEvent.keyPressed );
  DALI_TEST_CHECK( event4.state == static_cast<Integration::KeyEvent::State>( data.receivedKeyEvent.state ) );
  END_TEST;
}

int UtcDaliStageSignalKeyEventN(void)
{
  TestApplication application;
  Stage stage = Stage::GetCurrent();

  KeyEventSignalData data;
  KeyEventReceivedFunctor functor( data );
  stage.KeyEventSignal().Connect( &application, functor );

  // Check that a non-pressed key events data is not modified.
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );

  END_TEST;
}

int UtcDaliStageTouchedSignalP(void)
{
  TestApplication application;
  Stage stage = Stage::GetCurrent();

  TouchedSignalData data;
  TouchedFunctor functor( data );
  stage.TouchedSignal().Connect( &application, functor );

  // Render and notify.
  application.SendNotification();
  application.Render();

  // Basic test: No actors, single touch (down then up).
  {
    GenerateTouch( application, PointState::DOWN, Vector2( 10.0f, 10.0f ) );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchEvent.GetPointCount() != 0u );
    DALI_TEST_CHECK( !data.receivedTouchEvent.points[0].hitActor );
    data.Reset();

    GenerateTouch( application, PointState::UP, Vector2( 10.0f, 10.0f ) );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchEvent.GetPointCount() != 0u );
    DALI_TEST_CHECK( !data.receivedTouchEvent.points[0].hitActor );
    data.Reset();
  }

  // Add an actor to the scene.
  Actor actor = Actor::New();
  actor.SetSize( 100.0f, 100.0f );
  actor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  actor.SetParentOrigin( ParentOrigin::TOP_LEFT );
  actor.TouchedSignal().Connect( &DummyTouchCallback );
  stage.Add( actor );

  // Render and notify.
  application.SendNotification();
  application.Render();

  // Actor on scene, single touch, down in actor, motion, then up outside actor.
  {
    GenerateTouch( application, PointState::DOWN, Vector2( 10.0f, 10.0f ) );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchEvent.GetPointCount() != 0u );
    DALI_TEST_CHECK( data.receivedTouchEvent.points[0].hitActor == actor );
    data.Reset();

    GenerateTouch( application, PointState::MOTION, Vector2( 150.0f, 10.0f ) ); // Some motion

    DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
    data.Reset();

    GenerateTouch( application, PointState::UP, Vector2( 150.0f, 10.0f ) ); // Some motion

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchEvent.GetPointCount() != 0u );
    DALI_TEST_CHECK( !data.receivedTouchEvent.points[0].hitActor );
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
    DALI_TEST_EQUALS( data.receivedTouchEvent.GetPointCount(), 1u, TEST_LOCATION );
    data.Reset();

    // 2nd point
    touchEvent.points[0].SetState( PointState::STATIONARY );
    point.SetDeviceId( 1 );
    point.SetScreenPosition( Vector2( 50.0f, 50.0f ) );
    touchEvent.points.push_back( point );
    application.ProcessEvent( touchEvent );
    DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
    DALI_TEST_EQUALS( data.receivedTouchEvent.GetPointCount(), 0u, TEST_LOCATION );
    data.Reset();

    // Primary point is up
    touchEvent.points[0].SetState( PointState::UP );
    touchEvent.points[1].SetState( PointState::STATIONARY );
    application.ProcessEvent( touchEvent );
    DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
    DALI_TEST_EQUALS( data.receivedTouchEvent.GetPointCount(), 0u, TEST_LOCATION );
    data.Reset();

    // Remove 1st point now, 2nd point is now in motion
    touchEvent.points.erase( touchEvent.points.begin() );
    touchEvent.points[0].SetState( PointState::MOTION );
    touchEvent.points[0].SetScreenPosition( Vector2( 150.0f, 50.0f ) );
    application.ProcessEvent( touchEvent );
    DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
    DALI_TEST_EQUALS( data.receivedTouchEvent.GetPointCount(), 0u, TEST_LOCATION );
    data.Reset();

    // Final point Up
    touchEvent.points[0].SetState( PointState::UP );
    application.ProcessEvent( touchEvent );
    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_EQUALS( data.receivedTouchEvent.GetPointCount(), 1u, TEST_LOCATION );
    data.Reset();
  }
  END_TEST;
}

int UtcDaliStageTouchedSignalN(void)
{
  TestApplication application;
  Stage stage = Stage::GetCurrent();

  TouchedSignalData data;
  TouchedFunctor functor( data );
  stage.TouchedSignal().Connect( &application, functor );

  // Render and notify.
  application.SendNotification();
  application.Render();

  // Confirm functor not called before there has been any touch event.
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );

  // No actors, single touch, down, motion then up.
  {
    GenerateTouch( application, PointState::DOWN, Vector2( 10.0f, 10.0f ) );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchEvent.GetPointCount() != 0u );
    DALI_TEST_CHECK( !data.receivedTouchEvent.points[0].hitActor );
    data.Reset();

    // Confirm there is no signal when the touchpoint is only moved.
    GenerateTouch( application, PointState::MOTION, Vector2( 1200.0f, 10.0f ) ); // Some motion

    DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
    data.Reset();

    // Confirm a following up event generates a signal.
    GenerateTouch( application, PointState::UP, Vector2( 1200.0f, 10.0f ) );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchEvent.GetPointCount() != 0u );
    DALI_TEST_CHECK( !data.receivedTouchEvent.points[0].hitActor );
    data.Reset();
  }

  // Add an actor to the scene.
  Actor actor = Actor::New();
  actor.SetSize( 100.0f, 100.0f );
  actor.SetAnchorPoint( AnchorPoint::TOP_LEFT );
  actor.SetParentOrigin( ParentOrigin::TOP_LEFT );
  actor.TouchedSignal().Connect( &DummyTouchCallback );
  stage.Add( actor );

  // Render and notify.
  application.SendNotification();
  application.Render();

  // Actor on scene. Interrupted before down and interrupted after down.
  {
    GenerateTouch( application, PointState::INTERRUPTED, Vector2( 10.0f, 10.0f ) );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchEvent.GetPointCount() != 0u );
    DALI_TEST_CHECK( !data.receivedTouchEvent.points[0].hitActor );
    DALI_TEST_CHECK( data.receivedTouchEvent.points[0].state == TouchPoint::Interrupted );
    data.Reset();

    GenerateTouch( application, PointState::DOWN, Vector2( 10.0f, 10.0f ) );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchEvent.GetPointCount() != 0u );
    DALI_TEST_CHECK( data.receivedTouchEvent.points[0].hitActor == actor );
    DALI_TEST_CHECK( data.receivedTouchEvent.points[0].state == TouchPoint::Down );
    data.Reset();

    GenerateTouch( application, PointState::INTERRUPTED, Vector2( 10.0f, 10.0f ) );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchEvent.GetPointCount() != 0u );
    DALI_TEST_CHECK( !data.receivedTouchEvent.points[0].hitActor );
    DALI_TEST_CHECK( data.receivedTouchEvent.points[0].state == TouchPoint::Interrupted );

    DALI_TEST_EQUALS( data.receivedTouchEvent.GetPointCount(), 1u, TEST_LOCATION );

    // Check that getting info about a non-existent point causes an assert.
    bool asserted = false;
    try
    {
      data.receivedTouchEvent.GetPoint( 1 );
    }
    catch( Dali::DaliException& e )
    {
      DALI_TEST_PRINT_ASSERT( e );
      DALI_TEST_ASSERT( e, "point < points.size() && \"No point at index\"", TEST_LOCATION );
      asserted = true;
    }
    DALI_TEST_CHECK( asserted );

    data.Reset();
  }

  END_TEST;
}


int UtcDaliStageTouchSignalP(void)
{
  TestApplication application;
  Stage stage = Stage::GetCurrent();

  TouchedSignalData data;
  TouchFunctor functor( data );
  stage.TouchSignal().Connect( &application, functor );

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
  stage.Add( actor );

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

int UtcDaliStageTouchSignalN(void)
{
  TestApplication application;
  Stage stage = Stage::GetCurrent();

  TouchedSignalData data;
  TouchFunctor functor( data );
  stage.TouchSignal().Connect( &application, functor );

  TouchedSignalData data2;
  TouchFunctor functor2( data2 );
  GetImplementation( stage ).ConnectSignal( &application, "touch", functor2 );

  // Render and notify.
  application.SendNotification();
  application.Render();

  // Confirm functor not called before there has been any touch event.
  DALI_TEST_EQUALS( false, data.functorCalled, TEST_LOCATION );
  DALI_TEST_EQUALS( false, data2.functorCalled, TEST_LOCATION );

  // No actors, single touch, down, motion then up.
  {
    GenerateTouch( application, PointState::DOWN, Vector2( 10.0f, 10.0f ) );

    DALI_TEST_EQUALS( true, data.functorCalled, TEST_LOCATION );
    DALI_TEST_CHECK( data.receivedTouchData.GetPointCount() != 0u );
    DALI_TEST_CHECK( !data.receivedTouchData.GetHitActor(0));

    DALI_TEST_EQUALS( true, data2.functorCalled, TEST_LOCATION );

    data.Reset();
    data2.Reset();

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
  stage.Add( actor );

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

int UtcDaliStageSignalWheelEventP(void)
{
  TestApplication application;
  Stage stage = Stage::GetCurrent();

  WheelEventSignalData data;
  WheelEventReceivedFunctor functor( data );
  stage.WheelEventSignal().Connect( &application, functor );

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

int UtcDaliStageContextLostSignalP(void)
{
  TestApplication app;
  Stage stage = Stage::GetCurrent();

  bool contextLost = false;
  ContextStatusFunctor contextLostFunctor( contextLost );
  stage.ContextLostSignal().Connect( &app, contextLostFunctor );

  Integration::ContextNotifierInterface* notifier = app.GetCore().GetContextNotifier();
  notifier->NotifyContextLost();
  DALI_TEST_EQUALS( contextLost, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliStageContextLostSignalN(void)
{
  TestApplication app;
  Stage stage;

  // Check that connecting to the signal with a bad stage instance causes an assert.
  bool asserted = false;
  bool contextLost = false;
  ContextStatusFunctor contextLostFunctor( contextLost );
  try
  {
    stage.ContextLostSignal().Connect( &app, contextLostFunctor );
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "stage && \"Stage handle is empty\"", TEST_LOCATION );
    asserted = true;
  }
  DALI_TEST_CHECK( asserted );

  END_TEST;
}

int UtcDaliStageContextRegainedSignalP(void)
{
  TestApplication app;
  Stage stage = Stage::GetCurrent();

  bool contextRegained = false;
  ContextStatusFunctor contextRegainedFunctor( contextRegained );
  stage.ContextRegainedSignal().Connect( &app, contextRegainedFunctor );

  Integration::ContextNotifierInterface* notifier = app.GetCore().GetContextNotifier();
  notifier->NotifyContextLost();
  notifier->NotifyContextRegained();
  DALI_TEST_EQUALS( contextRegained, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliStageContextRegainedSignalN(void)
{
  TestApplication app;
  Stage stage;

  // Check that connecting to the signal with a bad stage instance causes an assert.
  bool asserted = false;
  bool contextRegained = false;
  ContextStatusFunctor contextRegainedFunctor( contextRegained );
  try
  {
    stage.ContextRegainedSignal().Connect( &app, contextRegainedFunctor );
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "stage && \"Stage handle is empty\"", TEST_LOCATION );
    asserted = true;
  }
  DALI_TEST_CHECK( asserted );

  END_TEST;
}

int UtcDaliStageSceneCreatedSignalP(void)
{
  TestApplication app;
  Stage stage = Stage::GetCurrent();

  bool signalCalled = false;
  SceneCreatedStatusFunctor sceneCreatedFunctor( signalCalled );
  stage.SceneCreatedSignal().Connect( &app, sceneCreatedFunctor );

  Integration::Core& core = app.GetCore();
  core.SceneCreated();
  DALI_TEST_EQUALS( signalCalled, true, TEST_LOCATION );

  END_TEST;
}

int UtcDaliStageSceneCreatedSignalN(void)
{
  TestApplication app;
  Stage stage;

  // Check that connecting to the signal with a bad stage instance causes an assert.
  bool asserted = false;
  bool signalCalled = false;
  SceneCreatedStatusFunctor sceneCreatedFunctor( signalCalled );
  try
  {
    stage.SceneCreatedSignal().Connect( &app, sceneCreatedFunctor );
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "stage && \"Stage handle is empty\"", TEST_LOCATION );
    asserted = true;
  }
  DALI_TEST_CHECK( asserted );

  END_TEST;
}

int UtcDaliStageGetRenderTaskListP(void)
{
  TestApplication app;
  Stage stage = Stage::GetCurrent();

  // Check we get a valid instance.
  const RenderTaskList& tasks = stage.GetRenderTaskList();

  // There should be 1 task by default.
  DALI_TEST_EQUALS( tasks.GetTaskCount(), 1u, TEST_LOCATION );

  // RenderTaskList has it's own UTC tests.
  // But we can confirm that GetRenderTaskList in Stage retrieves the same RenderTaskList each time.
  RenderTask newTask = stage.GetRenderTaskList().CreateTask();

  DALI_TEST_EQUALS( stage.GetRenderTaskList().GetTask( 1 ), newTask, TEST_LOCATION );

  END_TEST;
}

int UtcDaliStageGetRenderTaskListN(void)
{
  TestApplication app;
  Stage stage;

  // Check that getting the render task list with a bad stage instance causes an assert.
  bool asserted = false;
  try
  {
    stage.GetRenderTaskList();
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    DALI_TEST_ASSERT( e, "stage && \"Stage handle is empty\"", TEST_LOCATION );
    asserted = true;
  }
  DALI_TEST_CHECK( asserted );

  END_TEST;
}

int UtcDaliStageGetObjectRegistryP(void)
{
  TestApplication app;
  Stage stage = Stage::GetCurrent();

  ObjectRegistry objectRegistry = stage.GetObjectRegistry();

  // Object registry tests are covered in their own module.
  // However we want a basic test to confirm the returned registry is valid and works.
  bool verified = false;
  ActorCreatedFunctor test( verified );
  objectRegistry.ObjectCreatedSignal().Connect( &app, test );

  Actor actor = Actor::New();
  DALI_TEST_CHECK( test.mSignalVerified );

  END_TEST;
}

int UtcDaliStageGetObjectRegistryN(void)
{
  TestApplication app;
  Stage stage;

  // Check that getting the object registry with a bad stage instance DOES NOT cause an assert.
  // This is because GetCurrent() is used, always creating a stage if one does not exist.
  bool asserted = false;
  try
  {
    stage.GetObjectRegistry();
  }
  catch( Dali::DaliException& e )
  {
    DALI_TEST_PRINT_ASSERT( e );
    asserted = true;
  }
  DALI_TEST_CHECK( !asserted );

  END_TEST;
}

int UtcDaliStageOperatorAssign(void)
{
  TestApplication app;
  Stage stage;
  DALI_TEST_CHECK( !stage );

  stage = Stage::GetCurrent();
  DALI_TEST_CHECK( stage );

  END_TEST;
}
