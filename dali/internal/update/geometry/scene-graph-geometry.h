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
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/double-buffered.h>
#include <dali/internal/update/common/double-buffered-property.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/property-boolean.h>
#include <dali/internal/update/common/uniform-map.h>
#include <dali/internal/update/common/scene-graph-connection-observers.h>
#include <dali/internal/update/common/scene-graph-property-buffer.h>
#include <dali/internal/render/data-providers/geometry-data-provider.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class SceneController;

/**
 * This scene graph object is a property owner. It describes a geometry using a
 * number of PropertyBuffers acting as Vertex buffers.
 */
class Geometry : public PropertyOwner, public GeometryDataProvider, public UniformMap::Observer
{
public:

  /**
   * Constructor
   */
  Geometry();

  /**
   * Destructor
   */
  virtual ~Geometry();

  /**
   * Add a property buffer to be used as a vertex buffer
   */
  void AddVertexBuffer( const PropertyBuffer* vertexBuffer );

  /**
   * Remove a property buffer to be used as a vertex buffer
   * @param[in] vertexBuffer the associated vertex buffer to remove
   */
  void RemoveVertexBuffer( const PropertyBuffer* vertexBuffer );

  /**
   * Set the buffer to be used as a source of indices for the geometry
   * @param[in] indexBuffer the Property buffer describing the indexes for Line, Triangle tyes.
   */
  void SetIndexBuffer( const PropertyBuffer* indexBuffer );

  /**
   * Clear the index buffer if it is no longer required, e.g. if changing geometry type
   * to POINTS.
   */
  void ClearIndexBuffer();

  /**
   * Set the type of geometry to draw (Points, Lines, Triangles, etc)
   * @param[in] geometryType The geometry type
   */
  void SetGeometryType( BufferIndex bufferIndex, GeometryType geometryType );

  /**
   * Connect the object to the scene graph
   *
   * @param[in] sceneController The scene controller - used for sending messages to render thread
   * @param[in] bufferIndex The current buffer index - used for sending messages to render thread
   */
  void ConnectToSceneGraph( SceneController& sceneController, BufferIndex bufferIndex );

  /**
   * Disconnect the object from the scene graph
   * @param[in] sceneController The scene controller - used for sending messages to render thread
   * @param[in] bufferIndex The current buffer index - used for sending messages to render thread
   */
  void DisconnectFromSceneGraph( SceneController& sceneController, BufferIndex bufferIndex );

  /**
   * @copydoc ConnectionObservers::AddObserver
   */
  void AddConnectionObserver(ConnectionObservers::Observer& observer);

  /**
   * @copydoc ConnectionObservers::RemoveObserver
   */
  void RemoveConnectionObserver(ConnectionObservers::Observer& observer);

public: // UniformMap::Observer
  /**
   * @copydoc UniformMap::Observer::UniformMappingsChanged
   */
  virtual void UniformMappingsChanged( const UniformMap& mappings );

public: // GeometryDataProvider
  /**
   * Get the vertex buffers of the geometry
   * @return A const reference to the vertex buffers
   */
  virtual const GeometryDataProvider::VertexBuffers& GetVertexBuffers() const;

  /**
   * Get the index buffer of the geometry
   * @return A const pointer to the index buffer if it exists, or NULL if it doesn't.
   */
  virtual const PropertyBuffer* GetIndexBuffer() const;

  /**
   * Get the type of geometry to draw
   */
  virtual GeometryType GetGeometryType( BufferIndex bufferIndex ) const;

  /**
   * Returns true if this geometry requires depth testing, e.g. if it is
   * a set of vertices with z != 0
   */
  virtual bool GetRequiresDepthTesting( BufferIndex bufferIndex ) const;

protected: // From PropertyOwner
  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyOwner::ResetDefaultProperties()
   */
  virtual void ResetDefaultProperties( BufferIndex updateBufferIndex );

private:
  VertexBuffers mVertexBuffers; ///< The vertex buffers
  const PropertyBuffer* mIndexBuffer;  ///< The index buffer if required
  ConnectionObservers mConnectionObservers;

public: // Properties
  AnimatableProperty<Vector3>  mCenter;
  AnimatableProperty<Vector3>  mHalfExtents;
  DoubleBufferedProperty<int>  mGeometryType;
  DoubleBufferedProperty<bool> mRequiresDepthTest;
};

inline void AddVertexBufferMessage( EventThreadServices& eventThreadServices , const Geometry& geometry, const PropertyBuffer& vertexBuffer )
{
  typedef MessageValue1< Geometry, const PropertyBuffer* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &geometry, &Geometry::AddVertexBuffer, &vertexBuffer );
}

inline void RemoveVertexBufferMessage( EventThreadServices& eventThreadServices, const Geometry& geometry, const PropertyBuffer& vertexBuffer )
{
  typedef MessageValue1< Geometry, const PropertyBuffer* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &geometry, &Geometry::RemoveVertexBuffer, &vertexBuffer );
}

inline void SetIndexBufferMessage( EventThreadServices& eventThreadServices, const Geometry& geometry, const PropertyBuffer& indexBuffer )
{
  typedef MessageValue1< Geometry, const PropertyBuffer* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &geometry, &Geometry::SetIndexBuffer, &indexBuffer );
}

inline void ClearIndexBufferMessage( EventThreadServices& eventThreadServices, const Geometry& geometry )
{
  typedef Message< Geometry > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &geometry, &Geometry::ClearIndexBuffer );
}

} // namespace SceneGraph

// Declare enum as a message parameter type
template <> struct ParameterType< SceneGraph::Geometry::GeometryType > : public BasicType< SceneGraph::Geometry::GeometryType > {};

namespace SceneGraph
{

inline void SetGeometryTypeMessage( EventThreadServices& eventThreadServices, const Geometry& geometry, SceneGraph::Geometry::GeometryType geometryType )
{
  typedef MessageDoubleBuffered1< Geometry, SceneGraph::Geometry::GeometryType > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &geometry, &Geometry::SetGeometryType, geometryType );
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_GEOMETRY_H
