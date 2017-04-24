#ifndef DALI_CORE_GRAPHICS_VULKAN_LOGICAL_DEVICE_H
#define DALI_CORE_GRAPHICS_VULKAN_LOGICAL_DEVICE_H

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

#include <dali/graphics/graphics-logical-device.h>
#include <dali/graphics/vulkan/device-memory.h>
#include <dali/graphics/vulkan/command-pool.h>
#include <dali/graphics/vulkan/image.h>
#include <dali/graphics/vulkan/buffer.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class PhysicalDevice;
class CommandQueue;

class LogicalDevice : public GraphicsLogicalDevice
{
public:
  LogicalDevice( GraphicsLogicalDeviceBase* impl = nullptr ) : GraphicsLogicalDevice( impl )
  {
  }

  using GraphicsLogicalDevice::operator=;

  bool Initialise();

  const vk::Device GetVkDevice() const;

  const vk::AllocationCallbacks* GetVkAllocator() const;

  const PhysicalDevice& GetPhysicalDevice() const;

  static LogicalDevice New( const PhysicalDevice& physicalDevice );

  const vk::Device* operator->() const;

  /**
   *
   * @param image
   * @param flags
   * @param doBind
   * @return
   */
  DeviceMemory AllocateImageMemory(const Image& image, vk::MemoryPropertyFlags flags, bool doBind) const;

  /**
   *
   * @param buffer
   * @param flags
   * @param doBind
   * @return
   */
  DeviceMemory AllocateBufferMemory(const Buffer& buffer, vk::MemoryPropertyFlags flags, bool doBind) const;

  /**
   * Creates new command pool with given settings
   * @param type
   * @param createTransient
   * @param createResetCommandBuffer
   * @return
   */
  CommandPool CreateCommandPool( QueueType type, bool createTransient, bool createResetCommandBuffer) const;

  /**
   * Returns one of the pre-allocated command queues or nullptr
   * @param index
   * @param type
   * @return
   */
  const CommandQueue& GetCommandQueue( uint32_t index, QueueType type ) const;


  // Resources

  /*
   * IMAGE
   */

  /**
   * Creates Vulkan image object
   * @return
   */
  Image CreateImage2D( uint32_t width, uint32_t height, vk::Format pixelFormat, bool hostVisible, bool bindMemory );

  /**
   * Creates Vulkan Image from given VkImageCreateInfo structure
   * @param imageInfo
   * @return
   */
  Image CreateImage( vk::ImageCreateInfo imageInfo );


};


} // Vulkan
} // Graphics
} // Dali



#endif //DALI_CORE_GRAPHICS_VULKAN_LOGICAL_DEVICE_H
