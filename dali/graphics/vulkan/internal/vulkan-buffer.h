#ifndef DALI_GRAPHICS_VULKAN_BUFFER_H
#define DALI_GRAPHICS_VULKAN_BUFFER_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include <dali/graphics/vulkan/internal/vulkan-types.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

class Memory;

class Buffer : public VkManaged
{
  friend class Graphics;

public:

  const Buffer& ConstRef();

  Buffer& Ref();

  /**
   * Returns buffer usage flags
   * @return
   */
  vk::BufferUsageFlags GetUsage() const;

  /**
   * Returns buffer size in bytes
   * @return
   */
  uint32_t GetSize() const;

  /**
   * Returns Vulkan VkBuffer object associated with this buffer
   * @return
   */
  vk::Buffer GetVkHandle() const;

  /**
   * Returns handle to the allocated memory
   * @return
   */
  Memory* GetMemory() const;

  /**
   * Destroys underlying Vulkan resources on the caller thread.
   *
   * @note Calling this function is unsafe and makes any further use of
   * buffer invalid.
   */
  void DestroyNow();

  /**
   *
   * @return
   */
  bool OnDestroy() override;

  Buffer( const Buffer& ) = delete;

  Buffer& operator=( const Buffer& ) = delete;

private:

  /**
   * Creates new VkBuffer with given specification, it doesn't
   * bind the memory.
   * @param graphics
   * @param createInfo
   */
  Buffer( Graphics& graphics, const vk::BufferCreateInfo& createInfo );

  /**
   * Destroys used Vulkan resource objects
   * @param device Vulkan device
   * @param buffer Vulkan buffer
   * @param memory Vulkan device memory
   * @param allocator Pointer to the Vulkan allocator callbacks
   */
  static void DestroyVulkanResources( vk::Device device, vk::Buffer buffer, vk::DeviceMemory memory, const vk::AllocationCallbacks* allocator );

private:
  Graphics* mGraphics;
  std::unique_ptr<Memory> mDeviceMemory;
  vk::BufferCreateInfo mInfo;
  vk::Buffer mBuffer;
};


}
}
}

#endif //VULKAN_PROJECT_BUFFER_H
