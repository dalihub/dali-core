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

  // Observe our own PropertyOwner's uniform map
  AddUniformMapObserver( *this );
}

Geometry::~Geometry()
{
  // @todo Inform renderers of deletion of buffers?

  // could remove self from own uniform map observer, but it's about to be destroyed.
}

void Geometry::AddVertexBuffer( PropertyBuffer* vertexBuffer )
{
  mVertexBuffers.PushBack( vertexBuffer );
  vertexBuffer->AddUniformMapObserver(*this);
  mConnectionObservers.ConnectionsChanged(*this);
}

void Geometry::RemoveVertexBuffer( PropertyBuffer* vertexBuffer )
{
  DALI_ASSERT_DEBUG( NULL != vertexBuffer );

  // Find the object and destroy it
  Vector<PropertyBuffer*>::Iterator match = std::find(
    mVertexBuffers.Begin(),
    mVertexBuffers.End(),
    vertexBuffer );

  DALI_ASSERT_DEBUG( mVertexBuffers.End() != match );
  if( mVertexBuffers.End() != match )
  {
    vertexBuffer->RemoveUniformMapObserver(*this);
    mVertexBuffers.Erase( match );
    mConnectionObservers.ConnectionsChanged(*this);
  }
}

void Geometry::SetIndexBuffer( PropertyBuffer* indexBuffer )
{
  if( mIndexBuffer != indexBuffer )
  {
    mIndexBuffer = indexBuffer;
    indexBuffer->AddUniformMapObserver(*this);
    mConnectionObservers.ConnectionsChanged(*this);
  }
}

void Geometry::ClearIndexBuffer()
{
  // @todo Actually delete, or put on Discard Queue and tell Renderer in render thread?
  if( mIndexBuffer )
  {
    mIndexBuffer->RemoveUniformMapObserver(*this);
  }
  mIndexBuffer = 0;
  mConnectionObservers.ConnectionsChanged(*this);
}

void Geometry::SetGeometryType( BufferIndex bufferIndex, Geometry::GeometryType geometryType )
{
  mGeometryType.Set( bufferIndex, geometryType);
}

Vector<PropertyBuffer*>& Geometry::GetVertexBuffers()
{
  return mVertexBuffers;
}

PropertyBuffer* Geometry::GetIndexBuffer()
{
  return mIndexBuffer;
}

Geometry::GeometryType Geometry::GetGeometryType( BufferIndex bufferIndex) const
{
  int geometryType = mGeometryType[ bufferIndex ];
  return static_cast< GeometryDataProvider::GeometryType > ( geometryType );
}

bool Geometry::GetRequiresDepthTesting( BufferIndex bufferIndex ) const
{
  return mRequiresDepthTest.GetBoolean( bufferIndex );
}

void Geometry::ResetDefaultProperties( BufferIndex updateBufferIndex )
{
  // Reset the animated properties
  mCenter.ResetToBaseValue( updateBufferIndex );
  mHalfExtents.ResetToBaseValue( updateBufferIndex );

  // Age the double buffered properties
  mGeometryType.CopyPrevious(updateBufferIndex);
  mRequiresDepthTest.CopyPrevious(updateBufferIndex);
}

void Geometry::ConnectToSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
}

void Geometry::DisconnectFromSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
}

void Geometry::AddConnectionObserver( ConnectionChangePropagator::Observer& observer )
{
  mConnectionObservers.Add(observer);
}

void Geometry::RemoveConnectionObserver( ConnectionChangePropagator::Observer& observer )
{
  mConnectionObservers.Remove(observer);
}

void Geometry::UniformMappingsChanged( const UniformMap& mappings )
{
  // Our uniform map, or that of one of the watched children has changed.
  // Inform connected observers.
  mConnectionObservers.ConnectedUniformMapChanged();
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
