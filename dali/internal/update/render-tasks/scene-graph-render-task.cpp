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
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>

// INTERNAL INCLUDES
#include <dali/public-api/math/matrix.h>
#include <dali/internal/update/controllers/render-message-dispatcher.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/common/render-tracker.h>

#include <dali/internal/update/render-tasks/scene-graph-render-task-debug.h>

namespace Dali
{
namespace Internal
{

namespace SceneGraph
{

RenderTask* RenderTask::New()
{
  return new RenderTask();
}

RenderTask::~RenderTask()
{
  if ( mSourceNode )
  {
    mSourceNode->RemoveObserver( *this );
    if( mExclusive )
    {
      mSourceNode->SetExclusiveRenderTask( nullptr );
    }
  }
  if ( mCameraNode )
  {
    mCameraNode->RemoveObserver( *this );
  }
  if( mRenderSyncTracker )
  {
    mRenderMessageDispatcher->RemoveRenderTracker( *mRenderSyncTracker );
  }
}

void RenderTask::Initialize( RenderMessageDispatcher& renderMessageDispatcher )
{
  mRenderMessageDispatcher = &renderMessageDispatcher;
}

void RenderTask::SetSourceNode( Node* node )
{
  // Stop observing the old node (if we were)
  if ( mSourceNode )
  {
    mSourceNode->RemoveObserver( *this );
    if( this == mSourceNode->GetExclusiveRenderTask() )
    {
      mSourceNode->SetExclusiveRenderTask( nullptr );
    }
  }

  mSourceNode = node;

  if ( mSourceNode )
  {
    mSourceNode->AddObserver( *this );
    if( mExclusive )
    {
      mSourceNode->SetExclusiveRenderTask( this );
    }
  }
  SetActiveStatus();
}

Node* RenderTask::GetSourceNode() const
{
  return mSourceNode;
}

void RenderTask::SetExclusive( bool exclusive )
{
  mExclusive = exclusive;

  if ( mSourceNode )
  {
    if ( mExclusive )
    {
      mSourceNode->SetExclusiveRenderTask( this );
    }
    else if ( this == mSourceNode->GetExclusiveRenderTask() )
    {
      mSourceNode->SetExclusiveRenderTask( nullptr );
    }
  }
}

bool RenderTask::IsExclusive() const
{
  return mExclusive;
}

void RenderTask::SetCamera( Node* cameraNode, Camera* camera )
{
  if ( mCameraNode )
  {
    mCameraNode->RemoveObserver( *this );
  }

  mCameraNode = cameraNode;
  mCamera = camera;

  if ( mCameraNode )
  {
    mCameraNode->AddObserver( *this );
  }
  SetActiveStatus();
}

void RenderTask::SetFrameBuffer( Render::FrameBuffer* frameBuffer )
{
  mFrameBuffer = frameBuffer;
}

Render::FrameBuffer* RenderTask::GetFrameBuffer()
{
  return mFrameBuffer;
}

bool RenderTask::QueryViewport( BufferIndex bufferIndex, Viewport& viewport ) const
{
  if( ! GetViewportEnabled( bufferIndex ) )
  {
    return false;
  }

  viewport.x = static_cast<int>( mViewportPosition[bufferIndex].x ); // truncated
  viewport.y = static_cast<int>( mViewportPosition[bufferIndex].y ); // truncated
  viewport.width = static_cast<int>( mViewportSize[bufferIndex].width ); // truncated
  viewport.height = static_cast<int>( mViewportSize[bufferIndex].height ); // truncated

  return true;
}

void RenderTask::SetClearColor( BufferIndex updateBufferIndex, const Vector4& value )
{
  mClearColor.Set( updateBufferIndex, value );
}

const Vector4& RenderTask::GetClearColor( BufferIndex bufferIndex ) const
{
  return mClearColor[bufferIndex];
}

void RenderTask::BakeClearColor( BufferIndex updateBufferIndex, const Vector4& value )
{
  mClearColor.Bake( updateBufferIndex, value );
}

void RenderTask::SetClearEnabled( bool enabled )
{
  mClearEnabled = enabled;
}

bool RenderTask::GetClearEnabled() const
{
  return mClearEnabled;
}

void RenderTask::SetCullMode( bool mode )
{
  mCullMode = mode;
}

bool RenderTask::GetCullMode() const
{
  return mCullMode;
}

void RenderTask::SetRefreshRate( uint32_t refreshRate )
{
  DALI_LOG_TRACE_METHOD_FMT(gRenderTaskLogFilter, "this:%p RefreshRate:%d\n", this, refreshRate);

  mRefreshRate = refreshRate;

  if( mRefreshRate > 0 )
  {
    mState = RENDER_CONTINUOUSLY;
  }
  else
  {
    mState = RENDER_ONCE_WAITING_FOR_RESOURCES;
    mWaitingToRender = true;
    mNotifyTrigger = false;
  }

  mFrameCounter = 0u;
}

uint32_t RenderTask::GetRefreshRate() const
{
  return mRefreshRate;
}

bool RenderTask::ReadyToRender( BufferIndex updateBufferIndex )
{
  return mActive;
}

bool RenderTask::IsRenderRequired()
{
  bool required = false;

  switch( mState )
  {
    case RENDER_CONTINUOUSLY:
    {
      required = (mFrameCounter == 0);
      break;
    }
    case RENDER_ONCE_WAITING_FOR_RESOURCES:
    {
      required = true;
      break;
    }
    default:
    {
      required = false;
      break;
    }
  }

  TASK_LOG_FMT( Debug::General, " State:%s = %s\n", STATE_STRING(mState), required?"T":"F" );

  return required;
}

// Called every frame regardless of whether render was required.
// If render was not required, ignore resourcesFinished.
void RenderTask::UpdateState()
{
  TASK_LOG_FMT( Debug::General, "FC:%d State:%s RR:%d\n", mFrameCounter, STATE_STRING(mState), mRefreshRate );

  switch( mState )
  {
    case RENDER_CONTINUOUSLY:
    {
      if( mRefreshRate != Dali::RenderTask::REFRESH_ALWAYS )
      {
        if( mFrameCounter == 0 )
        {
          ++mFrameCounter; // Only start skipping frames when resources are loaded
        }
        else // Continue counting to skip frames
        {
          ++mFrameCounter;
          if( mFrameCounter >= mRefreshRate )
          {
            mFrameCounter = 0;
          }
        }
      }
      // else do nothing
    }
    break;

    case RENDER_ONCE_WAITING_FOR_RESOURCES:
    {
      mState = RENDERED_ONCE;
    }
    break;

    case RENDERED_ONCE:
    {
      mWaitingToRender = true;
      mNotifyTrigger = false;
      if( mFrameBuffer )
      {
        if( !mRenderSyncTracker || (mRenderSyncTracker && mRenderSyncTracker->IsSynced() ))
        {
          mWaitingToRender = false;
          mNotifyTrigger = true;
        }
      }
      else
      {
        mWaitingToRender = false;
        mNotifyTrigger = true;
      }
    }

    break;

    default:
      break;
  }

  TASK_LOG_FMT( Debug::General, " EXIT FC:%d State:%s Notify:%s\n", mFrameCounter, STATE_STRING(mState), mNotifyTrigger?"T":"F");
}

bool RenderTask::IsWaitingToRender()
{
  TASK_LOG_FMT(Debug::Verbose, " State:%s waiting:%s \n", STATE_STRING(mState), mWaitingToRender?"T":"F");
  return mWaitingToRender;
}

bool RenderTask::HasRendered()
{
  bool notify = false;
  if( mNotifyTrigger == true )
  {
    ++mRenderedOnceCounter;
    mState = RENDERED_ONCE_AND_NOTIFIED;
    mNotifyTrigger = false;
    notify = true;
  }

  TASK_LOG_FMT(Debug::Verbose, " State:%s hasRendered:%s \n", STATE_STRING(mState), notify?"T":"F");
  return notify;
}

uint32_t RenderTask::GetRenderedOnceCounter() const
{
  return mRenderedOnceCounter;
}

const Matrix& RenderTask::GetViewMatrix( BufferIndex bufferIndex ) const
{
  DALI_ASSERT_DEBUG( nullptr != mCamera );

  return mCamera->GetViewMatrix( bufferIndex );
}

SceneGraph::Camera& RenderTask::GetCamera() const
{
  DALI_ASSERT_DEBUG( nullptr != mCamera );
  return *mCamera;
}

const Matrix& RenderTask::GetProjectionMatrix( BufferIndex bufferIndex ) const
{
  DALI_ASSERT_DEBUG( nullptr != mCamera );

  return mCamera->GetProjectionMatrix( bufferIndex );
}

void RenderTask::PrepareRenderInstruction( RenderInstruction& instruction, BufferIndex updateBufferIndex )
{
  DALI_ASSERT_DEBUG( nullptr != mCamera );

  TASK_LOG(Debug::General);

  Viewport viewport;
  bool viewportSet = QueryViewport( updateBufferIndex, viewport );

  instruction.Reset( mCamera,
                     GetFrameBuffer(),
                     viewportSet ? &viewport : nullptr,
                     mClearEnabled ? &GetClearColor( updateBufferIndex ) : nullptr );

  if( mRequiresSync &&
      mRefreshRate == Dali::RenderTask::REFRESH_ONCE )
  {
    // create tracker if one doesn't yet exist.
    if( !mRenderSyncTracker )
    {
      mRenderSyncTracker = new Render::RenderTracker();
      mRenderMessageDispatcher->AddRenderTracker( *mRenderSyncTracker );
    }
    instruction.mRenderTracker = mRenderSyncTracker;
  }
  else
  {
    // no sync needed, texture FBOs are "ready" the same frame they are rendered to
    instruction.mRenderTracker = nullptr;
  }
}

bool RenderTask::ViewMatrixUpdated()
{
  bool retval = false;
  if( mCamera )
  {
    retval = mCamera->ViewMatrixUpdated();
  }
  return retval;
}

void RenderTask::SetViewportPosition( BufferIndex updateBufferIndex, const Vector2& value )
{
  mViewportPosition.Set( updateBufferIndex, value );
}

const Vector2& RenderTask::GetViewportPosition( BufferIndex bufferIndex ) const
{
  return mViewportPosition[bufferIndex];
}

void RenderTask::BakeViewportPosition( BufferIndex updateBufferIndex, const Vector2& value )
{
  mViewportPosition.Bake( updateBufferIndex, value );
}

void RenderTask::SetViewportSize( BufferIndex updateBufferIndex, const Vector2& value )
{
  mViewportSize.Set( updateBufferIndex, value );
}

const Vector2& RenderTask::GetViewportSize( BufferIndex bufferIndex ) const
{
  return mViewportSize[bufferIndex];
}

void RenderTask::BakeViewportSize( BufferIndex updateBufferIndex, const Vector2& value )
{
  mViewportSize.Bake( updateBufferIndex, value );
}

bool RenderTask::GetViewportEnabled( BufferIndex bufferIndex ) const
{
  if(fabsf(mViewportPosition[bufferIndex].x) > Math::MACHINE_EPSILON_1 ||
     fabsf(mViewportPosition[bufferIndex].y) > Math::MACHINE_EPSILON_1 ||
     fabsf(mViewportSize[bufferIndex].width) > Math::MACHINE_EPSILON_1 ||
     fabsf(mViewportSize[bufferIndex].height) > Math::MACHINE_EPSILON_1)
  {
    return true;
  }

  return false;
}

void RenderTask::SetSyncRequired( bool requiresSync )
{
  mRequiresSync = requiresSync;
}

void RenderTask::PropertyOwnerConnected( PropertyOwner& owner )
{
  // check if we've gone from inactive to active
  SetActiveStatus();
}

void RenderTask::PropertyOwnerDisconnected( BufferIndex /*updateBufferIndex*/, PropertyOwner& owner )
{
  mActive = false; // if either source or camera disconnected, we're no longer active
}

void RenderTask::PropertyOwnerDestroyed( PropertyOwner& owner )
{
  if( static_cast<PropertyOwner*>( mSourceNode ) == &owner )
  {
    mSourceNode = nullptr;
  }
  else if( static_cast<PropertyOwner*>( mCameraNode ) == &owner )
  {
    mCameraNode = nullptr;
  }
}

RenderTask::RenderTask()
: mViewportPosition( Vector2::ZERO),
  mViewportSize( Vector2::ZERO),
  mClearColor( Dali::RenderTask::DEFAULT_CLEAR_COLOR ),
  mRenderMessageDispatcher( nullptr ),
  mRenderSyncTracker( nullptr ),
  mSourceNode( nullptr ),
  mCameraNode( nullptr ),
  mCamera( nullptr ),
  mFrameBuffer(0),
  mRefreshRate( Dali::RenderTask::DEFAULT_REFRESH_RATE ),
  mFrameCounter( 0u ),
  mRenderedOnceCounter( 0u ),
  mState( (Dali::RenderTask::DEFAULT_REFRESH_RATE == Dali::RenderTask::REFRESH_ALWAYS)
          ? RENDER_CONTINUOUSLY
          : RENDER_ONCE_WAITING_FOR_RESOURCES ),
  mRequiresSync( false ),
  mActive( false ),
  mWaitingToRender( false ),
  mNotifyTrigger( false ),
  mExclusive( Dali::RenderTask::DEFAULT_EXCLUSIVE ),
  mClearEnabled( Dali::RenderTask::DEFAULT_CLEAR_ENABLED ),
  mCullMode( Dali::RenderTask::DEFAULT_CULL_MODE )
{
}

void RenderTask::SetActiveStatus()
{
  // must have a source and camera both connected to scene
  mActive = ( mSourceNode && mSourceNode->ConnectedToScene() &&
              mCameraNode && mCameraNode->ConnectedToScene() && mCamera );
  TASK_LOG_FMT( Debug::General, " Source node(%x) active %d.  Frame counter: %d\n", mSourceNode, mSourceNode && mSourceNode->ConnectedToScene(), mFrameCounter );
  TASK_LOG_FMT( Debug::General, " Camera node(%x) active %d\n", mCameraNode, mCameraNode && mCameraNode->ConnectedToScene() );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
