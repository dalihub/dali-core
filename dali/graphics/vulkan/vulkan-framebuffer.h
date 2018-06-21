#ifndef DALI_GRAPHICS_VULKAN_FRAMEBUFFER
#define DALI_GRAPHICS_VULKAN_FRAMEBUFFER

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

#include <dali/graphics/vulkan/vulkan-types.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

class Image;

enum class AttachmentType
{
  COLOR,
  DEPTH_STENCIL,
  DEPTH,
  INPUT,
  RESOLVE,
  PRESERVE,
  UNDEFINED
};

class Framebuffer;

class FramebufferAttachment : public VkManaged
{
public:
  friend class Framebuffer;

  static RefCountedFramebufferAttachment NewColorAttachment( RefCountedImageView imageView,
                                                             vk::ClearColorValue clearColorValue,
                                                             bool presentable );

  static RefCountedFramebufferAttachment NewDepthAttachment( RefCountedImageView imageView,
                                                             vk::ClearDepthStencilValue clearDepthStencilValue );

  RefCountedImageView GetImageView() const;

  const vk::AttachmentDescription& GetDescription() const;

  const vk::ClearValue& GetClearValue() const;

  bool IsValid() const;

private:
  FramebufferAttachment() = default;

  FramebufferAttachment( RefCountedImageView imageView,
                         vk::ClearValue clearColor,
                         bool presentable  );

  RefCountedImageView mImageView;

  vk::AttachmentDescription mDescription;

  vk::ClearValue mClearValue;
};

/**
 * Framebuffer encapsulates following objects:
 * - Images ( attachments )
 * - Framebuffer
 * - ImageViews
 */
class Framebuffer : public VkManaged
{
public:
  friend class Graphics;

  uint32_t GetWidth() const;

  uint32_t GetHeight() const;

  RefCountedFramebufferAttachment GetAttachment( AttachmentType type, uint32_t index ) const;

  std::vector< RefCountedFramebufferAttachment > GetAttachments( AttachmentType type ) const;

  uint32_t GetAttachmentCount( AttachmentType type ) const;

  vk::RenderPass GetRenderPass() const;

  vk::Framebuffer GetVkHandle() const;

  std::vector< vk::ClearValue > GetClearValues() const;

  bool OnDestroy() override;

private:
  static RefCountedFramebuffer New( Graphics& graphics,
                                    const std::vector< RefCountedFramebufferAttachment >& colorAttachments,
                                    const RefCountedFramebufferAttachment& depthAttachment,
                                    vk::Framebuffer vkHandle,
                                    vk::RenderPass renderPass,
                                    uint32_t width,
                                    uint32_t height,
                                    bool externalRenderPass = false );

  Framebuffer( Graphics& graphics,
               const std::vector< RefCountedFramebufferAttachment >& colorAttachments,
               const RefCountedFramebufferAttachment& depthAttachment,
               vk::Framebuffer vkHandle,
               vk::RenderPass renderPass,
               uint32_t width,
               uint32_t height,
               bool externalRenderPass = false );

private:
  Graphics* mGraphics;

  uint32_t mWidth;
  uint32_t mHeight;

  std::vector< RefCountedFramebufferAttachment > mColorAttachments;

  RefCountedFramebufferAttachment mDepthAttachment;

  vk::Framebuffer mFramebuffer;

  vk::RenderPass mRenderPass;

  bool mExternalRenderPass;
};


} // Namespace Vulkan

} // Namespace Graphics

} // Namespace Dali

#endif // DALI_GRAPHICS_VULKAN_FRAMEBUFFER
