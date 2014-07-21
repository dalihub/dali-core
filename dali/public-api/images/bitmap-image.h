#ifndef __DALI_BITMAP_IMAGE_H__
#define __DALI_BITMAP_IMAGE_H__

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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/math/rect.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class BitmapImage;
}

typedef unsigned char         PixelBuffer;  ///< pixel data buffer
typedef Rect<unsigned int>    RectArea;     ///< rectangular area (x,y,w,h)


/**
 * @brief BitmapImage represents an image resource that can be added to ImageActors.
 * Its pixel buffer data is provided by the application developer.
 *
 * Care should be taken with pixel data allocated by the application,
 * as the data is copied to GL both when the image is added to the
 * stage and after a call to Update().  In both of these cases, a
 * SignalUploaded will be sent to the application confirming that the
 * operation has completed.
 *
 * The application can free the pixel data after receiving a
 * SignalUploaded.
 *
 * Similarly, once the image is on stage (i.e. it's being used by an
 * ImageActor that is on stage), the application should only write to
 * the buffer after receiving a SignalUploaded, then call Update()
 * once the write is finished. This avoids the pixel data being changed
 * whilst it's being copied to GL. Writing to the buffer without waiting
 * for the signal will likely result in visible tearing.
 *
 * If the pixel format of the pixel buffer contains an alpha channel,
 * then the image is considered to be have transparent pixels without
 * regard for the actual content of the channel, and will be blended.
 *
 * If the image is opaque and blending is not required, then the user
 * should call ImageActor::SetUseImageAlpha(false) on the containing actor.
 */
class BitmapImage : public Image
{
public:
  /**
   * @brief Constructor which creates an uninitialized BitmapImage object.
   *
   * Use Image::New(...) to create an initialised object.
   */
  BitmapImage();

  /**
   * @brief Create a new BitmapImage.
   *
   * Also a pixel buffer for image data is allocated.
   * Dali has ownership of the buffer.
   * For better performance and portability use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   * @note: default resource management policies are Immediate and Never
   *
   * @pre width & height are greater than zero
   * @param [in] width       image width in pixels
   * @param [in] height      image height in pixels
   * @param [in] pixelformat the pixel format (rgba 32 bit by default)
   * @return a handle to a new instance of BitmapImage
   */
  static BitmapImage New(unsigned int width,
                         unsigned int height,
                         Pixel::Format pixelformat=Pixel::RGBA8888);

  /**
   * @brief Create a new BitmapImage.
   *
   * Also a pixel buffer for image data is allocated.
   * Dali has ownership of the buffer.
   * For better performance and portability use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   *
   * @pre width & height are greater than zero
   * @param [in] width          Image width in pixels
   * @param [in] height         Image height in pixels
   * @param [in] pixelFormat    The pixel format
   * @param [in] loadPolicy     Controls time of loading a resource from the filesystem.
   * @param [in] releasePolicy  Optionally release memory when image is not visible on screen.
   * @return a handle to a new instance of BitmapImage
   */
  static BitmapImage New(unsigned int  width,
                         unsigned int  height,
                         Pixel::Format pixelFormat,
                         LoadPolicy    loadPolicy,
                         ReleasePolicy releasePolicy);

  /**
   * @brief Create a new BitmapImage, which uses an external data source.
   *
   * The PixelBuffer has to be allocated by application.
   *
   * The application holds ownership of the buffer. It must not
   * destroy the PixelBuffer on a staged image if it has called
   * Update() and hasn't received a SignalUploaded, or if it has just
   * added it to the stage and has not received a SignalUploaded.
   *
   * For better performance and portability use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   *
   * @pre width & height are greater than zero
   * @param [in] pixelBuffer  pixel buffer. has to be allocated by application.
   * @param [in] width        image width in pixels
   * @param [in] height       image height in pixels
   * @param [in] pixelFormat  the pixel format (rgba 32 bit by default)
   * @param [in] stride       the internal stride of the pixelbuffer in pixels
   * @return a handle to a new instance of BitmapImage
   */
  static BitmapImage New(PixelBuffer*  pixelBuffer,
                         unsigned int  width,
                         unsigned int  height,
                         Pixel::Format pixelFormat=Pixel::RGBA8888,
                         unsigned int  stride=0);

