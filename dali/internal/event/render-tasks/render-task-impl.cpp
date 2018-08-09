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
#include <dali/internal/event/render-tasks/render-task-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/camera-actor-impl.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/projection.h>
#include <dali/internal/event/images/frame-buffer-image-impl.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/event/render-tasks/render-task-list-impl.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>

#if defined(DEBUG_ENABLED)
namespace
{
Debug::Filter* gLogRender = Debug::Filter::New(Debug::Concise, false, "LOG_RENDER_TASK");
}
#endif

namespace Dali
{

namespace Internal
{

namespace // For internal properties
{

// Properties

//              Name                 Type     writable animatable constraint-input  enum for index-checking
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "viewportPosition",   VECTOR2,    true,    true,    true,    Dali::RenderTask::Property::VIEWPORT_POSITION )
DALI_PROPERTY( "viewportSize",       VECTOR2,    true,    true,    true,    Dali::RenderTask::Property::VIEWPORT_SIZE     )
DALI_PROPERTY( "clearColor",         VECTOR4,    true,    true,    true,    Dali::RenderTask::Property::CLEAR_COLOR       )
DALI_PROPERTY( "requiresSync",       BOOLEAN,    true,    false,   false,   Dali::RenderTask::Property::REQUIRES_SYNC     )
DALI_PROPERTY_TABLE_END( DEFAULT_OBJECT_PROPERTY_START_INDEX )

// Signals

const char* const SIGNAL_FINISHED = "finished";

TypeRegistration mType( typeid( Dali::RenderTask ), typeid( Dali::BaseHandle ), NULL );

SignalConnectorType signalConnector1( mType, SIGNAL_FINISHED, &RenderTask::DoConnectSignal );

} // Unnamed namespace

RenderTask* RenderTask::New( bool isSystemLevel )
{
  RenderTask* task( new RenderTask( isSystemLevel ) );

  return task;
}

void RenderTask::SetSourceActor( Actor* actor )
{
  const Stage* stage = Stage::GetCurrent();
  if ( stage )
  {
    stage->GetRenderTaskList().SetExclusive( this, mExclusive );
  }
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

    const Stage* stage = Stage::GetCurrent();
    if ( stage )
    {
      stage->GetRenderTaskList().SetExclusive( this, exclusive );
    }

    if ( mSceneObject )
    {
      // mSceneObject is being used in a separate thread; queue a message to set the value
      SetExclusiveMessage( GetEventThreadServices(), *mSceneObject, mExclusive );
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
  if( cameraActor )
  {
    mCameraConnector.mCamera = cameraActor->GetCamera();
  }
  else
  {
    mCameraConnector.mCamera = NULL;
  }
  mCameraConnector.SetActor( cameraActor );
}

CameraActor* RenderTask::GetCameraActor() const
{
  // camera connector can only point to camera actor
  return static_cast< CameraActor* >( mCameraConnector.mActor );
}

void RenderTask::SetTargetFrameBuffer( FrameBufferImagePtr image )
{
  mFrameBufferImage = image;
  FrameBuffer* frameBufferPtr( NULL );
  if( image )
  {
    frameBufferPtr = image->GetFrameBuffer();
  }

  SetFrameBuffer( frameBufferPtr );
}

void RenderTask::SetFrameBuffer( FrameBufferPtr frameBuffer )
{
  mFrameBuffer = frameBuffer;
  SceneGraph::FrameBuffer* renderFrameBufferPtr( nullptr );
  if( frameBuffer )
  {
    renderFrameBufferPtr = mFrameBuffer->GetRenderObject();
  }

  SetFrameBufferMessage( GetEventThreadServices(), *mSceneObject, renderFrameBufferPtr );

  if( frameBuffer != nullptr &&
      EqualsZero( mViewportSize.x ) &&
      EqualsZero( mViewportSize.y ) )
  {
    // No viewport size has been set. Change it to the size of the framebuffer
    SetViewportSize( Vector2( frameBuffer->GetWidth(), frameBuffer->GetHeight() ));
  }
}

FrameBuffer* RenderTask::GetFrameBuffer() const
{
  return mFrameBuffer.Get();
}

FrameBufferImage* RenderTask::GetTargetFrameBuffer() const
{
  return mFrameBufferImage.Get();
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
  mViewportPosition = value;

  BakeViewportPositionMessage( GetEventThreadServices(), *mSceneObject, value );
}

Vector2 RenderTask::GetCurrentViewportPosition() const
{
  return mSceneObject->GetViewportPosition( GetEventThreadServices().GetEventBufferIndex() );
}

void RenderTask::SetViewportSize(const Vector2& value)
{
  mViewportSize = value;

  BakeViewportSizeMessage( GetEventThreadServices(), *mSceneObject, value );
}

Vector2 RenderTask::GetCurrentViewportSize() const
{
  return mSceneObject->GetViewportSize( GetEventThreadServices().GetEventBufferIndex() );
}

void RenderTask::SetViewport( const Viewport& viewport )
{
  SetViewportPosition(Vector2(viewport.x, viewport.y));
  SetViewportSize(Vector2(viewport.width, viewport.height));
}

void RenderTask::GetViewport( Viewport& viewPort ) const
{
  BufferIndex bufferIndex = GetEventThreadServices().GetEventBufferIndex();

  if(!mSceneObject->GetViewportEnabled( bufferIndex ))
  {
    if ( mFrameBufferImage )
    {
      viewPort.x = viewPort.y = 0;
      viewPort.width = mFrameBufferImage->GetWidth();
      viewPort.height = mFrameBufferImage->GetHeight();
    }
    else
    {
      Internal::Stage* stage = Internal::Stage::GetCurrent();
      if ( stage )
      {
        Vector2 size( stage->GetSize() );
        viewPort.x = viewPort.y = 0;
        viewPort.width = size.width;
        viewPort.height = size.height;
      }
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
      BakeClearColorMessage( GetEventThreadServices(), *mSceneObject, color );
    }
  }
}

const Vector4& RenderTask::GetClearColor() const
{
  return mSceneObject->GetClearColor( GetEventThreadServices().GetEventBufferIndex() );
}

void RenderTask::SetSyncRequired( bool requiresSync )
{
  if( mRequiresSync != requiresSync )
  {
    mRequiresSync = requiresSync;

    if( mSceneObject )
    {
      // mSceneObject is being used in a separate thread; queue a message to set the value
      SetSyncRequiredMessage( GetEventThreadServices(), *mSceneObject, requiresSync );
    }
  }
}

bool RenderTask::IsSyncRequired() const
{
  return mRequiresSync;
}

void RenderTask::SetClearEnabled( bool enabled )
{
  if ( mClearEnabled != enabled )
  {
    mClearEnabled = enabled;

    if ( mSceneObject )
    {
      // mSceneObject is being used in a separate thread; queue a message to set the value
      SetClearEnabledMessage( GetEventThreadServices(), *mSceneObject, mClearEnabled );
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
      SetCullModeMessage( GetEventThreadServices(), *mSceneObject, mCullMode );
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
    SetRefreshRateMessage( GetEventThreadServices(), *mSceneObject, refreshRate );
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
    if( stage )
    {
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

bool RenderTask::WorldToViewport(const Vector3 &position, float& viewportX, float& viewportY) const
{
  CameraActor* cam = GetCameraActor();

  Vector4 pos(position);
  pos.w = 1.0;

  Vector4 viewportPosition;

  Viewport viewport;
  GetViewport( viewport );

  bool ok = ProjectFull(pos,
                        cam->GetViewMatrix(),
                        cam->GetProjectionMatrix(),
                        viewport.x,
                        viewport.y,
                        viewport.width,
                        viewport.height,
                        viewportPosition);
  if(ok)
  {
    viewportX = viewportPosition.x;
    viewportY = viewportPosition.y;
  }

  return ok;
}

bool RenderTask::ViewportToLocal(Actor* actor, float viewportX, float viewportY, float &localX, float &localY) const
{
  return actor->ScreenToLocal( *this, localX, localY, viewportX, viewportY );
}

SceneGraph::RenderTask* RenderTask::CreateSceneObject()
{
  // This should only be called once, with no existing scene-object
  DALI_ASSERT_DEBUG( NULL == mSceneObject );

  // Keep the raw-pointer until DiscardSceneObject is called
  mSceneObject = SceneGraph::RenderTask::New();

  // Send messages to set other properties that may have changed since last time we were on stage
  SetExclusiveMessage( GetEventThreadServices(), *mSceneObject, mExclusive );
  SetClearColorMessage(  GetEventThreadServices(), *mSceneObject, mClearColor );
  SetClearEnabledMessage(  GetEventThreadServices(), *mSceneObject, mClearEnabled );
  SetCullModeMessage(  GetEventThreadServices(), *mSceneObject, mCullMode );
  SetRefreshRateMessage(  GetEventThreadServices(), *mSceneObject, mRefreshRate );
  SetSyncRequiredMessage( GetEventThreadServices(), *mSceneObject, mRequiresSync );
  SetFrameBuffer( mFrameBuffer );

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
}

/********************************************************************************
 ********************************   PROPERTY METHODS   **************************
 ********************************************************************************/

unsigned int RenderTask::GetDefaultPropertyCount() const
{
  return DEFAULT_PROPERTY_COUNT;
}

void RenderTask::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.Reserve( DEFAULT_PROPERTY_COUNT );

  for ( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    indices.PushBack( i );
  }
}

const char* RenderTask::GetDefaultPropertyName( Property::Index index ) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].name;
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
    if( 0 == strcmp( name.c_str(), DEFAULT_PROPERTY_DETAILS[i].name ) ) // dont want to convert rhs to string
    {
      index = i;
      break;
    }
  }

  return index;
}

bool RenderTask::IsDefaultPropertyWritable(Property::Index index) const
{
  return DEFAULT_PROPERTY_DETAILS[ index ].writable;
}

bool RenderTask::IsDefaultPropertyAnimatable(Property::Index index) const
{
  return DEFAULT_PROPERTY_DETAILS[ index ].animatable;
}

bool RenderTask::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return DEFAULT_PROPERTY_DETAILS[ index ].constraintInput;
}

Property::Type RenderTask::GetDefaultPropertyType(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].type;
  }

  // index out of range...return Property::NONE
  return Property::NONE;
}

