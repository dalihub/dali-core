/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
 */

// CLASS HEADER
#include <dali/internal/render/renderers/render-geometry.h>

// INTERNAL INCLUDES
#include <dali/internal/render/renderers/render-vertex-buffer.h>
#include <dali/internal/render/shaders/program.h>

namespace Dali::Internal::Render
{
namespace
{
inline constexpr size_t GetSizeOfIndexFromIndexType(Dali::Graphics::Format graphicsFormat)
{
  switch(graphicsFormat)
  {
    case Dali::Graphics::Format::R16_UINT:
    {
      return sizeof(uint16_t);
    }
    case Dali::Graphics::Format::R32_UINT:
    {
      return sizeof(uint32_t);
    }
    default:
    {
      // TODO : Not implemented.
      return sizeof(uint16_t);
    }
  }
}
} // unnamed namespace
Geometry::Geometry()
: mLifecycleObservers(),
  mIndices(),
  mIndexBuffer(nullptr),
  mIndexType(Dali::Graphics::Format::R16_UINT),
  mGeometryType(Dali::Geometry::TRIANGLES),
  mIndicesChanged(false),
  mHasBeenUploaded(false),
  mUpdated(true),
  mObserverNotifying(false)
{
}

Geometry::~Geometry()
{
  mObserverNotifying = true;
  for(auto&& iter : mLifecycleObservers)
  {
    auto* observer = iter.first;
    observer->GeometryDestroyed(this);
  }
  mLifecycleObservers.clear();

  // Note: We don't need to restore mObserverNotifying to false as we are deleting the object.
  // If someone calls AddObserver / RemoveObserver after this, assert.
}

void Geometry::AddVertexBuffer(Render::VertexBuffer* vertexBuffer)
{
  mVertexBuffers.PushBack(vertexBuffer);
  mUpdated = true;
}

const Vector<Render::VertexBuffer*>& Geometry::GetVertexBuffers() const
{
  return mVertexBuffers;
}

void Geometry::SetIndexBuffer(Uint16ContainerType& indices)
{
  mIndices.Swap(indices);
  mIndicesChanged = true;
  mUpdated        = true;
  mIndexType      = Graphics::Format::R16_UINT;
}

void Geometry::SetIndexBuffer(Uint32ContainerType& indices)
{
  // mIndices type is not matched with indices. Copy memory hardly.
  mIndices.ResizeUninitialized(indices.Count() * 2);
  memcpy(mIndices.Begin(), indices.Begin(), indices.Count() * sizeof(uint32_t));
  mIndicesChanged = true;
  mUpdated        = true;
  mIndexType      = Graphics::Format::R32_UINT;
}

void Geometry::RemoveVertexBuffer(const Render::VertexBuffer* vertexBuffer)
{
  const auto&& end = mVertexBuffers.End();

  for(auto&& iter = mVertexBuffers.Begin(); iter != end; ++iter)
  {
    if(*iter == vertexBuffer)
    {
      // This will delete the gpu buffer associated to the RenderVertexBuffer if there is one
      mVertexBuffers.Remove(iter);
      mUpdated = true;
      break;
    }
  }
}

void Geometry::OnRenderFinished()
{
  mHasBeenUploaded = false;
  mUpdated         = false;
}

void Geometry::Upload(Graphics::Controller& graphicsController)
{
  if(!mHasBeenUploaded)
  {
    // Update buffers
    if(mIndicesChanged)
    {
      if(mIndices.Empty())
      {
        mIndexBuffer = nullptr;
      }
      else
      {
        if(mIndexBuffer == nullptr)
        {
          // Currently we are unable to reuse the index buffer so the write policy is to preserve current content
          mIndexBuffer = new GpuBuffer(graphicsController, 0 | Graphics::BufferUsage::INDEX_BUFFER, GpuBuffer::WritePolicy::RETAIN);
        }

        auto bufferSize = static_cast<uint32_t>(sizeof(uint16_t) * mIndices.Size());
        mIndexBuffer->UpdateDataBuffer(graphicsController, bufferSize, &mIndices[0]);
      }

      mIndicesChanged = false;
    }

    // Make geometry buffer changed as true if Type changed.
    bool geometryBufferChanged = mUpdated;
    for(auto&& buffer : mVertexBuffers)
    {
      geometryBufferChanged = geometryBufferChanged || buffer->IsDataChanged();
      mUpdated              = mUpdated || geometryBufferChanged || buffer->IsRedrawRequired();
      if(!buffer->Update(graphicsController))
      {
        // Vertex buffer is not ready ( Size, data or format has not been specified yet )
        return;
      }
    }

    mHasBeenUploaded = true;

    // Notify to observers that geometry information has changed
    if(geometryBufferChanged)
    {
      mUpdated = true;

      mObserverNotifying = true;
      for(auto&& iter : mLifecycleObservers)
      {
        auto* observer = iter.first;
        observer->GeometryBufferChanged(this);
      }
      mLifecycleObservers.clear();
      mObserverNotifying = false;
    }
  }
}

bool Geometry::BindVertexAttributes(Graphics::CommandBuffer& commandBuffer)
{
  // Bind buffers to attribute locations
  const auto vertexBufferCount = static_cast<uint32_t>(mVertexBuffers.Count());
  if(DALI_UNLIKELY(vertexBufferCount == 0u))
  {
    return false;
  }

  std::vector<const Graphics::Buffer*> buffers;
  std::vector<uint32_t>                offsets;

  buffers.resize(vertexBufferCount);
  offsets.resize(vertexBufferCount);

  auto buffersIter = buffers.begin();
  auto offsetsIter = offsets.begin();
  for(auto& vertexBuffer : mVertexBuffers)
  {
    const GpuBuffer* gpuBuffer = vertexBuffer->GetGpuBuffer();
    if(DALI_LIKELY(gpuBuffer))
    {
      const Graphics::Buffer* buffer = gpuBuffer->GetGraphicsObject();

      if(DALI_LIKELY(buffer))
      {
        *(buffersIter++) = buffer;
        *(offsetsIter++) = 0u;
      }
      else
      {
        break;
      }
    }
    else
    {
      break;
    }
    //@todo Figure out why this is being drawn without geometry having been uploaded
  }
  if(DALI_UNLIKELY(buffersIter != buffers.end()))
  {
    return false;
  }

  commandBuffer.BindVertexBuffers(0, buffers, offsets);

  return true;
}

bool Geometry::Draw(
  Graphics::Controller&    graphicsController,
  Graphics::CommandBuffer& commandBuffer,
  uint32_t                 elementBufferOffset,
  uint32_t                 elementBufferCount,
  uint32_t                 instanceCount)
{
  uint32_t numIndices(0u);
  intptr_t firstIndexOffset(0u);
  if(mIndexBuffer)
  {
    std::size_t sizeOfIndex = GetSizeOfIndexFromIndexType(mIndexType);

    numIndices = static_cast<uint32_t>(mIndices.Size() * sizeof(uint16_t) / sizeOfIndex);

    if(elementBufferOffset != 0u)
    {
      elementBufferOffset = (elementBufferOffset >= numIndices) ? numIndices : elementBufferOffset;
      firstIndexOffset    = intptr_t(elementBufferOffset * sizeOfIndex);
      numIndices -= elementBufferOffset;
    }

    if(elementBufferCount != 0u)
    {
      numIndices = std::min(elementBufferCount, numIndices);
    }
  }

  // Draw call
  if(mIndexBuffer && mGeometryType != Dali::Geometry::POINTS)
  {
    // Issue draw call only if there's non-zero numIndices
    if(numIndices)
    {
      // Indexed draw call
      const Graphics::Buffer* ibo = mIndexBuffer->GetGraphicsObject();
      if(ibo)
      {
        commandBuffer.BindIndexBuffer(*ibo, 0, mIndexType);
      }

      commandBuffer.DrawIndexed(numIndices, instanceCount, firstIndexOffset, 0, 0);
    }
  }
  else
  {
    // Un-indexed draw call
    uint32_t numVertices(0u);
    uint32_t firstVertex(0u);

    // Use element buffer count for drawing arrays (needs changing API, for workaround)
    if(elementBufferCount)
    {
      numVertices = elementBufferCount;
      firstVertex = elementBufferOffset;
    }
    // In case we have more buffers, we select buffer with less elements to render
    // TODO: we may eventually support wrapping around buffers????
    else if(mVertexBuffers.Count() > 1)
    {
      auto elementsCount = mVertexBuffers[0]->GetRenderableElementCount();
      for(auto& vertexBuffer : mVertexBuffers)
      {
        elementsCount = std::min(elementsCount, vertexBuffer->GetRenderableElementCount());
      }
      numVertices = elementsCount;
    }
    else if(mVertexBuffers.Count() == 1)
    {
      // truncated, no value loss happening in practice
      numVertices = static_cast<uint32_t>(mVertexBuffers[0]->GetRenderableElementCount());
    }

    // Issue draw call only if there's non-zero numVertices
    if(numVertices)
    {
      commandBuffer.Draw(numVertices, instanceCount, firstVertex, 0);
    }
  }
  return true;
}

Graphics::PrimitiveTopology Geometry::GetTopology() const
{
  Graphics::PrimitiveTopology topology = Graphics::PrimitiveTopology::TRIANGLE_LIST;

  switch(mGeometryType)
  {
    case Dali::Geometry::TRIANGLES:
    {
      topology = Graphics::PrimitiveTopology::TRIANGLE_LIST;
      break;
    }
    case Dali::Geometry::LINES:
    {
      topology = Graphics::PrimitiveTopology::LINE_LIST;
      break;
    }
    case Dali::Geometry::POINTS:
    {
      topology = Graphics::PrimitiveTopology::POINT_LIST;
      break;
    }
    case Dali::Geometry::TRIANGLE_STRIP:
    {
      topology = Graphics::PrimitiveTopology::TRIANGLE_STRIP;
      break;
    }
    case Dali::Geometry::TRIANGLE_FAN:
    {
      topology = Graphics::PrimitiveTopology::TRIANGLE_FAN;
      break;
    }
    case Dali::Geometry::LINE_LOOP:
    {
      topology = Graphics::PrimitiveTopology::LINE_LOOP;
      break;
    }
    case Dali::Geometry::LINE_STRIP:
    {
      topology = Graphics::PrimitiveTopology::LINE_STRIP;
      break;
    }
  }
  return topology;
}

} // namespace Dali::Internal::Render
