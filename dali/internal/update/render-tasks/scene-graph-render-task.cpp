/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/internal/render/common/render-instruction.h>
#include <dali/internal/render/common/render-tracker.h>
#include <dali/internal/update/controllers/render-manager-dispatcher.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/public-api/math/matrix.h>

#include <dali/internal/update/common/property-resetter.h>
#include <dali/internal/update/common/resetter-manager.h> ///< For AddInitializeResetter

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
  Destroy();
}

void RenderTask::Initialize(ResetterManager& resetterManager, RenderManagerDispatcher& renderManagerDispatcher)
{
  mResetterManager         = &resetterManager;
  mRenderManagerDispatcher = &renderManagerDispatcher;
}

void RenderTask::SetSourceNode(Node* node)
{
  // Stop observing the old node (if we were)
  if(mSourceNode)
  {
    mSourceNode->RemoveObserver(*this);
    mSourceNode->RemoveExclusiveRenderTask(this);
  }

  mSourceNode = node;

  if(mSourceNode)
  {
    mSourceNode->AddObserver(*this);
    if(mExclusive)
    {
      mSourceNode->AddExclusiveRenderTask(this);
    }
  }
  SetActiveStatus();
}

Node* RenderTask::GetSourceNode() const
{
  return mSourceNode;
}

void RenderTask::SetStopperNode(Node* node)
{
  if(mStopperNode)
  {
    mStopperNode->RemoveObserver(*this);
  }

  mStopperNode = node;

  if(mStopperNode)
  {
    mStopperNode->AddObserver(*this);
  }
}

Node* RenderTask::GetStopperNode() const
{
  return mStopperNode;
}

void RenderTask::SetViewportGuideNode(Node* node)
{
  if(mViewportGuideNode)
  {
    mViewportGuideNode->RemoveObserver(*this);
  }

  mViewportGuideNode = node;

  if(mViewportGuideNode)
  {
    mViewportGuideNode->AddObserver(*this);
  }
}

Node* RenderTask::GetViewportGuideNode() const
{
  return mViewportGuideNode;
}

void RenderTask::SetRenderedScaleFactor(const Vector2& scaleFactor)
{
  mRenderedScaleFactor = scaleFactor;
}

Vector2 RenderTask::GetRenderedScaleFactor() const
{
  return mRenderedScaleFactor;
}

void RenderTask::SetExclusive(bool exclusive)
{
  mExclusive = exclusive;

  if(mSourceNode)
  {
    if(mExclusive)
    {
      mSourceNode->AddExclusiveRenderTask(this);
    }
    else
    {
      mSourceNode->RemoveExclusiveRenderTask(this);
    }
  }
}

bool RenderTask::IsExclusive() const
{
  return mExclusive;
}

void RenderTask::SetCamera(Camera* cameraNode)
{
  if(mCameraNode)
  {
    mCameraNode->RemoveObserver(*this);
  }

  mCameraNode = cameraNode;

  if(mCameraNode)
  {
    mCameraNode->AddObserver(*this);
  }
  SetActiveStatus();
}

void RenderTask::SetFrameBuffer(Render::FrameBuffer* frameBuffer)
{
  mFrameBuffer = frameBuffer;
}

Render::FrameBuffer* RenderTask::GetFrameBuffer()
{
  return mFrameBuffer;
}

bool RenderTask::QueryViewport(BufferIndex bufferIndex, Viewport& viewport) const
{
  if(!GetViewportEnabled(bufferIndex))
  {
    return false;
  }

  viewport.x      = static_cast<int>(mViewportPosition[bufferIndex].x);  // truncated
  viewport.y      = static_cast<int>(mViewportPosition[bufferIndex].y);  // truncated
  viewport.width  = static_cast<int>(mViewportSize[bufferIndex].width);  // truncated
  viewport.height = static_cast<int>(mViewportSize[bufferIndex].height); // truncated

  return true;
}

const Vector4& RenderTask::GetClearColor(BufferIndex bufferIndex) const
{
  return mClearColor[bufferIndex];
}

void RenderTask::SetClearEnabled(bool enabled)
{
  mClearEnabled = enabled;
}

bool RenderTask::GetClearEnabled() const
{
  return mClearEnabled;
}

void RenderTask::SetCullMode(bool mode)
{
  mCullMode = mode;
}

bool RenderTask::GetCullMode() const
{
  return mCullMode;
}

