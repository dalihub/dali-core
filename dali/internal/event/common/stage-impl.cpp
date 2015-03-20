/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/integration-api/system-overlay.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/actors/layer-list.h>
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/actor-attachments/camera-attachment-impl.h>
#include <dali/internal/event/common/system-overlay-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/common/property-notification-manager.h>
#include <dali/internal/event/render-tasks/render-task-list-impl.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/event/common/object-registry-impl.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/render-tasks/render-task-list.h>

#ifdef DYNAMICS_SUPPORT
#include <dali/internal/event/dynamics/dynamics-world-config-impl.h>
#include <dali/internal/event/dynamics/dynamics-world-impl.h>
#include <dali/integration-api/dynamics/dynamics-factory-intf.h>
#include <dali/integration-api/dynamics/dynamics-world-settings.h>
#endif

using Dali::Internal::SceneGraph::Node;

namespace Dali
{

namespace Internal
{

namespace
{

const float DEFAULT_STEREO_BASE( 65.0f );

// Signals

const char* const SIGNAL_KEY_EVENT =                 "key-event";
const char* const SIGNAL_EVENT_PROCESSING_FINISHED = "event-processing-finished";
const char* const SIGNAL_TOUCHED =                   "touched";
const char* const SIGNAL_CONTEXT_LOST =              "context-lost";
const char* const SIGNAL_CONTEXT_REGAINED =          "context-regained";
const char* const SIGNAL_SCENE_CREATED =             "scene-created";

TypeRegistration mType( typeid(Dali::Stage), typeid(Dali::BaseHandle), NULL );

SignalConnectorType signalConnector1( mType, SIGNAL_KEY_EVENT,                 &Stage::DoConnectSignal );
SignalConnectorType signalConnector2( mType, SIGNAL_EVENT_PROCESSING_FINISHED, &Stage::DoConnectSignal );
SignalConnectorType signalConnector3( mType, SIGNAL_TOUCHED,                   &Stage::DoConnectSignal );
SignalConnectorType signalConnector4( mType, SIGNAL_CONTEXT_LOST,              &Stage::DoConnectSignal );
SignalConnectorType signalConnector5( mType, SIGNAL_CONTEXT_REGAINED,          &Stage::DoConnectSignal );
SignalConnectorType signalConnector6( mType, SIGNAL_SCENE_CREATED,             &Stage::DoConnectSignal );

} // unnamed namespace

StagePtr Stage::New( AnimationPlaylist& playlist,
                     PropertyNotificationManager& propertyNotificationManager,
                     SceneGraph::UpdateManager& updateManager,
                     NotificationManager& notificationManager )
{
  return StagePtr( new Stage( playlist, propertyNotificationManager, updateManager, notificationManager ) );
}

void Stage::Initialize()
{
  mObjectRegistry = ObjectRegistry::New();

  // Create the ordered list of layers
  mLayerList = LayerList::New( mUpdateManager, false/*not system-level*/ );

  // The stage owns the default layer
  mRootLayer = Layer::NewRoot( *mLayerList, mUpdateManager, false/*not system-level*/ );
  mRootLayer->SetName("RootLayer");

  // Create the default camera actor first; this is needed by the RenderTaskList
  CreateDefaultCameraActor();

  // Create the list of render-tasks
  mRenderTaskList = RenderTaskList::New( *this, *this, false/*not system-level*/ );

  // Create the default render-task
  Dali::RenderTask defaultRenderTask = mRenderTaskList->CreateTask();
}

void Stage::Uninitialize()
{
  // Remove actors added to SystemOverlay
  delete mSystemOverlay;
  mSystemOverlay = NULL;

  if( mDefaultCamera )
  {
    Remove(*(mDefaultCamera.Get()));
  }

  if( mRootLayer )
  {
    // we are closing down so just delete the root, no point emit disconnect
    // signals or send messages to update
    mRootLayer.Reset();
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

void Stage::SetSize(float width, float height)
{
  // Internally we want to report the actual size of the stage.
  mSize.width  = width;
  mSize.height = height;

  // Calculates the aspect ratio, near and far clipping planes, field of view and camera Z position.
  mDefaultCamera->SetPerspectiveProjection( mSize );

  // The depth of the stage gets set to the maximun of these values
  mRootLayer->SetSize( mSize );

  // Repeat for SystemOverlay actors
  if( mSystemOverlay )
  {
    mSystemOverlay->GetImpl()->SetSize( mSize.width, mSize.height );
  }

  SetDefaultSurfaceRectMessage( mUpdateManager, Rect<int>( 0, 0, width, height ) );
}

Vector2 Stage::GetSize() const
{
  return mSize;
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

Actor& Stage::GetDefaultRootActor()
{
  return *mRootLayer;
}

CameraActor& Stage::GetDefaultCameraActor()
{
  return *mDefaultCamera;
}

unsigned int Stage::GetLayerCount() const
{
  return mLayerList->GetLayerCount();
}

Dali::Layer Stage::GetLayer( unsigned int depth ) const
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

void Stage::SetViewMode( ViewMode viewMode )
{
  if( mViewMode != viewMode )
  {
    DALI_LOG_INFO( Debug::Filter::gActor, Debug::Concise, "View mode changed from %d to %d\n", mViewMode, viewMode);

    if( mViewMode == MONO )
    {
      mDefaultCamera->SetOrientation( Degree( 180.0f ), Vector3::YAXIS );
      mRenderTaskList->GetTask(0).SetSourceActor( Dali::Actor() );

      //Create camera and RenderTask for left eye
      mLeftCamera = CameraActor::New( Size::ZERO );
      mLeftCamera->SetParentOrigin( ParentOrigin::CENTER );
      mDefaultCamera->Add( *mLeftCamera.Get() );
      mLeftRenderTask = mRenderTaskList->CreateTask();
      mLeftRenderTask.SetCameraActor( Dali::CameraActor( mLeftCamera.Get() ) );
      mLeftCamera->SetType( Dali::Camera::FREE_LOOK );

      //Create camera and RenderTask for right eye
      mRightCamera = CameraActor::New( Size::ZERO );
      mRightCamera->SetParentOrigin( ParentOrigin::CENTER );
      mDefaultCamera->Add( *mRightCamera.Get() );
      mRightRenderTask = mRenderTaskList->CreateTask();
      mRightRenderTask.SetClearColor( Vector4( 1.0f,0.0f,0.0f,1.0f));

      mRightRenderTask.SetCameraActor( Dali::CameraActor( mRightCamera.Get() ) );
      mRightCamera->SetType( Dali::Camera::FREE_LOOK );
    }

    // save new mode
    mViewMode = viewMode;

    switch( viewMode )
    {
      case MONO:
      {
        // delete extra stereoscopic render tasks and cameras
        mRenderTaskList->RemoveTask( mLeftRenderTask );
        mDefaultCamera->Remove( *mLeftCamera.Get() );
        mLeftRenderTask.Reset();
        mLeftCamera.Reset();
        mRenderTaskList->RemoveTask( mRightRenderTask );
        mDefaultCamera->Remove( *mRightCamera.Get() );
        mRightRenderTask.Reset();
        mRightCamera.Reset();

        mDefaultCamera->SetOrientation( Degree( 0.0f ), Vector3::YAXIS );
        mDefaultCamera->SetType( Dali::Camera::LOOK_AT_TARGET );
        mRenderTaskList->GetTask(0).SetSourceActor( Dali::Layer(mRootLayer.Get()) );

        break;
      }
      case STEREO_HORIZONTAL:
      {
        //Stereo mode with horizontal split is for landscape mode. That's the reason for the cameras being rotated
        //Top camera renders the scene as seen from the right eye and bottom camera as seen from left.

        //Calculate separation in pixels along vertical axis ( mStereoBase is defined in millimetres )
        const float stereoBase( ( (mStereoBase / 25.4f) * GetDpi().y ) * 0.5f );

        //Calculate aspect ratio
        float aspect = mSize.width / (mSize.height * 0.5f);

        mLeftCamera->SetPerspectiveProjection( mSize, Vector2( 0.0f,stereoBase) );
        mLeftCamera->SetAspectRatio( aspect );
        mLeftCamera->SetOrientation( Degree(-90.0f), Vector3::ZAXIS );
        mLeftCamera->SetPosition( Vector3( stereoBase, 0.0f, 0.0f ) );
        mLeftRenderTask.SetViewport( Viewport(0, mSize.height * 0.5f, mSize.width, mSize.height * 0.5f) );

        mRightCamera->SetPerspectiveProjection( mSize, Vector2( 0.0,  -stereoBase) );
        mRightCamera->SetAspectRatio( aspect );
        mRightCamera->SetOrientation( Degree(-90.0f), Vector3::ZAXIS );
        mRightCamera->SetPosition( Vector3(-stereoBase, 0.0f, 0.0f ) );
        mRightRenderTask.SetViewport( Viewport(0, 0, mSize.width, mSize.height * 0.5f ) );

        break;
      }
      case STEREO_VERTICAL:
      {
        //Calculate separation in pixels along horizontal axis
        const float stereoBase( ( (mStereoBase / 25.4f) * GetDpi().x ) * 0.5f );

        //Recalculate fov based on viewport size
        const float fov = 2.0f * std::atan(  mSize.y / (2.0f * std::max( mSize.x*0.5f, mSize.y )) );

        mLeftCamera->SetPerspectiveProjection( Size( mSize.x * 0.5f, mSize.y ), Vector2(stereoBase,0.0f) );
        mLeftCamera->SetFieldOfView( fov );
        mLeftCamera->SetOrientation( Degree(0.0f), Vector3::ZAXIS );
        mLeftCamera->SetPosition( Vector3( stereoBase, 0.0f, 0.0f ) );
        mLeftRenderTask.SetViewport( Viewport(0, 0, mSize.width * 0.5f, mSize.height ) );

        mRightCamera->SetPerspectiveProjection( Size( mSize.x * 0.5f, mSize.y ), Vector2(-stereoBase,0.0f) );
        mRightCamera->SetFieldOfView( fov );
        mRightCamera->SetOrientation( Degree(0.0f), Vector3::ZAXIS );
        mRightCamera->SetPosition( Vector3( -stereoBase, 0.0f, 0.0f ) );
        mRightRenderTask.SetViewport( Viewport(mSize.width * 0.5f, 0, mSize.width * 0.5f, mSize.height ) );

        break;
      }
      case STEREO_INTERLACED:
      {
        break;
      }
    }
  }
}

ViewMode Stage::GetViewMode() const
{
  return mViewMode;
}

void Stage::SetStereoBase( float stereoBase )
{
  if( ! Equals( mStereoBase, stereoBase ) )
  {
    DALI_LOG_INFO( Debug::Filter::gActor, Debug::Concise, "old( %.2f) new(%.2f)", mStereoBase, stereoBase );
    mStereoBase = stereoBase;

    switch( mViewMode  )
    {
      case STEREO_HORIZONTAL:
      {
        stereoBase = mStereoBase / 25.4f * GetDpi().y * 0.5f;
        float aspect = mSize.width / (mSize.height * 0.5f);

        mLeftCamera->SetPerspectiveProjection( mSize, Vector2( 0.0, stereoBase) );
        mLeftCamera->SetAspectRatio( aspect );
        mLeftCamera->SetPosition( Vector3( stereoBase, 0.0f, 0.0f ) );

        mRightCamera->SetPerspectiveProjection( mSize, Vector2( 0.0, -stereoBase) );
        mRightCamera->SetAspectRatio( aspect );
        mRightCamera->SetPosition( Vector3(-stereoBase, 0.0f, 0.0f ) );

        break;
      }
      case STEREO_VERTICAL:
      {
        stereoBase = mStereoBase / 25.4f * GetDpi().x * 0.5f;
        const float fov = 2.0f * std::atan(  mSize.y / (2.0f * std::max( mSize.x*0.5f, mSize.y )) );

        mLeftCamera->SetPerspectiveProjection( Size( mSize.x * 0.5f, mSize.y ), Vector2(stereoBase,0.0f) );
        mLeftCamera->SetFieldOfView( fov );
        mLeftCamera->SetPosition( Vector3( stereoBase, 0.0f, 0.0f ) );

        mRightCamera->SetPerspectiveProjection( Size( mSize.x * 0.5f, mSize.y ), Vector2(-stereoBase,0.0f) );
        mRightCamera->SetFieldOfView( fov );
        mRightCamera->SetPosition( Vector3(-stereoBase, 0.0f, 0.0f ) );

        break;
      }
      default:
        break;
    }
  }
}

float Stage::GetStereoBase() const
{
  return mStereoBase;
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

#ifdef DYNAMICS_SUPPORT

DynamicsNotifier& Stage::GetDynamicsNotifier()
{
  return mDynamicsNotifier;
}

DynamicsWorldPtr Stage::InitializeDynamics(DynamicsWorldConfigPtr config)
{
  if( !mDynamicsFactory )
  {
    mDynamicsFactory = ThreadLocalStorage::Get().GetPlatformAbstraction().GetDynamicsFactory();
  }

  if( mDynamicsFactory && !mDynamicsWorld )
  {
    if( mDynamicsFactory->InitializeDynamics( *(config->GetSettings()) ) )
    {
      mDynamicsWorld = DynamicsWorld::New();
      mDynamicsWorld->Initialize( *this, *mDynamicsFactory, config );
    }
  }
  return mDynamicsWorld;
}

DynamicsWorldPtr Stage::GetDynamicsWorld()
{
  return mDynamicsWorld;
}

void Stage::TerminateDynamics()
{
  if( mDynamicsWorld )
  {
    mDynamicsWorld->Terminate(*this);
    mDynamicsWorld = NULL;
  }
}

#endif // DYNAMICS_SUPPORT

void Stage::KeepRendering( float durationSeconds )
{
  // Send message to keep rendering
  KeepRenderingMessage( mUpdateManager, durationSeconds );
}

bool Stage::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  Stage* stage = dynamic_cast<Stage*>(object);

  if( 0 == strcmp( signalName.c_str(), SIGNAL_KEY_EVENT ) )
  {
    stage->KeyEventSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_EVENT_PROCESSING_FINISHED ) )
  {
    stage->EventProcessingFinishedSignal().Connect( tracker, functor );
  }
  else if( 0 == strcmp( signalName.c_str(), SIGNAL_TOUCHED ) )
  {
    stage->TouchedSignal().Connect( tracker, functor );
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

void Stage::EmitEventProcessingFinishedSignal()
{
   mEventProcessingFinishedSignal.Emit();
}

void Stage::EmitTouchedSignal( const TouchEvent& touch )
{
  mTouchedSignal.Emit( touch );
}


void Stage::EmitSceneCreatedSignal()
{
  mSceneCreatedSignal.Emit();
}

Dali::Stage::KeyEventSignalType& Stage::KeyEventSignal()
{
  return mKeyEventSignal;
}

Dali::Stage::EventProcessingFinishedSignalType& Stage::EventProcessingFinishedSignal()
{
  return mEventProcessingFinishedSignal;
}

Dali::Stage::TouchedSignalType& Stage::TouchedSignal()
{
  return mTouchedSignal;
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

Stage::Stage( AnimationPlaylist& playlist,
              PropertyNotificationManager& propertyNotificationManager,
              SceneGraph::UpdateManager& updateManager,
              NotificationManager& notificationManager )
: mAnimationPlaylist( playlist ),
  mPropertyNotificationManager(propertyNotificationManager),
  mUpdateManager(updateManager),
  mNotificationManager(notificationManager),
  mSize(Vector2::ZERO),
  mBackgroundColor(Dali::Stage::DEFAULT_BACKGROUND_COLOR),
  mViewMode( MONO ),
  mStereoBase( DEFAULT_STEREO_BASE ),
#ifdef DYNAMICS_SUPPORT
  mDynamicsFactory(NULL),
#endif
  mSystemOverlay(NULL)
{
}

SceneGraph::UpdateManager& Stage::GetUpdateManager()
{
  return mUpdateManager;
}

unsigned int* Stage::ReserveMessageSlot( std::size_t size, bool updateScene )
{
  return mUpdateManager.ReserveMessageSlot( size, updateScene );
}

BufferIndex Stage::GetEventBufferIndex() const
{
  return mUpdateManager.GetEventBufferIndex();
}

Stage::~Stage()
{
  delete mSystemOverlay;

  mObjectRegistry.Reset();
}

} // namespace Internal

} // namespace Dali
