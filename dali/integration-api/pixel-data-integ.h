#ifndef DALI_PIXEL_DATA_INTEG_H
#define DALI_PIXEL_DATA_INTEG_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
  uint8_t* buffer;
  uint32_t bufferSize;
  uint32_t width;
  uint32_t height;
  uint32_t strideBytes;

  PixelDataBuffer(uint8_t* buffer,
                  uint32_t bufferSize,
                  uint32_t width,
                  uint32_t height,
                  uint32_t strideBytes = 0)
  : buffer(buffer),
    bufferSize(bufferSize),
    width(width),
    height(height),
    strideBytes(strideBytes)
  {
  }
};

/**
 * Release the buffer from a pixel data object, zero it in the pixel data object.
 * @param[in] pixelData The pixel data object to take the buffer from
 */
DALI_CORE_API void ReleasePixelDataBuffer(Dali::PixelData pixelData);

/**
 * Get the buffer from a pixel data object.
 * @param[in] pixelData The pixel data object to get the buffer from
 * @return the buffer of pixelData.
 */
DALI_CORE_API PixelDataBuffer GetPixelDataBuffer(const Dali::PixelData& pixelData);

/**
 * Creates a PixelData object which will release the buffer automatically after upload to texture.
 * @return The pixel data object.
 */
DALI_CORE_API Dali::PixelData NewPixelDataWithReleaseAfterUpload(uint8_t*                   buffer,
                                                                 uint32_t                   bufferSize,
                                                                 uint32_t                   width,
                                                                 uint32_t                   height,
                                                                 uint32_t                   strideBytes,
                                                                 Pixel::Format              pixelFormat,
                                                                 PixelData::ReleaseFunction releaseFunction);

/**
 * Get whether we need to release pixel data after texture upload or not.
 * @note This function can be called from another thread. Be careful about thread safety.
 * @param[in] pixelData The pixel data object to get the release policy.
 * @return True if we need to release pixel data after texture upload. False otherwise.
 */
DALI_CORE_API bool IsPixelDataReleaseAfterUpload(const Dali::PixelData& pixelData);

} // namespace Dali::Integration

#endif // DALI_PIXEL_DATA_INTEG_H
