#ifndef DALI_FRAMEBUFFER_H
#define DALI_FRAMEBUFFER_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <cstdint> // uint32_t

// INTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <dali/public-api/rendering/texture.h>

namespace Dali
{
/**
 * @addtogroup dali_core_rendering_effects
 * @{
 */

namespace Internal DALI_INTERNAL
{
class FrameBuffer;
}

/**
 * @brief FrameBuffer is a collection of textures that can be used as the destination for rendering.
 * @SINCE_1_1.43
 */
class DALI_CORE_API FrameBuffer : public BaseHandle
{
public:
  /**
   * @brief The initial attachments to create the FrameBuffer with.
   * @note The color attachment can also be created on calling AttachColorTexture().
   * @note With "NONE", no attachments are created initially. However color attachments can still be added as described above.
   *
   * @SINCE_1_1.45
   */
  struct Attachment
  {
    /**
     * @brief Enumeration for the attachments and/or textures to be created by default
     * @SINCE_1_1.45
     */
    enum Mask
    {
      NONE                = 0,                    ///< No attachments are created initially.         @SINCE_1_1.45
      DEPTH               = 1 << 0,               ///< Depth buffer is created.                      @SINCE_1_1.45
      STENCIL             = 1 << 1,               ///< Stencil buffer is created.                    @SINCE_1_1.45
      DEPTH_STENCIL       = DEPTH | STENCIL,      ///< Depth and stencil buffer are created.         @SINCE_1_1.45
      COLOR               = 1 << 2,               ///< Color texture is created.                     @SINCE_1_4.0
      COLOR_DEPTH         = COLOR | DEPTH,        ///< Color texture and depth buffer are created.   @SINCE_1_4.0
      COLOR_STENCIL       = COLOR | STENCIL,      ///< Color texture and stencil buffer are created. @SINCE_1_4.0
      COLOR_DEPTH_STENCIL = COLOR_DEPTH | STENCIL ///< Color, depth and stencil buffer are created.  @SINCE_1_4.0
    };
  };

  /**
   * @brief Creates a new FrameBuffer, which attaches only COLOR texture.
   *
   * @SINCE_1_4.0
   *
   * @note Call GetColorTexture() to get the COLOR texture
   *
   * @param[in] width The width of the FrameBuffer and the color texture
   * @param[in] height The height of the FrameBuffer and the color texture
   * @return A handle to a newly allocated FrameBuffer
   */
  static FrameBuffer New(uint32_t width, uint32_t height);

  /**
   * @brief Creates a new FrameBuffer with specific attachments.
   *
   * @SINCE_1_4.0
   *
   * @param[in] width The width of the FrameBuffer and the attachments
   * @param[in] height The height of the FrameBuffer and the attachments
   * @param[in] attachments Enumeration of the attachments to create
   * @return A handle to a newly allocated FrameBuffer
   */
  static FrameBuffer New(uint32_t width, uint32_t height, Attachment::Mask attachments);

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
  FrameBuffer(const FrameBuffer& handle);

  /**
   * @brief Downcasts to a FrameBuffer.
   * If not, the returned handle is left uninitialized.
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   * @return FrameBuffer handle or an uninitialized handle
   */
  static FrameBuffer DownCast(BaseHandle handle);

  /**
   * @brief Assignment operator, changes this handle to point at the same object.
   *
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   * @return Reference to the assigned object
   */
  FrameBuffer& operator=(const FrameBuffer& handle);

  /**
   * @brief Move constructor.
   *
   * @SINCE_1_9.22
   * @param[in] rhs A reference to the moved handle
   */
  FrameBuffer(FrameBuffer&& rhs) noexcept;

  /**
   * @brief Move assignment operator.
   *
   * @SINCE_1_9.22
   * @param[in] rhs A reference to the moved handle
   * @return A reference to this handle
   */
  FrameBuffer& operator=(FrameBuffer&& rhs) noexcept;

  /**
   * @brief Attach the base LOD of a 2D texture to the framebuffer for color rendering.
   * @note This causes a color attachment to be added.
   * @note Repeated calls to this method add textures as subsequent color attachments.
   * @note A maximum of 8 color attachments are supported.
   *
   * @SINCE_1_1.43
   * @param[in] texture The texture that will be used as output when rendering
   * @note The texture has to have same size as that of FrameBuffer
   * otherwise it won't be attached.
   */
  void AttachColorTexture(Texture& texture);

  /**
   * @brief Attach a texture to the framebuffer for color rendering.
   * @note This causes a color attachment to be added.
   * @note Repeated calls to this method add textures as subsequent color attachments.
   * @note A maximum of 8 color attachments are supported.
   *
   * @SINCE_1_1.43
   * @param[in] texture The texture that will be used as output when rendering
   * @param[in] mipmapLevel The mipmap of the texture to be attached
   * @param[in] layer Indicates which layer of a cube map or array texture to attach. Unused for 2D textures
   * @note The mipmapped texture has to have same size as that of FrameBuffer
   * otherwise it won't be attached.
   */
  void AttachColorTexture(Texture& texture, uint32_t mipmapLevel, uint32_t layer);

  /**
   * @brief Gets the first color texture used as output in the FrameBuffer.
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
  explicit DALI_INTERNAL FrameBuffer(Internal::FrameBuffer* pointer);
};

/**
 * @}
 */
} //namespace Dali

#endif // DALI_FRAMEBUFFER_H
