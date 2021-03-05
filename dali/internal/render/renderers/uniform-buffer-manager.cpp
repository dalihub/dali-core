/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

#include "uniform-buffer-manager.h"
#include <dali/graphics-api/graphics-buffer-create-info.h>
#include <dali/graphics-api/graphics-buffer.h>

#include <cstring>
#include <memory>

namespace Dali
{
namespace Internal
{
namespace Render
{
UniformBuffer::UniformBuffer(Dali::Graphics::Controller* controller,
                             uint32_t                    sizeInBytes,
                             uint32_t                    alignment,
                             bool                        persistentMappedEnabled,
                             Graphics::BufferUsageFlags  usageFlags)
: mController(controller),
  mSize(0u),
  mPersistentMappedEnabled(persistentMappedEnabled),
  mUsageFlags(usageFlags)
{
  if(sizeInBytes)
  {
    Reserve(sizeInBytes);
  }
}

UniformBuffer::~UniformBuffer()
{
  if(mBuffer && mMemory)
  {
    mController->UnmapMemory(std::move(mMemory));
  }
}

void UniformBuffer::Flush()
{
  if(mBuffer && mMemory)
  {
    mMemory->Flush();
  }
}

void UniformBuffer::Reserve(uint32_t size)
{
  if(mBuffer && mMemory)
  {
    Unmap();
    mMemory = nullptr;
  }

  mSize = size;

  auto createInfo = Graphics::BufferCreateInfo()
    .SetSize(mSize)
    .SetBufferPropertiesFlags( 0 | Graphics::BufferPropertiesFlagBit::CPU_ALLOCATED )
    .SetUsage(mUsageFlags);

  mBuffer = mController->CreateBuffer(createInfo, std::move(mBuffer));

  mMapBufferInfo.buffer = mBuffer.get();
  mMapBufferInfo.usage  = 0 | Graphics::MemoryUsageFlagBits::WRITE;
  mMapBufferInfo.offset = 0;
  mMapBufferInfo.size   = size;

  if(mPersistentMappedEnabled)
  {
    Map();
  }
}

void UniformBuffer::Fill(char data, uint32_t offset, uint32_t size)
{
  bool locallyMapped = (mMemory == nullptr);
  if(locallyMapped)
  {
    Map();
  }

  if(mMemory)
  {
    void* ptr = mMemory->LockRegion(0, mSize);

    auto begin = (reinterpret_cast<char*>(ptr) + offset);
    if(size == 0)
    {
      size = mSize - offset - 1;
    }
    auto end = begin + size;
    std::fill(begin, end, data);

    mMemory->Unlock(true);
  }

  if(locallyMapped)
  {
    Unmap();
  }
}

void UniformBuffer::Write(const void* data, uint32_t size, uint32_t dstOffset)
{
  bool locallyMapped = (mMemory == nullptr);
  if(locallyMapped)
  {
    Map();
  }

  if(mMemory)
  {
    void* ptr = mMemory->LockRegion(0, size);
    if(dstOffset + size < mSize)
    {
      memcpy(reinterpret_cast<char*>(ptr) + dstOffset, data, size);
    }
    mMemory->Unlock(true);
  }

  if(locallyMapped)
  {
    Unmap();
  }
}

void UniformBuffer::Map()
{
  if(!mMemory)
  {
    mMemory = mController->MapBufferRange(mMapBufferInfo);
  }
}

void UniformBuffer::Unmap()
{
  if(mMemory)
  {
    mController->UnmapMemory(std::move(mMemory));
  }
}

UniformBufferManager::UniformBufferManager(Dali::Graphics::Controller* controller)
: mController(controller)
{
}

UniformBufferManager::~UniformBufferManager() = default;

Graphics::UniquePtr<UniformBuffer> UniformBufferManager::AllocateUniformBuffer(uint32_t size)
{
  return Graphics::UniquePtr<UniformBuffer>(
    new UniformBuffer(mController, size, 256u, true, Dali::Graphics::BufferUsageFlags{0u} | Dali::Graphics::BufferUsage::TRANSFER_DST | Dali::Graphics::BufferUsage::UNIFORM_BUFFER));
}

} // namespace Render

} // namespace Internal

} // namespace Dali
