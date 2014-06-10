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

// CLASS HEADER
#include <dali/internal/update/render-tasks/scene-graph-render-task.h>

// INTERNAL INCLUDES
#include <dali/public-api/math/matrix.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/internal/update/resources/complete-status-manager.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/node-attachments/scene-graph-camera-attachment.h>
#include <dali/internal/render/common/render-instruction.h>

#include <dali/internal/update/render-tasks/scene-graph-render-task-debug.h>

namespace
{
const unsigned int NUM_FRAMES_BEFORE_NOTIFY_FINISHED(3);
const unsigned int COUNTDOWN_COMPLETE(-1);
} // anonymous namespace

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
  if( mFrameBufferResourceId )
  {
    mCompleteStatusManager->StopTrackingResource( mFrameBufferResourceId );
  }
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

void RenderTask::SetCameraNode( Node* cameraNode )
{
  // if camera changed
  if( cameraNode != mCameraNode )
  {
    if( cameraNode )
    {
      // get attachment. when camera node is changed we will get a message from event thread object
      // so no need to observe the node here
      mCameraAttachment = dynamic_cast< CameraAttachment* >( &cameraNode->GetAttachment() );
      DALI_ASSERT_DEBUG( mCameraAttachment && "Camera without attachment" );
    }
    mCameraNode = cameraNode;
  }
}

Node* RenderTask::GetCameraNode() const
{
  return mCameraNode;
}

void RenderTask::SetFrameBufferId( unsigned int resourceId )
{
  if ( mFrameBufferResourceId != resourceId )
  {
    DALI_ASSERT_DEBUG(mCompleteStatusManager && "Complete status tracker is null");
    if( mCompleteStatusManager )
    {
      if( resourceId && mState == RENDER_ONCE_WAITING_FOR_RESOURCES )
      {
        mCompleteStatusManager->TrackResource( resourceId );
      }

      if( mFrameBufferResourceId )
      {
        mCompleteStatusManager->StopTrackingResource( mFrameBufferResourceId );
      }
    }

    mFrameBufferResourceId = resourceId;
  }
}

