/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

//INTERNAL INCLUDES
#include <dali/internal/render/renderers/render-texture.h>

namespace Dali
{
namespace Internal
{
namespace Render
{

FrameBuffer::FrameBuffer( unsigned int width, unsigned int height, Format format )
:mId( 0u ),
 mDepthBuffer( (format == Dali::FrameBuffer::COLOR_DEPTH || format == Dali::FrameBuffer::COLOR_DEPTH_STENCIL ) ? 1u : 0u ),
 mStencilBuffer( (format == Dali::FrameBuffer::COLOR_STENCIL || format == Dali::FrameBuffer::COLOR_DEPTH_STENCIL ) ? 1u : 0u ),
 mWidth( width ),
 mHeight( height )
{
}

FrameBuffer::~FrameBuffer()
{}

void FrameBuffer::Destroy( Context& context )
{
  if( mId )
  {
    context.DeleteFramebuffers( 1, &mId );
  }
}

void FrameBuffer::Initialize(Context& context)
{
  context.GenFramebuffers( 1, &mId );
  context.BindFramebuffer( GL_FRAMEBUFFER, mId );

  if( mDepthBuffer )
  {
    //Create a depth render target
    context.GenRenderbuffers(1, &mDepthBuffer );
    context.BindRenderbuffer(GL_RENDERBUFFER, mDepthBuffer);
    context.RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, mWidth, mHeight);
    context.FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthBuffer);
  }

  if( mStencilBuffer )
  {
    //Create a stencil render target
    context.GenRenderbuffers(1, &mStencilBuffer );
    context.BindRenderbuffer(GL_RENDERBUFFER, mStencilBuffer);
    context.RenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, mWidth, mHeight);
    context.FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mStencilBuffer );
  }

  context.BindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void FrameBuffer::AttachColorTexture( Context& context, Render::NewTexture* texture, unsigned int mipmapLevel, unsigned int layer )
{
  context.BindFramebuffer( GL_FRAMEBUFFER, mId );

  if( texture->GetType() == TextureType::TEXTURE_2D )
  {
    context.FramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->GetId(), mipmapLevel );
  }
  else
  {
    context.FramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer, texture->GetId(), mipmapLevel );
  }

  context.BindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void FrameBuffer::Bind( Context& context )
{
  context.BindFramebuffer( GL_FRAMEBUFFER, mId );
}

unsigned int FrameBuffer::GetWidth() const
{
  return mWidth;
}

unsigned int FrameBuffer::GetHeight() const
{
  return mHeight;
}


} //Render

} //Internal

} //Dali
