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
#include <dali/devel-api/images/pixel-buffer-devel.h>

// INTERNAL INCLUDES
#include <dali/internal/event/images/pixel-buffer-impl.h>

namespace DALI_NAMESPACE
{
namespace DevelPixelBuffer
{
PixelData Convert(PixelBuffer& pixelBuffer, bool releaseAfterUpload)
{
  Dali::PixelData pixelData = Internal::PixelBuffer::Convert(GetImplementation(pixelBuffer), releaseAfterUpload);
  pixelBuffer.Reset();
  return pixelData;
}

PixelData CreatePixelData(PixelBuffer pixelBuffer)
{
  return GetImplementation(pixelBuffer).CreatePixelData();
}

bool GetMetadata(PixelBuffer pixelBuffer, Property::Map& metadata)
{
  return GetImplementation(pixelBuffer).GetMetadata(metadata);
}

bool IsAlphaPreMultiplied(PixelBuffer pixelBuffer)
{
  return GetImplementation(pixelBuffer).IsAlphaPreMultiplied();
}

} // namespace DevelPixelBuffer

} //namespace DALI_NAMESPACE
