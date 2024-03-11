/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

#include <dali/internal/update/rendering/scene-graph-sampler.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
Sampler::Sampler()
: mGraphicsController(nullptr),
  mMinificationFilter(FilterMode::DEFAULT),
  mMagnificationFilter(FilterMode::DEFAULT),
  mSWrapMode(WrapMode::DEFAULT),
  mTWrapMode(WrapMode::DEFAULT),
  mRWrapMode(WrapMode::DEFAULT),
  mIsDirty(false)
{
}

Sampler::~Sampler() = default;

const Dali::Graphics::Sampler* Sampler::GetGraphicsObject()
{
  if(IsDefaultSampler())
  {
    mGraphicsSampler.reset();
  }
  else
  {
    CreateGraphicsObject();
  }
  mIsDirty = false;
  return mGraphicsSampler.get();
}

void Sampler::Initialize(Graphics::Controller& graphicsController)
{
  mGraphicsController = &graphicsController;
  mGraphicsSampler.reset(nullptr);
}

Graphics::Sampler* Sampler::CreateGraphicsObject()
{
  if(!mGraphicsSampler || mIsDirty)
  {
    Graphics::SamplerFilter     minFilter  = GetGraphicsFilter(mMinificationFilter);
    Graphics::SamplerFilter     magFilter  = GetGraphicsFilter(mMagnificationFilter);
    Graphics::SamplerMipmapMode mipmapMode = GetGraphicsSamplerMipmapMode(mMinificationFilter);

    if(mMinificationFilter == FilterMode::NONE)
    {
      minFilter  = Graphics::SamplerFilter::NEAREST;
      mipmapMode = Graphics::SamplerMipmapMode::LINEAR;
    }
    if(mMagnificationFilter == FilterMode::NONE)
    {
      magFilter = Graphics::SamplerFilter::LINEAR;
    }

    mGraphicsSampler = mGraphicsController->CreateSampler(
      Graphics::SamplerCreateInfo()
        .SetMinFilter(minFilter)
        .SetMagFilter(magFilter)
        .SetAddressModeU(GetGraphicsSamplerAddressMode(mSWrapMode))
        .SetAddressModeV(GetGraphicsSamplerAddressMode(mTWrapMode))
        .SetAddressModeW(GetGraphicsSamplerAddressMode(mRWrapMode))
        .SetMipMapMode(mipmapMode),
      nullptr);
  }
  mIsDirty = false;
  return mGraphicsSampler.get();
}

} // namespace Render

} // namespace Internal

} // namespace Dali
