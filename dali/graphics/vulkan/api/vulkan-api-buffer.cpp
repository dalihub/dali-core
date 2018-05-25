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
#include <dali/graphics/vulkan/vulkan-buffer.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-allocator.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-manager.h>
#include <dali/graphics/vulkan/api/vulkan-api-controller.h>

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{

Buffer::Buffer( Controller& controller, vk::BufferUsageFlagBits usage, API::Buffer::UsageHint usageHints, uint32_t size )
: mController( controller ), mGraphics( controller.GetGraphics() ), mUsageHints( usageHints ), mSize( size )
{

}

bool Buffer::Initialise()
{
  auto type = Vulkan::Buffer::Type{};
  if( mUsage == vk::BufferUsageFlagBits::eUniformBuffer )
  {
    type = Vulkan::Buffer::Type::UNIFORM;
  }
  else if( mUsage == vk::BufferUsageFlagBits::eIndexBuffer )
  {
    type = Vulkan::Buffer::Type::INDEX;
  }
  else if( mUsage == vk::BufferUsageFlagBits::eVertexBuffer )
  {
    type = Vulkan::Buffer::Type::VERTEX;
  }
  else
  {
    // unsupported usage
    return false;
  }

  // create buffer
  mBufferRef = Vulkan::Buffer::New( mGraphics, mSize, type );

  // allocate memory
  auto memory =
    mGraphics.GetDeviceMemoryManager().GetDefaultAllocator().Allocate( mBufferRef,
                      vk::MemoryPropertyFlagBits::eHostVisible ); // todo: host visible should be only for dynamic buffers
  mBufferRef->BindMemory( memory );

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

void Buffer::Write( void* src, uint32_t srcSize, uint32_t dstOffset )
{
  // depends whether the buffer is host visible or device local
  // TODO: implement in-GPU copying, for now all buffers are host-visible
  auto transfer = std::make_unique<VulkanAPI::BufferMemoryTransfer>();
  auto tmp = new char[srcSize];
  memcpy( tmp, src, srcSize );
  transfer->srcPtr.reset( tmp );
  transfer->dstBuffer = mBufferRef;
  transfer->dstOffset = dstOffset;
  transfer->srcSize = srcSize;
  mController.ScheduleBufferMemoryTransfer( std::move(transfer) );
}

Vulkan::BufferRef Buffer::GetBufferRef() const
{
  return mBufferRef;
}

}
}
}
