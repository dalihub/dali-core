/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
// GPU UBOs need to be double-buffered in order to avoid stalling the CPU during mapping/unmapping
constexpr uint32_t INTERNAL_UBO_BUFFER_COUNT = 2u;

Graphics::UniquePtr<UniformBufferV2> UniformBufferV2::New(Dali::Graphics::Controller* controller, bool emulated, uint32_t alignment)
{
  return Graphics::UniquePtr<UniformBufferV2>(new UniformBufferV2(controller, emulated, alignment));
}

UniformBufferV2::UniformBufferV2(Dali::Graphics::Controller* controller, bool emulated, uint32_t alignment)
: mController(controller),
  mBlockAlignment(alignment),
  mCurrentGraphicsBufferIndex(0),
  mEmulated(emulated)
{
  mBufferList.resize(emulated ? 1 : INTERNAL_UBO_BUFFER_COUNT);
}

void UniformBufferV2::ReSpecify(uint32_t sizeInBytes)
{
  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "Respec(%p) [%d] BufferType:%s  newSize:%d\n", this, mCurrentGraphicsBufferIndex, mEmulated ? "CPU" : "GPU", sizeInBytes);
  if(mEmulated)
  {
    ReSpecifyCPU(sizeInBytes);
  }
  else
  {
    ReSpecifyGPU(sizeInBytes);
  }
}

void UniformBufferV2::Write(const void* data, uint32_t size, uint32_t offset)
{
  // Very verbose logging!
  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::LogLevel(4), "Write(%p) [%d] BufferType:%s  offset:%d size:%d\n", this, mCurrentGraphicsBufferIndex, mEmulated ? "CPU" : "GPU", offset, size);
  if(mEmulated)
  {
    WriteCPU(data, size, offset);
  }
  else
  {
    WriteGPU(data, size, offset);
  }
}

void UniformBufferV2::Map()
{
  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "Map(%p) [%d] BufferType:%s\n", this, mCurrentGraphicsBufferIndex, mEmulated ? "CPU" : "GPU");
  if(mEmulated)
  {
    MapCPU();
  }
  else
  {
    MapGPU();
  }
}

void UniformBufferV2::Unmap()
{
  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "Unmap(%p) [%d] BufferType:%s\n", this, mCurrentGraphicsBufferIndex, mEmulated ? "CPU" : "GPU");
  if(mEmulated)
  {
    UnmapCPU();
  }
  else
  {
    UnmapGPU();
  }
}

void UniformBufferV2::Flush()
{
  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "Flush(%p) [%d] BufferType:%s\n", this, mCurrentGraphicsBufferIndex, mEmulated ? "CPU" : "GPU");

  // Flush only for GPU buffertype by unmapping
  if(!mEmulated && mMappedPtr)
  {
    auto& buffer = mBufferList[mCurrentGraphicsBufferIndex];
    if(buffer.graphicsMemory)
    {
      UnmapGPU();
      // flush range?
    }

    // Swap buffers for GPU UBOs
    auto s                      = mBufferList.size();
    mCurrentGraphicsBufferIndex = ((mCurrentGraphicsBufferIndex + 1) % s);
  }
}

void UniformBufferV2::Rollback()
{
  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "Rollback(%p) [%d]\n", this, mCurrentGraphicsBufferIndex);
  if(!mBufferList.empty())
  {
    mBufferList[mCurrentGraphicsBufferIndex].currentOffset = 0; // reset offset
  }
}

uint32_t UniformBufferV2::AlignSize(uint32_t size)
{
  if(size % mBlockAlignment != 0)
  {
    size = ((size / mBlockAlignment) + 1) * mBlockAlignment;
  }
  return size;
}

uint32_t UniformBufferV2::IncrementOffsetBy(uint32_t value)
{
  if(mEmulated && !mBufferList.empty())
  {
    mBufferList[mCurrentGraphicsBufferIndex].currentOffset += value; // reset offset
    return mBufferList[mCurrentGraphicsBufferIndex].currentOffset;
  } // GPU
  else if(!mBufferList.empty())
  {
    mBufferList[mCurrentGraphicsBufferIndex].currentOffset += value; // reset offset
    return mBufferList[mCurrentGraphicsBufferIndex].currentOffset;
  }
  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::General, "Buffer should be allocated before incrementing offset\n");
  return 0;
}

