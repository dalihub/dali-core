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

class DeviceMemory::Impl
{
public:
  /**
   *
   * @param graphics
   * @param requirements
   * @param properties
   */
  Impl(Graphics &graphics,
               const vk::MemoryRequirements &requirements,
               vk::MemoryPropertyFlags       properties)
    : mGraphics(graphics),
      mDeviceMemory{nullptr},
      mProperties{properties},
      mRequirements(requirements),
      mUserCount{0u}
  {

  }

  bool Initialise()
  {
    auto info = vk::MemoryAllocateInfo{};
    info.setAllocationSize(mRequirements.size)
        .setMemoryTypeIndex(
          GetMemoryIndex(mGraphics.GetMemoryProperties(), mRequirements.memoryTypeBits, mProperties));

    mDeviceMemory = VkAssert(mGraphics.GetDevice().allocateMemory(info));
    return true;//mDeviceMemory != nullptr;
  }
  /**
   *
   * @param offset
   * @param size
   * @return
   */
  void* Map(uint32_t offset, uint32_t size)
  {
    assert( !mMappedPointer && "Memory is already mapped!" );
    mMappedPointer = VkAssert(mGraphics.GetDevice().mapMemory(mDeviceMemory,offset, size));
    if( mMappedPointer )
      ++mUserCount;
    return mMappedPointer;
  }

  /**
   *
   */
  void Unmap()
  {
    mGraphics.GetDevice().unmapMemory( mDeviceMemory );
    mMappedPointer = nullptr;
    --mUserCount;
  }

  vk::DeviceMemory GetVkDeviceMemory() const
  {
    return mDeviceMemory;
  }

private:
  //Vulkan::DeviceMemoryManager& mManager;
  Graphics&                    mGraphics;

  vk::DeviceMemory             mDeviceMemory;
  vk::MemoryPropertyFlags      mProperties;
  vk::MemoryRequirements       mRequirements;

  void*                        mMappedPointer { nullptr };
  std::atomic< uint32_t > mUserCount; // this is just a refcount
};

/*
 * DeviceMemory
 */
DeviceMemory::DeviceMemory()
: mImpl(nullptr)
{
}

DeviceMemory::DeviceMemory(Graphics &graphics,
                           const vk::MemoryRequirements &requirements,
                           vk::MemoryPropertyFlags       properties)
{
  mImpl = MakeUnique<Impl>(graphics, requirements, properties);
}

DeviceMemory::~DeviceMemory()
{

}

DeviceMemory::operator bool() const
{
  return mImpl == nullptr;
}

void* DeviceMemory::Map(uint32_t offset, uint32_t size)
{
  return mImpl->Map( offset, size );
}

void* DeviceMemory::Map()
{
  return mImpl->Map( 0u, static_cast<uint32_t>(VK_WHOLE_SIZE) );
}

void DeviceMemory::Unmap()
{
  mImpl->Unmap();
}

class DeviceMemoryManager::Impl final
{
public:

  /**
   *
   * @param graphics
   */
  Impl(Graphics &graphics) : mGraphics(graphics)
  {
  }

  /**
   *
   * @param requirements
   * @param flags
   * @return
   */
  std::unique_ptr<Vulkan::DeviceMemory> Allocate(vk::MemoryRequirements requirements,
                                                 vk::MemoryPropertyFlags flags)
  {
    auto deviceMemory = MakeUnique<Vulkan::DeviceMemory>(mGraphics, requirements, flags);

  }

  /* Allocates memory for VkBuffer */
  const Vulkan::DeviceMemory& Allocate(Vulkan::Buffer &buffer, vk::MemoryPropertyFlags memoryFlags)
  {
    auto vkbuffer  = buffer.GetVkBuffer();
    auto vkdevice  = mGraphics.GetDevice();
    auto memoryReq = vkdevice.getBufferMemoryRequirements(vkbuffer);

    mAllocations.emplace_back(
      MakeUnique<Vulkan::DeviceMemory>(mGraphics, memoryReq, vk::MemoryPropertyFlagBits::eHostVisible)
    );

    return *mAllocations.back().get();
  }

private:

  Graphics& mGraphics;
  std::vector<std::unique_ptr<Vulkan::DeviceMemory>> mAllocations;

};

DeviceMemoryManager::~DeviceMemoryManager() = default;

DeviceMemoryManager::DeviceMemoryManager(Graphics &graphics)
{
  mImpl = MakeUnique<Impl>(graphics);
}

std::unique_ptr<DeviceMemory> DeviceMemoryManager::Allocate(vk::MemoryRequirements requirements,
                                                            vk::MemoryPropertyFlags flags)
{
  return mImpl->Allocate( requirements, flags  );
}

/* Allocates memory for VkBuffer */
const Vulkan::DeviceMemory &DeviceMemoryManager::Allocate(Vulkan::Buffer &buffer, vk::MemoryPropertyFlags memoryFlags)
{
  return mImpl->Allocate( buffer, memoryFlags );
}

vk::DeviceMemory DeviceMemory::GetVkDeviceMemory() const
{
  return mImpl->GetVkDeviceMemory();
}


} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
