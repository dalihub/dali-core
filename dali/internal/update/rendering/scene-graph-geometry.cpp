/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/rendering/scene-graph-geometry.h>

// INTERNAL HEADERS
#include <dali/integration-api/debug.h>
#include <dali/internal/update/rendering/scene-graph-property-buffer.h>

#include <dali/graphics-api/graphics-buffer.h>

// EXTERNAL HEADERS
#include <cstring>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

Geometry::Geometry()
: mGraphicsController( nullptr ),
  mIndexBuffer{ nullptr },
  mIndexBufferElementCount( 0 ),
  mGeometryType( Dali::Geometry::TRIANGLES ),
  mIndicesChanged(false),
  mHasBeenUpdated(false),
  mAttributesChanged(true),
  mHasIndexBuffer(false)
{
}

Geometry::~Geometry() = default;

void Geometry::Initialize( Graphics::Controller& graphics )
{
  mGraphicsController = &graphics;
}

void Geometry::AddPropertyBuffer( SceneGraph::PropertyBuffer* propertyBuffer )
{
  mVertexBuffers.PushBack( propertyBuffer );
  mAttributesChanged = true;
}

void Geometry::SetIndexBuffer( Dali::Vector<unsigned short>& indices )
{
  // set new index buffer
  //auto sizeInBytes = uint32_t(indices.Size() * sizeof(indices[0]));
  std::memcpy(mIndices.data(), indices.begin(), indices.Count()*sizeof(unsigned short));
  mIndicesChanged = true;
}

void Geometry::RemovePropertyBuffer( const SceneGraph::PropertyBuffer* propertyBuffer )
{
  size_t bufferCount = mVertexBuffers.Size();
  for( size_t i(0); i<bufferCount; ++i )
  {
    if( propertyBuffer == mVertexBuffers[i] )
    {
      //This will delete the gpu buffer associated to the RenderPropertyBuffer if there is one
      mVertexBuffers.Remove( mVertexBuffers.Begin()+i );
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
      if(mIndices.empty())
      {
        mIndexBuffer = nullptr;
      }
      else
      {
        if(mIndexBuffer == nullptr)
        {
          // Currently we are unable to reuse index buffer so the write policy is to preserve current content
          mIndexBuffer = new GpuBuffer(0 | Graphics::BufferUsage::INDEX_BUFFER, GpuBuffer::WritePolicy::RETAIN);
        }

        auto bufferSize = static_cast<uint32_t>(sizeof(uint16_t) * mIndices.size());
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

bool Geometry::BindVertexAttributes(Graphics::CommandBuffer& commandBuffer)
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
  }
  if(buffers.empty() || buffers.size() != vertexBufferCount)
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
    std::size_t sizeOfIndex = sizeof(uint16_t);

    numIndices = static_cast<uint32_t>(mIndices.size() * sizeof(uint16_t) / sizeOfIndex);

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

  //Draw call
  if(mIndexBuffer && mGeometryType != Dali::Geometry::POINTS)
  {
    // Issue draw call only if there's non-zero numIndices
    if(numIndices)
    {
      //Indexed draw call
      const Graphics::Buffer* ibo = mIndexBuffer->GetGraphicsObject();
      if(ibo)
      {
        commandBuffer.BindIndexBuffer(*ibo, 0, Graphics::Format::R16_UINT);
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
    else if(mVertexBuffers.Count() > 0)
    {
      // truncated, no value loss happening in practice
      numVertices = static_cast<uint32_t>(mVertexBuffers[0]->GetElementCount());
    }
    // In case we have more buffers, we select buffer with less elements to render
    // TODO: we may eventually support wrapping around buffers????
    else if(mVertexBuffers.Count() > 1)
    {
      auto elementsCount = mVertexBuffers[0]->GetElementCount();
      for(auto& vertexBuffer : mVertexBuffers)
      {
        elementsCount = std::min(elementsCount, vertexBuffer->GetElementCount());
      }
      numVertices = elementsCount;
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


} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
