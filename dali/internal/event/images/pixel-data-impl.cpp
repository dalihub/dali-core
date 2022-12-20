/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

#include <dali/integration-api/debug.h>

#if defined(DEBUG_ENABLED)
Debug::Filter* gPixelDataLogFilter = Debug::Filter::New(Debug::NoLogging, false, "DALI_LOG_PIXEL_DATA_SIZE");
#endif

namespace Dali
{
namespace Internal
{
#if defined(DEBUG_ENABLED)
// Only track data allocation if debug is enabled
uint32_t PixelData::gPixelDataAllocationTotal{0};
#endif

PixelData::PixelData(uint8_t*                         buffer,
                     uint32_t                         bufferSize,
                     uint32_t                         width,
                     uint32_t                         height,
                     uint32_t                         stride,
                     Pixel::Format                    pixelFormat,
                     Dali::PixelData::ReleaseFunction releaseFunction)
: mBuffer(buffer),
  mBufferSize(bufferSize),
  mWidth(width),
  mHeight(height),
  mStride(stride),
  mPixelFormat(pixelFormat),
  mReleaseFunction(releaseFunction)
{
  DALI_LOG_INFO(gPixelDataLogFilter, Debug::Concise, "Allocated PixelData of size %u\n", bufferSize);
#if defined(DEBUG_ENABLED)
  gPixelDataAllocationTotal += mBufferSize;
#endif
}

PixelData::~PixelData()
{
  if(mBuffer)
  {
    if(mReleaseFunction == Dali::PixelData::FREE)
    {
      free(mBuffer);
    }
    else
    {
      delete[] mBuffer;
    }
#if defined(DEBUG_ENABLED)
    gPixelDataAllocationTotal -= mBufferSize;
#endif
  }
}

PixelDataPtr PixelData::New(uint8_t*                         buffer,
                            uint32_t                         bufferSize,
                            uint32_t                         width,
                            uint32_t                         height,
                            uint32_t                         stride,
                            Pixel::Format                    pixelFormat,
                            Dali::PixelData::ReleaseFunction releaseFunction)
{
  return new PixelData(buffer, bufferSize, width, height, stride, pixelFormat, releaseFunction);
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
  mBuffer = nullptr;
  return pixelDataBuffer;
}

uint32_t PixelData::GetStride() const
{
  return mStride;
}

} // namespace Internal

} // namespace Dali
