//
// Created by adam.b on 06/06/17.
//

#ifndef VULKAN_PROJECT_DEVICEMEMORYMANAGER_H
#define VULKAN_PROJECT_DEVICEMEMORYMANAGER_H

#include <dali/graphics/vulkan/vulkan-types.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

class Graphics;
class Image;
class Buffer;
class DeviceMemoryManager;

/**
 * DeviceMemory represents continuous memory block with particular properties
 * like being mappable etc. The actual memory is being allocated from DeviceMemory,
 * however simplest use may assume use of whole DeviceMemory block for a single
 * resource/
 */
class DeviceMemory
{
public:
  DeviceMemory(DeviceMemoryManager& manager, Graphics& graphics,
               const vk::MemoryRequirements& requirements, vk::MemoryPropertyFlags properties);

  void* Map(uint32_t offset, uint32_t size);

  void Unmap();

  void Bind( Image& image, uint32_t offset );
  void Bind( Buffer& image, uint32_t offset );

private:
  DeviceMemoryManager& mManager;
  Graphics&            mGraphics;

  vk::DeviceMemory        mDeviceMemory;
  vk::MemoryPropertyFlags mProperties;
  vk::MemoryRequirements  mRequirements;

  std::atomic< uint32_t > mUserCount; // this is just a refcount
};

/**
 * DeviceMemoryManager
 */
class DeviceMemoryManager
{
public:
  DeviceMemoryManager() = delete;
  DeviceMemoryManager(Graphics& graphics);
  ~DeviceMemoryManager()                          = default;
  DeviceMemoryManager(const DeviceMemoryManager&) = delete;
  DeviceMemoryManager(DeviceMemoryManager&&)      = default;

  DeviceMemoryManager& operator=(const DeviceMemoryManager&) = delete;
  DeviceMemoryManager& operator=(DeviceMemoryManager&&) = default;

  /* Allocates memory for VkImage */
  vk::DeviceMemory& Allocate(vk::Image image, vk::MemoryPropertyFlags memoryFlags);

  /* Allocates memory for VkBuffer */
  vk::DeviceMemory& Allocate(vk::Buffer image, vk::MemoryPropertyFlags memoryFlags);

  std::unique_ptr< DeviceMemory > Allocate(vk::MemoryRequirements  requirements,
                                           vk::MemoryPropertyFlags flags);

  Graphics& GetGraphics() const
  {
    return mGraphics;
  }

private:
  Graphics& mGraphics;
};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif //VULKAN_PROJECT_DEVICEMEMORYMANAGER_H
