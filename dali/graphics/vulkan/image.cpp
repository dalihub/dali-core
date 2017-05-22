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

#include <dali/graphics/vulkan/device-memory.h>
#include <dali/graphics/vulkan/image.h>
#include <dali/graphics/vulkan/logical-device.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

class ImageImpl : public VkObject
{
public:
  ImageImpl(const LogicalDevice& device, const vk::ImageCreateInfo& createInfo);

  /**
   * Initialises new image
   * @return
   */
  bool Initialise();

  /**
   * Creates VkImageMemoryBarrier instance in order to change image layout
   * @param newLayout
   * @param srcAccess
   * @param dstAccess
   * @return
   */
  vk::ImageMemoryBarrier CreateLayoutBarrier(vk::ImageLayout newLayout, vk::AccessFlags srcAccess,
                                             vk::AccessFlags dstAccess);

  const vk::Image& GetVkImage() const
  {
    return mImage;
  }

  bool BindDeviceMemory(const DeviceMemory& memory, size_t offset);

  const vk::ImageCreateInfo& GetImageCreateInfo() const
  {
    return mCreateInfo;
  }

  const vk::ImageLayout& GetLayout() const
  {
    return mLayout;
  }

  void SetLayout(vk::ImageLayout layout )
  {
    mLayout = layout;
  }

private:
  LogicalDevice       mDevice{nullptr};
  vk::ImageCreateInfo mCreateInfo{};
  vk::Image           mImage{nullptr};
  vk::ImageLayout     mLayout;

  DeviceMemory        mDeviceMemory{nullptr}; // todo future higher-level wrapper on device memory
  size_t              mBoundMemoryOffset{0u};
};

// Implementation

ImageImpl::ImageImpl(const LogicalDevice& device, const vk::ImageCreateInfo& createInfo)
: mDevice(device), mCreateInfo(createInfo)
{
}

bool ImageImpl::Initialise()
{
  mLayout = mCreateInfo.initialLayout;
  return VkTestBool(mDevice->createImage(&mCreateInfo, mDevice.GetVkAllocator(), &mImage));
}

bool ImageImpl::BindDeviceMemory(const DeviceMemory& memory, size_t offset)
{
  assert(!mDeviceMemory && "DeviceMemory is already set on the Image!");
  if( VkTestBool(mDevice->bindImageMemory(mImage, memory.GetVkDeviceMemory(), offset)) )
  {
    mDeviceMemory = memory;
    mBoundMemoryOffset = offset;
    return true;
  }
  return false;
}

vk::ImageMemoryBarrier ImageImpl::CreateLayoutBarrier(vk::ImageLayout newLayout,
                                                      vk::AccessFlags srcAccess, vk::AccessFlags dstAccess)
{
  return vk::ImageMemoryBarrier{};
}

// Handle
namespace
{
ImageImpl* GetImpl(Image* handle)
{
  return static_cast< ImageImpl* >(handle->GetObject());
}
ImageImpl* GetImpl(const Image* handle)
{
  return static_cast< ImageImpl* >(handle->GetObject());
}
}

Image Image::New(const LogicalDevice& device, const vk::ImageCreateInfo& createInfo)
{
  auto impl = new ImageImpl(device, createInfo);
  if(impl->Initialise())
    return impl;
  return nullptr;
}

const vk::Image& Image::GetVkResource() const
{
  return GetImpl(this)->GetVkImage();
}

const vk::Image& Image::operator*() const
{
  return GetImpl(this)->GetVkImage();
}

bool Image::BindDeviceMemory(const DeviceMemory& memory)
{
  return GetImpl(this)->BindDeviceMemory(memory, 0);
}

bool Image::BindDeviceMemory(const DeviceMemory& memory, size_t offset)
{
  return GetImpl(this)->BindDeviceMemory(memory, offset);
}

vk::ImageMemoryBarrier Image::GetLayoutChangeBarrier(vk::ImageLayout newLayout,
                                                     vk::AccessFlags srcAccess, vk::AccessFlags dstAccess,
                                                     vk::ImageAspectFlags imageAspect) const
{
  auto& info = GetImpl(this)->GetImageCreateInfo();

  vk::ImageSubresourceRange subresourceRange;
  subresourceRange.setBaseArrayLayer(0)
      .setLayerCount(info.arrayLayers)
      .setAspectMask(imageAspect)
      .setBaseArrayLayer(0)
      .setLevelCount(info.mipLevels)
      .setBaseMipLevel(0);

  vk::ImageMemoryBarrier barrier;
  barrier.setNewLayout(newLayout)
      .setOldLayout(GetImpl(this)->GetLayout())
      .setSrcAccessMask(srcAccess)
      .setDstAccessMask(dstAccess)
      .setSubresourceRange(subresourceRange)
      .setImage(GetImpl(this)->GetVkImage());

  return barrier;
}

void Image::SetLayout( vk::ImageLayout layout )
{
  GetImpl(this)->SetLayout(layout);
}

vk::ImageLayout Image::GetLayout() const
{
  return GetImpl(this)->GetLayout();
}
}
}
}
