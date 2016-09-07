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

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/gl-resources/texture-units.h>
#include <dali/internal/render/gl-resources/gl-texture.h>
#include <dali/devel-api/images/native-image-interface-extension.h>

namespace Dali
{

namespace Internal
{

NativeTexture::NativeTexture(NativeImageInterface* nativeImg, Context& context)
: Texture(context,
          nativeImg->GetWidth(),
          nativeImg->GetHeight(),
          nativeImg->GetWidth(),
          nativeImg->GetHeight()),
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

bool NativeTexture::Bind( GLenum target, TextureUnit textureunit )
{
  bool result = true;

  if( mId == 0 )
  {
    result = CreateGlTexture();
  }

  if( result )
  {
    // Bind the texture id
    mContext.ActiveTexture( textureunit );

    int textureTarget = GL_TEXTURE_2D;
    NativeImageInterface::Extension* extension = mNativeImage->GetExtension();
    if( extension )
    {
      textureTarget = extension->GetEglImageTextureTarget();
    }

    mContext.BindTexture( textureTarget, mId );
    mNativeImage->PrepareTexture();
  }

  return result;
}

bool NativeTexture::IsFullyOpaque() const
{
  return !HasAlphaChannel();
}

bool NativeTexture::HasAlphaChannel() const
{
  return mNativeImage->RequiresBlending();
}

bool NativeTexture::CreateGlTexture()
{
  if( mId != 0 )
  {
    DALI_LOG_INFO( Debug::Filter::gImage, Debug::General, "GL texture creation duplicate for GL id: %d\n", &mId );
    return true;
  }

  if( mNativeImage->GlExtensionCreate() )
  {
    mContext.GenTextures( 1, &mId );
    mContext.ActiveTexture( TEXTURE_UNIT_UPLOAD );  // bind in unused unit so rebind works the first time

    int textureTarget = GL_TEXTURE_2D;
    NativeImageInterface::Extension* extension = mNativeImage->GetExtension();
    if( extension )
    {
      textureTarget = extension->GetEglImageTextureTarget();
    }

    mContext.BindTexture( textureTarget, mId );

    mContext.PixelStorei( GL_UNPACK_ALIGNMENT, 1 ); // We always use tightly packed data

    mContext.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    mContext.TexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    // platform specific implementation decides on what GL extension to use
    mNativeImage->TargetTexture();
  }
  else
  {
    DALI_LOG_ERROR( "Error creating native image!\n" );
  }

  return mId != 0;
}

void NativeTexture::GlCleanup()
{
  Texture::GlCleanup();

  DALI_ASSERT_DEBUG( mNativeImage );

  mNativeImage->GlExtensionDestroy();
  mNativeImage.Reset();
}

bool NativeTexture::Init()
{
  return true;
}

} //namespace Internal

} //namespace Dali
