#ifndef DALI_PIXEL_DATA_H
#define DALI_PIXEL_DATA_H

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
#include <cstdint> // uint8_t, uint32_t

// INTERNAL INCLUDES
#include <dali/public-api/images/pixel.h>
#include <dali/public-api/object/base-handle.h>

namespace Dali
{
/**
 * @addtogroup dali_core_images
 * @{
 */

namespace Internal
{
class PixelData;
}

/**
 * @brief The PixelData object holds a pixel buffer .
 * The PixelData takes over the ownership of the pixel buffer.
 * The buffer memory must NOT be released outside of this class, instead, the PixelData object will release it automatically when the reference count falls to zero.
 *
 * @SINCE_1_1.43
 */
class DALI_CORE_API PixelData : public BaseHandle
{
public:
  /**
   * @brief Enumeration for Function to release the pixel buffer.
   * @SINCE_1_1.43
   */
  enum ReleaseFunction
  {
    FREE,         ///< Use free function to release the pixel buffer     @SINCE_1_1.43
    DELETE_ARRAY, ///< Use delete[] operator to release the pixel buffer @SINCE_1_1.43
  };

  /**
   * @brief Creates a PixelData object.
   *
   * @SINCE_1_1.43
   * @param[in] buffer          The raw pixel data
   * @param[in] bufferSize      The size of the buffer in bytes
   * @param[in] width           Buffer width in pixels
   * @param[in] height          Buffer height in pixels
   * @param[in] pixelFormat     The pixel format
   * @param[in] releaseFunction The function used to release the memory
   * @return A handle to the PixelData
   */
  static PixelData New(uint8_t*        buffer,
                       uint32_t        bufferSize,
                       uint32_t        width,
                       uint32_t        height,
                       Pixel::Format   pixelFormat,
                       ReleaseFunction releaseFunction);

  /**
   * @brief Creates a PixelData object.
   *
   * @SINCE_2_1.10
   * @param[in] buffer          The raw pixel data
   * @param[in] bufferSize      The size of the buffer in bytes
   * @param[in] width           Buffer width in pixels
   * @param[in] height          Buffer height in pixels
   * @param[in] stride          Buffer stride in pixels, 0 means the buffer is tightly packed
   * @param[in] pixelFormat     The pixel format
   * @param[in] releaseFunction The function used to release the memory
   * @return A handle to the PixelData
   */
  static PixelData New(uint8_t*        buffer,
                       uint32_t        bufferSize,
                       uint32_t        width,
                       uint32_t        height,
                       uint32_t        stride,
                       Pixel::Format   pixelFormat,
                       ReleaseFunction releaseFunction);
  /**
   * @brief Creates an empty handle.
   * Use PixelData::New() to create an initialized object.
   *
   * @SINCE_1_1.43
   */
  PixelData();

  /**
   * @brief Destructor.
   *
   * @SINCE_1_1.43
   */
  ~PixelData();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_1.43
   * @param[in] handle A reference to the copied handle
   */
  PixelData(const PixelData& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_1.43
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this object
   */
  PixelData& operator=(const PixelData& rhs);

  /**
   * @brief Move constructor.
   *
   * @SINCE_1_9.22
   * @param[in] rhs A reference to the moved handle
   */
  PixelData(PixelData&& rhs) noexcept;

  /**
   * @brief Move assignment operator.
   *
   * @SINCE_1_9.22
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this handle
   */
  PixelData& operator=(PixelData&& rhs) noexcept;

  /**
   * @brief Gets the width of the buffer in pixels.
   *
   * @SINCE_1_1.43
   * @return The width of the buffer in pixels
   */
  uint32_t GetWidth() const;

  /**
   * @brief Gets the height of the buffer in pixels.
   *
   * @SINCE_1_1.43
   * @return The height of the buffer in pixels
   */
  uint32_t GetHeight() const;

  /**
   * @brief Gets the pixel format.
   *
   * @SINCE_1_1.43
   * @return The pixel format
   */
  Pixel::Format GetPixelFormat() const;

  /**
   * @brief Gets the stride of the buffer in pixels.
   *
   * @SINCE_2_1.10
   * @DEPRECATED_2_4.8 Use GetStrideBytes() instead.
   * @return The stride of the buffer in pixels. 0 means the buffer is tightly packed.
   */
  uint32_t GetStride() const;

  /**
   * @brief Gets the stride of the buffer in bytes.
   *
   * @SINCE_2_4.8
   * @return The stride of the buffer in bytes. 0 means the buffer is tightly packed.
   */
  uint32_t GetStrideBytes() const;

  /**
   * Class method to get the total currently allocated size of pixel buffers
   */
  static uint32_t GetTotalAllocatedSize();

public:
  /**
   * @brief The constructor.
   * @note  Not intended for application developers.
   * @SINCE_1_1.43
   * @param[in] pointer A pointer to a newly allocated PixelData
   */
  explicit DALI_INTERNAL PixelData(Internal::PixelData* pointer);
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_PIXEL_DATA_H
