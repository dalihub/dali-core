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
#include <dali/public-api/rendering/texture.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class FrameBuffer;
}

/**
 * @brief FrameBuffer is a collection of textures that can be used as the destination for rendering.
 * @SINCE_1_1.43
 */
class DALI_IMPORT_API FrameBuffer : public BaseHandle
{
public:

  /**
   * @brief The initial attachments to create the FrameBuffer with.
   * @note The color attachment is created on calling AttachColorTexture(). If a color attachment is not required, omit this call.
   * @note With "NONE", no attachments are created initially. However color attachments can still be added as described above.
   *
   * @SINCE_1_1.45
   */
  struct Attachment
  {
    /**
     * @brief Enumeration for the bit-mask value.
     * @SINCE_1_1.45
     */
    enum Mask
    {
      NONE          = 0,               ///< No attachments are created initially                            @SINCE_1_1.45

      DEPTH         = 1 << 0,          ///< Depth buffer bit-mask value                                     @SINCE_1_1.45
      STENCIL       = 1 << 1,          ///< Stencil buffer bit-mask value                                   @SINCE_1_1.45

      // Preset bit-mask combinations:
      DEPTH_STENCIL = DEPTH | STENCIL  ///< The Framebuffer will be created with depth and stencil buffer   @SINCE_1_1.45
    };
  };

  /**
   * @brief Creates a new FrameBuffer object.
   *
   * @SINCE_1_1.43
   * @param[in] width The width of the FrameBuffer
   * @param[in] height The height of the FrameBuffer
   * @param[in] attachments The attachments comprising the format of the FrameBuffer (the type is int to allow multiple bitmasks to be ORd)
   * @return A handle to a newly allocated FrameBuffer
   */
  static FrameBuffer New( unsigned int width, unsigned int height, unsigned int attachments );

  /**
   * @brief Default constructor, creates an empty handle.
   */
  FrameBuffer();

  /**
   * @brief Destructor.
   * @SINCE_1_1.43
   */
  ~FrameBuffer();

  /**
   * @brief Copy constructor, creates a new handle to the same object.
   *
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   */
  FrameBuffer( const FrameBuffer& handle );

  /**
   * @brief Downcasts to a FrameBuffer.
   * If not, the returned handle is left uninitialized.
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   * @return FrameBuffer handle or an uninitialized handle
   */
  static FrameBuffer DownCast( BaseHandle handle );

  /**
   * @brief Assignment operator, changes this handle to point at the same object.
   *
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   * @return Reference to the assigned object
   */
  FrameBuffer& operator=( const FrameBuffer& handle );

  /**
   * @brief Attach the base LOD of a 2D texture to the framebuffer for color rendering.
   * @note This causes a color attachment to be added.
   *
   * @SINCE_1_1.43
   * @param[in] texture The texture that will be used as output when rendering
   * @note The texture has to have the same size than the FrameBuffer
   * otherwise it won't be attached.
   */
  void AttachColorTexture( Texture& texture );

  /**
   * @brief Attach a texture to the framebuffer for color rendering.
   * @note This causes a color attachment to be added.
   *
   * @SINCE_1_1.43
   * @param[in] texture The texture that will be used as output when rendering
   * @param[in] mipmapLevel The mipmap of the texture to be attached
   * @param[in] layer Indicates which layer of a cube map or array texture to attach. Unused for 2D textures
   * @note The specified texture mipmap has to have the same size than the FrameBuffer
   * otherwise it won't be attached.
   */
  void AttachColorTexture( Texture& texture, unsigned int mipmapLevel, unsigned int layer );

  /**
   * @brief Gets the color texture used as output in the FrameBuffer.
   *
   * @SINCE_1_1.43
   * @returns A handle to the texture used as color output, or an uninitialized handle
   */
  Texture GetColorTexture();

public:

  /**
   * @brief The constructor.
   * @note  Not intended for application developers.
   * @SINCE_1_1.43
   * @param[in] pointer A pointer to a newly allocated FrameBuffer
   */
  explicit DALI_INTERNAL FrameBuffer( Internal::FrameBuffer* pointer );
};

} //namespace Dali

#endif // DALI_FRAMEBUFFER_H
