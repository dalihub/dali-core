/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

#ifdef DEBUG_ENABLED
Debug::Filter* gUniformBufferLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_UNIFORM_BUFFER");
#endif

namespace Dali::Internal::Render
{
namespace
{
static constexpr uint32_t INVALID_BUFFER_INDEX = std::numeric_limits<uint32_t>::max();
}
UniformBuffer::UniformBuffer(Dali::Graphics::Controller*     controller,
                             uint32_t                        sizeInBytes,
                             uint32_t                        alignment,
                             Graphics::BufferUsageFlags      usageFlags,
                             Graphics::BufferPropertiesFlags propertiesFlags)
: mController(controller),
  mSize(0u),
  mUsageFlags(usageFlags),
  mPropertiesFlags(propertiesFlags),
  mLockedBufferIndex(INVALID_BUFFER_INDEX),
  mLockedPtr(nullptr),
  mReadyToBeLocked(false)
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
  for(auto i = 0u; i < mBuffers.size(); ++i)
  {
    Flush(i);
    Unmap(i);
  }
}

void UniformBuffer::Flush(uint32_t bufferIndex)
{
  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::General, "Flush (bufferIndex : %d / %d [%d])\n", bufferIndex, mBuffers.size(), mSize);
  const auto& buffer = mBuffers[bufferIndex];
  if(buffer.buffer && buffer.memory)
  {
    buffer.memory->Flush();
  }
}

void UniformBuffer::Resize(uint32_t newSize, bool invalidate)
{
  // Adjust alignment, the alignment is needed for
  // real UBOs (it should be given by the buffer requirements)
  if(DALI_LIKELY(mAlignment && newSize > 0))
  {
    newSize = (((newSize - 1) / mAlignment) + 1) * mAlignment;
  }

  // The buffer is already optimal
  if(newSize == mSize && !invalidate)
  {
    return;
  }
  if(invalidate && newSize == mSize && mBuffers.size() == 1)
  {
    return;
  }

  if(DALI_UNLIKELY(mReadyToBeLocked))
  {
    DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "Resize %d --> %d with %s [mBuffers : %d] during lock (lockedBufferIndex : %d, lockedPtr : %p)\n", mSize, newSize, invalidate ? "Invalidate" : "Rendering", mBuffers.size(), mLockedBufferIndex, mLockedPtr);
  }
  else
  {
    DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "Resize %d --> %d with %s [mBuffers : %d]\n", mSize, newSize, invalidate ? "Invalidate" : "Rendering", mBuffers.size());
  }

  // Throw away content
  if(invalidate)
  {
    if(mReadyToBeLocked)
    {
      UnlockUniformBuffer();
      mReadyToBeLocked = true;
    }
    // Flush and unmap all allocated buffers
    for(auto i = 0u; i < mBuffers.size(); ++i)
    {
      Flush(i);
      Unmap(i);
    }
    mBuffers.clear();
    mSize = 0;
  }

  if(newSize > mSize)
  {
    auto createInfo = Graphics::BufferCreateInfo()
                        .SetSize(newSize - mSize)
                        .SetBufferPropertiesFlags(mPropertiesFlags)
                        .SetUsage(mUsageFlags);

    auto buffer = mController->CreateBuffer(createInfo, nullptr);

    mBuffers.emplace_back(GfxBuffer(std::move(buffer), createInfo));

    mSize = newSize;
  }

  // If invalidate during locked, begin lock again.
  if(DALI_UNLIKELY(invalidate && mReadyToBeLocked))
  {
    mReadyToBeLocked = false;
    ReadyToLockUniformBuffer();
  }

  if(DALI_UNLIKELY(mReadyToBeLocked))
  {
    DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "Resize done as %d with %s [mBuffers : %d] during lock (lockedBufferIndex : %d, lockedPtr : %p)\n", newSize, invalidate ? "Invalidate" : "Rendering", mBuffers.size(), mLockedBufferIndex, mLockedPtr);
  }
  else
  {
    DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "Resize done as %d with %s [mBuffers : %d]\n", newSize, invalidate ? "Invalidate" : "Rendering", mBuffers.size());
  }
}

