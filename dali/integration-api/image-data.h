#ifndef __DALI_INTEGRATION_IMAGE_DATA_H__
#define __DALI_INTEGRATION_IMAGE_DATA_H__

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

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h> // For DALI_LOG_OBJECT_STRING_DECLARATION
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/images/pixel.h>

// EXTERNAL INCLUDES
#include <stdint.h>

namespace Dali
{

namespace Integration
{

class ImageData;
typedef IntrusivePtr<ImageData> ImageDataPtr;

typedef uint8_t PixelBuffer;

/**
 * Used to avoid accidentally mixing the order of parameters where some are uint
 * dimensions and one is a buffer size.
 **/
class BufferSize
{
public:
  explicit BufferSize(size_t size) : bufferSize( size )
  { }
  operator size_t() const { return bufferSize; }
  const size_t bufferSize;
};

/**
 * @brief A simple container for image data.
 *
 * Just a pointer to a buffer and some minimal metadata. It always owns the
 * buffer of image data that it points to until it is destroyed or the buffer
 * is released with ReleaseImageBuffer().
 *
 * The width and height stored are the logical image values. Compressed formats
 * such as ETC group pixels into blocks (ETC = 4*4) and these logical width and
 * height may indicate, for example for ETC, that 1 to 3 pixels in the rightmost
 * column of blocks or bottom row are ignored for rendering.
 * For simple uncompressed images, the buffer is exactly width * height *
 * bytes-per-pixel in size with no spare bytes at right or bottom edge.
 */
class DALI_IMPORT_API ImageData : public Dali::RefObject
{
public:
  /**
   * @brief Three values for the information about the alpha values in the
   * pixels.
   *
   * Alpha in pixels with the channel can be undetermined, it can be all-ones,
   * or it can have at least one pixel that is translucent with an alpha < 1.0.
   **/
  enum AlphaUsage
  {
    /** Alpha not yet been tested.*/
    ALPHA_USAGE_UNDETERMINED,
    /** Alpha == 1.0 in every pixel.*/
    ALPHA_USAGE_ALL_OPAQUE,
    /** Alpha < 1.0 in at least one pixel.*/
    ALPHA_USAGE_SOME_TRANSLUCENT
  };

private:

  /**
   * Construct an instance and allocate a buffer owned by it.
   * @param[in] numBytes The number of bytes to allocate to hold the image data.
   * @param[in] imageWidth The width in pixels of the image.
   * @param[in] imageHeight The height in pixels of the image.
   * @param[in] pixelFormat The format of the image data in the buffer.
   */
  ImageData( const size_t numBytes, const unsigned imageWidth, const unsigned imageHeight, const Pixel::Format pixelFormat );

  /**
   * Construct an instance and have it take ownership of the buffer passed in.
   * @param[in] imageBuffer A buffer of pixel data that should have been allocated using <code>new uint8_t[]</code>.
   * @param[in] numBytes The number of bytes in the buffer pointed to by imageBuffer.
   * @param[in] imageWidth The width in pixels of the image.
   * @param[in] imageHeight The height in pixels of the image.
   * @param[in] pixelFormat The format of the image data in the buffer.
   */
  ImageData( uint8_t * const imageBuffer, const size_t numBytes, const unsigned imageWidth, const unsigned imageHeight, const Pixel::Format pixelFormat );

  ~ImageData();

public:

  /** Allocate and initialize a fresh ImageData object pointing at a new buffer,
   * returning a smart pointer owning it.
   * @note Clients can alternatively use the helper function NewBitmapImageData() to
   * calculate the buffer size from other parameters if the image data represents
   * a bitmap (a 2d grid of addressable pixels).
   * @param[in] numBytes The number of bytes to allocate to hold the image data.
   * @param[in] imageWidth The width in pixels of the image.
   * @param[in] imageHeight The height in pixels of the image.
   * @param[in] pixelFormat The format of the image data in the buffer.
   **/
  static ImageDataPtr New( const BufferSize numBytes, unsigned imageWidth, unsigned imageHeight, Pixel::Format pixelFormat );

  /** Allocate and initialise a fresh ImageData object, taking ownership of the
   * buffer passed in.
   * @param[in] imageBuffer A block of memory allocated with
   * <code>new uint8_t[]</code>. <em>This is owned by the new ImageData instance
   * on successful return and should be forgotten by the caller</em>.
   * @param[in] numBytes The number of bytes in the buffer pointed to by imageBuffer.
   * @param[in] imageWidth The width in pixels of the image.
   * @param[in] imageHeight The height in pixels of the image.
   * @param[in] pixelFormat The format of the image data in the buffer.
   **/
  static ImageDataPtr New( uint8_t * const imageBuffer, const BufferSize numBytes, unsigned imageWidth, unsigned imageHeight, Pixel::Format pixelFormat );

  /** Access the buffer of image data. */
  const uint8_t * GetBuffer() const { return mData; }

  /** Access the buffer of image data. */
  uint8_t * GetBuffer() { return mData; }

  /**
   * Pass ownership of the buffer of pixel-level data that this instance
   * currently owns to the caller, and forget about the buffer here as a
   * side effect.
   * @returns A pointer to the underlying buffer of pixel-level image data.
   * The caller takes ownership of the buffer and is responsible for calling
   * delete[] on it eventually.
   **/
  uint8_t * ReleaseImageBuffer() { uint8_t * const data = mData; mData = 0; return data; }

  /**
   * @brief Get whether the alpha channel in the pixels is used.
   */
  AlphaUsage GetAlphaUsage() const { return mAlphaChannelUsed; }

  /**
   * @brief Set whether the alpha channel in the pixels is used.
   */
  void SetAlphaUsed( const bool alphaUsed ) { mAlphaChannelUsed = alphaUsed ? ALPHA_USAGE_SOME_TRANSLUCENT : ALPHA_USAGE_ALL_OPAQUE; }

private:

  ImageData(const ImageData& other);  ///< defined private to prevent use
  ImageData& operator = (const ImageData& other); ///< defined private to prevent use

  /** Pointer to the buffer of image data. */
  uint8_t*     mData;

public:
  const size_t        dataSize;   ///< Number of bytes in buffer pointed at by mData
  const uint16_t      imageWidth;  ///< Image logical width in pixels
  const uint16_t      imageHeight; ///< Image logical height in pixels
  const Pixel::Format pixelFormat; ///< Pixel format

private:
  AlphaUsage mAlphaChannelUsed;

  // Changes scope, should be at end of class
  DALI_LOG_OBJECT_STRING_DECLARATION;
};

/**
 * A convenience function for creating the common case of an uncompressed image
 * having width * height pixels in the buffer.
 * @param[in] imageWidth The width in pixels of the image.
 * @param[in] imageHeight The height in pixels of the image.
 * @param[in] pixelFormat The format of the image data in the buffer.
 */
DALI_IMPORT_API ImageDataPtr NewBitmapImageData( unsigned imageWidth, unsigned imageHeight, Pixel::Format pixelFormat );

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_IMAGE_DATA_H__
