//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "texture-factory.h"

#include <dali/integration-api/bitmap.h>
#include <dali/internal/render/gl-resources/texture.h>
#include <dali/internal/render/gl-resources/bitmap-texture.h>
#include <dali/internal/render/gl-resources/compressed-bitmap-texture.h>
#include <dali/internal/render/gl-resources/native-texture.h>
#include <dali/internal/render/gl-resources/frame-buffer-texture.h>
#include <dali/internal/render/gl-resources/native-frame-buffer-texture.h>
#include <dali/public-api/images/native-image.h>


namespace Dali
{
class NativeImage;

namespace Internal
{

namespace TextureFactory
{

Internal::Texture* NewBitmapTexture( Integration::Bitmap* const bitmap, Context& context )
{
  DALI_ASSERT_DEBUG( bitmap );
  Texture * texture = 0;
  Integration::Bitmap::PackedPixelsProfile * const  packedPixelBitmapView = bitmap->GetPackedPixelsProfile();
  if( packedPixelBitmapView )
  {
    texture = new BitmapTexture( bitmap, packedPixelBitmapView, context );
  }
  else
  {
    Internal::BitmapCompressed * const compressedBitmap = dynamic_cast<Dali::Internal::BitmapCompressed*>( bitmap );
    if( compressedBitmap != 0 )
    {
      texture = new CompressedBitmapTexture( compressedBitmap, context );
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
                                     Context&          context )
{
  Texture *texture=new BitmapTexture(width, height, pixelFormat, clearPixels, context);

  return texture;
}


Internal::Texture* NewNativeImageTexture( NativeImage& nativeImg, Context& context )
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
                                          Context& context )
{
  FrameBufferTexture* texture = new FrameBufferTexture(width, height, pixelFormat, context);
  if (!texture->Init())
  {
    delete texture;
    return NULL;
  }
  return texture;
}

Internal::Texture* NewFrameBufferTexture( NativeImagePtr nativeImage,
                                          Context& context )
{
  NativeFrameBufferTexture* texture = new NativeFrameBufferTexture(nativeImage, context);
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

