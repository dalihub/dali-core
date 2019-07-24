#ifndef DALI_INTERNAL_RENDER_TEXTURE_FRAME_BUFFER_H
#define DALI_INTERNAL_RENDER_TEXTURE_FRAME_BUFFER_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/internal/render/renderers/render-frame-buffer.h>

namespace Dali
{
using Mask = Dali::FrameBuffer::Attachment::Mask;

namespace Internal
{
namespace Render
{
class Texture;

class TextureFrameBuffer : public FrameBuffer
{
public:

  /**
   * Constructor
   * @param[in] width The width of the FrameBuffer
   * @param[in] height The height of the FrameBuffer
   * @param[in] attachments The attachments comprising the format of the FrameBuffer (bit-mask)
   */
  TextureFrameBuffer( uint32_t width, uint32_t height, Mask attachments );

  /**
   * Destructor
   */
  virtual ~TextureFrameBuffer();

  /**
   * @copydoc Dali::Internal::Renderer::FrameBuffer::Initialize()
   */
  void Initialize( Context& context ) override;

  /**
   * @copydoc Dali::Internal::Renderer::FrameBuffer::Destroy()
   */
  void Destroy( Context& context ) override;

  /**
   * @copydoc Dali::Internal::Renderer::FrameBuffer::GlContextDestroyed()
   */
  void GlContextDestroyed() override;

  /**
   * @copydoc Dali::Internal::Renderer::FrameBuffer::Bind()
   */
  void Bind( Context& context ) override;

  /**
   * @copydoc Dali::Internal::Renderer::FrameBuffer::GetWidth()
   */
  uint32_t GetWidth() const override;

  /**
   * @copydoc Dali::Internal::Renderer::FrameBuffer::GetHeight()
   */
  uint32_t GetHeight() const override;

  /**
   * @copydoc Dali::Internal::Renderer::FrameBuffer::IsSurfaceBacked()
   */
  bool IsSurfaceBacked() override { return false; };

  /**
   * @brief Attach a texture for color rendering. Valid only for Framebuffers with COLOR attachments.
   * param[in] context The GL context
   * @param[in] texture The texture that will be used as output when rendering
   * @param[in] mipmapLevel The mipmap of the texture to be attached
   * @param[in] layer Indicates which layer of a cube map or array texture to attach. Unused for 2D textures
   */
  void AttachColorTexture( Context& context, Render::Texture* texture, uint32_t mipmapLevel, uint32_t layer );

  /**
   * @brief Get the id of the texture binded to this frame buffer
   * @return The texture id
   */
  GLuint GetTextureId() { return mTextureId; };

private:

  GLuint mId;
  GLuint mTextureId;
  GLuint mDepthBuffer;
  GLuint mStencilBuffer;
  uint32_t mWidth;
  uint32_t mHeight;
};


} // namespace Render

} // namespace Internal

} // namespace Dali


#endif // DALI_INTERNAL_RENDER_TEXTURE_FRAME_BUFFER_H
