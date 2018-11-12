/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/system-overlay.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/actors/layer-list.h>
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/common/system-overlay-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/common/property-notification-manager.h>
#include <dali/internal/event/render-tasks/render-task-list-impl.h>
#include <dali/internal/event/update/frame-callback-interface-impl.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/manager/scene-graph-frame-callback.h>
#include <dali/internal/event/common/object-registry-impl.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/public-api/common/constants.h>
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

const float DEFAULT_STEREO_BASE( 65.0f );

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

StagePtr Stage::New( AnimationPlaylist& playlist,
                     PropertyNotificationManager& propertyNotificationManager,
                     SceneGraph::UpdateManager& updateManager,
                     NotificationManager& notificationManager,
                     Integration::RenderController& renderController )
{
  return StagePtr( new Stage( playlist, propertyNotificationManager, updateManager, notificationManager, renderController ) );
}

void Stage::Initialize( bool renderToFbo )
{
  mRenderToFbo = renderToFbo;
  mObjectRegistry = ObjectRegistry::New();

  // Create the ordered list of layers
  mLayerList = LayerList::New( mUpdateManager );

  // The stage owns the default layer
  mRootLayer = Layer::NewRoot( *mLayerList, mUpdateManager );
  mRootLayer->SetName("RootLayer");
  // The root layer needs to have a fixed resize policy (as opposed to the default USE_NATURAL_SIZE).
  // This stops actors parented to the stage having their relayout requests propagating
  // up to the root layer, and down through other children unnecessarily.
  mRootLayer->SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );

  // Create the default camera actor first; this is needed by the RenderTaskList
  CreateDefaultCameraActor();

  // Create the list of render-tasks
  mRenderTaskList = RenderTaskList::New();

  // Create the default render-task (don't need the returned handle)
  mRenderTaskList->CreateTask( mRootLayer.Get(), mDefaultCamera.Get() );
}

void Stage::Uninitialize()
{
  // Remove actors added to SystemOverlay
  delete mSystemOverlay;
  mSystemOverlay = NULL;

  if( mDefaultCamera )
  {
    // its enough to release the handle so the object is released
    // don't need to remove it from root actor as root actor will delete the object
    mDefaultCamera.Reset();
  }

  if( mRootLayer )
  {
    // we are closing down so just delete the root, no point emit disconnect
    // signals or send messages to update
    mRootLayer.Reset();
  }

  if( mRenderTaskList )
  {
    mRenderTaskList.Reset();
  }
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
  return *mObjectRegistry;
}

void Stage::RegisterObject( Dali::BaseObject* object )
{
  mObjectRegistry->RegisterObject( object );
}

void Stage::UnregisterObject( Dali::BaseObject* object )
{
  mObjectRegistry->UnregisterObject( object );
}

Layer& Stage::GetRootActor()
{
  return *mRootLayer;
}

AnimationPlaylist& Stage::GetAnimationPlaylist()
{
  return mAnimationPlaylist;
}

PropertyNotificationManager& Stage::GetPropertyNotificationManager()
{
  return mPropertyNotificationManager;
}

void Stage::Add( Actor& actor )
{
  mRootLayer->Add( actor );
}

void Stage::Remove( Actor& actor )
{
  mRootLayer->Remove( actor );
}

