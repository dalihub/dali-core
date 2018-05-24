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
#include <utility>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

RefCountedBuffer Buffer::New( Graphics& graphics, vk::BufferCreateInfo info )
{
  return RefCountedBuffer(new Buffer(graphics, info));
}

const Buffer& Buffer::ConstRef()
{
  return *this;
}

Buffer& Buffer::Ref()
{
  return *this;
}

Buffer::Buffer( Graphics& graphics, const vk::BufferCreateInfo& createInfo )
        : VkManaged(),
          mGraphics(&graphics),
          mDeviceMemory(nullptr),
          mInfo(createInfo)
{
}

Buffer::~Buffer() = default;

vk::BufferUsageFlags Buffer::GetUsage() const
{
  return mInfo.usage;
}

const RefCountedGpuMemoryBlock& Buffer::GetMemoryHandle() const
{
  return mDeviceMemory;
}

uint32_t Buffer::GetSize() const
{
  return static_cast<uint32_t>(mInfo.size);
}

vk::Buffer Buffer::GetVkHandle() const
{
  return mBuffer;
}

void Buffer::BindMemory( const RefCountedGpuMemoryBlock& handle )
{
  assert(mBuffer && "Buffer not initialised!");
  VkAssert(mGraphics->GetDevice().bindBufferMemory(mBuffer, (*handle), 0));
  mDeviceMemory = handle;
}

bool Buffer::OnDestroy()
{
  mGraphics->RemoveBuffer(*this);

  mGraphics->DiscardResource([this]() {
    mGraphics->GetDevice().destroyBuffer(mBuffer, mGraphics->GetAllocator());
  });

  return false;
}

Buffer::operator vk::Buffer*()
{
  return &mBuffer;
}


}

}

}