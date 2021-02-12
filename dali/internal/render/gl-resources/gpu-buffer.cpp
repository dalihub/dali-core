/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/internal/render/gl-resources/gpu-buffer.h>

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-types.h>
#include <dali/public-api/common/dali-common.h>
#include <cstring>

namespace Dali
{
namespace Internal
{
namespace
{
} // namespace

GpuBuffer::GpuBuffer(Graphics::Controller& graphicsController, Graphics::BufferUsageFlags usage)
: mUsage(usage),
  mBufferCreated(false)
{
}

GpuBuffer::~GpuBuffer()
{
}

void GpuBuffer::UpdateDataBuffer(Graphics::Controller& graphicsController, uint32_t size, const void* data)
{
  DALI_ASSERT_DEBUG(size > 0);
  mSize = size;

  if(!mGraphicsObject || size > mCapacity)
  {
    Graphics::BufferCreateInfo createInfo{};
    createInfo.SetUsage(mUsage).SetSize(size);
    mGraphicsObject = graphicsController.CreateBuffer(createInfo, std::move(mGraphicsObject));
    mCapacity       = size;
  }

  Graphics::MapBufferInfo info;
  info.buffer = mGraphicsObject.get();
  info.usage  = 0 | Graphics::MemoryUsageFlagBits::WRITE;
  info.offset = 0;
  info.size   = size;

  auto  memory = graphicsController.MapBufferRange(info);
  void* ptr    = memory->LockRegion(0, size);
  memcpy(ptr, data, size);
  memory->Unlock(true);
  graphicsController.UnmapMemory(std::move(memory));
}

bool GpuBuffer::BufferIsValid() const
{
  return mGraphicsObject && (0 != mCapacity);
}

void GpuBuffer::Destroy()
{
  mCapacity = 0;
  mSize     = 0;
  mGraphicsObject.reset();
}

} // namespace Internal

} //namespace Dali
