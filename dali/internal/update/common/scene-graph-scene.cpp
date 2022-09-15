/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
  mSkipRendering(false),
  mSurfaceRect(),
  mSurfaceOrientation(0),
  mSurfaceRectChanged(false),
  mRotationCompletedAcknowledgement(false)
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

  mRenderPassNoClear = graphicsController.CreateRenderPass(rpInfo, nullptr); // Warning: Shallow ptr
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
  mSurfaceRect        = rect;
  mSurfaceRectChanged = true;

  if(mRoot)
  {
    mRoot->SetUpdated(true);
  }
}

const Rect<int32_t>& Scene::GetSurfaceRect() const
{
  return mSurfaceRect;
}

void Scene::SetSurfaceOrientation(int32_t orientation)
{
  mSurfaceOrientation = orientation;

  if(mRoot)
  {
    mRoot->SetUpdated(true);
  }
}

int32_t Scene::GetSurfaceOrientation() const
{
  return mSurfaceOrientation;
}

bool Scene::IsSurfaceRectChanged()
{
  bool surfaceRectChanged = mSurfaceRectChanged;
  mSurfaceRectChanged     = false;

  return surfaceRectChanged;
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

std::vector<DirtyRect>& Scene::GetItemsDirtyRects()
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

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