unsigned int RenderTask::GetFrameBufferId() const
{
  return mFrameBufferResourceId;
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

void RenderTask::SetRefreshRate( unsigned int refreshRate )
{
  DALI_LOG_TRACE_METHOD_FMT(gRenderTaskLogFilter, "this:%p RefreshRate:%d\n", this, refreshRate);

  mRefreshRate = refreshRate;

  if( mRefreshRate > 0 )
  {
    mState = RENDER_CONTINUOUSLY;

    if( mFrameBufferResourceId )
    {
      // Don't need tracking
      DALI_ASSERT_DEBUG(mCompleteStatusManager && "Ready state tracker is null");
      if( mCompleteStatusManager != NULL )
      {
        mCompleteStatusManager->StopTrackingResource( mFrameBufferResourceId );
      }
    }
  }
  else
  {
    mState = RENDER_ONCE_WAITING_FOR_RESOURCES;
    mWaitingToRender = true;
    mNotifyTrigger = false;

    if( mFrameBufferResourceId )
    {
      DALI_ASSERT_DEBUG(mCompleteStatusManager && "Ready state tracker is null");
      if( mCompleteStatusManager != NULL )
      {
        mCompleteStatusManager->TrackResource( mFrameBufferResourceId );
      }
    }
  }

  mFrameCounter = 0u;
}

unsigned int RenderTask::GetRefreshRate() const
{
  return mRefreshRate;
}

bool RenderTask::ReadyToRender( BufferIndex updateBufferIndex )
{
  if ( NULL == mSourceNode ||
       ( !mSourceNode->IsRoot() && NULL == mSourceNode->GetParent() ) )
  {
    TASK_LOG_FMT(Debug::General, " =F  No source actor  FC:%d\n", mFrameCounter );

    // Source node is missing or disconnected
    return false;
  }

  // Check if the source node (root actor) and all its ancestors are visible.
  if( !mSourceNode->IsFullyVisible( updateBufferIndex ) )
  {
    TASK_LOG_FMT(Debug::General, " =F  No visible source  FC:%d\n", mFrameCounter );
    return false;
  }

  if ( NULL == mCameraNode ||
       NULL == mCameraNode->GetParent() ||
       !mCameraNode->HasAttachment() )
  {
    // Camera node is missing or disconnected
    TASK_LOG_FMT(Debug::General, " =F  No Camera  FC:%d\n", mFrameCounter );

    return false;
  }

  TASK_LOG_FMT(Debug::General, " =T (FBO ID:%d) FC:%d\n", mFrameBufferResourceId , mFrameCounter );
  return true;
}

bool RenderTask::IsRenderRequired()
{
  bool required = false;

  switch( mState )
  {
    case RENDER_CONTINUOUSLY:
      required = (mFrameCounter == 0);
      break;

    case RENDER_ONCE_WAITING_FOR_RESOURCES:
      required = true;
      break;

    default:
      required = false;
      break;
  }

  TASK_LOG_FMT( Debug::General, " State:%s = %s\n", STATE_STRING(mState), required?"T":"F" );

  return required;
}

// Called every frame regardless of whether render was required.
// If render was not required, ignore resourcesFinished.
void RenderTask::UpdateState(bool resourcesFinished)
{
  TASK_LOG_FMT( Debug::General, "(resourcesFinished:%s)  FC:%d State:%s RR:%d\n", resourcesFinished?"T":"F", mFrameCounter, STATE_STRING(mState), mRefreshRate );

  switch( mState )
  {
    case RENDER_CONTINUOUSLY:
    {
      if( mRefreshRate != Dali::RenderTask::REFRESH_ALWAYS )
      {
        if( mFrameCounter == 0 )
        {
          if( resourcesFinished )
          {
            ++mFrameCounter; // Only start missing frames when resources are loaded
          }
        }
        else // Continue counting to miss frames
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
      if( resourcesFinished )
      {
        mState = RENDERED_ONCE;
      }
    }
    break;

    case RENDERED_ONCE:
    {
      if( mFrameBufferResourceId > 0 )
      {
        // Query if the framebuffer is complete:
        DALI_ASSERT_DEBUG(mCompleteStatusManager && "Complete status tracker is null");
        if( mCompleteStatusManager != NULL &&
            CompleteStatusManager::COMPLETE == mCompleteStatusManager->GetStatus( mFrameBufferResourceId ) )
        {
          mWaitingToRender = false;
          mNotifyTrigger = true;
        }
        else
        {
          mWaitingToRender = true;
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

unsigned int RenderTask::GetRenderedOnceCounter() const
{
  return mRenderedOnceCounter;
}


const Matrix& RenderTask::GetViewMatrix( BufferIndex bufferIndex ) const
{
  DALI_ASSERT_DEBUG( NULL != mCameraAttachment );

  return mCameraAttachment->GetViewMatrix( bufferIndex );
}

const Matrix& RenderTask::GetProjectionMatrix( BufferIndex bufferIndex ) const
{
  DALI_ASSERT_DEBUG( NULL != mCameraAttachment );

  return mCameraAttachment->GetProjectionMatrix( bufferIndex );
}

void RenderTask::PrepareRenderInstruction( RenderInstruction& instruction, BufferIndex updateBufferIndex )
{
  TASK_LOG(Debug::General);

  Viewport viewport;
  bool viewportSet = QueryViewport( updateBufferIndex, viewport );

  instruction.Reset( &GetViewMatrix( updateBufferIndex ),
                     &GetProjectionMatrix( updateBufferIndex ),
                     GetFrameBufferId(),
                     viewportSet ? &viewport : NULL,
                     mClearEnabled ? &GetClearColor( updateBufferIndex ) : NULL );
}

bool RenderTask::ViewMatrixUpdated()
{
  bool retval = false;
  if( mCameraAttachment )
  {
    retval = mCameraAttachment->ViewMatrixUpdated();
  }
  return retval;
}

void RenderTask::SetCompleteStatusManager(CompleteStatusManager* completeStatusManager)
{
  mCompleteStatusManager = completeStatusManager;
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
  mCompleteStatusManager( NULL ),
  mSourceNode( NULL ),
  mCameraNode( NULL ),
  mCameraAttachment( NULL ),
  mFrameBufferResourceId( 0 ),
  mWaitingToRender( false ),
  mNotifyTrigger( false ),
  mExclusive( Dali::RenderTask::DEFAULT_EXCLUSIVE ),
  mClearEnabled( Dali::RenderTask::DEFAULT_CLEAR_ENABLED ),
  mRenderTarget( NULL ),
  mState( (Dali::RenderTask::DEFAULT_REFRESH_RATE == Dali::RenderTask::REFRESH_ALWAYS)
          ? RENDER_CONTINUOUSLY
          : RENDER_ONCE_WAITING_FOR_RESOURCES ),
  mRefreshRate( Dali::RenderTask::DEFAULT_REFRESH_RATE ),
  mFrameCounter( 0u ),
  mRenderedOnceCounter( 0u )
{
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
