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
#include <dali/integration-api/bitmap.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/images/bitmap-packed-pixel.h>
#include <dali/internal/event/images/bitmap-compressed.h>
#include <dali/internal/event/images/bitmap-external.h>
#include <dali/integration-api/gl-abstraction.h>
#include <dali/integration-api/gl-defines.h>

namespace Dali
{

namespace Integration
{
using namespace Dali::Pixel;

void ConvertToGlFormat( Format pixelformat, unsigned& pixelDataType, unsigned& internalFormat )
{
  // Compressed textures have no pixelDataType, so init to an invalid value:
  pixelDataType  = -1;

  switch( pixelformat )
  {
    case A8:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
      internalFormat= GL_ALPHA;
      break;
    }

    case L8:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
      internalFormat= GL_LUMINANCE;
      break;
    }

    case LA88:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
      internalFormat= GL_LUMINANCE_ALPHA;
      break;
    }

    case RGB565:
    {
      pixelDataType = GL_UNSIGNED_SHORT_5_6_5;
      internalFormat= GL_RGB;
      break;
    }

    case BGR565:
    {
      DALI_LOG_ERROR("Pixel format BGR565 is not supported by GLES.\n");
      pixelDataType  = GL_UNSIGNED_SHORT_5_6_5;
#ifdef _ARCH_ARM_
      internalFormat= GL_BGRA_EXT; // alpha is reserved but not used
#else
      internalFormat= GL_RGBA;     // alpha is reserved but not used
#endif
      break;
    }

    case RGBA4444:
    {
      pixelDataType = GL_UNSIGNED_SHORT_4_4_4_4;
      internalFormat= GL_RGBA;
      break;
    }

    case BGRA4444:
    {
      DALI_LOG_ERROR("Pixel format BGRA4444 is not supported by GLES.\n");
      pixelDataType  = GL_UNSIGNED_SHORT_4_4_4_4;
#ifdef _ARCH_ARM_
      internalFormat= GL_BGRA_EXT; // alpha is reserved but not used
#else
      internalFormat= GL_RGBA;     // alpha is reserved but not used
#endif
      break;
    }

    case RGBA5551:
    {
      pixelDataType = GL_UNSIGNED_SHORT_5_5_5_1;
      internalFormat= GL_RGBA;
      break;
    }

    case BGRA5551:
    {
      DALI_LOG_ERROR("Pixel format BGRA5551 is not supported by GLES.\n");
      pixelDataType  = GL_UNSIGNED_SHORT_5_5_5_1;
#ifdef _ARCH_ARM_
      internalFormat= GL_BGRA_EXT; // alpha is reserved but not used
#else
      internalFormat= GL_RGBA;     // alpha is reserved but not used
#endif
      break;
    }

    case RGB888:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
      internalFormat= GL_RGB;
      break;
    }

    case RGB8888:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
      internalFormat= GL_RGBA;     // alpha is reserved but not used
      break;
    }

    case BGR8888:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
#ifdef GL_BGRA_EXT
      internalFormat= GL_BGRA_EXT; // alpha is reserved but not used
#else
      internalFormat= GL_RGBA;     // alpha is reserved but not used
#endif
    break;
    }

    case RGBA8888:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
      internalFormat= GL_RGBA;
      break;
    }

    case BGRA8888:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
#ifdef GL_BGRA_EXT
      internalFormat= GL_BGRA_EXT; // alpha is reserved but not used
#else
      internalFormat= GL_RGBA;     // alpha is reserved but not used
#endif
      break;
    }

    // GLES 3.0 standard compressed formats:
    case COMPRESSED_R11_EAC:
    {
      DALI_LOG_INFO(Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_R11_EAC.\n");
      internalFormat = 0x9270; ///! < Hardcoded until we move to GLES 3.0 or greater.
      break;
    }
    case COMPRESSED_SIGNED_R11_EAC:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_SIGNED_R11_EAC.\n" );
      internalFormat = 0x9271; ///! < Hardcoded until we move to GLES 3.0 or greater.
      ;
      break;
    }
    case COMPRESSED_RG11_EAC:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_RG11_EAC.\n" );
      internalFormat = 0x9272; ///! < Hardcoded until we move to GLES 3.0 or greater.
      break;
    }
    case COMPRESSED_SIGNED_RG11_EAC:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_SIGNED_RG11_EAC.\n" );
      internalFormat = 0x9273; ///! < Hardcoded until we move to GLES 3.0 or greater.
      break;
    }
    case COMPRESSED_RGB8_ETC2:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_RGB8_ETC2.\n" );
      internalFormat = 0x9274; ///! < Hardcoded until we move to GLES 3.0 or greater.
      break;
    }
    case COMPRESSED_SRGB8_ETC2:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_SRGB8_ETC2.\n" );
      internalFormat = 0x9275; ///! < Hardcoded until we move to GLES 3.0 or greater.
      break;
    }
    case COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2.\n" );
      internalFormat = 0x9276; ///! < Hardcoded until we move to GLES 3.0 or greater.
      break;
    }
    case COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2.\n" );
      internalFormat = 0x9277; ///! < Hardcoded until we move to GLES 3.0 or greater.
      break;
    }
    case COMPRESSED_RGBA8_ETC2_EAC:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_RGBA8_ETC2_EAC.\n" );
      internalFormat = 0x9278; ///! < Hardcoded until we move to GLES 3.0 or greater.
      break;
    }
    case COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using GLES 3.0 standard compressed pixel format COMPRESSED_SRGB8_ALPHA8_ETC2_EAC.\n" );
      internalFormat = 0x9279; ///! < Hardcoded until we move to GLES 3.0 or greater.
      break;
    }

    // GLES 2 extension compressed formats:
    case COMPRESSED_RGB8_ETC1:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using non-standard GLES 2.0 extension compressed pixel format COMPRESSED_RGB8_ETC1.\n" );
      internalFormat = 0x8D64; ///! < Hardcoded so we can test before we move to GLES 3.0 or greater.
      break;
    }
    case COMPRESSED_RGB_PVRTC_4BPPV1:
    {
      DALI_LOG_INFO( Debug::Filter::gImage, Debug::Verbose, "Using non-standard GLES 2.0 extension compressed pixel format COMPRESSED_RGB_PVRTC_4BPPV1.\n" );
      internalFormat = 0x8C00; ///! < Hardcoded so we can test before we move to GLES 3.0 or greater.
      break;
    }
  }
}

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

void Bitmap::DiscardBuffer()
{
  if( mDiscardable == ResourcePolicy::OWNED_DISCARD )
  {
    DeletePixelBuffer();
  }
}

PixelBuffer* Bitmap::ReleaseBuffer()
{
  PixelBuffer* const data = mData;

  // Ownership of mData has been transferred, so indicate that mData pointer is no longer valid:
  mData = NULL;

  return data;
}

Bitmap::~Bitmap()
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);

  // If owned
  if( mDiscardable == ResourcePolicy::OWNED_DISCARD ||
      mDiscardable == ResourcePolicy::OWNED_RETAIN )
  {
    DeletePixelBuffer();
  }
}

/**
 * */
void Bitmap::DeletePixelBuffer()
{
  if( !mData )
  {
    return;
  }
  delete [] mData;
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
