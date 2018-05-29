#ifndef DALI_GRAPHICS_VULKAN_IMAGE
#define DALI_GRAPHICS_VULKAN_IMAGE

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
class ImageView;
using InternalVkImage = vk::Image;
class Image : public VkManaged
{
public:
  /**
   * Creates new managed Image object
   * @param graphics
   * @param createInfo
   * @return
   */
  static RefCountedImage New( Graphics& graphics, vk::ImageCreateInfo createInfo );

  /**
   * Creates new managed object from external image, lifecycle must be managed
   * explicitly, as well as any data
   * @param graphics
   * @param createInfo
   * @param image
   * @return
   */
  static RefCountedImage New( Graphics& graphics, vk::ImageCreateInfo createInfo, vk::Image externalImage );

  /**
   * Destructor
   */
  ~Image() override;

  /**
   * Returns underlying Vulkan object
   * @return
   */
  vk::Image GetVkHandle() const;

  /**
   * Returns VkImageLayout associated with the image
   * @return
   */
  vk::ImageLayout GetVkImageLayout() const;

  /**
   * Returns width in pixels
   * @return
   */
  uint32_t GetWidth() const;

  /**
   * Returns height in pixels
   * @return
   */
  uint32_t GetHeight() const;

  /**
   * Returns number of layers
   * @return
   */
  uint32_t GetLayerCount() const;

  /**
   * Returns number of mipmap levels
   * @return
   */
  uint32_t GetMipLevelCount() const;

  /**
   * Returns pixel format
   * @return
   */
  vk::Format GetVkFormat() const;

  /**
   * returns image type ( VkImageType)
   * @return
   */
  vk::ImageType GetVkImageType() const;

  /**
   * Returns used image tiling mode
   * @return
   */
  vk::ImageTiling GetVkImageTiling() const;

  /**
   *
   * @return
   */
  vk::ImageUsageFlags  GetVkImageUsageFlags() const;

  /**
   * Binds image memory
   * @param handle
   */
  void BindMemory( const RefCountedGpuMemoryBlock& handle );

  /**
   * Creates new VkImage with given specification, it doesn't
   * bind the memory.
   * @param graphics
   * @param createInfo
   */
  Image( Graphics& graphics, const vk::ImageCreateInfo& createInfo, vk::Image externalImage );

  operator InternalVkImage() const
  {
    return GetVkHandle();
  }

private:
  struct Impl;
  std::unique_ptr<Impl> mImpl;
};

} // namespace Vulkan

} // namespace Graphics

} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_IMAGE
