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
#include <dali/internal/render/renderers/render-frame-buffer.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/render/renderers/render-texture.h>

namespace Dali
{
namespace Internal
{
namespace Render
{
FrameBuffer::FrameBuffer(uint32_t width, uint32_t height, Mask attachments)
: mWidth(width),
  mHeight(height),
  mRenderedBuffer(nullptr),
  mCaptureRenderedResult(false),
  mCaptured(false),
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
  if(mRenderedBuffer != nullptr)
  {
    delete[] mRenderedBuffer;
  }
}

void FrameBuffer::Destroy()
{
  mGraphicsObject.reset();
}

void FrameBuffer::Initialize(Graphics::Controller& graphicsController)
{
  mGraphicsController = &graphicsController;
}

void FrameBuffer::AttachColorTexture(Render::Texture* texture, uint32_t mipmapLevel, uint32_t layer)
{
  if(texture)
  {
    if(!texture->GetGraphicsObject())
    {
      texture->Create(0 | Graphics::TextureUsageFlagBits::COLOR_ATTACHMENT | Graphics::TextureUsageFlagBits::SAMPLE);
    }

    uint32_t                  attachmentId = mCreateInfo.colorAttachments.size();
    Graphics::ColorAttachment colorAttachment{attachmentId, texture->GetGraphicsObject(), layer, mipmapLevel};
    mCreateInfo.colorAttachments.push_back(colorAttachment);
  }
}

void FrameBuffer::AttachDepthTexture(Render::Texture* texture, uint32_t mipmapLevel)
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
  }
}

void FrameBuffer::AttachDepthStencilTexture(Render::Texture* texture, uint32_t mipmapLevel)
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
  }
}

void FrameBuffer::SetMultiSamplingLevel(uint8_t multiSamplingLevel)
{
  mCreateInfo.multiSamplingLevel = multiSamplingLevel;
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
      mGraphicsObject = mGraphicsController->CreateFramebuffer(mCreateInfo, std::move(mGraphicsObject));

      if(mCaptureRenderedResult)
      {
        mCaptureRenderedCallback.reset(MakeCallback(this, &FrameBuffer::DrawRenderedBuffer));
        mGraphicsController->CaptureRenderingResult(*mGraphicsObject, mCaptureRenderedCallback.get(), mRenderedBuffer);
        mCaptureRenderedResult = false;
      }

      // Create render target
      Graphics::RenderTargetCreateInfo rtInfo{};
      rtInfo
        .SetFramebuffer(mGraphicsObject.get())
        .SetExtent({mWidth, mHeight})
        .SetPreTransform(0 | Graphics::RenderTargetTransformFlagBits::TRANSFORM_IDENTITY_BIT);
      mRenderTarget = mGraphicsController->CreateRenderTarget(rtInfo, std::move(mRenderTarget));

      std::vector<Graphics::AttachmentDescription> attachmentDescriptions;

      // Default behaviour for color attachments is to CLEAR and STORE
      mClearValues.clear();
      for(auto& attachments : mCreateInfo.colorAttachments)
      {
        if(attachments.texture)
        {
          Graphics::AttachmentDescription desc{};
          desc.SetLoadOp(Graphics::AttachmentLoadOp::CLEAR);
          desc.SetStoreOp(Graphics::AttachmentStoreOp::STORE);
          attachmentDescriptions.push_back(desc);
          mClearValues.emplace_back();
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
        mClearValues.emplace_back();
        attachmentDescriptions.push_back(depthStencilDesc);
      }

      Graphics::RenderPassCreateInfo rpInfo{};
      rpInfo.SetAttachments(attachmentDescriptions);

      // Add default render pass (loadOp = clear)
      mRenderPass.emplace_back(mGraphicsController->CreateRenderPass(rpInfo, nullptr));

      // Add default render pass (loadOp = dontcare)
      attachmentDescriptions[0].SetLoadOp(Graphics::AttachmentLoadOp::DONT_CARE);
      mRenderPass.emplace_back(mGraphicsController->CreateRenderPass(rpInfo, nullptr));

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

[[nodiscard]] Graphics::RenderPass* FrameBuffer::GetGraphicsRenderPass(Graphics::AttachmentLoadOp  colorLoadOp,
                                                                       Graphics::AttachmentStoreOp colorStoreOp) const
{
  // clear only when requested
  if(colorLoadOp == Graphics::AttachmentLoadOp::CLEAR)
  {
    return mRenderPass[0].get();
  }
  else
  {
    return mRenderPass[1].get();
  }
}

void FrameBuffer::DrawRenderedBuffer(uint8_t* inputBuffer)
{
  mCaptured = true;
  mCaptureRenderedCallback.reset();
}

uint8_t* FrameBuffer::GetRenderedBuffer()
{
  if(mCaptured)
  {
    return mRenderedBuffer;
  }
  else
  {
    return nullptr;
  }
}

void FrameBuffer::CaptureRenderingResult()
{
  if(mRenderedBuffer == nullptr)
  {
    mRenderedBuffer = new uint8_t[mWidth * mHeight * sizeof(uint8_t) * 4u];
  }

  // Reset captured flag
  mCaptured = false;

  if(mGraphicsObject)
  {
    mCaptureRenderedCallback.reset(MakeCallback(this, &FrameBuffer::DrawRenderedBuffer));
    mGraphicsController->CaptureRenderingResult(*mGraphicsObject, mCaptureRenderedCallback.get(), mRenderedBuffer);
  }
  else
  {
    mCaptureRenderedResult = true;
  }
}


} // namespace Render

} // namespace Internal

} // namespace Dali
