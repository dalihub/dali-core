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

/**
 * Framebuffer encapsulates following objects:
 * - Images ( attachments )
 * - Framebuffer
 * - ImageViews
 */
class Framebuffer : public VkManaged
{
public:

  enum class AttachmentType
  {
    COLOR,
    DEPTH_STENCIL,
    DEPTH,
    INPUT,
    RESOLVE,
    PRESERVE
  };

  static FramebufferRef New( Graphics& graphics, uint32_t width, uint32_t height );

  uint32_t GetWidth() const;

  uint32_t GetHeight() const;

  ImageViewRef GetAttachment( AttachmentType type, uint32_t index ) const;

  std::vector<ImageViewRef> GetAttachments( AttachmentType type ) const;

  uint32_t GetAttachmentCount( AttachmentType type ) const;

  void SetAttachment( ImageViewRef imageViewRef, Framebuffer::AttachmentType type, uint32_t index );

  void Commit();

private:

  Framebuffer( Graphics& graphics, uint32_t width, uint32_t height );

  struct Impl;
  std::unique_ptr<Impl> mImpl;

};


} // Namespace Vulkan

} // Namespace Graphics

} // Namespace Dali

#endif // DALI_GRAPHICS_VULKAN_FRAMEBUFFER
