#ifndef DALI_INTERNAL_PIXEL_BUFFER_H
#define DALI_INTERNAL_PIXEL_BUFFER_H

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
#include <memory>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/pixel-buffer-integ.h>
#include <dali/public-api/images/pixel-buffer.h>
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/property-map.h>

namespace DALI_NAMESPACE
{
namespace Internal
{
class PixelBuffer;
typedef IntrusivePtr<PixelBuffer> PixelBufferPtr;

/**
 * The container for an editable pixel buffer.
 *
 * Image operations such as blur, resize, crop or masking are not implemented
 * here; the platform layer provides them on top of this container, through
 * the buffer accessors and the integration API.
 */
class PixelBuffer : public BaseObject
{
public:
  /**
   * @brief Create a PixelBuffer object with a pre-allocated buffer.
   * The PixelBuffer object owns this buffer, which may be retrieved
   * and modified using GetBuffer().
   *
   * @param [in] width            Buffer width in pixels
   * @param [in] height           Buffer height in pixels
   * @param [in] pixelFormat      The pixel format
   */
  static PixelBufferPtr New(uint32_t      width,
                            uint32_t      height,
                            Pixel::Format pixelFormat);

  /**
   * @brief Create a PixelBuffer object which takes over the given buffer. For internal use only.
   *
   * @warning The buffer must have been allocated with malloc(), since the PixelBuffer object
   * takes ownership of it and releases it with free(). There is no way to tell it to use
   * another release method, so a buffer that came from an allocator of its own - a decoder
   * library, new[], and so on - has to be copied into a malloc'd buffer first.
   * The PixelData that Convert() hands the buffer over to keeps the same contract, as it is
   * created with PixelData::FREE.
   *
   * @param [in] buffer           The raw pixel data, allocated with malloc()
   * @param [in] bufferSize       The size of the buffer in bytes
   * @param [in] width            Buffer width in pixels
   * @param [in] height           Buffer height in pixels
   * @param [in] strideBytes      Buffer stride in bytes, 0 means the buffer is tightly packed
   * @param [in] pixelFormat      The pixel format
   */
  static PixelBufferPtr New(uint8_t*      buffer,
                            uint32_t      bufferSize,
                            uint32_t      width,
                            uint32_t      height,
                            uint32_t      strideBytes,
                            Pixel::Format pixelFormat);

  /**
   * Convert a pixelBuffer object into a PixelData object.
   * The new object takes ownership of the buffer data, and the
   * mBuffer pointer is reset to nullptr.
   * @param[in] pixelBuffer The buffer to convert
   * @param[in] releaseAfterUpload Whether converted PixelData released after upload or not.
   * @return the pixelData
   */
  static Dali::PixelData Convert(PixelBuffer& pixelBuffer, bool releaseAfterUpload);

  /**
   * @brief Constructor.
   *
   * @warning The buffer must have been allocated with malloc(). @see New()
   *
   * @param [in] buffer           The raw pixel data, allocated with malloc()
   * @param [in] bufferSize       The size of the buffer in bytes
   * @param [in] width            Buffer width in pixels
   * @param [in] height           Buffer height in pixels
   * @param [in] strideBytes      Buffer stride in bytes, 0 means the buffer is tightly packed
   * @param [in] pixelFormat      The pixel format
   */
  PixelBuffer(uint8_t*      buffer,
              uint32_t      bufferSize,
              uint32_t      width,
              uint32_t      height,
              uint32_t      strideBytes,
              Pixel::Format pixelFormat);

protected:
  /**
   * @brief Destructor.
   *
   * Release the pixel buffer if exists.
   */
  ~PixelBuffer() override;

public:
  /**
   * Get the total allocated size of current pixel buffers
   */
  static uint32_t GetTotalAllocatedSize()
  {
#if defined(DEBUG_ENABLED)
    return gPixelBufferAllocationTotal;
#else
    return 0;
#endif
  }

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
   * @brief Gets the stride of the buffer in bytes.
   * @note A tightly packed buffer reports the width in bytes, since the constructor
   * resolves a 0 stride to width * bpp.
   * @return The stride of the buffer in bytes, or 0 if there is no pixel buffer.
   */
  uint32_t GetStrideBytes() const;

  /**
   * Get the pixel format
   * @return The pixel format
   */
  Pixel::Format GetPixelFormat() const;

  /**
   * Get the pixel buffer if it's present.
   * @return The buffer if exists, or nullptr if there is no pixel buffer.
   */
  uint8_t* GetBuffer() const;

  /**
   * @copydoc Dali::PixelBuffer::GetBuffer()
   */
  const uint8_t* GetConstBuffer() const;

  /**
   * Get the size of the buffer in bytes
   * @return The size of the buffer
   */
  uint32_t GetBufferSize() const;

  /**
   * Copy the buffer into a new PixelData
   */
  Dali::PixelData CreatePixelData() const;

