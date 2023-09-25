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
 *
 */

// CLASS HEADER
#include <dali/internal/event/rendering/texture-set-impl.h> // Dali::Internal::TextureSet

// INTERNAL INCLUDES
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/rendering/scene-graph-texture-set.h>
#include <dali/public-api/object/type-registry.h>

namespace Dali
{
namespace Internal
{
TextureSetPtr TextureSet::New()
{
  TextureSetPtr textureSet(new TextureSet());
  textureSet->Initialize();
  return textureSet;
}

void TextureSet::SetTexture(uint32_t index, TexturePtr texture)
{
  uint32_t textureCount = static_cast<uint32_t>(mTextures.size());

  if(textureCount < index + 1)
  {
    SetTextureCount(index + 1);
  }

  mTextures[index] = texture;

  Render::TextureKey renderTexture{};
  if(texture)
  {
    renderTexture = texture->GetRenderTextureKey();
  }

  SceneGraph::SetTextureMessage(mEventThreadServices, *mSceneObject, index, renderTexture);

  if(!texture)
  {
    // Check wheter we need to pop back textures
    TrimContainers();
  }
}

Texture* TextureSet::GetTexture(uint32_t index) const
{
  Texture* result(nullptr);
  if(index < mTextures.size())
  {
    result = mTextures[index].Get();
  }
  else
  {
    DALI_LOG_ERROR("Error: Invalid index to TextureSet::GetTexture\n");
  }

  return result;
}

void TextureSet::SetSampler(uint32_t index, SamplerPtr sampler)
{
  uint32_t samplerCount = static_cast<uint32_t>(mSamplers.size());
  if(samplerCount < index + 1)
  {
    SetSamplerCount(index + 1);
  }

  mSamplers[index] = sampler;

  Render::Sampler* renderSampler(nullptr);
  if(sampler)
  {
    renderSampler = sampler->GetSamplerRenderObject();
  }

  SceneGraph::SetSamplerMessage(mEventThreadServices, *mSceneObject, index, renderSampler);

  if(!sampler)
  {
    // Check wheter we need to pop back sampler
    TrimContainers();
  }
}

Sampler* TextureSet::GetSampler(uint32_t index) const
{
  Sampler* result(nullptr);
  if(index < mSamplers.size())
  {
    result = mSamplers[index].Get();
  }
  else
  {
    DALI_LOG_ERROR("Error: Invalid index to TextureSet::GetSampler\n");
  }

  return result;
}

uint32_t TextureSet::GetTextureCount() const
{
  return static_cast<uint32_t>(mTextures.size());
}

const SceneGraph::TextureSet* TextureSet::GetTextureSetSceneObject() const
{
  return mSceneObject;
}

void TextureSet::SetTextureCount(uint32_t count)
{
  uint32_t textureCount = static_cast<uint32_t>(mTextures.size());
  if(textureCount != count)
  {
    mTextures.resize(count, nullptr);
  }
}

void TextureSet::SetSamplerCount(uint32_t count)
{
  uint32_t samplerCount = static_cast<uint32_t>(mSamplers.size());
  if(samplerCount != count)
  {
    mSamplers.resize(count, nullptr);
  }
}

void TextureSet::TrimContainers()
{
  uint32_t textureCount = static_cast<uint32_t>(mTextures.size());
  uint32_t samplerCount = static_cast<uint32_t>(mSamplers.size());

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

TextureSet::TextureSet()
: mEventThreadServices(EventThreadServices::Get()),
  mSceneObject(nullptr)
{
}

void TextureSet::Initialize()
{
  SceneGraph::UpdateManager& updateManager = mEventThreadServices.GetUpdateManager();

  mSceneObject = SceneGraph::TextureSet::New();
  OwnerPointer<SceneGraph::TextureSet> transferOwnership(mSceneObject);
  AddTextureSetMessage(updateManager, transferOwnership);
}

TextureSet::~TextureSet()
{
  if(EventThreadServices::IsCoreRunning())
  {
    SceneGraph::UpdateManager& updateManager = mEventThreadServices.GetUpdateManager();
    RemoveTextureSetMessage(updateManager, *mSceneObject);
  }
}

} // namespace Internal
} // namespace Dali
