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
 *
 */

// CLASS HEADER
#include <dali/internal/event/rendering/geometry-impl.h> // Dali::Internal::Geometry

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/manager/update-manager.h>

namespace Dali
{
namespace Internal
{

GeometryPtr Geometry::New()
{
  GeometryPtr geometry( new Geometry() );
  geometry->Initialize();
  return geometry;
}

uint32_t Geometry::AddVertexBuffer( PropertyBuffer& vertexBuffer )
{
  mVertexBuffers.push_back( &vertexBuffer );
  SceneGraph::AttachVertexBufferMessage( mEventThreadServices, *mRenderObject, *vertexBuffer.GetRenderObject() );
  return static_cast<uint32_t>( mVertexBuffers.size() - 1u );
}

uint32_t Geometry::GetNumberOfVertexBuffers() const
{
  return static_cast<uint32_t>( mVertexBuffers.size() );
}

void Geometry::RemoveVertexBuffer( uint32_t index )
{
  const SceneGraph::PropertyBuffer& renderPropertyBuffer = static_cast<const SceneGraph::PropertyBuffer&>( *(mVertexBuffers[index]->GetRenderObject()) );
  SceneGraph::RemoveVertexBufferMessage( mEventThreadServices, *mRenderObject, renderPropertyBuffer );

  mVertexBuffers.erase( mVertexBuffers.begin() + index );
}

void Geometry::SetIndexBuffer( const uint16_t* indices, uint32_t count )
{
  Dali::Vector<uint16_t> indexData;
  if( indices && count )
  {
    indexData.Resize( count );
    std::copy( indices, indices + count, indexData.Begin() );
  }

  SceneGraph::SetIndexBufferMessage( mEventThreadServices, *mRenderObject, indexData );
}

void Geometry::SetType( Dali::Geometry::Type geometryType )
{
  if( geometryType != mType )
  {
    SceneGraph::SetGeometryTypeMessage(mEventThreadServices, *mRenderObject, geometryType );

    mType = geometryType;
  }
}

Dali::Geometry::Type Geometry::GetType() const
{
  return mType;
}

const SceneGraph::Geometry* Geometry::GetRenderObject() const
{
  return mRenderObject;
}

Geometry::Geometry()
: mEventThreadServices( *Stage::GetCurrent() ),
  mRenderObject( NULL ),
  mType(Dali::Geometry::TRIANGLES)
{
}

void Geometry::Initialize()
{
  mRenderObject = new SceneGraph::Geometry();
  OwnerPointer< SceneGraph::Geometry > transferOwnership( mRenderObject );
  AddGeometryMessage( mEventThreadServices.GetUpdateManager(), transferOwnership );
}

Geometry::~Geometry()
{
  if( EventThreadServices::IsCoreRunning() && mRenderObject )
  {
    RemoveGeometryMessage( mEventThreadServices.GetUpdateManager(), *mRenderObject );
  }
}

} // namespace Internal

} // namespace Dali