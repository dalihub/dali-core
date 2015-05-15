#ifndef __DALI_INTERNAL_MESH_ACTOR_H__
#define __DALI_INTERNAL_MESH_ACTOR_H__

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

// EXTERNAL INCLUDES
#include <cstddef>

// INTERNAL INCLUDES
#include <dali/public-api/object/handle.h>
#include <dali/devel-api/actors/mesh-actor.h>
#include <dali/devel-api/geometry/mesh.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/actors/actor-declarations.h>
#include <dali/internal/event/modeling/material-impl.h>
#include <dali/internal/event/actor-attachments/mesh-attachment-impl.h>
#include <dali/internal/event/actors/renderable-actor-impl.h>

namespace Dali
{
class Mesh;


namespace Internal
{

class MeshActor : public RenderableActor
{
public:

  /**
   * Create an un-intialised mesh actor
   * @return A smart-pointer to a newly allocated mesh actor
   */
  static MeshActorPtr New();

  /**
   * Create an intialised mesh actor from the given mesh
   * @param[in] mesh An object containing mesh data
   * @return A smart-pointer to a newly allocated mesh actor
   */
  static MeshActorPtr New(Dali::Mesh mesh);

    /**
   * Create an intialised mesh actor from the given mesh
   * @param[in] mesh An object containing mesh data
   * @return A smart-pointer to a newly allocated mesh actor
   */
  static MeshActorPtr New(Dali::AnimatableMesh mesh);

public:

  /**
   * Add a mesh to this actor.
   * @param[in] mesh A mesh structure
   */
  void SetMesh(Dali::Mesh mesh);

  /**
   * Add an animated mesh to this actor
   * @param[in] mesh An animatable mesh
   */
  void SetMesh(Dali::AnimatableMesh mesh);

  /**
   * Add a mesh resource to this actor
   * @param[in] mesh A mesh resource
   */
  void SetMesh(MeshIPtr meshPtr);

  /**
   * @copydoc Dali::MeshActor::SetMaterial(Material material)
   */
  void SetMaterial(const Dali::Material material);

  /**
   * Get a material for a given mesh.
   * @return The material
   */
  Dali::Material GetMaterial() const;

  /**
   * Search the actor tree for all named bones in the mesh array and connect them.
   * @param[in] rootActor - the root actor of the actor tree.
   */
  void BindBonesToMesh(Internal::ActorPtr rootActor);

private: // From RenderableActor

  /**
   * @copydoc RenderableActor::GetRenderableAttachment
   */
  virtual RenderableAttachment& GetRenderableAttachment() const
  {
    DALI_ASSERT_DEBUG( mMeshAttachment );
    return *mMeshAttachment;
  }

protected:
  /**
   * Protected constructor; see also MeshActor::New()
   */
  MeshActor();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~MeshActor();

private:
  Internal::MeshAttachmentPtr mMeshAttachment;
  // To keep a reference to the AnimatableMesh so it would not be destroyed before the MeshActor object
  Dali::Handle                mAnimatableMeshHandle;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::MeshActor& GetImplementation(Dali::MeshActor& meshActor)
{
  DALI_ASSERT_ALWAYS( meshActor && "MeshActor handle is empty" );

  BaseObject& handle = meshActor.GetBaseObject();

  return static_cast<Internal::MeshActor&>(handle);
}

inline const Internal::MeshActor& GetImplementation(const Dali::MeshActor& meshActor)
{
  DALI_ASSERT_ALWAYS( meshActor && "MeshActor handle is empty" );

  const BaseObject& handle = meshActor.GetBaseObject();

  return static_cast<const Internal::MeshActor&>(handle);
}

} // namespace Internal

#endif // __DALI_INTERNAL_MESH_ACTOR_H__
