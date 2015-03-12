#ifndef DALI_GEOMETRY_H
#define DALI_GEOMETRY_H

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

// EXTERNAL INCLUDES
#include <cstddef> // std::size_t

// INTERNAL INCLUDES
#include <dali/public-api/object/handle.h> // Dali::Handle
#include <dali/public-api/object/property-buffer.h> // Dali::PropertyBuffer
#include <dali/public-api/object/property-index-ranges.h> // DEFAULT_DERIVED_HANDLE_PROPERTY_START_INDEX

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class Geometry;
}

/**
 * @brief Geometry is handle to an object that can be used to define a geometric elements.
 */
class DALI_IMPORT_API Geometry : public Handle
{
public:

  enum GeometryType
  {
    POINTS,
    LINES,
    LINE_LOOP,
    LINE_STRIP,
    TRIANGLES,
    TRIANGLE_FAN,
    TRIANGLE_STRIP
  };

  /**
   * @brief An enumeration of properties belonging to the Geometry class.
   */
  struct Property
  {
    enum
    {
      GEOMETRY_TYPE = DEFAULT_DERIVED_HANDLE_PROPERTY_START_INDEX,  ///< name "depth-index",            type STRING
      GEOMETRY_CENTER,                                              ///< name "geometry-center",        type VECTOR3
      GEOMETRY_HALF_EXTENTS,                                        ///< name "geometry-half-extents",  type VECTOR3
      REQUIRES_DEPTH_TEST,                                          ///< name "requires-depth-testing", type BOOLEAN
    };
  };

  /**
   * @brief Creates a new Geometry object
   *
   * @return An handle to a newly allocated Geometry
   */
  static Geometry New();

  /**
   * @brief Default constructor, creates an empty handle
   */
  Geometry();

  /**
   * @brief Destructor
   */
  ~Geometry();

  /**
   * @brief Copy constructor, creates a new handle to the same object
   *
   * @param[in] handle Handle to an object
   */
  Geometry( const Geometry& handle );

  /**
   * @brief Assignment operator, changes this handle to point at the same object
   *
   * @param[in] handle Handle to an object
   * @return Reference to the assigned object
   */
  Geometry& operator=( const Geometry& handle );

  /**
   * @brief Add a PropertyBuffer to be used as source of geometry vertices
   *
   * @param[in] vertexBuffer PropertyBuffer to be used as source of geometry vertices
   * @return Index of the newly added buffer, can be used with RemoveVertexBuffer to remove
   *         this buffer if no longer required
   */
  std::size_t AddVertexBuffer( PropertyBuffer& vertexBuffer );

  /**
   * @brief Retrieve the number of vertex buffers that have been added to this geometry
   *
   * @return Number of vertex buffers that have been added to this geometry
   */
  std::size_t GetNumberOfVertexBuffers() const;

  /**
   * @brief Remove a vertex buffer
   *
   * The index must be between 0 and GetNumberOfVertexBuffers()
   *
   * @param[in] index Index to the vertex buffer to remove
   */
  void RemoveVertexBuffer( std::size_t index );

  /**
   * @brief Set a PropertyBuffer to be used as a source of indices for the geometry
   *
   * This buffer is required to have exactly one component and it must be of the type Property::INTEGER
   *
   * By setting this buffer the will case the geometry to be rendered using indices.
   * To unset call SetIndexBuffer with an empty handle.
   *
   * @param[in] indexBuffer PropertyBuffer to be used as a source of indices for the geometry
   */
  void SetIndexBuffer( PropertyBuffer& indexBuffer );

  /**
   * @brief Set the type of primitives this geometry contains
   *
   * @param[in] geometryType Type of primitives this geometry contains
   */
  void SetGeometryType( GeometryType geometryType );

  /**
   * @brief Get the type of primitives this geometry contains
   *
   * Calling this function sets the property GEOMETRY_TYPE
   *
   * @return Type of primitives this geometry contains
   */
  GeometryType GetGeometryType() const;

  /**
   * @brief Set if the geometry requires depth testing
   *
   * Should be set to true if this geometry has overlapping triangles in arbitrary order
   *
   * Calling this function is equivalent to setting the REQUIRES_DEPTH_TESTING property
   *
   * @param[in] requiresDepthTest Specifies if the geometry requires depth testing
   */
  void SetRequiresDepthTesting( bool requiresDepthTest );

  /**
   * @brief Return if the geometry requires depth testing
   *
   * @return True if the geometry requires depth testing
   */
  bool GetRequiresDepthTesting() const;

public:
  /**
   * @brief The constructor
   *
   * @param [in] pointer A pointer to a newly allocated Geometry
   */
  explicit DALI_INTERNAL Geometry( Internal::Geometry* pointer );
};

} //namespace Dali

#endif // DALI_GEOMETRY_H
