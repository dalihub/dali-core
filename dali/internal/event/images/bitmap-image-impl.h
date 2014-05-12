#ifndef __DALI_INTERNAL_BITMAP_IMAGE_H__
#define __DALI_INTERNAL_BITMAP_IMAGE_H__

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
#include <dali/public-api/object/ref-object.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/bitmap-image.h>

namespace Dali
{

namespace Internal
{

class BitmapImage;
typedef IntrusivePtr<BitmapImage> BitmapImagePtr;

class ResourceClient;
class ResourceManager;

/**
 * BitmapImage represents an image resource that can be added to actors etc.
 * Its pixel buffer data is provided by the application developer.
 * Pixel buffer memory allocation can be handled by dali or application.
 */
class BitmapImage : public Image
{
public:
  /**
   * Create a new BitmapImage.
   * Also a pixel buffer for image data is allocated.
   * Dali has ownership of the buffer.
   * For better performance and portability use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   * @param [in] width       image width in pixels
   * @param [in] height      image height in pixels
   * @param [in] pixelformat the pixel format (rgba 32 bit by default)
   * @param [in] loadPol     controls time of loading a resource from the filesystem (default: load when Image is created).
   * @param [in] releasePol  optionally relase memory when image is not visible on screen (default: keep image data until Image object is alive).
   */
  static BitmapImagePtr New( unsigned int width,
                             unsigned int height,
                             Pixel::Format pixelformat,
                             LoadPolicy loadPol=ImageLoadPolicyDefault,
                             ReleasePolicy releasePol=ImageReleasePolicyDefault);

  /**
   * Create a new BitmapImage, which uses external data source.
   * Pixel buffer has to be allocated by application.
   * Application holds ownership of the buffer.
   * For better performance and portability use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   * @note  in case releasePol is "OffStage", application has to call Image::BufferUpdated() whenever image is re-added to the stage
   * @param [in] pixBuf      pixel buffer. has to be allocated by application.
   * @param [in] width       image width in pixels
   * @param [in] height      image height in pixels
   * @param [in] pixelformat the pixel format (rgba 32 bit by default)
   * @param [in] stride      the internal stride of the pixelbuffer in pixels
   * @param [in] releasePol  optionally relase memory when image is not visible on screen (default: keep image data until Image object is alive).
   */
  static BitmapImagePtr New( PixelBuffer* pixBuf,
                             unsigned int width,
                             unsigned int height,
                             Pixel::Format pixelformat,
                             unsigned int stride,
                             ReleasePolicy releasePol=ImageReleasePolicyDefault );

  /**
   * Create a new BitmapImage.
   * Also a pixel buffer for image data is allocated.
   * Dali has ownership of the buffer.
   * For better performance use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   * @param [in] width image width in pixels
   * @param [in] height image height in pixels
   * @param [in] pixelformat the pixel format (rgba 32 bit by default)
   * @param [in] loadPol optionally delay loading until it used (default: immediate loading)
   * @param [in] releasePol optionally release memory when image is not visible on screen (default: keep image data until Image object is alive).
   */
  BitmapImage(unsigned int width,
              unsigned int height,
              Pixel::Format pixelformat,
              LoadPolicy loadPol=ImageLoadPolicyDefault,
              ReleasePolicy releasePol=ImageReleasePolicyDefault);

  /**
   * Create a new BitmapImage, which uses external data source.
   * Pixel buffer has to be allocated by application.
   * Application holds ownership of the buffer.
   * For better performance and portability use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   * @note  in case releasePol is "OffStage", application has to call Image::BufferUpdated() whenever image is re-added to the stage
   * @param [in] pixBuf      pixel buffer. has to be allocated by application.
   * @param [in] width       image width in pixels
   * @param [in] height      image height in pixels
   * @param [in] pixelformat the pixel format (rgba 32 bit by default)
   * @param [in] stride      the internal stride of the pixelbuffer in pixels
   * @param [in] releasePol  optionally relase memory when image is not visible on screen (default: keep image data until Image object is alive).
   */
  BitmapImage(PixelBuffer* pixBuf,
              unsigned int width,
              unsigned int height,
              Pixel::Format pixelformat,
              unsigned int stride,
              ReleasePolicy releasePol=ImageReleasePolicyDefault);

protected:
  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~BitmapImage();

public:
  /**
   * Notify Dali that the contents of the buffer have changed.
   * @param [in] updateArea area that has changed in buffer. An empty rect means the whole buffer has changed.
   */
  void Update (RectArea& updateArea);

  /**
   * @copydoc Dali::BitmapImage::IsDataExternal
   */
  bool IsDataExternal() const;

  /**
   * Returns the pixel buffer of the Image.
   * The application developer can write to the buffer.
   * Upload the modified contents with BufferUpdated.
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
inline Internal::BitmapImage& GetImplementation(Dali::BitmapImage& image)
{
  DALI_ASSERT_ALWAYS( image && "BitmapImage handle is empty" );

  BaseObject& handle = image.GetBaseObject();

  return static_cast<Internal::BitmapImage&>(handle);
}

inline const Internal::BitmapImage& GetImplementation(const Dali::BitmapImage& image)
{
  DALI_ASSERT_ALWAYS( image && "BitmapImage handle is empty" );

  const BaseObject& handle = image.GetBaseObject();

  return static_cast<const Internal::BitmapImage&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_BITMAP_IMAGE_H__
