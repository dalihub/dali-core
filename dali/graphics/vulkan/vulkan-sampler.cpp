/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/graphics/vulkan/vulkan-sampler.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

/**
 * Sampler
 */

/**
 * Creates new mutable sampler
 * @param graphics
 * @return
 */
RefCountedSampler Sampler::New( Graphics& graphics, const vk::SamplerCreateInfo& createInfo )
{
  return RefCountedSampler( new Sampler( graphics, createInfo ) );
}

Sampler::Sampler( Graphics& graphics, const vk::SamplerCreateInfo& createInfo )
        : mGraphics(&graphics),
          mCreateInfo(createInfo)
{
}

Sampler::~Sampler() = default;

vk::Sampler Sampler::GetVkHandle() const
{
  return mSampler;
}

const Sampler& Sampler::ConstRef()
{
  return *this;
}

Sampler& Sampler::Ref()
{
  return *this;
}

Sampler::operator vk::Sampler*()
{
  return &mSampler;
}

bool Sampler::OnDestroy()
{
  mGraphics->RemoveSampler( *this );

  mGraphics->DiscardResource( [this]() {
    mGraphics->GetDevice().destroySampler( mSampler, mGraphics->GetAllocator() );
  } );

  return false;
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali