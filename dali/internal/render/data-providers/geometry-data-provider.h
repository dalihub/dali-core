#ifndef __DALI_INTERNAL_SCENE_GRAPH_GEOMETRY_DATA_PROVIDER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_GEOMETRY_DATA_PROVIDER_H__
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

#include <dali/public-api/geometry/geometry.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/owner-container.h>
#include <dali/internal/render/data-providers/property-buffer-data-provider.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class PropertyBuffer;

/**
 * An interface to provide geometry data, such as the vertex buffers
 * and index buffers (if present).  It provides a means of getting the
 * vertex attribute meta data and vertex data from each buffer.  It
 * provides the geometry type;
 */
class GeometryDataProvider
{
public:
  typedef Dali::Geometry::GeometryType GeometryType;
  typedef Dali::Vector< const PropertyBufferDataProvider* > VertexBuffers;

  /**
   * Constructor. Nothing to do as a pure interface.
   */
  GeometryDataProvider() { }

public: // GeometryDataProvider
  /**
   * @todo MESH_REWORK FIX ME Don't get objects through this interface. Send via message instead
   * Get the vertex buffers of the geometry
   * @return A const reference to the vertex buffers
   */
  virtual const VertexBuffers& GetVertexBuffers() const = 0;

  /**
   * @todo MESH_REWORK FIX ME Don't get objects through this interface. Send via message instead
   * Get the index buffer of the geometry
   * @return A const reference to the index buffer
   */
  virtual const PropertyBuffer* GetIndexBuffer() const = 0;

  /**
   * Get the type of geometry to draw
   */
  virtual GeometryType GetGeometryType( BufferIndex bufferIndex ) const = 0;

  /**
   * @todo MESH_REWORK - Should only use this in Update Sorting algorithm
   * Returns true if this geometry requires depth testing, e.g. if it is
   * a set of vertices with z != 0
   */
  virtual bool GetRequiresDepthTesting( BufferIndex bufferIndex ) const = 0;

protected:

  /**
   * Virtual destructor, this is an interface, no deletion through this interface
   */
  virtual ~GeometryDataProvider() { }
};

} // SceneGraph
} // Internal
} // Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_GEOMETRY_DATA_PROVIDER_H__
