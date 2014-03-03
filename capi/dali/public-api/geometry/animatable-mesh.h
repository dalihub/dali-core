#ifndef __DALI_ANIMATABLE_MESH__H__
#define __DALI_ANIMATABLE_MESH__H__

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

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/object/constrainable.h>
#include <dali/public-api/geometry/animatable-vertex.h>
#include <dali/public-api/modeling/material.h>

namespace Dali DALI_IMPORT_API
{
class AnimatableVertex;

namespace Internal DALI_INTERNAL
{
class AnimatableMesh;
}

/**
 * An animatable mesh can have any of its vertices animated using Dali's animation and
 * constraint systems.
 * It is recommended that the vertices of the mesh remain in the bounds -0.5 - 0.5, which
 * will match the actor size boundaries. The origin of the mesh matches the actor's position.
 */
class AnimatableMesh : public Constrainable
{
public:
  typedef std::vector<unsigned short> Faces;
  typedef Faces::iterator             FacesIter;
  typedef Faces::const_iterator       FacesConstIter;

  /**
   * Create an uninitialized handle, this can be initialized with New().
   * Calling member functions on an uninitialized handle will result
   * in an assertion
   */
  AnimatableMesh();

  /**
   * Create a new animatable mesh with a given number of vertices
   * and triangles. This will assert if any index is out of range.
   * Using this constructor enables the vertex color property.
   * AnimatableMesh does not take ownership of the faceIndices.
   * @param[in] numVertices The number of vertices in the mesh
   * @param[in] faceIndices A set of vertex indices, 3 per face.
   * @return an initialized handle to the animatable mesh
   */
  static AnimatableMesh New( unsigned int numVertices,
                             const Faces& faceIndices );

  /**
   * Create a new animatable mesh with a given number of vertices
   * and triangles. This will assert if any index is out of range.
   * Using this constructor disables the vertex color property.
   * AnimatableMesh does not take ownership of the faceIndices.
   * @param[in] numVertices The number of vertices in the mesh
   * @param[in] faceIndices A set of vertex indices, 3 per face.
   * @param[in] material Material used to render mesh
   * @return an initialized handle to the animatable mesh
   */
  static AnimatableMesh New( unsigned int numVertices,
                             const Faces& faceIndices,
                             Dali::Material material );

  /**
   * Downcast an Object handle to AnimatableMesh. If the handle points
   * to an AnimatableMesh objec, the downcast produces a valid
   * handle. If not, the handle is left uninitialized.
   *
   * @param[in] handle to an Object
   * @return handle to an AnimatableMesh or an uninitialized handle
   */
  static AnimatableMesh DownCast( BaseHandle handle );

  /**
   * Destructor
   */
  virtual ~AnimatableMesh();

  /**
   * Get the number of vertices with which this mesh was created.
   * @return number of vertices in this mesh
   */
  unsigned int GetNumberOfVertices() const;

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

  /**
   * Array subscript operator overload.
   *
   * @pre The vertex index is in range
   * @param[in] index subscript
   * @return    the vertex at the given index
   */
  AnimatableVertex operator[]( unsigned int index ) const;

  /**
   * Get a property index for a given vertex
   * @pre The vertex index and property is in range
   * @param[in] vertex The vertex
   * @param[in] property The vertex attribute
   * @return A property index for use in constraints or animations
   */
  Property::Index GetPropertyIndex( unsigned int vertex, Property::Index property) const;

  /**
   * Get the property for a given vertex.
   *
   * @pre The vertex index and property is in range
   * @param[in] vertex The vertex
   * @param[in] property The vertex attribute
   * @return A property index for use in constraints or animations
   */
  Property GetVertexProperty( unsigned int vertex, Property::Index property );

public: // Not for use by application developer

  /**
   * This constructor is used by Dali New() methods
   * @param [in] mesh A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL AnimatableMesh(Internal::AnimatableMesh* mesh);
};

}// Dali

/**
 * @}
 */
#endif
