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

#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-handle.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-allocator.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

void GpuMemoryBlock::OnRelease( uint32_t refcount )
{
  mAllocator.Release( *this );
}

void GpuMemoryBlock::OnRetain( uint32_t refcount )
{
  mAllocator.Retain( *this );
}

bool GpuMemoryBlock::OnDestroy()
{
  // don't use default delete
  return true;
}

GpuMemoryBlock::operator vk::DeviceMemory()
{
  return mAllocator.GetVkDeviceMemory( *this );
}

void* GpuMemoryBlock::Map()
{
  return mAllocator.Map( *this, 0, static_cast<uint32_t>(VK_WHOLE_SIZE) );
}

void* GpuMemoryBlock::Map( uint32_t offset, uint32_t size )
{
  return mAllocator.Map( *this, offset, size );
}

void GpuMemoryBlock::Unmap()
{
  mAllocator.Unmap( *this );
}

void GpuMemoryBlock::Flush()
{
  NotImplemented();
}

}

}

}