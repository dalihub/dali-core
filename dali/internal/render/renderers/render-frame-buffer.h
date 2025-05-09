#ifndef DALI_INTERNAL_RENDER_FRAME_BUFFER_H
#define DALI_INTERNAL_RENDER_FRAME_BUFFER_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/threading/mutex.h>
#include <dali/internal/render/common/render-target-graphics-objects.h>
#include <dali/internal/render/renderers/render-sampler.h>
#include <dali/internal/render/renderers/render-texture-key.h>

#include <unordered_map>

namespace Dali
{
using Mask = Dali::FrameBuffer::Attachment::Mask;

namespace Internal
{
namespace SceneGraph
{
class RenderInstruction;
class RenderManager;
} //namespace SceneGraph

namespace Render
{
class Texture;

class FrameBuffer : public SceneGraph::RenderTargetGraphicsObjects
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
   * @brief Requests to Keep rendering result.
   */
  void KeepRenderResult();

  /**
   * @brief Requests to Clear the rendered result.
   * @note Since the rendered result is kept in the render thread resource, this method asynchronously clears the result.
   */
  void ClearRenderResult();

  /**
   * @brief Retrieves the rendered result as PixelData.
   * @return Dali::PixelData that contains rendered result.
   * If the frame is not yet rendered, empty handle is returned.
   */
  Dali::PixelData GetRenderResult();

  /**
   * @brief Retrieves whether keeping render result is requested or not.
   * @return True if keeping render result is requested.
   */
  bool IsKeepingRenderResultRequested() const;

  /**
   * @brief Retrieves buffer pointer that the render result is stored.
   * @return Buffer pointer of the render result buffer.
   */
  uint8_t* GetRenderResultBuffer();

  /**
   * @brief Notifies the drawing call of the frame is finished.
   */
  void SetRenderResultDrawn();

  /**
   * @brief Mark all textures updated.
   * @param[in] renderManager The render manager who will mark updated textures.
   */
  void UpdateAttachedTextures(SceneGraph::RenderManager& renderManager);

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

  /**
   * @brief Undefined copy constructor. FrameBuffer cannot be copied
   */
  FrameBuffer(const FrameBuffer& rhs) = delete;

  /**
   * @brief Undefined assignment operator. FrameBuffer cannot be copied
   */
  FrameBuffer& operator=(const FrameBuffer& rhs) = delete;

private:
  Graphics::UniquePtr<Graphics::Framebuffer> mGraphicsObject{nullptr};

  Graphics::FramebufferCreateInfo mCreateInfo;

  // Attachments texutres (not owned)
  std::vector<Render::TextureKey> mColorTextures{};
  Render::TextureKey              mDepthTexture;
  Render::TextureKey              mDepthStencilTexture;

  bool            mIsKeepingRenderResultRequested{false};
  bool            mIsRenderResultDrawn{false};
  uint8_t*        mRenderResult;
  Dali::PixelData mRenderedPixelData;
  Dali::Mutex     mPixelDataMutex;

  uint32_t mWidth;
  uint32_t mHeight;

  bool mDepthBuffer;
  bool mStencilBuffer;
};

} // namespace Render

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_RENDER_FRAME_BUFFER_H