void RenderTask::SetRefreshRate(uint32_t refreshRate)
{
  DALI_LOG_TRACE_METHOD_FMT(gRenderTaskLogFilter, "this:%p RefreshRate:%d\n", this, refreshRate);

  mRefreshRate = refreshRate;

  if(mRefreshRate > 0)
  {
    mState = RENDER_CONTINUOUSLY;
  }
  else
  {
    mState           = RENDER_ONCE_WAITING_FOR_RESOURCES;
    mWaitingToRender = true;
    mNotifyTrigger   = false;
  }

  mFrameCounter = 0u;
}

uint32_t RenderTask::GetRefreshRate() const
{
  return mRefreshRate;
}

bool RenderTask::ReadyToRender(BufferIndex updateBufferIndex)
{
  TASK_LOG_FMT(Debug::General, " Active(ReadyToRender):%s\n", mActive ? "T" : "F");
  return mActive;
}

bool RenderTask::IsRenderRequired()
{
  bool required = false;

  switch(mState)
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

  TASK_LOG_FMT(Debug::General, " State:%s = %s\n", STATE_STRING(mState), required ? "T" : "F");

  return required;
}

// Called every frame regardless of whether render was required.
// If render was not required, ignore resourcesFinished.
void RenderTask::UpdateState()
{
  TASK_LOG_FMT(Debug::General, "FC:%d State:%s RR:%d\n", mFrameCounter, STATE_STRING(mState), mRefreshRate);

  switch(mState)
  {
    case RENDER_CONTINUOUSLY:
    {
      if(mRefreshRate != Dali::RenderTask::REFRESH_ALWAYS)
      {
        if(mFrameCounter == 0)
        {
          ++mFrameCounter; // Only start skipping frames when resources are loaded
        }
        else // Continue counting to skip frames
        {
          ++mFrameCounter;
          if(mFrameCounter >= mRefreshRate)
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
      mNotifyTrigger   = false;
      if(mFrameBuffer)
      {
        if(!mRenderSyncTracker || (mRenderSyncTracker && mRenderSyncTracker->IsSynced()))
        {
          mWaitingToRender = false;
          mNotifyTrigger   = true;
        }
      }
      else
      {
        mWaitingToRender = false;
        mNotifyTrigger   = true;
      }
    }

    break;

    default:
      break;
  }

  TASK_LOG_FMT(Debug::General, " EXIT FC:%d State:%s Notify:%s\n", mFrameCounter, STATE_STRING(mState), mNotifyTrigger ? "T" : "F");
}

bool RenderTask::IsWaitingToRender()
{
  TASK_LOG_FMT(Debug::Verbose, " State:%s waiting:%s \n", STATE_STRING(mState), mWaitingToRender ? "T" : "F");
  return mWaitingToRender;
}

bool RenderTask::HasRendered()
{
  bool notify = false;
  if(mNotifyTrigger == true)
  {
    ++mRenderedOnceCounter;
    mState         = RENDERED_ONCE_AND_NOTIFIED;
    mNotifyTrigger = false;
    notify         = true;
  }

  TASK_LOG_FMT(Debug::Verbose, " State:%s hasRendered:%s \n", STATE_STRING(mState), notify ? "T" : "F");
  return notify;
}

uint32_t RenderTask::GetRenderedOnceCounter() const
{
  return mRenderedOnceCounter;
}

const Matrix& RenderTask::GetViewMatrix(BufferIndex bufferIndex) const
{
  DALI_ASSERT_DEBUG(nullptr != mCameraNode);

  return mCameraNode->GetViewMatrix(bufferIndex);
}

const SceneGraph::Camera& RenderTask::GetCamera() const
{
  DALI_ASSERT_DEBUG(nullptr != mCameraNode);
  return *mCameraNode;
}

const Matrix& RenderTask::GetProjectionMatrix(BufferIndex bufferIndex) const
{
  DALI_ASSERT_DEBUG(nullptr != mCameraNode);

  return mCameraNode->GetProjectionMatrix(bufferIndex);
}

RenderInstruction& RenderTask::PrepareRenderInstruction(BufferIndex updateBufferIndex)
{
  DALI_ASSERT_DEBUG(nullptr != mCameraNode);

  TASK_LOG(Debug::General);

  Viewport viewport;
  bool     viewportSet = QueryViewport(updateBufferIndex, viewport);

  mRenderInstruction[updateBufferIndex].Reset(mCameraNode,
                                              GetFrameBuffer(),
                                              viewportSet ? &viewport : nullptr,
                                              mClearEnabled ? &GetClearColor(updateBufferIndex) : nullptr,
                                              mRenderedScaleFactor);

  if(mRequiresSync &&
     mRefreshRate == Dali::RenderTask::REFRESH_ONCE)
  {
    // create tracker if one doesn't yet exist.
    if(!mRenderSyncTracker)
    {
      DALI_ASSERT_ALWAYS(mRenderManagerDispatcher && "We don't allow to call PrepareRenderInstruction after Graphics context destroyed!");

      mRenderSyncTracker = new Render::RenderTracker();
      mRenderManagerDispatcher->AddRenderTracker(*mRenderSyncTracker);
    }
    mRenderInstruction[updateBufferIndex].mRenderTracker = mRenderSyncTracker;
  }
  else
  {
    // no sync needed, texture FBOs are "ready" the same frame they are rendered to
    mRenderInstruction[updateBufferIndex].mRenderTracker = nullptr;
  }

  mRenderInstruction[updateBufferIndex].mRenderPassTag = mRenderPassTag;
  return mRenderInstruction[updateBufferIndex];
}

bool RenderTask::ViewMatrixUpdated()
{
  bool retval = false;
  if(mCameraNode)
  {
    retval = mCameraNode->ViewMatrixUpdated();
  }
  return retval;
}

void RenderTask::UpdateViewport(BufferIndex updateBufferIndex, Vector2 sceneSize, Vector3 cameraPosition)
{
  if(GetViewportGuideNode() && GetViewportGuideNode()->ConnectedToScene())
  {
    Vector3 worldPosition = GetViewportGuideNode()->GetWorldPosition(updateBufferIndex);
    worldPosition -= cameraPosition;

    Vector3 nodeSize = GetViewportGuideNode()->GetSize(updateBufferIndex) * GetViewportGuideNode()->GetWorldScale(updateBufferIndex);
    Vector2 halfSceneSize(sceneSize.width * 0.5f, sceneSize.height * 0.5f); // World position origin is center of scene
    Vector3 halfNodeSize(nodeSize * 0.5f);
    Vector2 screenPosition(halfSceneSize.width + worldPosition.x - halfNodeSize.x,
                           halfSceneSize.height + worldPosition.y - halfNodeSize.y);

    /**
     * This is an implicit constraint - the properties will be dirty until the node
     * is removed. (RenderTask::Impl manages this)
     *
     * TODO : Need to make Resseter for these properties.
     */
    mViewportPosition.Set(updateBufferIndex, screenPosition);
    mViewportSize.Set(updateBufferIndex, Vector2(nodeSize));
  }
}

const Vector2& RenderTask::GetViewportPosition(BufferIndex bufferIndex) const
{
  return mViewportPosition[bufferIndex];
}

const Vector2& RenderTask::GetViewportSize(BufferIndex bufferIndex) const
{
  return mViewportSize[bufferIndex];
}

bool RenderTask::GetViewportEnabled(BufferIndex bufferIndex) const
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

void RenderTask::SetSyncRequired(bool requiresSync)
{
  mRequiresSync = requiresSync;
}

void RenderTask::SetRenderPassTag(uint32_t renderPassTag)
{
  mRenderPassTag = renderPassTag;
}

void RenderTask::Destroy()
{
  TASK_LOG(Debug::General);

  if(mSourceNode)
  {
    mSourceNode->RemoveObserver(*this);
    if(mExclusive)
    {
      mSourceNode->RemoveExclusiveRenderTask(this);
    }
    mSourceNode = nullptr;
  }
  if(mStopperNode)
  {
    mStopperNode->RemoveObserver(*this);
    mStopperNode = nullptr;
  }
  if(mCameraNode)
  {
    mCameraNode->RemoveObserver(*this);
    mCameraNode = nullptr;
  }
  if(mViewportGuideNode)
  {
    mViewportGuideNode->RemoveObserver(*this);
    mViewportGuideNode = nullptr;
  }
  if(mRenderSyncTracker)
  {
    if(DALI_LIKELY(mRenderManagerDispatcher))
    {
      mRenderManagerDispatcher->RemoveRenderTracker(*mRenderSyncTracker);
    }
    mRenderSyncTracker = nullptr;
  }
}

void RenderTask::ContextDestroyed()
{
  // Note : We don't need to call RemoveRenderTracker in this case.
  // (Since RenderManager::ContextDestroyed will delete it.)
  mRenderSyncTracker = nullptr;

  mRenderManagerDispatcher = nullptr;

  mRenderInstruction[0].ContextDestroyed();
  mRenderInstruction[1].ContextDestroyed();
}

void RenderTask::PropertyOwnerConnected(PropertyOwner& owner)
{
  if(&owner == static_cast<PropertyOwner*>(mSourceNode) || &owner == static_cast<PropertyOwner*>(mCameraNode))
  {
    // check if we've gone from inactive to active
    SetActiveStatus();
  }
}

PropertyOwner::Observer::NotifyReturnType RenderTask::PropertyOwnerDisconnected(BufferIndex /*updateBufferIndex*/, PropertyOwner& owner)
{
  if(&owner == static_cast<PropertyOwner*>(mSourceNode) || &owner == static_cast<PropertyOwner*>(mCameraNode))
  {
    mActive = false; // if either source or camera disconnected, we're no longer active
  }
  return PropertyOwner::Observer::NotifyReturnType::KEEP_OBSERVING;
}

void RenderTask::PropertyOwnerDestroyed(PropertyOwner& owner)
{
  if(static_cast<PropertyOwner*>(mSourceNode) == &owner)
  {
    mSourceNode = nullptr;

    mActive = false; // if either source or camera destroyed, we're no longer active
  }
  if(static_cast<PropertyOwner*>(mCameraNode) == &owner)
  {
    mCameraNode = nullptr;

    mActive = false; // if either source or camera destroyed, we're no longer active
  }
  if(static_cast<PropertyOwner*>(mStopperNode) == &owner)
  {
    mStopperNode = nullptr;
  }
  if(static_cast<PropertyOwner*>(mViewportGuideNode) == &owner)
  {
    mViewportGuideNode = nullptr;

    if(DALI_LIKELY(!Dali::Stage::IsShuttingDown()))
    {
      // TODO : Until SG::RenderTask Resseter preopared, just call this internal API without dirty flag down.
      mViewportPosition.ResetToBaseValueInternal(0);
      mViewportPosition.ResetToBaseValueInternal(1);
      mViewportSize.ResetToBaseValueInternal(0);
      mViewportSize.ResetToBaseValueInternal(1);
    }
  }
}

void RenderTask::AddInitializeResetter(ResetterManager& manager) const
{
  OwnerPointer<SceneGraph::PropertyResetterBase> resetterViewportPosition = SceneGraph::BakerResetter::New(*this, mViewportPosition, SceneGraph::BakerResetter::Lifetime::BAKE);
  OwnerPointer<SceneGraph::PropertyResetterBase> resetterViewportSize     = SceneGraph::BakerResetter::New(*this, mViewportSize, SceneGraph::BakerResetter::Lifetime::BAKE);
  OwnerPointer<SceneGraph::PropertyResetterBase> resetterClearColor       = SceneGraph::BakerResetter::New(*this, mClearColor, SceneGraph::BakerResetter::Lifetime::BAKE);
  manager.AddPropertyResetter(resetterViewportPosition);
  manager.AddPropertyResetter(resetterViewportSize);
  manager.AddPropertyResetter(resetterClearColor);
}

RenderTask::RenderTask()
: mViewportPosition(Vector2::ZERO),
  mViewportSize(Vector2::ZERO),
  mClearColor(Dali::RenderTask::DEFAULT_CLEAR_COLOR),
  mResetterManager(nullptr),
  mRenderManagerDispatcher(nullptr),
  mRenderSyncTracker(nullptr),
  mSourceNode(nullptr),
  mStopperNode(nullptr),
  mCameraNode(nullptr),
  mViewportGuideNode(nullptr),
  mFrameBuffer(nullptr),
  mRefreshRate(Dali::RenderTask::DEFAULT_REFRESH_RATE),
  mFrameCounter(0u),
  mRenderedOnceCounter(0u),
  mRenderedScaleFactor(Vector2::ONE),
  mState((Dali::RenderTask::DEFAULT_REFRESH_RATE == Dali::RenderTask::REFRESH_ALWAYS)
           ? RENDER_CONTINUOUSLY
           : RENDER_ONCE_WAITING_FOR_RESOURCES),
  mRequiresSync(false),
  mActive(false),
  mWaitingToRender(false),
  mNotifyTrigger(false),
  mExclusive(Dali::RenderTask::DEFAULT_EXCLUSIVE),
  mClearEnabled(Dali::RenderTask::DEFAULT_CLEAR_ENABLED),
  mCullMode(Dali::RenderTask::DEFAULT_CULL_MODE)
{
}

void RenderTask::SetActiveStatus()
{
  bool oldActive = mActive;

  // must have a source and camera both connected to scene
  mActive = (mSourceNode && mSourceNode->ConnectedToScene() &&
             mCameraNode && mCameraNode->ConnectedToScene());
  TASK_LOG_FMT(Debug::General, " Source node(%x) active %d.  Frame counter: %d\n", mSourceNode, mSourceNode && mSourceNode->ConnectedToScene(), mFrameCounter);
  TASK_LOG_FMT(Debug::General, " Camera node(%x) active %d\n", mCameraNode, mCameraNode && mCameraNode->ConnectedToScene());

  if(!oldActive && mActive)
  {
    // Send resetter only if newly activated
    AddInitializeResetter(*mResetterManager);
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
