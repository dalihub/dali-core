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

GpuMemoryHandle::GpuMemoryHandle( GpuMemoryAllocator* allocator, GpuMemoryAllocationId allocationId )
: mAllocator( allocator ), mAllocationId( allocationId )
{
  if(mAllocator && mAllocationId != INVALID_ALLOCATION)
  {
    mAllocator->Retain( mAllocationId );
  }
}

GpuMemoryHandle::GpuMemoryHandle( const GpuMemoryHandle& handle) :
mAllocator(handle.mAllocator), mAllocationId(handle.mAllocationId)
{
  if(mAllocator)
  {
    mAllocator->Retain(handle.mAllocationId);
  }
}

GpuMemoryHandle::GpuMemoryHandle( GpuMemoryHandle&& handle ) noexcept
: mAllocator( handle.mAllocator ),
  mAllocationId( handle.mAllocationId )
{
  handle.mAllocator = nullptr;
  handle.mAllocationId = INVALID_ALLOCATION;
}

GpuMemoryHandle::operator bool() const
{
  return (mAllocator != nullptr && mAllocationId != INVALID_ALLOCATION);
}

GpuMemoryHandle::operator vk::DeviceMemory() const
{
  return GetVkDeviceMemory();
}

GpuMemoryHandle& GpuMemoryHandle::operator=( GpuMemoryHandle&& handle )
{
  mAllocator = handle.mAllocator;
  mAllocationId = handle.mAllocationId;
  handle.mAllocator = nullptr;
  handle.mAllocationId = INVALID_ALLOCATION;
}

GpuMemoryHandle& GpuMemoryHandle::operator=( const GpuMemoryHandle& handle )
{
  if(handle.mAllocator != nullptr && mAllocationId != INVALID_ALLOCATION)
  {
    mAllocator->Release(mAllocationId);
  }
  (mAllocator = handle.mAllocator)->Retain( mAllocationId = handle.mAllocationId );
  return *this;
}

GpuMemoryHandle::~GpuMemoryHandle()
{
  if(mAllocator != nullptr && mAllocationId != INVALID_ALLOCATION)
  {
    mAllocator->Release(mAllocationId);
  }
}

uint32_t GpuMemoryHandle::GetRefCount() const
{
  assert( (mAllocator != nullptr && mAllocationId != INVALID_ALLOCATION) && "Invalid allocation!" );
  return mAllocator->GetRefCount( mAllocationId );
}

vk::DeviceMemory GpuMemoryHandle::GetVkDeviceMemory() const
{
  assert( (mAllocator != nullptr && mAllocationId != INVALID_ALLOCATION) && "Invalid allocation!" );
  return mAllocator->GetVkDeviceMemory( mAllocationId );
}

void* GpuMemoryHandle::Map()
{
  assert( (mAllocator != nullptr && mAllocationId != INVALID_ALLOCATION) && "Invalid allocation!" );
  return mAllocator->Map( mAllocationId, 0, static_cast<uint32_t>(VK_WHOLE_SIZE) );
}

void* GpuMemoryHandle::Map( uint32_t offset, uint32_t size )
{
  assert( (mAllocator != nullptr && mAllocationId != INVALID_ALLOCATION) && "Invalid allocation!" );
  return mAllocator->Map( mAllocationId, offset, size );
}

void GpuMemoryHandle::Unmap()
{
  assert( (mAllocator != nullptr && mAllocationId != INVALID_ALLOCATION) && "Invalid allocation!" );
  return mAllocator->Unmap( mAllocationId );
}

void GpuMemoryHandle::Flush()
{
  assert( (mAllocator != nullptr && mAllocationId != INVALID_ALLOCATION) && "Invalid allocation!" );
  mAllocator->Flush( mAllocationId );
}

GpuMemoryAllocator& GpuMemoryHandle::GetAllocator() const
{
  assert( (mAllocator != nullptr && mAllocationId != INVALID_ALLOCATION) && "Invalid allocation!" );
  return *mAllocator;
}

}

}

}