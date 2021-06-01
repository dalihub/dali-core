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
 */

// CLASS HEADER
#include <dali/internal/render/renderers/render-geometry.h>

// INTERNAL INCLUDES
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/render/renderers/render-vertex-buffer.h>
#include <dali/internal/render/shaders/program.h>

namespace Dali
{
namespace Internal
{
namespace Render
{
Geometry::Geometry()
: mIndices(),
  mIndexBuffer(nullptr),
  mGeometryType(Dali::Geometry::TRIANGLES),
  mIndicesChanged(false),
  mHasBeenUpdated(false),
  mAttributesChanged(true)
{
}

Geometry::~Geometry() = default;

void Geometry::AddVertexBuffer(Render::VertexBuffer* vertexBuffer)
{
  mVertexBuffers.PushBack(vertexBuffer);
  mAttributesChanged = true;
}

const Vector<Render::VertexBuffer*>& Geometry::GetVertexBuffers() const
{
  return mVertexBuffers;
}

void Geometry::SetIndexBuffer(Dali::Vector<uint16_t>& indices)
{
  mIndices.Swap(indices);
  mIndicesChanged = true;
}

void Geometry::RemoveVertexBuffer(const Render::VertexBuffer* vertexBuffer)
{
  const auto&& end = mVertexBuffers.End();
  for(auto&& iter = mVertexBuffers.Begin(); iter != end; ++iter)
  {
    if(*iter == vertexBuffer)
    {
      //This will delete the gpu buffer associated to the RenderVertexBuffer if there is one
      mVertexBuffers.Remove(iter);
      mAttributesChanged = true;
      break;
    }
  }
}

void Geometry::OnRenderFinished()
{
  mHasBeenUpdated    = false;
  mAttributesChanged = false;
}

void Geometry::Upload(Graphics::Controller& graphicsController)
{
  if(!mHasBeenUpdated)
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
          mIndexBuffer = new GpuBuffer(graphicsController, 0 | Graphics::BufferUsage::INDEX_BUFFER);
        }

        uint32_t bufferSize = static_cast<uint32_t>(sizeof(uint16_t) * mIndices.Size());
        mIndexBuffer->UpdateDataBuffer(graphicsController, bufferSize, &mIndices[0]);
      }

      mIndicesChanged = false;
    }

    for(auto&& buffer : mVertexBuffers)
    {
      if(!buffer->Update(graphicsController))
      {
        //Vertex buffer is not ready ( Size, data or format has not been specified yet )
        return;
      }
    }

    mHasBeenUpdated = true;
  }
}

bool Geometry::Draw(
  Graphics::Controller&    graphicsController,
  Graphics::CommandBuffer& commandBuffer,
  uint32_t                 elementBufferOffset,
  uint32_t                 elementBufferCount)
{
  //Bind buffers to attribute locations
  const auto vertexBufferCount = static_cast<uint32_t>(mVertexBuffers.Count());

  std::vector<const Graphics::Buffer*> buffers;
  std::vector<uint32_t>                offsets;

  for(uint32_t i = 0; i < vertexBufferCount; ++i)
  {
    const GpuBuffer* gpuBuffer = mVertexBuffers[i]->GetGpuBuffer();
    if(gpuBuffer)
    {
      const Graphics::Buffer* buffer = gpuBuffer->GetGraphicsObject();

      if(buffer)
      {
        buffers.push_back(buffer);
        offsets.push_back(0u);
      }
    }
    //@todo Figure out why this is being drawn without geometry having been uploaded
  }
  if(buffers.empty())
  {
    return false;
  }

  commandBuffer.BindVertexBuffers(0, buffers, offsets);

  uint32_t numIndices(0u);
  intptr_t firstIndexOffset(0u);
  if(mIndexBuffer)
  {
    numIndices = static_cast<uint32_t>(mIndices.Size());

    if(elementBufferOffset != 0u)
    {
      elementBufferOffset = (elementBufferOffset >= numIndices) ? numIndices - 1 : elementBufferOffset;
      firstIndexOffset    = elementBufferOffset * sizeof(GLushort);
      numIndices -= elementBufferOffset;
    }

    if(elementBufferCount != 0u)
    {
      numIndices = std::min(elementBufferCount, numIndices);
    }
  }

  //Draw call
  if(mIndexBuffer && mGeometryType != Dali::Geometry::POINTS)
  {
    //Indexed draw call
    const Graphics::Buffer* ibo = mIndexBuffer->GetGraphicsObject();
    if(ibo)
    {
      commandBuffer.BindIndexBuffer(*ibo, 0, Graphics::Format::R16_UINT);
    }

    commandBuffer.DrawIndexed(numIndices, 1, firstIndexOffset, 0, 0);
  }
  else
  {
    // Un-indexed draw call
    GLsizei numVertices(0u);
    if(vertexBufferCount > 0)
    {
      // truncated, no value loss happening in practice
      numVertices = static_cast<GLsizei>(mVertexBuffers[0]->GetElementCount());
    }

    commandBuffer.Draw(numVertices, 1, 0, 0);
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

} // namespace Render
} // namespace Internal
} // namespace Dali
