#ifndef DALI_PIXEL_DATA_INTEG_H
#define DALI_PIXEL_DATA_INTEG_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <cstdint>

// INTERNAL INCLUDES
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/images/pixel.h>

namespace Dali::Integration
{
/**
 * Struct to keep the buffer pointer and the allocation method.
 *
 * @note Need to find a better solution - one library should not
 * be freeing data from a different source with potentially
 * different allocators.
 */
struct PixelDataBuffer
{
  uint8_t*                         buffer;
  uint32_t                         bufferSize;
  Dali::PixelData::ReleaseFunction releaseFunction;

  PixelDataBuffer(uint8_t*                         buffer,
                  uint32_t                         bufferSize,
                  Dali::PixelData::ReleaseFunction releaseFunction)
  : buffer(buffer),
    bufferSize(bufferSize),
    releaseFunction(releaseFunction)
  {
  }
};

/**
 * Get the buffer from a pixel data object, zero it in the pixel data object
 * and release the handle.
 * @param[in,out] pixelData The pixel data object to take the buffer from
 * @return the buffer and the data release mechanism
 */
DALI_CORE_API PixelDataBuffer ReleasePixelDataBuffer(Dali::PixelData& pixelData);

/**
 * Get the buffer from a pixel data object.
 * @param[in] pixelData The pixel data object to get the buffer from
 * @return the buffer of pixelData.
 */
DALI_CORE_API PixelDataBuffer GetPixelDataBuffer(const Dali::PixelData& pixelData);

} // namespace Dali::Integration

#endif // DALI_PIXEL_DATA_INTEG_H
