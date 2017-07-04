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
  // Remove exclusive flag from source node
  if( mExclusive )
  {
    if( mSourceNode && (this == mSourceNode->GetExclusiveRenderTask() ) )
    {
      mSourceNode->SetExclusiveRenderTask( NULL );
    }
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
  // Remove exclusive flag from the old node, if necessary
  if ( mSourceNode &&
       this == mSourceNode->GetExclusiveRenderTask() )
  {
    mSourceNode->SetExclusiveRenderTask( NULL );
  }

  mSourceNode = node;

  if ( mSourceNode && mExclusive )
  {
    mSourceNode->SetExclusiveRenderTask( this );
  }
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
      mSourceNode->SetExclusiveRenderTask( NULL );
    }
  }
}

bool RenderTask::IsExclusive() const
{
  return mExclusive;
}

void RenderTask::SetCamera( Node* cameraNode, Camera* camera )
{
  mCameraNode = cameraNode;
  mCamera = camera;
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

  viewport.x = mViewportPosition[bufferIndex].x;
  viewport.y = mViewportPosition[bufferIndex].y;
  viewport.width = mViewportSize[bufferIndex].width;
  viewport.height = mViewportSize[bufferIndex].height;

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

void RenderTask::SetRefreshRate( unsigned int refreshRate )
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

unsigned int RenderTask::GetRefreshRate() const
{
  return mRefreshRate;
}

bool RenderTask::ReadyToRender( BufferIndex updateBufferIndex )
{
  // If the source node of the render task is invisible we should still render
  // We want the render task to complete and possible clear colors to happen

  // Check the source node.
  if( NULL == mSourceNode ||
      ( !mSourceNode->IsRoot() && NULL == mSourceNode->GetParent() ) )
  {
    TASK_LOG_FMT( Debug::General, " Source actor not on stage.  Frame counter: %d\n", mFrameCounter );

    // The source node is missing or disconnected.
    return false;
  }

  // Check camera node
  if( NULL == mCameraNode ||
      NULL == mCameraNode->GetParent() ||
      NULL == mCamera )
  {
    // The camera node is missing or disconnected.
    TASK_LOG_FMT(Debug::General, " =F  No Camera  FC:%d\n", mFrameCounter );
    return false;
  }

  return true;
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

unsigned int RenderTask::GetRenderedOnceCounter() const
{
  return mRenderedOnceCounter;
}


const Matrix& RenderTask::GetViewMatrix( BufferIndex bufferIndex ) const
{
  DALI_ASSERT_DEBUG( NULL != mCamera );

  return mCamera->GetViewMatrix( bufferIndex );
}

SceneGraph::Camera& RenderTask::GetCamera() const
{
  DALI_ASSERT_DEBUG( NULL != mCamera );
  return *mCamera;
}

const Matrix& RenderTask::GetProjectionMatrix( BufferIndex bufferIndex ) const
{
  DALI_ASSERT_DEBUG( NULL != mCamera );

  return mCamera->GetProjectionMatrix( bufferIndex );
}

void RenderTask::PrepareRenderInstruction( RenderInstruction& instruction, BufferIndex updateBufferIndex )
{
  DALI_ASSERT_DEBUG( NULL != mCamera );

  TASK_LOG(Debug::General);

  Viewport viewport;
  bool viewportSet = QueryViewport( updateBufferIndex, viewport );

  instruction.Reset( mCamera,
                     GetFrameBuffer(),
                     viewportSet ? &viewport : NULL,
                     mClearEnabled ? &GetClearColor( updateBufferIndex ) : NULL );

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
    instruction.mRenderTracker = NULL;
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

void RenderTask::ResetDefaultProperties( BufferIndex updateBufferIndex )
{
  // Reset default properties
  mViewportPosition.ResetToBaseValue( updateBufferIndex );
  mViewportSize.ResetToBaseValue( updateBufferIndex );
  mClearColor.ResetToBaseValue( updateBufferIndex );
}

RenderTask::RenderTask()
: mViewportPosition( Vector2::ZERO),
  mViewportSize( Vector2::ZERO),
  mClearColor( Dali::RenderTask::DEFAULT_CLEAR_COLOR ),
  mRenderMessageDispatcher( NULL ),
  mRenderSyncTracker( NULL ),
  mSourceNode( NULL ),
  mCameraNode( NULL ),
  mCamera( NULL ),
  mFrameBuffer(0),
  mWaitingToRender( false ),
  mNotifyTrigger( false ),
  mExclusive( Dali::RenderTask::DEFAULT_EXCLUSIVE ),
  mClearEnabled( Dali::RenderTask::DEFAULT_CLEAR_ENABLED ),
  mCullMode( Dali::RenderTask::DEFAULT_CULL_MODE ),
  mState( (Dali::RenderTask::DEFAULT_REFRESH_RATE == Dali::RenderTask::REFRESH_ALWAYS)
          ? RENDER_CONTINUOUSLY
          : RENDER_ONCE_WAITING_FOR_RESOURCES ),
  mRefreshRate( Dali::RenderTask::DEFAULT_REFRESH_RATE ),
  mFrameCounter( 0u ),
  mRenderedOnceCounter( 0u ),
  mRequiresSync( false )
{
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