  /**
   * @brief Returns image metadata as a property map
   * @param[out] outMetadata Property map to copy the data into
   * @return True on success
   */
  bool GetMetadata(Property::Map& outMetadata) const;

  /**
   * @brief Sets metadata property map for the pixel buffer
   * @note The function takes over the ownership of the property map
   * @param[in] metadata Property map to copy the data into
   */
  void SetMetadata(std::unique_ptr<Property::Map> metadata);

  /**
   * Allocates fixed amount of memory for the pixel data. Used by compressed formats.
   * @note The memory is allocated with malloc(), to match ReleaseBuffer().
   * @param[in] size Size of memory to be allocated
   */
  void AllocateFixedSize(uint32_t size);

  /**
   * @brief Replaces the buffer and its geometry, keeping the metadata and the
   * pre-multiplied flag.
   *
   * The current buffer is released, and this object takes ownership of the given one.
   *
   * @warning The buffer must have been allocated with malloc(). @see New()
   *
   * @param [in] buffer           The raw pixel data, allocated with malloc()
   * @param [in] bufferSize       The size of the buffer in bytes
   * @param [in] width            Buffer width in pixels
   * @param [in] height           Buffer height in pixels
   * @param [in] strideBytes      Buffer stride in bytes, 0 means the buffer is tightly packed
   * @param [in] pixelFormat      The pixel format
   */
  void ResetBufferData(uint8_t*      buffer,
                       uint32_t      bufferSize,
                       uint32_t      width,
                       uint32_t      height,
                       uint32_t      strideBytes,
                       Pixel::Format pixelFormat);

  /**
   * Takes ownership of the other object's pixel buffer, along with its geometry,
   * metadata and pre-multiplied flag. The other object is left without a buffer.
   */
  void TakeOwnershipOfBuffer(PixelBuffer& pixelBuffer);

  /**
   * @brief Sets whether the colours of this buffer have been multiplied by the alpha value.
   * @param[in] preMultiplied True if the buffer holds pre-multiplied pixels
   */
  void SetPreMultiplied(bool preMultiplied);

  /**
   * @copydoc Dali::DevelPixelBuffer::IsAlphaPreMultiplied()
   */
  bool IsAlphaPreMultiplied() const;

public: // Image operations registered by the platform layer
  /**
   * @brief Stores the image operations that back the deprecated public member functions.
   * @param[in] gaussianBlurFunction Function performing Dali::PixelBuffer::ApplyGaussianBlur()
   * @param[in] resizeFunction Function performing Dali::PixelBuffer::Resize()
   */
  static void RegisterImageOperations(Dali::Integration::PixelBufferGaussianBlurFunction gaussianBlurFunction,
                                      Dali::Integration::PixelBufferResizeFunction       resizeFunction);

  /**
   * @brief Gets the registered Gaussian blur operation, or nullptr if none is registered.
   */
  static Dali::Integration::PixelBufferGaussianBlurFunction GetGaussianBlurFunction();

  /**
   * @brief Gets the registered resize operation, or nullptr if none is registered.
   */
  static Dali::Integration::PixelBufferResizeFunction GetResizeFunction();

private:
  /*
   * Undefined copy constructor.
   */
  PixelBuffer(const PixelBuffer& other);

  /*
   * Undefined assignment operator.
   */
  PixelBuffer& operator=(const PixelBuffer& other);

  /**
   * Release the buffer with free(), whether this object allocated it itself or took it
   * over through New() / the constructor.
   */
  void ReleaseBuffer();

private:
  std::unique_ptr<Property::Map> mMetadata; ///< Metadata fields

  uint8_t*      mBuffer;      ///< The raw pixel data
  uint32_t      mBufferSize;  ///< Buffer sized in bytes
  uint32_t      mWidth;       ///< Buffer width in pixels
  uint32_t      mHeight;      ///< Buffer height in pixels
  uint32_t      mStrideBytes; ///< Buffer stride in bytes, resolved to width * bpp when tightly packed
  Pixel::Format mPixelFormat; ///< Pixel format

  bool mPreMultiplied : 1; ///< PreMultiplied

#if defined(DEBUG_ENABLED)
  static uint32_t gPixelBufferAllocationTotal;
#endif
};

} // namespace Internal

/**
 * Helper methods for public API
 */
inline Internal::PixelBuffer& GetImplementation(Dali::PixelBuffer& handle)
{
  DALI_ASSERT_ALWAYS(handle && "handle is empty");

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::PixelBuffer&>(object);
}

inline const Internal::PixelBuffer& GetImplementation(const Dali::PixelBuffer& handle)
{
  DALI_ASSERT_ALWAYS(handle && "handle is empty");

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::PixelBuffer&>(object);
}

} //namespace DALI_NAMESPACE

#endif // DALI_INTERNAL_PIXEL_BUFFER_H
