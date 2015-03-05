#ifndef DALI_INTERNAL_SCENE_GRAPH_GEOMETRY_H
#define DALI_INTERNAL_SCENE_GRAPH_GEOMETRY_H

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
 */

#include <dali/public-api/geometry/geometry.h>
#include <dali/internal/update/common/double-buffered.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/property-buffer.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

/**
 * This scene graph object is a property owner. It describes a geometry using a
 * number of PropertyBuffers acting as Vertex buffers.
 */
class Geometry : public PropertyOwner
{
public:
  typedef Dali::Geometry::GeometryType GeometryType;

  typedef OwnerContainer< PropertyBuffer* > VertexBuffers;

  /**
   * Constructor
   */
  Geometry();

  /**
   * Destructor
   */
  ~Geometry();

  /**
   * Add a property buffer to be used as a vertex buffer
   */
  void AddVertexBuffer( PropertyBuffer vertexBuffer );

  /**
   * Remove a property buffer to be used as a vertex buffer
   * @param[in] vertexBuffer the associated vertex buffer to remove
   */
  void RemoveVertexBuffer( PropertyBuffer vertexBuffer );

  /**
   * Set the buffer to be used as a source of indices for the geometry
   * @param[in] indexBuffer the Property buffer describing the indexes for Line, Triangle tyes.
   */
  void SetIndexBuffer( PropertyBuffer indexBuffer );

  /**
   * Set the type of geometry to draw (Points, Lines, Triangles, etc)
   * @param[in] geometryType The geometry type
   */
  void SetGeometryType( GeometryType geometryType );

public: // GeometryDataProvider
  /**
   * Get the vertex buffers of the geometry
   * @return A const reference to the vertex buffers
   */
  virtual const VertexBuffers& GetVertexBuffers();

  /**
   * Get the index buffer of the geometry
   * @return A const reference to the index buffer
   */
  virtual const PropertyBuffer& GetIndexBuffer();

  /**
   * Get the type of geometry to draw
   */
  virtual GeometryType GetGeometryType( );

private:
  VertexBuffers mVertexBuffers; ///< The vertex buffers
  PropertyBuffer mIndexBuffer;  ///< The index buffer if required
  GeometryType mGeometryType;   ///< The type of geometry (tris/lines etc)
};

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_GEOMETRY_H
