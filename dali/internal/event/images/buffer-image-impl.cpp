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
#include <dali/internal/event/images/buffer-image-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/integration-api/bitmap.h>
#include <dali/internal/event/images/bitmap-external.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/event/images/image-factory.h>

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
  mIsDataExternal(false)
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  mResourceClient = &tls.GetResourceClient();
  mWidth  = width;
  mHeight = height;

  const ImageTicketPtr& t = mResourceClient->AllocateBitmapImage(width, height, width, height, pixelformat);
  mTicket = t.Get();

  mTicket->AddObserver(*this);
}

BufferImage::BufferImage(PixelBuffer* pixBuf, unsigned int width, unsigned int height, Pixel::Format pixelformat, unsigned int stride, ReleasePolicy releasePol)
: Image(releasePol),
  mIsDataExternal(true)
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  mResourceClient = &tls.GetResourceClient();
  mWidth  = width;
  mHeight = height;
  Integration::Bitmap* bitmap = new BitmapExternal(pixBuf, width, height, pixelformat, stride);
  const ImageTicketPtr& t = mResourceClient->AddBitmapImage(bitmap);
  mTicket = t.Get();

  mTicket->AddObserver(*this);
}

BufferImage::~BufferImage()
{
}

void BufferImage::Update( RectArea& updateArea )
{
  if (mTicket)
  {
    // TODO:
    // If updateArea is empty or same as image size, then pass on.
    // If updateArea is larger than image size, throw exception
    // Otherwise, copy updateArea window of pixelBuffer into newly
    // allocated buffer and pass that to resource client. (it will
    // tramp through to BitmapTexture eventually!)
    mResourceClient->UpdateBitmapArea( mTicket, updateArea );
  }
  else if (mIsDataExternal && mBitmapCached)
  {
    // previously freed up resource memory, dali was informed about external BufferImage put back on screen
    Integration::Bitmap* bitmap = mBitmapCached.Get();
    mTicket.Reset((mResourceClient->AddBitmapImage(bitmap)).Get());

    mTicket->AddObserver(*this);
  }
}

bool BufferImage::IsDataExternal() const
{
  return mIsDataExternal;
}

PixelBuffer* BufferImage::GetBuffer()
{
  PixelBuffer* buffer = NULL;

  Integration::Bitmap* const bitmap = GetBitmap();

  if(bitmap)
  {
    buffer = bitmap->GetBuffer();
  }
  return buffer;
}

unsigned int BufferImage::GetBufferSize() const
{
  unsigned int bufferSize = 0;

  Integration::Bitmap* const bitmap = GetBitmap();

  if(bitmap)
  {
    bufferSize = bitmap->GetBufferSize();
  }
  return bufferSize;
}

unsigned int BufferImage::GetBufferStride() const
{
  unsigned int bufferStride = 0;

  Integration::Bitmap* const bitmap = GetBitmap();

  if(bitmap)
  {
    Integration::Bitmap::PackedPixelsProfile* packedBitmap = bitmap->GetPackedPixelsProfile();
    DALI_ASSERT_DEBUG(packedBitmap);
    bufferStride = packedBitmap->GetBufferStride();
  }

  return bufferStride;
}

Pixel::Format BufferImage::GetPixelFormat() const
{
  Pixel::Format format( Pixel::RGBA8888 );

  Integration::Bitmap* const bitmap = GetBitmap();

  if( bitmap )
  {
    format = bitmap->GetPixelFormat();
  }

  return format;
}

void BufferImage::Connect()
{
  ++mConnectionCount;

  // application owns bitmap buffer, don't do anything. Update() has to be called manually.
  if (mIsDataExternal)
  {
    return;
  }

  if (mConnectionCount == 1)
  {
    if (!mTicket && mBitmapCached)
    {
      const ImageTicketPtr& t = mResourceClient->AddBitmapImage(mBitmapCached.Get());
      mTicket = t.Get();
      mTicket->AddObserver(*this);
    }
  }
}

void BufferImage::Disconnect()
{
  if (!mTicket)
  {
    return;
  }

  --mConnectionCount;

  if (mConnectionCount == 0 && mReleasePolicy == Dali::Image::UNUSED)
  {
    mBitmapCached = mResourceClient->GetBitmap(mTicket);
    // release image memory when it's not visible anymore (decrease ref. count of texture)
    mTicket->RemoveObserver(*this);
    mTicket.Reset();
  }
}

Integration::Bitmap * BufferImage::GetBitmap() const
{
  Integration::Bitmap* bitmap = NULL;

  if (mTicket)
  {
    bitmap = mResourceClient->GetBitmap(mTicket);
  }
  else
  {
    // off screen and freeing memory was requested
    bitmap = mBitmapCached.Get();
  }

  DALI_ASSERT_DEBUG(bitmap);

  return bitmap;
}

} // namespace Internal

} // namespace Dali
