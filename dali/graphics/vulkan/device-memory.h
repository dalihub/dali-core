#ifndef DALI_CORE_GRAPHICS_VULKAN_DEVICE_MEMORY_H
#define DALI_CORE_GRAPHICS_VULKAN_DEVICE_MEMORY_H

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

#include <dali/graphics/vulkan/common.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

class DeviceMemory : public VkHandle
{
public:
  DeviceMemory(VkObject *object = nullptr) : VkHandle(object)
  {
  }

  using VkHandle::operator=;

  /**
   * New DeviceMemory associated with logical device
   * @param device
   * @param allocateInfo
   * @return
   */
  static DeviceMemory New(class LogicalDevice &device, const vk::MemoryAllocateInfo &allocateInfo);

  /**
   * Map memory ( only if host visible )
   * @param offset
   * @param range
   * @return
   */
  void *Map(size_t offset, size_t range);

  /**
   * Unmap previously mapped memory
   */
  void Unmap();

  /**
   * Flush device memory in selected range
   * @param offset
   * @param range
   */
  void Flush(size_t offset, size_t range);

  /**
   * Flush entire block allocated as device memory
   */
  void Flush();

  const class LogicalDevice &GetLogicalDevice() const;
  const vk::DeviceMemory &   GetVkDeviceMemory() const;
  const vk::DeviceMemory &operator*() const;
  size_t GetSize() const;
};
}
}
}

#endif // DALI_CORE_GRAPHICS_VULKAN_DEVICE_MEMORY_H
