#ifndef DALI_INTERNAL_PIXEL_DATA_H
#define DALI_INTERNAL_PIXEL_DATA_H

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

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/pixel-data-integ.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/object/base-object.h>

namespace Dali
{
namespace Internal
{
class PixelData;
using PixelDataPtr = IntrusivePtr<PixelData>;

class PixelData : public BaseObject
{
public:
  /**
   * @brief Create a PixelData object.
   *
   * @param [in] buffer           The raw pixel data.
   * @param [in] bufferSize       The size of the buffer in bytes
   * @param [in] width            Buffer width in pixels
   * @param [in] height           Buffer height in pixels
   * @param [in] stride           Buffer stride in pixels, 0 means the buffer is tightly packed
   * @param [in] pixelFormat      The pixel format
   * @param [in] releaseFunction  The function used to release the memory.
   * @param [in] releaseAfterUpload Whether we release buffer after texture upload or not.
   */
  static PixelDataPtr New(uint8_t*                         buffer,
                          uint32_t                         bufferSize,
                          uint32_t                         width,
                          uint32_t                         height,
                          uint32_t                         stride,
                          Pixel::Format                    pixelFormat,
                          Dali::PixelData::ReleaseFunction releaseFunction,
                          bool                             releaseAfterUpload);

  /**
   * @brief Constructor.
   *
   * @param [in] buffer             The raw pixel data.
   * @param [in] bufferSize         The size of the buffer in bytes
   * @param [in] width              Buffer width in pixels
   * @param [in] height             Buffer height in pixels
   * @param [in] stride             Buffer stride in pixels, 0 means the buffer is tightly packed
   * @param [in] pixelFormat        The pixel format
   * @param [in] releaseFunction    The function used to release the memory.
   * @param [in] releaseAfterUpload Whether we release buffer after texture upload or not.
   */
  PixelData(uint8_t*                         buffer,
            uint32_t                         bufferSize,
            uint32_t                         width,
            uint32_t                         height,
            uint32_t                         stride,
            Pixel::Format                    pixelFormat,
            Dali::PixelData::ReleaseFunction releaseFunction,
            bool                             releaseAfterUpload);

protected:
  /**
   * @brief Destructor.
   *
   * Release the pixel buffer if exists.
   */
  ~PixelData() override;

public:
  /**
   * Get the width of the buffer in pixels.
   * @return The width of the buffer in pixels
   */
  uint32_t GetWidth() const;

  /**
   * Get the height of the buffer in pixels
   * @return The height of the buffer in pixels
   */
  uint32_t GetHeight() const;

  /**
   * Get the pixel format
   * @return The pixel format
   */
  Pixel::Format GetPixelFormat() const;

  /**
   * Get the pixel buffer if it's present.
   * @return The buffer if exists, or NULL if there is no pixel buffer.
   */
  uint8_t* GetBuffer() const;

  /**
   * Get the size of the buffer in bytes
   * @return The size of the buffer
   */
  uint32_t GetBufferSize() const;

  /**
   * Release the buffer data and reset the internal buffer to zero.
   */
  void ReleasePixelDataBuffer();

  /**
   * Return the buffer pointer.
   * @return The buffer pointer.
   */
  Dali::Integration::PixelDataBuffer GetPixelDataBuffer() const;

  /**
   * Get whether we need to release pixel data after texture upload or not.
   * @note This function can be called from another thread. Be careful.
   * @return True if we need to release pixel data after texture upload. False otherwise.
   */
  bool IsPixelDataReleaseAfterUpload() const
  {
    return mReleaseAfterUpload;
  }

  /**
   * @copydoc PixelData::GetStride()
   */
  uint32_t GetStride() const;

  /**
   * @copydoc PixelData::GetStrideBytes()
   */
  uint32_t GetStrideBytes() const;

  /**
   * Class method to get the total currently allocated size of pixel buffers
   */
  static uint32_t GetTotalAllocatedSize()
  {
#if defined(DEBUG_ENABLED)
    return gPixelDataAllocationTotal;
#else
    return 0;
#endif
  }

private:
  /*
   * Undefined copy constructor.
   */
  PixelData(const PixelData& other);

  /*
   * Undefined assignment operator.
   */
  PixelData& operator=(const PixelData& other);

private:
  uint8_t*                         mBuffer;          ///< The raw pixel data
  uint32_t                         mBufferSize;      ///< Buffer size in bytes
  uint32_t                         mWidth;           ///< Buffer width in pixels
  uint32_t                         mHeight;          ///< Buffer height in pixels
  uint32_t                         mStrideBytes;     ///< Buffer stride in bytes, 0 means the buffer is tightly packed
  Pixel::Format                    mPixelFormat;     ///< Pixel format
  Dali::PixelData::ReleaseFunction mReleaseFunction; ///< Function for releasing memory

  const bool mReleaseAfterUpload;

#if defined(DEBUG_ENABLED)
  static uint32_t gPixelDataAllocationTotal;
#endif
};

} // namespace Internal

/**
 * Helper methods for public API
 */
inline Internal::PixelData& GetImplementation(Dali::PixelData& handle)
{
  DALI_ASSERT_ALWAYS(handle && "handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::PixelData&>(object);
}

inline const Internal::PixelData& GetImplementation(const Dali::PixelData& handle)
{
  DALI_ASSERT_ALWAYS(handle && "handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::PixelData&>(object);
}

} // namespace Dali

#endif // __DALI_INTERNAL_PIXEL_DATA_H__
