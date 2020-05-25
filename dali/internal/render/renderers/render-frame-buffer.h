#ifndef DALI_INTERNAL_RENDER_FRAME_BUFFER_H
#define DALI_INTERNAL_RENDER_FRAME_BUFFER_H

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
 */

// INTERNAL INCLUDES
#include <dali/public-api/rendering/frame-buffer.h>
#include <dali/devel-api/rendering/frame-buffer-devel.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/renderers/render-sampler.h>
#include <dali/integration-api/gl-defines.h>

namespace Dali
{
using Mask = Dali::FrameBuffer::Attachment::Mask;

namespace Internal
{
namespace Render
{
class Texture;

class FrameBuffer
{
public:

  /**
   * Constructor
   */
  FrameBuffer( uint32_t width, uint32_t height, Mask attachments );

  /**
   * Destructor
   */
  virtual ~FrameBuffer();

  /**
   * Creates a FrameBuffer object in the GPU.
   * @param[in] context The GL context
   */
  virtual void Initialize( Context& context );

  /**
   * Deletes the framebuffer object from the GPU
   * @param[in] context The GL context
   */
  virtual void Destroy( Context& context );

  /**
   * Called by RenderManager to inform the framebuffer that the context has been destroyed
   */
  virtual void GlContextDestroyed();

  /**
   * @brief Bind the framebuffer
   * @param[in] context The GL context
   */
  virtual void Bind( Context& context );

  /**
   * @brief Get the width of the FrameBuffer
   * @return The width of the framebuffer
   */
  virtual uint32_t GetWidth() const;

  /**
   * @brief Get the height of the FrameBuffer
   * @return The height of the framebuffer
   */
  virtual uint32_t GetHeight() const;

  /**
   * @brief Attaches a texture for the color rendering. This API is valid only for frame buffer with COLOR attachments.
   * @param[in] context The GL context
   * @param[in] texture The texture that will be used as output when rendering
   * @param[in] mipmapLevel The mipmap of the texture to be attached
   * @param[in] layer Indicates which layer of a cube map or array texture to attach. Unused for 2D textures
   * @note A maximum of Dali::FrameBuffer::MAX_COLOR_ATTACHMENTS are supported.
   */
  void AttachColorTexture( Context& context, Render::Texture* texture, uint32_t mipmapLevel, uint32_t layer );

  /**
   * @brief Attaches a texture for the depth rendering. This API is valid only for frame buffer with DEPTH attachments.
   * @param[in] context The GL context
   * @param[in] texture The texture that will be used as output when rendering
   * @param[in] mipmapLevel The mipmap of the texture to be attached
   */
  void AttachDepthTexture( Context& context, Render::Texture* texture, uint32_t mipmapLevel );

  /**
   * @brief Attaches a texture for the depth/stencil rendering. This API is valid only for frame buffer with DEPTH_STENCIL attachments.
   * @param[in] context The GL context
   * @param[in] texture The texture that will be used as output when rendering
   * @param[in] mipmapLevel The mipmap of the texture to be attached
   */
  void AttachDepthStencilTexture( Context& context, Render::Texture* texture, uint32_t mipmapLevel );

  /**
   * @brief Get the number of textures bound to this frame buffer as color attachments.
   * @return The number of color attachments.
   */
  uint8_t GetColorAttachmentCount() const { return mColorAttachmentCount; }

  /**
   * @brief Get the id (OpenGL handle) of the texture bound to this frame buffer as color attachment @a index.
   * @return The texture id.
   */
  GLuint GetTextureId(uint8_t index) { return mTextureId[index]; };

private:

  /**
   * @brief Undefined copy constructor. FrameBuffer cannot be copied
   */
  FrameBuffer( const FrameBuffer& rhs ) = delete;

  /**
   * @brief Undefined assignment operator. FrameBuffer cannot be copied
   */
  FrameBuffer& operator=( const FrameBuffer& rhs ) = delete;

private:

  GLuint mId;
  GLuint mTextureId[ Dali::DevelFrameBuffer::MAX_COLOR_ATTACHMENTS ];
  GLuint mDepthBuffer;
  GLuint mStencilBuffer;
  uint32_t mWidth;
  uint32_t mHeight;
  uint8_t mColorAttachmentCount;
};

} // namespace Render

} // namespace Internal

} // namespace Dali


#endif // DALI_INTERNAL_RENDER_FRAME_BUFFER_H
