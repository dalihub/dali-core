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
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/gl-resources/gpu-buffer.h>
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

void Geometry::GlContextCreated(Context& context)
{
}

void Geometry::GlContextDestroyed()
{
}

void Geometry::AddVertexBuffer(Render::VertexBuffer* vertexBuffer)
{
  mVertexBuffers.PushBack(vertexBuffer);
  mAttributesChanged = true;
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

void Geometry::GetAttributeLocationFromProgram(Vector<GLint>& attributeLocation, Program& program, BufferIndex bufferIndex) const
{
  attributeLocation.Clear();

  for(auto&& vertexBuffer : mVertexBuffers)
  {
    const uint32_t attributeCount = vertexBuffer->GetAttributeCount();
    for(uint32_t j = 0; j < attributeCount; ++j)
    {
      auto     attributeName = vertexBuffer->GetAttributeName(j);
      uint32_t index         = program.RegisterCustomAttribute(attributeName);
      GLint    location      = program.GetCustomAttributeLocation(index);

      if(-1 == location)
      {
        DALI_LOG_WARNING("Attribute not found in the shader: %s\n", attributeName.GetCString());
      }

      attributeLocation.PushBack(location);
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

void Geometry::Draw(
  Context&                 context,
  Graphics::Controller&    graphicsController,
  Graphics::CommandBuffer& commandBuffer,
  BufferIndex              bufferIndex,
  Vector<GLint>&           attributeLocation,
  uint32_t                 elementBufferOffset,
  uint32_t                 elementBufferCount)
{
  //Bind buffers to attribute locations
  uint32_t       base              = 0u;
  const uint32_t vertexBufferCount = static_cast<uint32_t>(mVertexBuffers.Count());

  std::vector<const Graphics::Buffer*> buffers;
  std::vector<uint32_t>                offsets;

  for(uint32_t i = 0; i < vertexBufferCount; ++i)
  {
    Graphics::Buffer* buffer = mVertexBuffers[i]->GetGpuBuffer().GetGraphicsObject();

    if(buffer)
    {
      buffers.push_back(buffer);
      offsets.push_back(0u);
    }
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

  GLenum geometryGLType(GL_NONE);
  switch(mGeometryType)
  {
    case Dali::Geometry::TRIANGLES:
    {
      geometryGLType = GL_TRIANGLES;
      break;
    }
    case Dali::Geometry::LINES:
    {
      geometryGLType = GL_LINES;
      break;
    }
    case Dali::Geometry::POINTS:
    {
      geometryGLType = GL_POINTS;
      break;
    }
    case Dali::Geometry::TRIANGLE_STRIP:
    {
      geometryGLType = GL_TRIANGLE_STRIP;
      break;
    }
    case Dali::Geometry::TRIANGLE_FAN:
    {
      geometryGLType = GL_TRIANGLE_FAN;
      break;
    }
    case Dali::Geometry::LINE_LOOP:
    {
      geometryGLType = GL_LINE_LOOP;
      break;
    }
    case Dali::Geometry::LINE_STRIP:
    {
      geometryGLType = GL_LINE_STRIP;
      break;
    }
  }

  //Draw call
  if(mIndexBuffer && geometryGLType != GL_POINTS)
  {
    //Indexed draw call
    Graphics::Buffer* ibo = mIndexBuffer->GetGraphicsObject();
    if(ibo)
    {
      commandBuffer.BindIndexBuffer(*ibo, 0, Graphics::Format::R16_UINT);
    }

    // Command buffer contains Texture bindings, vertex bindings and index buffer binding.
    Graphics::SubmitInfo submitInfo{{}, 0 | Graphics::SubmitFlagBits::FLUSH};
    submitInfo.cmdBuffer.push_back(&commandBuffer);
    graphicsController.SubmitCommandBuffers(submitInfo);

    //@todo This should all be done from inside Pipeline implementation.
    //If there is only 1 vertex buffer, it should have been bound by SubmitCommandBuffers,
    //and the single GL call from this will work on that bound buffer.
    for(uint32_t i = 0; i < vertexBufferCount; ++i)
    {
      base += mVertexBuffers[i]->EnableVertexAttributes(context, attributeLocation, base);
    }

    // numIndices truncated, no value loss happening in practice
    context.DrawElements(geometryGLType, static_cast<GLsizei>(numIndices), GL_UNSIGNED_SHORT, reinterpret_cast<void*>(firstIndexOffset));
  }
  else
  {
    //Unindex draw call
    GLsizei numVertices(0u);
    if(vertexBufferCount > 0)
    {
      // truncated, no value loss happening in practice
      numVertices = static_cast<GLsizei>(mVertexBuffers[0]->GetElementCount());
    }

    // Command buffer contains Texture bindings & vertex bindings
    Graphics::SubmitInfo submitInfo{{}, 0 | Graphics::SubmitFlagBits::FLUSH};
    submitInfo.cmdBuffer.push_back(&commandBuffer);
    graphicsController.SubmitCommandBuffers(submitInfo);

    //@todo This should all be done from inside Pipeline implementation.
    //If there is only 1 vertex buffer, it should have been bound by SubmitCommandBuffers,
    //and the single GL call from this will work on that bound buffer.
    for(uint32_t i = 0; i < vertexBufferCount; ++i)
    {
      base += mVertexBuffers[i]->EnableVertexAttributes(context, attributeLocation, base);
    }

    context.DrawArrays(geometryGLType, 0, numVertices);
  }

  //Disable attributes
  for(auto&& attribute : attributeLocation)
  {
    if(attribute != -1)
    {
      context.DisableVertexAttributeArray(static_cast<GLuint>(attribute));
    }
  }
}

} // namespace Render
} // namespace Internal
} // namespace Dali
