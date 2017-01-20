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
 *
 */

// CLASS HEADER
#include <dali/internal/event/images/nine-patch-image-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for memcmp

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/integration-api/bitmap.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/integration-api/resource-types.h>
#include <dali/integration-api/resource-cache.h>


namespace
{
void GetRedOffsetAndMask(Dali::Pixel::Format pixelFormat, int& byteOffset, int& bitMask)
{
  switch (pixelFormat)
  {
    case Dali::Pixel::A8:
    case Dali::Pixel::L8:
    case Dali::Pixel::LA88:
    {
      byteOffset=0;
      bitMask=0;
      break;
    }

    case Dali::Pixel::RGB888:
    case Dali::Pixel::RGB8888:
    case Dali::Pixel::RGBA8888:
    {
      byteOffset=0;
      bitMask=0xFF;
      break;
    }
    case Dali::Pixel::BGR8888:
    case Dali::Pixel::BGRA8888:
    {
      byteOffset=2;
      bitMask=0xff;
      break;
    }
    case Dali::Pixel::RGB565:
    {
      byteOffset=0;
      bitMask=0xf8;
      break;
    }
    case Dali::Pixel::BGR565:
    {
      byteOffset=1;
      bitMask=0x1f;
      break;
    }

    case Dali::Pixel::RGBA4444:
    {
      byteOffset=0;
      bitMask=0xf0;
      break;
    }
    case Dali::Pixel::BGRA4444:
    {
      byteOffset=1;
      bitMask=0xf0;
      break;
    }

    case Dali::Pixel::RGBA5551:
    {
      byteOffset=0;
      bitMask=0xf8;
      break;
    }

    case Dali::Pixel::BGRA5551:
    {
      byteOffset=1;
      bitMask=0x1e;
      break;
    }

    case Dali::Pixel::INVALID:
    case Dali::Pixel::COMPRESSED_R11_EAC:
    case Dali::Pixel::COMPRESSED_SIGNED_R11_EAC:
    case Dali::Pixel::COMPRESSED_RG11_EAC:
    case Dali::Pixel::COMPRESSED_SIGNED_RG11_EAC:
    case Dali::Pixel::COMPRESSED_RGB8_ETC2:
    case Dali::Pixel::COMPRESSED_SRGB8_ETC2:
    case Dali::Pixel::COMPRESSED_RGB8_ETC1:
    case Dali::Pixel::COMPRESSED_RGB_PVRTC_4BPPV1:
    case Dali::Pixel::COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case Dali::Pixel::COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case Dali::Pixel::COMPRESSED_RGBA8_ETC2_EAC:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_4x4_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_5x4_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_5x5_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_6x5_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_6x6_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_8x5_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_8x6_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_8x8_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_10x5_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_10x6_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_10x8_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_10x10_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_12x10_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_12x12_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
    {
      DALI_LOG_ERROR("Pixel formats for compressed images are not compatible with simple masking-out of per-pixel alpha.\n");
      byteOffset=0;
      bitMask=0;
      break;
    }
  }
}

} // anonymous namespace


