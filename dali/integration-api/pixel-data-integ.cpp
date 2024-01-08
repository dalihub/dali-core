/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

// FILE HEADER
#include <dali/integration-api/pixel-data-integ.h>

// INTERNAL INCLUDES
#include <dali/internal/event/images/pixel-data-impl.h>

namespace Dali::Integration
{
void ReleasePixelDataBuffer(Dali::PixelData pixelData)
{
  Internal::PixelData& pixelDataImpl = GetImplementation(pixelData);
  pixelDataImpl.ReleasePixelDataBuffer();
}

PixelDataBuffer GetPixelDataBuffer(const Dali::PixelData& pixelData)
{
  const Internal::PixelData& pixelDataImpl   = GetImplementation(pixelData);
  PixelDataBuffer            pixelDataBuffer = pixelDataImpl.GetPixelDataBuffer();
  return pixelDataBuffer;
}

Dali::PixelData NewPixelDataWithReleaseAfterUpload(uint8_t*                   buffer,
                                                   uint32_t                   bufferSize,
                                                   uint32_t                   width,
                                                   uint32_t                   height,
                                                   uint32_t                   stride,
                                                   Pixel::Format              pixelFormat,
                                                   PixelData::ReleaseFunction releaseFunction)
{
  IntrusivePtr<Internal::PixelData> internal = Internal::PixelData::New(buffer, bufferSize, width, height, stride, pixelFormat, releaseFunction, true);
  return PixelData(internal.Get());
}

bool IsPixelDataReleaseAfterUpload(const Dali::PixelData& pixelData)
{
  const Internal::PixelData& pixelDataImpl = GetImplementation(pixelData);
  return pixelDataImpl.IsPixelDataReleaseAfterUpload();
}
} // namespace Dali::Integration
