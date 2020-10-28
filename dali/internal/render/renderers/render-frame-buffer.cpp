/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/internal/render/renderers/render-texture.h>

namespace Dali
{
namespace Internal
{
namespace Render
{
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
}

FrameBuffer::FrameBuffer( uint32_t width, uint32_t height, Mask attachments )
: mId( 0u ),
  mTextureId{ 0u },
  mDepthBuffer( attachments & Dali::FrameBuffer::Attachment::DEPTH ),
  mStencilBuffer( attachments & Dali::FrameBuffer::Attachment::STENCIL ),
  mWidth( width ),
  mHeight( height ),
  mColorAttachmentCount( 0u )
{
}

FrameBuffer::~FrameBuffer() = default;

void FrameBuffer::Destroy( Context& context )
{
  if( mId )
  {
    context.DeleteFramebuffers( 1, &mId );
  }
}

void FrameBuffer::GlContextDestroyed()
{
  mId = 0u;
}

void FrameBuffer::Initialize(Context& context)
{
  context.GenFramebuffers( 1, &mId );
  context.BindFramebuffer( GL_FRAMEBUFFER, mId );

  if( mDepthBuffer )
  {
    // Create a depth render target.
    context.GenRenderbuffers( 1, &mDepthBuffer );
    context.BindRenderbuffer( GL_RENDERBUFFER, mDepthBuffer );
    context.RenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, mWidth, mHeight );
    context.FramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthBuffer );
  }

  if( mStencilBuffer )
  {
    // Create a stencil render target.
    context.GenRenderbuffers( 1, &mStencilBuffer );
    context.BindRenderbuffer( GL_RENDERBUFFER, mStencilBuffer );
    context.RenderbufferStorage( GL_RENDERBUFFER, GL_STENCIL_INDEX8, mWidth, mHeight );
    context.FramebufferRenderbuffer( GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mStencilBuffer );
  }

  context.BindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void FrameBuffer::AttachColorTexture( Context& context, Render::Texture* texture, uint32_t mipmapLevel, uint32_t layer )
{
  context.BindFramebuffer( GL_FRAMEBUFFER, mId );

  const GLuint textureId = texture->GetId();
  mTextureId[mColorAttachmentCount] = textureId;

  // Create a color attachment.
  const GLenum iAttachment = COLOR_ATTACHMENTS[mColorAttachmentCount];
  if( texture->GetType() == TextureType::TEXTURE_2D )
  {
    context.FramebufferTexture2D( GL_FRAMEBUFFER, iAttachment, texture->GetTarget(), textureId, mipmapLevel );
  }
  else
  {
    context.FramebufferTexture2D( GL_FRAMEBUFFER, iAttachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer, textureId, mipmapLevel );
  }

  ++mColorAttachmentCount;
  context.DrawBuffers(mColorAttachmentCount, COLOR_ATTACHMENTS);
  DALI_ASSERT_DEBUG(context.CheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

  context.BindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void FrameBuffer::AttachDepthTexture( Context& context, Render::Texture* texture, uint32_t mipmapLevel )
{
  context.BindFramebuffer( GL_FRAMEBUFFER, mId );

  // Create a depth attachment.
  if( texture->GetType() == TextureType::TEXTURE_2D )
  {
    context.FramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->GetId(), mipmapLevel );
  }

  context.BindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void FrameBuffer::AttachDepthStencilTexture( Context& context, Render::Texture* texture, uint32_t mipmapLevel )
{
  context.BindFramebuffer( GL_FRAMEBUFFER, mId );

  // Create a depth/stencil attachment.
  if( texture->GetType() == TextureType::TEXTURE_2D )
  {
    context.FramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->GetId(), mipmapLevel );
  }

  context.BindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void FrameBuffer::Bind( Context& context )
{
  context.BindFramebuffer( GL_FRAMEBUFFER, mId );
}

uint32_t FrameBuffer::GetWidth() const
{
  return mWidth;
}

uint32_t FrameBuffer::GetHeight() const
{
  return mHeight;
}


} //Render

} //Internal

} //Dali
