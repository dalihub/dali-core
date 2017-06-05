#include <dali/graphics/vulkan/buffer.h>
#include <dali/graphics/vulkan/device-memory-manager.h>
#include <dali/graphics/vulkan/graphics.h>
#include <dali/graphics/vulkan/image.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

namespace
{

const uint32_t INVALID_MEMORY_INDEX = -1u;

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

/*
 * DeviceMemory
 */

DeviceMemory::DeviceMemory(DeviceMemoryManager &manager, Graphics &graphics,
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

void *DeviceMemory::Map(uint32_t offset, uint32_t size)
{
}

void DeviceMemory::Unmap()
{
}

void DeviceMemory::Bind(Image &image, uint32_t offset)
{
  assert(image.GetImage() && "Image is nullptr!");
  VkAssert(mGraphics.GetDevice().bindImageMemory(image.GetImage(), mDeviceMemory, offset));
  ++mUserCount;
}

void DeviceMemory::Bind(Buffer &image, uint32_t offset)
{
}

/*
 * DeviceMemoryManager
 */

DeviceMemoryManager::DeviceMemoryManager(Graphics &graphics) : mGraphics(graphics)
{
}

std::unique_ptr< DeviceMemory > DeviceMemoryManager::Allocate(vk::MemoryRequirements  requirements,
                                                              vk::MemoryPropertyFlags flags)
{
  return MakeUnique< DeviceMemory >(*this, mGraphics, requirements, flags);
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