bool UniformBufferV2::MemoryCompare(void* data, uint32_t offset, uint32_t size)
{
  return !memcmp(data, reinterpret_cast<uint8_t*>(mMappedPtr) + offset, size);
}

uint32_t UniformBufferV2::GetBlockAlignment() const
{
  return mBlockAlignment;
}

uint32_t UniformBufferV2::GetCurrentOffset() const
{
  if(!mBufferList.empty())
  {
    return mBufferList[mCurrentGraphicsBufferIndex].currentOffset;
  }
  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::General, "Buffer should be allocated before getting offset\n");
  return 0;
}

uint32_t UniformBufferV2::GetCurrentCapacity() const
{
  uint32_t capacity = 0;
  if(!mBufferList.empty())
  {
    DALI_ASSERT_DEBUG(mBufferList.size() > mCurrentGraphicsBufferIndex);
    capacity = mBufferList[mCurrentGraphicsBufferIndex].capacity;
  }
  return capacity;
}

Dali::Graphics::Buffer* UniformBufferV2::GetGraphicsBuffer() const
{
  return mBufferList[mCurrentGraphicsBufferIndex].graphicsBuffer.get();
}

void UniformBufferV2::ReSpecifyCPU(uint32_t sizeInBytes)
{
  GfxBuffer gfxBuffer;

  uint32_t currentCapacity = GetCurrentCapacity();

  if(sizeInBytes > currentCapacity)
  {
    Graphics::UniquePtr<Graphics::Buffer> oldBuffer{nullptr};

    // If the CPU buffer already exist use it when applying respec
    if(!mBufferList.empty())
    {
      mBufferList[0].graphicsMemory = nullptr;                                  // Discard mapped memory if exists
      oldBuffer                     = std::move(mBufferList[0].graphicsBuffer); // Store old buffer for re-using
    }
    Graphics::BufferPropertiesFlags flags = 0u | Graphics::BufferPropertiesFlagBit::CPU_ALLOCATED;

    auto createInfo = Graphics::BufferCreateInfo()
                        .SetSize(sizeInBytes)
                        .SetBufferPropertiesFlags(flags)
                        .SetUsage(0u | Graphics::BufferUsage::UNIFORM_BUFFER);

    gfxBuffer.graphicsBuffer = mController->CreateBuffer(createInfo, std::move(oldBuffer));
    gfxBuffer.capacity       = sizeInBytes;
    gfxBuffer.currentOffset  = 0;

    mBufferList[0] = std::move(gfxBuffer);
    // make sure buffer is created (move creation to run in parallel in the backed
    // as this may be a major slowdown)
    mController->WaitIdle();
  }

  mMappedPtr = nullptr;

  if(sizeInBytes)
  {
    // After respecifying the buffer, we can map it persistently as it already exists
    // in the CPU memory
    MapCPU();
  }
}

void UniformBufferV2::ReSpecifyGPU(uint32_t sizeInBytes)
{
  uint32_t currentCapacity = GetCurrentCapacity();

  if(sizeInBytes > currentCapacity)
  {
    GfxBuffer                             gfxBuffer;
    Graphics::UniquePtr<Graphics::Buffer> oldBuffer{nullptr};

    // If the GPU buffer already exist use it when applying respec
    if(!mBufferList.empty())
    {
      mBufferList[mCurrentGraphicsBufferIndex].graphicsMemory = nullptr;                                                            // Discard mapped memory if exists
      oldBuffer                                               = std::move(mBufferList[mCurrentGraphicsBufferIndex].graphicsBuffer); // Store old buffer for re-using
    }
    Graphics::BufferPropertiesFlags flags = 0u;

    auto createInfo = Graphics::BufferCreateInfo()
                        .SetSize(sizeInBytes)
                        .SetBufferPropertiesFlags(flags)
                        .SetUsage(0u | Graphics::BufferUsage::UNIFORM_BUFFER);

    gfxBuffer.graphicsBuffer = mController->CreateBuffer(createInfo, std::move(oldBuffer));
    gfxBuffer.capacity       = sizeInBytes;
    gfxBuffer.currentOffset  = 0;

    mBufferList[mCurrentGraphicsBufferIndex] = std::move(gfxBuffer);
    // make sure buffer is created (move creation to run in parallel in the backend
    // as this may be a major slowdown)
    mController->WaitIdle();
    DALI_ASSERT_ALWAYS(mBufferList[mCurrentGraphicsBufferIndex].capacity == sizeInBytes && "std::move failed");
  }

  mMappedPtr = nullptr;

  if(sizeInBytes)
  {
    MapGPU(); // Note, this will flush the creation buffer queues in the backend and initialize buffer.
  }
}

