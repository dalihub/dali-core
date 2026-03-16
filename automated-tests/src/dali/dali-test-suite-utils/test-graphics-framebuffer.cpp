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

#include "test-graphics-framebuffer.h"
#include <dali/integration-api/gl-defines.h>
#include "test-graphics-controller.h"
#include "test-graphics-texture.h"

namespace
{
const GLenum COLOR_ATTACHMENTS[] =
  {
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3,
    GL_COLOR_ATTACHMENT4,
    GL_COLOR_ATTACHMENT5,
    GL_COLOR_ATTACHMENT6,
    GL_COLOR_ATTACHMENT7,
};

struct DEPTH_STENCIL_ATTACHMENT_TYPE
{
  constexpr explicit DEPTH_STENCIL_ATTACHMENT_TYPE(Graphics::Format textureFormat)
  {
    switch(textureFormat)
    {
      case Graphics::Format::D16_UNORM:
      case Graphics::Format::D32_SFLOAT:
      case Graphics::Format::X8_D24_UNORM_PACK32:
      {
        attachment = GL_DEPTH_ATTACHMENT;
        break;
      }

      case Graphics::Format::S8_UINT:
      {
        attachment = GL_STENCIL_ATTACHMENT;
        break;
      }

      case Graphics::Format::D16_UNORM_S8_UINT:
      case Graphics::Format::D24_UNORM_S8_UINT:
      case Graphics::Format::D32_SFLOAT_S8_UINT:
      {
        attachment = GL_DEPTH_STENCIL_ATTACHMENT;
        break;
      }
      default:
      {
        attachment = GL_NONE;
        break;
      }
    }
  }
  GLenum attachment{GL_NONE};
};

} // namespace
//namespace

