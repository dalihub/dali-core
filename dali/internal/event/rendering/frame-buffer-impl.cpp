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
 *
 */

// CLASS HEADER
#include <dali/internal/event/rendering/frame-buffer-impl.h>

// INTERNAL INCLUDES
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/render/renderers/render-frame-buffer.h>

namespace Dali
{
namespace Internal
{

FrameBufferPtr FrameBuffer::New( uint32_t width, uint32_t height, Mask attachments  )
{
  FrameBufferPtr frameBuffer( new FrameBuffer( width, height, attachments ) );
  frameBuffer->Initialize();
  return frameBuffer;
}

Render::FrameBuffer* FrameBuffer::GetRenderObject() const
{
  return mRenderObject;
}

FrameBuffer::FrameBuffer( uint32_t width, uint32_t height, Mask attachments )
: mEventThreadServices( EventThreadServices::Get() ),
  mRenderObject( nullptr ),
  mColor{ nullptr },
  mDepth( nullptr ),
  mStencil( nullptr ),
  mWidth( width ),
  mHeight( height ),
  mAttachments( attachments ),
  mColorAttachmentCount( 0 )
{
}

void FrameBuffer::Initialize()
{
   mRenderObject = new Render::FrameBuffer( mWidth, mHeight, mAttachments );

  OwnerPointer< Render::FrameBuffer > transferOwnership( mRenderObject );
  AddFrameBuffer( mEventThreadServices.GetUpdateManager(), transferOwnership );
}

void FrameBuffer::AttachColorTexture( TexturePtr texture, uint32_t mipmapLevel, uint32_t layer )
{
  if( ( texture->GetWidth() / ( 1u << mipmapLevel ) != mWidth ) ||
      ( texture->GetHeight() / ( 1u << mipmapLevel ) != mHeight ) )
  {
    DALI_LOG_ERROR( "Failed to attach color texture to FrameBuffer: Size mismatch \n" );
  }
  else if ( mColorAttachmentCount >= Dali::DevelFrameBuffer::MAX_COLOR_ATTACHMENTS )
  {
    DALI_LOG_ERROR( "Failed to attach color texture to FrameBuffer: Exceeded maximum supported color attachments.\n" );
  }
  else
  {
    mColor[mColorAttachmentCount] = texture;
    ++mColorAttachmentCount;

    AttachColorTextureToFrameBuffer( mEventThreadServices.GetUpdateManager(), *mRenderObject, texture->GetRenderObject(), mipmapLevel, layer );
  }
}

void FrameBuffer::AttachDepthTexture( TexturePtr texture, uint32_t mipmapLevel )
{
  if( ( texture->GetWidth() / ( 1u << mipmapLevel ) != mWidth ) ||
      ( texture->GetHeight() / ( 1u << mipmapLevel ) != mHeight ) )
  {
    DALI_LOG_ERROR( "Failed to attach depth texture to FrameBuffer: Size mismatch \n" );
  }
  else
  {
    mDepth = texture;
    AttachDepthTextureToFrameBuffer( mEventThreadServices.GetUpdateManager(), *mRenderObject, texture->GetRenderObject(), mipmapLevel );
  }
}

void FrameBuffer::AttachDepthStencilTexture( TexturePtr texture, unsigned int mipmapLevel )
{
  if( ( texture->GetWidth() / ( 1u << mipmapLevel ) != mWidth ) ||
      ( texture->GetHeight() / ( 1u << mipmapLevel ) != mHeight ) )
  {
    DALI_LOG_ERROR( "Failed to attach depth/stencil texture to FrameBuffer: Size mismatch \n" );
  }
  else
  {
    mStencil = texture;
    AttachDepthStencilTextureToFrameBuffer( mEventThreadServices.GetUpdateManager(), *mRenderObject, texture->GetRenderObject(), mipmapLevel );
  }
}

Texture* FrameBuffer::GetColorTexture(uint8_t index) const
{
  return ( index >= mColorAttachmentCount ) ? nullptr : mColor[index].Get();
}

Texture* FrameBuffer::GetDepthTexture() const
{
  return ( mDepth ) ? mDepth.Get() : nullptr;
}

Texture* FrameBuffer::GetDepthStencilTexture() const
{
  return ( mStencil ) ? mStencil.Get() : nullptr;
}

void FrameBuffer::SetSize( uint32_t width, uint32_t height )
{
  mWidth = width;
  mHeight = height;
}

FrameBuffer::~FrameBuffer()
{
  if( EventThreadServices::IsCoreRunning() && mRenderObject )
  {
    RemoveFrameBuffer( mEventThreadServices.GetUpdateManager(), *mRenderObject );
  }
}


} // namespace Internal
} // namespace Dali
