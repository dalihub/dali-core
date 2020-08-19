/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/internal/event/common/stage-impl.h>

// EXTERNAL INCLUDES
#include <algorithm>
#include <cmath>
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/actors/layer-list.h>
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/common/property-notification-manager.h>
#include <dali/internal/event/render-tasks/render-task-list-impl.h>
#include <dali/internal/event/update/frame-callback-interface-impl.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/manager/scene-graph-frame-callback.h>
#include <dali/internal/event/common/object-registry-impl.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/events/touch-event.h>
#include <dali/public-api/events/touch-data.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/public-api/rendering/frame-buffer.h>

using Dali::Internal::SceneGraph::Node;

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_DEPTH_TIMER" );
#endif
}

namespace Dali
{

namespace Internal
{

namespace
{

// Signals

const char* const SIGNAL_KEY_EVENT =                 "keyEvent";
const char* const SIGNAL_KEY_EVENT_GENERATED =       "keyEventGenerated";
const char* const SIGNAL_EVENT_PROCESSING_FINISHED = "eventProcessingFinished";
const char* const SIGNAL_TOUCHED =                   "touched";
const char* const SIGNAL_TOUCH =                     "touch";
const char* const SIGNAL_WHEEL_EVENT =               "wheelEvent";
const char* const SIGNAL_CONTEXT_LOST =              "contextLost";
const char* const SIGNAL_CONTEXT_REGAINED =          "contextRegained";
const char* const SIGNAL_SCENE_CREATED =             "sceneCreated";

TypeRegistration mType( typeid(Dali::Stage), typeid(Dali::BaseHandle), NULL );

SignalConnectorType signalConnector1( mType, SIGNAL_KEY_EVENT,                 &Stage::DoConnectSignal );
SignalConnectorType signalConnector2( mType, SIGNAL_EVENT_PROCESSING_FINISHED, &Stage::DoConnectSignal );
SignalConnectorType signalConnector3( mType, SIGNAL_TOUCHED,                   &Stage::DoConnectSignal );
SignalConnectorType signalConnector4( mType, SIGNAL_WHEEL_EVENT,               &Stage::DoConnectSignal );
SignalConnectorType signalConnector5( mType, SIGNAL_CONTEXT_LOST,              &Stage::DoConnectSignal );
SignalConnectorType signalConnector6( mType, SIGNAL_CONTEXT_REGAINED,          &Stage::DoConnectSignal );
SignalConnectorType signalConnector7( mType, SIGNAL_SCENE_CREATED,             &Stage::DoConnectSignal );
SignalConnectorType signalConnector8( mType, SIGNAL_KEY_EVENT_GENERATED,       &Stage::DoConnectSignal );
SignalConnectorType signalConnector9( mType, SIGNAL_TOUCH,                     &Stage::DoConnectSignal );

} // unnamed namespace

StagePtr Stage::New( SceneGraph::UpdateManager& updateManager )
{
  return StagePtr( new Stage( updateManager ) );
}

void Stage::Initialize( Scene& scene )
{
  mScene = &scene;
  mScene->SetBackgroundColor( Dali::Stage::DEFAULT_BACKGROUND_COLOR );
  mScene->EventProcessingFinishedSignal().Connect( this, &Stage::OnEventProcessingFinished );
  mScene->KeyEventSignal().Connect( this, &Stage::OnKeyEvent );
  mScene->TouchedSignal().Connect( this, &Stage::OnTouchedEvent );
  mScene->TouchSignal().Connect( this, &Stage::OnTouchEvent );
  mScene->WheelEventSignal().Connect( this, &Stage::OnWheelEvent );
}

StagePtr Stage::GetCurrent()
{
  StagePtr stage( NULL );
  // no checking in this version
  ThreadLocalStorage* tls = ThreadLocalStorage::GetInternal();
  if( tls )
  {
    stage = tls->GetCurrentStage();
  }
  return stage;
}

bool Stage::IsInstalled()
{
  return ThreadLocalStorage::Created();
}

ObjectRegistry& Stage::GetObjectRegistry()
{
  return ThreadLocalStorage::Get().GetObjectRegistry();
}

Layer& Stage::GetRootActor()
{
  Dali::Layer rootLayer = GetRootLayer();
  return GetImplementation( rootLayer );
}

void Stage::Add( Actor& actor )
{
  mScene->Add( actor );
}

void Stage::Remove( Actor& actor )
{
  mScene->Remove( actor );
}

Vector2 Stage::GetSize() const
{
  return mScene->GetSize();
}

RenderTaskList& Stage::GetRenderTaskList() const
{
  return mScene->GetRenderTaskList();
}

Actor& Stage::GetDefaultRootActor()
{
  return mScene->GetDefaultRootActor();
}

CameraActor& Stage::GetDefaultCameraActor()
{
  return mScene->GetDefaultCameraActor();
}

uint32_t Stage::GetLayerCount() const
{
  return mScene->GetLayerCount();
}

Dali::Layer Stage::GetLayer( uint32_t depth ) const
{
  return mScene->GetLayer( depth );
}

Dali::Layer Stage::GetRootLayer() const
{
  return mScene->GetRootLayer();
}

LayerList& Stage::GetLayerList()
{
  return mScene->GetLayerList();
}

void Stage::SetBackgroundColor(Vector4 color)
{
  mScene->SetBackgroundColor( color );
}

Vector4 Stage::GetBackgroundColor() const
{
  return mScene->GetBackgroundColor();
}

Vector2 Stage::GetDpi() const
{
  return mScene->GetDpi();
}

void Stage::KeepRendering( float durationSeconds )
{
  // Send message to keep rendering
  KeepRenderingMessage( mUpdateManager, durationSeconds );
}

void Stage::SetRenderingBehavior( DevelStage::Rendering renderingBehavior )
{
  if( mRenderingBehavior != renderingBehavior )
  {
    // Send message to change the rendering behavior
    SetRenderingBehaviorMessage( mUpdateManager, renderingBehavior );

    mRenderingBehavior = renderingBehavior;
  }
}

DevelStage::Rendering Stage::GetRenderingBehavior() const
{
  return mRenderingBehavior;
}

bool Stage::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  Stage* stage = static_cast< Stage* >(object); // TypeRegistry guarantees that this is the correct type.

