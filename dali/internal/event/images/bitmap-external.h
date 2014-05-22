#ifndef __DALI_INTERNAL_BITMAP_EXTERNAL_H__
#define __DALI_INTERNAL_BITMAP_EXTERNAL_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali/internal/event/images/bitmap-packed-pixel.h>

namespace Dali
{

namespace Internal
{

/**
 * Bitmap class.
 * A container for external image data
 */
class BitmapExternal : public BitmapPackedPixel
{
public:
  /**
   * Constructor
   */

  /**
   * Creates new BitmapExternal instance with pixel buffer pointer and details.
   * Application has ownership of the buffer, its contents can be modified.
   * Bitmap stores given size information about the image.
   * @pre bufferWidth, bufferHeight have to be power of two
   * @param[in] pixBuf        pointer to external pixel buffer
   * @param[in] width         Image width in pixels
   * @param[in] height        Image height in pixels
   * @param[in] pixelformat   pixel format
   * @param[in] bufferWidth   Buffer width in pixels
   * @param[in] bufferHeight  Buffer height in pixels
   */
  BitmapExternal(Dali::Integration::PixelBuffer* pixBuf,
                 unsigned int width,
                 unsigned int height,
                 Pixel::Format pixelformat,
                 unsigned int bufferWidth  = 0,
                 unsigned int bufferHeight = 0);

  /**
   * This does nothing, data is owned by external application.
   */
  virtual Dali::Integration::PixelBuffer* ReserveBuffer(Pixel::Format pixelFormat,
                                     unsigned int width,
                                     unsigned int height,
                                     unsigned int bufferWidth = 0,
                                     unsigned int bufferHeight = 0)
  {
    return NULL;
  }

  /**
   * Get the pixel buffer
   * @return The buffer. You can modify its contents.
   */
  virtual Dali::Integration::PixelBuffer* GetBuffer()
  {
    return mExternalData;
  }

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~BitmapExternal();

private:

  Dali::Integration::PixelBuffer* mExternalData; ///< Externally owned pixel data

private:
  BitmapExternal();  ///< defined private to prevent use
  BitmapExternal(const BitmapExternal& other);  ///< defined private to prevent use
  BitmapExternal& operator = (const BitmapExternal& other); ///< defined private to prevent use

  // Changes scope, should be at end of class
  DALI_LOG_OBJECT_STRING_DECLARATION;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_BITMAP_EXTERNAL_H__
