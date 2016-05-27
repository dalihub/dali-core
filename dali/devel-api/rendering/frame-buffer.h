#ifndef DALI_FRAMEBUFFER_H
#define DALI_FRAMEBUFFER_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/base-handle.h>
#include <dali/devel-api/rendering/texture.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class FrameBuffer;
}

/**
 * @brief FrameBuffer is a collection of textures that can be used as the destination for rendering
 */
class DALI_IMPORT_API FrameBuffer : public BaseHandle
{
public:

  enum Format
  {
    COLOR,                ///< Framebuffer will be created with color buffer.
    COLOR_DEPTH,          ///< Framebuffer will be created with color and depth buffer
    COLOR_STENCIL,        ///< Framebuffer will be created with color and stencil buffer
    COLOR_DEPTH_STENCIL   ///< Framebuffer will be created with color, depth and stencil buffer. @note May be not supported in all devices
  };

  /**
   * @brief Creates a new FrameBuffer object
   *
   * @param[in] width The width of the FrameBuffer
   * @param[in] height The height of the FrameBuffer
   * @param[in] format The format of the FrameBuffer
   * @return A handle to a newly allocated FrameBuffer
   */
  static FrameBuffer New( unsigned int width, unsigned int height, Format format );

  /**
   * @brief Default constructor, creates an empty handle
   */
  FrameBuffer();

  /**
   * @brief Destructor
   */
  ~FrameBuffer();

  /**
   * @brief Copy constructor, creates a new handle to the same object
   *
   * @param[in] handle Handle to an object
   */
  FrameBuffer( const FrameBuffer& handle );

  /**
   * @brief Downcast to a FrameBuffer.
   *
   * If not the returned handle is left uninitialized.
   * @param[in] handle to an object
   * @return FrameBuffer handle or an uninitialized handle
   */
  static FrameBuffer DownCast( BaseHandle handle );

  /**
   * @brief Assignment operator, changes this handle to point at the same object
   *
   * @param[in] handle Handle to an object
   * @return Reference to the assigned object
   */
  FrameBuffer& operator=( const FrameBuffer& handle );

  /**
   * @brief Attach a texture for color rendering
   * @param[in] texture The texture that will be used as output when rendering
   * @param[in] mipmapLevel The mipmap of the texture to be attached
   * @param[in] layer Indicates which layer of a cube map or array texture to attach. Unused for 2D textures
   * @note The specified texture mipmap has to have the same size than the FrameBuffer
   * otherwise it won't be attached
   */
  void AttachColorTexture( Texture& texture, unsigned int mipmapLevel = 0u, unsigned int layer = 0u );

  /**
   * @brief Get the color texture used as output in the FrameBuffer
   * @returns A handle to the texture used as color output, or an uninitialized handle
   */
  Texture GetColorTexture();

public:
  /**
   * @brief The constructor
   *
   * @param [in] pointer A pointer to a newly allocated FrameBuffer
   */
  explicit DALI_INTERNAL FrameBuffer( Internal::FrameBuffer* pointer );
};

} //namespace Dali

#endif // DALI_FRAMEBUFFER_H
