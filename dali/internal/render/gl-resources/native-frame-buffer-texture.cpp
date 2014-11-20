/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/internal/render/gl-resources/native-frame-buffer-texture.h>

// INTERNAL INCLUDES
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/gl-resources/texture-units.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{

NativeFrameBufferTexture::NativeFrameBufferTexture( NativeImagePtr nativeImage, Context& context)
  : FrameBufferTexture(nativeImage->GetWidth(),
                       nativeImage->GetHeight(),
                       nativeImage->GetPixelFormat(),
                       context),
    mNativeImage(nativeImage)
{
  DALI_LOG_INFO( Debug::Filter::gImage, Debug::General, "NativeFrameBufferTexture created 0x%x\n", &nativeImage );
}

NativeFrameBufferTexture::~NativeFrameBufferTexture()
{
  DALI_LOG_INFO (Debug::Filter::gImage, Debug::General, "NativeFrameBufferTexture destroyed\n");
  // GlCleanup() should already have been called by TextureCache ensuring the resource is destroyed
  // on the render thread. (And avoiding a potentially problematic virtual call in the destructor)
}

bool NativeFrameBufferTexture::IsFullyOpaque() const
{
  // TODO - Should test actual texture...
  return !HasAlphaChannel();
}

bool NativeFrameBufferTexture::HasAlphaChannel() const
{
  return Pixel::HasAlpha(mNativeImage->GetPixelFormat());
}

bool NativeFrameBufferTexture::Init()
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);

  bool status( true ); // assume success
  if( mFrameBufferName == 0 )
  {
    status = CreateGlTexture();
  }

  return status;
}

bool NativeFrameBufferTexture::CreateGlTexture()
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);

  if( mNativeImage->GlExtensionCreate() )
  {
    mContext.GenTextures(1, &mId);
    mContext.ActiveTexture( TEXTURE_UNIT_UPLOAD );  // bind in unused unit so rebind works the first time
    mContext.Bind2dTexture(mId);

    mContext.PixelStorei(GL_UNPACK_ALIGNMENT, 1); // We always use tightly packed data

    mContext.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    mContext.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // platform specific implementation decides on what GL extension to use
    mNativeImage->TargetTexture();

    // generate frame and render buffer names
    mContext.GenFramebuffers(1, &mFrameBufferName);
    mContext.GenRenderbuffers(1, &mRenderBufferName);

    // Bind render buffer and create 16 depth buffer
    mContext.BindRenderbuffer(GL_RENDERBUFFER, mRenderBufferName);
    mContext.RenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, mWidth, mHeight);
  }
  else
  {
    DALI_LOG_ERROR( "Error creating native image!" );
  }

  return mId != 0;
}

void NativeFrameBufferTexture::GlCleanup()
{
  Texture::GlCleanup();

  if (mFrameBufferName != 0)
  {
    mContext.DeleteFramebuffers(1, &mFrameBufferName );
    mFrameBufferName = 0;
  }

  if (mRenderBufferName != 0)
  {
    mContext.DeleteRenderbuffers(1, &mRenderBufferName );
    mRenderBufferName = 0;
  }

  mNativeImage->GlExtensionDestroy();

  mNativeImage.Reset();
}


} //namespace Internal

} //namespace Dali
