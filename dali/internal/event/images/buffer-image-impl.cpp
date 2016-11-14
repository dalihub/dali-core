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

BufferImagePtr BufferImage::New( unsigned int width,
                                 unsigned int height,
                                 Pixel::Format pixelformat,
                                 ReleasePolicy releasePol )
{
  BufferImagePtr internal = new BufferImage( width, height, pixelformat, releasePol );
  internal->Initialize();
  return internal;
}

BufferImagePtr BufferImage::New( PixelBuffer* pixBuf,
                                 unsigned int width,
                                 unsigned int height,
                                 Pixel::Format pixelformat,
                                 unsigned int stride,
                                 ReleasePolicy releasePol )
{
  BufferImagePtr internal = new BufferImage( pixBuf, width, height, pixelformat, stride, releasePol );
  internal->Initialize();
  return internal;
}

BufferImage::BufferImage(unsigned int width, unsigned int height, Pixel::Format pixelformat, ReleasePolicy releasePol)
: Image( releasePol ),
  mInternalBuffer( NULL ),
  mExternalBuffer( NULL ),
  mResourceClient( NULL ),
  mBufferSize( 0 ),
  mByteStride( 0 ),
  mBytesPerPixel( 0 ),
  mBufferWidth( 0 ),
  mPixelFormat( pixelformat ),
  mResourcePolicy( ResourcePolicy::OWNED_DISCARD )
{
  SetupBuffer( width, height, pixelformat, width, releasePol );

  // Allocate a persistent internal buffer
  mInternalBuffer = new PixelBuffer[ mBufferSize ];
}

BufferImage::BufferImage(PixelBuffer* pixBuf,
                         unsigned int width,
                         unsigned int height,
                         Pixel::Format pixelformat,
                         unsigned int stride,
                         ReleasePolicy releasePol )
: Image( releasePol ),
  mInternalBuffer( NULL ),
  mExternalBuffer( pixBuf ),
  mResourceClient( NULL ),
  mBufferSize( 0 ),
  mByteStride( 0 ),
  mBytesPerPixel( 0 ),
  mBufferWidth( 0 ),
  mPixelFormat( pixelformat ),
  mResourcePolicy( ResourcePolicy::OWNED_DISCARD )
{
  SetupBuffer( width, height, pixelformat, stride ? stride: width, releasePol );
}

BufferImage::~BufferImage()
{
  delete[] mInternalBuffer;
}

void BufferImage::SetupBuffer( unsigned int width,
                               unsigned int height,
                               Pixel::Format pixelformat,
                               unsigned int byteStride,
                               ReleasePolicy releasePol )
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  mResourceClient = &tls.GetResourceClient();
  mWidth  = width;
  mHeight = height;
  mPixelFormat = pixelformat;
  mBytesPerPixel = Pixel::GetBytesPerPixel( pixelformat );

  mByteStride = byteStride * mBytesPerPixel;
  mBufferSize = height * mByteStride;

  // Respect the desired release policy
  mResourcePolicy = releasePol == Dali::Image::UNUSED ? ResourcePolicy::OWNED_DISCARD : ResourcePolicy::OWNED_RETAIN;
}

bool BufferImage::IsDataExternal() const
{
  return ( mExternalBuffer ? true : false );
}

void BufferImage::Update( RectArea& updateArea )
{
  if ( !mTicket )
  {
    CreateHostBitmap();
  }
  DALI_ASSERT_DEBUG( updateArea.x + updateArea.width <= mWidth && updateArea.y + updateArea.height <= mHeight );
  UploadArea( mTicket->GetId(), updateArea );
}

void BufferImage::CreateHostBitmap()
{
  Integration::Bitmap* bitmap = Bitmap::New( Bitmap::BITMAP_2D_PACKED_PIXELS, mResourcePolicy );
  Bitmap::PackedPixelsProfile* const packedBitmap = bitmap->GetPackedPixelsProfile();
  DALI_ASSERT_DEBUG(packedBitmap);

  packedBitmap->ReserveBuffer( mPixelFormat, mWidth, mHeight );
  DALI_ASSERT_DEBUG(bitmap->GetBuffer() != 0);
  DALI_ASSERT_DEBUG(bitmap->GetBufferSize() >= mHeight * mWidth * mBytesPerPixel );

  mTicket = mResourceClient->AddBitmapImage( bitmap );
  mTicket->AddObserver(*this);
}

void BufferImage::UploadArea( ResourceId destId, const RectArea& area )
{
  Integration::Bitmap* bitmap = Bitmap::New( Bitmap::BITMAP_2D_PACKED_PIXELS, mResourcePolicy );
  Bitmap::PackedPixelsProfile* const packedBitmap = bitmap->GetPackedPixelsProfile();
  DALI_ASSERT_DEBUG(packedBitmap);
  DALI_ASSERT_DEBUG( area.width <= mWidth && area.height <= mHeight );

  mBufferWidth = area.width ? area.width : mWidth;
  packedBitmap->ReserveBuffer( mPixelFormat, mBufferWidth, area.height ? area.height : mHeight );
  DALI_ASSERT_DEBUG(bitmap->GetBuffer() != 0);
  DALI_ASSERT_DEBUG(bitmap->GetBufferSize() >= mBufferWidth * ( area.height ? area.height : mHeight ) * mBytesPerPixel );

  // Are we uploading from an external or internal buffer ?
  if ( mExternalBuffer )
  {
    // Check if we're doing the entire area without stride mismatch between source and dest ?
    if( ( mByteStride == mWidth * mBytesPerPixel ) && area.IsEmpty() )
    {
      memcpy( bitmap->GetBuffer(), mExternalBuffer, mBufferSize );
    }
    else
    {
      UpdateBufferArea( mExternalBuffer, bitmap->GetBuffer(), area );
    }
  }
  else
  {
    // Check if we're doing the entire internal buffer ?
    if( area.IsEmpty() )
    {
      memcpy( bitmap->GetBuffer(), mInternalBuffer, bitmap->GetBufferSize() );
    }
    else
    {
      UpdateBufferArea( mInternalBuffer, bitmap->GetBuffer(), area );
    }
  }
  mResourceClient->UploadBitmap( destId, bitmap, area.x, area.y );

}

void BufferImage::UploadBitmap( ResourceId destId, std::size_t xOffset, std::size_t yOffset )
{
  RectArea area( xOffset, yOffset, 0, 0 );
  if ( !mTicket )
  {
    CreateHostBitmap();
  }

  UploadArea( destId, area );
}

void BufferImage::UpdateBufferArea( PixelBuffer* src, PixelBuffer* dest, const RectArea& area )
{
  DALI_ASSERT_DEBUG( area.x + area.width <= mWidth && area.y + area.height <= mHeight );

  uint32_t width = mBufferWidth * mBytesPerPixel;

  src += ( area.y * mByteStride ) + ( area.x * mBytesPerPixel );
  for ( uint32_t i = 0; i < area.height; ++i )
  {
    memcpy( dest, src, width );
    src += mByteStride;
    dest += width;
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
