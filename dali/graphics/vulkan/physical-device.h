#ifndef DALI_CORE_GRAPHICS_VULKAN_PHYSICAL_DEVICE_H
#define DALI_CORE_GRAPHICS_VULKAN_PHYSICAL_DEVICE_H

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

#include <dali/graphics/graphics-physical-device.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class PhysicalDevice : public GraphicsPhysicalDevice
{
public:
  PhysicalDevice(Integration::GraphicsPhysicalDeviceBase* impl = nullptr)
  : GraphicsPhysicalDevice{impl}
  {
  }

  using GraphicsPhysicalDevice::operator=;

  // must not be virtual! ( for now )
  static GraphicsPhysicalDevice New();

  uint32_t GetQueueFamilyIndex(QueueType type) const;

public:
  // getters of bound Vulkan objects
  vk::Instance                              GetInstance() const;
  vk::PhysicalDevice                        GetPhysicalDevice() const;
  const vk::AllocationCallbacks*            GetAllocator() const;
  const vk::PhysicalDeviceFeatures&         GetFeatures() const;
  const vk::PhysicalDeviceProperties&       GetProperties() const;
  const vk::PhysicalDeviceLimits&           GetLimits() const;
  const vk::PhysicalDeviceMemoryProperties& GetMemoryProperties() const;
  const std::string&                        GetName() const;
  const std::string&                        GetVendorName() const;
};
}
}
}

#endif //DALI_CORE_GRAPHICS_VULKAN_PHYSICAL_DEVICE_H
