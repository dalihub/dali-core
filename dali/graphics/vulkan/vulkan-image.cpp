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

#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-image.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

Image::Image(Graphics& graphics, const vk::ImageCreateInfo& createInfo) : mGraphics(graphics)
{
}

Image::Image(Graphics& graphics, vk::Image externalImage)
: mGraphics(graphics), mImage(externalImage)
{
}

vk::ImageView Image::CreateUnmanagedView(const vk::ImageViewCreateInfo& info)
{
  NotImplemented();
  return nullptr;
}

ImageViewRef Image::CreateView(const vk::ImageViewCreateInfo& info)
{
  return MakeRef< ImageView >(mGraphics, *this, info);
}

ImageView::ImageView(Graphics& graphics, Image& image) : mGraphics(graphics), mImageRef(&image)
{
}

ImageView::ImageView(Graphics& graphics, Image& image, const VkImageViewCreateInfo& createInfo)
: mGraphics(graphics), mImageRef(&image)
{
  mImageView =
      VkAssert(mGraphics.GetDevice().createImageView(createInfo, mGraphics.GetAllocator()));
}

ImageView::~ImageView()
{
  if(mImageView)
  {
    mGraphics.GetDevice().destroyImageView( mImageView, mGraphics.GetAllocator());
  }
}

} // namespace Vulkan

} // namespace Graphics

} // namespace Dali