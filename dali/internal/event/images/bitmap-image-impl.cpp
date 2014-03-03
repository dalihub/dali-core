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
#include <dali/internal/event/images/bitmap-image-impl.h>

// INTERNAL INCLUDES
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


BitmapImagePtr BitmapImage::New( unsigned int width, unsigned int height, Pixel::Format pixelformat, LoadPolicy loadPol, ReleasePolicy releasePol )
{
  Internal::BitmapImagePtr internal( new BitmapImage( width, height, pixelformat, loadPol, releasePol ) );
  internal->Initialize();

  return internal;
}

BitmapImagePtr BitmapImage::New( PixelBuffer* pixBuf, unsigned int width, unsigned int height, Pixel::Format pixelformat, unsigned int stride, ReleasePolicy releasePol )
{
  Internal::BitmapImagePtr internal( new BitmapImage( pixBuf, width, height, pixelformat, stride, releasePol ) );

  return internal;
}

BitmapImage::BitmapImage(unsigned int width, unsigned int height, Pixel::Format pixelformat, LoadPolicy loadPol, ReleasePolicy releasePol)
: Image(loadPol, releasePol),
  mIsDataExternal(false)
{
  Initialize();

  const ImageTicketPtr& t = mResourceClient->AllocateBitmapImage(width, height, width, height, pixelformat);
  mTicket = t.Get();

  mTicket->AddObserver(*this);
}

BitmapImage::BitmapImage(PixelBuffer* pixBuf, unsigned int width, unsigned int height, Pixel::Format pixelformat, unsigned int stride, ReleasePolicy releasePol)
: Image(ImageLoadPolicyDefault, releasePol),
  mIsDataExternal(true)
{
  Initialize();

  Integration::Bitmap* bitmap = new BitmapExternal(pixBuf, width, height, pixelformat, stride);
  const ImageTicketPtr& t = mResourceClient->AddBitmapImage(bitmap);
  mTicket = t.Get();

  mTicket->AddObserver(*this);
}

BitmapImage::~BitmapImage()
{
}

void BitmapImage::Update( RectArea& updateArea )
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
    // previously freed up resource memory, dali was informed about external BitmapImage put back on screen
    Integration::Bitmap* bitmap = mBitmapCached.Get();
    mTicket.Reset((mResourceClient->AddBitmapImage(bitmap)).Get());

    mTicket->AddObserver(*this);
  }
}

bool BitmapImage::IsDataExternal() const
{
  return mIsDataExternal;
}

PixelBuffer* BitmapImage::GetBuffer()
{
  PixelBuffer* buffer = NULL;

  Integration::Bitmap* const bitmap = GetBitmap();

  if(bitmap)
  {
    buffer = bitmap->GetBuffer();
  }
  return buffer;
}

unsigned int BitmapImage::GetBufferSize() const
{
  unsigned int bufferSize = 0;

  Integration::Bitmap* const bitmap = GetBitmap();

  if(bitmap)
  {
    bufferSize = bitmap->GetBufferSize();
  }
  return bufferSize;
}

unsigned int BitmapImage::GetBufferStride() const
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

void BitmapImage::Initialize()
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();
  mUpdateManager   = &tls.GetUpdateManager();
  mResourceClient = &tls.GetResourceClient();
}

void BitmapImage::Connect()
{
  ++mConnectionCount;

  // application owns bitmap buffer, don't do anything. BufferUpdated() has to be called manually.
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

void BitmapImage::Disconnect()
{
  if (!mTicket)
  {
    return;
  }

  --mConnectionCount;

  if (mConnectionCount == 0 && mReleasePolicy == Dali::Image::Unused)
  {
    mBitmapCached = mResourceClient->GetBitmap(mTicket);
    // release image memory when it's not visible anymore (decrease ref. count of texture)
    mTicket->RemoveObserver(*this);
    mTicket.Reset();
  }
}

Integration::Bitmap * BitmapImage::GetBitmap() const
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

