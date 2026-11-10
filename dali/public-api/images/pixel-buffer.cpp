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
#include <dali/public-api/images/pixel-buffer.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/event/images/pixel-buffer-impl.h>

namespace DALI_NAMESPACE
{
PixelBuffer PixelBuffer::New(uint32_t      width,
                             uint32_t      height,
                             Pixel::Format pixelFormat)
{
  Internal::PixelBufferPtr internal =
    Internal::PixelBuffer::New(width, height, pixelFormat);
  return PixelBuffer(internal.Get());
}

PixelBuffer::PixelBuffer()
{
}

PixelBuffer::~PixelBuffer()
{
}

PixelBuffer::PixelBuffer(Internal::PixelBuffer* internal)
: BaseHandle(internal)
{
}

PixelBuffer::PixelBuffer(const PixelBuffer& handle) = default;

PixelBuffer& PixelBuffer::operator=(const PixelBuffer& rhs) = default;

PixelBuffer::PixelBuffer(PixelBuffer&& handle) noexcept = default;

PixelBuffer& PixelBuffer::operator=(PixelBuffer&& rhs) noexcept = default;

Dali::PixelData PixelBuffer::Convert(PixelBuffer& pixelBuffer)
{
  Dali::PixelData pixelData = Internal::PixelBuffer::Convert(GetImplementation(pixelBuffer), false);
  pixelBuffer.Reset();
  return pixelData;
}

uint8_t* PixelBuffer::GetBuffer()
{
  return GetImplementation(*this).GetBuffer();
}

const uint8_t* PixelBuffer::GetBuffer() const
{
  return GetImplementation(*this).GetConstBuffer();
}

uint32_t PixelBuffer::GetBufferSize() const
{
  return GetImplementation(*this).GetBufferSize();
}

uint32_t PixelBuffer::GetWidth() const
{
  return GetImplementation(*this).GetWidth();
}

uint32_t PixelBuffer::GetHeight() const
{
  return GetImplementation(*this).GetHeight();
}

uint32_t PixelBuffer::GetStrideBytes() const
{
  return GetImplementation(*this).GetStrideBytes();
}

Pixel::Format PixelBuffer::GetPixelFormat() const
{
  return GetImplementation(*this).GetPixelFormat();
}

void PixelBuffer::SetAlphaPreMultiplied(bool preMultiplied)
{
  GetImplementation(*this).SetPreMultiplied(preMultiplied);
}

bool PixelBuffer::IsAlphaPreMultiplied() const
{
  return GetImplementation(*this).IsAlphaPreMultiplied();
}

bool PixelBuffer::ApplyGaussianBlur(float blurRadius)
{
  auto gaussianBlurFunction = Internal::PixelBuffer::GetGaussianBlurFunction();
  if(DALI_LIKELY(gaussianBlurFunction))
  {
    return gaussianBlurFunction(*this, blurRadius);
  }
  DALI_LOG_ERROR("No Gaussian blur operation is registered. The platform layer library is required.\n");
  return false;
}

bool PixelBuffer::Resize(uint16_t width, uint16_t height)
{
  auto resizeFunction = Internal::PixelBuffer::GetResizeFunction();
  if(DALI_LIKELY(resizeFunction))
  {
    return resizeFunction(*this, width, height);
  }
  DALI_LOG_ERROR("No resize operation is registered. The platform layer library is required.\n");
  return false;
}

} //namespace DALI_NAMESPACE
