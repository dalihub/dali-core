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
#include <dali/graphics/vulkan/internal/vulkan-image.h>
#include <dali/graphics/vulkan/internal/vulkan-debug.h>
#include <dali/graphics/vulkan/internal/vulkan-gpu-memory-handle.h>

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
        : mGraphics( &graphics ),
          mCreateInfo( createInfo ),
          mImage( externalImage ),
          mImageLayout( mCreateInfo.initialLayout ),
          mIsExternal( static_cast<bool>(externalImage) )
{
  auto depthFormats = std::vector< vk::Format >{
          vk::Format::eD32Sfloat,
          vk::Format::eD16Unorm,
          vk::Format::eD32SfloatS8Uint,
          vk::Format::eD24UnormS8Uint,
          vk::Format::eD16UnormS8Uint
  };

  auto hasDepth = std::find( depthFormats.begin(), depthFormats.end(), createInfo.format );

  if( hasDepth != depthFormats.end() )
  {
    auto format = *hasDepth;

    if( format == vk::Format::eD32Sfloat || format == vk::Format::eD16Unorm )
    {
      mAspectFlags = vk::ImageAspectFlagBits::eDepth;
    }
    else
    {
      mAspectFlags = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
    }
  }
  else
  {
    mAspectFlags = vk::ImageAspectFlagBits::eColor;
  }
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

vk::ImageAspectFlags Image::GetAspectFlags() const
{
  return mAspectFlags;
}

void Image::AssignMemory( RefCountedGpuMemoryBlock memory )
{
  mDeviceMemory = memory;
}

void Image::SetImageLayout( vk::ImageLayout imageLayout )
{
  mImageLayout = imageLayout;
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

vk::SampleCountFlagBits Image::GetSampleCount() const
{
  return mCreateInfo.samples;
}

bool Image::OnDestroy()
{
  if( !mIsExternal )
  {
    if( !mGraphics->IsShuttingDown() )
    {
      mGraphics->RemoveImage( *this );
    }

    auto device = mGraphics->GetDevice();
    auto image = mImage;
    auto allocator = &mGraphics->GetAllocator();

    mGraphics->DiscardResource( [ device, image, allocator ]() {
      DALI_LOG_INFO( gVulkanFilter, Debug::General, "Invoking deleter function: image->%p\n",
                     static_cast< void* >(image) )
      device.destroyImage( image, allocator );
    } );
  }

  return false;
}

} // namespace Vulkan

} // namespace Graphics

} // namespace Dali
