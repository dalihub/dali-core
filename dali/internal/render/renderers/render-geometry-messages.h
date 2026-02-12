#ifndef DALI_INTERNAL_RENDER_GEOMETRY_MESSAGES_H
#define DALI_INTERNAL_RENDER_GEOMETRY_MESSAGES_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/internal/common/message.h>
#include <dali/internal/common/type-abstraction-enums.h> ///< for ParameterType<Dali::Geometry::Type>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/render/renderers/render-geometry.h>
#include <dali/internal/render/renderers/render-vertex-buffer.h>

namespace Dali::Internal::Render
{
inline void AttachVertexBufferMessage(EventThreadServices& eventThreadServices, Render::Geometry& geometry, const Render::VertexBuffer& vertexBuffer)
{
  using LocalType = MessageValue1<Render::Geometry, Render::VertexBuffer*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&geometry, &Render::Geometry::AddVertexBuffer, const_cast<Render::VertexBuffer*>(&vertexBuffer));
}

inline void RemoveVertexBufferMessage(EventThreadServices& eventThreadServices, Render::Geometry& geometry, const Render::VertexBuffer& vertexBuffer)
{
  using LocalType = MessageValue1<Render::Geometry, const Render::VertexBuffer*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&geometry, &Render::Geometry::RemoveVertexBuffer, &vertexBuffer);
}

// Custom message type for SetIndexBuffer() used to move data with Vector::Swap()
template<typename IndexContainerType>
class IndexBufferMessage : public MessageBase
{
public:
  /**
   * Constructor which does a Vector::Swap()
   */
  IndexBufferMessage(Render::Geometry* geometry, IndexContainerType& indices)
  : MessageBase(),
    mRenderGeometry(geometry)
  {
    mIndices.Swap(indices);
  }

  /**
   * Virtual destructor
   */
  ~IndexBufferMessage() override = default;

  /**
   * @copydoc MessageBase::Process
   */
  void Process() override
  {
    DALI_ASSERT_DEBUG(mRenderGeometry && "Message does not have an object");
    mRenderGeometry->SetIndexBuffer(mIndices);
  }

private:
  Render::Geometry*  mRenderGeometry;
  IndexContainerType mIndices;
};

inline void SetIndexBufferMessage(EventThreadServices& eventThreadServices, Render::Geometry& geometry, Render::Geometry::Uint16ContainerType& indices)
{
  using LocalType = IndexBufferMessage<Render::Geometry::Uint16ContainerType>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&geometry, indices);
}

inline void SetIndexBufferMessage(EventThreadServices& eventThreadServices, Render::Geometry& geometry, Render::Geometry::Uint32ContainerType& indices)
{
  using LocalType = IndexBufferMessage<Render::Geometry::Uint32ContainerType>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&geometry, indices);
}

inline void SetGeometryTypeMessage(EventThreadServices& eventThreadServices, Render::Geometry& geometry, Dali::Geometry::Type geometryType)
{
  using LocalType = MessageValue1<Render::Geometry, Dali::Geometry::Type>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&geometry, &Render::Geometry::SetType, geometryType);
}
} // namespace Dali::Internal::Render

#endif //  DALI_INTERNAL_RENDER_GEOMETRY_MESSAGES_H
