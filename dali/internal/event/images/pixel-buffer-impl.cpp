/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/images/pixel-buffer-impl.h>

// EXTERNAL INCLUDES
#include <stdlib.h>
#include <cstring>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/pixel-data-integ.h>
#include <dali/integration-api/trace.h>

namespace DALI_NAMESPACE
{
namespace Internal
{
namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gPixelBufferFilter = Debug::Filter::New(Debug::NoLogging, false, "DALI_LOG_PIXEL_BUFFER_SIZE");
#endif

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_IMAGE_PERFORMANCE_MARKER, false);

Dali::Integration::PixelBufferGaussianBlurFunction gGaussianBlurFunction{nullptr};
Dali::Integration::PixelBufferResizeFunction       gResizeFunction{nullptr};
} // namespace

#if defined(DEBUG_ENABLED)
uint32_t PixelBuffer::gPixelBufferAllocationTotal{0};
#endif

PixelBuffer::PixelBuffer(uint8_t*            buffer,
                         uint32_t            bufferSize,
                         uint32_t            width,
                         uint32_t            height,
                         uint32_t            strideBytes,
                         Dali::Pixel::Format pixelFormat)
: mMetadata(),
  mBuffer(buffer),
  mBufferSize(bufferSize),
  mWidth(width),
  mHeight(height),
  mStrideBytes(strideBytes ? strideBytes : (width * Dali::Pixel::GetBytesPerPixel(pixelFormat))),
  mPixelFormat(pixelFormat),
  mPreMultiplied(false)
{
}

PixelBuffer::~PixelBuffer()
{
  ReleaseBuffer();
}

PixelBufferPtr PixelBuffer::New(uint32_t            width,
                                uint32_t            height,
                                Dali::Pixel::Format pixelFormat)
{
  const uint32_t bufferBytesPerPixel = Dali::Pixel::GetBytesPerPixel(pixelFormat);

  const uint64_t requiredSize = static_cast<uint64_t>(width) * static_cast<uint64_t>(height) * bufferBytesPerPixel;

  uint32_t bufferSize = 0u;
  uint8_t* buffer     = nullptr;
  if(DALI_UNLIKELY(requiredSize > 0xFFFFFFFFull))
  {
    DALI_LOG_ERROR("Buffer size %llu for %ux%u exceeds the maximum\n", static_cast<unsigned long long>(requiredSize), width, height);
  }
  else if(requiredSize > 0u)
  {
    bufferSize = static_cast<uint32_t>(requiredSize);
    buffer     = static_cast<uint8_t*>(malloc(bufferSize));
    if(DALI_UNLIKELY(!buffer))
    {
      DALI_LOG_ERROR("malloc is failed. request malloc size : %u\n", bufferSize);
      bufferSize = 0u; // Keep GetBufferSize() consistent with the nullptr buffer
    }
#if defined(DEBUG_ENABLED)
    else
    {
      gPixelBufferAllocationTotal += bufferSize;
    }
#endif
  }
  DALI_LOG_INFO(gPixelBufferFilter, Debug::Concise, "Allocated PixelBuffer of size %u\n", bufferSize);

  return new PixelBuffer(buffer, bufferSize, width, height, width * bufferBytesPerPixel, pixelFormat);
}

PixelBufferPtr PixelBuffer::New(uint8_t*            buffer,
                                uint32_t            bufferSize,
                                uint32_t            width,
                                uint32_t            height,
                                uint32_t            strideBytes,
                                Dali::Pixel::Format pixelFormat)
{
  return new PixelBuffer(buffer, bufferSize, width, height, strideBytes, pixelFormat);
}

Dali::PixelData PixelBuffer::Convert(PixelBuffer& pixelBuffer, bool releaseAfterUpload)
{
#if defined(DEBUG_ENABLED)
  gPixelBufferAllocationTotal -= pixelBuffer.mBufferSize;
#endif
  Dali::PixelData pixelData;
  if(releaseAfterUpload)
  {
    pixelData = Dali::Integration::NewPixelDataWithReleaseAfterUpload(pixelBuffer.mBuffer,
                                                                      pixelBuffer.mBufferSize,
                                                                      pixelBuffer.mWidth,
                                                                      pixelBuffer.mHeight,
                                                                      pixelBuffer.mStrideBytes,
                                                                      pixelBuffer.mPixelFormat,
                                                                      Dali::PixelData::FREE);
  }
  else
  {
    pixelData = Dali::PixelData::New(pixelBuffer.mBuffer,
                                     pixelBuffer.mBufferSize,
                                     pixelBuffer.mWidth,
                                     pixelBuffer.mHeight,
                                     pixelBuffer.mStrideBytes,
                                     pixelBuffer.mPixelFormat,
                                     Dali::PixelData::FREE);
  }
  pixelBuffer.mBuffer      = nullptr;
  pixelBuffer.mWidth       = 0;
  pixelBuffer.mHeight      = 0;
  pixelBuffer.mBufferSize  = 0;
  pixelBuffer.mStrideBytes = 0;

  return pixelData;
}

uint32_t PixelBuffer::GetWidth() const
{
  return mWidth;
}

uint32_t PixelBuffer::GetHeight() const
{
  return mHeight;
}

uint32_t PixelBuffer::GetStrideBytes() const
{
  return mStrideBytes;
}

