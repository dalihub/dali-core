#ifndef DALI_GEOMETRY_H
#define DALI_GEOMETRY_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/property-index-ranges.h> // DEFAULT_DERIVED_HANDLE_PROPERTY_START_INDEX
#include <dali/public-api/rendering/property-buffer.h> // Dali::PropertyBuffer

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class Geometry;
}

/**
 * @brief Geometry is handle to an object that can be used to define a geometric elements.
 *
 * @SINCE_1_1.43
 */
class DALI_IMPORT_API Geometry : public BaseHandle
{
public:

  /**
   * @brief Describes the type of geometry, used to determine how the coordinates will be used.
   * @SINCE_1_1.45
   */
  enum Type
  {
    POINTS,        ///< Individual points                                                                                          @SINCE_1_1.45
    LINES,         ///< Individual lines (made of 2 points each)                                                                   @SINCE_1_1.45
    LINE_LOOP,     ///< A strip of lines (made of 1 point each) which also joins the first and last point                          @SINCE_1_1.45
    LINE_STRIP,    ///< A strip of lines (made of 1 point each)                                                                    @SINCE_1_1.45
    TRIANGLES,     ///< Individual triangles (made of 3 points each)                                                               @SINCE_1_1.45
    TRIANGLE_FAN,  ///< A fan of triangles around a centre point (after the first triangle, following triangles need only 1 point) @SINCE_1_1.45
    TRIANGLE_STRIP ///< A strip of triangles (after the first triangle, following triangles need only 1 point)                     @SINCE_1_1.45
  };


  /**
   * @brief Creates a new Geometry object
   *
   * @SINCE_1_1.43
   * @return A handle to a newly allocated Geometry object
   */
  static Geometry New();

  /**
   * @brief Default constructor, creates an empty handle
   *
   * @SINCE_1_1.43
   */
  Geometry();

  /**
   * @brief Destructor
   *
   * @SINCE_1_1.43
   */
  ~Geometry();

  /**
   * @brief Copy constructor, creates a new handle to the same object
   *
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   */
  Geometry( const Geometry& handle );

  /**
   * @brief Downcast to a geometry.
   * If not the returned handle is left uninitialized.
   *
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   * @return geometry Handle or an uninitialized handle
   */
  static Geometry DownCast( BaseHandle handle );

  /**
   * @brief Assignment operator, changes this handle to point at the same object
   *
   * @SINCE_1_1.43
   * @param[in] handle Handle to an object
   * @return Reference to the assigned object
   */
  Geometry& operator=( const Geometry& handle );

  /**
   * @brief Add a PropertyBuffer to be used as source of geometry vertices
   *
   * @SINCE_1_1.43
   * @param[in] vertexBuffer PropertyBuffer to be used as source of geometry vertices
   * @return Index of the newly added buffer, can be used with RemoveVertexBuffer to remove
   *         this buffer if no longer required
   */
  std::size_t AddVertexBuffer( PropertyBuffer& vertexBuffer );

  /**
   * @brief Retrieve the number of vertex buffers that have been added to this geometry
   *
   * @SINCE_1_1.43
   * @return Number of vertex buffers that have been added to this geometry
   */
  std::size_t GetNumberOfVertexBuffers() const;

  /**
   * @brief Remove a vertex buffer
   * The index must be between 0 and GetNumberOfVertexBuffers()
   *
   * @SINCE_1_1.43
   * @param[in] index Index to the vertex buffer to remove
   */
  void RemoveVertexBuffer( std::size_t index );

  /**
   * @brief Set a the index data to be used as a source of indices for the geometry
   * Setting this buffer will cause the geometry to be rendered using indices.
   * To unset call SetIndexBuffer with a null pointer or count 0
   *
   * @SINCE_1_1.43
   * @param[in] indices Array of indices
   * @param[in] count Number of indices in the array
   */
  void SetIndexBuffer( const unsigned short* indices, size_t count );

  /**
   * @brief Set the type of primitives this geometry contains
   *
   * @SINCE_1_1.43
   * @param[in] geometryType Type of primitives this geometry contains
   */
  void SetType( Type geometryType );

  /**
   * @brief Get the type of primitives this geometry contains
   * Calling this function sets the property GEOMETRY_TYPE
   *
   * @SINCE_1_1.43
   * @return Type of primitives this geometry contains
   */
  Type GetType() const;

public:

  /**
   * @brief The constructor
   * @note  Not intended for application developers.
   * @SINCE_1_1.43
   * @param[in] pointer A pointer to a newly allocated Geometry
   */
  explicit DALI_INTERNAL Geometry( Internal::Geometry* pointer );
};

} //namespace Dali

#endif // DALI_GEOMETRY_H
