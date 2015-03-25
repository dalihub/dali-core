#ifndef __DALI_INTERNAL_SCENE_GRAPH_MESH_ATTACHMENT_H__
#define __DALI_INTERNAL_SCENE_GRAPH_MESH_ATTACHMENT_H__

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

// INTERNAL INCLUDES
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/node-attachments/scene-graph-renderable-attachment.h>
#include <dali/internal/update/modeling/bone-transforms.h>
#include <dali/internal/event/actor-attachments/mesh-attachment-impl.h>
#include <dali/internal/event/actors/mesh-actor-impl.h>
#include <dali/public-api/geometry/mesh.h>
#include <dali/public-api/math/matrix.h>

namespace Dali
{

namespace Internal
{

class ResourceManager;

namespace SceneGraph
{
class Mesh;
class MeshRenderer;
class Material;
class Shader;

/**
 * An attachment for rendering a mesh with a material.
 */
class MeshAttachment : public RenderableAttachment, public PropertyOwner::Observer
{
public:

  typedef std::vector<Node*> BoneNodeContainer;

  struct MeshMatBones
  {
    MeshMatBones()
    : meshResourceId( 0 ),
      mesh( NULL ),
      material( NULL ),
      boneCount( 0 )
    {
    }

    ResourceId                  meshResourceId;
    SceneGraph::Mesh*           mesh;
    const SceneGraph::Material* material;
    std::size_t                 boneCount;
    BoneNodeContainer           boneNodes;
  };

  /**
   * Construct a new MeshAttachment.
   * @return a new MeshAttachment
   */
  static MeshAttachment* New();

  /**
   * Virtual destructor
   */
  virtual ~MeshAttachment();

  /**
   * TODO: remove geometry scaling and this method can be gone
   * @copydoc RenderableAttachment::ResolveVisibility
   */
  virtual bool ResolveVisibility( BufferIndex updateBufferIndex )
  {
    mHasSizeAndColorFlag = false;
    const Vector4& color = mParent->GetWorldColor( updateBufferIndex );
    if( color.a > FULLY_TRANSPARENT )               // not fully transparent
    {
      mHasSizeAndColorFlag = true;
    }
    return mHasSizeAndColorFlag;
  }

  /**
   * @copydoc RenderableAttachment::GetRenderer().
   */
  virtual Renderer& GetRenderer();

  /**
   * @copydoc RenderableAttachment::GetRenderer().
   */
  virtual const Renderer& GetRenderer() const;

  /**
   * @copydoc Dali::Internal::SceneGraph::RenderableAttachment::ShaderChanged()
   */
  virtual void ShaderChanged( BufferIndex updateBufferIndex );

  /**
   * @copydoc RenderableAttachment::SizeChanged()
   */
  virtual void SizeChanged( BufferIndex updateBufferIndex );

  /**
   * @copydoc Dali::Internal::SceneGraph::RenderableAttachment::DoPrepareRender()
   */
  virtual void DoPrepareRender( BufferIndex updateBufferIndex );

  /**
   * @copydoc RenderableAttachment::IsFullyOpaque()
   */
  virtual bool IsFullyOpaque( BufferIndex updateBufferIndex );

  /**
   * @copydoc Dali::Internal::SceneGraph::RenderableAttachment::DoGetScaleForSize()
   */
  virtual void DoGetScaleForSize( const Vector3& nodeSize, Vector3& scaling );

  /**
   * Set the mesh to be rendererd by this attachment
   * @param[in] meshId   The resource id of the mesh
   * @param[in] material The material
   * @param[in] boneCount The bone count
   */
  void SetMesh( ResourceId meshId, const SceneGraph::Material* material, std::size_t boneCount );

  /**
   * @copydoc Dali::Internal::MeshActor::SetMaterial(Material material)
   */
  void SetMaterial( const SceneGraph::Material* material );

  /**
   * Set the node for the given mesh index and bone index
   */
  void SetBoneNode( Node* boneNode, size_t boneIdx, size_t boneCount );

  /**
   * Calculate matrices for bones based on bone node transforms.
   */
  void CalculateBoneTransforms( BufferIndex               updateBufferIndex,
                                const Mesh*               mesh,
                                const BoneNodeContainer&  boneNodes,
                                BoneTransforms&           boneTransforms );

private:

  /**
    * @copydoc PropertyOwner::Observer::PropertyOwnerConnected()
    */
   virtual void PropertyOwnerConnected( PropertyOwner& owner ){}

  /**
   * @copydoc PropertyOwner::Observer::PropertyOwnerDisconnected()
   */
  virtual void PropertyOwnerDisconnected( BufferIndex bufferIndex, PropertyOwner& owner );

  /**
   * @copydoc PropertyOwner::Observer::PropertyOwnerDestroyed()
   */
  virtual void PropertyOwnerDestroyed( PropertyOwner& owner );

  /**
   * @copydoc RenderableAttachment::ConnectToSceneGraph2()
   */
  virtual void ConnectToSceneGraph2( BufferIndex updateBufferIndex );

  /**
   * @copydoc RenderableAttachment::OnDestroy2()
   */
  virtual void OnDestroy2();

  /**
   * @copydoc Dali::Internal::SceneGraph::RenderableAttachment::DoPrepareResources()
   */
  virtual bool DoPrepareResources( BufferIndex updateBufferIndex, ResourceManager& resourceManager );

  /**
   * Protected constructor. See New()
   */
  MeshAttachment();

  // Undefined
  MeshAttachment(const MeshAttachment&);

  // Undefined
  MeshAttachment& operator=(const MeshAttachment& rhs);

private:

  MeshRenderer* mRenderer; ///< Raw-pointer to renderer that is owned by RenderManager

  MeshMatBones  mMesh; ///< The mesh used to prepare for rendering, with associated materials and bones
};

// Messages for MeshAttachment

inline void SetMeshMessage( EventThreadServices& eventThreadServices, const MeshAttachment& attachment, ResourceId meshId, const Material* material, std::size_t boneCount )
{
  typedef MessageValue3< MeshAttachment, ResourceId, const Material*, std::size_t > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &MeshAttachment::SetMesh, meshId, material, boneCount );
}

inline void SetMaterialMessage( EventThreadServices& eventThreadServices, const MeshAttachment& attachment, const Material* material )
{
  typedef MessageValue1< MeshAttachment, const Material* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &MeshAttachment::SetMaterial, material );
}

inline void SetBoneNodeMessage( EventThreadServices& eventThreadServices, const MeshAttachment& attachment, Node* boneNode, size_t boneIdx, size_t boneCount )
{
  typedef MessageValue3< MeshAttachment, Node*, size_t, size_t > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &MeshAttachment::SetBoneNode, boneNode, boneIdx, boneCount );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_MESH_ATTACHMENT_H__