Dali::Pixel::Format PixelBuffer::GetPixelFormat() const
{
  return mPixelFormat;
}

uint8_t* PixelBuffer::GetBuffer() const
{
  return mBuffer;
}

const uint8_t* PixelBuffer::GetConstBuffer() const
{
  return mBuffer;
}

uint32_t PixelBuffer::GetBufferSize() const
{
  return mBufferSize;
}

Dali::PixelData PixelBuffer::CreatePixelData() const
{
  uint8_t* destBuffer = nullptr;

  if(mBufferSize > 0)
  {
    DALI_TRACE_BEGIN_WITH_MESSAGE_GENERATOR(gTraceFilter, "DALI_PIXEL_BUFFER_CREATE_PIXEL_DATA", [&](std::ostringstream& oss)
    { oss << "[" << mWidth << "x" << mHeight << " bufferSize " << mBufferSize << " format " << mPixelFormat << "]"; });
    destBuffer = static_cast<uint8_t*>(malloc(mBufferSize));
    if(DALI_UNLIKELY(!destBuffer))
    {
      DALI_LOG_ERROR("malloc is failed. request malloc size : %u\n", mBufferSize);
      DALI_TRACE_END(gTraceFilter, "DALI_PIXEL_BUFFER_CREATE_PIXEL_DATA");
      return Dali::PixelData();
    }
    memcpy(destBuffer, mBuffer, mBufferSize);
    DALI_TRACE_END(gTraceFilter, "DALI_PIXEL_BUFFER_CREATE_PIXEL_DATA");
  }

  Dali::PixelData pixelData = Dali::PixelData::New(destBuffer, mBufferSize, mWidth, mHeight, mStrideBytes, mPixelFormat, Dali::PixelData::FREE);
  return pixelData;
}

bool PixelBuffer::GetMetadata(Property::Map& outMetadata) const
{
  if(!mMetadata)
  {
    return false;
  }
  outMetadata = *mMetadata;
  return true;
}

void PixelBuffer::SetMetadata(std::unique_ptr<Property::Map> metadata)
{
  mMetadata = std::move(metadata);
}

void PixelBuffer::AllocateFixedSize(uint32_t size)
{
  ReleaseBuffer();
  mBuffer     = reinterpret_cast<unsigned char*>(malloc(size));
  mBufferSize = size;
  if(DALI_UNLIKELY(!mBuffer))
  {
    DALI_LOG_ERROR("malloc is failed. request malloc size : %u\n", mBufferSize);
    mBufferSize = 0u; // Keep GetBufferSize() consistent with the nullptr buffer
  }
#if defined(DEBUG_ENABLED)
  else
  {
    gPixelBufferAllocationTotal += size;
  }
#endif
}

void PixelBuffer::ResetBufferData(uint8_t*      buffer,
                                  uint32_t      bufferSize,
                                  uint32_t      width,
                                  uint32_t      height,
                                  uint32_t      strideBytes,
                                  Pixel::Format pixelFormat)
{
  ReleaseBuffer();

  mBuffer      = buffer;
  mBufferSize  = bufferSize;
  mWidth       = width;
  mHeight      = height;
  mStrideBytes = strideBytes ? strideBytes : (width * Dali::Pixel::GetBytesPerPixel(pixelFormat));
  mPixelFormat = pixelFormat;

#if defined(DEBUG_ENABLED)
  gPixelBufferAllocationTotal += mBufferSize;
#endif
}

void PixelBuffer::TakeOwnershipOfBuffer(PixelBuffer& pixelBuffer)
{
  ReleaseBuffer();

  // Take ownership of new buffer
  mMetadata      = std::move(pixelBuffer.mMetadata);
  mBuffer        = pixelBuffer.mBuffer;
  mBufferSize    = pixelBuffer.mBufferSize;
  mWidth         = pixelBuffer.mWidth;
  mHeight        = pixelBuffer.mHeight;
  mStrideBytes   = pixelBuffer.mStrideBytes;
  mPixelFormat   = pixelBuffer.mPixelFormat;
  mPreMultiplied = pixelBuffer.mPreMultiplied;

  pixelBuffer.mBuffer = nullptr;
}

void PixelBuffer::SetPreMultiplied(bool preMultiplied)
{
  mPreMultiplied = preMultiplied;
}

bool PixelBuffer::IsAlphaPreMultiplied() const
{
  return mPreMultiplied;
}

void PixelBuffer::ReleaseBuffer()
{
  if(mBuffer)
  {
#if defined(DEBUG_ENABLED)
    gPixelBufferAllocationTotal -= mBufferSize;
#endif
    free(mBuffer);
    mBuffer = nullptr;
  }
}

void PixelBuffer::RegisterImageOperations(Dali::Integration::PixelBufferGaussianBlurFunction gaussianBlurFunction,
                                          Dali::Integration::PixelBufferResizeFunction       resizeFunction)
{
  gGaussianBlurFunction = gaussianBlurFunction;
  gResizeFunction       = resizeFunction;
}

Dali::Integration::PixelBufferGaussianBlurFunction PixelBuffer::GetGaussianBlurFunction()
{
  return gGaussianBlurFunction;
}

Dali::Integration::PixelBufferResizeFunction PixelBuffer::GetResizeFunction()
{
  return gResizeFunction;
}

} // namespace Internal
} //namespace DALI_NAMESPACE
