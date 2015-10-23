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

// CLASS HEADER
#include "scene-graph-geometry.h"

// INTERNAL HEADERS
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/render/renderers/render-geometry.h>
#include <dali/internal/render/renderers/render-property-buffer.h>
#include <dali/internal/update/controllers/render-message-dispatcher.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

Geometry::Geometry()
: mRenderGeometry(NULL),
  mSceneController(NULL),
  mIndexBuffer( NULL ),
  mGeometryType(Dali::Geometry::TRIANGLES),
  mRendererRefCount(0u),
  mRequiresDepthTest(false)
{

  // Observe our own PropertyOwner's uniform map
  AddUniformMapObserver( *this );
}

Geometry::~Geometry()
{
  mConnectionObservers.Destroy( *this );
}

void Geometry::AddVertexBuffer( Render::PropertyBuffer* vertexBuffer )
{
  mVertexBuffers.PushBack( vertexBuffer );
  mConnectionObservers.ConnectionsChanged(*this);

  if( mRenderGeometry )
  {
    mSceneController->GetRenderMessageDispatcher().AddPropertyBuffer( *mRenderGeometry, vertexBuffer, false);
  }
}

void Geometry::RemoveVertexBuffer( Render::PropertyBuffer* vertexBuffer )
{
  DALI_ASSERT_DEBUG( NULL != vertexBuffer );

  // Find the object and destroy it
  size_t bufferCount(mVertexBuffers.Size());
  for( size_t i(0); i<bufferCount; ++i )
  {
    if( vertexBuffer == mVertexBuffers[i] )
    {
      mVertexBuffers.Erase( mVertexBuffers.Begin()+i );
      mConnectionObservers.ConnectionsChanged(*this);

      if( mRenderGeometry )
      {
        mSceneController->GetRenderMessageDispatcher().RemovePropertyBuffer( *mRenderGeometry, vertexBuffer );
      }
    }
  }
}

void Geometry::SetIndexBuffer( Render::PropertyBuffer* indexBuffer )
{
  if( mIndexBuffer != indexBuffer )
  {
    mIndexBuffer = indexBuffer;
    mConnectionObservers.ConnectionsChanged(*this);

    if( mRenderGeometry )
    {
      mSceneController->GetRenderMessageDispatcher().AddPropertyBuffer( *mRenderGeometry, indexBuffer, true );
    }
  }
}

void Geometry::ClearIndexBuffer()
{
  if( mIndexBuffer )
  {
    if( mRenderGeometry )
    {
      mSceneController->GetRenderMessageDispatcher().RemovePropertyBuffer( *mRenderGeometry, mIndexBuffer );
    }
  }
  mIndexBuffer = 0;
  mConnectionObservers.ConnectionsChanged(*this);
}

void Geometry::SetGeometryType(Geometry::GeometryType geometryType )
{
  mGeometryType = geometryType;
  if( mRenderGeometry )
  {
    mSceneController->GetRenderMessageDispatcher().SetGeometryType( *mRenderGeometry, static_cast<int>(geometryType) );
  }
}

void Geometry::SetRequiresDepthTest( bool requiresDepthTest )
{
  mRequiresDepthTest = requiresDepthTest;
  if( mRenderGeometry )
  {
    mSceneController->GetRenderMessageDispatcher().SetGeometryRequiresDepthTest( *mRenderGeometry, requiresDepthTest );
  }
}

Vector<Render::PropertyBuffer*>& Geometry::GetVertexBuffers()
{
  return mVertexBuffers;
}

Render::PropertyBuffer* Geometry::GetIndexBuffer()
{
  return mIndexBuffer;
}

Geometry::GeometryType Geometry::GetGeometryType( BufferIndex bufferIndex) const
{
  return mGeometryType;
}

bool Geometry::GetRequiresDepthTesting( BufferIndex bufferIndex ) const
{
  return mRequiresDepthTest;
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

RenderGeometry* Geometry::GetRenderGeometry(SceneController* sceneController)
{
  if(!mRenderGeometry )
  {
    //Create RenderGeometry
    mSceneController = sceneController;
    mRenderGeometry = new RenderGeometry( mGeometryType, mRequiresDepthTest );

    size_t vertexBufferCount( mVertexBuffers.Size() );
    for( size_t i(0); i<vertexBufferCount; ++i )
    {
      mRenderGeometry->AddPropertyBuffer( mVertexBuffers[i], false );
    }

    if( mIndexBuffer )
    {
      mRenderGeometry->AddPropertyBuffer( mIndexBuffer, true );
    }

    //Transfer ownership to RenderManager
    sceneController->GetRenderMessageDispatcher().AddGeometry( *mRenderGeometry );
  }

  ++mRendererRefCount;
  return mRenderGeometry;
}

void Geometry::OnRendererDisconnect()
{
  --mRendererRefCount;
  if( mRendererRefCount == 0 )
  {
    //Delete the corresponding RenderGeometry via message to RenderManager
    mSceneController->GetRenderMessageDispatcher().RemoveGeometry( *mRenderGeometry );

    mRenderGeometry = 0;
    mSceneController = 0;
  }
}


} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