void Stage::SurfaceResized( float width, float height )
{
  if( ( fabsf( width - mSurfaceSize.width ) > Math::MACHINE_EPSILON_1000 ) || ( fabsf( height - mSurfaceSize.height ) > Math::MACHINE_EPSILON_1000 ) )
  {
    mSurfaceSize.width = width;
    mSurfaceSize.height = height;

    // Internally we want to report the actual size of the stage.
    mSize.width = width;
    mSize.height = height - static_cast<float>( mTopMargin );

    // Calculates the aspect ratio, near and far clipping planes, field of view and camera Z position.
    mDefaultCamera->SetPerspectiveProjection( mSurfaceSize );

    // Adjust the camera height to allow for top-margin
    SetDefaultCameraPosition();

    mRootLayer->SetSize( mSize.width, mSize.height );

    // Repeat for SystemOverlay actors
    if( mSystemOverlay )
    {
      // Note that the SystemOverlay has a separate camera, configured for the full surface-size.
      // This will remain unaffected by changes in SetDefaultCameraPosition()
      mSystemOverlay->GetImpl()->SetSize( width, height );
    }

    SetDefaultSurfaceRectMessage( mUpdateManager, Rect<int32_t>( 0, 0, static_cast<int32_t>( width ), static_cast<int32_t>( height ) ) ); // truncated

    // if single render task to screen then set its viewport parameters
    if( 1 == mRenderTaskList->GetTaskCount() )
    {
      Dali::RenderTask defaultRenderTask = mRenderTaskList->GetTask( 0u );

      if(!defaultRenderTask.GetTargetFrameBuffer())
      {
        defaultRenderTask.SetViewport( Viewport( 0, 0, static_cast<int32_t>( width ), static_cast<int32_t>( height ) ) ); // truncated
      }
    }

    if( mRenderToFbo )
    {
      Dali::FrameBuffer frameBuffer = Dali::FrameBuffer::New( static_cast<uint32_t>( width ), static_cast<uint32_t>( height ), Dali::FrameBuffer::Attachment::NONE );
      Dali::Texture texture = Dali::Texture::New( Dali::TextureType::TEXTURE_2D, Dali::Pixel::RGB888, static_cast<uint32_t>( width ), static_cast<uint32_t>( height ) );
      frameBuffer.AttachColorTexture( texture );

      Dali::RenderTask defaultRenderTask = mRenderTaskList->GetTask( 0u );
      defaultRenderTask.SetFrameBuffer( frameBuffer );
    }
  }
}

Vector2 Stage::GetSize() const
{
  return mSize;
}

void Stage::SetTopMargin( uint32_t margin )
{
  if (mTopMargin == margin)
  {
    return;
  }
  mTopMargin = margin;

  mSize.width = mSurfaceSize.width;
  mSize.height = mSurfaceSize.height - static_cast<float>( mTopMargin );

  // Adjust the camera height to allow for top-margin
  SetDefaultCameraPosition();

  mRootLayer->SetSize( mSize.width, mSize.height );
}

RenderTaskList& Stage::GetRenderTaskList() const
{
  return *mRenderTaskList;
}

void Stage::CreateDefaultCameraActor()
{
  // The default camera attributes and position is such that
  // children of the default layer, can be positioned at (0,0) and
  // be at the top-left of the viewport.
  mDefaultCamera = CameraActor::New( Size::ZERO );
  mDefaultCamera->SetParentOrigin(ParentOrigin::CENTER);
  Add(*(mDefaultCamera.Get()));
}

void Stage::SetDefaultCameraPosition()
{
  mDefaultCamera->SetY( -(static_cast<float>(mTopMargin) * 0.5f) );
}

Actor& Stage::GetDefaultRootActor()
{
  return *mRootLayer;
}

CameraActor& Stage::GetDefaultCameraActor()
{
  return *mDefaultCamera;
}

uint32_t Stage::GetLayerCount() const
{
  return mLayerList->GetLayerCount();
}

Dali::Layer Stage::GetLayer( uint32_t depth ) const
{
  return Dali::Layer(mLayerList->GetLayer( depth ));
}

Dali::Layer Stage::GetRootLayer() const
{
  return Dali::Layer( mRootLayer.Get() );
}

LayerList& Stage::GetLayerList()
{
  return *mLayerList;
}

Integration::SystemOverlay& Stage::GetSystemOverlay()
{
  // Lazily create system-level if requested
  if( !mSystemOverlay )
  {
    mSystemOverlay = new Integration::SystemOverlay( SystemOverlay::New( *this ) );
    DALI_ASSERT_ALWAYS( NULL != mSystemOverlay && "Failed to create system overlay" );

    mSystemOverlay->GetImpl()->SetSize( mSize.width, mSize.height );
  }

  return *mSystemOverlay;
}

