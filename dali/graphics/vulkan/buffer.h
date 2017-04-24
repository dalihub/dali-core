#ifndef DALI_CORE_GRAPHICS_VULKAN_BUFFER_H
#define DALI_CORE_GRAPHICS_VULKAN_BUFFER_H

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

class Buffer : public VkHandle
{
public:
  Buffer(VkObject* impl = nullptr) : VkHandle(impl){}
  using VkHandle::operator=;

  static Buffer New( const class LogicalDevice& device, const vk::BufferCreateInfo& info );

  const vk::Buffer& GetVkResource() const;
  const vk::Buffer& operator*() const;

  bool BindDeviceMemory( const class DeviceMemory& memory );

  bool BindDeviceMemory( const class DeviceMemory& memory, size_t offset );



};

} // namespace Vulkan

} // namespace Graphics

} // namespace Dali

#endif // DALI_CORE_GRAPHICS_VULKAN_BUFFER_H
