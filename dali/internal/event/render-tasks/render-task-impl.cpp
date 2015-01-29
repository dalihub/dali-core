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
#include <dali/internal/event/render-tasks/render-task-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/common/event-to-update.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/images/frame-buffer-image-impl.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>
#include <dali/internal/update/nodes/node.h>

#if defined(DEBUG_ENABLED)
namespace
{
Debug::Filter* gLogRender = Debug::Filter::New(Debug::Concise, false, "LOG_RENDER_TASK");
}
#endif

namespace Dali
{

const Property::Index RenderTask::VIEWPORT_POSITION         = 0;
const Property::Index RenderTask::VIEWPORT_SIZE             = 1;
const Property::Index RenderTask::CLEAR_COLOR               = 2;

namespace Internal
{

namespace // For internal properties
{

const char* DEFAULT_PROPERTY_NAMES[] =
{
  "viewport-position",
  "viewport-size",
  "clear-color"
};
const int DEFAULT_PROPERTY_COUNT = sizeof( DEFAULT_PROPERTY_NAMES ) / sizeof( DEFAULT_PROPERTY_NAMES[0]);

const Property::Type DEFAULT_PROPERTY_TYPES[DEFAULT_PROPERTY_COUNT] =
{
  Property::VECTOR2,    // viewport-position
  Property::VECTOR2,    // viewport-size
  Property::VECTOR4,    // clear-color
};

}// unnamed namespace

RenderTask* RenderTask::New( bool isSystemLevel )
{
  RenderTask* task( new RenderTask( ThreadLocalStorage::Get().GetEventToUpdate(), isSystemLevel ) );

  return task;
}

void RenderTask::SetSourceActor( Actor* actor )
{
  mSourceConnector.SetActor( actor );
}

Actor* RenderTask::GetSourceActor() const
{
  return mSourceConnector.mActor;
}

void RenderTask::SetExclusive( bool exclusive )
{
  if ( mExclusive != exclusive )
  {
    mExclusive = exclusive;

    if ( mSceneObject )
    {
      // mSceneObject is being used in a separate thread; queue a message to set the value
      SetExclusiveMessage( mEventToUpdate, *mSceneObject, mExclusive );
    }
  }
}

bool RenderTask::IsExclusive() const
{
  return mExclusive;
}

void RenderTask::SetInputEnabled( bool enabled )
{
  mInputEnabled = enabled;
}

bool RenderTask::GetInputEnabled() const
{
  return mInputEnabled;
}

void RenderTask::SetCameraActor( CameraActor* cameraActor )
{
  mCameraConnector.SetActor( cameraActor );
}

CameraActor* RenderTask::GetCameraActor() const
{
  // camera connector can only point to camera actor
  return static_cast< CameraActor* >( mCameraConnector.mActor );
}

void RenderTask::SetTargetFrameBuffer( Dali::FrameBufferImage image )
{
  if ( mFrameBufferImage != image )
  {
    // if we have a scene object we need to track connection status and set frame buffer id as well as updating target frame buffer
    if ( mSceneObject )
    {
      if(mFrameBufferImage)
      {
        GetImplementation(mFrameBufferImage).Disconnect();
      }

      // update target frame buffer
      mFrameBufferImage = image;

      unsigned int resourceId = 0;
      if(mFrameBufferImage)
      {
        GetImplementation(mFrameBufferImage).Connect();

        resourceId = GetImplementation( mFrameBufferImage ).GetResourceId();
      }

      // mSceneObject is being used in a separate thread; queue a message to set the value
      SetFrameBufferIdMessage( mEventToUpdate, *mSceneObject, resourceId );
    }
    else
    {
      // update target frame buffer
      mFrameBufferImage = image;
    }
  }
}

Dali::FrameBufferImage RenderTask::GetTargetFrameBuffer() const
{
  return mFrameBufferImage;
}

void RenderTask::SetScreenToFrameBufferFunction( ScreenToFrameBufferFunction conversionFunction )
{
  mScreenToFrameBufferFunction = conversionFunction;
}

RenderTask::ScreenToFrameBufferFunction RenderTask::GetScreenToFrameBufferFunction() const
{
  return mScreenToFrameBufferFunction;
}

void RenderTask::SetScreenToFrameBufferMappingActor( Actor* mappingActor )
{
  mMappingConnector.SetActor( mappingActor );
}

Actor* RenderTask::GetScreenToFrameBufferMappingActor() const
{
  return mMappingConnector.mActor;
}

void RenderTask::SetViewportPosition(const Vector2& value)
{
  StagePtr stage = Stage::GetCurrent();
  BakeViewportPositionMessage( stage->GetUpdateInterface(), *mSceneObject, value );
}

Vector2 RenderTask::GetCurrentViewportPosition() const
{
  StagePtr stage = Stage::GetCurrent();
  return mSceneObject->GetViewportPosition(stage->GetEventBufferIndex());
}

void RenderTask::SetViewportSize(const Vector2& value)
{
  StagePtr stage = Stage::GetCurrent();
  BakeViewportSizeMessage( stage->GetUpdateInterface(), *mSceneObject, value );
}

Vector2 RenderTask::GetCurrentViewportSize() const
{
  StagePtr stage = Stage::GetCurrent();
  return mSceneObject->GetViewportSize(stage->GetEventBufferIndex());
}

void RenderTask::SetViewport( const Viewport& viewport )
{
  SetViewportPosition(Vector2(viewport.x, viewport.y));
  SetViewportSize(Vector2(viewport.width, viewport.height));
}

void RenderTask::GetViewport( Viewport& viewPort ) const
{
  BufferIndex bufferIndex = Stage::GetCurrent()->GetEventBufferIndex();

  if(!mSceneObject->GetViewportEnabled( bufferIndex ))
  {
    if ( mFrameBufferImage )
    {
      viewPort.x = viewPort.y = 0;
      viewPort.width = mFrameBufferImage.GetWidth();
      viewPort.height = mFrameBufferImage.GetHeight();
    }
    else
    {
      Vector2 size( Stage::GetCurrent()->GetSize() );
      viewPort.x = viewPort.y = 0;
      viewPort.width = size.width;
      viewPort.height = size.height;
    }
  }
  else
  {
    const Vector2& position = mSceneObject->GetViewportPosition(bufferIndex);
    const Vector2& size = mSceneObject->GetViewportSize(bufferIndex);
    viewPort.x = position.x;
    viewPort.y = position.y;
    viewPort.width = size.width;
    viewPort.height = size.height;
  }
}

void RenderTask::SetClearColor( const Vector4& color )
{
  if ( mClearColor != color )
  {
    mClearColor = color;

    if ( mSceneObject )
    {
      // mSceneObject is being used in a separate thread; queue a message to set the value
      StagePtr stage = Stage::GetCurrent();
      BakeClearColorMessage( stage->GetUpdateInterface(), *mSceneObject, color );
    }
  }
}

const Vector4& RenderTask::GetClearColor() const
{
  StagePtr stage = Stage::GetCurrent();
  return mSceneObject->GetClearColor(stage->GetEventBufferIndex());
}

void RenderTask::SetClearEnabled( bool enabled )
{
  if ( mClearEnabled != enabled )
  {
    mClearEnabled = enabled;

    if ( mSceneObject )
    {
      // mSceneObject is being used in a separate thread; queue a message to set the value
      SetClearEnabledMessage( mEventToUpdate, *mSceneObject, mClearEnabled );
    }
  }
}

bool RenderTask::GetClearEnabled() const
{
  return mClearEnabled;
}

void RenderTask::SetCullMode( bool mode )
{
  if ( mCullMode != mode )
  {
    mCullMode = mode;

    if ( mSceneObject )
    {
      // mSceneObject is being used in a separate thread; queue a message to set the value
      SetCullModeMessage( mEventToUpdate, *mSceneObject, mCullMode );
    }
  }
}

bool RenderTask::GetCullMode() const
{
  return mCullMode;
}

void RenderTask::SetRefreshRate( unsigned int refreshRate )
{
  DALI_LOG_TRACE_METHOD_FMT(gLogRender, "this:%p  rate:%d\n", this, refreshRate);
  DALI_LOG_INFO(gLogRender, Debug::General, "RenderTask::SetRefreshRate(this:%p, %d)\n", this, refreshRate);

  mRefreshRate = refreshRate; // cached for GetRefreshRate()

  // Note - even when refreshRate is the same as mRefreshRate, a message should be sent

  if ( mSceneObject )
  {
    // mSceneObject is being used in a separate thread; queue a message to set the value
    SetRefreshRateMessage( mEventToUpdate, *mSceneObject, refreshRate );
  }
}

unsigned int RenderTask::GetRefreshRate() const
{
  return mRefreshRate;
}

bool RenderTask::IsHittable( Vector2& screenCoords ) const
{
  // True when input is enabled, source & camera actor are valid
  bool inputEnabled( false );

  Actor* sourceActor = GetSourceActor();
  CameraActor* cameraActor = GetCameraActor();

  if ( mInputEnabled  &&
       NULL != sourceActor    &&
       sourceActor->OnStage() &&
       NULL != cameraActor    &&
       cameraActor->OnStage() )
  {
    // If the actors are rendered off-screen, then the screen coordinates must be converted
    // and the conversion function will tell us if they are inside or outside
    if ( TranslateCoordinates( screenCoords ) )
    {
      // This is a suitable render-task for input handling
      inputEnabled = true;
    }
  }

  return inputEnabled;
}

bool RenderTask::TranslateCoordinates( Vector2& screenCoords ) const
{
  // return true for on-screen tasks
  bool inside( true );
  // If the actors are rendered off-screen, then the screen coordinates must be converted
  // the function should only be called for offscreen tasks
  if( mFrameBufferImage && mMappingConnector.mActor )
  {
    CameraActor* localCamera = GetCameraActor();
    StagePtr stage = Stage::GetCurrent();
    CameraActor& defaultCamera = stage->GetDefaultCameraActor();
    if( localCamera )
    {
      Viewport viewport;
      Vector2 size( stage->GetSize() );
      viewport.x = viewport.y = 0;
      viewport.width = size.width;
      viewport.height = size.height;

      float localX, localY;
      inside = mMappingConnector.mActor->ScreenToLocal(defaultCamera.GetViewMatrix(), defaultCamera.GetProjectionMatrix(), viewport, localX, localY, screenCoords.x, screenCoords.y);
      Vector3 actorSize = mMappingConnector.mActor->GetCurrentSize();
      if( inside && localX >= 0.f && localX <= actorSize.x && localY >= 0.f && localY <= actorSize.y)
      {
        screenCoords.x = localX;
        screenCoords.y = localY;
      }
      else
      {
        inside = false;
      }
    }
    else
    {
      inside = false;
    }
  }
  else if ( mFrameBufferImage && mScreenToFrameBufferFunction )
  {
    inside = mScreenToFrameBufferFunction( screenCoords );
  }
  return inside;
}

bool RenderTask::IsSystemLevel() const
{
  return mIsSystemLevel;
}

SceneGraph::RenderTask* RenderTask::CreateSceneObject()
{
  // This should only be called once, with no existing scene-object
  DALI_ASSERT_DEBUG( NULL == mSceneObject );

  // Keep the raw-pointer until DiscardSceneObject is called
  mSceneObject = SceneGraph::RenderTask::New();

  // if we have a frame buffer we need to track connection status then send a message to set the frame buffer id in case it has changed since last time we were on stage
  unsigned int resourceId = 0;
  if(mFrameBufferImage)
  {
    GetImplementation(mFrameBufferImage).Connect();

    resourceId = GetImplementation( mFrameBufferImage ).GetResourceId();
  }

  // mSceneObject is being used in a separate thread; queue a message to set the value
  SetFrameBufferIdMessage( mEventToUpdate, *mSceneObject, resourceId );

  // Send messages to set other properties that may have changed since last time we were on stage
  SetExclusiveMessage( mEventToUpdate, *mSceneObject, mExclusive );
  SetClearColorMessage(  mEventToUpdate, *mSceneObject, mClearColor );
  SetClearEnabledMessage(  mEventToUpdate, *mSceneObject, mClearEnabled );
  SetCullModeMessage(  mEventToUpdate, *mSceneObject, mCullMode );
  SetRefreshRateMessage(  mEventToUpdate, *mSceneObject, mRefreshRate );

  // Caller takes ownership
  return mSceneObject;
}

SceneGraph::RenderTask* RenderTask::GetRenderTaskSceneObject()
{
  return mSceneObject;
}

void RenderTask::DiscardSceneObject()
{
  // mSceneObject is not owned; throw away the raw-pointer
  mSceneObject = NULL;

  // if we have a frame buffer we need to track connection status
  if(mFrameBufferImage)
  {
    GetImplementation(mFrameBufferImage).Disconnect();
  }
}

/********************************************************************************
 ********************************************************************************
 ********************************   PROPERTY METHODS   **************************
 ********************************************************************************
 ********************************************************************************/

unsigned int RenderTask::GetDefaultPropertyCount() const
{
  return DEFAULT_PROPERTY_COUNT;
}

void RenderTask::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.reserve( DEFAULT_PROPERTY_COUNT );

