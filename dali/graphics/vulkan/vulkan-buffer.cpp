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
namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
namespace Impl
{

struct Buffer final
{
  Graphics&                             mGraphics;
  vk::BufferCreateInfo                  mInfo;
  vk::Buffer                            mBuffer;
  bool                                  mHasDeviceMemory;

  Buffer(Graphics& graphics, const vk::BufferCreateInfo& createInfo)
  : mGraphics(graphics),
    mHasDeviceMemory(false)
  {
    mBuffer = VkAssert(graphics.GetDevice().createBuffer( createInfo, graphics.GetAllocator()));
    mInfo = createInfo;
  }

  ~Buffer()
  {
    mGraphics.GetDevice().destroyBuffer( mBuffer, mGraphics.GetAllocator() );
  }

  vk::BufferUsageFlags GetUsage() const
  {
    return mInfo.usage;
  }

  DeviceMemory& GetDeviceMemory() const
  {

  }

  size_t GetSize() const
  {
    return static_cast<size_t>(mInfo.size);
  }

  bool HasDeviceMemory() const
  {
    return mHasDeviceMemory;
  }

  vk::Buffer GetVkObject() const
  {
    return mBuffer;
  }

};
}

/**
 *
 * @param graphics
 * @param size
 * @return
 */
std::unique_ptr<VertexBuffer> Buffer::NewVertexBuffer(Graphics& graphics, size_t size)
{
  vk::BufferCreateInfo info = {};
  info.setSharingMode( vk::SharingMode::eExclusive );
  info.setSize( size );
  info.setUsage( vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst );
  return MakeUnique<VertexBuffer>( graphics, info );
}

/**
 *
 * @param graphics
 * @param size
 * @return
 */
std::unique_ptr<UniformBuffer> Buffer::NewUniformBuffer(Graphics& graphics, size_t size)
{
  vk::BufferCreateInfo info = {};
  info.setSharingMode( vk::SharingMode::eExclusive );
  info.setSize( size );
  info.setUsage( vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst );
  return MakeUnique<VertexBuffer>( graphics, info );
}

/**
 *
 * @param graphics
 * @param size
 * @return
 */
std::unique_ptr<IndexBuffer> Buffer::NewIndexBuffer(Graphics& graphics, size_t size)
{
  vk::BufferCreateInfo info = {};
  info.setSharingMode( vk::SharingMode::eExclusive );
  info.setSize( size );
  info.setUsage( vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst );
  return MakeUnique<VertexBuffer>( graphics, info );
}

Buffer::Buffer(Graphics& graphics, const vk::BufferCreateInfo& createInfo)
{
  mImpl = MakeUnique<Impl::Buffer>(graphics, createInfo);
}

Buffer::~Buffer() = default;

vk::BufferUsageFlags Buffer::GetUsage() const
{
  return mImpl->GetUsage();
}

DeviceMemory& Buffer::GetDeviceMemory() const
{
  return mImpl->GetDeviceMemory();
}

size_t Buffer::GetSize() const
{
  return mImpl->GetSize();
}

bool Buffer::HasDeviceMemory() const
{
  return mImpl->HasDeviceMemory();
}

vk::Buffer Buffer::GetVkObject() const
{
  return mImpl->GetVkObject();
}


}

}

}