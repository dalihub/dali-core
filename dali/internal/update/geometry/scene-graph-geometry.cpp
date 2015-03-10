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

#include "scene-graph-geometry.h"

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

Geometry::Geometry()
: mCenter(),
  mHalfExtents(),
  mGeometryType(Dali::Geometry::TRIANGLES),
  mRequiresDepthTest(false)
{
  // @todo MESH_REWORK Remove this code when we have working property buffers
  // @todo It's also the wrong place to do this temporary work
  PropertyBuffer* vertexPropertyBuffer = PropertyBuffer::NewQuadVertices();
  mVertexBuffers.PushBack( vertexPropertyBuffer );

  mIndexBuffer = PropertyBuffer::NewQuadIndices();
}

Geometry::~Geometry()
{
  // @todo Inform renderers of deletion of buffers?
}

void Geometry::AddVertexBuffer( PropertyBuffer* vertexBuffer )
{
  mVertexBuffers.PushBack( vertexBuffer );
}

void Geometry::RemoveVertexBuffer( PropertyBuffer* vertexBuffer )
{
  DALI_ASSERT_DEBUG( NULL != vertexBuffer );

  // Find the object and destroy it
  for ( OwnerContainer< PropertyBuffer* >::Iterator iter = mVertexBuffers.Begin(); iter != mVertexBuffers.End(); ++iter )
  {
    PropertyBuffer* current = *iter;
    if ( current == vertexBuffer )
    {
      mVertexBuffers.Erase( iter );
      return;
    }
  }

  DALI_ASSERT_DEBUG(false);
}

void Geometry::SetIndexBuffer( PropertyBuffer* indexBuffer )
{
  mIndexBuffer = indexBuffer;
}

void Geometry::ClearIndexBuffer()
{
  // @todo Actually delete, or put on Discard Queue and tell Renderer in render thread?
  mIndexBuffer.Reset();
}

void Geometry::SetGeometryType( BufferIndex bufferIndex, Geometry::GeometryType geometryType )
{
  mGeometryType[bufferIndex] = geometryType;
}

const GeometryDataProvider::VertexBuffers& Geometry::GetVertexBuffers() const
{
  return mVertexBuffers;
}

const PropertyBuffer* Geometry::GetIndexBuffer() const
{
  return mIndexBuffer.Get();
}

Geometry::GeometryType Geometry::GetGeometryType( BufferIndex bufferIndex) const
{
  int geometryType = mGeometryType[ bufferIndex ];
  return static_cast< GeometryDataProvider::GeometryType > ( geometryType );
}

bool Geometry::GetRequiresDepthTest( BufferIndex bufferIndex ) const
{
  return mRequiresDepthTest.GetBoolean( bufferIndex );
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
