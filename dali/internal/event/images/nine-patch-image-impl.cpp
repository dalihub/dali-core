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

// CLASS HEADER
#include <dali/internal/event/images/nine-patch-image-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/integration-api/bitmap.h>
#include <dali/internal/event/images/bitmap-external.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/event/images/image-factory.h>
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

NinePatchImagePtr NinePatchImage::New( const std::string& filename, const Dali::ImageAttributes& attributes, LoadPolicy loadPol, ReleasePolicy releasePol )
{
  Internal::NinePatchImagePtr internal( new NinePatchImage( filename, attributes, loadPol, releasePol ) );
  internal->Initialize();
  return internal;
}

NinePatchImage::NinePatchImage( const std::string& filename, const Dali::ImageAttributes& attributes, LoadPolicy loadPol, ReleasePolicy releasePol)
: Image(Dali::Image::Immediate, Dali::Image::Never),
  mParsedBorder(false)
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  mResourceClient = &tls.GetResourceClient();

  Integration::PlatformAbstraction& platformAbstraction = Internal::ThreadLocalStorage::Get().GetPlatformAbstraction();

  Vector2 closestSize;
  platformAbstraction.GetClosestImageSize( filename, attributes, closestSize );
  ImageAttributes loadedAttrs;
  loadedAttrs.SetSize( closestSize );
  mWidth = closestSize.width;
  mHeight = closestSize.height;
  Integration::BitmapResourceType resourceType( loadedAttrs );

  // Note, bitmap is only destroyed when the image is destroyed.
  Integration::ResourcePointer resource = platformAbstraction.LoadResourceSynchronously(resourceType, filename);
  mBitmap = static_cast<Integration::Bitmap*>( resource.Get());
}

NinePatchImage* NinePatchImage::GetNinePatchImage( Image* image)
{
  return dynamic_cast<NinePatchImage*>(image);
}

NinePatchImage::~NinePatchImage()
{
}

Vector4 NinePatchImage::GetStretchBorders()
{
  if( ! mParsedBorder )
  {
    ParseBorders();
  }
  return mStretchBorders;
}

Rect<int> NinePatchImage::GetChildRectangle()
{
  if( ! mParsedBorder )
  {
    ParseBorders();
  }
  return mChildRectangle;
}

Internal::BitmapImagePtr NinePatchImage::CreateCroppedBitmapImage()
{
  Pixel::Format pixelFormat = mBitmap->GetPixelFormat();

  BitmapImagePtr cropped = BitmapImage::New( mWidth-2, mHeight-2, pixelFormat,
                                             Dali::Image::Immediate, Dali::Image::Never );

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
  return cropped;
}

void NinePatchImage::Connect()
{
  if( mConnectionCount == 0 && !mTicket )
  {
    const ImageTicketPtr& t = mResourceClient->AddBitmapImage(mBitmap.Get());
    mTicket = t.Get();
    mTicket->AddObserver(*this);
  }

  ++mConnectionCount;
}

void NinePatchImage::Disconnect()
{
  if( mConnectionCount > 0 )
  {
    --mConnectionCount;
  }
}


void NinePatchImage::ParseBorders()
{
  Pixel::Format pixelFormat = mBitmap->GetPixelFormat();

  Integration::Bitmap::PackedPixelsProfile* srcProfile = mBitmap->GetPackedPixelsProfile();
  DALI_ASSERT_DEBUG( srcProfile && "Wrong profile for source bitmap");

  if( srcProfile )
  {
    unsigned int pixelWidth = GetBytesPerPixel(pixelFormat);
    PixelBuffer* srcPixels = mBitmap->GetBuffer();
    unsigned int srcStride = srcProfile->GetBufferStride();

    int alphaByte=0;
    int alphaBits=0;
    Pixel::GetAlphaOffsetAndMask(pixelFormat, alphaByte, alphaBits);
    int redByte=0;
    int redBits=0;
    GetRedOffsetAndMask(pixelFormat, redByte, redBits);

    int testByte = alphaByte;
    int testBits = alphaBits;
    int testValue = alphaBits; // Opaque == stretch
    if( ! alphaBits )
    {
      testByte = redByte;
      testBits = redBits;
      testValue = 0;           // Black == stretch
    }

    int startX1=-1;
    int endX1=-1;
    int startY1=-1;
    int endY1=-1;
    int startX2=-1;
    int endX2=-1;
    int startY2=-1;
    int endY2=-1;

    PixelBuffer* top = srcPixels + pixelWidth;
    PixelBuffer* bottom = srcPixels + (mHeight-1)*srcStride + pixelWidth;

    // Read the top and bottom rows:
    // (Also read the last column to ensure end value gets set)
    for( unsigned int col=1; col < mWidth; ++col )
    {
      if( (top[testByte] & testBits) == testValue )
      {
        if(startX1 < 0)
        {
          startX1 = col;
        }
      }
      else if(startX1 >= 0 && endX1 < 0)
      {
        endX1 = col-1;
        break;
      }

      if( (bottom[testByte] & testBits) == testValue )
      {
        if(startX2 < 0)
        {
          startX2 = col;
        }
      }
      else if(startX2 >= 0 && endX2 < 0)
      {
        endX2 = col-1;
        break;
      }
      top+=pixelWidth;
      bottom+=pixelWidth;
    }

    // Read the left and right columns:
    PixelBuffer* left  = srcPixels + srcStride;
    PixelBuffer* right = left + (srcStride - pixelWidth);

    // (Also read the last row to ensure end value gets set)
    for( unsigned int row=1; row < mHeight; ++row )
    {
      if((left[testByte] & testBits) == testValue)
      {
        if(startY1 < 0)
        {
          startY1 = row;
        }
      }
      else if(startY1 >= 0 && endY1 < 0)
      {
        endY1 = row-1;
        break;
      }

      if((right[testByte] & testBits) == testValue)
      {
        if(startY2 < 0)
        {
          startY2 = row;
        }
      }
      else if(startY2 >= 0 && endY2 < 0)
      {
        endY2 = row-1;
        break;
      }
      left += srcStride;
      right += srcStride;
    }

    mStretchBorders.x = startX1-1;
    mStretchBorders.y = startY1-1;
    mStretchBorders.z = mWidth-endX1-1;
    mStretchBorders.w = mHeight-endY1-1;

    mChildRectangle.x = startX2-1;
    mChildRectangle.y = startY2-1;
    mChildRectangle.width = endX2-startX2;
    mChildRectangle.height = endY2-startY2;

    mParsedBorder = true;
  }
}

} // namespace Internal

} // namespace Dali
