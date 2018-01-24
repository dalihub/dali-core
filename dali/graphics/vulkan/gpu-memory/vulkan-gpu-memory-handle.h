#ifndef DALI_VULKAN_161117_GPU_MEMORY_HANDLE_H
#define DALI_VULKAN_161117_GPU_MEMORY_HANDLE_H

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

#include <dali/graphics/vulkan/vulkan-types.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
using GpuMemoryAllocationId = uint32_t;
class GpuMemoryAllocator;
const uint32_t INVALID_ALLOCATION { UINT32_MAX };

class GpuMemoryHandle final
{
public:

  GpuMemoryHandle( GpuMemoryAllocator* allocator = nullptr, GpuMemoryAllocationId allocationId = INVALID_ALLOCATION);

  GpuMemoryHandle( const GpuMemoryHandle& handle);
  GpuMemoryHandle& operator=( const GpuMemoryHandle& handle );
  GpuMemoryHandle& operator=( GpuMemoryHandle&& handle );
  GpuMemoryHandle( GpuMemoryHandle&& handle ) noexcept;
  ~GpuMemoryHandle();

  operator bool() const;
  operator vk::DeviceMemory() const;

  void* Map();

  void* Map( uint32_t offset, uint32_t size );

  void Unmap();

  void Flush();

  GpuMemoryAllocator& GetAllocator() const;

  uint32_t GetRefCount() const;

  vk::DeviceMemory GetVkDeviceMemory() const;

private:

  GpuMemoryAllocator*              mAllocator { nullptr };
  GpuMemoryAllocationId            mAllocationId { INVALID_ALLOCATION };
};
}
}
}

#endif //DALI_VULKAN_161117_GPU_MEMORY_HANDLE_H
