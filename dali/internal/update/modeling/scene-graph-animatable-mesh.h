#ifndef __DALI_INTERNAL_SCENE_GRAPH_ANIMATABLE_MESH_H__
#define __DALI_INTERNAL_SCENE_GRAPH_ANIMATABLE_MESH_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL INCLUDES
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/message.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/resources/resource-manager-declarations.h>
#include <dali/internal/update/modeling/internal-mesh-data.h>

namespace Dali
{

namespace Internal
{

class ResourceManager;

namespace SceneGraph
{

class Mesh;

/**
 * This manages a set of animatable vertex properties and the updating
 * of an associated SceneGraph::Mesh object.
 */
class AnimatableMesh : public PropertyOwner
{
public:
  /**
   * Constructor
   */
  AnimatableMesh( ResourceManager& resourceManager,
                  ResourceId meshId,
                  const MeshData::VertexContainer& vertices );

  /**
   * Destructor
   */
  ~AnimatableMesh();

  /**
   * Update the associated scenegraph mesh from the current properties
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void UpdateMesh( BufferIndex updateBufferIndex );

  /**
   * Set the vertex position
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] vertex The index of the vertex
   * @param[in] position The position.
   */
  void BakePosition( BufferIndex updateBufferIndex, unsigned int vertex, const Vector3& position );

  /**
   * Set the vertex color
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] vertex The index of the vertex
   * @param[in] color The color.
   */
  void BakeColor( BufferIndex updateBufferIndex, unsigned int vertex, const Vector4& color );

  /**
   * Set the vertex texture coordinates
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] vertex The index of the vertex
   * @param[in] coords The texture coordinates.
   */
  void BakeTextureCoords( BufferIndex updateBufferIndex, unsigned int vertex, const Vector2& coords );

  /**
   * Get the vertex position
   * @param[in] bufferIndex The buffer to read from
   * @param[in] vertexIndex The index of the vertex
   * @return The position.
   */
  const Vector3& GetPosition( BufferIndex bufferIndex, unsigned int vertexIndex );

  /**
   * Get the vertex color
   * @param[in] bufferIndex The buffer to read from
   * @param[in] vertex The index of the vertex
   * @return The color.
   */
  const Vector4& GetColor( BufferIndex bufferIndex, unsigned int vertexIndex );

  /**
   * Get the vertex texture coordinates
   * @param[in] bufferIndex The buffer to read from
   * @param[in] vertex The index of the vertex
   * @return coords The texture coordinates.
   */
  const Vector2& GetTextureCoords( BufferIndex bufferIndex, unsigned int vertex );

private: // PropertyOwner

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyOwner::ResetDefaultProperties()
   */
  virtual void ResetDefaultProperties( BufferIndex updateBufferIndex );

public:

  struct VertexProperties
  {
    VertexProperties( const Dali::Internal::MeshData::Vertex& v )
    : position( Vector3(v.x, v.y, v.z) ),
      color( Vector4(v.vertexR, v.vertexG, v.vertexB, 1.0f) ),
      textureCoords( Vector2(v.u, v.v) )
    {
    }

    VertexProperties(const VertexProperties& rhs)
    : position( rhs.position.Get(0) ),
      color( rhs.color.Get(0) ),
      textureCoords( rhs.textureCoords.Get(0) )
    {
    }

    AnimatableProperty<Vector3> position;
    AnimatableProperty<Vector4> color;
    AnimatableProperty<Vector2> textureCoords;
  };

  VertexProperties* mVertices;
  std::size_t mNumVertices;

private:

  ResourceManager& mResourceManager; ///< The resource manager object
  ResourceId mMeshId;                ///< The resource id of the scene graph mesh
};

inline void BakeVertexPositionMessage( EventToUpdate& eventToUpdate, const SceneGraph::AnimatableMesh& mesh, unsigned int vertex, const Vector3& position )
{
  typedef MessageDoubleBuffered2< SceneGraph::AnimatableMesh, unsigned int, Vector3 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &mesh, &SceneGraph::AnimatableMesh::BakePosition, vertex, position );
}

inline void BakeVertexColorMessage( EventToUpdate& eventToUpdate, const SceneGraph::AnimatableMesh& mesh, unsigned int vertex, const Vector4& color )
{
  typedef MessageDoubleBuffered2< SceneGraph::AnimatableMesh, unsigned int, Vector4 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &mesh, &SceneGraph::AnimatableMesh::BakeColor, vertex, color );
}

inline void BakeVertexTextureCoordsMessage( EventToUpdate& eventToUpdate, const SceneGraph::AnimatableMesh& mesh, unsigned int vertex, const Vector2& coords )
{
  typedef MessageDoubleBuffered2< SceneGraph::AnimatableMesh, unsigned int, Vector2 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &mesh, &SceneGraph::AnimatableMesh::BakeTextureCoords, vertex, coords );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali


#endif
