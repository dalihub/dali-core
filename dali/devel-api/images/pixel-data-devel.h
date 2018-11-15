#ifndef DALI_PIXEL_DATA_DEVEL_H
#define DALI_PIXEL_DATA_DEVEL_H

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

// EXTERNAL INCLUDES
#include <cstdint>

// INTERNAL INCLUDES
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/images/pixel.h>

namespace Dali
{

namespace DevelPixelData
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
  PixelData::ReleaseFunction releaseFunction;

  PixelDataBuffer(  uint8_t* buffer,
                    uint32_t bufferSize,
                    PixelData::ReleaseFunction releaseFunction )
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
PixelDataBuffer ReleasePixelDataBuffer( PixelData& pixelData );

} // namespace DevelPixelData

} // Dali

#endif // DALI_PIXEL_DATA_DEVEL_H
