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
#include <dali/internal/render/common/terminated-native-draw-manager.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-controller.h>

namespace Dali::Internal::Render
{
TerminatedNativeDrawManager::TerminatedNativeDrawManager(Graphics::Controller& graphicsController)
: mGraphicsController(&graphicsController),
  mTerminatedRenderTargets()
{
}

bool TerminatedNativeDrawManager::AnyTerminatedCallbackExist() const
{
  return !mTerminatedRenderTargets.empty();
}

bool TerminatedNativeDrawManager::TerminatedCallbackExist(const SceneGraph::RenderTargetGraphicsObjects& renderTarget) const
{
  return mTerminatedRenderTargets.find(&renderTarget) != mTerminatedRenderTargets.end();
}

void TerminatedNativeDrawManager::RegisterTerminatedRenderCallback(const SceneGraph::RenderTargetGraphicsObjects& renderTarget, RenderCallback* renderCallback, std::unique_ptr<Dali::RenderCallbackInput> renderCallbackInput)
{
  if(mTerminatedRenderTargets.find(&renderTarget) == mTerminatedRenderTargets.end())
  {
    auto& mutableRenderTargetGraphicsObjects = *const_cast<SceneGraph::RenderTargetGraphicsObjects*>(&renderTarget);
    mutableRenderTargetGraphicsObjects.AddLifecycleObserver(*this);
  }

  const bool isolatedNotDirect = (renderCallback->GetExecutionMode() == RenderCallback::ExecutionMode::ISOLATED);

  Graphics::DrawNativeInfo info{};
  info.api      = Graphics::DrawNativeAPI::GLES;
  info.userData = renderCallbackInput.get();

  // Set storage for the context to be used
  info.executionMode = isolatedNotDirect ? Graphics::DrawNativeExecutionMode::ISOLATED : Graphics::DrawNativeExecutionMode::DIRECT;
  info.reserved      = nullptr;

  // Do not send texture binding info for terminate case.
  info.textureCount = 0u;
  info.textureList  = nullptr;

  // Get or create container for given renderTarget
  auto& callbackList = mTerminatedRenderTargets[&renderTarget];
  callbackList.emplace_back(renderCallback, std::move(info), std::move(renderCallbackInput));
}

void TerminatedNativeDrawManager::SubmitAllTerminatedRenderCallback()
{
  while(!mTerminatedRenderTargets.empty())
  {
    auto  iter         = mTerminatedRenderTargets.begin();
    auto* renderTarget = iter->first;
    SubmitTerminatedRenderCallback(*renderTarget);
  }
}

void TerminatedNativeDrawManager::SubmitTerminatedRenderCallback(const SceneGraph::RenderTargetGraphicsObjects& renderTarget)
{
  auto iter = mTerminatedRenderTargets.find(&renderTarget);
  if(iter != mTerminatedRenderTargets.end())
  {
    {
      auto& mutableRenderTargetGraphicsObjects = *const_cast<SceneGraph::RenderTargetGraphicsObjects*>(&renderTarget);
      mutableRenderTargetGraphicsObjects.RemoveLifecycleObserver(*this);
    }

    auto& callbackList = iter->second;
    if(DALI_LIKELY(mGraphicsController && !callbackList.empty() && renderTarget.GetGraphicsRenderTarget() && renderTarget.GetGraphicsRenderPass(Graphics::AttachmentLoadOp::LOAD, Graphics::AttachmentStoreOp::STORE)))
    {
      auto& currentRenderTarget      = *renderTarget.GetGraphicsRenderTarget();
      auto& currentRenderPassNoClear = *renderTarget.GetGraphicsRenderPass(Graphics::AttachmentLoadOp::LOAD, Graphics::AttachmentStoreOp::STORE);

      auto commandBuffer = mGraphicsController->CreateCommandBuffer(Graphics::CommandBufferCreateInfo().SetLevel(Graphics::CommandBufferLevel::PRIMARY), nullptr);
      commandBuffer->Begin(Graphics::CommandBufferBeginInfo()
                             .SetUsage(0 | Graphics::CommandBufferUsageFlagBits::ONE_TIME_SUBMIT)
                             .SetRenderTarget(currentRenderTarget));

      commandBuffer->BeginRenderPass(&currentRenderPassNoClear, &currentRenderTarget, {0, 0, 1, 1}, {});

      for(auto& callback : callbackList)
      {
        // Ensure to use the valid pointer of render callback.
        callback.info.callback = &static_cast<Dali::CallbackBase&>(*callback.renderCallback);

        callback.info.glesNativeInfo.eglSharedContextStoragePointer = &callback.renderCallbackInput->eglContext;

        // submit draw
        commandBuffer->DrawNative(&callback.info);
      }

      commandBuffer->EndRenderPass(nullptr);
      commandBuffer->End();

      Graphics::SubmitInfo submitInfo;
      submitInfo.flags = 0 | Graphics::SubmitFlagBits::FLUSH;
      submitInfo.cmdBuffer.push_back(commandBuffer.get());
      mGraphicsController->SubmitCommandBuffers(submitInfo);
    }

    // Remove callbacks after flush, due to the ownership of renderCallbackInput memory is in callbackList.
    mTerminatedRenderTargets.erase(iter);
  }
}

void TerminatedNativeDrawManager::RenderTargetGraphicsObjectsDestroyed(const SceneGraph::RenderTargetGraphicsObjects* renderTargetGraphicsObjects)
{
  mTerminatedRenderTargets.erase(renderTargetGraphicsObjects);
}

} // namespace Dali::Internal::Render
