#ifndef DALI_CORE_PIXEL_BUFFER_DEVEL_H
#define DALI_CORE_PIXEL_BUFFER_DEVEL_H

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

// INTERNAL INCLUDES
#include <dali/public-api/images/pixel-buffer.h>
#include <dali/public-api/object/property-map.h>

namespace DALI_NAMESPACE
{
namespace DevelPixelBuffer
{
/**
 * @brief Converts a PixelBuffer into a PixelData, which can be uploaded to a texture.
 *
 * The buffer is handed over to the new PixelData without being copied, and the given
 * handle is reset to an empty handle.
 *
 * @warning Any other handle that keeps a reference to the same object is left with no
 * buffer. GetBuffer() returns nullptr for such a handle, and the sizes it reports are 0.
 *
 * @param[in,out] pixelBuffer The pixel buffer to convert. Reset to an empty handle on return
 * @param[in] releaseAfterUpload Whether the converted PixelData releases its buffer as soon
 * as it has been uploaded to a texture. Such a PixelData cannot be uploaded twice
 * @return A new PixelData which owns the pixel buffer's buffer
 */
DALI_CORE_API PixelData Convert(PixelBuffer& pixelBuffer, bool releaseAfterUpload);

/**
 * @brief Copies the data of the given pixel buffer into a new PixelData object, which
 * could be used for uploading to a texture.
 *
 * Unlike Convert(), the given pixel buffer keeps its buffer, so it can be edited and
 * copied again.
 *
 * @param[in] pixelBuffer The pixel buffer to copy
 * @return A new PixelData object containing a copy of the pixel buffer's data
 */
DALI_CORE_API PixelData CreatePixelData(PixelBuffer pixelBuffer);

/**
 * @brief Returns the Exif metadata of the given pixel buffer as a property map.
 *
 * @param[in] pixelBuffer The pixel buffer to read the metadata from
 * @param[out] metadata Property map object to write into
 * @return True on success
 */
DALI_CORE_API bool GetMetadata(PixelBuffer pixelBuffer, Property::Map& metadata);

} // namespace DevelPixelBuffer

} //namespace DALI_NAMESPACE

#endif // DALI_CORE_PIXEL_BUFFER_DEVEL_H
