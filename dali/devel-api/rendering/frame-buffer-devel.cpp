/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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

// HEADER
#include <dali/devel-api/rendering/frame-buffer-devel.h>

// INTERNAL INCLUDES
#include <dali/internal/event/rendering/frame-buffer-impl.h> // Dali::Internal::FrameBuffer

namespace Dali
{
namespace DevelFrameBuffer
{
void AttachDepthTexture(FrameBuffer frameBuffer, Texture& texture)
{
  AttachDepthTexture(frameBuffer, texture, 0u);
}

void AttachDepthTexture(FrameBuffer frameBuffer, Texture& texture, uint32_t mipmapLevel)
{
  if(texture)
  {
    Internal::TexturePtr texturePtr(&GetImplementation(texture));
    GetImplementation(frameBuffer).AttachDepthTexture(texturePtr, mipmapLevel);
  }
}

void AttachDepthStencilTexture(FrameBuffer frameBuffer, Texture& texture)
{
  AttachDepthStencilTexture(frameBuffer, texture, 0u);
}

void AttachDepthStencilTexture(FrameBuffer frameBuffer, Texture& texture, uint32_t mipmapLevel)
{
  if(texture)
  {
    Internal::TexturePtr texturePtr(&GetImplementation(texture));
    GetImplementation(frameBuffer).AttachDepthStencilTexture(texturePtr, mipmapLevel);
  }
}

void SetMultiSamplingLevel(FrameBuffer frameBuffer, uint8_t multiSamplingLevel)
{
  GetImplementation(frameBuffer).SetMultiSamplingLevel(multiSamplingLevel);
}
uint8_t GetMultiSamplingLevel(FrameBuffer frameBuffer)
{
  return GetImplementation(frameBuffer).GetMultiSamplingLevel();
}

Texture GetColorTexture(const FrameBuffer frameBuffer, uint8_t index)
{
  Internal::Texture* texturePtr = GetImplementation(frameBuffer).GetColorTexture(index);
  return Dali::Texture(texturePtr);
}

Texture GetDepthTexture(FrameBuffer frameBuffer)
{
  Internal::Texture* texturePtr = GetImplementation(frameBuffer).GetDepthTexture();
  return Dali::Texture(texturePtr);
}

Texture GetDepthStencilTexture(FrameBuffer frameBuffer)
{
  Internal::Texture* texturePtr = GetImplementation(frameBuffer).GetDepthStencilTexture();
  return Dali::Texture(texturePtr);
}

uint8_t GetColorAttachmentCount(const FrameBuffer frameBuffer)
{
  return GetImplementation(frameBuffer).GetColorAttachmentCount();
}

Uint16Pair GetSize(FrameBuffer frameBuffer)
{
  return GetImplementation(frameBuffer).GetSize();
}

FrameBuffer::Attachment::Mask GetMask(FrameBuffer frameBuffer)
{
  return GetImplementation(frameBuffer).GetMask();
}

} // namespace DevelFrameBuffer

} // namespace Dali
