#ifndef DALI_INTERNAL_RENDER_FRAME_BUFFER_MESSAGES_H
#define DALI_INTERNAL_RENDER_FRAME_BUFFER_MESSAGES_H

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
#include <dali/internal/render/renderers/render-frame-buffer.h>

namespace Dali::Internal::Render
{
inline void KeepRenderResultMessage(EventThreadServices& eventThreadServices, Render::FrameBuffer& frameBuffer)
{
  using LocalType = Message<Render::FrameBuffer>;

  // Reserve some memory inside the render queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&frameBuffer, &Render::FrameBuffer::KeepRenderResult);
}

inline void ClearRenderResultMessage(EventThreadServices& eventThreadServices, Render::FrameBuffer& frameBuffer)
{
  using LocalType = Message<Render::FrameBuffer>;

  // Reserve some memory inside the render queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the render queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&frameBuffer, &Render::FrameBuffer::ClearRenderResult);
}

inline void AttachColorTextureMessage(EventThreadServices& eventThreadServices, Render::FrameBuffer& frameBuffer, const Render::TextureKey& texture, uint32_t mipmapLevel, uint32_t layer)
{
  using LocalType = MessageValue3<Render::FrameBuffer, Render::TextureKey, uint32_t, uint32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&frameBuffer, &Render::FrameBuffer::AttachColorTexture, texture, mipmapLevel, layer);
}

inline void AttachDepthTextureMessage(EventThreadServices& eventThreadServices, Render::FrameBuffer& frameBuffer, const Render::TextureKey& texture, uint32_t mipmapLevel)
{
  using LocalType = MessageValue2<Render::FrameBuffer, Render::TextureKey, uint32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&frameBuffer, &Render::FrameBuffer::AttachDepthTexture, texture, mipmapLevel);
}

inline void AttachDepthStencilTextureMessage(EventThreadServices& eventThreadServices, Render::FrameBuffer& frameBuffer, const Render::TextureKey& texture, uint32_t mipmapLevel)
{
  using LocalType = MessageValue2<Render::FrameBuffer, Render::TextureKey, uint32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&frameBuffer, &Render::FrameBuffer::AttachDepthStencilTexture, texture, mipmapLevel);
}

inline void SetMultiSamplingLevelMessage(EventThreadServices& eventThreadServices, Render::FrameBuffer& frameBuffer, uint8_t multiSamplingLevel)
{
  using LocalType = MessageValue1<Render::FrameBuffer, uint8_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&frameBuffer, &Render::FrameBuffer::SetMultiSamplingLevel, multiSamplingLevel);
}
} // namespace Dali::Internal::Render

#endif // DALI_INTERNAL_RENDER_FRAME_BUFFER_MESSAGES_H
