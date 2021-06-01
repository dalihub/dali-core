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

// CLASS HEADER
#include <dali/internal/render/renderers/uniform-buffer.h>

namespace Dali::Internal::Render
{
UniformBuffer::UniformBuffer(Dali::Graphics::Controller* controller,
                             uint32_t                    sizeInBytes,
                             uint32_t                    alignment,
                             bool                        persistentMappedEnabled,
                             Graphics::BufferUsageFlags  usageFlags)
  : mController(controller),
    mSize(0u),
    mUsageFlags(usageFlags)
{
  mAlignment = alignment;
  if(sizeInBytes)
  {
    Resize(sizeInBytes, true);
  }
}

UniformBuffer::~UniformBuffer()
{
  // Unmap and flush all allocated buffers
  for( auto i = 0u; i < mBuffers.size(); ++i)
  {
    Flush(i);
    Unmap(i);
  }
}

void UniformBuffer::Flush(uint32_t bufferIndex)
{
  const auto& buffer = mBuffers[bufferIndex];
  if(buffer.buffer && buffer.memory)
  {
    buffer.memory->Flush();
  }
}

void UniformBuffer::Resize( uint32_t newSize, bool invalidate )
{
  // The buffer is already optimal
  if(newSize == mSize && !invalidate)
  {
    return;
  }
  if(invalidate && newSize == mSize && mBuffers.size() == 1)
  {
    return;
  }

  // Throw away content
  if(invalidate)
  {
    mBuffers.clear();
    mSize = 0;
  }

  // Adjust alignment, the alignment is needed for
  // real UBOs (it should be given by the buffer requirements)
  if(mAlignment)
  {
    newSize = ((newSize / mAlignment)+1)*mAlignment;
  }

  if(newSize > mSize)
  {
    auto createInfo = Graphics::BufferCreateInfo()
      .SetSize(newSize - mSize)
      .SetBufferPropertiesFlags(0 | Graphics::BufferPropertiesFlagBit::CPU_ALLOCATED)
      .SetUsage(mUsageFlags);

    auto buffer = mController->CreateBuffer(createInfo, nullptr);

    mBuffers.emplace_back(GfxBuffer(std::move(buffer), createInfo));

    mSize = newSize;
  }
}


const UniformBuffer::GfxBuffer* UniformBuffer::GetBufferByOffset( uint32_t offset, uint32_t* newOffset, uint32_t* outBufferIndex ) const
{
  uint32_t bufferOffset = offset;
  uint32_t bufferIndex = 0u;

  // Find buffer if UBO is fragmented
  if(mBuffers.size() > 1)
  {
    for(const auto& buffer : mBuffers)
    {
      if( bufferOffset >= buffer.createInfo.size)
      {
        bufferOffset -= buffer.createInfo.size;
      }
      else
      {
        break;
      }
      bufferIndex++;
    }
  }

  auto& bufferDesc = mBuffers[bufferIndex];

  if(outBufferIndex)
  {
    *outBufferIndex = bufferIndex;
  }

  if(newOffset)
  {
    *newOffset = bufferOffset;
  }

  return &bufferDesc;
}

void UniformBuffer::Write(const void* data, uint32_t size, uint32_t dstOffset)
{
  // find which buffer we want to write into
  uint32_t bufferOffset = dstOffset;
  uint32_t bufferIndex = 0u;

  // Find buffer if UBO is fragmented
  if(mBuffers.size() > 1)
  {
    for(const auto& buffer : mBuffers)
    {
      if( bufferOffset >= buffer.createInfo.size)
      {
        bufferOffset -= buffer.createInfo.size;
      }
      else
      {
        break;
      }
      bufferIndex++;
    }
  }

  auto& bufferDesc = mBuffers[bufferIndex];

  if(bufferDesc.needsUpdate)
  {
    mController->WaitIdle();
    bufferDesc.needsUpdate = false;
  }

  DALI_ASSERT_ALWAYS( mBuffers.size() > bufferIndex );
  DALI_ASSERT_ALWAYS( mBuffers[bufferIndex].buffer );
  DALI_ASSERT_ALWAYS( mBuffers[bufferIndex].createInfo.size > bufferOffset + size );

  bool locallyMapped = (bufferDesc.mappedPtr != nullptr);
  if(!locallyMapped)
  {
    // Map once and keep it
    Map( bufferIndex );
  }

  if(bufferDesc.memory)
  {
    void* ptr = bufferDesc.memory->LockRegion(bufferOffset, size);
    if(ptr && bufferOffset + size < mSize)
    {
      memcpy(ptr, data, size);
    }
    bufferDesc.memory->Unlock(true);
  }
}

void UniformBuffer::Map( uint32_t bufferIndex)
{
  auto& buffer = mBuffers[bufferIndex];

  if(buffer.needsUpdate)
  {
    mController->WaitIdle();
    buffer.needsUpdate = false;
  }

  if(!buffer.memory)
  {
    Graphics::MapBufferInfo info{};
    info.buffer = buffer.buffer.get();
    info.usage  = 0 | Graphics::MemoryUsageFlagBits::WRITE;
    info.offset = 0;
    info.size   = buffer.createInfo.size;
    buffer.memory = mController->MapBufferRange(info);
  }
}

void UniformBuffer::Unmap( uint32_t bufferIndex)
{
  auto& buffer = mBuffers[bufferIndex];
  if(buffer.memory)
  {
    mController->UnmapMemory(std::move(buffer.memory));
  }
}
}