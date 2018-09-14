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

#include <dali/graphics/vulkan/api/vulkan-api-buffer.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/internal/vulkan-buffer.h>
#include <dali/graphics/vulkan/internal/vulkan-gpu-memory-allocator.h>
#include <dali/graphics/vulkan/internal/vulkan-gpu-memory-manager.h>
#include <dali/graphics/vulkan/api/vulkan-api-controller.h>

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{

Buffer::Buffer( Controller& controller,
                vk::BufferUsageFlags usage,
                API::Buffer::UsageHint usageHints,
                uint32_t size )
        : mController( controller ),
          mGraphics( controller.GetGraphics() ),
          mUsage( usage ),
          mUsageHints( usageHints ),
          mSize( size )
{

}

bool Buffer::Initialise()
{
  // create buffer
  mBufferRef = mGraphics.CreateBuffer( mSize, mUsage );

  // allocate memory
  // todo: host visible should be only for dynamic buffers
  auto memory = mGraphics.AllocateMemory( mBufferRef, vk::MemoryPropertyFlagBits::eHostVisible );
  mGraphics.BindBufferMemory( mBufferRef, memory, 0 );

  return true;
}

void* Buffer::Map()
{
  return mBufferRef->GetMemoryHandle()->Map();
}

void Buffer::Unmap()
{
  mBufferRef->GetMemoryHandle()->Unmap();
}

void Buffer::Flush()
{
  mBufferRef->GetMemoryHandle()->Flush();
}

void Buffer::Write( void* src, uint32_t srcSize, uint32_t dstOffset )
{
  // depends whether the buffer is host visible or device local
  // TODO: implement in-GPU copying, for now all buffers are host-visible
  auto transfer = std::make_unique< VulkanAPI::BufferMemoryTransfer >();
  transfer->srcPtr = new char[srcSize];
  memcpy( transfer->srcPtr, src, srcSize );
  transfer->dstBuffer = mBufferRef;
  transfer->dstOffset = dstOffset;
  transfer->srcSize = srcSize;
  mController.ScheduleBufferMemoryTransfer( std::move( transfer ) );
}

Vulkan::RefCountedBuffer Buffer::GetBufferRef() const
{
  return mBufferRef;
}

}
}
}
