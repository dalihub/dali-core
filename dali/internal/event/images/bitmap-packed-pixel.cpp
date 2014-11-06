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
#include <dali/internal/event/images/bitmap-packed-pixel.h>

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
Dali::Integration::PixelBuffer* BitmapPackedPixel::ReserveBuffer(Pixel::Format pixelFormat,
                                    unsigned int  width,
                                    unsigned int  height,
                                    unsigned int  bufferWidth,
                                    unsigned int  bufferHeight)
{
  // delete existing buffer
  DeletePixelBuffer();

  Initialize(pixelFormat, width, height, bufferWidth, bufferHeight);

  //allocate buffer
  unsigned int bufSize = mBufferWidth * mBufferHeight * mBytesPerPixel;

  mData = new Dali::Integration::PixelBuffer[bufSize];

  return mData;
}

void BitmapPackedPixel::AssignBuffer(Pixel::Format pixelFormat,
                                     Dali::Integration::PixelBuffer* buffer,
                                     std::size_t bufferSize,
                                     unsigned int width,
                                     unsigned int height,
                                     unsigned int bufferWidth,
                                     unsigned int bufferHeight)
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
    unsigned char* pixelBuffer=GetBuffer();
    if(pixelBuffer != NULL)
    {
      unsigned char* row = pixelBuffer;

      int byte; int bits;
      Pixel::GetAlphaOffsetAndMask(mPixelFormat, byte, bits);

      int stride       = mBufferWidth * mBytesPerPixel;
      int pixelsPerRow = mImageWidth;

      for(size_t j=0; j<mImageHeight; j++)
      {
        unsigned char* pixels = row;
        for(int i=0; i<pixelsPerRow; i++)
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
                          unsigned int width,
                          unsigned int height,
                          unsigned int bufferWidth,
                          unsigned int bufferHeight)
{
  Dali::Integration::Bitmap::Initialize(pixelFormat, width, height);
  mBufferWidth  = (bufferWidth  != 0) ? bufferWidth  : width;
  mBufferHeight = (bufferHeight != 0) ? bufferHeight : height;
  mBytesPerPixel = Pixel::GetBytesPerPixel(pixelFormat);
  DALI_ASSERT_DEBUG(mBufferWidth >= mImageWidth && mBufferHeight >= mImageHeight);
}

unsigned int BitmapPackedPixel::GetBufferStride() const
{
  return mBufferWidth*mBytesPerPixel;
}



} //namespace Integration

} //namespace Dali
