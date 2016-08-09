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
#include <dali/internal/event/images/pixel-data-impl.h>

// EXTERNAL INCLUDES
#include <stdlib.h>

namespace Dali
{

namespace Internal
{

PixelData::PixelData( unsigned char* buffer,
                      unsigned int bufferSize,
                      unsigned int width,
                      unsigned int height,
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
    if( mReleaseFunction == Dali::PixelData::FREE)
    {
      free( mBuffer );
    }
    else
    {
      delete[] mBuffer;
    }
  }
 }

PixelDataPtr PixelData::New(unsigned char* buffer,
                            unsigned int bufferSize,
                            unsigned int width,
                            unsigned int height,
                            Pixel::Format pixelFormat,
                            Dali::PixelData::ReleaseFunction releaseFunction)
{
  return new PixelData( buffer, bufferSize, width, height, pixelFormat, releaseFunction );
}

unsigned int PixelData::GetWidth() const
{
  return mWidth;
}

unsigned int PixelData::GetHeight() const
{
  return mHeight;
}

Pixel::Format PixelData::GetPixelFormat() const
{
  return mPixelFormat;
}

unsigned char* PixelData::GetBuffer() const
{
  return mBuffer;
}

unsigned int PixelData::GetBufferSize() const
{
  return mBufferSize;
}

}// namespace Internal

}// namespace Dali