SystemOverlay* Stage::GetSystemOverlayInternal()
{
  SystemOverlay* overlay( NULL );

  if( mSystemOverlay )
  {
    overlay = mSystemOverlay->GetImpl();
  }

  return overlay;
}

void Stage::SetBackgroundColor(Vector4 color)
{
  // Cache for public GetBackgroundColor()
  mBackgroundColor = color;

  // Send message to change color in next frame
  SetBackgroundColorMessage( mUpdateManager, color );
}

Vector4 Stage::GetBackgroundColor() const
{
  return mBackgroundColor;
}

Vector2 Stage::GetDpi() const
{
  return mDpi;
}

void Stage::SetDpi(Vector2 dpi)
{
  mDpi = dpi;
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

void Stage::EmitWheelEventSignal(const WheelEvent& event)
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


void Stage::RequestRebuildDepthTree()
{
  DALI_LOG_INFO(gLogFilter, Debug::General, "RequestRebuildDepthTree()\n");
  mDepthTreeDirty = true;
}

void Stage::RebuildDepthTree()
{
  // If the depth tree needs rebuilding, do it in this frame only.
  if( mDepthTreeDirty )
  {
    DALI_LOG_INFO(gLogFilter, Debug::Concise, "RebuildDepthTree() dirty:T\n");

    ActorPtr actor( mRootLayer.Get() );
    actor->RebuildDepthTree();
    mDepthTreeDirty = false;
  }
}


Stage::Stage( AnimationPlaylist& playlist,
              PropertyNotificationManager& propertyNotificationManager,
              SceneGraph::UpdateManager& updateManager,
              NotificationManager& notificationManager,
              Integration::RenderController& renderController )
: mAnimationPlaylist( playlist ),
  mPropertyNotificationManager( propertyNotificationManager ),
  mUpdateManager( updateManager ),
  mNotificationManager( notificationManager ),
  mRenderController( renderController ),
  mSize( Vector2::ZERO ),
  mSurfaceSize( Vector2::ZERO ),
  mBackgroundColor( Dali::Stage::DEFAULT_BACKGROUND_COLOR ),
  mTopMargin( 0 ),
  mDpi( Vector2::ZERO ),
  mSystemOverlay( NULL ),
  mKeyEventSignal(),
  mKeyEventGeneratedSignal(),
  mEventProcessingFinishedSignal(),
  mTouchedSignal(),
  mTouchSignal(),
  mWheelEventSignal(),
  mContextLostSignal(),
  mContextRegainedSignal(),
  mSceneCreatedSignal(),
  mRenderingBehavior( DevelStage::Rendering::IF_REQUIRED ),
  mDepthTreeDirty( false ),
  mForceNextUpdate( false ),
  mRenderToFbo( false )
{
}

SceneGraph::UpdateManager& Stage::GetUpdateManager()
{
  return mUpdateManager;
}

Integration::RenderController& Stage::GetRenderController()
{
  return mRenderController;
}

uint32_t* Stage::ReserveMessageSlot( uint32_t size, bool updateScene )
{
  return mUpdateManager.ReserveMessageSlot( size, updateScene );
}

BufferIndex Stage::GetEventBufferIndex() const
{
  return mUpdateManager.GetEventBufferIndex();
}

void Stage::ForceNextUpdate()
{
  mForceNextUpdate = true;
}

bool Stage::IsNextUpdateForced()
{
  bool nextUpdateForced = mForceNextUpdate;
  mForceNextUpdate = false;
  return nextUpdateForced;
}

Stage::~Stage()
{
  delete mSystemOverlay;

  mObjectRegistry.Reset();
}

} // namespace Internal

} // namespace Dali