  for ( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    indices.push_back( i );
  }
}

const char* RenderTask::GetDefaultPropertyName( Property::Index index ) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_NAMES[index];
  }
  else
  {
    return NULL;
  }
}

Property::Index RenderTask::GetDefaultPropertyIndex(const std::string& name) const
{
  Property::Index index = Property::INVALID_INDEX;

  // Look for name in default properties
  for( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    if( 0 == strcmp( name.c_str(), DEFAULT_PROPERTY_NAMES[ i ] ) ) // dont want to convert rhs to string
    {
      index = i;
      break;
    }
  }

  return index;
}

bool RenderTask::IsDefaultPropertyWritable(Property::Index index) const
{
  return true; // all properties writable
}

bool RenderTask::IsDefaultPropertyAnimatable(Property::Index index) const
{
  return true; // all properties animatable
}

bool RenderTask::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return true; // all properties can be used as constraint input
}

Property::Type RenderTask::GetDefaultPropertyType(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_TYPES[index];
  }
  else
  {
    // index out of range...return Property::NONE
    return Property::NONE;
  }
}

void RenderTask::SetDefaultProperty( Property::Index index, const Property::Value& property )
{
  switch ( index )
  {
    case Dali::RenderTask::VIEWPORT_POSITION:
    {
      SetViewportPosition( property.Get<Vector2>() );
      break;
    }
    case Dali::RenderTask::VIEWPORT_SIZE:
    {
      SetViewportSize( property.Get<Vector2>() );
      break;
    }
    case Dali::RenderTask::CLEAR_COLOR:
    {
      SetClearColor( property.Get<Vector4>() );
      break;
    }
    default:
    {
      // nothing to do
      break;
    }
  }
}

