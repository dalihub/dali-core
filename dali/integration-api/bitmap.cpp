/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/bitmap.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/images/bitmap-packed-pixel.h>
#include <dali/internal/event/images/bitmap-compressed.h>

namespace Dali
{

namespace Integration
{
using namespace Dali::Pixel;


Bitmap* Bitmap::New( const Profile profile = BITMAP_2D_PACKED_PIXELS,
                     ResourcePolicy::Discardable discardable = ResourcePolicy::OWNED_DISCARD )
{
  DALI_ASSERT_DEBUG(profile == BITMAP_2D_PACKED_PIXELS || profile == BITMAP_COMPRESSED);

  switch( profile )
  {
    /** A 2D array of pixels where each pixel is a whole number of bytes
     * and each scanline of the backing memory buffer may have additional
     * bytes off the right edge if requested, and there may be additional
     * scanlines past the bottom of the image in the buffer if requested.*/
    case BITMAP_2D_PACKED_PIXELS:
    {
      Bitmap * const bitmap = new Dali::Internal::BitmapPackedPixel( discardable );
      return bitmap;
    }

    /** The data for the bitmap is buffered in an opaque form.*/
    case BITMAP_COMPRESSED:
    {
      return new Dali::Internal::BitmapCompressed( discardable );
    }
  }
  return 0;
}


Bitmap::Bitmap( ResourcePolicy::Discardable discardable, Dali::Integration::PixelBuffer* pixBuf)
: mImageWidth(0),
  mImageHeight(0),
  mPixelFormat(Pixel::RGBA8888),
  mHasAlphaChannel(true),
  mAlphaChannelUsed(true),
  mData(pixBuf),
  mDiscardable(discardable)
{
}

PixelBuffer* Bitmap::GetBufferOwnership()
{
  PixelBuffer* buffer = mData;
  mData = NULL;
  return buffer;
}

void Bitmap::DiscardBuffer()
{
  if( mDiscardable == ResourcePolicy::OWNED_DISCARD )
  {
    DeletePixelBuffer();
  }
}

Bitmap::~Bitmap()
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);
  DeletePixelBuffer();
}

/**
 * */
void Bitmap::DeletePixelBuffer()
{
  if( !mData )
  {
    return;
  }
  free ( mData );
  mData = NULL;
}


void Bitmap::Initialize( Pixel::Format pixelFormat,
                          unsigned int width,
                          unsigned int height)
{
  DALI_ASSERT_DEBUG(width * height < (32 * 1024) * (32 * 1024) && "The total area of the bitmap is too great.\n");
  mImageWidth   = width;
  mImageHeight  = height;
  mPixelFormat  = pixelFormat;

  mHasAlphaChannel = Pixel::HasAlpha(pixelFormat);
}

} //namespace Integration

} //namespace Dali
