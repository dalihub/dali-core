#ifndef DALI_INTERNAL_SCENE_GRAPH_GEOMETRY_H
#define DALI_INTERNAL_SCENE_GRAPH_GEOMETRY_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/rendering/geometry.h>
#include <dali/devel-api/common/owner-container.h>
#include <dali/graphics-api/graphics-controller.h>

#include <dali/internal/common/message.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/rendering/gpu-buffer.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class PropertyBuffer;

/**
 * This class encapsulates vertex data and index data used to describe a mesh.
 */
class Geometry
{
public:
  typedef Dali::Geometry::Type Type;

  Geometry();

  /**
   * Destructor
   */
  ~Geometry();

  /**
   * Initialize the geometry object with the Graphics API when added to UpdateManager
   *
   * @param[in] graphics The Graphics API
   */
  void Initialize( Graphics::Controller& graphics );

  /**
   * Adds a property buffer to the geometry
   * @param[in] dataProvider The PropertyBuffer data provider
   */
  void AddPropertyBuffer( SceneGraph::PropertyBuffer* propertyBuffer );

  /**
   * Set the data for the index buffer to be used by the geometry
   * @param[in] indices A vector containing the indices
   */
  void SetIndexBuffer( Dali::Vector<unsigned short>& indices );

  /**
   * Removes a PropertyBuffer from the geometry
   * @param[in] propertyBuffer The property buffer to be removed
   */
  void RemovePropertyBuffer(  const SceneGraph::PropertyBuffer* propertyBuffer );

  /**
   * @return the topology of this geometry
   */
  [[nodiscard]] Graphics::PrimitiveTopology GetTopology() const;

  /**
   * Upload the geometry if it has changed
   */
  void Upload(Graphics::Controller& graphicsController);

  /**
   * Set up the attributes and perform the Draw call corresponding to the geometry type.
   *
   * @param[in] graphicsController The graphics controller
   * @param[in,out] commandBuffer The current command buffer queue
   * @param[in] elementBufferOffset The index of first vertex / element to draw if index buffer bound
   * @param[in] elementBufferCount Number of vertices / elements to draw if index buffer bound, uses whole buffer when 0
   * @param[in] instanceCount Number of instances to draw (use in conjunction with VertexBuffer divisor)
   * @return true if the draw command was issued, false otherwise
   */
  bool Draw(Graphics::Controller&    graphicsController,
            Graphics::CommandBuffer& commandBuffer,
            uint32_t                 elementBufferOffset,
            uint32_t                 elementBufferCount,
            uint32_t                 instanceCount);

  /**
   * @brief Set up the attributes bind commaneds
   *
   * @param[in,out] commandBuffer The current command buffer queue
   * @return true if the bind command was issued, false otherwise
   */
  bool BindVertexAttributes(Graphics::CommandBuffer& commandBuffer);

  /**
   * Chack if the attributes for the geometry have changed
   * @return True if vertex buffers have been added or removed since last frame, false otherwise
   */
  bool AttributesChanged() const
  {
    return mAttributesChanged;
  }

  /**
   * Sets the geometry type
   * @param[in] type The new geometry type
   */
  void SetType( Type type )
  {
    mGeometryType = type;
  }

  Type GetType() const
  {
    return mGeometryType;
  }

  /**
   * @return
   */
  const Vector< SceneGraph::PropertyBuffer* >& GetVertexBuffers() const
  {
    return mVertexBuffers;
  }

  const GpuBuffer* GetIndexBuffer() const
  {
    return mIndexBuffer.Get();
  }

  bool HasIndexBuffer() const
  {
    return mHasIndexBuffer;
  }

  uint32_t GetIndexBufferElementCount() const
  {
    return mIndexBufferElementCount;
  }

  void DestroyGraphicsObjects()
  {
    mIndexBuffer.Reset();
  }

private:
  Graphics::Controller* mGraphicsController; ///< Graphics interface object

  // PropertyBuffers
  Vector< SceneGraph::PropertyBuffer* > mVertexBuffers;
  OwnerPointer<GpuBuffer> mIndexBuffer;
  uint32_t mIndexBufferElementCount;
  Type mGeometryType;

  // Booleans
  bool mIndicesChanged : 1;
  bool mHasBeenUpdated : 1;
  bool mAttributesChanged : 1;
  bool mHasIndexBuffer : 1;
};

} // SceneGraph

// Enums can be passed via message by templating a suitable ParameterType.
template <> struct ParameterType< Dali::Geometry::Type > : public BasicType< Dali::Geometry::Type > {};

namespace SceneGraph
{

// Custom message type for SetIndexBuffer() used to move data with Vector::Swap()
class IndexBufferMessage : public MessageBase
{
public:

  /**
   * Constructor which does a Vector::Swap()
   */
  IndexBufferMessage( SceneGraph::Geometry* geometry, Dali::Vector<unsigned short>& indices )
  : MessageBase(),
    mGeometry( geometry )
  {
    mIndices.Swap( indices );
  }

  /**
   * Virtual destructor
   */
  virtual ~IndexBufferMessage()
  {
  }

  /**
   * @copydoc MessageBase::Process
   */
  virtual void Process( BufferIndex /*bufferIndex*/ )
  {
    DALI_ASSERT_DEBUG( mGeometry && "Message does not have an object" );
    mGeometry->SetIndexBuffer( mIndices );
  }

private:
  SceneGraph::Geometry* mGeometry;
  Dali::Vector<unsigned short> mIndices;
};

inline void SetIndexBufferMessage( EventThreadServices& eventThreadServices, SceneGraph::Geometry& geometry, Dali::Vector<unsigned short>& indices )
{
  typedef IndexBufferMessage LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &geometry, indices );
}

inline void AttachVertexBufferMessage( EventThreadServices& eventThreadServices, SceneGraph::Geometry& geometry, const SceneGraph::PropertyBuffer& vertexBuffer )
{
  typedef MessageValue1< SceneGraph::Geometry, SceneGraph::PropertyBuffer* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &geometry, &Geometry::AddPropertyBuffer, const_cast<SceneGraph::PropertyBuffer*>(&vertexBuffer) );
}

inline void RemoveVertexBufferMessage( EventThreadServices& eventThreadServices, SceneGraph::Geometry& geometry, const SceneGraph::PropertyBuffer& vertexBuffer )
{
  typedef MessageValue1< SceneGraph::Geometry, const SceneGraph::PropertyBuffer* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &geometry, &Geometry::RemovePropertyBuffer, &vertexBuffer );
}

inline void SetGeometryTypeMessage( EventThreadServices& eventThreadServices, SceneGraph::Geometry& geometry, Dali::Geometry::Type geometryType )
{
  typedef MessageValue1< Geometry, Dali::Geometry::Type > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &geometry, &Geometry::SetType, geometryType );
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_GEOMETRY_H
