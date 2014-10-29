#ifndef __DALI_ENTITY_H__
#define __DALI_ENTITY_H__

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

namespace Dali
{

class Entity;
class Matrix;
class Mesh;
class ModelData;
struct Vector4;

namespace Internal DALI_INTERNAL
{
class Entity;
}

typedef std::vector<Entity>             EntityContainer;    ///< Container of Entity handles
typedef EntityContainer::iterator       EntityIter;         ///< iterator for Entity container
typedef EntityContainer::const_iterator EntityConstIter;    ///< const_iterator for Entity container
typedef std::vector<unsigned int>       EntityMeshIndices;  ///< Container to hold indexes of meshes this entity uses in the associated Dali::Model.

/**
 * @brief A 3D model comprises a tree of named entities. Each entity has a
 * transform, regardless of type.
 *
 * It can contain zero or more meshes.  The entity names are used for
 * cross referencing, e.g. from animations or from bone structures.
 */
class DALI_IMPORT_API Entity : public BaseHandle
{
public:

  /**
   * @brief The entity type
   */
  enum EntityType
  {
    OBJECT, ///< A generic entity
    CAMERA, ///< The entity represents a camera
    LIGHT,  ///< The entity represents a light
  };

  /**
   * @brief Create an uninitialized Entity; this can be initialized with Entity::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  Entity();

  /**
   * @brief Create an initialized handle.
   *
   * @param[in] name The name of the entity
   * @return a handle to a newly allocated Dali resource
   */
  static Entity New(const std::string name);

  /**
   * @brief Downcast an Object handle to Entity handle.
   *
   * If handle points to a Entity object the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle to An object
   * @return handle to a Entity object or an uninitialized handle
   */
  static Entity DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Entity();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  Entity(const Entity& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  Entity& operator=(const Entity& rhs);

  /**
   * @brief This method is defined to allow assignment of the NULL value,
   * and will throw an exception if passed any other value.
   *
   * Assigning to NULL is an alias for Reset().
   * @param [in] rhs  A NULL pointer
   * @return A reference to this handle
   */
  Entity& operator=(BaseHandle::NullType* rhs);

  /**
   * @brief Find an entity by name.
   *
   * The search includes this entity and it's children.
   * @param[in] name The search name.
   * @return    A handle to the searched entity, or an uninitialized handle if not found.
   */
  Entity Find(const std::string& name) const;

  /**
   * @brief Set the name of the entity.
   *
   * @param[in] name The name for the entity.
   */
  void SetName(const char* name);

  /**
   * @copydoc SetName(const char*)
   */
  void SetName(const std::string& name);

  /**
   * @brief Get the name of the entity.
   *
   * @return The name of the entity.
   */
  const std::string& GetName() const;

  /**
   * @brief Set the entity type.
   *
   * @param[in] type The entity's type, a member of the EntityType enumeration.
   */
  void SetType(const EntityType type);

  /**
   * @brief Get the entity's type.
   *
   * @return The entity's type, a member of the EntityType enumeration.
   */
  EntityType GetType() const;

  /**
   * @brief Set the transform matrix.
   *
   * @param[in] matrix The transform matrix
   */
  void SetTransformMatrix(Matrix& matrix);

  /**
   * @brief Returns a reference to the transform matrix.
   *
   * @return The transform matrix
   */
  const Matrix& GetTransformMatrix() const;

  /**
   * @brief Checks if the entity has children.
   *
   * @return true if the entity has children, otherwise false.
   */
  bool HasChildren() const;

  /**
   * @brief Get the number of children.
   *
   * @return The number of children.
   */
  int NumberOfChildren() const;

  /**
   * @brief Get the list of children.
   *
   * @return A reference to the list of children.
   */
  const EntityContainer& GetChildren() const;

  /**
   * @brief Add an entity to the end of the child list.
   *
   * Sets the child's parent.
   * @param[in] child A handle to a child Entity.
   */
  void Add(Entity child);

  /**
   * @brief Get the entity's parent.
   *
   * @return A pointer to the entity's parent.
   */
  Entity GetParent() const;

  /**
   * @brief Change the size of storage allocated to the mesh index list.
   *
   * This may be greater than the number of indices stored as returned
   * by NumberOfMeshes.
   *
   * @param[in] capacity The new size of the storage allocated to the mesh
   * index list.
   */
  void SetMeshCapacity(unsigned int capacity);

  /**
   * @brief Add a mesh to the entity.
   *
   * @param[in] meshIdx The mesh index into the model data's list of meshes
   */
  void AddMeshIndex(unsigned int meshIdx);

  /**
   * @brief Checks if the entity references any meshes.
   *
   * @return true if the entity refers to 1 or more meshes, otherwise false.
   */
  bool HasMeshes() const;

  /**
   * @brief Get the number of meshes referenced by this entity.
   *
   * @return The number of meshes referenced by this entity.
   */
  int NumberOfMeshes() const;

  /**
   * @brief Returns a mesh in the model's mesh array.
   *
   * @param[in] meshIndex The index of the mesh within the entity.
   * @return The index of the mesh in the model's mesh array.
   */
  unsigned int GetMeshByIndex(unsigned int meshIndex) const;

  /**
   * @brief Get the entity's list of mesh indices.
   *
   * @return The entity's list of mesh indices.
   */
  const EntityMeshIndices& GetMeshes() const;

  /**
   * @brief Expand the bounding volume to include the child's bounding volume.
   *
   * @param[in] child A handle to the child.
   */
  void AddToBounds(Entity child);

  /**
   * @brief Expand the entity bounding box to include the new boundaries.
   *
   * @param[in] lowerBounds to extend the entity bounds
   * @param[in] upperBounds to extend the entity bounds
   */
   void AddToBounds( const Vector3& lowerBounds, const Vector3& upperBounds );

  /**
   * @brief Get the lower bounds of the bounding volume which contains all vertices in all meshes
   * referenced by this entity.
   *
   * @return A vector describing the lower bounds.
   */
  const Vector3&  GetLowerBounds() const;

  /**
   * @brief Set the lower bounds of the bounding volume which contains all vertices in all meshes
   * referenced by this entity.
   *
   * @param[in] lowerBounds of the entity.
   */
  void SetLowerBounds( const Vector3& lowerBounds );

  /**
   * @brief Get the upper bounds of the bounding volume which contains all vertices in all meshes
   * referenced by this entity.
   *
   * @return A vector describing the upper bounds.
   */
  const Vector3&  GetUpperBounds() const;

  /**
   * @brief Set the upper bounds of the bounding volume which contains all vertices in all meshes
   * referenced by this entity.
   *
   * @param[in] upperBounds of the entity.
   */
  void SetUpperBounds( const Vector3& upperBounds );

public: // Not intended for application developers

  /**
   * @brief This constructor is used by Dali New() methods.
   * @param [in] entity A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL Entity(Internal::Entity* entity);

}; // class Entity

} // namespace Dali

#endif // __DALI_ENTITY_H__
