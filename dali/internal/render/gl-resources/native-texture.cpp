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
#include <dali/internal/render/gl-resources/native-texture.h>

// EXTERNAL INCLUDES
#include <math.h>
#include <memory.h>

// INTERNAL INCLUDES
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/render/common/vertex.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/gl-resources/texture.h>

namespace Dali
{

namespace Internal
{

NativeTexture::NativeTexture(NativeImage* nativeImg, Context& context)
: Texture(context,
          nativeImg->GetWidth(),
          nativeImg->GetHeight(),
          nativeImg->GetWidth(),
          nativeImg->GetHeight(),
          nativeImg->GetPixelFormat()),
          mNativeImage(nativeImg)
{
  DALI_LOG_INFO( Debug::Filter::gImage, Debug::General, "NativeTexture created 0x%x\n", &nativeImg );
}

NativeTexture::~NativeTexture()
{
  DALI_LOG_INFO (Debug::Filter::gImage, Debug::General, "NativeTexture destroyed\n");
  // GlCleanup() should already have been called by TextureCache ensuring the resource is destroyed
  // on the render thread. (And avoiding a potentially problematic virtual call in the destructor)
}

bool NativeTexture::Bind(GLenum target, GLenum textureunit )
{
  bool created = false;

  if( mId==0 )
  {
    CreateGlTexture();
    created = true;
  }

  // Bind the texture id
  mContext.ActiveTexture( textureunit );
  mContext.Bind2dTexture( mId );

  mNativeImage->PrepareTexture();

  return created;
}

bool NativeTexture::IsFullyOpaque() const
{
  // TODO - Should test actual texture...
  return !HasAlphaChannel();
}

bool NativeTexture::HasAlphaChannel() const
{
  return Pixel::HasAlpha( mNativeImage->GetPixelFormat() );
}

Pixel::Format NativeTexture::GetPixelFormat() const
{
  return mNativeImage->GetPixelFormat();
}

bool NativeTexture::CreateGlTexture()
{
  if( mNativeImage->GlExtensionCreate() )
  {
    mContext.GenTextures( 1, &mId );
    mContext.ActiveTexture( GL_TEXTURE7 );  // bind in unused unit so rebind works the first time
    mContext.Bind2dTexture( mId );

    mContext.PixelStorei( GL_UNPACK_ALIGNMENT, 1 ); // We always use tightly packed data

    mContext.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    mContext.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    // platform specific implementation decides on what GL extension to use
    mNativeImage->TargetTexture();
  }
  else
  {
    DALI_LOG_ERROR( "Error creating native image!" );
  }

  return mId != 0;
}

void NativeTexture::GlCleanup()
{
  Texture::GlCleanup();

  DALI_ASSERT_DEBUG(mNativeImage);

  mNativeImage->GlExtensionDestroy();

  mNativeImage.Reset();
}

bool NativeTexture::Init()
{
  return true;
}

} //namespace Internal

} //namespace Dali
