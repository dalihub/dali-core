/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/internal/render/renderers/render-vertex-buffer.h>

// INTERNAL HEADERS
#include <dali/internal/event/rendering/vertex-buffer-impl.h> // Dali::Internal::VertexBuffer

namespace Dali
{
namespace Internal
{
namespace Render
{
VertexBuffer::VertexBuffer()
: mFormat(nullptr),
  mData(nullptr),
  mGpuBuffer(nullptr),
  mSize(0),
  mElementCount(0),
  mDataChanged(true)
{
}

VertexBuffer::~VertexBuffer() = default;

void VertexBuffer::SetFormat(OwnerPointer<Render::VertexBuffer::Format>& format)
{
  mFormat      = format.Release();
  mDataChanged = true;
}

void VertexBuffer::SetData(OwnerPointer<Dali::Vector<uint8_t>>& data, uint32_t size)
{
  mData        = data.Release();
  mSize        = size;
  mDataChanged = true;
}

void VertexBuffer::SetVertexBufferUpdateCallback(Dali::VertexBufferUpdateCallback* callback)
{
  mVertexBufferStateLock.ChangeState(VertexBufferSyncState::UNLOCKED, VertexBufferSyncState::LOCKED_FOR_EVENT);
  mVertexBufferUpdateCallback.reset(callback);
  mVertexBufferStateLock.ChangeState(VertexBufferSyncState::LOCKED_FOR_EVENT, VertexBufferSyncState::UNLOCKED);
}

bool VertexBuffer::Update(Graphics::Controller& graphicsController)
{
  if(!mFormat || !mSize)
  {
    return false;
  }

  if(!mVertexBufferUpdateCallback && (!mData || mData->Empty()))
  {
    return false;
  }

  if(!mGpuBuffer || mDataChanged)
  {
    if(!mGpuBuffer)
    {
      mGpuBuffer = new GpuBuffer(graphicsController, 0 | Graphics::BufferUsage::VERTEX_BUFFER, GpuBuffer::WritePolicy::DISCARD);
    }

    // Update the GpuBuffer
    if(mGpuBuffer && mData && !mData->Empty())
    {
      DALI_ASSERT_DEBUG(mSize && "No data in the property buffer!");
      mGpuBuffer->UpdateDataBuffer(graphicsController, GetDataSize(), &((*mData)[0]));
    }

    mElementCount = mSize;

    mDataChanged = false;
  }

  // To execute the callback the buffer must be already initialized.
  mVertexBufferStateLock.ChangeState(VertexBufferSyncState::UNLOCKED, VertexBufferSyncState::LOCKED_FOR_UPDATE);
  if(mVertexBufferUpdateCallback && mGpuBuffer)
  {
    // If running callback, we may end up with less elements in the buffer
    // of the same capacity
    uint32_t updatedSize = mSize * mFormat->size;
    mGpuBuffer->UpdateDataBufferWithCallback(graphicsController, mVertexBufferUpdateCallback.get(), updatedSize);
    mElementCount = updatedSize / mFormat->size;
  }
  mVertexBufferStateLock.ChangeState(VertexBufferSyncState::LOCKED_FOR_UPDATE, VertexBufferSyncState::UNLOCKED);
  return true;
}

void VertexBuffer::SetDivisor(uint32_t divisor)
{
  mDivisor     = divisor;
  mDataChanged = true;
}

uint32_t VertexBuffer::GetDivisor()
{
  return mDivisor;
}

} // namespace Render
} // namespace Internal
} // namespace Dali
