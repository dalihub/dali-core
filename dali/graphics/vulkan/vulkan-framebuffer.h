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
    INPUT,
    RESOLVE,
    PRESERVE
  };

  static Handle<Framebuffer> New( Graphics& graphics, uint32_t width, uint32_t height );

  uint32_t GetWidth() const;

  uint32_t GetHeight() const;

  Handle<Image> GetAttachmentImage( AttachmentType type, uint32_t index ) const;

  Handle<ImageView> GetAttachmentImageView( AttachmentType type, uint32_t index ) const;

  void SetAttachment( Handle<Image> image, Framebuffer::AttachmentType type, uint32_t index );

private:

  class Impl;
  std::unique_ptr<Impl> mImpl;

};


} // Namespace Vulkan

} // Namespace Graphics

} // Namespace Dali

#endif // DALI_GRAPHICS_VULKAN_FRAMEBUFFER
