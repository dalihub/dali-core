#ifndef __DALI_ANIMATABLE_MESH__H__
#define __DALI_ANIMATABLE_MESH__H__

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
 *
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/geometry/animatable-vertex.h>
#include <dali/public-api/modeling/material.h>

namespace Dali
{
class AnimatableVertex;

namespace Internal DALI_INTERNAL
{
class AnimatableMesh;
}

/**
 * @brief An animatable mesh can have any of its vertices animated using Dali's animation and
 * constraint systems.
 *
 * It is recommended that the vertices of the mesh remain in the bounds -0.5 - 0.5, which
 * will match the actor size boundaries. The origin of the mesh matches the actor's position.
 */
class DALI_IMPORT_API AnimatableMesh : public Handle
{
public:
  /**
   * @brief Vector of face indices.
   */
  typedef std::vector<unsigned short> Faces;
  typedef Faces::iterator             FacesIter;      ///< Iterator for Dali::AnimatableMesh::Faces
  typedef Faces::const_iterator       FacesConstIter; ///< Const Iterator for Dali::AnimatableMesh::Faces

  /**
   * @brief Create an uninitialized handle, this can be initialized with New().
   *
   * Calling member functions on an uninitialized handle will result
   * in an assertion
   */
  AnimatableMesh();

  /**
   * @brief Create a new animatable mesh with a given number of vertices and triangles.
   *
   * This will assert if any index is out of range.
   * Using this constructor enables the vertex color property.
   * AnimatableMesh does not take ownership of the faceIndices.
   * @param[in] numVertices The number of vertices in the mesh
   * @param[in] faceIndices A set of vertex indices, 3 per face.
   * @return an initialized handle to the animatable mesh
   * @note The maximum number of supported vertices is 3333333.
   */
  static AnimatableMesh New( unsigned int numVertices,
                             const Faces& faceIndices );

  /**
   * @brief Create a new animatable mesh with a given number of vertices and triangles.
   *
   * This will assert if any index is out of range.
   * Using this constructor disables the vertex color property.
   * AnimatableMesh does not take ownership of the faceIndices.
   * @param[in] numVertices The number of vertices in the mesh
   * @param[in] faceIndices A set of vertex indices, 3 per face.
   * @param[in] material Material used to render mesh
   * @return an initialized handle to the animatable mesh
   * @note The maximum number of supported vertices is 3333333.
   */
  static AnimatableMesh New( unsigned int numVertices,
                             const Faces& faceIndices,
                             Dali::Material material );

  /**
   * @brief Downcast an Object handle to AnimatableMesh.
   *
   * If the handle points to an AnimatableMesh objec, the downcast
   * produces a valid handle. If not, the handle is left
   * uninitialized.
   *
   * @param[in] handle to an Object
   * @return handle to an AnimatableMesh or an uninitialized handle
   */
  static AnimatableMesh DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~AnimatableMesh();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  AnimatableMesh(const AnimatableMesh& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  AnimatableMesh& operator=(const AnimatableMesh& rhs);

  /**
   * @brief Get the number of vertices with which this mesh was created.
   *
   * @return number of vertices in this mesh
   */
  unsigned int GetNumberOfVertices() const;

  /**
   * @brief Array subscript operator overload.
   *
   * @pre The vertex index is in range
   * @param[in] index subscript
   * @return    the vertex at the given index
   */
  AnimatableVertex operator[]( unsigned int index ) const;

  /**
   * @brief Get a property index for a given vertex.
   *
   * @pre The vertex index and property is in range
   * @param[in] vertex The vertex
   * @param[in] property The vertex attribute
   * @return A property index for use in constraints or animations
   */
  Property::Index GetPropertyIndex( unsigned int vertex, Property::Index property) const;

  /**
   * @brief Get the property for a given vertex.
   *
   * @pre The vertex index and property is in range
   * @param[in] vertex The vertex
   * @param[in] property The vertex attribute
   * @return A property index for use in constraints or animations
   */
  Property GetVertexProperty( unsigned int vertex, Property::Index property );

public: // Not for use by application developer

  /**
   * @brief This constructor is used by Dali New() methods
   *
   * @param [in] mesh A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL AnimatableMesh(Internal::AnimatableMesh* mesh);
};

}// Dali

#endif