Property::Value RenderTask::GetDefaultProperty(Property::Index index) const
{
  Property::Value value;

  switch ( index )
  {

    case Dali::RenderTask::VIEWPORT_POSITION:
    {
      value = GetCurrentViewportPosition();
      break;
    }
    case Dali::RenderTask::VIEWPORT_SIZE:
    {
      value = GetCurrentViewportSize();
      break;
    }
    case Dali::RenderTask::CLEAR_COLOR:
    {
      value = GetClearColor();
      break;
    }

    default:
    {
      DALI_ASSERT_ALWAYS(false && "RenderTask property index out of range"); // should not come here
      break;
    }
  }

  return value;
}

const SceneGraph::PropertyOwner* RenderTask::GetSceneObject() const
{
  return mSceneObject;
}

const SceneGraph::PropertyBase* RenderTask::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  DALI_ASSERT_ALWAYS( IsPropertyAnimatable(index) && "Property is not animatable" );

  const SceneGraph::PropertyBase* property( NULL );

  // This method should only return a property which is part of the scene-graph
  if( mSceneObject != NULL )
  {
    switch ( index )
    {
      case Dali::RenderTask::VIEWPORT_POSITION:
        property = &mSceneObject->mViewportPosition;
        break;

      case Dali::RenderTask::VIEWPORT_SIZE:
        property = &mSceneObject->mViewportSize;
        break;

      case Dali::RenderTask::CLEAR_COLOR:
        property = &mSceneObject->mClearColor;
        break;

      default:
        break;
    }
  }

  return property;
}

