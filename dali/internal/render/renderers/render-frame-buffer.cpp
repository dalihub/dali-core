/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/internal/render/renderers/render-frame-buffer.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/render/common/render-manager.h> ///< TODO : Could we remove it?
#include <dali/internal/render/renderers/render-texture.h>

namespace Dali
{
namespace Internal
{
namespace Render
{
FrameBuffer::FrameBuffer(uint32_t width, uint32_t height, Mask attachments)
: mDepthTexture(),
  mDepthStencilTexture(),
  mRenderResult(nullptr),
  mWidth(width),
  mHeight(height),
  mDepthBuffer(attachments & Dali::FrameBuffer::Attachment::DEPTH),
  mStencilBuffer(attachments & Dali::FrameBuffer::Attachment::STENCIL)
{
  mCreateInfo.size.width  = width;
  mCreateInfo.size.height = height;
  if(mDepthBuffer)
  {
    mCreateInfo.depthStencilAttachment.depthUsage = Graphics::DepthStencilAttachment::Usage::WRITE;
  }
  if(mStencilBuffer)
  {
    mCreateInfo.depthStencilAttachment.stencilUsage = Graphics::DepthStencilAttachment::Usage::WRITE;
  }
}

FrameBuffer::~FrameBuffer()
{
  ClearRenderResult();
}

void FrameBuffer::Destroy()
{
  mGraphicsObject.reset();
}

void FrameBuffer::AttachColorTexture(const Render::TextureKey& texture, uint32_t mipmapLevel, uint32_t layer)
{
  if(texture)
  {
    if(!texture->GetGraphicsObject())
    {
      texture->Create(0 | Graphics::TextureUsageFlagBits::COLOR_ATTACHMENT | Graphics::TextureUsageFlagBits::SAMPLE);
    }

    uint32_t                  attachmentId = mCreateInfo.colorAttachments.size();
    Graphics::ColorAttachment colorAttachment{attachmentId, texture->GetGraphicsObject(), layer, mipmapLevel, texture->GetPixelFormat()};
    mCreateInfo.colorAttachments.push_back(colorAttachment);

    mColorTextures.push_back(texture);
  }
}

void FrameBuffer::AttachDepthTexture(const Render::TextureKey& texture, uint32_t mipmapLevel)
{
  if(texture)
  {
    if(!texture->GetGraphicsObject())
    {
      texture->Create(0 | Graphics::TextureUsageFlagBits::DEPTH_STENCIL_ATTACHMENT | Graphics::TextureUsageFlagBits::SAMPLE);
    }

    mCreateInfo.depthStencilAttachment.depthTexture = texture->GetGraphicsObject();
    mCreateInfo.depthStencilAttachment.depthUsage   = Graphics::DepthStencilAttachment::Usage::WRITE;
    mCreateInfo.depthStencilAttachment.depthLevel   = mipmapLevel;

    mDepthTexture = texture;
  }
}

void FrameBuffer::AttachDepthStencilTexture(const Render::TextureKey& texture, uint32_t mipmapLevel)
{
  if(texture)
  {
    if(!texture->GetGraphicsObject())
    {
      texture->Create(0 | Graphics::TextureUsageFlagBits::DEPTH_STENCIL_ATTACHMENT | Graphics::TextureUsageFlagBits::SAMPLE);
    }
    mCreateInfo.depthStencilAttachment.stencilTexture = texture->GetGraphicsObject();
    mCreateInfo.depthStencilAttachment.stencilUsage   = Graphics::DepthStencilAttachment::Usage::WRITE;
    mCreateInfo.depthStencilAttachment.stencilLevel   = mipmapLevel;

    mDepthStencilTexture = texture;
  }
}

void FrameBuffer::SetMultiSamplingLevel(uint8_t multiSamplingLevel)
{
  mCreateInfo.multiSamplingLevel = multiSamplingLevel;
}

void FrameBuffer::KeepRenderResult()
{
  mIsKeepingRenderResultRequested = true;
}

void FrameBuffer::ClearRenderResult()
{
  if(mIsKeepingRenderResultRequested)
  {
    mIsKeepingRenderResultRequested = false;
    delete[] mRenderResult;
  }

  Dali::Mutex::ScopedLock lock(mPixelDataMutex);
  if(mRenderedPixelData)
  {
    mRenderedPixelData.Reset();
  }
}

bool FrameBuffer::IsKeepingRenderResultRequested() const
{
  return mIsKeepingRenderResultRequested;
}

uint8_t* FrameBuffer::GetRenderResultBuffer()
{
  uint8_t* buffer = nullptr;
  if(mIsKeepingRenderResultRequested)
  {
    if(mRenderResult)
    {
      delete[] mRenderResult;
    }
    mRenderResult = new uint8_t[mWidth * mHeight * Dali::Pixel::GetBytesPerPixel(Pixel::Format::RGBA8888)];
    buffer        = mRenderResult;
  }
  return buffer;
}

void FrameBuffer::SetRenderResultDrawn()
{
  Dali::Mutex::ScopedLock lock(mPixelDataMutex);
  mRenderedPixelData              = Dali::PixelData::New(mRenderResult, mWidth * mHeight * Dali::Pixel::GetBytesPerPixel(Pixel::Format::RGBA8888), mWidth, mHeight, Pixel::Format::RGBA8888, Dali::PixelData::DELETE_ARRAY);
  mRenderResult                   = nullptr;
  mIsKeepingRenderResultRequested = false;
}

void FrameBuffer::UpdateAttachedTextures(SceneGraph::RenderManager& renderManager)
{
  for(auto colorTexture : mColorTextures)
  {
    if(colorTexture)
    {
      renderManager.SetTextureUpdated(colorTexture);
    }
  }

  if(mDepthTexture)
  {
    renderManager.SetTextureUpdated(mDepthTexture);
  }
  if(mDepthStencilTexture)
  {
    renderManager.SetTextureUpdated(mDepthStencilTexture);
  }
}

// Called from Main thread.
Dali::PixelData FrameBuffer::GetRenderResult()
{
  Dali::Mutex::ScopedLock lock(mPixelDataMutex);
  Dali::PixelData         pixelData;
  if(!mIsKeepingRenderResultRequested && mRenderedPixelData)
  {
    pixelData = mRenderedPixelData;
  }
  return pixelData;
}

bool FrameBuffer::CreateGraphicsObjects()
{
  bool created = false;

  if(!mGraphicsObject)
  {
    // Only create a graphics object if there are attachments for it to render into
    if(mCreateInfo.colorAttachments.empty() &&
       mCreateInfo.depthStencilAttachment.depthTexture == nullptr &&
       mCreateInfo.depthStencilAttachment.stencilTexture == nullptr &&
       !mDepthBuffer && !mStencilBuffer)
    {
      DALI_LOG_ERROR("Attempting to bind a framebuffer with no attachments\n");
    }
    else
    {
      std::vector<Graphics::AttachmentDescription> attachmentDescriptions;

      // Default behaviour for color attachments is to CLEAR and STORE
      auto& clearValues = RenderTargetGraphicsObjects::GetGraphicsRenderPassClearValues();
      clearValues.clear();
      for(auto& attachments : mCreateInfo.colorAttachments)
      {
        if(attachments.texture)
        {
          Graphics::AttachmentDescription desc{};
          desc.SetLoadOp(Graphics::AttachmentLoadOp::CLEAR);
          desc.SetStoreOp(Graphics::AttachmentStoreOp::STORE);
          attachmentDescriptions.push_back(desc);
          clearValues.emplace_back();
        }
      }

      if(mCreateInfo.depthStencilAttachment.depthTexture || mCreateInfo.depthStencilAttachment.stencilTexture ||
         mDepthBuffer || mStencilBuffer)
      {
        Graphics::AttachmentDescription depthStencilDesc{};
        depthStencilDesc.SetLoadOp(Graphics::AttachmentLoadOp::CLEAR)
          .SetStoreOp(Graphics::AttachmentStoreOp::DONT_CARE);

        if(mCreateInfo.depthStencilAttachment.stencilTexture || mStencilBuffer)
        {
          depthStencilDesc.SetStencilLoadOp(Graphics::AttachmentLoadOp::CLEAR)
            .SetStencilStoreOp(Graphics::AttachmentStoreOp::DONT_CARE);
        }
        clearValues.emplace_back();
        clearValues.back().depthStencil.depth   = 1.0f;
        clearValues.back().depthStencil.stencil = 0;
        attachmentDescriptions.push_back(depthStencilDesc);
      }

      Graphics::RenderPassCreateInfo rpInfo{};
      rpInfo.SetAttachments(attachmentDescriptions);

      // Add default render pass (loadOp = clear)
      RenderTargetGraphicsObjects::CreateRenderPass(*mGraphicsController, rpInfo);

      // Add default render pass (loadOp = dontcare)
      attachmentDescriptions[0].SetLoadOp(Graphics::AttachmentLoadOp::DONT_CARE);
      RenderTargetGraphicsObjects::CreateRenderPassNoClear(*mGraphicsController, rpInfo);

      std::vector<Graphics::RenderPass*> renderPasses;
      renderPasses.push_back(mRenderPass.get());
      renderPasses.push_back(mRenderPassNoClear.get());
      mCreateInfo.SetRenderPasses(std::move(renderPasses));

      mGraphicsObject = mGraphicsController->CreateFramebuffer(mCreateInfo, std::move(mGraphicsObject));

      // Create render target
      Graphics::RenderTargetCreateInfo rtInfo{};
      rtInfo
        .SetFramebuffer(mGraphicsObject.get())
        .SetExtent({mWidth, mHeight})
        .SetPreTransform(0 | Graphics::RenderTargetTransformFlagBits::TRANSFORM_IDENTITY_BIT);

      RenderTargetGraphicsObjects::CreateRenderTarget(*mGraphicsController, rtInfo);

      created = true;
    }
  }
  return created;
}

uint32_t FrameBuffer::GetWidth() const
{
  return mWidth;
}

uint32_t FrameBuffer::GetHeight() const
{
  return mHeight;
}

} // namespace Render

} // namespace Internal

} // namespace Dali
