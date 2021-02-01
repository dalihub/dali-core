#ifndef DALI_INTERNAL_COMPRESSED_BITMAP_H
#define DALI_INTERNAL_COMPRESSED_BITMAP_H

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
class BitmapCompressed;
using BitmapCompressedPtr = IntrusivePtr<BitmapCompressed>;

/**
 * BitmapCompressed class.
 * A container for image data that remains in compresssed form as an opaque blob
 * in memory rather than being decompressed at load time.
 * Used for formats that are supported as GLES texture data directly.
 */
class BitmapCompressed : public Dali::Integration::Bitmap, Dali::Integration::Bitmap::CompressedProfile
{
public:
  /**
   * Constructor
   * @param[in] discardable Flag to tell the bitmap if it can delete the buffer with the pixel data.
   */
  BitmapCompressed(ResourcePolicy::Discardable discardable = ResourcePolicy::OWNED_RETAIN);

  const Bitmap::CompressedProfile* GetCompressedProfile() const override
  {
    return this;
  }
  Bitmap::CompressedProfile* GetCompressedProfile() override
  {
    return this;
  }

private:
  /**
   * Initializes internal class members
   * @param[in] pixelFormat   pixel format
   * @param[in] width         Image width in pixels
   * @param[in] height        Image height in pixels
   * @param[in] bufferSize    Buffer cpacity in pixels
   */
  void Initialize(Pixel::Format pixelFormat,
                  uint32_t      width,
                  uint32_t      height,
                  uint32_t      bufferSize);

public:
  /**
   * (Re-)Allocate pixel buffer for the Bitmap. Any previously allocated pixel buffer
   * is deleted.
   * Dali has ownership of the buffer, and contents are opaque and immutable.
   * Bitmap stores given size information about the image which the client is assumed
   * to have retrieved from out-of-band image metadata.
   * @param[in] pixelFormat   pixel format
   * @param[in] width         Image width in pixels
   * @param[in] height        Image height in pixels
   * @param[in] bufferSize    Buffer size in bytes
   * @return pixel buffer pointer
   */
  Dali::Integration::PixelBuffer* ReserveBufferOfSize(Pixel::Format  pixelFormat,
                                                      const uint32_t width,
                                                      const uint32_t height,
                                                      const uint32_t numBytes) override;

  /**
   * Get the pixel buffer size in bytes
   * @return The buffer size in bytes.
   */
  uint32_t GetBufferSize() const override
  {
    return mBufferSize;
  }

  /**
   * See Dali::Integration::Bitmap::GetReleaseFunction()
   */
  ReleaseFunction GetReleaseFunction() override
  {
    return FREE;
  }

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~BitmapCompressed() override;

private:
  uint32_t mBufferSize;

  BitmapCompressed(const BitmapCompressed& other);            ///< defined private to prevent use
  BitmapCompressed& operator=(const BitmapCompressed& other); ///< defined private to prevent use

  // Changes scope, should be at end of class
  DALI_LOG_OBJECT_STRING_DECLARATION;
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_COMPRESSED_BITMAP_H
