/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include "scene-graph-texture-set.h"

// INTERNAL HEADERS
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/common/memory-pool-object-allocator.h>
#include <dali/internal/render/common/render-manager.h>
#include <dali/internal/render/renderers/render-texture.h>
#include <dali/internal/update/controllers/render-message-dispatcher.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>

namespace //Unnamed namespace
{
//Memory pool used to allocate new texture sets. Memory used by this pool will be released when shutting down DALi
Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::TextureSet>& GetTextureSetMemoryPool()
{
  static Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::TextureSet> gTextureSetMemoryPool;
  return gTextureSetMemoryPool;
}
} // namespace

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
TextureSet* TextureSet::New()
{
  return new(GetTextureSetMemoryPool().AllocateRawThreadSafe()) TextureSet();
}

void TextureSet::ResetMemoryPool()
{
  GetTextureSetMemoryPool().ResetMemoryPool();
}

TextureSet::TextureSet()
: mSamplers(),
  mTextures(),
  mHasAlpha(false)
{
}

TextureSet::~TextureSet()
{
}

void TextureSet::operator delete(void* ptr)
{
  GetTextureSetMemoryPool().FreeThreadSafe(static_cast<TextureSet*>(ptr));
}

void TextureSet::SetSampler(uint32_t index, Render::Sampler* sampler)
{
  const uint32_t samplerCount = static_cast<uint32_t>(mSamplers.Size());
  if(samplerCount < index + 1)
  {
    SetSamplerCount(index + 1);
  }

  mSamplers[index] = sampler;

  if(index < static_cast<uint32_t>(mTextures.Size()) && mTextures[index])
  {
    // Send a message to the RenderManagerReserveMessageSlot
    using DerivedType = MessageValue1<RenderManager, Render::TextureKey>;

    // Reserve some memory inside the render queue
    uint32_t* slot = mRenderMessageDispatcher->ReserveMessageSlot(sizeof(DerivedType));

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new(slot) DerivedType(&mRenderMessageDispatcher->GetRenderManager(), &RenderManager::SetTextureUpdated, mTextures[index]);
  }

  if(!sampler)
  {
    // Check wheter we need to pop back sampler
    TrimContainers();
  }
}

void TextureSet::SetTexture(uint32_t index, const Render::TextureKey& texture)
{
  const uint32_t textureCount = static_cast<uint32_t>(mTextures.Size());
  if(textureCount < index + 1)
  {
    SetTextureCount(index + 1);
  }

  mTextures[index] = texture;
  if(texture)
  {
    mHasAlpha |= texture->HasAlphaChannel();

    // Send a message to the RenderManagerReserveMessageSlot
    using DerivedType = MessageValue1<RenderManager, Render::TextureKey>;

    // Reserve some memory inside the render queue
    uint32_t* slot = mRenderMessageDispatcher->ReserveMessageSlot(sizeof(DerivedType));

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new(slot) DerivedType(&mRenderMessageDispatcher->GetRenderManager(), &RenderManager::SetTextureUpdated, texture);
  }
  else
  {
    // Check wheter we need to pop back textures
    TrimContainers();
  }
}

void TextureSet::SetTextureCount(uint32_t count)
{
  const uint32_t textureCount = static_cast<uint32_t>(mTextures.Size());

  if(textureCount != count)
  {
    mTextures.Resize(count, Render::TextureKey{});
  }
}

void TextureSet::SetSamplerCount(uint32_t count)
{
  const uint32_t samplerCount = static_cast<uint32_t>(mSamplers.Size());

  if(samplerCount != count)
  {
    mSamplers.Resize(count, nullptr);
  }
}

void TextureSet::TrimContainers()
{
  uint32_t textureCount = static_cast<uint32_t>(mTextures.Size());
  uint32_t samplerCount = static_cast<uint32_t>(mSamplers.Size());

  while(textureCount > 0u)
  {
    if(mTextures[textureCount - 1u])
    {
      break;
    }
    --textureCount;
  }

  while(samplerCount > 0u)
  {
    if(mSamplers[samplerCount - 1u])
    {
      break;
    }
    --samplerCount;
  }

  SetTextureCount(textureCount);
  SetSamplerCount(samplerCount);
}

bool TextureSet::HasAlpha() const
{
  return mHasAlpha;
}

void TextureSet::SetRenderMessageDispatcher(RenderMessageDispatcher* renderMessageDispatcher)
{
  mRenderMessageDispatcher = renderMessageDispatcher;
}

uint32_t TextureSet::GetMemoryPoolCapacity()
{
  return GetTextureSetMemoryPool().GetCapacity();
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
