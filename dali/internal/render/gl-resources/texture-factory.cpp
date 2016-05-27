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

#include "texture-factory.h"

#include <dali/integration-api/bitmap.h>
#include <dali/integration-api/gl-defines.h>
#include <dali/internal/render/gl-resources/bitmap-texture.h>
#include <dali/internal/render/gl-resources/compressed-bitmap-texture.h>
#include <dali/internal/render/gl-resources/native-texture.h>
#include <dali/internal/render/gl-resources/frame-buffer-texture.h>
#include <dali/internal/render/gl-resources/gl-texture.h>


namespace Dali
{
class NativeImageInterface;

namespace Internal
{

namespace TextureFactory
{

Internal::Texture* NewBitmapTexture( Integration::Bitmap* const bitmap, Context& context, ResourcePolicy::Discardable discardPolicy )
{
  DALI_ASSERT_DEBUG( bitmap );
  Texture * texture = 0;
  Integration::Bitmap::PackedPixelsProfile * const  packedPixelBitmapView = bitmap->GetPackedPixelsProfile();
  if( packedPixelBitmapView )
  {
    texture = new BitmapTexture( bitmap, packedPixelBitmapView, context, discardPolicy );
  }
  else
  {
    Internal::BitmapCompressed * const compressedBitmap = dynamic_cast<Dali::Internal::BitmapCompressed*>( bitmap );
    if( compressedBitmap != 0 )
    {
      texture = new CompressedBitmapTexture( compressedBitmap, context, discardPolicy );
    }
  }
  if( texture )
  {
    if( !texture->Init() )
    {
      delete texture;
      return NULL;
    }
  }
  return texture;
}

Internal::Texture* NewBitmapTexture( unsigned int      width,
                                     unsigned int      height,
                                     Pixel::Format     pixelFormat,
                                     bool              clearPixels,
                                     Context&          context,
                                     ResourcePolicy::Discardable discardPolicy )
{
  Texture *texture=new BitmapTexture(width, height, pixelFormat, clearPixels, context, discardPolicy);

  return texture;
}


Internal::Texture* NewNativeImageTexture( NativeImageInterface& nativeImg, Context& context )
{
  NativeTexture* texture = new NativeTexture(&nativeImg, context);
  if (!texture->Init())
  {
    delete texture;
    return NULL;
  }
  return texture;
}

Internal::Texture* NewFrameBufferTexture( unsigned int width,
                                          unsigned int height,
                                          Pixel::Format pixelFormat,
                                          RenderBuffer::Format bufferFormat,
                                          Context& context )
{
  FrameBufferTexture* texture = new FrameBufferTexture(width, height, pixelFormat, bufferFormat, context);
  if (!texture->Init())
  {
    delete texture;
    return NULL;
  }
  return texture;
}

Internal::Texture* NewFrameBufferTexture( NativeImageInterfacePtr nativeImage,
                                          Context& context )
{
  FrameBufferTexture* texture = new FrameBufferTexture(nativeImage, context);
  if (!texture->Init())
  {
    delete texture;
    return NULL;
  }
  return texture;
}



} // TextureFactory
} // Internal
} // Dali
