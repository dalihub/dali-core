#ifndef DALI_INTERNAL_RENDER_TEXTURE_MESSAGES_H
#define DALI_INTERNAL_RENDER_TEXTURE_MESSAGES_H

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
#include <dali/graphics-api/graphics-texture-upload-helper.h> ///< for Graphics::UploadParams
#include <dali/internal/event/images/pixel-data-impl.h>       ///< for PixelDataPtr
#include <dali/public-api/images/image-operations.h>          ///< for Dali::ImageDimensions
#include <dali/public-api/images/pixel.h>                     ///< for Dali::Pixel::Format

#include <dali/internal/common/message.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/render/renderers/render-texture-key.h>
#include <dali/internal/render/renderers/render-texture.h>

namespace Dali::Internal::Render
{
inline void UploadTextureMessage(EventThreadServices& eventThreadServices, Render::TextureKey texture, PixelDataPtr pixelData, const Graphics::UploadParams& params)
{
  using LocalType = MessageValue2<Render::Texture, PixelDataPtr, Graphics::UploadParams>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(texture.Get(), &Render::Texture::Upload, pixelData, params);
}

inline void GenerateMipmapsMessage(EventThreadServices& eventThreadServices, Render::TextureKey texture)
{
  using LocalType = Message<Render::Texture>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(texture.Get(), &Render::Texture::GenerateMipmaps);
}

inline void SetTextureSizeMessage(EventThreadServices& eventThreadServices, Render::TextureKey texture, const Dali::ImageDimensions& size)
{
  using LocalType = MessageValue1<Render::Texture, Dali::ImageDimensions>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(texture.Get(), &Render::Texture::SetSize, size);
}

inline void SetTextureFormatMessage(EventThreadServices& eventThreadServices, Render::TextureKey texture, Dali::Pixel::Format pixelFormat)
{
  using LocalType = MessageValue1<Render::Texture, Dali::Pixel::Format>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(texture.Get(), &Render::Texture::SetPixelFormat, pixelFormat);
}
} // namespace Dali::Internal::Render

#endif //  DALI_INTERNAL_RENDER_TEXTURE_MESSAGES_H
