#ifndef DALI_INTERNAL_RENDER_VERTEX_BUFFER_MESSAGES_H
#define DALI_INTERNAL_RENDER_VERTEX_BUFFER_MESSAGES_H

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
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/render/renderers/render-vertex-buffer.h>

namespace Dali::Internal::Render
{
inline void SetVertexBufferFormatMessage(EventThreadServices& eventThreadServices, Render::VertexBuffer& vertexBuffer, OwnerPointer<Render::VertexBuffer::Format>& format)
{
  // Message has ownership of VertexBuffer::Format while in transit from event -> update
  using LocalType = MessageValue1<Render::VertexBuffer, OwnerPointer<Render::VertexBuffer::Format>>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&vertexBuffer, &Render::VertexBuffer::SetFormat, format);
}

inline void SetVertexBufferDataMessage(EventThreadServices& eventThreadServices, Render::VertexBuffer& vertexBuffer, OwnerPointer<Vector<uint8_t>>& data, uint32_t size)
{
  // Message has ownership of VertexBuffer data while in transit from event -> update
  using LocalType = MessageValue2<Render::VertexBuffer, OwnerPointer<Vector<uint8_t>>, uint32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&vertexBuffer, &Render::VertexBuffer::SetData, data, size);
}

inline void SetVertexBufferDivisorMessage(EventThreadServices& eventThreadServices, Render::VertexBuffer& vertexBuffer, uint32_t divisor)
{
  using LocalType = MessageValue1<Render::VertexBuffer, uint32_t>;
  uint32_t* slot  = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));
  new(slot) LocalType(&vertexBuffer, &Render::VertexBuffer::SetDivisor, divisor);
}

inline void SetVertexBufferUpdateCallbackMessage(EventThreadServices& eventThreadServices, Render::VertexBuffer& vertexBuffer, Dali::VertexBufferUpdateCallback* callback)
{
  using LocalType = MessageValue1<Render::VertexBuffer, Dali::VertexBufferUpdateCallback*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&vertexBuffer, &Render::VertexBuffer::SetVertexBufferUpdateCallback, callback);
}
} // namespace Dali::Internal::Render

#endif //  DALI_INTERNAL_RENDER_VERTEX_BUFFER_MESSAGES_H