const UniformBuffer::GfxBuffer* UniformBuffer::GetBufferByOffset(uint32_t offset, uint32_t* newOffset, uint32_t* outBufferIndex) const
{
  uint32_t bufferOffset = offset;
  uint32_t bufferIndex  = 0u;

  // Find buffer if UBO is fragmented
  if(mBuffers.size() > 1)
  {
    for(const auto& buffer : mBuffers)
    {
      if(bufferOffset >= buffer.createInfo.size)
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

  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "GetBufferByOffset (bufferIndex : %d / %d [%d], bufferOffset : %d, Graphics::BufferPtr : %p)\n", bufferIndex, mBuffers.size(), mSize, bufferOffset, bufferDesc.buffer.get());

  return &bufferDesc;
}

void UniformBuffer::Write(const void* data, uint32_t size, uint32_t dstOffset)
{
  // find which buffer we want to write into
  uint32_t bufferOffset = dstOffset;
  uint32_t bufferIndex  = 0u;

  // Find buffer if UBO is fragmented
  if(mBuffers.size() > 1)
  {
    for(const auto& buffer : mBuffers)
    {
      if(bufferOffset >= buffer.createInfo.size)
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

  DALI_ASSERT_ALWAYS(mBuffers.size() > bufferIndex);
  DALI_ASSERT_ALWAYS(mBuffers[bufferIndex].buffer);
  DALI_ASSERT_ALWAYS(mBuffers[bufferIndex].createInfo.size > bufferOffset + size);

  const bool locallyMapped = (bufferDesc.mappedPtr != nullptr);
  if(!locallyMapped)
  {
    // Map once and keep it
    Map(bufferIndex);
  }

  if(bufferDesc.memory)
  {
    // Rarely happened that we use over the locked memory
    // Unlock previous buffer, and lock as current bufferIndex again
    if(DALI_UNLIKELY(mLockedBufferIndex != bufferIndex))
    {
      DALI_LOG_INFO(gUniformBufferLogFilter, Debug::General, "Unlock (lockedBufferIndex : %d / %d [%d], lockedPtr : %p)\n", mLockedBufferIndex, mBuffers.size(), mSize, mLockedPtr);

      // mLockedBufferIndex == INVALID_BUFFER_INDEX only first time of current RenderScene.
      if(DALI_LIKELY(mLockedBufferIndex != INVALID_BUFFER_INDEX))
      {
        // Unlock previous memory
        if(mBuffers[mLockedBufferIndex].memory)
        {
          mBuffers[mLockedBufferIndex].memory->Unlock(true);
        }
      }
      mLockedBufferIndex = bufferIndex;
      mLockedPtr         = nullptr;

      // Initial mapping done previously. Just lock and roll now.
      if(mBuffers[mLockedBufferIndex].memory)
      {
        mLockedPtr = reinterpret_cast<uint8_t*>(mBuffers[mLockedBufferIndex].memory->LockRegion(0, mBuffers[mLockedBufferIndex].createInfo.size));
      }
      DALI_LOG_INFO(gUniformBufferLogFilter, Debug::General, "Lock (lockedBufferIndex : %d / %d [%d], lockedPtr : %p)\n", mLockedBufferIndex, mBuffers.size(), mSize, mLockedPtr);
    }

    DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "memcpy (lockedBufferIndex : %d / %d [%d], lockedPtr : %p, offset : %d, size : %d, lockedBufferSize : %d)\n", mLockedBufferIndex, mBuffers.size(), mSize, mLockedPtr, bufferOffset, size, mBuffers[mLockedBufferIndex].createInfo.size);

    // We already check validation of buffer range. We can assume that bufferOffset + size <= mBuffers[mLockedBufferIndex].createInfo.size
    if(mLockedPtr)
    {
      memcpy(mLockedPtr + bufferOffset, data, size);
    }
  }
}

void UniformBuffer::Map(uint32_t bufferIndex)
{
  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::General, "Map (bufferIndex : %d / %d [%d])\n", bufferIndex, mBuffers.size(), mSize);
  auto& buffer = mBuffers[bufferIndex];

  if(buffer.needsUpdate)
  {
    mController->WaitIdle();
    buffer.needsUpdate = false;
  }

  if(!buffer.memory)
  {
    Graphics::MapBufferInfo info{};
    info.buffer   = buffer.buffer.get();
    info.usage    = 0 | Graphics::MemoryUsageFlagBits::WRITE;
    info.offset   = 0;
    info.size     = buffer.createInfo.size;
    buffer.memory = mController->MapBufferRange(info);
    DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "GraphicsMemoryMapped (bufferIndex : %d / %d [%d], size : %d)\n", bufferIndex, mBuffers.size(), mSize, info.size);
  }
}

void UniformBuffer::Unmap(uint32_t bufferIndex)
{
  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::General, "Unmap (bufferIndex : %d / %d [%d])\n", bufferIndex, mBuffers.size(), mSize);
  auto& buffer = mBuffers[bufferIndex];
  if(buffer.memory)
  {
    mController->UnmapMemory(std::move(buffer.memory));
  }
}

void UniformBuffer::ReadyToLockUniformBuffer()
{
  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::General, "LockUniformBuffer\n");
  if(DALI_UNLIKELY(mReadyToBeLocked && mLockedBufferIndex != INVALID_BUFFER_INDEX))
  {
    // Unlock previous locked buffer first
    DALI_LOG_ERROR("Warning! : called LockUniformBuffer() before called UnlockUniformBuffer()!\n");
    UnlockUniformBuffer();
  }

  mReadyToBeLocked   = true;
  mLockedBufferIndex = INVALID_BUFFER_INDEX;
  mLockedPtr         = nullptr;
}

void UniformBuffer::UnlockUniformBuffer()
{
  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::General, "UnlockUniformBuffer (lockedBufferIndex : %d / %d [%d], lockedPtr : %p)\n", mLockedBufferIndex, mBuffers.size(), mSize, mLockedPtr);
  if(mReadyToBeLocked && mLockedBufferIndex != INVALID_BUFFER_INDEX)
  {
    auto& bufferDesc = mBuffers[mLockedBufferIndex];
    if(bufferDesc.memory)
    {
      bufferDesc.memory->Unlock(true);
    }
    // Flush all allocated buffers
    for(auto i = 0u; i < mBuffers.size(); ++i)
    {
      Flush(i);
    }
  }
  mLockedPtr         = nullptr;
  mLockedBufferIndex = INVALID_BUFFER_INDEX;
  mReadyToBeLocked   = false;
}

} // namespace Dali::Internal::Render