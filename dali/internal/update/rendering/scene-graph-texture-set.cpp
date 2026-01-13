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
 */

// CLASS HEADER
#include <dali/internal/update/rendering/scene-graph-texture-set.h>

// INTERNAL HEADERS
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/common/memory-pool-object-allocator.h>
#include <dali/internal/render/renderers/render-texture.h>
#include <dali/internal/update/common/scene-graph-memory-pool-collection.h>
#include <dali/internal/update/controllers/render-manager-dispatcher.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>

namespace //Unnamed namespace
{
Dali::Internal::SceneGraph::MemoryPoolCollection*                                 gMemoryPoolCollection = nullptr;
static constexpr Dali::Internal::SceneGraph::MemoryPoolCollection::MemoryPoolType gMemoryPoolType       = Dali::Internal::SceneGraph::MemoryPoolCollection::MemoryPoolType::TEXTURE_SET;
} // namespace

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
TextureSet* TextureSet::New()
{
  DALI_ASSERT_DEBUG(gMemoryPoolCollection && "TextureSet::RegisterMemoryPoolCollection not called!");
  return new(gMemoryPoolCollection->AllocateRawThreadSafe(gMemoryPoolType)) TextureSet();
}

void TextureSet::RegisterMemoryPoolCollection(MemoryPoolCollection& memoryPoolCollection)
{
  gMemoryPoolCollection = &memoryPoolCollection;
}

void TextureSet::UnregisterMemoryPoolCollection()
{
  gMemoryPoolCollection = nullptr;
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
  DALI_ASSERT_DEBUG(gMemoryPoolCollection && "TextureSet::RegisterMemoryPoolCollection not called!");
  gMemoryPoolCollection->FreeThreadSafe(gMemoryPoolType, ptr);
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
    mRenderManagerDispatcher->SetTextureUpdated(mTextures[index]);
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

    mRenderManagerDispatcher->SetTextureUpdated(texture);
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

void TextureSet::SetRenderManagerDispatcher(RenderManagerDispatcher* renderManagerDispatcher)
{
  mRenderManagerDispatcher = renderManagerDispatcher;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
