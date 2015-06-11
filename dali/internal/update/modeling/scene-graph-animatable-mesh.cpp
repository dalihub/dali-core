/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/modeling/scene-graph-animatable-mesh.h>

// INTERNAL INCLUDES
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/internal/update/modeling/scene-graph-mesh.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

AnimatableMesh::AnimatableMesh( ResourceManager&                  resourceManager,
                                ResourceId                        meshId,
                                const MeshData::VertexContainer& vertices )
: mResourceManager(resourceManager),
  mMeshId(meshId)
{
  mNumVertices = vertices.size();

  // Allocate a single block of memory for all properties
  // (Can't use std::vector, as AnimatableProperties is a non-copyable type).
  mVertices = static_cast<VertexProperties*>( operator new [] (sizeof(VertexProperties) * mNumVertices) );

  for(unsigned int i=0; i<mNumVertices; i++)
  {
    // Use placement new to instantiate the vertex properties
    new ( &mVertices[i] ) VertexProperties( vertices[i] );
  }
}

AnimatableMesh::~AnimatableMesh()
{
  for(unsigned int i=0; i<mNumVertices; i++)
  {
    // Used placement new to instantiate these objects, so have to directly call destructors
    mVertices[i].~VertexProperties();
  }
  operator delete [] (mVertices);
}

void AnimatableMesh::UpdateMesh( BufferIndex updateBufferIndex )
{
  // Copy properties to associated scenegraph mesh
  SceneGraph::Mesh* mesh(mResourceManager.GetMesh(mMeshId));
  DALI_ASSERT_DEBUG( mesh );

  // TODO: Should be double buffered - pass in buffer index
  MeshData& meshData = mesh->GetMeshData(Mesh::UPDATE_THREAD);

  bool meshUpdated( false );
  MeshData::VertexContainer& oldVertices = meshData.GetVertices();
  DALI_ASSERT_DEBUG( oldVertices.size() == mNumVertices );
  DALI_ASSERT_DEBUG( oldVertices.size() > 0 );

  MeshData::Vertex* vertices = &oldVertices.at(0);
  for(std::size_t i=0; i<mNumVertices; i++)
  {
    if ( ! mVertices[i].position.IsClean() )
    {
      const Vector3& position = GetPosition(updateBufferIndex, i);
      vertices[i].x = position.x;
      vertices[i].y = position.y;
      vertices[i].z = position.z;

      meshUpdated = true;
    }

    if ( ! mVertices[i].color.IsClean() )
    {
      const Vector4& color = GetColor(updateBufferIndex, i);
      vertices[i].vertexR = color.r;
      vertices[i].vertexG = color.g;
      vertices[i].vertexB = color.b;

      meshUpdated = true;
    }

    if ( ! mVertices[i].textureCoords.IsClean() )
    {
      const Vector2& texcoords = GetTextureCoords(updateBufferIndex, i);
      vertices[i].u = texcoords.x;
      vertices[i].v = texcoords.y;

      meshUpdated = true;
    }
  }

  if ( meshUpdated )
  {
    mesh->MeshDataUpdated(updateBufferIndex, Mesh::UPDATE_THREAD, NULL);
  }
}

void AnimatableMesh::BakePosition( BufferIndex updateBufferIndex, unsigned int vertex, const Vector3& position )
{
  mVertices[vertex].position.Bake( updateBufferIndex, position );
}

void AnimatableMesh::BakeColor( BufferIndex updateBufferIndex, unsigned int vertex, const Vector4& color )
{
  mVertices[vertex].color.Bake( updateBufferIndex, color );
}

void AnimatableMesh::BakeTextureCoords( BufferIndex updateBufferIndex, unsigned int vertex, const Vector2& coords )
{
  mVertices[vertex].textureCoords.Bake( updateBufferIndex, coords );
}

const Vector3& AnimatableMesh::GetPosition( BufferIndex bufferIndex, unsigned int vertexIndex )
{
  return mVertices[vertexIndex].position.Get(bufferIndex);
}

const Vector4& AnimatableMesh::GetColor( BufferIndex bufferIndex, unsigned int vertexIndex )
{
  return mVertices[vertexIndex].color.Get(bufferIndex);
}

const Vector2& AnimatableMesh::GetTextureCoords( BufferIndex bufferIndex, unsigned int vertexIndex )
{
  return mVertices[vertexIndex].textureCoords.Get(bufferIndex);
}

void AnimatableMesh::ResetDefaultProperties( BufferIndex updateBufferIndex )
{
  for( std::size_t i=0; i<mNumVertices; i++)
  {
    mVertices[i].position.ResetToBaseValue( updateBufferIndex );
    mVertices[i].color.ResetToBaseValue( updateBufferIndex );
    mVertices[i].textureCoords.ResetToBaseValue( updateBufferIndex );
  }
}

} // SceneGraph

} // Internal

} // Dali