const PropertyInputImpl* RenderTask::GetSceneObjectInputProperty( Property::Index index ) const
{
  const PropertyInputImpl* property( NULL );
  if( mSceneObject != NULL )
  {
    switch ( index )
    {
      case Dali::RenderTask::VIEWPORT_POSITION:
        property = &mSceneObject->mViewportPosition;
        break;

      case Dali::RenderTask::VIEWPORT_SIZE:
        property = &mSceneObject->mViewportSize;
        break;

      case Dali::RenderTask::CLEAR_COLOR:
        property = &mSceneObject->mViewportSize;
        break;

      default:
        break;
    }
  }

  return property;
}

bool RenderTask::HasFinished()
{
  bool finished = false;
  const unsigned int counter = mSceneObject->GetRenderedOnceCounter();

  if( mRefreshOnceCounter < counter )
  {
    finished = true;
    mRefreshOnceCounter = counter;
  }

  DALI_LOG_INFO(gLogRender, Debug::General, "RenderTask::HasFinished()=%s SCRT:%p  SC\n", finished?"T":"F", mSceneObject);

  return finished;
}

void RenderTask::EmitSignalFinish()
{
  DALI_LOG_INFO(gLogRender, Debug::General, "RenderTask::EmitSignalFinish(this:%p)\n", this);

  if( !mSignalFinishedV2.Empty() )
  {
    Dali::RenderTask handle( this );
    mSignalFinishedV2.Emit(handle );
  }
}

