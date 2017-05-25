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

#include <dali/graphics/vulkan/buffer.h>
#include <dali/graphics/vulkan/device-memory.h>
#include <dali/graphics/vulkan/logical-device.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
// Implementation

class BufferImpl : public VkObject
{
public:
  BufferImpl(const LogicalDevice& device, const vk::BufferCreateInfo& info)
  : mDevice(device), mCreateInfo(info)
  {
  }

  bool Initialise();

  const vk::Buffer& GetVkBuffer() const
  {
    return mBuffer;
  }

  const DeviceMemory& GetDeviceMemory() const
  {
    return mDeviceMemory;
  }

  bool BindDeviceMemory( const DeviceMemory& memory, size_t offset );

private:
  LogicalDevice        mDevice{nullptr};
  DeviceMemory         mDeviceMemory{nullptr}; // bound device memory
  vk::BufferCreateInfo mCreateInfo{};
  vk::Buffer           mBuffer{nullptr};
  size_t               mMemoryBoundOffset{0u};
};

bool BufferImpl::Initialise()
{
  return VkTestBool(mDevice.GetVkDevice().createBuffer(&mCreateInfo, mDevice.GetVkAllocator(), &mBuffer));
}

bool BufferImpl::BindDeviceMemory( const DeviceMemory& memory, size_t offset )
{
  assert( !mDeviceMemory && "There is a DeviceMemory already bound!");
  if( VkTestBool( mDevice->bindBufferMemory( mBuffer, *memory, offset ) ))
  {
    mDeviceMemory = memory;
    mMemoryBoundOffset = offset;
    return true;
  }
  return false;
}

// Handle
namespace
{
BufferImpl* GetImpl(Buffer* handle)
{
  return static_cast< BufferImpl* >(handle->GetObject());
}
BufferImpl* GetImpl(const Buffer* handle)
{
  return static_cast< BufferImpl* >(handle->GetObject());
}
}

Buffer Buffer::New(const LogicalDevice& device, const vk::BufferCreateInfo& info)
{
  auto impl = new BufferImpl(device, info);
  if(impl->Initialise())
    return impl;
  return nullptr;
}

const vk::Buffer& Buffer::GetVkResource() const
{
  return GetImpl(this)->GetVkBuffer();
}

const vk::Buffer& Buffer::operator*() const
{
  return GetImpl(this)->GetVkBuffer();
}

bool Buffer::BindDeviceMemory( const DeviceMemory& memory )
{
  return GetImpl(this)->BindDeviceMemory( memory, 0 );
}

bool Buffer::BindDeviceMemory( const DeviceMemory& memory, size_t offset )
{
  return GetImpl(this)->BindDeviceMemory( memory, offset );
}


}
}
}