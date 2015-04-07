/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include "render-geometry.h"

#include <dali/internal/common/buffer-index.h>
#include <dali/internal/update/geometry/scene-graph-geometry.h>
#include <dali/internal/update/common/scene-graph-property-buffer.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/gl-resources/gpu-buffer.h>
#include <dali/internal/render/shaders/program.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

RenderGeometry::RenderGeometry()
: mDataNeedsUploading( true )
{
}

RenderGeometry::~RenderGeometry()
{
}

void RenderGeometry::GlContextCreated( Context& context )
{
  mDataNeedsUploading = true;
}

void RenderGeometry::GlContextDestroyed()
{
  for( GpuBuffers::Iterator iter=mVertexBuffers.Begin(); iter != mVertexBuffers.End(); ++iter )
  {
    GpuBuffer* gpuBuffer = *iter;
    if( gpuBuffer )
    {
      gpuBuffer->GlContextDestroyed();
    }
  }

  if( mIndexBuffer )
  {
    mIndexBuffer->GlContextDestroyed();
  }
}

void RenderGeometry::UploadAndDraw(
  Context& context,
  Program& program,
  BufferIndex bufferIndex,
  const GeometryDataProvider& geometryDataProvider )
{
  UploadVertexData( context, bufferIndex, geometryDataProvider );
  BindBuffers();
  EnableVertexAttributes( context, program );
  Draw( context, bufferIndex, geometryDataProvider );
  DisableVertexAttributes( context, program );
}

void RenderGeometry::GeometryUpdated()
{
  mDataNeedsUploading = true;
}

void RenderGeometry::UploadVertexData(
  Context& context,
  BufferIndex bufferIndex,
  const GeometryDataProvider& geometry )
{
  if( mDataNeedsUploading ) // @todo Or if any of the property buffers are dirty
  {
    DoUpload( context, bufferIndex, geometry );

    mDataNeedsUploading = false;
  }
}

void RenderGeometry::DoUpload(
  Context& context,
  BufferIndex bufferIndex,
  const GeometryDataProvider& geometry)
{
  // Vertex buffer
  const Geometry::VertexBuffers& vertexBuffers = geometry.GetVertexBuffers();
  DALI_ASSERT_DEBUG( vertexBuffers.Count() > 0 && "Need vertex buffers to upload" );

  for( unsigned int i=0; i<vertexBuffers.Count(); ++i)
  {
    const PropertyBuffer* vertexBuffer = vertexBuffers[i];

    // @todo MESH_REWORK STATIC_DRAW or DYNAMIC_DRAW depends on property buffer type (static / animated)
    GpuBuffer* vertexGpuBuffer = new GpuBuffer( context, GpuBuffer::ARRAY_BUFFER, GpuBuffer::STATIC_DRAW );

    std::size_t dataSize = vertexBuffer->GetDataSize( bufferIndex );
    vertexGpuBuffer->UpdateDataBuffer( dataSize, vertexBuffer->GetData( bufferIndex ) );
    vertexGpuBuffer->SetStride( vertexBuffer->GetElementSize( bufferIndex ) );

    mVertexBuffers.PushBack( vertexGpuBuffer );
  }

  // Index buffer
  const PropertyBuffer* indexBuffer = geometry.GetIndexBuffer();
  if( indexBuffer )
  {
    GpuBuffer* indexGpuBuffer = new GpuBuffer( context, GpuBuffer::ELEMENT_ARRAY_BUFFER, GpuBuffer::STATIC_DRAW );

    unsigned int dataSize = indexBuffer->GetDataSize( bufferIndex );
    indexGpuBuffer->UpdateDataBuffer( dataSize, indexBuffer->GetData( bufferIndex ) );

    mIndexBuffer.Reset();
    mIndexBuffer = indexGpuBuffer;
  }
}

void RenderGeometry::BindBuffers()
{
  for( GpuBuffers::Iterator iter=mVertexBuffers.Begin(); iter != mVertexBuffers.End(); ++iter )
  {
    (*iter)->Bind();
  }

  if( mIndexBuffer )
  {
    mIndexBuffer->Bind();
  }
}

void RenderGeometry::EnableVertexAttributes( Context& context, Program& program )
{
  // @todo Loop thru the array of vertex buffers
  // @todo Use AttributeDataProvider to get the attrs and enable them
  // Need mapping from gpu buffers index to a particular attributes
  Vector4 *vertex=0;

  unsigned int gpuBufferIndex = 0;

  GLint positionLoc = program.GetAttribLocation( Program::ATTRIB_POSITION );
  context.VertexAttribPointer( positionLoc,
                                2,         // 2D position
                                GL_FLOAT,
                                GL_FALSE,  // Not normalized
                                mVertexBuffers[gpuBufferIndex]->GetStride(),
                                &vertex->x );

  context.EnableVertexAttributeArray( positionLoc );

  GLint textureCoordsLoc = program.GetAttribLocation( Program::ATTRIB_TEXCOORD );
  context.VertexAttribPointer( textureCoordsLoc,
                                2,         // Texture Coords = U, V
                                GL_FLOAT,
                                GL_FALSE,
                                mVertexBuffers[gpuBufferIndex]->GetStride(),
                                &vertex->z );
  context.EnableVertexAttributeArray( textureCoordsLoc );
}

void RenderGeometry::DisableVertexAttributes( Context& context, Program& program )
{
  // @todo Loop thru the array of vertex buffers
  // @todo Use AttributeDataProvider to get the attrs and disable them
  GLint positionLoc = program.GetAttribLocation( Program::ATTRIB_POSITION );
  GLint textureCoordsLoc = program.GetAttribLocation( Program::ATTRIB_TEXCOORD );
  context.DisableVertexAttributeArray( positionLoc );
  context.DisableVertexAttributeArray( textureCoordsLoc );
}

void RenderGeometry::Draw( Context& context, BufferIndex bufferIndex, const GeometryDataProvider& geometry )
{
  GeometryDataProvider::GeometryType type = geometry.GetGeometryType( bufferIndex );

  unsigned int numIndices = 0;
  const PropertyBuffer* indexBuffer = geometry.GetIndexBuffer();

  if( indexBuffer )
  {
    numIndices = /* TODO: MESH_REWORK remove this 2, should implement unsigned short properties  */ 2 * indexBuffer->GetDataSize(bufferIndex) / indexBuffer->GetElementSize(bufferIndex);
  }

  switch(type)
  {
    case Dali::Geometry::TRIANGLES:
    {
      context.DrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);
      break;
    }
    case Dali::Geometry::LINES:
    {
      context.DrawElements(GL_LINES, numIndices, GL_UNSIGNED_SHORT, 0);
      break;
    }
    case Dali::Geometry::POINTS:
    {
      GpuBuffer* firstVertexBuffer = mVertexBuffers[0];

      unsigned int numVertices = 0;
      GLuint stride = firstVertexBuffer->GetStride();
      if( stride != 0 )
      {
        numVertices = firstVertexBuffer->GetBufferSize() / stride;
      }

      context.DrawArrays(GL_POINTS, 0, numVertices );
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS( 0 && "Geometry type not supported (yet)" );
      break;
    }
  }
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
