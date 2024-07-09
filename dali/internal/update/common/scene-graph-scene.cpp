/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
 */

// CLASS HEADER
#include <dali/internal/update/common/scene-graph-scene.h>

// INTERNAL INCLUDES
#include <dali/integration-api/core-enumerations.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task-list.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
Scene::Scene()
: mFrameRenderedCallbacks(),
  mFramePresentedCallbacks(),
  mSurfaceRect(),
  mSurfaceOrientation(0),
  mScreenOrientation(0),
  mSurfaceRectChangedCount(0u),
  mRotationCompletedAcknowledgement(false),
  mSkipRendering(false),
  mNeedFullUpdate(false),
  mPartialUpdateEnabled(true)
{
}

Scene::~Scene()
{
  mFrameRenderedCallbacks.clear();
  mFramePresentedCallbacks.clear();
}

void Scene::Initialize(Graphics::Controller& graphicsController, Integration::DepthBufferAvailable depthBufferAvailable, Integration::StencilBufferAvailable stencilBufferAvailable)
{
  mGraphicsController = &graphicsController;

  // Create the render target for the surface. It should already have been sent via message.
  mRenderTarget = graphicsController.CreateRenderTarget(mRenderTargetCreateInfo, std::move(mRenderTarget));

  // Create the render pass for the surface
  std::vector<Graphics::AttachmentDescription> attachmentDescriptions;

  // Default behaviour for color attachments is to CLEAR and STORE
  mClearValues.clear();
  mClearValues.emplace_back();

  // Assume single color attachment
  Graphics::AttachmentDescription desc{};
  desc.SetLoadOp(Graphics::AttachmentLoadOp::CLEAR);
  desc.SetStoreOp(Graphics::AttachmentStoreOp::STORE);
  attachmentDescriptions.push_back(desc);

  if(depthBufferAvailable == Integration::DepthBufferAvailable::TRUE ||
     stencilBufferAvailable == Integration::StencilBufferAvailable::TRUE)
  {
    // Depth
    desc.SetLoadOp(Graphics::AttachmentLoadOp::CLEAR);
    desc.SetStoreOp(Graphics::AttachmentStoreOp::STORE);

    // Stencil
    desc.SetStencilLoadOp(Graphics::AttachmentLoadOp::CLEAR);
    desc.SetStencilStoreOp(Graphics::AttachmentStoreOp::STORE);
    attachmentDescriptions.push_back(desc);

    mClearValues.emplace_back();
    mClearValues.back().depthStencil.depth   = 0;
    mClearValues.back().depthStencil.stencil = 0;
  }

  /* Defines 2 render passes, one to clear, the other not.
   * These are matched up to actual render pass objects in the backend.
   * (Expect that swapchains are created with matching load/store ops)
   */
  Graphics::RenderPassCreateInfo rpInfo{};
  rpInfo.SetAttachments(attachmentDescriptions);

  // Add default render pass (loadOp = clear)
  mRenderPass = graphicsController.CreateRenderPass(rpInfo, nullptr); // Warning: Shallow ptr

  desc.SetLoadOp(Graphics::AttachmentLoadOp::LOAD);
  attachmentDescriptions[0] = desc;
  if(attachmentDescriptions.size() > 1)
  {
    desc.SetLoadOp(Graphics::AttachmentLoadOp::LOAD);
    desc.SetStencilLoadOp(Graphics::AttachmentLoadOp::LOAD);
    attachmentDescriptions.back() = desc;
  }

  mRenderPassNoClear = graphicsController.CreateRenderPass(rpInfo, nullptr);
}

RenderInstructionContainer& Scene::GetRenderInstructions()
{
  return mInstructions;
}

void Scene::AddFrameRenderedCallback(CallbackBase* callback, int32_t frameId)
{
  mFrameRenderedCallbacks.push_back(std::make_pair(std::unique_ptr<CallbackBase>(callback), frameId));
}

void Scene::AddFramePresentedCallback(CallbackBase* callback, int32_t frameId)
{
  mFramePresentedCallbacks.push_back(std::make_pair(std::unique_ptr<CallbackBase>(callback), frameId));
}

void Scene::GetFrameRenderedCallback(Dali::Integration::Scene::FrameCallbackContainer& callbacks)
{
  // Transfer owership of the callbacks
  for(auto&& iter : mFrameRenderedCallbacks)
  {
    callbacks.push_back(std::make_pair(std::move(iter.first), iter.second));
  }

  mFrameRenderedCallbacks.clear();
}

