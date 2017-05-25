#ifndef DALI_CORE_GRAPHICS_VULKAN_IMAGE_H
#define DALI_CORE_GRAPHICS_VULKAN_IMAGE_H

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

class LogicalDevice;
class DeviceMemory;

class Image : public VkHandle
{
public:

  Image(VkObject* impl = nullptr ) : VkHandle(impl){}
  using VkHandle::operator=;

  static Image New( const LogicalDevice& device, const vk::ImageCreateInfo& createInfo );

  const vk::Image& GetVkResource() const;
  const vk::Image& operator*() const;

  vk::ImageLayout GetLayout() const;
  void SetLayout( vk::ImageLayout layout );
  bool BindDeviceMemory( const class DeviceMemory& memory );
  bool BindDeviceMemory( const class DeviceMemory& memory, size_t offset );

  vk::ImageMemoryBarrier GetLayoutChangeBarrier( vk::ImageLayout newLayout, vk::AccessFlags srcAccess, vk::AccessFlags dstAccess, vk::ImageAspectFlags imageAspect ) const;

private:

};


} // Vulkan
} // Graphics
} // Dali


#endif // DALI_CORE_GRAPHICS_VULKAN_IMAGE_H
