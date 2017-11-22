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

#include <dali/graphics/vulkan/vulkan-buffer.h>
#include <dali/graphics/vulkan/vulkan-device-memory-manager.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-image.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

namespace
{

const uint32_t INVALID_MEMORY_INDEX = -1u;

/**
 * Helper function which returns GPU heap index that can be used to allocate
 * particular type of resource
 */
uint32_t GetMemoryIndex(const vk::PhysicalDeviceMemoryProperties &memoryProperties,
                        uint32_t memoryTypeBits, vk::MemoryPropertyFlags properties)
{
  for(uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
  {
    if((memoryTypeBits & (1u << i)) &&
       ((memoryProperties.memoryTypes[i].propertyFlags & properties) == properties))
    {
      return i;
    }
  }
  return INVALID_MEMORY_INDEX;
}
}

namespace Impl
{
struct DeviceMemory
{
  DeviceMemoryManager& mManager;
  Graphics&            mGraphics;

  vk::DeviceMemory        mDeviceMemory;
  vk::MemoryPropertyFlags mProperties;
  vk::MemoryRequirements  mRequirements;

  std::atomic< uint32_t > mUserCount; // this is just a refcount

  DeviceMemory(DeviceMemoryManager &manager, Graphics &graphics,
                             const vk::MemoryRequirements &requirements,
                             vk::MemoryPropertyFlags       properties)
    : mManager(manager),
      mGraphics(graphics),
      mDeviceMemory{nullptr},
      mProperties{properties},
      mRequirements(requirements),
      mUserCount{0u}
  {
    auto info = vk::MemoryAllocateInfo{};
    info.setAllocationSize(requirements.size)
        .setMemoryTypeIndex(
          GetMemoryIndex(mGraphics.GetMemoryProperties(), requirements.memoryTypeBits, properties));

    mDeviceMemory = VkAssert(mGraphics.GetDevice().allocateMemory(info));
  }

  void* Map(uint32_t offset, uint32_t size)
  {
    return nullptr;
  }

  void Unmap()
  {
  }

  void Bind(Image &image, uint32_t offset)
  {
    assert(image.GetImage() && "Image is nullptr!");
    VkAssert(mGraphics.GetDevice().bindImageMemory(image.GetImage(), mDeviceMemory, offset));
    ++mUserCount;
  }

  void Bind(Vulkan::Buffer &buffer, uint32_t offset)
  {
  }


};

}

/*
 * DeviceMemory
 */

DeviceMemory::DeviceMemory(DeviceMemoryManager &manager, Graphics &graphics,
                           const vk::MemoryRequirements &requirements,
                           vk::MemoryPropertyFlags       properties)
{
  mImpl = MakeUnique<Impl::DeviceMemory>(manager, graphics, requirements, properties);
}

void* DeviceMemory::Map(uint32_t offset, uint32_t size)
{
  return mImpl->Map( offset, size );
}

void DeviceMemory::Unmap()
{
  mImpl->Unmap();
}

void DeviceMemory::Bind(Image& image, uint32_t offset)
{
  mImpl->Bind(image, offset);
}

void DeviceMemory::Bind(Vulkan::Buffer& buffer, uint32_t offset)
{
  mImpl->Bind(buffer, offset);
}


namespace Impl
{
struct DeviceMemoryManager
{
  Graphics& mGraphics;
  std::vector<Vulkan::DeviceMemory> mAllocations;

  DeviceMemoryManager(Graphics &graphics) : mGraphics(graphics)
  {
  }

  std::unique_ptr<Vulkan::DeviceMemory> Allocate(vk::MemoryRequirements requirements,
                                                              vk::MemoryPropertyFlags flags)
  {
    return MakeUnique<Vulkan::DeviceMemory>(*this, mGraphics, requirements, flags);
  }

/* Allocates memory for VkBuffer */
  const Vulkan::DeviceMemory& Allocate(Vulkan::Buffer &buffer, vk::MemoryPropertyFlags memoryFlags)
  {
    auto vkbuffer  = buffer.GetVkObject();
    auto vkdevice  = mGraphics.GetDevice();
    auto memoryReq = vkdevice.getBufferMemoryRequirements(vkbuffer);

    mAllocations.emplace_back(*this, mGraphics, memoryReq, vk::MemoryPropertyFlagBits::eHostVisible);

    return mAllocations.back();
  }
};

DeviceMemoryManager::DeviceMemoryManager(Graphics &graphics)
{
  mImpl = MakeUnique<Impl::DeviceMemoryManager>(graphics);
}

std::unique_ptr<DeviceMemory> DeviceMemoryManager::Allocate(vk::MemoryRequirements requirements,
                                                            vk::MemoryPropertyFlags flags)
{
  return MakeUnique<DeviceMemory>(*this, mGraphics, requirements, flags);
}

/* Allocates memory for VkBuffer */
const Vulkan::DeviceMemory &DeviceMemoryManager::Allocate(Vulkan::Buffer &buffer, vk::MemoryPropertyFlags memoryFlags)
{
  auto vkbuffer  = buffer.GetVkObject();
  auto vkdevice  = mGraphics.GetDevice();
  auto memoryReq = vkdevice.getBufferMemoryRequirements(vkbuffer);

  mAllocations.emplace_back(*this, mGraphics, memoryReq, vk::MemoryPropertyFlagBits::eHostVisible);

  return mAllocations.back();
}

}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
