/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

TextureSet::TextureSet()
: mSamplers(),
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
    mSamplers.Resize(index + 1);
    for(uint32_t i(samplerCount); i <= index; ++i)
    {
      mSamplers[i] = nullptr;
    }
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
}

void TextureSet::SetTexture(uint32_t index, const Render::TextureKey& texture)
{
  const uint32_t textureCount = static_cast<uint32_t>(mTextures.Size());
  if(textureCount < index + 1)
  {
    mTextures.Resize(index + 1);

    bool samplerExist = true;
    if(mSamplers.Size() < index + 1)
    {
      mSamplers.Resize(index + 1);
      samplerExist = false;
    }

    for(uint32_t i(textureCount); i <= index; ++i)
    {
      mTextures[i] = Render::TextureKey{};

      if(!samplerExist)
      {
        mSamplers[i] = nullptr;
      }
    }
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
