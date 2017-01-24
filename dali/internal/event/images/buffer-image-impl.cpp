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
#include <dali/internal/event/images/buffer-image-impl.h>

// EXTERNAL INCLUDES
#include <string.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/update/manager/update-manager.h>

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
                                 Pixel::Format pixelformat )
{
  BufferImagePtr internal = new BufferImage( width, height, pixelformat );
  internal->Initialize();
  internal->Update( RectArea() );
  return internal;
}

BufferImagePtr BufferImage::New( PixelBuffer* pixBuf,
                                 unsigned int width,
                                 unsigned int height,
                                 Pixel::Format pixelformat,
                                 unsigned int stride )
{
  BufferImagePtr internal = new BufferImage( pixBuf, width, height, pixelformat, stride );
  internal->Initialize();
  internal->Update( RectArea() );
  return internal;
}

BufferImage::BufferImage(unsigned int width, unsigned int height, Pixel::Format pixelformat)
: Image(),
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
  SetupBuffer( width, height, pixelformat, width );

  // Allocate a persistent internal buffer
  mInternalBuffer = new PixelBuffer[ mBufferSize ];
}

BufferImage::BufferImage(PixelBuffer* pixBuf,
                         unsigned int width,
                         unsigned int height,
                         Pixel::Format pixelformat,
                         unsigned int stride)
: Image(),
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
  SetupBuffer( width, height, pixelformat, stride ? stride: width );
}

BufferImage::~BufferImage()
{
  delete[] mInternalBuffer;
}

void BufferImage::SetupBuffer( unsigned int width,
                               unsigned int height,
                               Pixel::Format pixelformat,
                               unsigned int byteStride )
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
  mResourcePolicy = ResourcePolicy::OWNED_RETAIN;
}

bool BufferImage::IsDataExternal() const
{
  return ( mExternalBuffer ? true : false );
}

void BufferImage::Update( const RectArea& updateArea )
{
  if ( !mTexture )
  {
    mTexture = NewTexture::New( Dali::TextureType::TEXTURE_2D, mPixelFormat, mWidth, mHeight );
  }
  DALI_ASSERT_DEBUG( updateArea.x + updateArea.width <= mWidth && updateArea.y + updateArea.height <= mHeight );
  UploadArea( updateArea );
}

void BufferImage::UploadArea( const RectArea& area )
{
  DALI_ASSERT_DEBUG( area.width <= mWidth && area.height <= mHeight );

  mBufferWidth = area.width ? area.width : mWidth;
  uint32_t bufferHeight = area.height ? area.height : mHeight;
  size_t bufferSize = mBytesPerPixel * mBufferWidth * bufferHeight;
  unsigned char* buffer = reinterpret_cast< Dali::Integration::PixelBuffer* >( malloc( bufferSize ) );
  DALI_ASSERT_DEBUG(buffer != 0);

  // Are we uploading from an external or internal buffer ?
  if ( mExternalBuffer )
  {
    // Check if we're doing the entire area without stride mismatch between source and dest ?
    if( ( mByteStride == mWidth * mBytesPerPixel ) && area.IsEmpty() )
    {
      memcpy( buffer, mExternalBuffer, mBufferSize );
    }
    else
    {
      UpdateBufferArea( mExternalBuffer, buffer, area );
    }
  }
  else
  {
    // Check if we're doing the entire internal buffer ?
    if( area.IsEmpty() )
    {
      memcpy( buffer, mInternalBuffer, bufferSize );
    }
    else
    {
      UpdateBufferArea( mInternalBuffer, buffer, area );
    }
  }
  PixelDataPtr pixelData = PixelData::New( buffer, bufferSize, mBufferWidth, bufferHeight, mPixelFormat, Dali::PixelData::FREE );
  mTexture->Upload( pixelData, 0u, 0u, area.x, area.y, mBufferWidth, bufferHeight );
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

} // namespace Internal

} // namespace Dali
