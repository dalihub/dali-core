#ifndef __DALI_INTERNAL_MESH_ATTACHMENT_H__
#define __DALI_INTERNAL_MESH_ATTACHMENT_H__

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
#include <dali/public-api/geometry/mesh.h>
#include <dali/public-api/modeling/material.h>
#include <dali/internal/common/owner-container.h>
#include <dali/internal/event/actors/actor-declarations.h>
#include <dali/internal/event/actor-attachments/actor-attachment-declarations.h>
#include <dali/internal/event/actor-attachments/renderable-attachment-impl.h>
#include <dali/internal/event/modeling/mesh-impl.h>
#include <dali/internal/event/modeling/material-impl.h>
#include <dali/internal/event/resources/resource-ticket.h>
#include <dali/internal/event/common/proxy-object.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
class MeshAttachment;
class Node;
}

typedef std::vector<std::string>               BoneNames;
typedef BoneNames::const_iterator              BoneNamesIter;

/**
 * An attachment for rendering a 3D mesh with a material.
 */
class MeshAttachment : public RenderableAttachment
{
public:

  struct MeshAndCustomMaterial
  {
    Internal::MeshIPtr          mMesh;
    Internal::MaterialIPtr      mMaterial;
    Internal::MaterialIPtr      mCustomMaterial;
    BoneNames                   mBoneNames;
  };

  /**
   * Create an initialised MeshAttachment.
   * @param[in] parentNode The node to attach a scene-object to.
   * This will not be displayed until a mesh is added with SetMesh().
   * @return A handle to a newly allocated Dali resource.
   */
  static MeshAttachmentPtr New( const SceneGraph::Node& parentNode );

  /**
   * Set the mesh to be rendererd by this attachment
   * @param[in] meshPtr  A mesh impl (may be null)
   * @param[in] meshId   The resource id of the mesh
   * @param[in] bones    Container of bones for this mesh
   * @param[in] material The material
   */
  void SetMesh( const MeshIPtr        meshPtr,
                ResourceId            meshId,
                const BoneContainer&  bones,
                const MaterialIPtr    material );

  /**
   * Add a mesh to the collection of meshes being rendered by this attachment
   * @param [in] meshResource A mesh resource
   * @param[in] bones    Container of bones for this mesh
   * @param [in] material Material
   */
  void SetMesh( const ResourceTicketPtr     meshResource,
                const BoneContainer&  bones,
                const MaterialIPtr    material );

  /**
   * Set the material of the mesh
   * @param[in] materialPtr - a custom material to apply to this mesh
   */
  void SetMaterial( Internal::MaterialIPtr materialPtr );

  /**
   * Get the material of the mesh
   * @return The internal material pointer
   */
  Internal::MaterialIPtr GetMaterial( ) const;

  /**
   * Disconnect any connected material from the mesh
   */
  void DisconnectMaterial( );

  /**
   * Set whether this mesh actor should be affected by lights in the
   * scene.  If it set to false, then the mesh will be unaffected by
   * lighting, and will be evenly lit without any shading.
   * This property is not inherited.
   * @param[in] affectedByLighting Whether the actor should be lit by the scene lighting.
   */
  void SetAffectedByLighting( bool affectedByLighting );

  /**
   * @return true if the actor is lit by the scene, or false if evenly lit.
   */
  bool IsAffectedByLighting();

  /**
   * Search the actor tree for all named bones in the mesh  and connect them.
   * @param[in] rootActor - the root actor of the actor tree.
   */
  void BindBonesToMesh( Internal::ActorPtr rootActor );

  /**
   * Connects a boneActor to the mesh (allows the mesh renderer to do
   * skeletal animation by calculating the actor transforms)
   * @param[in] boneActor - the actor containing the bone's current position
   * @param[in] boneIdx   - the index of the bone in this mesh
   * @param[in] boneCount - the bone count
   */
  void ConnectBoneActor( Internal::ActorPtr boneActor, size_t boneIdx, size_t boneCount );

  const MeshAndCustomMaterial& GetMesh() const
  {
    return mMesh;
  }

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~MeshAttachment();

private:

  /**
   * First stage construction of a MeshAttachment.
   * @param[in] stage Used to send messages to scene-graph.
   */
  MeshAttachment( Stage& stage );

  /**
   * @copydoc Dali::Internal::RenderableAttachment::OnStageConnection2()
   */
  virtual void OnStageConnection2();

  /**
   * @copydoc Dali::Internal::RenderableAttachment::OnStageDisconnection2()
   */
  virtual void OnStageDisconnection2();

  /**
   * @copydoc Dali::Internal::RenderableAttachment::GetSceneObject()
   */
  virtual const SceneGraph::RenderableAttachment& GetSceneObject() const;

protected:
  /**
   * Set the node for given bone index in the scene graph object
   * @param[in] node The node to set
   * @param[in] boneIdx The bone index
   */
  void SetBoneNode( SceneGraph::Node* node, size_t boneIdx );

  /**
   * Helper class for connecting Nodes to the scene-graph MeshAttachment
   */
  class Connector : public ProxyObject::Observer
  {
  public:
    /**
     * Create the helper class
     */
    Connector( Internal::ActorPtr boneActor, size_t boneIdx, MeshAttachment& meshAttachment);

    /**
     * Copy constructor required for vector storage
     */
    Connector(const Connector&);

    /**
     * Assignment constructor required for vector storage
     */
    Connector& operator=(const Connector& rhs);

    /**
     * Destructor
     */
    ~Connector();

    /**
     * Update the scene-graph mesh attachment with this node
     */
    void ConnectNode();

  public: // From ProxyObject::Observer

    /**
     * @copydoc Dali::Internal::ProxyObject::Observer::SceneObjectAdded
     */
    virtual void SceneObjectAdded( ProxyObject& proxy );

    /**
     * @copydoc Dali::Internal::ProxyObject::Observer::SceneObjectAdded
     */
    virtual void SceneObjectRemoved( ProxyObject& proxy );

    /**
     * @copydoc Dali::Internal::ProxyObject::Observer::ProxyDestroyed
     */
    virtual void ProxyDestroyed( ProxyObject& proxy );

  public:
    MeshAttachment& mMeshAttachment; ///< Parent
    Actor* mActor;   ///< Raw-pointer to the actor; not owned.
    size_t mBoneIdx; ///< Index of the bone with which this connecter is associated
  };
  typedef OwnerContainer< Connector* > ConnectorList;

private:

  const SceneGraph::MeshAttachment* mSceneObject; ///< Not owned

  MeshAndCustomMaterial mMesh;
  bool                  mAffectedByLighting;
  ConnectorList         mConnectors;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_MESH_ATTACHMENT_H__
