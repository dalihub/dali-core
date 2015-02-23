#ifndef __DALI_INTERNAL_BUFFER_IMAGE_H__
#define __DALI_INTERNAL_BUFFER_IMAGE_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

class ResourceClient;
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
   * @param [in] releasePol  optionally relase memory when image is not visible on screen (default: keep image data until Image object is alive).
   */
  static BufferImagePtr New( unsigned int width,
                             unsigned int height,
                             Pixel::Format pixelformat,
                             ReleasePolicy releasePol = IMAGE_RELEASE_POLICY_DEFAULT );

  /**
   * Create a new BufferImage, which uses external data source.
   * Pixel buffer has to be allocated by application.
   * Application holds ownership of the buffer.
   * For better performance and portability use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   * @note  in case releasePol is "OffStage", application has to call Update() whenever image is re-added to the stage
   * @param [in] pixBuf      pixel buffer. has to be allocated by application.
   * @param [in] width       image width in pixels
   * @param [in] height      image height in pixels
   * @param [in] pixelformat the pixel format (rgba 32 bit by default)
   * @param [in] stride      the internal stride of the pixelbuffer in pixels
   * @param [in] releasePol  optionally relase memory when image is not visible on screen (default: keep image data until Image object is alive).
   */
  static BufferImagePtr New( PixelBuffer* pixBuf,
                             unsigned int width,
                             unsigned int height,
                             Pixel::Format pixelformat,
                             unsigned int stride,
                             ReleasePolicy releasePol = IMAGE_RELEASE_POLICY_DEFAULT );

  /**
   * Create a new BufferImage.
   * Also a pixel buffer for image data is allocated.
   * Dali has ownership of the buffer.
   * For better performance use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   * @param [in] width image width in pixels
   * @param [in] height image height in pixels
   * @param [in] pixelformat the pixel format (rgba 32 bit by default)
   * @param [in] releasePol optionally release memory when image is not visible on screen (default: keep image data until Image object is alive).
   */
  BufferImage(unsigned int width,
              unsigned int height,
              Pixel::Format pixelformat,
              ReleasePolicy releasePol = IMAGE_RELEASE_POLICY_DEFAULT);

  /**
   * Create a new BufferImage, which uses external data source.
   * Pixel buffer has to be allocated by application.
   * Application holds ownership of the buffer.
   * For better performance and portability use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   * @note  in case releasePol is "OffStage", application has to call Update() whenever image is re-added to the stage
   * @param [in] pixBuf      pixel buffer. has to be allocated by application.
   * @param [in] width       image width in pixels
   * @param [in] height      image height in pixels
   * @param [in] pixelformat the pixel format (rgba 32 bit by default)
   * @param [in] stride      the internal stride of the pixelbuffer in pixels
   * @param [in] releasePol  optionally relase memory when image is not visible on screen (default: keep image data until Image object is alive).
   */
  BufferImage(PixelBuffer* pixBuf,
              unsigned int width,
              unsigned int height,
              Pixel::Format pixelformat,
              unsigned int stride,
              ReleasePolicy releasePol = IMAGE_RELEASE_POLICY_DEFAULT);

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
  void Update (RectArea& updateArea);

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
  PixelBuffer* GetBuffer();

  /**
   * Returns buffer size in bytes.
   * @return the buffer size in bytes
   */
  unsigned int GetBufferSize() const;

  /**
   * Returns buffer stride (in bytes).
   * @return the buffer stride
   */
  unsigned int GetBufferStride() const;

  /**
   * Get the pixel format
   * @return The pixel format
   */
  Pixel::Format GetPixelFormat() const;

protected: // From Resource
  /**
   * @copydoc Dali::Internal::Image::Connect
   */
  virtual void Connect();

  /**
   * @copydoc Dali::Internal::Image::Disconnect
   */
  virtual void Disconnect();

  /**
   * Get the bitmap from local cache or ticket.
   **/
  Integration::Bitmap * GetBitmap() const;

private:
  bool mIsDataExternal; ///< whether application holds ownership of pixel buffer or not

  ResourceClient*            mResourceClient;

protected:
  Integration::BitmapPtr     mBitmapCached;
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
