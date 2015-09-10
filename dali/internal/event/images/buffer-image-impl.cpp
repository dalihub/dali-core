/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/images/buffer-image-impl.h>

// EXTERNAL INCLUDES
#include <string.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/event/images/image-factory.h>

using namespace Dali::Integration;

namespace Dali
{
namespace Internal
{

namespace
{
TypeRegistration mType( typeid( Dali::BufferImage ), typeid( Dali::Image ), NULL );
} // unnamed namespace

BufferImagePtr BufferImage::New( unsigned int width, unsigned int height, Pixel::Format pixelformat, ReleasePolicy releasePol )
{
  BufferImagePtr internal = new BufferImage( width, height, pixelformat, releasePol );
  internal->Initialize();
  return internal;
}

BufferImagePtr BufferImage::New( PixelBuffer* pixBuf, unsigned int width, unsigned int height, Pixel::Format pixelformat, unsigned int stride, ReleasePolicy releasePol )
{
  BufferImagePtr internal = new BufferImage( pixBuf, width, height, pixelformat, stride, releasePol );
  internal->Initialize();
  return internal;
}

BufferImage::BufferImage(unsigned int width, unsigned int height, Pixel::Format pixelformat, ReleasePolicy releasePol)
: Image(releasePol),
  mInternalBuffer(NULL),
  mExternalBuffer(NULL),
  mBitmap(NULL)
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  mResourceClient = &tls.GetResourceClient();
  mWidth  = width;
  mHeight = height;
  mPixelFormat = pixelformat;
  mBytesPerPixel = Pixel::GetBytesPerPixel( pixelformat );
  mByteStride = width * mBytesPerPixel;
  mBufferSize = height * mByteStride;

  // Allocate a persistent internal buffer
  mInternalBuffer = new PixelBuffer[ mBufferSize ];

  // Respect the desired release policy
  mResourcePolicy = releasePol == Dali::Image::UNUSED ? ResourcePolicy::OWNED_DISCARD : ResourcePolicy::OWNED_RETAIN;
}

BufferImage::BufferImage(PixelBuffer* pixBuf, unsigned int width, unsigned int height, Pixel::Format pixelformat, unsigned int stride, ReleasePolicy releasePol )
: Image(releasePol),
  mInternalBuffer(NULL),
  mExternalBuffer(pixBuf),
  mBitmap(NULL)
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  mResourceClient = &tls.GetResourceClient();
  mWidth  = width;
  mHeight = height;
  mPixelFormat = pixelformat;
  mBytesPerPixel = Pixel::GetBytesPerPixel( pixelformat );
  mByteStride = ( stride ? stride : mWidth ) * mBytesPerPixel;
  mBufferSize = height * mByteStride;

  // Respect the desired release policy
  mResourcePolicy = releasePol == Dali::Image::UNUSED ? ResourcePolicy::OWNED_DISCARD : ResourcePolicy::OWNED_RETAIN;

  // Create a bitmap to hold copy of external buffer
  ReserveBitmap();

  // Take a copy of the external buffer immediately, so it can be released if desired
  RectArea area;
  MirrorExternal( area );
}

BufferImage::~BufferImage()
{
  delete[] mInternalBuffer;
}

bool BufferImage::IsDataExternal() const
{
  return ( mExternalBuffer ? true : false );
}

void BufferImage::Update( RectArea& updateArea )
{
  ValidateBitmap();
  UpdateBitmap( updateArea );

  if (mTicket)
  {
    DALI_ASSERT_DEBUG( updateArea.x + updateArea.width <= mWidth && updateArea.y + updateArea.height <= mHeight );
    mResourceClient->UpdateBitmapArea( mTicket, updateArea );

    // Bitmap ownership has been passed on, so any subsequent update will need another bitmap
    mBitmap = NULL;
  }
}

void BufferImage::UpdateBitmap( RectArea& updateArea )
{
  if ( mExternalBuffer )
  {
    MirrorExternal( updateArea );
  }
  else
  {
    // Copy the internal buffer to the bitmap area
    memcpy( mBitmap->GetBuffer(), mInternalBuffer, mBufferSize );
  }
}

void BufferImage::ValidateBitmap()
{
  ReserveBitmap();
  if ( !mTicket )
  {
    mTicket = mResourceClient->AddBitmapImage( mBitmap );
    mTicket->AddObserver(*this);
  }
}

void BufferImage::ReserveBitmap()
{
   // Does a bitmap currently exist ?
  if ( !mBitmap )
  {
    mBitmap = Bitmap::New( Bitmap::BITMAP_2D_PACKED_PIXELS, mResourcePolicy );
  }

  if ( !mBitmap->GetBuffer() )
  {
    Bitmap::PackedPixelsProfile* const packedBitmap = mBitmap->GetPackedPixelsProfile();
    DALI_ASSERT_DEBUG(packedBitmap);

    packedBitmap->ReserveBuffer( mPixelFormat, mWidth, mHeight, mByteStride / mBytesPerPixel, mHeight );
    DALI_ASSERT_DEBUG(mBitmap->GetBuffer() != 0);
    DALI_ASSERT_DEBUG(mBitmap->GetBufferSize() >= mWidth * mHeight * Pixel::GetBytesPerPixel( mBitmap->GetPixelFormat() ) );
  }
}

void BufferImage::UploadBitmap( ResourceId destId, std::size_t xOffset, std::size_t yOffset )
{
  // Make sure we have a bitmap for transport
  ReserveBitmap();

  // Copy source pixel data into bitmap
  RectArea area;
  UpdateBitmap( area );

  mResourceClient->UploadBitmap( destId, mBitmap, xOffset, yOffset);
  mBitmap = NULL;
}

void BufferImage::UpdateBufferArea( PixelBuffer* src, const RectArea& area )
{
  DALI_ASSERT_DEBUG( area.x + area.width <= mWidth && area.y + area.height <= mHeight );

  PixelBuffer* dest = mBitmap->GetBuffer();
  uint32_t width = area.width * mBytesPerPixel;
  uint32_t stride = mWidth * mBytesPerPixel;

  src += ( area.y * mByteStride ) + ( area.x * mBytesPerPixel );
  dest +=( ( area.y * mWidth ) + area.x ) * mBytesPerPixel;
  for ( uint32_t i = 0; i < area.height; ++i )
  {
    memcpy( dest, src, width );
    src += mByteStride;
    dest += stride;
  }
}

void BufferImage::MirrorExternal( const RectArea& area )
{
  if( ( mByteStride == mWidth * mBytesPerPixel ) && area.IsEmpty() )
  {
    memcpy( mBitmap->GetBuffer(), mExternalBuffer, mBufferSize );
  }
  else
  {
    UpdateBufferArea( mExternalBuffer, area );
  }
}

void BufferImage::Connect()
{
  if ( !mConnectionCount++ )
  {
    RectArea area;
    Update( area );
  }
}

void BufferImage::Disconnect()
{
  if ( mTicket )
  {
    if ( !( --mConnectionCount ) && mReleasePolicy == Dali::Image::UNUSED )
    {
      mTicket->RemoveObserver(*this);
      mTicket.Reset();
    }
  }
}

} // namespace Internal

} // namespace Dali
