#ifndef __DALI_MESH_H__
#define __DALI_MESH_H__

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
#include <dali/public-api/geometry/mesh-data.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/object/base-handle.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class Mesh;
}

/**
 * @brief A Mesh holds a MeshData object supplied by the application writer.
 *
 * It is used to tell the Scene Graph if the mesh data has changed.
 */
class Mesh : public BaseHandle
{
public:

  /**
   * @brief Create an initialized Mesh.
   *
   * @param[in] meshData The mesh data with which to initialize the handle
   * @return A handle to a newly allocated Dali resource.
   */
  static Mesh New( const MeshData& meshData );

  /**
   * @brief Create an uninitialized Mesh; this can be initialized with Mesh::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  Mesh();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Mesh();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

  /**
   * @brief Create an initialized plane aligned on the XY axis.
   *
   * @param[in] width   The width of the plane
   * @param[in] height  The height of the plane
   * @param[in] xSteps  The number of vertices along the X axis
   * @param[in] ySteps  The number of vertices along the Y axis
   * @param[in] textureCoordinates  UV coordinates.
   * @return A handle to a newly allocated Dali resource.
   */
  static Mesh NewPlane(const float width,
                       const float height,
                       const int xSteps,
                       const int ySteps,
                       const Rect<float>& textureCoordinates = Rect<float>(0.0f, 0.0f, 1.0f, 1.0f));

  /**
   * @brief Downcast an Object handle to Mesh handle.
   *
   * If handle points to a Mesh object the downcast produces valid
   * handle. If not the returned handle is left uninitialized.
   *
   * @param[in] handle to An object
   * @return handle to an Mesh object or an uninitialized handle
   */
  static Mesh DownCast( BaseHandle handle );

  /**
   * @brief Tell Dali to update it's internal meshes from the changed mesh data.
   *
   * @param[in] meshData
   */
  void UpdateMeshData( const MeshData& meshData );

public: // Not intended for application developers

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in] mesh A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL Mesh(Internal::Mesh* mesh);
};

} // namespace Dali

#endif // __DALI_MESH_H__
