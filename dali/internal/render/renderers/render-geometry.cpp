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
#include <dali/internal/update/common/scene-graph-property-buffer.h>
#include <dali/internal/update/rendering/scene-graph-geometry.h>
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

RenderGeometry::RenderGeometry( const GeometryDataProvider& geometryDataProvider )
: mGeometryDataProvider( geometryDataProvider ),
  mHasBeenUpdated(false),
  mAttributesChanged(true)
{
}

RenderGeometry::~RenderGeometry()
{
}

void RenderGeometry::GlContextCreated( Context& context )
{
}

void RenderGeometry::GlContextDestroyed()
{
}

void RenderGeometry::AddPropertyBuffer( const PropertyBufferDataProvider* dataProvider, GpuBuffer::Target gpuBufferTarget, GpuBuffer::Usage gpuBufferUsage )
{
  if( gpuBufferTarget == GpuBuffer::ELEMENT_ARRAY_BUFFER )
  {
    RenderPropertyBuffer* renderPropertyBuffer = new RenderPropertyBuffer( *dataProvider, gpuBufferTarget, gpuBufferUsage );
    mIndexBuffer = renderPropertyBuffer;
  }
  else if( gpuBufferTarget == GpuBuffer::ARRAY_BUFFER )
  {
    RenderPropertyBuffer* renderPropertyBuffer = new RenderPropertyBuffer( *dataProvider, gpuBufferTarget, gpuBufferUsage );
    mVertexBuffers.PushBack( renderPropertyBuffer );
    mAttributesChanged = true;
  }
}

void RenderGeometry::RemovePropertyBuffer( const PropertyBufferDataProvider* dataProvider )
{
  if( dataProvider == &mIndexBuffer->GetDataProvider() )
  {
    mIndexBuffer.Reset();
  }
  else
  {
    for( RenderPropertyBufferIter iter( mVertexBuffers.Begin() ); iter != mVertexBuffers.End(); ++iter )
    {
      if( dataProvider == &(*iter)->GetDataProvider() )
      {
        //This will delete the gpu buffer associated to the RenderPropertyBuffer if there is one
        mVertexBuffers.Remove( iter );
        mAttributesChanged = true;
        break;
      }
    }
  }
}

void RenderGeometry::GetAttributeLocationFromProgram( Vector<GLint>& attributeLocation, Program& program, BufferIndex bufferIndex ) const
{
  attributeLocation.Clear();

  for( size_t i(0); i< mVertexBuffers.Size(); ++i )
  {
    unsigned int attributeCount = mVertexBuffers[i]->GetDataProvider().GetAttributeCount( bufferIndex );
    for( unsigned int j = 0; j < attributeCount; ++j )
    {
      const std::string& attributeName = mVertexBuffers[i]->GetDataProvider().GetAttributeName( bufferIndex, j );
      unsigned int index = program.RegisterCustomAttribute( attributeName );
      GLint location = program.GetCustomAttributeLocation( index );

      if( -1 == location )
      {
        DALI_LOG_WARNING( "Attribute not found in the shader: %s\n", attributeName.c_str() );
      }

      attributeLocation.PushBack( location );
    }
  }
}

void RenderGeometry::OnRenderFinished()
{
  mHasBeenUpdated = false;
  mAttributesChanged = false;
}

void RenderGeometry::UploadAndDraw(
    Context& context,
    BufferIndex bufferIndex,
    Vector<GLint>& attributeLocation )
{
  if( !mHasBeenUpdated )
  {
    //Update buffers
    if( mIndexBuffer )
    {
      mIndexBuffer->Update( context, bufferIndex );
    }
    for( unsigned int i = 0; i < mVertexBuffers.Count(); ++i )
    {
      mVertexBuffers[i]->Update( context, bufferIndex );
    }
    mHasBeenUpdated = true;
  }

  //Bind buffers to attribute locations
  unsigned int base = 0;
  for( unsigned int i = 0; i < mVertexBuffers.Count(); ++i )
  {
    mVertexBuffers[i]->BindBuffer( context );
    base += mVertexBuffers[i]->EnableVertexAttributes( context, bufferIndex, attributeLocation, base );
  }

  if( mIndexBuffer )
  {
    mIndexBuffer->BindBuffer( context );
  }

  //Bind index buffer
  unsigned int numIndices(0u);
  if( mIndexBuffer )
  {
    const PropertyBufferDataProvider& indexBuffer = mIndexBuffer->GetDataProvider();
    numIndices = mIndexBuffer->GetDataProvider().GetDataSize(bufferIndex) / indexBuffer.GetElementSize(bufferIndex);
  }

  //Draw call
  GeometryDataProvider::GeometryType type = mGeometryDataProvider.GetGeometryType( bufferIndex );
  switch(type)
  {
    case Dali::Geometry::TRIANGLES:
    {
      if( numIndices )
      {
        context.DrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);
      }
      else
      {
        const PropertyBufferDataProvider& firstVertexBuffer = mVertexBuffers[0]->GetDataProvider();
        unsigned int numVertices = firstVertexBuffer.GetElementCount( bufferIndex );
        context.DrawArrays( GL_TRIANGLES, 0, numVertices );
      }
      break;
    }
    case Dali::Geometry::LINES:
    {
      if( numIndices )
      {
        context.DrawElements(GL_LINES, numIndices, GL_UNSIGNED_SHORT, 0);
      }
      else
      {
        const PropertyBufferDataProvider& firstVertexBuffer = mVertexBuffers[0]->GetDataProvider();
        unsigned int numVertices = firstVertexBuffer.GetElementCount( bufferIndex );
        context.DrawArrays( GL_LINES, 0, numVertices );
      }
      break;
    }
    case Dali::Geometry::POINTS:
    {
      const PropertyBufferDataProvider& firstVertexBuffer = mVertexBuffers[0]->GetDataProvider();
      unsigned int numVertices = firstVertexBuffer.GetElementCount( bufferIndex );
      context.DrawArrays(GL_POINTS, 0, numVertices );
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS( 0 && "Geometry type not supported (yet)" );
      break;
    }
  }

  //Disable atrributes
  for( unsigned int i = 0; i < attributeLocation.Count(); ++i )
  {
    if( attributeLocation[i] != -1 )
    {
      context.DisableVertexAttributeArray( attributeLocation[i] );
    }
  }
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