Dali::RenderTask::RenderTaskSignalV2& RenderTask::FinishedSignal()
{
  return mSignalFinishedV2;
}

bool RenderTask::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  RenderTask* renderTask = dynamic_cast<RenderTask*>(object);

  if ( Dali::RenderTask::SIGNAL_FINISHED == signalName )
  {
    renderTask->FinishedSignal().Connect( tracker, functor );
  }
  else
  {
    // signalName does not match any signal
    connected = false;
  }

  return connected;
}

RenderTask::RenderTask( EventToUpdate& eventToUpdate, bool isSystemLevel )
: mEventToUpdate( eventToUpdate ),
  mSceneObject( NULL ),
  mSourceConnector( Connector::SOURCE_CONNECTOR, *this ),
  mCameraConnector( Connector::CAMERA_CONNECTOR, *this ),
  mMappingConnector( Connector::MAPPING_CONNECTOR, *this  ),
  mClearColor( Dali::RenderTask::DEFAULT_CLEAR_COLOR ),
  mRefreshRate( Dali::RenderTask::DEFAULT_REFRESH_RATE ),
  mRefreshOnceCounter( 0u ),
  mScreenToFrameBufferFunction( Dali::RenderTask::DEFAULT_SCREEN_TO_FRAMEBUFFER_FUNCTION ),
  mExclusive( Dali::RenderTask::DEFAULT_EXCLUSIVE ),
  mInputEnabled( Dali::RenderTask::DEFAULT_INPUT_ENABLED ),
  mClearEnabled( Dali::RenderTask::DEFAULT_CLEAR_ENABLED ),
  mCullMode( Dali::RenderTask::DEFAULT_CULL_MODE ),
  mIsSystemLevel( isSystemLevel )
{
  DALI_LOG_INFO(gLogRender, Debug::General, "RenderTask::RenderTask(this:%p)\n", this);
}

RenderTask::~RenderTask()
{
  DALI_LOG_INFO(gLogRender, Debug::General, "RenderTask::~RenderTask(this:%p)\n", this);
}

// Helper class for connecting Nodes to the scene-graph RenderTask

RenderTask::Connector::Connector( Type type, RenderTask& renderTask )
: mType( type ),
  mRenderTask( renderTask ),
  mActor( NULL )
{
}

RenderTask::Connector::~Connector()
{
  SetActor( NULL );
}

void RenderTask::Connector::SetActor( Actor* actor )
{
  if ( mActor != actor )
  {
    if ( mActor )
    {
      mActor->RemoveObserver( *this );
    }

    mActor = actor;

    if ( mActor )
    {
      mActor->AddObserver( *this );
    }

    UpdateRenderTask();
  }
}

void RenderTask::Connector::SceneObjectAdded( ProxyObject& proxy )
{
  UpdateRenderTask();
}

void RenderTask::Connector::SceneObjectRemoved( ProxyObject& proxy )
{
  UpdateRenderTask();
}

void RenderTask::Connector::ProxyDestroyed( ProxyObject& proxy )
{
  mActor = NULL;

  UpdateRenderTask();
}

void RenderTask::Connector::UpdateRenderTask()
{
  // Guard to allow handle destruction after Core has been destroyed
  if( Internal::Stage::IsInstalled() &&
      mRenderTask.mSceneObject )
  {
    const SceneGraph::Node* node( NULL );

    // Check whether a Node exists in the scene-graph
    if ( NULL != mActor )
    {
      const SceneGraph::PropertyOwner* object = mActor->GetSceneObject();
      if ( NULL != object )
      {
        // actors only point to nodes as their scene objects
        node = static_cast< const SceneGraph::Node* >( object );
      }
    }

    //the mapping node is not used in the scene graph
    if ( SOURCE_CONNECTOR == mType )
    {
      SetSourceNodeMessage( mRenderTask.mEventToUpdate, *(mRenderTask.mSceneObject), node );
    }
    else if( CAMERA_CONNECTOR == mType )
    {
      SetCameraNodeMessage( mRenderTask.mEventToUpdate, *(mRenderTask.mSceneObject), node );
    }
  }
}

} // namespace Internal

} // namespace Dali
