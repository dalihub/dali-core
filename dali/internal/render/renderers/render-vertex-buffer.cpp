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
  mDataChanged(true)
{
}

VertexBuffer::~VertexBuffer() = default;

void VertexBuffer::SetFormat(VertexBuffer::Format* format)
{
  mFormat      = format;
  mDataChanged = true;
}

void VertexBuffer::SetData(Dali::Vector<uint8_t>* data, uint32_t size)
{
  mData        = data;
  mSize        = size;
  mDataChanged = true;
}

bool VertexBuffer::Update(Graphics::Controller& graphicsController)
{
  if(!mData || !mFormat || !mSize)
  {
    return false;
  }

  if(!mGpuBuffer || mDataChanged)
  {
    if(!mGpuBuffer)
    {
      mGpuBuffer = new GpuBuffer(graphicsController, 0 | Graphics::BufferUsage::VERTEX_BUFFER);
    }

    // Update the GpuBuffer
    if(mGpuBuffer)
    {
      DALI_ASSERT_DEBUG(mSize && "No data in the property buffer!");
      mGpuBuffer->UpdateDataBuffer(graphicsController, GetDataSize(), &((*mData)[0]));
    }

    mDataChanged = false;
  }

  return true;
}

} // namespace Render
} // namespace Internal
} // namespace Dali
