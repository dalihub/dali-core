#ifndef DALI_GRAPHICS_VULKAN_IMAGE_H
#define DALI_GRAPHICS_VULKAN_IMAGE_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include <dali/graphics/vulkan/types.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

class ImageView;
enum class ResourceOwnershipType
{
  OWNED,
  EXTERNAL
};

class Image : public Resource
{
public:
  /**
   * Creates new VkImage with given specification, it doesn't
   * bind the memory.
   * @param graphics
   * @param createInfo
   */
  Image(Graphics& graphics, const vk::ImageCreateInfo& createInfo);

  /**
   * Constructor creates wrapper on the VkImage coming from external
   * source. It doesn't take over ownership so it's still owner's
   * responsibility to destroy it and maintain the usage.
   *
   * @param graphics
   * @param externalImage
   */
  Image(Graphics& graphics, vk::Image externalImage);

  virtual ~Image() = default;

  /**
   * Creates UNMANAGED VkImageView from the current image.
   * Usage requires external lifecycle management and synchronization
   * Memory MUST be bound for this function to work!
   * @param info
   * @return
   */
  vk::ImageView CreateUnmanagedView(const vk::ImageViewCreateInfo& info);

  /**
   * Creates MANAGED ImageView from the current image
   * Memory MUST be bound for this function to work!
   * @param info
   * @return
   */
  UniqueImageView CreateView(const vk::ImageViewCreateInfo& info);

  /**
   * Returns underlying Vulkan object
   * @return
   */
  vk::Image GetImage() const
  {
    return mImage;
  }

private:

  Graphics& mGraphics;

  vk::Image       mImage;
  vk::ImageLayout mLayout;

  ResourceOwnershipType mOwnershipType;
};

/*
 * ImageView
 * todo: move it to its own file
 */
class ImageView
{
public:
  ImageView(Graphics& graphics, Image& image);
  ImageView(Graphics& graphics, Image& image, const VkImageViewCreateInfo& createInfo);

  const vk::ImageView& GetImageView() const
  {
    return mImageView;
  }

  Image& GetImage() const
  {
    return mImageRef.GetResource();
  }

private:
  Graphics&             mGraphics;
  ResourceRef<Image>    mImageRef;

  vk::ImageView mImageView;
};

} // namespace Vulkan

} // namespace Graphics

} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_IMAGE_H
