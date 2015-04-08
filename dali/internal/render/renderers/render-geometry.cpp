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

#include <dali/internal/render/renderers/render-geometry.h>

#include <dali/internal/common/buffer-index.h>
#include <dali/internal/update/geometry/scene-graph-geometry.h>
#include <dali/internal/update/common/scene-graph-property-buffer.h>
#include <dali/internal/render/data-providers/render-data-provider.h>
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
: mDataNeedsUploading( true ),
  mShaderChanged( true )
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
}

void RenderGeometry::UploadAndDraw(
  Context& context,
  Program& program,
  BufferIndex bufferIndex,
  const RenderDataProvider* dataProviders )
{
  UploadVertexData( context, bufferIndex, dataProviders );

  for( unsigned int i = 0; i < mVertexBuffers.Count(); ++i )
  {
    mVertexBuffers[i]->BindBuffer( context, program );
    mVertexBuffers[i]->EnableVertexAttributes( context, bufferIndex, program );
  }

  if( mIndexBuffer )
  {
    mIndexBuffer->BindBuffer( context, program );
  }

//  BindBuffers( context, bufferIndex, program );
//  EnableVertexAttributes( context, bufferIndex, program );
  Draw( context, bufferIndex, dataProviders );

  for( unsigned int i = 0; i < mVertexBuffers.Count(); ++i )
  {
    mVertexBuffers[i]->DisableVertexAttributes( context, bufferIndex, program );
  }
  //DisableVertexAttributes( context, bufferIndex, program );
}

void RenderGeometry::GeometryUpdated()
{
  mDataNeedsUploading = true;
}

void RenderGeometry::UploadVertexData(
  Context& context,
  BufferIndex bufferIndex,
  const RenderDataProvider* dataProviders )
{
  if( mDataNeedsUploading ) // @todo Or if any of the property buffers are dirty
  {
    DoUpload( context, bufferIndex, dataProviders );

    mDataNeedsUploading = false;
  }
}

void RenderGeometry::DoUpload(
  Context& context,
  BufferIndex bufferIndex,
  const RenderDataProvider* dataProvider )
{
  // Vertex buffer
  RenderDataProvider::VertexBuffers vertexBuffers = dataProvider->GetVertexBuffers();

  DALI_ASSERT_DEBUG( vertexBuffers.Count() > 0 && "Need vertex buffers to upload" );

  for( unsigned int i=0; i<vertexBuffers.Count(); ++i)
  {
    const PropertyBufferDataProvider* vertexBuffer = vertexBuffers[i];

    RenderPropertyBuffer* propertyBuffer = new RenderPropertyBuffer( *vertexBuffer, false );

    propertyBuffer->DoUpload( context, bufferIndex );

    mVertexBuffers.PushBack( propertyBuffer );
  }

  // Index buffer
  const PropertyBufferDataProvider* indexBuffer = dataProvider->GetIndexBuffer();
  if( indexBuffer )
  {
    mIndexBuffer = new RenderPropertyBuffer( *indexBuffer, true );

    mIndexBuffer->DoUpload( context, bufferIndex );
  }
}

void RenderGeometry::BindBuffers( Context& context, BufferIndex bufferIndex, Program& program )
{
  for( unsigned int i = 0; i < mVertexBuffers.Count(); ++i )
  {
    mVertexBuffers[i]->BindBuffer( context, program );
  }

  if( mIndexBuffer )
  {
    mIndexBuffer->BindBuffer( context, program );
  }
}

void RenderGeometry::EnableVertexAttributes( Context& context, BufferIndex bufferIndex, Program& program )
{
  OwnerContainer< RenderPropertyBuffer* >::Iterator it = mVertexBuffers.Begin();
  OwnerContainer< RenderPropertyBuffer* >::ConstIterator end = mVertexBuffers.End();
  for( ; it != end; ++it )
  {
    (*it)->EnableVertexAttributes( context, bufferIndex, program );
  }
}

void RenderGeometry::DisableVertexAttributes( Context& context, BufferIndex bufferIndex, Program& program )
{
  OwnerContainer< RenderPropertyBuffer* >::Iterator it = mVertexBuffers.Begin();
  OwnerContainer< RenderPropertyBuffer* >::ConstIterator end = mVertexBuffers.End();
  for( ; it != end; ++it )
  {
    (*it)->DisableVertexAttributes( context, bufferIndex, program );
  }
}

void RenderGeometry::Draw( Context& context, BufferIndex bufferIndex, const RenderDataProvider* dataProvider )
{
  const GeometryDataProvider& geometry = dataProvider->GetGeometry();
  const PropertyBufferDataProvider* indexBuffer = dataProvider->GetIndexBuffer();

  GeometryDataProvider::GeometryType type = geometry.GetGeometryType( bufferIndex );

  unsigned int numIndices = 0;
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
      const PropertyBufferDataProvider* firstVertexBuffer = dataProvider->GetVertexBuffers()[0];

      unsigned int numVertices = firstVertexBuffer->GetElementCount( bufferIndex );
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