void RenderTask::SetDefaultProperty( Property::Index index, const Property::Value& property )
{
  switch ( index )
  {
    case Dali::RenderTask::Property::VIEWPORT_POSITION:
    {
      SetViewportPosition( property.Get<Vector2>() );
      break;
    }
    case Dali::RenderTask::Property::VIEWPORT_SIZE:
    {
      SetViewportSize( property.Get<Vector2>() );
      break;
    }
    case Dali::RenderTask::Property::CLEAR_COLOR:
    {
      SetClearColor( property.Get<Vector4>() );
      break;
    }
    case Dali::RenderTask::Property::REQUIRES_SYNC:
    {
      SetSyncRequired( property.Get<bool>() );
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

    case Dali::RenderTask::Property::VIEWPORT_POSITION:
    {
      value = mViewportPosition;
      break;
    }
    case Dali::RenderTask::Property::VIEWPORT_SIZE:
    {
      value = mViewportSize;
      break;
    }
    case Dali::RenderTask::Property::CLEAR_COLOR:
    {
      value = mClearColor;
      break;
    }
    case Dali::RenderTask::Property::REQUIRES_SYNC:
    {
      value = IsSyncRequired();
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

Property::Value RenderTask::GetDefaultPropertyCurrentValue( Property::Index index ) const
{
  Property::Value value;

  switch ( index )
  {

    case Dali::RenderTask::Property::VIEWPORT_POSITION:
    {
      value = GetCurrentViewportPosition();
      break;
    }
    case Dali::RenderTask::Property::VIEWPORT_SIZE:
    {
      value = GetCurrentViewportSize();
      break;
    }
    case Dali::RenderTask::Property::CLEAR_COLOR:
    {
      value = GetClearColor();
      break;
    }
    case Dali::RenderTask::Property::REQUIRES_SYNC:
    {
      value = IsSyncRequired();
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

void RenderTask::OnNotifyDefaultPropertyAnimation( Animation& animation, Property::Index index, const Property::Value& value, Animation::Type animationType )
{
  switch( animationType )
  {
    case Animation::TO:
    case Animation::BETWEEN:
    {
      switch ( index )
      {
        case Dali::RenderTask::Property::VIEWPORT_POSITION:
        {
          value.Get( mViewportPosition );
          break;
        }
        case Dali::RenderTask::Property::VIEWPORT_SIZE:
        {
          value.Get( mViewportSize );
          break;
        }
        case Dali::RenderTask::Property::CLEAR_COLOR:
        {
          value.Get( mClearColor );
          break;
        }
        case Dali::RenderTask::Property::REQUIRES_SYNC:
        default:
        {
          // Nothing to do as not animatable
          break;
        }
      }
      break;
    }

    case Animation::BY:
    {
      switch ( index )
      {
        case Dali::RenderTask::Property::VIEWPORT_POSITION:
        {
          AdjustValue< Vector2 >( mViewportPosition, value );
          break;
        }
        case Dali::RenderTask::Property::VIEWPORT_SIZE:
        {
          AdjustValue< Vector2 >( mViewportSize, value );
          break;
        }
        case Dali::RenderTask::Property::CLEAR_COLOR:
        {
          AdjustValue< Vector4 >( mClearColor, value );
          break;
        }
        case Dali::RenderTask::Property::REQUIRES_SYNC:
        default:
        {
          // Nothing to do as not animatable
          break;
        }
      }
      break;
    }
  }
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
      case Dali::RenderTask::Property::VIEWPORT_POSITION:
        property = &mSceneObject->mViewportPosition;
        break;

      case Dali::RenderTask::Property::VIEWPORT_SIZE:
        property = &mSceneObject->mViewportSize;
        break;

      case Dali::RenderTask::Property::CLEAR_COLOR:
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
      case Dali::RenderTask::Property::VIEWPORT_POSITION:
        property = &mSceneObject->mViewportPosition;
        break;

      case Dali::RenderTask::Property::VIEWPORT_SIZE:
        property = &mSceneObject->mViewportSize;
        break;

      case Dali::RenderTask::Property::CLEAR_COLOR:
        property = &mSceneObject->mClearColor;
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

  if( !mSignalFinished.Empty() )
  {
    Dali::RenderTask handle( this );
    mSignalFinished.Emit(handle );
  }
}

Dali::RenderTask::RenderTaskSignalType& RenderTask::FinishedSignal()
{
  return mSignalFinished;
}

bool RenderTask::DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor )
{
  bool connected( true );
  RenderTask* renderTask = static_cast< RenderTask* >(object); // TypeRegistry guarantees that this is the correct type.

  if ( 0 == strcmp( signalName.c_str(), SIGNAL_FINISHED ) )
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

RenderTask::RenderTask( bool isSystemLevel )
: mSceneObject( NULL ),
  mSourceConnector( Connector::SOURCE_CONNECTOR, *this ),
  mCameraConnector( Connector::CAMERA_CONNECTOR, *this ),
  mMappingConnector( Connector::MAPPING_CONNECTOR, *this  ),
  mClearColor( Dali::RenderTask::DEFAULT_CLEAR_COLOR ),
  mViewportPosition( Vector2::ZERO ),
  mViewportSize( Vector2::ZERO ),
  mRefreshRate( Dali::RenderTask::DEFAULT_REFRESH_RATE ),
  mRefreshOnceCounter( 0u ),
  mScreenToFrameBufferFunction( Dali::RenderTask::DEFAULT_SCREEN_TO_FRAMEBUFFER_FUNCTION ),
  mExclusive( Dali::RenderTask::DEFAULT_EXCLUSIVE ),
  mInputEnabled( Dali::RenderTask::DEFAULT_INPUT_ENABLED ),
  mClearEnabled( Dali::RenderTask::DEFAULT_CLEAR_ENABLED ),
  mCullMode( Dali::RenderTask::DEFAULT_CULL_MODE ),
  mIsSystemLevel( isSystemLevel ),
  mRequiresSync( false )
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
  mActor( NULL ),
  mCamera( NULL )
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

void RenderTask::Connector::SceneObjectAdded( Object& object )
{
  UpdateRenderTask();
}

void RenderTask::Connector::SceneObjectRemoved( Object& object )
{
  UpdateRenderTask();
}

void RenderTask::Connector::ObjectDestroyed( Object& object )
{
  if ( SOURCE_CONNECTOR == mType )
  {
    const Stage* stage = Stage::GetCurrent();
    if ( stage )
    {
      stage->GetRenderTaskList().SetExclusive( &mRenderTask, false );
    }
  }

  mActor = NULL;
  mCamera = NULL; // only meaningful for the camera connector but no simple way to distinguish

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
      SetSourceNodeMessage( mRenderTask.GetEventThreadServices(), *(mRenderTask.mSceneObject), node );
    }
    else if( CAMERA_CONNECTOR == mType )
    {
      SetCameraMessage( mRenderTask.GetEventThreadServices(), *(mRenderTask.mSceneObject), node, mCamera );
    }
  }
}

} // namespace Internal

} // namespace Dali
