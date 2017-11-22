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
 * DeviceMemory represents continuous memory block with particular properties
 * like being mappable etc. The actual memory is being allocated from DeviceMemory,
 * however simplest use may assume use of whole DeviceMemory block for a single
 * resource/
 */
class DeviceMemory
{
public:

  DeviceMemory(DeviceMemoryManager& manager,
               Graphics& graphics,
               const vk::MemoryRequirements& requirements,
               vk::MemoryPropertyFlags properties);

  void* Map(uint32_t offset, uint32_t size);

  void Unmap();

  void Bind( Image& image, uint32_t offset );
  void Bind( Buffer& buffer, uint32_t offset );

private:

  std::unique_ptr<Impl::DeviceMemory> mImpl;
};

/**
 * DeviceMemoryManager
 */
namespace Impl
{
class DeviceMemoryManager;
}
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
  vk::DeviceMemory& Allocate(Vulkan::Image& image, vk::MemoryPropertyFlags memoryFlags);

  /* Allocates memory for VkBuffer */
  const Vulkan::DeviceMemory& Allocate(Vulkan::Buffer& buffer, vk::MemoryPropertyFlags memoryFlags);

  std::unique_ptr< DeviceMemory > Allocate(vk::MemoryRequirements  requirements,
                                           vk::MemoryPropertyFlags flags);

  Graphics& GetGraphics() const;

private:

  std::unique_ptr<Impl::DeviceMemoryManager> mImpl;


};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif //VULKAN_PROJECT_DEVICEMEMORYMANAGER_H
