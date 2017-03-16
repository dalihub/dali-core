
/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include "graphics-test-implementation.h"

namespace Dali
{

namespace Graphics
{
namespace Test
{

API::Accessor< API::Shader > Controller::CreateShader(
    const API::BaseFactory< API::Shader >& factory)
{
  auto handle = mShaders.CreateObject(factory);
  return API::Accessor< API::Shader >{mShaders, handle};
}

API::Accessor< API::Texture > Controller::CreateTexture(
    const API::BaseFactory< API::Texture >& factory)
{
  auto handle = mTextures.CreateObject(factory);
  return API::Accessor< API::Texture >{mTextures, handle};
}

API::Accessor< API::TextureSet > Controller::CreateTextureSet(
    const API::BaseFactory< API::TextureSet >& factory)
{
  auto handle = mTextureSets.CreateObject(factory);
  return API::Accessor< API::TextureSet >{mTextureSets, handle};
}

API::Accessor< API::DynamicBuffer > Controller::CreateDynamicBuffer(
    const API::BaseFactory< API::DynamicBuffer >& factory)
{
  auto handle = mDynamicBuffers.CreateObject(factory);
  return API::Accessor< API::DynamicBuffer >{mDynamicBuffers, handle};
}

API::Accessor< API::StaticBuffer > Controller::CreateStaticBuffer(
    const API::BaseFactory< API::StaticBuffer >& factory)
{
  auto handle = mStaticBuffers.CreateObject(factory);
  return API::Accessor< API::StaticBuffer >{mStaticBuffers, handle};
}

API::Accessor< API::Sampler > Controller::CreateSampler(
    const API::BaseFactory< API::Sampler >& factory)
{
  auto handle = mSamplers.CreateObject(factory);
  return API::Accessor< API::Sampler >{mSamplers, handle};
}

API::Accessor< API::Framebuffer > Controller::CreateFramebuffer(
    const API::BaseFactory< API::Framebuffer >& factory)
{
  auto handle = mFramebuffers.CreateObject(factory);
  return API::Accessor< API::Framebuffer >{mFramebuffers, handle};
}

void Controller::GetRenderItemList()
{
}

} // namespace Test
} // namespace Graphics
} // namespace Dali
