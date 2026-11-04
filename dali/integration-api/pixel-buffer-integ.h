#ifndef DALI_PIXEL_BUFFER_INTEG_H
#define DALI_PIXEL_BUFFER_INTEG_H

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

// EXTERNAL INCLUDES
#include <cstdint>

// INTERNAL INCLUDES
#include <dali/public-api/images/pixel-buffer.h>
#include <dali/public-api/images/pixel.h>
#include <dali/public-api/object/property-map.h>

namespace DALI_NAMESPACE::Integration
{
/**
 * Creates a PixelBuffer object which takes over the given buffer.
 * @warning The buffer must have been allocated with malloc(), since it is released
 * with free().
 * @param[in] buffer The raw pixel data, allocated with malloc()
 * @param[in] bufferSize The size of the buffer in bytes
 * @param[in] width Buffer width in pixels
 * @param[in] height Buffer height in pixels
 * @param[in] strideBytes Buffer stride in bytes, 0 means the buffer is tightly packed
 * @param[in] pixelFormat The pixel format
 * @return The pixel buffer object.
 */
DALI_CORE_API Dali::PixelBuffer NewPixelBuffer(uint8_t*      buffer,
                                               uint32_t      bufferSize,
                                               uint32_t      width,
                                               uint32_t      height,
                                               uint32_t      strideBytes,
                                               Pixel::Format pixelFormat);

/**
 * Replaces the buffer and the geometry of a pixel buffer object, keeping its metadata
 * and pre-multiplied flag. The current buffer is released with free().
 * @warning The buffer must have been allocated with malloc(), since it is released
 * with free().
 * @param[in] pixelBuffer The pixel buffer object to reset
 * @param[in] buffer The raw pixel data, allocated with malloc()
 * @param[in] bufferSize The size of the buffer in bytes
 * @param[in] width Buffer width in pixels
 * @param[in] height Buffer height in pixels
 * @param[in] strideBytes Buffer stride in bytes, 0 means the buffer is tightly packed
 * @param[in] pixelFormat The pixel format
 */
DALI_CORE_API void ResetPixelBufferData(Dali::PixelBuffer pixelBuffer,
                                        uint8_t*          buffer,
                                        uint32_t          bufferSize,
                                        uint32_t          width,
                                        uint32_t          height,
                                        uint32_t          strideBytes,
                                        Pixel::Format     pixelFormat);

/**
 * Moves the buffer, geometry, metadata and pre-multiplied flag of the source pixel
 * buffer object into the destination one. The destination's current buffer is released
 * with free(), and the source object is left without a buffer.
 * @param[in] destination The pixel buffer object taking over the data
 * @param[in] source The pixel buffer object to take the data from
 */
DALI_CORE_API void TakePixelBufferOwnership(Dali::PixelBuffer destination, Dali::PixelBuffer source);

/**
 * Allocates a fixed amount of memory in a pixel buffer object, releasing any current
 * buffer. Used by compressed formats, whose buffer size cannot be derived from the
 * dimensions and the pixel format.
 * @note The memory is allocated with malloc().
 * @param[in] pixelBuffer The pixel buffer object to allocate the memory in
 * @param[in] size Size of memory to be allocated in bytes
 */
DALI_CORE_API void AllocatePixelBufferFixedSize(Dali::PixelBuffer pixelBuffer, uint32_t size);

/**
 * Sets whether the colours of a pixel buffer have been multiplied by the alpha value.
 * @param[in] pixelBuffer The pixel buffer object to mark
 * @param[in] preMultiplied True if the buffer holds pre-multiplied pixels
 */
DALI_CORE_API void SetPixelBufferPreMultiplied(Dali::PixelBuffer pixelBuffer, bool preMultiplied);

/**
 * Sets the image metadata, such as Exif fields, of a pixel buffer object.
 * @param[in] pixelBuffer The pixel buffer object to set the metadata on
 * @param[in] metadata Property map containing the metadata fields
 */
DALI_CORE_API void SetPixelBufferMetadata(Dali::PixelBuffer pixelBuffer, Property::Map metadata);

/**
 * Gets the total allocated size of the current pixel buffers.
 * @note Returns 0 unless the library is built with DEBUG_ENABLED.
 * @return The total allocated size in bytes.
 */
DALI_CORE_API uint32_t GetPixelBufferAllocationTotal();

/**
 * The function that performs Dali::PixelBuffer::ApplyGaussianBlur().
 */
using PixelBufferGaussianBlurFunction = bool (*)(Dali::PixelBuffer& pixelBuffer, float blurRadius);

/**
 * The function that performs Dali::PixelBuffer::Resize().
 */
using PixelBufferResizeFunction = bool (*)(Dali::PixelBuffer& pixelBuffer, uint16_t width, uint16_t height);

/**
 * Registers the image operations that back the deprecated member functions
 * Dali::PixelBuffer::ApplyGaussianBlur() and Dali::PixelBuffer::Resize(), whose
 * implementations live in the platform layer. The platform layer registers them when
 * its library is loaded. While no functions are registered, the member functions fail
 * and return false.
 * @note Temporary: this registration is removed together with the deprecated member
 * functions.
 * @param[in] gaussianBlurFunction Function performing the Gaussian blur
 * @param[in] resizeFunction Function performing the resize
 */
DALI_CORE_API void RegisterPixelBufferImageOperations(PixelBufferGaussianBlurFunction gaussianBlurFunction,
                                                      PixelBufferResizeFunction       resizeFunction);

} //namespace DALI_NAMESPACE::Integration

#endif // DALI_PIXEL_BUFFER_INTEG_H
