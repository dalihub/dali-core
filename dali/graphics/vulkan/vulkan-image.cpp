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
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-handle.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

Image::~Image() = default;

RefCountedImage Image::New( Graphics& graphics, vk::ImageCreateInfo createInfo )
{
  return RefCountedImage( new Image( graphics, createInfo, nullptr ) );
}

RefCountedImage Image::NewFromExternal( Graphics& graphics, vk::ImageCreateInfo createInfo, vk::Image externalImage )
{
  return RefCountedImage( new Image( graphics, createInfo, externalImage ) );
}

Image::Image( Graphics& graphics, const vk::ImageCreateInfo& createInfo, vk::Image externalImage )
        : mGraphics(&graphics),
          mCreateInfo(createInfo),
          mImage(externalImage),
          mImageLayout(mCreateInfo.initialLayout),
          mIsExternal( static_cast<bool>(externalImage))
{
}

vk::Image Image::GetVkHandle() const
{
  return mImage;
}

vk::ImageLayout Image::GetImageLayout() const
{
  return mImageLayout;
}

uint32_t Image::GetWidth() const
{
  return mCreateInfo.extent.width;
}

uint32_t Image::GetHeight() const
{
  return mCreateInfo.extent.height;
}

uint32_t Image::GetLayerCount() const
{
  return mCreateInfo.arrayLayers;
}

uint32_t Image::GetMipLevelCount() const
{
  return mCreateInfo.mipLevels;
}

vk::Format Image::GetFormat() const
{
  return mCreateInfo.format;
}

vk::ImageType Image::GetImageType() const
{
  return mCreateInfo.imageType;
}

vk::ImageTiling Image::GetImageTiling() const
{
  return mCreateInfo.tiling;
}

void Image::AssignMemory( RefCountedGpuMemoryBlock memory )
{
  mDeviceMemory = memory;
}

const Image& Image::ConstRef()
{
  return *this;
}

Image& Image::Ref()
{
  return *this;
}

Image::operator vk::Image*()
{
  return &mImage;
}

vk::ImageUsageFlags Image::GetUsageFlags() const
{
  return mCreateInfo.usage;
}

bool Image::OnDestroy()
{
  if( !mIsExternal )
  {
    mGraphics->RemoveImage(*this);

    auto device = mGraphics->GetDevice();
    auto image = mImage;
    auto allocator = &mGraphics->GetAllocator();

    mGraphics->DiscardResource([device, image, allocator]() {
      device.destroyImage(image, allocator);
    });
  }

  return false;
}

} // namespace Vulkan

} // namespace Graphics

} // namespace Dali