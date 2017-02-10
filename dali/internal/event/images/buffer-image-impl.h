#ifndef __DALI_INTERNAL_BUFFER_IMAGE_H__
#define __DALI_INTERNAL_BUFFER_IMAGE_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <stdint.h> // for uint32_t

// INTERNAL INCLUDES
#include <dali/integration-api/bitmap.h> // For Integration::BitmapPtr
#include <dali/public-api/object/ref-object.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/buffer-image.h>

namespace Dali
{

namespace Internal
{

class BufferImage;
typedef IntrusivePtr<BufferImage> BufferImagePtr;

class ResourceManager;

/**
 * BufferImage represents an image resource that can be added to actors etc.
 * Its pixel buffer data is provided by the application developer.
 * Pixel buffer memory allocation can be handled by dali or application.
 */
class BufferImage : public Image
{
public:
  /**
   * Create a new BufferImage.
   * Also a pixel buffer for image data is allocated.
   * Dali has ownership of the buffer.
   * For better performance and portability use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   * @param [in] width       image width in pixels
   * @param [in] height      image height in pixels
   * @param [in] pixelformat the pixel format (rgba 32 bit by default)
   */
  static BufferImagePtr New( unsigned int width,
                             unsigned int height,
                             Pixel::Format pixelformat );

  /**
   * @DEPRECATED_1_1.5. Support for externally owned Pixel Buffers is due to be removed TBA. It is recommended that a BufferImage owned Buffer be used instead.
   *
   * @brief Create a new BufferImage, which uses external data source.
   *
   * Pixel buffer has to be allocated by application.
   * An internal copy is made of the Pixel Buffer, which can then be freed by the Application, unless if there will be a call to Update() later.
   * The buffer should only be freed when there is no chance of an Update() being called again.
   * Obtaining the buffer with GetBuffer() and altering the contents, then Update() will not work with externally owned buffers.
   * For better performance and portability use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   *
   * @param [in] pixBuf      pixel buffer. has to be allocated by application.
   * @param [in] width       image width in pixels
   * @param [in] height      image height in pixels
   * @param [in] pixelformat the pixel format (rgba 32 bit by default)
   * @param [in] stride      the internal stride of the pixelbuffer in pixels
   */
  static BufferImagePtr New( PixelBuffer* pixBuf,
                             unsigned int width,
                             unsigned int height,
                             Pixel::Format pixelformat,
                             unsigned int stride );

  /**
   * Create a new BufferImage.
   * Also a pixel buffer for image data is allocated.
   * Dali has ownership of the buffer.
   * For better performance use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   * @param [in] width image width in pixels
   * @param [in] height image height in pixels
   * @param [in] pixelformat the pixel format (rgba 32 bit by default)
   */
  BufferImage(unsigned int width,
              unsigned int height,
              Pixel::Format pixelformat );

  /**
   * Create a new BufferImage, which uses external data source.
   * Pixel buffer has to be allocated by application.
   * An internal copy is made of the Pixel Buffer, which can then be freed by the Application, unless if there will be a call to Update() later.
   * The buffer should only be freed when there is no chance of Update() being called again.
   * Note: obtaining the buffer with GetBuffer(), writing changes, then Update() will cause any changes to be lost.
   * In this case, the BufferImage will update from the external buffer and so changes should be written there.
   * For better performance and portability use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   * @param [in] pixBuf      pixel buffer. has to be allocated by application.
   * @param [in] width       image width in pixels
   * @param [in] height      image height in pixels
   * @param [in] pixelformat the pixel format (rgba 32 bit by default)
   * @param [in] stride      the internal stride of the pixelbuffer in pixels
   */
  BufferImage(PixelBuffer* pixBuf,
              unsigned int width,
              unsigned int height,
              Pixel::Format pixelformat,
              unsigned int stride );

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~BufferImage();

public:
  /**
   * Notify Dali that the contents of the buffer have changed.
   * @param [in] updateArea area that has changed in buffer. An empty rect means the whole buffer has changed.
   */
  void Update( const RectArea& updateArea);

  /**
   * @copydoc Dali::BufferImage::IsDataExternal
   */
  bool IsDataExternal() const;

  /**
   * Returns the pixel buffer of the Image.
   * The application developer can write to the buffer.
   * Upload the modified contents with Update().
   * @return the pixel buffer
   */
  PixelBuffer* GetBuffer() const
  {
    return ( mExternalBuffer ? mExternalBuffer : mInternalBuffer );
  }

  /**
   * Returns buffer size in bytes.
   * @return the buffer size in bytes
   */
  unsigned int GetBufferSize() const
  {
    return mBufferSize;
  }

  /**
   * Returns buffer stride (in bytes).
   * @return the buffer stride
   */
  unsigned int GetBufferStride() const
  {
    return mByteStride;
  }

  /**
   * Get the pixel format
   * @return The pixel format
   */
  Pixel::Format GetPixelFormat() const
  {
    return mPixelFormat;
  }

private:

  void SetupBuffer( unsigned int width,
                    unsigned int height,
                    Pixel::Format pixelformat,
                    unsigned int byteStride );

  void UploadArea( const RectArea& area );

  void UpdateBufferArea( PixelBuffer* src, PixelBuffer* dest, const RectArea& area );

private:

  PixelBuffer*                 mInternalBuffer;       ///< NULL if the data is supplied by an external buffer.
  PixelBuffer*                 mExternalBuffer;       ///< NULL if there is no external pixel data (this is never owned by BufferImage).
  uint32_t                     mBufferSize;           ///< size of the pixel buffer.
  uint32_t                     mByteStride;           ///< width of the pixel buffer in bytes.
  uint32_t                     mBytesPerPixel;        ///< width of a pixel in bytes.
  uint32_t                     mBufferWidth;          ///< cached pixel width of bitmap used for transport.
  Pixel::Format                mPixelFormat;          ///< pixel format of bitmap.
  ResourcePolicy::Discardable  mResourcePolicy;       ///< whether to discard the pixel buffer when removed from the stage or to retain the data.
};

} // namespace Internal

/**
 * Helper methods for public API.
 */
inline Internal::BufferImage& GetImplementation(Dali::BufferImage& image)
{
  DALI_ASSERT_ALWAYS( image && "BufferImage handle is empty" );

  BaseObject& handle = image.GetBaseObject();

  return static_cast<Internal::BufferImage&>(handle);
}

inline const Internal::BufferImage& GetImplementation(const Dali::BufferImage& image)
{
  DALI_ASSERT_ALWAYS( image && "BufferImage handle is empty" );

  const BaseObject& handle = image.GetBaseObject();

  return static_cast<const Internal::BufferImage&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_BUFFER_IMAGE_H__
