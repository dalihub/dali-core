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
#include <dali/internal/render/renderers/render-texture-frame-buffer.h>

// INTERNAL INCLUDES
#include <dali/internal/render/renderers/render-texture.h>

namespace Dali
{
namespace Internal
{
namespace Render
{

TextureFrameBuffer::TextureFrameBuffer( uint32_t width, uint32_t height, Mask attachments )
: FrameBuffer(),
  mId( 0u ),
  mTextureId( 0u ),
  mDepthBuffer( attachments & Dali::FrameBuffer::Attachment::DEPTH ),
  mStencilBuffer( attachments & Dali::FrameBuffer::Attachment::STENCIL ),
  mWidth( width ),
  mHeight( height )
{
}

TextureFrameBuffer::~TextureFrameBuffer()
{}

void TextureFrameBuffer::Destroy( Context& context )
{
  if( mId )
  {
    context.DeleteFramebuffers( 1, &mId );
  }
}

void TextureFrameBuffer::GlContextDestroyed()
{
  mId = 0u;
}

void TextureFrameBuffer::Initialize(Context& context)
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

void TextureFrameBuffer::AttachColorTexture( Context& context, Render::Texture* texture, uint32_t mipmapLevel, uint32_t layer )
{
  context.BindFramebuffer( GL_FRAMEBUFFER, mId );

  mTextureId = texture->GetId();

  // Create a color attachment.
  if( texture->GetType() == TextureType::TEXTURE_2D )
  {
    if( !texture->IsNativeImage() )
    {
      context.FramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureId, mipmapLevel );
    }
    else
    {
      // If it's a native image we need to use GL_TEXTURE_EXTERNAL_OES as the texture target parameter
      context.FramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_EXTERNAL_OES, mTextureId, mipmapLevel );
    }
  }
  else
  {
    context.FramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer, mTextureId, mipmapLevel );
  }

  context.BindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void TextureFrameBuffer::Bind( Context& context )
{
  context.BindFramebuffer( GL_FRAMEBUFFER, mId );
}

uint32_t TextureFrameBuffer::GetWidth() const
{
  return mWidth;
}

uint32_t TextureFrameBuffer::GetHeight() const
{
  return mHeight;
}


} //Render

} //Internal

} //Dali