namespace Dali
{
TestGraphicsFramebuffer::TestGraphicsFramebuffer(
  TraceCallStack&                        callStack,
  TestGlAbstraction&                     glAbstraction,
  const Graphics::FramebufferCreateInfo& createInfo)
: mGl(glAbstraction),
  mCallStack(callStack),
  mDepthBufferUsed(createInfo.depthStencilAttachment.depthUsage == Graphics::DepthStencilAttachment::Usage::WRITE),
  mStencilBufferUsed(createInfo.depthStencilAttachment.stencilUsage == Graphics::DepthStencilAttachment::Usage::WRITE),
  mAttachedDepthWrite(false),
  mAttachedStencilWrite(false),
  mAttachedAttachment(GL_NONE),
  mAttachedInternalFormat(GL_NONE)
{
  mCreateInfo.colorAttachments       = std::move(createInfo.colorAttachments);
  mCreateInfo.depthStencilAttachment = createInfo.depthStencilAttachment;
  mCreateInfo.size                   = createInfo.size;
}

TestGraphicsFramebuffer::~TestGraphicsFramebuffer()
{
  if(mId)
  {
    mGl.DeleteFramebuffers(1, &mId);
  }
  if(mDepthBufferId)
  {
    mGl.DeleteRenderbuffers(1, &mDepthBufferId);
    mDepthBufferId = 0u;
  }
  if(mStencilBufferId)
  {
    mGl.DeleteRenderbuffers(1, &mStencilBufferId);
    mStencilBufferId = 0u;
  }
}

void TestGraphicsFramebuffer::Initialize()
{
  mCallStack.PushCall("Initialize", "");

  mGl.GenFramebuffers(1, &mId);
  mGl.BindFramebuffer(GL_FRAMEBUFFER, mId);

  for(Graphics::ColorAttachment& attachment : mCreateInfo.colorAttachments)
  {
    AttachTexture(attachment.texture, COLOR_ATTACHMENTS[attachment.attachmentId], attachment.layerId, attachment.levelId);
  }
  mGl.DrawBuffers(mCreateInfo.colorAttachments.size(), COLOR_ATTACHMENTS);

  if(mCreateInfo.depthStencilAttachment.stencilTexture)
  {
    auto stencilTexture = Uncast<TestGraphicsTexture>(mCreateInfo.depthStencilAttachment.stencilTexture);
    auto attachmentId   = DEPTH_STENCIL_ATTACHMENT_TYPE(stencilTexture->GetFormat()).attachment;

    AttachTexture(stencilTexture, attachmentId, 0, mCreateInfo.depthStencilAttachment.stencilLevel);
  }
  else if(mCreateInfo.depthStencilAttachment.depthTexture)
  {
    // Create a depth or depth/stencil render target.
    auto depthTexture = Uncast<TestGraphicsTexture>(mCreateInfo.depthStencilAttachment.depthTexture);
    auto attachmentId = DEPTH_STENCIL_ATTACHMENT_TYPE(depthTexture->GetFormat()).attachment;

    AttachTexture(depthTexture, attachmentId, 0, mCreateInfo.depthStencilAttachment.depthLevel);
  }
  mGl.BindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TestGraphicsFramebuffer::AttachTexture(Graphics::Texture* texture, uint32_t attachmentId, uint32_t layerId, uint32_t levelId)
{
  auto graphicsTexture = Uncast<TestGraphicsTexture>(texture);
  if(graphicsTexture->GetType() == Graphics::TextureType::TEXTURE_2D)
  {
    mGl.FramebufferTexture2D(GL_FRAMEBUFFER, attachmentId, graphicsTexture->GetTarget(), graphicsTexture->mId, levelId);
  }
  else
  {
    mGl.FramebufferTexture2D(GL_FRAMEBUFFER, attachmentId, GL_TEXTURE_CUBE_MAP_POSITIVE_X + layerId, graphicsTexture->mId, levelId);
  }
}

void TestGraphicsFramebuffer::Bind()
{
  mCallStack.PushCall("Bind", "");

  if(!mId)
  {
    Initialize();
  }
  mGl.BindFramebuffer(GL_FRAMEBUFFER, mId);

  PrepareRenderBuffer();
}

void TestGraphicsFramebuffer::UpdateDepthStencilState(const Graphics::DepthStencilState& depthStencilState)
{
  mCallStack.PushCall("UpdateDepthStencilState", "");

  if(mCreateInfo.depthStencilAttachment.depthUsage == Graphics::DepthStencilAttachment::Usage::WRITE)
  {
    mDepthBufferUsed = depthStencilState.depthTestEnable;
  }
  if(mCreateInfo.depthStencilAttachment.stencilUsage == Graphics::DepthStencilAttachment::Usage::WRITE)
  {
    mStencilBufferUsed = depthStencilState.stencilTestEnable;
  }
}

void TestGraphicsFramebuffer::PrepareRenderBuffer()
{
  mCallStack.PushCall("PrepareRenderBuffer", "");

  if(!mCreateInfo.depthStencilAttachment.stencilTexture && !mCreateInfo.depthStencilAttachment.depthTexture)
  {
    const bool depthWrite   = (mCreateInfo.depthStencilAttachment.depthUsage == Graphics::DepthStencilAttachment::Usage::WRITE) && mDepthBufferUsed;
    const bool stencilWrite = (mCreateInfo.depthStencilAttachment.stencilUsage == Graphics::DepthStencilAttachment::Usage::WRITE) && mStencilBufferUsed;

    // Check if the state has changed
    if(depthWrite == mAttachedDepthWrite && stencilWrite == mAttachedStencilWrite)
    {
      return; // No change, nothing to do
    }

    // State has changed, need to update renderbuffer

    // Delete old renderbuffers
    if(mDepthBufferId != 0u)
    {
      mGl.DeleteRenderbuffers(1, &mDepthBufferId);
      mDepthBufferId = 0u;
    }
    if(mStencilBufferId != 0u)
    {
      mGl.DeleteRenderbuffers(1, &mStencilBufferId);
      mStencilBufferId = 0u;
    }

    // Reset previous framebuffer attachment
    if(mAttachedAttachment != GL_NONE)
    {
      mGl.FramebufferRenderbuffer(GL_FRAMEBUFFER, mAttachedAttachment, GL_RENDERBUFFER, 0);
      mAttachedAttachment = GL_NONE;
    }

    if(depthWrite || stencilWrite)
    {
      // Determine attachment and internal format
      const auto attachment     = depthWrite ? (stencilWrite ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT) : GL_STENCIL_ATTACHMENT;
      const auto internalFormat = depthWrite ? (stencilWrite ? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT16) : GL_STENCIL_INDEX8;

      // Choose which buffer ID to use
      uint32_t& bufferId = stencilWrite ? mStencilBufferId : mDepthBufferId;

      // Create new renderbuffer
      mGl.GenRenderbuffers(1, &bufferId);
      mGl.BindRenderbuffer(GL_RENDERBUFFER, bufferId);
      mGl.RenderbufferStorage(GL_RENDERBUFFER, internalFormat, mCreateInfo.size.width, mCreateInfo.size.height);

      // Attach to framebuffer
      mGl.FramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, bufferId);

      // Update attached state
      mAttachedAttachment     = attachment;
      mAttachedInternalFormat = internalFormat;
    }
    else
    {
      // No renderbuffer needed
      mAttachedAttachment     = GL_NONE;
      mAttachedInternalFormat = GL_NONE;
    }

    // Update attached write states
    mAttachedDepthWrite   = depthWrite;
    mAttachedStencilWrite = stencilWrite;
  }
}

} // namespace Dali
