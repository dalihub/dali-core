#ifndef __DALI_BUFFER_IMAGE_H__
#define __DALI_BUFFER_IMAGE_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/pixel.h>
#include <dali/public-api/math/rect.h>

namespace Dali
{
/**
 * @addtogroup dali_core_images
 * @{
 */

namespace Internal DALI_INTERNAL
{
class BufferImage;
}

typedef unsigned char         PixelBuffer;  ///< pixel data buffer @SINCE_1_0.0
typedef Rect<unsigned int>    RectArea;     ///< rectangular area (x,y,w,h) @SINCE_1_0.0


/**
 * @brief BufferImage represents an image resource as a pixel data buffer.
 *
 * Its pixel buffer data is provided by the application developer.
 *
 * If the pixel format of the pixel buffer contains an alpha channel,
 * then the image is considered to be have transparent pixels without
 * regard for the actual content of the channel, and will be blended.
 *
 * @SINCE_1_0.0
 */
class DALI_IMPORT_API BufferImage : public Image
{
public:
  /**
   * @brief Constructor which creates an uninitialized BufferImage object.
   *
   * Use BufferImage::New(...) to create an initialized object.
   * @SINCE_1_0.0
   */
  BufferImage();

  /**
   * @brief Creates a new BufferImage.
   *
   * Also a pixel buffer for image data is allocated.
   * Dali has ownership of the buffer.
   * For better performance and portability use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   * @SINCE_1_0.0
   * @param[in] width  Image width in pixels
   * @param[in] height Image height in pixels
   * @param[in] pixelformat The pixel format (rgba 32 bit by default)
   * @return A handle to a new instance of BufferImage
   * @pre width & height are greater than zero
   * @note default resource management policies are Immediate and Never
   *
   */
  static BufferImage New(unsigned int width,
                         unsigned int height,
                         Pixel::Format pixelformat=Pixel::RGBA8888);

  /**
   * @brief Creates a new BufferImage, which uses an external data source.
   *
   * The PixelBuffer has to be allocated by application.
   *
   * The application holds ownership of the buffer. It must not
   * destroy the PixelBuffer on a staged image if it has called
   * Update() and hasn't received a Image::UploadedSignal, or if it has just
   * added it to the stage and has not received a Image::UploadedSignal.
   *
   * For better performance and portability use power of two dimensions.
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   *
   * @SINCE_1_0.0
   * @param[in] pixelBuffer  Pixel buffer has to be allocated by application
   * @param[in] width        Image width in pixels
   * @param[in] height       Image height in pixels
   * @param[in] pixelFormat  The pixel format (rgba 32 bit by default)
   * @param[in] stride       The internal stride of the pixelbuffer in pixels
   * @return A handle to a new instance of BufferImage
   * @pre width & height are greater than zero
   */
  static BufferImage New(PixelBuffer*  pixelBuffer,
                         unsigned int  width,
                         unsigned int  height,
                         Pixel::Format pixelFormat=Pixel::RGBA8888,
                         unsigned int  stride=0);

  /**
   * @brief Downcasts a handle to BufferImage handle.
   *
   * If the handle points to a BufferImage, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return Handle to a BufferImage or an uninitialized handle
   */
  static BufferImage DownCast( BaseHandle handle );

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~BufferImage();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] handle A reference to the copied handle
   */
  BufferImage(const BufferImage& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  BufferImage& operator=(const BufferImage& rhs);

  /**
   * @brief White pixel as image data.
   *
   * Can be used to create solid color actors.
   * @SINCE_1_0.0
   * @return 1 white pixel with 32 bit colordepth
   */
  static const BufferImage WHITE();

public:
  /**
   * @brief Returns the pixel buffer of the Image.
   *
   * The application can write to the buffer to modify its contents.
   *
   * Whilst the image is on stage, after writing to the buffer the
   * application should call Update() and wait for the
   * Image::UploadedSignal() method before writing again.
   *
   * @SINCE_1_0.0
   * @return The pixel buffer
   */
  PixelBuffer* GetBuffer();

  /**
   * @brief Returns buffer size in bytes.
   *
   * @SINCE_1_0.0
   * @return The buffer size in bytes
   */
  unsigned int GetBufferSize() const;

  /**
   * @brief Returns buffer stride (in bytes).
   *
   * @SINCE_1_0.0
   * @return The buffer stride
   */
  unsigned int GetBufferStride() const;

  /**
   * @brief Returns the pixel format of the contained buffer
   *
   * @SINCE_1_0.0
   * @return The pixel format
   */
  Pixel::Format GetPixelFormat() const;

  /**
   * @brief Inform Dali that the contents of the buffer have changed.
   *
   * Image::UploadedSignal will be sent in response if the image is on stage
   * and the image data has been successfully copied to graphics
   * memory. To avoid visual tearing, the application should wait for
   * the Image::UploadedSignal before modifying the data.
   *
   * The application must not destroy an external PixelBuffer on a staged
   * image after calling this method until the Image::UploadedSignal has been
   * successfully received.
   *
   * @SINCE_1_0.0
   * @note BufferImage::Update might not work with BGR/BGRA formats!
   * @note Some GPUs may not support Non power of two buffer updates (for
   * example C110/SGX540)
   */
  void Update();

  /**
   * @copydoc Update()
   * @param[in] updateArea Area that has changed in buffer
   */
  void Update( RectArea updateArea );

  /**
   * @brief Returns whether BufferImage uses external data source or not.
   *
   * If not, dali holds ownership of the PixelBuffer, otherwise the application
   * is responsible for freeing it.
   *
   * The application must not destroy an external PixelBuffer on a staged image
   * if it has called Update() and hasn't received a Image::UploadedSignal.
   *
   * @SINCE_1_0.0
   * @return @c true if application owns data, @c false otherwise
   */
  bool IsDataExternal() const;

public: // Not intended for application developers

  explicit DALI_INTERNAL BufferImage(Internal::BufferImage*);
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_BUFFER_IMAGE_H__
