#ifndef __DALI_FRAME_BUFFER_IMAGE_H__
#define __DALI_FRAME_BUFFER_IMAGE_H__

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
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/native-image-interface.h>
#include <dali/public-api/images/pixel.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class FrameBufferImage;
}

/**
 * @brief FrameBufferImage represents a GLES Frame Buffer Object and contains the result
 * of an 'off screen' render pass of a RenderTask.
 * The FrameBufferImage can then be used with an ImageActor (with optional shader
 * effects) and rendered to the screen.
 */
class DALI_IMPORT_API FrameBufferImage : public Image
{
public:
  /**
   * @brief Constructor which creates an uninitialized FrameBufferImage object.
   *
   * Use Image::New(...) to create an initialised object.
   */
  FrameBufferImage();

  /**
   * @brief Create a new FrameBufferImage.
   *
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   * The ReleasePolicy defaults to Dali::Image::Never.
   * @param [in] width       The width in pixels. Setting to zero will use the width of the stage.
   * @param [in] height      The height in pixels. Setting to zero will use the height of the stage.
   * @param [in] pixelformat The pixel format (rgba 32 bit by default)
   * @post When the FrameBufferImage is first used as a render target, an exception may be thrown if pixelFormat is not supported on the hardware platform.
   * @return A handle to a new instance of a FrameBufferImage.
   */
  static FrameBufferImage New(unsigned int width = 0, unsigned int height = 0, Pixel::Format pixelformat = Pixel::RGBA8888);

  /**
   * @brief Create a new FrameBufferImage.
   *
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   * @param [in] width       The width in pixels. Setting to zero will use the width of the stage.
   * @param [in] height      The height in pixels. Setting to zero will use the height of the stage.
   * @param [in] pixelformat The pixel format.
   * @param [in] releasePolicy The ReleasePolicy to apply to the FrameBufferImage.
   *
   * Note that there is no need for a LoadPolicy - by definition it is always OnDemand, since there is no point in the FrameBufferImage existing unless someone is rendering to
   * it, or it is being used as an input (e.g. ShaderEffect / ImageActor).
   *
   * @post When the FrameBufferImage is first used as a render target, an exception may be thrown if pixelFormat is not supported on the hardware platform.
   * @return A handle to a new instance of a FrameBufferImage.
   */
  static FrameBufferImage New(unsigned int width, unsigned int height, Pixel::Format pixelformat, ReleasePolicy releasePolicy );

  /**
   * @brief Create a new FrameBufferImage.
   *
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   * @param [in] image       The native image.
   *
   * Note that there is no need for a LoadPolicy - by definition it is always OnDemand, since there is no point in the FrameBufferImage existing unless someone is rendering to
   * it, or it is being used as an input (e.g. ShaderEffect / ImageActor).
   *
   * @post When the FrameBufferImage is first used as a render target, an exception may be thrown if the NativeImage cannot be mapped to a texture.
   * @return A handle to a new instance of a FrameBufferImage.
   */
  static FrameBufferImage New(NativeImageInterface& image);

  /**
   * @brief Create a new FrameBufferImage.
   *
   * The maximum size of the image is limited by GL_MAX_TEXTURE_SIZE.
   * @param [in] image       The native image.
   * @param [in] releasePolicy The ReleasePolicy to apply to the FrameBufferImage.
   *
   * Note that there is no need for a LoadPolicy - by definition it is always OnDemand, since there is no point in the FrameBufferImage existing unless someone is rendering to
   * it, or it is being used as an input (e.g. ShaderEffect / ImageActor).
   *
   * @post When the FrameBufferImage is first used as a render target, an exception may be thrown if the NativeImage cannot be mapped to a texture.
   * @return A handle to a new instance of a FrameBufferImage.
   */
  static FrameBufferImage New(NativeImageInterface& image, ReleasePolicy releasePolicy);

  /**
   * @brief Downcast an Object handle to FrameBufferImage handle.
   *
   * If handle points to a FrameBufferImage object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a FrameBufferImage object or an uninitialized handle
   */
  static FrameBufferImage DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~FrameBufferImage();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  FrameBufferImage(const FrameBufferImage& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  FrameBufferImage& operator=(const FrameBufferImage& rhs);

public: // Not intended for application developers

  explicit DALI_INTERNAL FrameBufferImage(Internal::FrameBufferImage*);
}; //class FrameBufferImage

} // namespace Dali

#endif // __DALI_FRAME_BUFFER_IMAGE_H__
