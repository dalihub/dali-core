#ifndef DALI_VULKAN_161117_GPU_MEMORY_ALLOCATOR_H
#define DALI_VULKAN_161117_GPU_MEMORY_ALLOCATOR_H

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
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-handle.h>
namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class Buffer;
class Image;
class GpuMemoryBlock;
/**
 * Interface to Gpu memory allocation, must be overriden.
 */
using GpuMemoryAllocationData = void*;

class GpuMemoryAllocator
{
public:

  GpuMemoryAllocator() = default;

  /**
   *
   */
  virtual ~GpuMemoryAllocator() = default;

  /**
   *
   * @param requirements
   * @param memoryProperties
   * @return
   */
  virtual GpuMemoryBlockHandle Allocate( const vk::MemoryRequirements& requirements, vk::MemoryPropertyFlags memoryProperties ) = 0;

  /**
   *
   * @param buffer
   * @param memoryProperties
   * @return
   */
  virtual GpuMemoryBlockHandle Allocate( Buffer& buffer, vk::MemoryPropertyFlags memoryProperties ) = 0;

  /**
   *
   * @param buffer
   * @param memoryProperties
   * @return
   */
  virtual GpuMemoryBlockHandle Allocate( Image& buffer, vk::MemoryPropertyFlags memoryProperties ) = 0;

  // refcounting managed via allocator which ownes all the blocks, allocator may
  // implement this feature any way ( or simply ignore it )
  /**
   *
   * @param allocationId
   */
  virtual void Retain( GpuMemoryBlock& allocationId ) {}

  /**
   *
   * @param allocationId
   */
  virtual void Release( GpuMemoryBlock& allocationId ) {}

  /**
   *
   * @param allocationId
   */
  virtual uint32_t GetRefCount( GpuMemoryBlock& allocationId )
  {
    return 0u;
  }

  // Garbage collection
  /**
   *
   * @param userdata
   */
  virtual void GC( void* userdata ) = 0;

  // Retrieve Vulkan object for this allocation
  /**
   *
   * @return
   */
  virtual vk::DeviceMemory GetVkDeviceMemory( GpuMemoryBlock& allocationId ) const = 0;

  /**
   * Memory mapping
   * @param allocationId
   * @param offset
   * @param size
   * @return
   */
  virtual void* Map( GpuMemoryBlock& allocationId, uint32_t offset, uint32_t size ) = 0;

  /**
   * Memory mapping
   * @param allocationId
   * @param offset
   * @param size
   * @return
   */
  virtual void Unmap( GpuMemoryBlock& allocationId ) = 0;

  /**
   *
   * @param allocationId
   */
  virtual void Flush( GpuMemoryBlock& allocationId ) = 0;
};

} // Namespace Vulkan

} // namespace Graphics

} // Namespace Dali

#endif //DALI_VULKAN_161117_GPU_MEMORY_ALLOCATOR_H