void Scene::GetFramePresentedCallback(Dali::Integration::Scene::FrameCallbackContainer& callbacks)
{
  // Transfer owership of the callbacks
  for(auto&& iter : mFramePresentedCallbacks)
  {
    callbacks.push_back(std::make_pair(std::move(iter.first), iter.second));
  }

  mFramePresentedCallbacks.clear();
}

void Scene::SetSkipRendering(bool skip)
{
  mSkipRendering = skip;
}

bool Scene::IsRenderingSkipped() const
{
  return mSkipRendering;
}

void Scene::SetSurfaceRect(const Rect<int32_t>& rect)
{
  DALI_LOG_RELEASE_INFO("update surfce rect in scene-graph, from width[%d], height[%d], to width[%d], height[%d]. Changed count [%d]\n", mSurfaceRect.width, mSurfaceRect.height, rect.width, rect.height, mSurfaceRectChangedCount + 1u);

  mSurfaceRect = rect;
  ++mSurfaceRectChangedCount;

  if(mRoot)
  {
    mRoot->SetUpdated(true);
  }
}

const Rect<int32_t>& Scene::GetSurfaceRect() const
{
  return mSurfaceRect;
}

uint32_t Scene::GetSurfaceRectChangedCount()
{
  uint32_t surfaceRectChangedCount = mSurfaceRectChangedCount;
  mSurfaceRectChangedCount         = 0u;

  return surfaceRectChangedCount;
}

void Scene::SetSurfaceOrientations(int32_t windowOrientation, int32_t screenOrienation)
{
  DALI_LOG_RELEASE_INFO("update orientation in scene-graph, from surface [%d], screen[%d], to surface [%d], screen[%d]\n", mSurfaceOrientation, mScreenOrientation, windowOrientation, screenOrienation);

  mSurfaceOrientation = windowOrientation;
  mScreenOrientation  = screenOrienation;

  if(mRoot)
  {
    mRoot->SetUpdated(true);
  }
}

int32_t Scene::GetSurfaceOrientation() const
{
  return mSurfaceOrientation;
}

int32_t Scene::GetScreenOrientation() const
{
  return mScreenOrientation;
}

void Scene::SetRotationCompletedAcknowledgement()
{
  mRotationCompletedAcknowledgement = true;
}

bool Scene::IsRotationCompletedAcknowledgementSet()
{
  bool setRotationCompletedAcknowledgement = mRotationCompletedAcknowledgement;
  mRotationCompletedAcknowledgement        = false;
  return setRotationCompletedAcknowledgement;
}

Scene::ItemsDirtyRectsContainer& Scene::GetItemsDirtyRects()
{
  return mItemsDirtyRects;
}

void Scene::SetSurfaceRenderTargetCreateInfo(const Graphics::RenderTargetCreateInfo& renderTargetCreateInfo)
{
  if(mRenderTarget != nullptr &&
     mRenderTargetCreateInfo.surface != renderTargetCreateInfo.surface)
  {
    // Only recreate if the surface has changed.
    mRenderTargetCreateInfo = renderTargetCreateInfo;
    if(mGraphicsController) // shouldn't be null, as we can't have already set mRenderTarget unless graphics controller exists.
    {
      mRenderTarget = mGraphicsController->CreateRenderTarget(renderTargetCreateInfo, std::move(mRenderTarget));
    }
  }
  else
  {
    // 2nd Stage initialization happens in RenderManager, not UpdateManager, so is delayed.
    mRenderTargetCreateInfo = renderTargetCreateInfo;
  }
}

void Scene::KeepRendering(float durationSeconds)
{
  mKeepRenderingSeconds = std::max(mKeepRenderingSeconds, durationSeconds);
}

bool Scene::KeepRenderingCheck(float elapsedSeconds)
{
  if(mKeepRenderingSeconds > 0.0f)
  {
    mNeedFullUpdate       = true; // Full update if KeepRendering is required
    mKeepRenderingSeconds = std::max(0.0f, mKeepRenderingSeconds - elapsedSeconds);
    return true;
  }

  mNeedFullUpdate       = false;
  mKeepRenderingSeconds = 0.0f;
  return false;
}

void Scene::SetPartialUpdateEnabled(bool enabled)
{
  mPartialUpdateEnabled = enabled;
}

bool Scene::IsPartialUpdateEnabled() const
{
  return mPartialUpdateEnabled;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
