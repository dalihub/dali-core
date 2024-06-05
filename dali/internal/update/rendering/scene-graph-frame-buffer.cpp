/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/rendering/scene-graph-frame-buffer.h>

// INTERNAL INCLUDES
#include <dali/internal/update/rendering/scene-graph-texture.h>
#include <dali/graphics-api/graphics-types.h>
#include <dali/graphics-api/graphics-controller.h>
#include <dali/integration-api/debug.h>

namespace Dali::Internal::SceneGraph
{

FrameBuffer::FrameBuffer( unsigned int width, unsigned int height, unsigned int attachments )
: mWidth( width ),
  mHeight( height ),
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

FrameBuffer::~FrameBuffer() = default;

void FrameBuffer::Initialize( Graphics::Controller& graphicsController )
{
  mGraphicsController = &graphicsController;
  // Defer creation of framebuffer until a render instruction uses it
}

void FrameBuffer::AttachColorTexture( SceneGraph::Texture* texture, unsigned int mipmapLevel, unsigned int layer )
{
  if(texture)
  {
    if(!texture->GetGraphicsObject())
    {
      texture->CreateTexture(0 | Graphics::TextureUsageFlagBits::COLOR_ATTACHMENT | Graphics::TextureUsageFlagBits::SAMPLE);
    }

    uint32_t                  attachmentId = mCreateInfo.colorAttachments.size();
    Graphics::ColorAttachment colorAttachment{attachmentId, texture->GetGraphicsObject(), layer, mipmapLevel};
    mCreateInfo.colorAttachments.push_back(colorAttachment);
  }
}

void FrameBuffer::AttachDepthStencilTexture( SceneGraph::Texture* texture,
                                             Dali::FrameBuffer::Attachment::Mask format,
                                             unsigned int mipmapLevel,
                                             unsigned int layer )
{
  if(texture)
  {
    if(!texture->GetGraphicsObject())
    {
      texture->CreateTexture(0 | Graphics::TextureUsageFlagBits::DEPTH_STENCIL_ATTACHMENT | Graphics::TextureUsageFlagBits::SAMPLE);
    }

    mCreateInfo.depthStencilAttachment.depthTexture = texture->GetGraphicsObject();
    mCreateInfo.depthStencilAttachment.depthUsage   = Graphics::DepthStencilAttachment::Usage::WRITE;
    mCreateInfo.depthStencilAttachment.depthLevel   = mipmapLevel;
  }
}

unsigned int FrameBuffer::GetWidth() const
{
  return mWidth;
}

unsigned int FrameBuffer::GetHeight() const
{
  return mHeight;
}

void FrameBuffer::PrepareFramebuffer()
{
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
      rpInfo.SetRenderTarget(mRenderTarget.get());

      // Add default render pass (loadOp = clear)
      mRenderPass.emplace_back(mGraphicsController->CreateRenderPass(rpInfo, nullptr));

      // Add default render pass (loadOp = dontcare)
      attachmentDescriptions[0].SetLoadOp(Graphics::AttachmentLoadOp::DONT_CARE);
      mRenderPass.emplace_back(mGraphicsController->CreateRenderPass(rpInfo, nullptr));
    }
  }
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

void FrameBuffer::DestroyGraphicsObjects()
{
  mGraphicsObject.reset();
}

} //Render

//Internal

//Dali