namespace Dali
{
namespace Internal
{

namespace
{
TypeRegistration mType( typeid( Dali::NinePatchImage ), typeid( Dali::Image ), NULL );
} // unnamed namespace

NinePatchImagePtr NinePatchImage::New( const std::string& filename )
{
  Internal::NinePatchImagePtr internal( new NinePatchImage( filename ) );
  internal->Initialize();
  return internal;
}

NinePatchImage::NinePatchImage( const std::string& filename )
: ResourceImage(),
  mParsedBorder(false)
{
  mUrl = filename;
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  mResourceClient = &tls.GetResourceClient();

  Integration::PlatformAbstraction& platformAbstraction = tls.GetPlatformAbstraction();
  Integration::BitmapResourceType resourceType;

  // Note, bitmap is only destroyed when the image is destroyed.
  Integration::ResourcePointer resource = platformAbstraction.LoadResourceSynchronously( resourceType, filename );
  if( resource )
  {
    mBitmap = static_cast<Integration::Bitmap*>( resource.Get());

    mWidth = mBitmap->GetImageWidth();
    mHeight = mBitmap->GetImageHeight();
    mTexture = NewTexture::New( Dali::TextureType::TEXTURE_2D, mBitmap->GetPixelFormat(), mWidth, mHeight );

    size_t bufferSize = mBitmap->GetBufferSize();
    unsigned char* buffer = new unsigned char[bufferSize];
    memcpy( buffer, mBitmap->GetBuffer(), bufferSize );
    PixelDataPtr pixelData = PixelData::New( buffer, bufferSize, mWidth, mHeight, mBitmap->GetPixelFormat(), Dali::PixelData::DELETE_ARRAY );
    mTexture->Upload( pixelData );
  }
  else
  {
    mBitmap.Reset();
    mWidth = 0;
    mHeight = 0;
  }
}

NinePatchImage* NinePatchImage::DownCast( Image* image)
{
  return dynamic_cast<NinePatchImage*>(image);
}

NinePatchImage::~NinePatchImage()
{
}

const NinePatchImage::StretchRanges& NinePatchImage::GetStretchPixelsX()
{
  if( ! mParsedBorder )
  {
    ParseBorders();
  }
  return mStretchPixelsX;
}

const NinePatchImage::StretchRanges& NinePatchImage::GetStretchPixelsY()
{
  if( ! mParsedBorder )
  {
    ParseBorders();
  }
  return mStretchPixelsY;
}

Rect<int> NinePatchImage::GetChildRectangle()
{
  if( ! mParsedBorder )
  {
    ParseBorders();
  }
  return mChildRectangle;
}

Internal::BufferImagePtr NinePatchImage::CreateCroppedBufferImage()
{
  BufferImagePtr cropped;

  if( ! mBitmap )
  {
    DALI_LOG_ERROR( "NinePatchImage: Bitmap not loaded, cannot perform operation\n");
  }
  else
  {
    Pixel::Format pixelFormat = mBitmap->GetPixelFormat();

    cropped = BufferImage::New( mWidth-2, mHeight-2, pixelFormat );

    Integration::Bitmap::PackedPixelsProfile* srcProfile = mBitmap->GetPackedPixelsProfile();
    DALI_ASSERT_DEBUG( srcProfile && "Wrong profile for source bitmap");

    if( srcProfile )
    {
      PixelBuffer* destPixels = cropped->GetBuffer();
      unsigned int destStride = cropped->GetBufferStride();
      unsigned int pixelWidth = GetBytesPerPixel(pixelFormat);

      PixelBuffer* srcPixels = mBitmap->GetBuffer();
      unsigned int srcStride = srcProfile->GetBufferStride();

      for( unsigned int row=1; row < mHeight-1; ++row )
      {
        PixelBuffer* src  = srcPixels + row*srcStride + pixelWidth;
        PixelBuffer* dest = destPixels + (row-1)*destStride;
        memcpy(dest, src, destStride );
      }
    }

    RectArea area;
    cropped->Update(area); // default area has no width or height
  }
  return cropped;
}

const std::string& NinePatchImage::GetUrl() const
{
  return mUrl;
}

void NinePatchImage::ParseBorders()
{
  if( !mBitmap )
  {
    DALI_LOG_ERROR( "NinePatchImage: Bitmap not loaded, cannot perform operation\n");
    return;
  }

  mStretchPixelsX.Clear();
  mStretchPixelsY.Clear();

  Pixel::Format pixelFormat = mBitmap->GetPixelFormat();

  const Integration::Bitmap::PackedPixelsProfile* srcProfile = mBitmap->GetPackedPixelsProfile();
  DALI_ASSERT_DEBUG( srcProfile && "Wrong profile for source bitmap" );

  if( srcProfile )
  {
    int alphaByte = 0;
    int alphaBits = 0;
    Pixel::GetAlphaOffsetAndMask( pixelFormat, alphaByte, alphaBits );

    int testByte = alphaByte;
    int testBits = alphaBits;
    int testValue = alphaBits; // Opaque == stretch
    if( ! alphaBits )
    {
      GetRedOffsetAndMask( pixelFormat, testByte, testBits );
      testValue = 0;           // Black == stretch
    }

    unsigned int pixelWidth = GetBytesPerPixel( pixelFormat );
    const PixelBuffer* srcPixels = mBitmap->GetBuffer();
    unsigned int srcStride = srcProfile->GetBufferStride();

    //TOP
    const PixelBuffer* top = srcPixels + pixelWidth;
    unsigned int index = 0;
    unsigned int width = mBitmap->GetImageWidth();
    unsigned int height = mBitmap->GetImageHeight();

    for(; index < width - 2; )
    {
      Uint16Pair range = ParseRange( index, width - 2, top, pixelWidth, testByte, testBits, testValue );
      if( range.GetX() != 0xFFFF )
      {
        mStretchPixelsX.PushBack( range );
      }
    }

    //LEFT
    const PixelBuffer* left  = srcPixels + srcStride;
    index = 0;
    for(; index < height - 2; )
    {
      Uint16Pair range = ParseRange( index, height - 2, left, srcStride, testByte, testBits, testValue );
      if( range.GetX() != 0xFFFF )
      {
        mStretchPixelsY.PushBack( range );
      }
    }

    //If there are no stretch pixels then make the entire image stretchable
    if( mStretchPixelsX.Size() == 0 )
    {
      mStretchPixelsX.PushBack( Uint16Pair( 0, width - 2 ) );
    }
    if( mStretchPixelsY.Size() == 0 )
    {
      mStretchPixelsY.PushBack( Uint16Pair( 0, height - 2 ) );
    }

    //Child Rectangle
    //BOTTOM
    const PixelBuffer* bottom = srcPixels + ( height - 1 ) * srcStride + pixelWidth;
    index = 0;
    Uint16Pair contentRangeX = ParseRange( index, width - 2, bottom, pixelWidth, testByte, testBits, testValue );
    if( contentRangeX.GetX() == 0xFFFF )
    {
      contentRangeX = Uint16Pair();
    }

    //RIGHT
    const PixelBuffer* right = srcPixels + srcStride + ( width - 1 ) * pixelWidth;
    index = 0;
    Uint16Pair contentRangeY = ParseRange( index, height - 2, right, srcStride, testByte, testBits, testValue );
    if( contentRangeY.GetX() == 0xFFFF )
    {
      contentRangeY = Uint16Pair();
    }

    mChildRectangle.x = contentRangeX.GetX() + 1;
    mChildRectangle.y = contentRangeY.GetX() + 1;
    mChildRectangle.width = contentRangeX.GetY() - contentRangeX.GetX();
    mChildRectangle.height = contentRangeY.GetY() - contentRangeY.GetX();

    mParsedBorder = true;
  }
}

Uint16Pair NinePatchImage::ParseRange( unsigned int& index, unsigned int width, const PixelBuffer* & pixel, unsigned int pixelStride, int testByte, int testBits, int testValue )
{
  unsigned int start = 0xFFFF;
  for( ; index < width; ++index, pixel += pixelStride )
  {
    if( ( pixel[ testByte ] & testBits ) == testValue )
    {
        start = index;
        ++index;
        pixel += pixelStride;
        break;
    }
  }

  unsigned int end = width;
  for( ; index < width; ++index, pixel += pixelStride )
  {
    if( ( pixel[ testByte ] & testBits ) != testValue )
    {
        end = index;
        ++index;
        pixel += pixelStride;
        break;
    }
  }

  return Uint16Pair( start, end );
}

bool NinePatchImage::IsNinePatchUrl( const std::string& url )
{
  bool match = false;

  std::string::const_reverse_iterator iter = url.rbegin();
  enum { SUFFIX, HASH, HASH_DOT, DONE } state = SUFFIX;
  while(iter < url.rend())
  {
    switch(state)
    {
      case SUFFIX:
      {
        if(*iter == '.')
        {
          state = HASH;
        }
        else if(!isalnum(*iter))
        {
          state = DONE;
        }
      }
      break;
      case HASH:
      {
        if( *iter == '#' || *iter == '9' )
        {
          state = HASH_DOT;
        }
        else
        {
          state = DONE;
        }
      }
      break;
      case HASH_DOT:
      {
        if(*iter == '.')
        {
          match = true;
        }
        state = DONE; // Stop testing characters
      }
      break;
      case DONE:
      {
      }
      break;
    }

    // Satisfy prevent
    if( state == DONE )
    {
      break;
    }

    ++iter;
  }
  return match;
}

} // namespace Internal

} // namespace Dali
