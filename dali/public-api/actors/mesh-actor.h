#ifndef __DALI_MESH_ACTOR_H__
#define __DALI_MESH_ACTOR_H__

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
#include <dali/public-api/actors/renderable-actor.h>

namespace Dali DALI_IMPORT_API
{
class Material;
class Mesh;
class Model;
class AnimatableMesh;

namespace Internal DALI_INTERNAL
{
class MeshActor;
}

/**
 * @brief This actor is used to draw a mesh geometry with a material.
 *
 * It allows for a custom material to be drawn on the mesh.
 * By default CullFaceMode is set to CullBack to enable back face culling.
 */
class DALI_IMPORT_API MeshActor : public RenderableActor
{
public:

  /**
   * @brief Create an uninitialized MeshActor handle.
   *
   * This can be initialised with MeshActor::New().
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  MeshActor();

  /**
   * @brief Create a MeshActor object.
   * @return the newly allocated MeshActor object
   */
  static MeshActor New();

  /**
   * @brief Create a MeshActor object.
   *
   * @param[in] mesh the Mesh the actor will use.
   * @return the newly allocated MeshActor object
   */
  static MeshActor New( Mesh mesh );

  /**
   * @brief Create a MeshActor object
   *
   * @param[in] mesh the Animated Mesh the actor will use.
   * @return the newly allocated MeshActor object
   */
  static MeshActor New( AnimatableMesh mesh );

  /**
   * @brief Downcast an Object handle to MeshActor.
   *
   * If handle points to a MeshActor the downcast produces a valid
   * handle. If not the returned handle is left uninitialized.
   * @param[in] handle to an object
   * @return handle to a MeshActor or an uninitialized handle
   */
  static MeshActor DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~MeshActor();

  /**
   * @brief Copy constructor
   *
   * @param [in] copy The actor to copy.
   */
  MeshActor(const MeshActor& copy);

  /**
   * @brief Assignment operator
   *
   * @param [in] rhs The actor to copy.
   */
  MeshActor& operator=(const MeshActor& rhs);

  /**
   * @brief This method is defined to allow assignment of the NULL value,
   * and will throw an exception if passed any other value.
   *
   * Assigning to NULL is an alias for Reset().
   * @param [in] rhs  A NULL pointer
   * @return A reference to this handle
   */
  MeshActor& operator=(BaseHandle::NullType* rhs);

  /**
   * @brief Set a custom material on the given actor or one of it's children.
   *
   * @pre the mesh is loaded
   * @param[in] actor      The first actor in the tree of model actors
   * @param[in] actorName  The name of the actor to search for.
   * @param[in] material   The custom material.<br>Pass NULL to revert to the
   *                       original material.
   */
  static void SetMaterial(Actor actor, const std::string& actorName, Material material);

  /**
   * @brief Set a custom material on this actor.
   *
   * @pre the mesh is loaded
   * @param[in] material   The custom material.@n
   *                       Pass an uninitialised Material to revert to the original material.
   */
  void SetMaterial(Material material);

  /**
   * @brief Get the material for this mesh actor.
   *
   * @pre the mesh is loaded
   * @return The material
   */
  Material GetMaterial() const;

  /**
   * @brief Set whether this mesh actor should be affected by lights in the scene.
   *
   * If it is set to false, then the mesh will be unaffected by
   * lighting, and will be drawn with flat lighting, applying
   * the material's diffuse & ambient colors and the actor's color to the
   * material texture. If it is set to true, and there are no lights in the
   * scene, the mesh will not be drawn.
   *
   * @note This property is not inherited.
   * @note The default value is true.
   * @param[in] affectedByLighting Whether the actor should be lit by the scene lighting.
   */
  void SetAffectedByLighting(bool affectedByLighting);

  /**
   * @brief Get the lighting status.
   *
   * @return true if the actor is affected by the scene lighting, or false if its evenly lit.
   */
  bool IsAffectedByLighting();

  /**
   * @brief Search the actor tree for all named bones in the mesh and connect them.
   *
   * @param[in] rootActor The root actor of the actor tree.
   */
  void BindBonesToMesh(Actor rootActor);

public: // Not intended for application developers

  /**
   * @brief This constructor is used by Dali New() methods
   *
   * @param [in] actor A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL MeshActor(Internal::MeshActor* actor);
};

} // namespace Dali

#endif /* _MESH_ACTOR_H_ */
