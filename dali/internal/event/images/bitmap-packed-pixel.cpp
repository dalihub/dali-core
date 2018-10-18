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
#include <dali/internal/event/images/bitmap-packed-pixel.h>

// EXTERNAL INCLUDES
#include <cstdlib>

// INTERNAL INCLUDES
#include <dali/public-api/object/ref-object.h>
#include <dali/internal/common/core-impl.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{
using namespace Dali::Pixel;


BitmapPackedPixel::BitmapPackedPixel( ResourcePolicy::Discardable discardable, Dali::Integration::PixelBuffer* pixBuf )
: Bitmap( discardable, pixBuf ),
  mBufferWidth(0),
  mBufferHeight(0),
  mBytesPerPixel(0)
{
}

// use power of two bufferWidth and bufferHeight for better performance
Dali::Integration::PixelBuffer* BitmapPackedPixel::ReserveBuffer( Pixel::Format pixelFormat,
                                                                  uint32_t width,
                                                                  uint32_t height,
                                                                  uint32_t bufferWidth,
                                                                  uint32_t bufferHeight )
{
  // delete existing buffer
  DeletePixelBuffer();

  Initialize(pixelFormat, width, height, bufferWidth, bufferHeight);

  //allocate buffer
  uint32_t bufSize = mBufferWidth * mBufferHeight * mBytesPerPixel;

  mData = reinterpret_cast< Dali::Integration::PixelBuffer* >( malloc( bufSize) );

  return mData;
}

void BitmapPackedPixel::AssignBuffer( Pixel::Format pixelFormat,
                                      Dali::Integration::PixelBuffer* buffer,
                                      uint32_t bufferSize,
                                      uint32_t width,
                                      uint32_t height,
                                      uint32_t bufferWidth,
                                      uint32_t bufferHeight)
{
  DALI_ASSERT_DEBUG( buffer );

  // delete existing buffer
  DeletePixelBuffer();

  Initialize( pixelFormat, width, height, bufferWidth, bufferHeight );

   // make sure the buffer size matches what is being passed in
  DALI_ASSERT_DEBUG( bufferSize ==  (mBufferWidth * mBufferHeight * mBytesPerPixel))

  mData = buffer;
}

void BitmapPackedPixel::TestForTransparency()
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);

  mAlphaChannelUsed = false;

  if(HasAlphaChannel())
  {
    uint8_t* pixelBuffer=GetBuffer();
    if(pixelBuffer != NULL)
    {
      uint8_t* row = pixelBuffer;

      int32_t byte; int32_t bits;
      Pixel::GetAlphaOffsetAndMask(mPixelFormat, byte, bits);

      int32_t stride       = mBufferWidth * mBytesPerPixel;
      int32_t pixelsPerRow = mImageWidth;

      for(uint32_t j=0; j<mImageHeight; j++)
      {
        uint8_t* pixels = row;
        for(int32_t i=0; i<pixelsPerRow; i++)
        {
          if((pixels[byte] & bits) != bits)
          {
            mAlphaChannelUsed = true;
            j=mImageHeight; // break out of outer loop
            break;
          }
          pixels+=mBytesPerPixel;
        }
        row += stride;
      }
    }
  }
}

BitmapPackedPixel::~BitmapPackedPixel()
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);
}

void BitmapPackedPixel::Initialize( Pixel::Format pixelFormat,
                                    uint32_t width,
                                    uint32_t height,
                                    uint32_t bufferWidth,
                                    uint32_t bufferHeight)
{
  Dali::Integration::Bitmap::Initialize(pixelFormat, width, height);
  mBufferWidth  = (bufferWidth  != 0) ? bufferWidth  : width;
  mBufferHeight = (bufferHeight != 0) ? bufferHeight : height;
  mBytesPerPixel = Pixel::GetBytesPerPixel(pixelFormat);
  DALI_ASSERT_DEBUG(mBufferWidth >= mImageWidth && mBufferHeight >= mImageHeight);
}

uint32_t BitmapPackedPixel::GetBufferStride() const
{
  return mBufferWidth*mBytesPerPixel;
}



} //namespace Integration

} //namespace Dali
