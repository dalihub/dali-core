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
#include <dali/internal/event/images/pixel-data-impl.h>

namespace Dali
{

namespace Internal
{

PixelData::PixelData( uint8_t* buffer,
                      uint32_t bufferSize,
                      uint32_t width,
                      uint32_t height,
                      Pixel::Format pixelFormat,
                      Dali::PixelData::ReleaseFunction releaseFunction )
: mBuffer( buffer ),
  mBufferSize( bufferSize ),
  mWidth( width ),
  mHeight( height ),
  mPixelFormat( pixelFormat ),
  mReleaseFunction( releaseFunction )
{
}

PixelData::~PixelData()
{
  if( mBuffer )
  {
    if( mReleaseFunction == Dali::PixelData::FREE )
    {
      free( mBuffer );
    }
    else
    {
      delete[] mBuffer;
    }
  }
 }

PixelDataPtr PixelData::New( uint8_t* buffer,
                             uint32_t bufferSize,
                             uint32_t width,
                             uint32_t height,
                             Pixel::Format pixelFormat,
                             Dali::PixelData::ReleaseFunction releaseFunction )
{
  return new PixelData( buffer, bufferSize, width, height, pixelFormat, releaseFunction );
}

uint32_t PixelData::GetWidth() const
{
  return mWidth;
}

uint32_t PixelData::GetHeight() const
{
  return mHeight;
}

Pixel::Format PixelData::GetPixelFormat() const
{
  return mPixelFormat;
}

uint8_t* PixelData::GetBuffer() const
{
  return mBuffer;
}

uint32_t PixelData::GetBufferSize() const
{
  return mBufferSize;
}

DevelPixelData::PixelDataBuffer PixelData::ReleaseBuffer()
{
  DevelPixelData::PixelDataBuffer pixelDataBuffer(mBuffer, mBufferSize, mReleaseFunction);
  mBuffer = NULL;
  return pixelDataBuffer;
}


}// namespace Internal

}// namespace Dali
