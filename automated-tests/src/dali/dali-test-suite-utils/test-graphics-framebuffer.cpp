/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
  mCallStack(callStack)
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

  if(mCreateInfo.depthStencilAttachment.depthTexture)
  {
    // Create a depth or depth/stencil render target.
    auto depthTexture = Uncast<TestGraphicsTexture>(mCreateInfo.depthStencilAttachment.depthTexture);
    auto attachmentId = DEPTH_STENCIL_ATTACHMENT_TYPE(depthTexture->GetFormat()).attachment;

    mGl.GenRenderbuffers(1, &mDepthBuffer);
    mGl.BindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
    mGl.RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, mCreateInfo.size.width, mCreateInfo.size.height);
    mGl.FramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentId, GL_RENDERBUFFER, mDepthBuffer);

    AttachTexture(depthTexture, attachmentId, 0, mCreateInfo.depthStencilAttachment.depthLevel);
  }

  if(mCreateInfo.depthStencilAttachment.stencilTexture)
  {
    auto stencilTexture = Uncast<TestGraphicsTexture>(mCreateInfo.depthStencilAttachment.stencilTexture);
    auto attachmentId   = DEPTH_STENCIL_ATTACHMENT_TYPE(stencilTexture->GetFormat()).attachment;

    // Create a stencil render target.
    mGl.GenRenderbuffers(1, &mStencilBuffer);
    mGl.BindRenderbuffer(GL_RENDERBUFFER, mStencilBuffer);
    mGl.RenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, mCreateInfo.size.width, mCreateInfo.size.height);
    mGl.FramebufferRenderbuffer(GL_FRAMEBUFFER, attachmentId, GL_RENDERBUFFER, mStencilBuffer);

    AttachTexture(stencilTexture, attachmentId, 0, mCreateInfo.depthStencilAttachment.stencilLevel);
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
}

} // namespace Dali
