#ifndef DALI_GRAPHICS_VULKAN_IMAGE_VIEW
#define DALI_GRAPHICS_VULKAN_IMAGE_VIEW

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

/*
 * ImageView
 */
class ImageView : public VkManaged
{
public:

  /**
   * Creates ImageView according to the given spec
   * @param graphics
   * @param image
   * @param info
   * @return
   */
  static RefCountedImageView New( Graphics& graphics, const RefCountedImage& image, const vk::ImageViewCreateInfo& createInfo);


  ~ImageView() override;

  /**
   *
   * @return
   */
  vk::ImageView GetVkHandle() const;

  /**
   * Returns bound ImageRef
   * @return
   */
  RefCountedImage GetImage() const;

  /**
   *
   * @return
   */
  uint32_t GetLayerCount() const;

  /**
   *
   * @return
   */
  uint32_t GetMipLevelCount() const;

  /**
   *
   * @return
   */
  vk::ImageAspectFlags GetImageAspectMask() const;

  const ImageView& ConstRef();

  ImageView& Ref();

  operator vk::ImageView*();

  bool OnDestroy() override;
//TODO: Use the graphics class to manage lifetime.

private:
  ImageView( Graphics& graphics,
             RefCountedImage image,
             vk::ImageViewCreateInfo createInfo );

private:
  Graphics*                      mGraphics;
  RefCountedImage                mImage;
  vk::ImageViewCreateInfo        mCreateInfo;
  vk::ImageView                  mImageView;
};

} //namespace Vulkan
} //namespace Graphics
} //namespace Dali

#endif //DALI_GRAPHICS_VULKAN_IMAGE_VIEW
