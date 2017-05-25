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
#include <dali/graphics/vulkan/device-memory.h>
#include <dali/graphics/vulkan/logical-device.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

class DeviceMemoryImpl : public VkObject
{
public:
  DeviceMemoryImpl(const LogicalDevice& device, const vk::MemoryAllocateInfo& allocateInfo);

  bool Initialise();

  virtual bool OnSafeDelete();

  const LogicalDevice& GetLogicalDevice() const
  {
    return mDevice;
  }

  const vk::DeviceMemory& GetVkDeviceMemory() const
  {
    return mMemory;
  }

  size_t GetSize() const
  {
    return mSize;
  }

  void* Map(size_t offset, size_t range);

  void Unmap();

  void Flush(size_t offset, size_t range);

private:
  LogicalDevice          mDevice{nullptr};
  vk::MemoryAllocateInfo mAllocateInfo;
  vk::DeviceMemory       mMemory{nullptr};
  size_t                 mSize{0u};
  void*                  mMappedPtr{nullptr};
};

// Implementation

DeviceMemoryImpl::DeviceMemoryImpl(const LogicalDevice& device, const vk::MemoryAllocateInfo& allocateInfo)
: mDevice(device), mAllocateInfo(allocateInfo), mMemory(nullptr), mSize(0u)
{
}

bool DeviceMemoryImpl::Initialise()
{
  auto vkDevice    = mDevice.GetVkDevice();
  auto vkAllocator = mDevice.GetVkAllocator();

  VkAssertCall(vkDevice.allocateMemory(&mAllocateInfo, vkAllocator, &mMemory));

  mSize = mAllocateInfo.allocationSize;
  return true;
}

bool DeviceMemoryImpl::OnSafeDelete()
{
  if(mMemory)
  {
    mDevice.GetVkDevice().freeMemory(mMemory, mDevice.GetVkAllocator());
    return true;
  }
  return false;
}

void* DeviceMemoryImpl::Map(size_t offset, size_t range)
{
  assert(mMappedPtr == nullptr && "Device memory must not be previously mapped!");
  void *ptr = nullptr;
  if (VkTestCall(mDevice.GetVkDevice()
                        .mapMemory(mMemory, offset, range, vk::MemoryMapFlags(), &ptr)) !=
      vk::Result::eSuccess)
  {
    return nullptr;
  }
  mMappedPtr = ptr;
  return ptr;
}

void DeviceMemoryImpl::Unmap()
{
  assert(mMappedPtr != nullptr && "Device memory must be mapped!");
  mDevice.GetVkDevice().unmapMemory(mMemory);
  mMappedPtr = nullptr;
}

void DeviceMemoryImpl::Flush(size_t offset, size_t size)
{
  vk::MappedMemoryRange range;
  range.setOffset(offset).setMemory(mMemory).setSize(size);
  mDevice.GetVkDevice().flushMappedMemoryRanges(1, &range);
}

// Handle

namespace
{
DeviceMemoryImpl* GetImpl(DeviceMemory* handle)
{
  return static_cast< DeviceMemoryImpl* >(handle->GetObject());
}

DeviceMemoryImpl* GetImpl(const DeviceMemory* handle)
{
  return static_cast< DeviceMemoryImpl* >(handle->GetObject());
}
}

DeviceMemory DeviceMemory::New(class LogicalDevice& device, const vk::MemoryAllocateInfo& allocateInfo)
{
  auto impl = new DeviceMemoryImpl(device, allocateInfo);
  if(impl->Initialise())
  {
    return impl;
  }
  return nullptr;
}

const LogicalDevice& DeviceMemory::GetLogicalDevice() const
{
  return GetImpl(this)->GetLogicalDevice();
}

const vk::DeviceMemory& DeviceMemory::GetVkDeviceMemory() const
{
  return GetImpl(this)->GetVkDeviceMemory();
}

const vk::DeviceMemory& DeviceMemory::operator*() const
{
  return GetImpl(this)->GetVkDeviceMemory();
}

size_t DeviceMemory::GetSize() const
{
  return GetImpl(this)->GetSize();
}

void* DeviceMemory::Map(size_t offset, size_t range)
{
  return GetImpl(this)->Map(offset, range);
}

void DeviceMemory::Unmap()
{
  GetImpl(this)->Unmap();
}

void DeviceMemory::Flush(size_t offset, size_t range)
{
  GetImpl(this)->Flush(offset, range);
}

void DeviceMemory::Flush()
{
  GetImpl(this)->Flush(0, VK_WHOLE_SIZE);
}

} // Vulkan
} // Graphics
} // Dali
