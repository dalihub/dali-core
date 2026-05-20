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
 *
 */

// CLASS HEADER
#include <dali/internal/event/rendering/frame-buffer-impl.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/render/renderers/render-frame-buffer-messages.h>
#include <dali/internal/render/renderers/render-frame-buffer.h>
#include <dali/internal/update/manager/update-manager.h>

#if defined(GPU_MEMORY_PROFILE_ENABLED)
#include <dali/internal/event/images/pixel-data-impl.h>
#include <dali/internal/event/rendering/texture-impl.h>
#endif

namespace Dali
{
namespace Internal
{
FrameBufferPtr FrameBuffer::New(uint32_t width, uint32_t height, Mask attachments)
{
  FrameBufferPtr frameBuffer(new FrameBuffer(width, height, attachments));
  frameBuffer->Initialize();
  return frameBuffer;
}

Render::FrameBuffer* FrameBuffer::GetRenderObject() const
{
  return mRenderObject;
}

FrameBuffer::FrameBuffer(uint32_t width, uint32_t height, Mask attachments)
: EventThreadServicesHolder(EventThreadServices::Get()),
  mRenderObject(nullptr),
  mColor{nullptr},
  mDepth(nullptr),
  mStencil(nullptr),
  mWidth(width),
  mHeight(height),
  mAttachments(attachments),
  mColorAttachmentCount(0)
{
}

void FrameBuffer::Initialize()
{
  mRenderObject = new Render::FrameBuffer(mWidth, mHeight, mAttachments);

  OwnerPointer<Render::FrameBuffer> transferOwnership(mRenderObject);
  SceneGraph::AddFrameBufferMessage(GetEventThreadServices().GetUpdateManager(), transferOwnership);
}

void FrameBuffer::AttachColorTexture(TexturePtr texture, uint32_t mipmapLevel, uint32_t layer)
{
  if((texture->GetWidth() / (1u << mipmapLevel) != mWidth) ||
     (texture->GetHeight() / (1u << mipmapLevel) != mHeight))
  {
    DALI_LOG_ERROR("Failed to attach color texture to FrameBuffer: Size mismatch \n");
  }
  else if(mColorAttachmentCount >= Dali::DevelFrameBuffer::MAX_COLOR_ATTACHMENTS)
  {
    DALI_LOG_ERROR("Failed to attach color texture to FrameBuffer: Exceeded maximum supported color attachments.\n");
  }
  else
  {
#if defined(GPU_MEMORY_PROFILE_ENABLED)
    auto typeHint = static_cast<Dali::Integration::TextureContextTypeHint::Type>(static_cast<int>(Dali::Integration::TextureContextTypeHint::FBO_ATTACHED_COLOR_TEXTURE) + mColorAttachmentCount);

    // Use previous context if it was exist. Else, just notify it is color attachment.
    texture->Upload(nullptr, "FrameBuffer::ColorAttachment", typeHint, true, true);
#endif
    mColor[mColorAttachmentCount] = texture;
    ++mColorAttachmentCount;

    Render::AttachColorTextureMessage(GetEventThreadServices(), *mRenderObject, texture->GetRenderTextureKey(), mipmapLevel, layer);
  }
}

void FrameBuffer::AttachDepthTexture(TexturePtr texture, uint32_t mipmapLevel)
{
  if((texture->GetWidth() / (1u << mipmapLevel) != mWidth) ||
     (texture->GetHeight() / (1u << mipmapLevel) != mHeight))
  {
    DALI_LOG_ERROR("Failed to attach depth texture to FrameBuffer: Size mismatch \n");
  }
  else
  {
#if defined(GPU_MEMORY_PROFILE_ENABLED)
    auto typeHint = Dali::Integration::TextureContextTypeHint::FBO_ATTACHED_DEPTH_TEXTURE;

    // Use previous context if it was exist. Else, just notify it is depth attachment.
    texture->Upload(nullptr, "FrameBuffer::DepthAttachment", typeHint, true, true);
#endif
    mDepth = texture;
    Render::AttachDepthTextureMessage(GetEventThreadServices(), *mRenderObject, texture->GetRenderTextureKey(), mipmapLevel);
  }
}

void FrameBuffer::AttachDepthStencilTexture(TexturePtr texture, unsigned int mipmapLevel)
{
  if((texture->GetWidth() / (1u << mipmapLevel) != mWidth) ||
     (texture->GetHeight() / (1u << mipmapLevel) != mHeight))
  {
    DALI_LOG_ERROR("Failed to attach depth/stencil texture to FrameBuffer: Size mismatch \n");
  }
  else
  {
#if defined(GPU_MEMORY_PROFILE_ENABLED)
    auto typeHint = Dali::Integration::TextureContextTypeHint::FBO_ATTACHED_DEPTH_STENCIL_TEXTURE;

    // Use previous context if it was exist. Else, just notify it is depth stencil attachment.
    texture->Upload(nullptr, "FrameBuffer::DepthStencilAttachment", typeHint, true, true);
#endif
    mStencil = texture;
    Render::AttachDepthStencilTextureMessage(GetEventThreadServices(), *mRenderObject, texture->GetRenderTextureKey(), mipmapLevel);
  }
}

void FrameBuffer::SetMultiSamplingLevel(uint8_t multiSamplingLevel)
{
  mMultiSamplingLevel = multiSamplingLevel;
  Render::SetMultiSamplingLevelMessage(GetEventThreadServices(), *mRenderObject, multiSamplingLevel);
}

Texture* FrameBuffer::GetColorTexture(uint8_t index) const
{
  return (index >= mColorAttachmentCount) ? nullptr : mColor[index].Get();
}

Texture* FrameBuffer::GetDepthTexture() const
{
  return (mDepth) ? mDepth.Get() : nullptr;
}

Texture* FrameBuffer::GetDepthStencilTexture() const
{
  return (mStencil) ? mStencil.Get() : nullptr;
}

void FrameBuffer::SetSize(uint32_t width, uint32_t height)
{
  mWidth  = width;
  mHeight = height;
}

void FrameBuffer::KeepRenderResult()
{
  Render::KeepRenderResultMessage(GetEventThreadServices(), *mRenderObject);
}

void FrameBuffer::ClearRenderResult()
{
  if(DALI_LIKELY(EventThreadServices::IsCoreRunning() && mRenderObject))
  {
    Render::ClearRenderResultMessage(GetEventThreadServices(), *mRenderObject);
  }
}

Dali::PixelData FrameBuffer::GetRenderResult()
{
  return mRenderObject->GetRenderResult();
}

FrameBuffer::~FrameBuffer()
{
  if(DALI_UNLIKELY(!EventThreadServices::IsEventThread()))
  {
    DALI_LOG_ERROR("~FrameBuffer[%p] called from non-UI thread! something unknown issue will be happened!\n", this);
  }

  if(DALI_LIKELY(EventThreadServices::IsCoreRunning() && mRenderObject))
  {
    SceneGraph::RemoveFrameBufferMessage(GetEventThreadServices().GetUpdateManager(), *mRenderObject);
  }
}

} // namespace Internal
} // namespace Dali
