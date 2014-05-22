#ifndef __DALI_INTERNAL_MODEL_ENTITY_H__
#define __DALI_INTERNAL_MODEL_ENTITY_H__

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
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/geometry/mesh.h>
#include <dali/public-api/math/matrix.h>
#include <dali/public-api/modeling/entity.h>
#include <dali/public-api/object/base-object.h>

namespace Dali
{
namespace Internal
{
class ModelData;
class Entity;

typedef IntrusivePtr<Entity>    EntityPtr;          ///< smart pointer to an Internal::Entity
typedef Dali::EntityContainer           EntityContainer;    ///< Handles to entities
typedef EntityContainer::iterator       EntityIter;         ///< iterator for Entity container
typedef EntityContainer::const_iterator EntityConstIter;    ///< const_iterator for Entity container
typedef Dali::EntityMeshIndices         EntityMeshIndices;


class Entity : public BaseObject
{
public:
  /**
   * Constructor
   */
  Entity();

  static EntityPtr New(const std::string name);

private:
  // Unimplemented copy contructor/assignment operator
  Entity(const Entity& rhs);
  Entity& operator=(const Entity& rhs);

  /**
   * Destructor - A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Entity();

public:
  /**
   * Find an entity by name. The search includes this entity and it's children.
   * @param[in] name The search name.
   * @return    A pointer to an entity or NULL.
   */
  Entity* Find(const std::string& name) const;

  /**
   * Set the name of the entity.
   * @param[in] name The name for the entity.
   */
  void SetName(const char* name);

  /**
   * @copydoc SetName(const char*)
   */
  void SetName(const std::string& name);

  /**
   * Get the name of the entity.
   * @return The name of the entity.
   */
  const std::string& GetName() const;

  /**
   * Get the lower bounds of the bounding volume which contains all vertices in all meshes
   * referenced by this entity.
   * @return A vector describing the lower bounds.
   */
  const Vector3&  GetLowerBounds() const;

  /**
   * Set the lower bounds of the bounding volume which contains all vertices in all meshes
   * referenced by this entity.
   * @param[in] bounds The precalculated bounds (usually read from a binary format model file)
   */
  void SetLowerBounds(const Vector3& bounds);

  /**
   * Get the upper bounds of the bounding volume which contains all vertices in all meshes
   * referenced by this entity.
   * @return A vector describing the upper bounds.
   */
  const Vector3&  GetUpperBounds() const;

  /**
   * Set the upper bounds of the bounding volume which contains all vertices in all meshes
   * referenced by this entity.
   * @param[in] bounds The precalculated bounds (usually read from a binary format model file)
   */
  void SetUpperBounds(const Vector3& bounds);

  /**
   * Set the entity type.
   * @param type  The entity's type, a member of the EntityType enumeration.
   */
  void SetType(const Dali::Entity::EntityType type);

  /**
   * Get the entity's type.
   * @return The entity's type, a member of the EntityType enumeration.
   */
  Dali::Entity::EntityType GetType() const;

  /**
   * Get the entity's transform matrix.
   * @return The entity's transform matrix.
   */
  void SetTransformMatrix(Matrix& matrix);

  /**
   * @copydoc Dali::Entity::GetTransformMatrix()
   */
  const Matrix& GetTransformMatrix() const;

  /**
   * Checks if the entity has children
   * @return true if the entity has children, otherwise false.
   */
  bool HasChildren() const;

  /**
   * Get the number of children.
   * @return The number of children.
   */
  int NumberOfChildren() const;

  /**
   * Get the list of children.
   * @return A reference to the list of children.
   */
  const EntityContainer& GetChildren() const;

  /**
   * Add a child to the end of the child list
   * @param[in] child A pointer to an Entity.
   */
  void Add(Entity& child);

  /**
   * Change the size of storage allocated to the entity's child list. This may be greater than the number of
   * children stored as returned by NumberOfChildren.
   * @param[in] capacity The new size of the storage allocated to the child list.
   */
  void SetChildCapacity(unsigned int capacity);

  /**
   * Set the entity's parent.
   * @param[in] parent A pointer to the entity's parent.
   */
  void SetParent(EntityPtr parent);

  /**
   * Get the entity's parent.
   * @return A pointer to the entity's parent.
   */
  Entity* GetParent() const;

  /**
   * Checks if the entity references any meshes.
   * @return true if the entity refers to 1 or more meshes, otherwise false.
   */
  bool HasMeshes() const;

  /**
   * Change the size of storage allocated to the mesh index list. This may be greater than the number of
   * indices stored as returned by NumberOfMeshes.
   * @param[in] capacity The new size of the storage allocated to the mesh index list.
   */
  void SetMeshCapacity(unsigned int capacity);

  /**
   * Get the number of meshes referenced by this entity.
   * @return The number of meshes referenced by this entity.
   */
  int NumberOfMeshes() const;

  /**
   * Add a mesh to the entity.
   * @param[in] meshIdx The mesh index into the ModelData's list of meshes
   */
  void AddMeshIndex(unsigned int meshIdx);

  /**
   * Returns a mesh in the model's mesh array.
   * @param[in] meshIndex The index of the mesh within the entity.
   * @return         The index of the mesh in the ModelData's mesh array.
   */
  unsigned int GetMeshByIndex(unsigned int meshIndex) const;

  /**
   * Expand the bounding volume to include the child's bounding volume.
   * @param[in] child A pointer to the child.
   */
  void AddToBounds(Entity& child);

  /**
   * Expand the entity bounding box to include the new boundaries.
   * @param[in] lowerBounds to extend the entity bounds
   * @param[in] upperBounds to extend the entity bounds
   */
   void AddToBounds( const Vector3& lowerBounds, const Vector3& upperBounds );

  /**
   * Get the entity's list of mesh indices.
   * @return The entity's list of mesh indices.
   */
  const EntityMeshIndices& GetMeshes() const;

private:
  /**
   * Set our parent, with assert on failure
   */
  void SetParent(Entity* parent);


private:
  std::string               mName;            ///< name of Entity
  ModelData*                mModel;           ///< owning model
  Entity*                   mParent;          ///< parent entity or false if root of the scene
  EntityContainer           mChildren;        ///< List of child entities
  EntityMeshIndices         mMeshes;          ///< List of meshes for this Entity
  Matrix                    mTransformMatrix; ///< Transform matrix
  Vector3                   mLowerBounds;     ///< The lower bounds of the bounding volume
  Vector3                   mUpperBounds;     ///< The upper bounds of the bounding volume
  Dali::Entity::EntityType  mType;            ///< The entity's type
}; // class Entity

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::Entity& GetImplementation(Dali::Entity& entity)
{
  DALI_ASSERT_ALWAYS(entity && "Entity handle is empty" );

  BaseObject& handle = entity.GetBaseObject();

  return static_cast<Internal::Entity&>(handle);
}

inline const Internal::Entity& GetImplementation(const Dali::Entity& entity)
{
  DALI_ASSERT_ALWAYS(entity && "Entity handle is empty" );

  const BaseObject& handle = entity.GetBaseObject();

  return static_cast<const Internal::Entity&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_MODEL_ENTITY_H__
