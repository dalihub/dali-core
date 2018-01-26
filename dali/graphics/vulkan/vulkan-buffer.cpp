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

#include <dali/graphics/vulkan/vulkan-buffer.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-manager.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-handle.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

struct Buffer::Impl final
{
  /**
   *
   * @param graphics
   * @param createInfo
   */
  Impl(Graphics& graphics, const vk::BufferCreateInfo& createInfo)
  : mGraphics(graphics),
    mDeviceMemory(nullptr),
    mInfo(createInfo)
  {
  }

  /**
   *
   */
  ~Impl()
  {
    mGraphics.GetDevice().destroyBuffer( mBuffer, mGraphics.GetAllocator() );
  }

  /**
   * Initialises Buffer with memory, as host visible ( for write )
   * @return
   */
  bool Initialise()
  {
    mBuffer = VkAssert(mGraphics.GetDevice().createBuffer( mInfo, mGraphics.GetAllocator()));
    return true;
  }

  /**
   *
   * @return
   */
  vk::BufferUsageFlags GetUsage() const
  {
    return mInfo.usage;
  }

  /**
   *
   * @return
   */
  size_t GetSize() const
  {
    return static_cast<size_t>(mInfo.size);
  }

  /**
   *
   * @return
   */
  vk::Buffer GetVkBuffer() const
  {
    return mBuffer;
  }

  void BindMemory( GpuMemoryBlockHandle handle )
  {
    assert( mBuffer && "Buffer not initialised!");
    VkAssert(mGraphics.GetDevice().bindBufferMemory( mBuffer, (*handle), 0 ));
    mDeviceMemory = handle;
  }

  Vulkan::Graphics&                     mGraphics;
  GpuMemoryBlockHandle                  mDeviceMemory;
  vk::BufferCreateInfo                  mInfo;
  vk::Buffer                            mBuffer;
};

/**
 *
 * @param graphics
 * @param size
 * @return
 */
BufferHandle Buffer::New(Graphics& graphics, size_t size, Type type)
{
  auto usageFlags = vk::BufferUsageFlags{};

  switch( type )
  {
    case Type::VERTEX: { usageFlags |= vk::BufferUsageFlagBits::eVertexBuffer; break; };
    case Type::INDEX: { usageFlags |= vk::BufferUsageFlagBits::eIndexBuffer; break; };
    case Type::UNIFORM: { usageFlags |= vk::BufferUsageFlagBits::eUniformBuffer; break; };
    case Type::SHADER_STORAGE: { usageFlags |= vk::BufferUsageFlagBits::eStorageBuffer; break; };
  }

  auto info = vk::BufferCreateInfo{};
  info.setSharingMode( vk::SharingMode::eExclusive );
  info.setSize( size );
  info.setUsage( usageFlags | vk::BufferUsageFlagBits::eTransferDst );
  auto buffer = std::unique_ptr<Buffer>( new Buffer(graphics, info) );

  if(buffer && buffer->mImpl->Initialise())
  {
    auto handle =  BufferHandle(buffer.get());
    graphics.AddBuffer( std::move(buffer) );
    return handle;
  }
  return BufferHandle();
}

Buffer::Buffer(Graphics& graphics, const vk::BufferCreateInfo& createInfo)
: VkManaged()
{
  mImpl = MakeUnique<Buffer::Impl>(graphics, createInfo);
}

Buffer::~Buffer() = default;

vk::BufferUsageFlags Buffer::GetUsage() const
{
  return mImpl->GetUsage();
}

const GpuMemoryBlockHandle& Buffer::GetMemoryHandle() const
{
  return mImpl->mDeviceMemory;
}

size_t Buffer::GetSize() const
{
  return mImpl->GetSize();
}

vk::Buffer Buffer::GetVkBuffer() const
{
  return mImpl->GetVkBuffer();
}

void Buffer::BindMemory( const GpuMemoryBlockHandle& handle )
{
  mImpl->BindMemory( handle );
}

bool Buffer::OnDestroy()
{
  mImpl->mGraphics.RemoveBuffer( *this );
  return true;
}


}

}

}