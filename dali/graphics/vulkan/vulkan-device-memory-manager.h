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
namespace Impl
{
class DeviceMemory;
}
class Graphics;
class Image;
class Buffer;
class DeviceMemoryManager;

/**
 * Notes:
 * 1. All allocated memory is owned by the DeviceMemoryManager
 * 2. Each memory block is refcounted
 * 3. Memory which is mapped to the client space increases refcount
 * 4. Memory bound to an Image or Buffer increses refcount
 * 5. Unused memory is placed in the discard queue
 * 6. Flushing unused memory may be postponed
 */

/**
 * DeviceMemory represents continuous memory block with particular properties
 * like being mappable etc. The actual memory is being allocated from DeviceMemory,
 * however simplest use may assume use of whole DeviceMemory block for a single
 * resource/
 */
class DeviceMemory
{
public:

  DeviceMemory();
  DeviceMemory(Graphics& graphics,
               const vk::MemoryRequirements& requirements,
               vk::MemoryPropertyFlags properties);

  ~DeviceMemory();

  void* Map(uint32_t offset, uint32_t size);

  void* Map();

  void Unmap();

  vk::DeviceMemory GetVkDeviceMemory() const;

  operator bool() const;

private:

  struct Impl;
  std::unique_ptr<Impl> mImpl;
};

/**
 * DeviceMemoryManager
 */
class DeviceMemoryManager final
{
public:
  DeviceMemoryManager() = delete;
  DeviceMemoryManager(Graphics& graphics);
  ~DeviceMemoryManager();
  DeviceMemoryManager(const DeviceMemoryManager&) = delete;
  DeviceMemoryManager(DeviceMemoryManager&&)      = default;

  DeviceMemoryManager& operator=(const DeviceMemoryManager&) = delete;
  DeviceMemoryManager& operator=(DeviceMemoryManager&&) = default;

  /* Allocates memory for VkImage */
  vk::DeviceMemory& Allocate(Vulkan::Image& image, vk::MemoryPropertyFlags memoryFlags);

  /* Allocates memory for VkBuffer */
  const Vulkan::DeviceMemory& Allocate(Vulkan::Buffer& buffer, vk::MemoryPropertyFlags memoryFlags);

  std::unique_ptr< DeviceMemory > Allocate(vk::MemoryRequirements  requirements,
                                           vk::MemoryPropertyFlags flags);

  Graphics& GetGraphics() const;

private:

  class Impl;
  std::unique_ptr<Impl> mImpl;

};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif //VULKAN_PROJECT_DEVICEMEMORYMANAGER_H