  if( 0 == strcmp( signalName.c_str(), SIGNAL_KEY_EVENT ) )
  {
    stage->KeyEventSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_KEY_EVENT_GENERATED ) )
  {
    stage->KeyEventGeneratedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_EVENT_PROCESSING_FINISHED ) )
  {
    stage->EventProcessingFinishedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_TOUCHED ) )
  {
    stage->TouchedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_TOUCH ) )
  {
    stage->TouchSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_WHEEL_EVENT ) )
  {
    stage->WheelEventSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_CONTEXT_LOST ) )
  {
    stage->ContextLostSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_CONTEXT_REGAINED ) )
  {
    stage->ContextRegainedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_SCENE_CREATED ) )
  {
    stage->SceneCreatedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

void Stage::OnEventProcessingFinished()
{
  EmitEventProcessingFinishedSignal();
}

void Stage::OnKeyEvent( const Dali::KeyEvent& event )
{
  bool consumed = EmitKeyEventGeneratedSignal( event );
  if( !consumed )
  {
    EmitKeyEventSignal( event );
  }
}

void Stage::OnTouchedEvent( const Dali::TouchEvent& touchEvent )
{
  mTouchedSignal.Emit( touchEvent );
}

void Stage::OnTouchEvent( const Dali::TouchData& touch )
{
  mTouchSignal.Emit( touch );
}

void Stage::OnWheelEvent( const Dali::WheelEvent& event )
{
  EmitWheelEventSignal( event );
}

void Stage::EmitKeyEventSignal(const KeyEvent& event)
{
  // Emit the key event signal when no actor in the stage has gained the key input focus

  mKeyEventSignal.Emit( event );
}

bool Stage::EmitKeyEventGeneratedSignal(const KeyEvent& event)
{
  // Emit the KeyEventGenerated signal when KeyEvent is generated

  return mKeyEventGeneratedSignal.Emit( event );
}

void Stage::EmitEventProcessingFinishedSignal()
{
   mEventProcessingFinishedSignal.Emit();
}

void Stage::EmitTouchedSignal( const TouchEvent& touchEvent, const Dali::TouchData& touch )
{
  mTouchedSignal.Emit( touchEvent );
  mTouchSignal.Emit( touch );
}

void Stage::EmitWheelEventSignal( const WheelEvent& event )
{
  // Emit the wheel event signal when no actor in the stage has gained the wheel input focus

  mWheelEventSignal.Emit( event );
}

void Stage::EmitSceneCreatedSignal()
{
  mSceneCreatedSignal.Emit();
}

Dali::Stage::KeyEventSignalType& Stage::KeyEventSignal()
{
  return mKeyEventSignal;
}

Dali::DevelStage::KeyEventGeneratedSignalType& Stage::KeyEventGeneratedSignal()
{
  return mKeyEventGeneratedSignal;
}

void Stage::AddFrameCallback( FrameCallbackInterface& frameCallback, Actor& rootActor )
{
  DALI_ASSERT_ALWAYS( ( ! FrameCallbackInterface::Impl::Get( frameCallback ).IsConnectedToSceneGraph() )
                      && "FrameCallbackInterface implementation already added" );

  // Create scene-graph object and transfer to UpdateManager
  OwnerPointer< SceneGraph::FrameCallback > transferOwnership( SceneGraph::FrameCallback::New( frameCallback ) );
  AddFrameCallbackMessage( mUpdateManager, transferOwnership, rootActor.GetNode() );
}

void Stage::RemoveFrameCallback( FrameCallbackInterface& frameCallback )
{
  FrameCallbackInterface::Impl::Get( frameCallback ).Invalidate();
  RemoveFrameCallbackMessage( mUpdateManager, frameCallback );
}

Dali::Stage::EventProcessingFinishedSignalType& Stage::EventProcessingFinishedSignal()
{
  return mEventProcessingFinishedSignal;
}

Dali::Stage::TouchedSignalType& Stage::TouchedSignal()
{
  DALI_LOG_WARNING( "Deprecated. Use TouchSignal() instead.\n" );
  return mTouchedSignal;
}

Dali::Stage::TouchSignalType& Stage::TouchSignal()
{
  return mTouchSignal;
}

Dali::Stage::WheelEventSignalType& Stage::WheelEventSignal()
{
  return mWheelEventSignal;
}

Dali::Stage::ContextStatusSignal& Stage::ContextLostSignal()
{
  return mContextLostSignal;
}

Dali::Stage::ContextStatusSignal& Stage::ContextRegainedSignal()
{
  return mContextRegainedSignal;
}

Dali::Stage::SceneCreatedSignalType& Stage::SceneCreatedSignal()
{
  return mSceneCreatedSignal;
}

void Stage::NotifyContextLost()
{
  mContextLostSignal.Emit();
}

void Stage::NotifyContextRegained()
{
  mContextRegainedSignal.Emit();
}

Stage::Stage( SceneGraph::UpdateManager& updateManager )
: mUpdateManager( updateManager ),
  mKeyEventSignal(),
  mKeyEventGeneratedSignal(),
  mEventProcessingFinishedSignal(),
  mTouchedSignal(),
  mTouchSignal(),
  mWheelEventSignal(),
  mContextLostSignal(),
  mContextRegainedSignal(),
  mSceneCreatedSignal(),
  mRenderingBehavior( DevelStage::Rendering::IF_REQUIRED )
{
}

Stage::~Stage()
{
}

} // namespace Internal

} // namespace Dali
