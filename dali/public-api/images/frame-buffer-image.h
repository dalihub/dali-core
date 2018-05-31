#ifndef __DALI_FRAME_BUFFER_IMAGE_H__
#define __DALI_FRAME_BUFFER_IMAGE_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/images/native-image-interface.h>
#include <dali/public-api/images/pixel.h>

namespace Dali
{
/**
 * @addtogroup dali_core_images
 * @{
 */

namespace Internal DALI_INTERNAL
{
class FrameBufferImage;
}

namespace RenderBuffer
{
/**
 * @brief Enumeration for rendering buffer formats.
 *
 * The default format for framebuffer creation is COLOR, so If a depth buffer for 3D rendering is required use
 * COLOR_DEPTH instead.
 * @SINCE_1_0.0
 */
enum Format ///< Framebuffer format, default color depth is RGBA 32 bit with alpha @SINCE_1_0.0
{
  COLOR,                ///< Framebuffer will be created with color buffer. @SINCE_1_0.0
  COLOR_DEPTH,          ///< Framebuffer will be created with color and depth buffer @SINCE_1_0.0
  COLOR_STENCIL,        ///< Framebuffer will be created with color and stencil buffer @SINCE_1_0.0
  COLOR_DEPTH_STENCIL   ///< Framebuffer will be created with color, depth and stencil buffer. @note May be not supported in all devices @SINCE_1_0.0
};
}

/**
 * @DEPRECATED_1_2.41
 *
 * @brief FrameBufferImage represents an Open GL ES Frame Buffer Object and contains the result
 * of an 'off screen' render pass of a RenderTask.
 *
 * The FrameBufferImage can then be used for rendering to the screen.
 * @SINCE_1_0.0
 */
class DALI_CORE_API FrameBufferImage : public Image
{
public:

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief Constructor which creates an uninitialized FrameBufferImage object.
   *
   * Use @ref FrameBufferImage::New to create an initialized object.
   * @SINCE_1_0.0
   */
  FrameBufferImage() DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief Creates a new FrameBufferImage.
   *
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   *
   * @SINCE_1_0.0
   * @param[in] width The width in pixels. Setting to zero will use the width of the stage
   * @param[in] height The height in pixels. Setting to zero will use the height of the stage
   * @param[in] pixelFormat The pixel format (rgba 32 bit by default)
   * @param[in] bufferFormat The format of the buffers that are going to be created for the FBO, (COLOR and DEPTH buffer as default)
   * @return A handle to a new instance of a FrameBufferImage
   * @post When the FrameBufferImage is first used as a render target, an exception may be thrown if pixelFormat is not supported on the hardware platform.
   */
  static FrameBufferImage New(unsigned int width = 0, unsigned int height = 0, Pixel::Format pixelFormat = Pixel::RGBA8888,
                              RenderBuffer::Format bufferFormat = RenderBuffer::COLOR) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief Creates a new FrameBufferImage.
   *
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   * @SINCE_1_0.0
   * @param[in] image The native image
   *
   * @return A handle to a new instance of a FrameBufferImage
   * @post When the FrameBufferImage is first used as a render target, an exception may be thrown if the NativeImage cannot be mapped to a texture.
   */
  static FrameBufferImage New(NativeImageInterface& image) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief Downcasts a handle to FrameBufferImage handle.
   *
   * If handle points to a FrameBufferImage object, the
   * downcast produces valid handle. If not, the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return Handle to a FrameBufferImage object or an uninitialized handle
   */
  static FrameBufferImage DownCast( BaseHandle handle ) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~FrameBufferImage() DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] handle A reference to the copied handle
   */
  FrameBufferImage(const FrameBufferImage& handle) DALI_DEPRECATED_API;

  /**
   * @DEPRECATED_1_2.41
   *
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  FrameBufferImage& operator=(const FrameBufferImage& rhs) DALI_DEPRECATED_API;

public: // Not intended for application developers

  explicit DALI_INTERNAL FrameBufferImage(Internal::FrameBufferImage*);
}; //class FrameBufferImage

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_FRAME_BUFFER_IMAGE_H__