  /**
   * @brief Create a new BitmapImage, which uses an external data source.
   *
   * The PixelBuffer has to be allocated by application.
   *
   * The application holds ownership of the buffer. It must not
   * destroy the PixelBuffer on a staged image if it has called
   * Update() and hasn't received a SignalUploaded, or if it has just
   * added it to the stage and has not received a SignalUploaded.
   *
   * For better performance and portability use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   *
   * @note in case releasePolicy is "Unused", application has to call
   * BitmapImage::Update() whenever image is re-added to the stage
   *
   * @pre width & height are greater than zero
   * @param [in] pixelBuffer   pixel buffer. has to be allocated by application.
   * @param [in] width         image width in pixels
   * @param [in] height        image height in pixels
   * @param [in] pixelFormat   the pixel format
   * @param [in] stride        the internal stride of the pixelbuffer in pixels
   * @param [in] releasePolicy optionally relase memory when image is not visible on screen.
   * @return a handle to a new instance of BitmapImage
   */
  static BitmapImage New(PixelBuffer*  pixelBuffer,
                         unsigned int  width,
                         unsigned int  height,
                         Pixel::Format pixelFormat,
                         unsigned int  stride,
                         ReleasePolicy releasePolicy);

  /**
   * @brief Downcast an Object handle to BitmapImage.
   *
   * If handle points to a BitmapImage the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle to An object
   * @return handle to a BitmapImage or an uninitialized handle
   */
  static BitmapImage DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~BitmapImage();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

  /**
   * @brief White pixel as image data.
   *
   * Can be used to create solid color actors.
   * @return 1 white pixel with 32 bit colordepth
   */
  static const BitmapImage WHITE();

public:
  /**
   * @brief Returns the pixel buffer of the Image.
   *
   * The application can write to the buffer to modify its contents.
   *
   * Whilst the image is on stage, after writing to the buffer the
   * application should call Update() and wait for the
   * SignalUploaded() method before writing again.
   *
   * @return the pixel buffer
   */
  PixelBuffer* GetBuffer();

  /**
   * @brief Returns buffer size in bytes.
   *
   * @return the buffer size in bytes
   */
  unsigned int GetBufferSize() const;

  /**
   * @brief Returns buffer stride (in bytes).
   *
   * @return the buffer stride
   */
  unsigned int GetBufferStride() const;

  /**
   * @brief Inform Dali that the contents of the buffer have changed.
   *
   * SignalUploaded will be sent in response if the image is on stage
   * and the image data has been successfully copied to graphics
   * memory. To avoid visual tearing, the application should wait for
   * the SignalUploaded before modifying the data.
   *
   * The application must not destroy an external PixelBuffer on a staged
   * image after calling this method until the SignalUploaded has been
   * successfully received.
   *
   * @note: BitmapImage::Update might not work with BGR/BGRA formats!
   * @note: Some GPUs may not support Non power of two buffer updates (for
   * example C110/SGX540)
   */
  void Update();

  /**
   * @copydoc Update()
   * @param [in] updateArea area that has changed in buffer
   */
  void Update( RectArea updateArea );

  /**
   * @brief Returns whether BitmapImage uses external data source or not.
   *
   * If not, dali holds ownership of the PixelBuffer, otherwise the application
   * is responsible for freeing it.
   *
   * The application must not destroy an external PixelBuffer on a staged image
   * if it has called Update() and hasn't received a SignalUploaded.
   *
   * @return true if application owns data, false otherwise
   */
  bool IsDataExternal() const;

public: // Not intended for application developers

  explicit DALI_INTERNAL BitmapImage(Internal::BitmapImage*);
};

} // namespace Dali

#endif // __DALI_BITMAP_IMAGE_H__
