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
#include <dali/internal/event/images/atlas-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/images/bitmap-image-impl.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/integration-api/bitmap.h>

namespace Dali
{

namespace Internal
{

namespace
{
TypeRegistration mType( typeid( Dali::Atlas ), typeid( Dali::Image ), NULL );
}

Atlas* Atlas::New( std::size_t width,
                   std::size_t height,
                   Pixel::Format pixelFormat )
{
  return new Atlas( width, height, pixelFormat );
}

bool Atlas::Upload( const BitmapImage& bitmapImage,
                    std::size_t xOffset,
                    std::size_t yOffset )
{
  bool uploadSuccess( false );

  AllocateAtlas();

  if( IsWithin(bitmapImage, xOffset, yOffset) )
  {
    ResourceId destId = GetResourceId();
    ResourceId srcId = bitmapImage.GetResourceId();

    if( destId && srcId )
    {
      mResourceClient.UploadBitmap( destId, srcId, xOffset, yOffset );
      uploadSuccess = true;
    }
  }

  return uploadSuccess;
}

Atlas::~Atlas()
{
  ReleaseAtlas();
}

Atlas::Atlas( std::size_t width,
              std::size_t height,
              Pixel::Format pixelFormat )
: mResourceClient( ThreadLocalStorage::Get().GetResourceClient() ),
  mPixelFormat( pixelFormat )
{
  mWidth  = width;
  mHeight = height;
}

void Atlas::Connect()
{
  ++mConnectionCount;

  if( mConnectionCount == 1 )
  {
    AllocateAtlas();
  }
}

void Atlas::Disconnect()
{
  if( mConnectionCount )
  {
    --mConnectionCount;

    if ( Dali::Image::UNUSED == mReleasePolicy &&
         mConnectionCount == 0 )
    {
      ReleaseAtlas();
    }
  }
}

bool Atlas::IsWithin( const BitmapImage& bitmapImage,
                      std::size_t xOffset,
                      std::size_t yOffset )
{
  bool within(false);

  if( mPixelFormat != bitmapImage.GetPixelFormat() )
  {
    DALI_LOG_ERROR( "Pixel format %d does not match Atlas format %d\n", bitmapImage.GetPixelFormat(), mPixelFormat );
  }
  else
  {
    const unsigned int width  = bitmapImage.GetWidth();
    const unsigned int height = bitmapImage.GetHeight();

    if( xOffset < mWidth  &&
        yOffset < mHeight &&
        xOffset+width  <= mWidth &&
        yOffset+height <= mHeight )
    {
      within = true;
    }
    else
    {
      DALI_LOG_ERROR( "%dx%d image does not fit at offset %d,%d\n", width, height, xOffset, yOffset );
    }
  }

  return within;
}

void Atlas::AllocateAtlas()
{
  if( !mTicket )
  {
    mTicket = mResourceClient.AllocateTexture( mWidth, mHeight, mPixelFormat );
  }
}

void Atlas::ReleaseAtlas()
{
  mTicket.Reset();
}

} // namespace Internal

} // namespace Dali
