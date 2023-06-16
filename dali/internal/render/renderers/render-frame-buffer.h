#ifndef DALI_INTERNAL_RENDER_FRAME_BUFFER_H
#define DALI_INTERNAL_RENDER_FRAME_BUFFER_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/rendering/frame-buffer-devel.h>
#include <dali/internal/render/renderers/render-sampler.h>
#include <dali/public-api/rendering/frame-buffer.h>

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
  FrameBuffer(uint32_t width, uint32_t height, Mask attachments);

  /**
   * Destructor
   */
  virtual ~FrameBuffer();

  /**
   * Creates a FrameBuffer object in the GPU.
   * @param[in] graphicsController The Graphics Controller
   */
  virtual void Initialize(Graphics::Controller& graphicsController);

  /**
   * Deletes the framebuffer object from the GPU
   */
  virtual void Destroy();

  /**
   * @brief Create the graphics objects if needed.
   *
   * Doesn't re-create them if they are already generated, also doesn't
   * check for new attachments.
   *
   * Creates the framebuffer, attaches color and depth textures, set multi sampling level,
   * generates render target and render passes.
   *
   * @return true if there are attachments and the graphics objects have been created.
   */
  virtual bool CreateGraphicsObjects();

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
   * @param[in] texture The texture that will be used as output when rendering
   * @param[in] mipmapLevel The mipmap of the texture to be attached
   * @param[in] layer Indicates which layer of a cube map or array texture to attach. Unused for 2D textures
   * @note A maximum of Dali::FrameBuffer::MAX_COLOR_ATTACHMENTS are supported.
   */
  void AttachColorTexture(Render::Texture* texture, uint32_t mipmapLevel, uint32_t layer);

  /**
   * @brief Attaches a texture for the depth rendering. This API is valid only for frame buffer with DEPTH attachments.
   * @param[in] texture The texture that will be used as output when rendering
   * @param[in] mipmapLevel The mipmap of the texture to be attached
   */
  void AttachDepthTexture(Render::Texture* texture, uint32_t mipmapLevel);

  /**
   * @brief Attaches a texture for the depth/stencil rendering. This API is valid only for frame buffer with DEPTH_STENCIL attachments.
   * @param[in] texture The texture that will be used as output when rendering
   * @param[in] mipmapLevel The mipmap of the texture to be attached
   */
  void AttachDepthStencilTexture(Render::Texture* texture, uint32_t mipmapLevel);

  /**
   * @brief Sets the level of multisampling in the frame buffer. This API is valid only GL_EXT_multisampled_render_to_texture supported.
   * @param[in] multiSamplingLevel The level of multisampling
   */
  void SetMultiSamplingLevel(uint8_t multiSamplingLevel);

  /**
   * @brief Get the number of textures bound to this frame buffer as color attachments.
   * @return The number of color attachments.
   */
  uint8_t GetColorAttachmentCount() const
  {
    return mCreateInfo.colorAttachments.size();
  }

  Graphics::Framebuffer* GetGraphicsObject()
  {
    return mGraphicsObject.get();
  }

  [[nodiscard]] Graphics::RenderTarget* GetGraphicsRenderTarget() const
  {
    return mRenderTarget.get();
  }

  [[nodiscard]] Graphics::RenderPass* GetGraphicsRenderPass(Graphics::AttachmentLoadOp  colorLoadOp,
                                                            Graphics::AttachmentStoreOp colorStoreOp) const;

  /**
   * The function returns initialized array of clear values
   * which then can be modified and assed to BeginRenderPass()
   * command.
   */
  [[nodiscard]] auto& GetGraphicsRenderPassClearValues()
  {
    return mClearValues;
  }

private:
  /**
   * @brief Undefined copy constructor. FrameBuffer cannot be copied
   */
  FrameBuffer(const FrameBuffer& rhs) = delete;

  /**
   * @brief Undefined assignment operator. FrameBuffer cannot be copied
   */
  FrameBuffer& operator=(const FrameBuffer& rhs) = delete;

private:
  Graphics::Controller*                      mGraphicsController{nullptr};
  Graphics::UniquePtr<Graphics::Framebuffer> mGraphicsObject{nullptr};

  Graphics::FramebufferCreateInfo mCreateInfo;

  // Render pass and render target

  /**
   * Render passes are created on fly depending on Load and Store operations
   * The default render pass (most likely to be used) is the load = CLEAR
   * amd store = STORE for color attachment.
   */
  std::vector<Graphics::UniquePtr<Graphics::RenderPass>> mRenderPass{};
  Graphics::UniquePtr<Graphics::RenderTarget>            mRenderTarget{nullptr};

  // clear colors
  std::vector<Graphics::ClearValue> mClearValues{};

  uint32_t mWidth;
  uint32_t mHeight;

  bool mDepthBuffer;
  bool mStencilBuffer;
};

} // namespace Render

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_RENDER_FRAME_BUFFER_H
