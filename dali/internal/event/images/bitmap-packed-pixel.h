#ifndef DALI_INTERNAL_BITMAP_H
#define DALI_INTERNAL_BITMAP_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/integration-api/bitmap.h>

namespace Dali
{
namespace Internal
{
class BitmapPackedPixel;
using BitmapPackedPixelPtr = IntrusivePtr<BitmapPackedPixel>;

/**
 * BitmapPackedPixel class.
 * A container for image data that is packed into individual struct-like
 * pixels in an addressable 2D array, with each pixel occupying a whole
 * number of bytes.
 * This is a vanilla Bitmap class, typically used to hold data decompressed
 * from PNG and JPEG file formats for example.
 */
class BitmapPackedPixel : public Dali::Integration::Bitmap, Dali::Integration::Bitmap::PackedPixelsProfile
{
public:
  /**
   * Constructor
   * @param[in] discardable Flag to tell the bitmap if it can delete the buffer with the pixel data.
   */
  BitmapPackedPixel(ResourcePolicy::Discardable discardable = ResourcePolicy::OWNED_RETAIN, Dali::Integration::PixelBuffer* pixBuf = nullptr);

public:
  const Bitmap::PackedPixelsProfile* GetPackedPixelsProfile() const override
  {
    return this;
  }
  Bitmap::PackedPixelsProfile* GetPackedPixelsProfile() override
  {
    return this;
  }

  /**
   * (Re-)Allocate pixel buffer for the Bitmap. Any previously allocated pixel buffer is deleted.
   * Dali has ownership of the buffer, but its contents can be modified.
   * Bitmap stores given size information about the image.
   * @pre bufferWidth, bufferHeight have to be power of two
   * @param[in] pixelFormat   pixel format
   * @param[in] width         Image width in pixels
   * @param[in] height        Image height in pixels
   * @param[in] bufferWidth   Buffer width (stride) in pixels
   * @param[in] bufferHeight  Buffer height in pixels
   * @return pixel buffer pointer
   */
  Dali::Integration::PixelBuffer* ReserveBuffer(Pixel::Format pixelFormat,
                                                uint32_t      width,
                                                uint32_t      height,
                                                uint32_t      bufferWidth  = 0,
                                                uint32_t      bufferHeight = 0) override;

  /**
   * Assign a pixel buffer. Any previously allocated pixel buffer is deleted.
   * Dali has ownership of the buffer, but its contents can be modified.
   * Bitmap stores given size information about the image.
   * @pre bufferWidth, bufferHeight have to be power of two
   * @param[in] pixelFormat   pixel format
   * @param[in] buffer        the pixel buffer
   * @param[in] bufferSize    size of the pixel buffer
   * @param[in] width         Image width in pixels
   * @param[in] height        Image height in pixels
   * @param[in] bufferWidth   Buffer width (stride) in pixels
   * @param[in] bufferHeight  Buffer height in pixels
   */
  void AssignBuffer(Pixel::Format                   pixelFormat,
                    Dali::Integration::PixelBuffer* buffer,
                    uint32_t                        bufferSize,
                    uint32_t                        width,
                    uint32_t                        height,
                    uint32_t                        bufferWidth  = 0,
                    uint32_t                        bufferHeight = 0) override;

  /**
   * Get the width of the buffer (stride)
   * @return The width of the buffer in pixels
   */
  unsigned GetBufferWidth() const override
  {
    return mBufferWidth;
  }

  /**
   * Get the height of the buffer
   * @return The height of the buffer in pixels
   */
  unsigned GetBufferHeight() const override
  {
    return mBufferHeight;
  }

  /**
   * Get the pixel buffer size in bytes
   * @return The buffer size in bytes.
   */
  uint32_t GetBufferSize() const override
  {
    return mBufferWidth * mBytesPerPixel * mBufferHeight;
  }

  /**
   * See Dali::Integration::Bitmap::GetReleaseFunction()
   */
  ReleaseFunction GetReleaseFunction() override
  {
    return FREE;
  }

  /**
   * Get the pixel buffer stride.
   * @return The buffer stride (in bytes).
   */
  uint32_t GetBufferStride() const override;

  /**
   * Get the pixel format
   * @return The pixel format
   */
  Pixel::Format GetPixelFormat() const
  {
    return mPixelFormat;
  }

  /**
   * Check the bitmap data and test whether it has any transparent pixels.
   * This property can then be tested for with IsFullyOpaque().
   */
  void TestForTransparency() override;

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~BitmapPackedPixel() override;

protected:
  uint32_t mBufferWidth;   ///< Buffer width (stride) in pixels
  uint32_t mBufferHeight;  ///< Buffer height in pixels
  uint32_t mBytesPerPixel; ///< Bytes per pixel

private:
  /**
   * Initializes internal class members
   * @param[in] pixelFormat   pixel format
   * @param[in] width         Image width in pixels
   * @param[in] height        Image height in pixels
   * @param[in] bufferWidth   Buffer width (stride) in pixels
   * @param[in] bufferHeight  Buffer height in pixels
   */
  void Initialize(Pixel::Format pixelFormat,
                  uint32_t      width,
                  uint32_t      height,
                  uint32_t      bufferWidth,
                  uint32_t      bufferHeight);

  BitmapPackedPixel(const BitmapPackedPixel& other);            ///< defined private to prevent use
  BitmapPackedPixel& operator=(const BitmapPackedPixel& other); ///< defined private to prevent use

  // Changes scope, should be at end of class
  DALI_LOG_OBJECT_STRING_DECLARATION;
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_BITMAP_H