void UniformBufferV2::WriteCPU(const void* data, uint32_t size, uint32_t offset)
{
  // If not mapped
  if(!mMappedPtr)
  {
    DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "Warning: CPU buffer should already be mapped!\n");
    MapCPU();
  }

  DALI_ASSERT_DEBUG(offset + size <= mBufferList[mCurrentGraphicsBufferIndex].capacity);

  // just copy whatever comes here
  memcpy(reinterpret_cast<uint8_t*>(mMappedPtr) + offset, data, size);
}

void UniformBufferV2::WriteGPU(const void* data, uint32_t size, uint32_t offset)
{
  if(!mMappedPtr)
  {
    DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "Warning: GPU buffer should already be mapped!\n");
    MapGPU();
  }

  DALI_ASSERT_DEBUG(offset + size <= mBufferList[mCurrentGraphicsBufferIndex].capacity);

  memcpy(reinterpret_cast<uint8_t*>(mMappedPtr) + offset, data, size);
}

void UniformBufferV2::MapCPU()
{
  auto& buffer = mBufferList[0]; // CPU contains always one buffer
  if(!buffer.graphicsMemory)
  {
    Graphics::MapBufferInfo info{};
    info.buffer           = buffer.graphicsBuffer.get();
    info.usage            = 0 | Graphics::MemoryUsageFlagBits::WRITE;
    info.offset           = 0;
    info.size             = buffer.capacity;
    buffer.graphicsMemory = mController->MapBufferRange(info);
  }

  // obtain pointer instantly
  if(buffer.graphicsMemory)
  {
    mMappedPtr = buffer.graphicsMemory->LockRegion(0, buffer.capacity);
    DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "CPU buffer is mapped to %p\n", mMappedPtr);
  }
}

void UniformBufferV2::MapGPU()
{
  auto& buffer = mBufferList[mCurrentGraphicsBufferIndex];
  if(!buffer.graphicsMemory)
  {
    Graphics::MapBufferInfo info{};
    info.buffer           = buffer.graphicsBuffer.get();
    info.usage            = 0 | Graphics::MemoryUsageFlagBits::WRITE;
    info.offset           = 0;
    info.size             = buffer.capacity;
    buffer.graphicsMemory = mController->MapBufferRange(info);
  }

  // obtain pointer instantly
  if(buffer.graphicsMemory)
  {
    mMappedPtr = buffer.graphicsMemory->LockRegion(0, buffer.capacity);
    DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "GPU buffer is mapped to %p\n", mMappedPtr);
  }
}

void UniformBufferV2::UnmapCPU()
{
  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "CPU buffer is unmapped\n");
}

void UniformBufferV2::UnmapGPU()
{
  auto& buffer = mBufferList[mCurrentGraphicsBufferIndex];
  if(buffer.graphicsMemory)
  {
    mController->UnmapMemory(std::move(buffer.graphicsMemory));
    buffer.graphicsMemory = nullptr;
  }
  mMappedPtr = nullptr;
  DALI_LOG_INFO(gUniformBufferLogFilter, Debug::Verbose, "GPU buffer is unmapped\n");
}

} // namespace Dali::Internal::Render
