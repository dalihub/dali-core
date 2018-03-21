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

struct Sampler::Impl
{
  Impl( Sampler& owner, Graphics& graphics, vk::SamplerCreateInfo info, bool immutable )
  : mOwner( owner ), mGraphics( graphics ), mCreateInfo( info ), mImmutable( immutable ), mModified( true )
  {

  }

  ~Impl()
  {
    DestroySafe();
  }

  bool CreateSampler()
  {
    if( !mModified )
    {
      return true;
    }

    if( mVkSampler )
    {
      DestroySafe();
    }
    mVkSampler = VkAssert( mGraphics.GetDevice().createSampler( mCreateInfo, mGraphics.GetAllocator()));

    // push to the resource list in graphics

    mModified = false;

    return true;
  }

  void DestroySafe()
  {
    // fixme: push sampler to the destroy queue, will be destroyed with the end of the frame
    mGraphics.GetDevice().destroySampler( mVkSampler );
  }

  Impl( const Impl& ) = delete;
  Impl& operator=( const Impl& ) = delete;

  Sampler&              mOwner;
  Graphics&             mGraphics;
  vk::SamplerCreateInfo mCreateInfo;
  vk::Sampler           mVkSampler;
  bool                  mImmutable;
  bool                  mModified;
};



/**
 * Sampler
 */

/**
 * Creates new mutable sampler
 * @param graphics
 * @return
 */
SamplerRef Sampler::New( Graphics& graphics )
{
  return SamplerRef(new Sampler(graphics, vk::SamplerCreateInfo{}, false));
}

/**
 * Creates new immutable sampler
 * @param graphics
 * @param createInfo
 * @return
 */
SamplerRef Sampler::NewImmutable( Graphics& graphics, vk::SamplerCreateInfo createInfo )
{
  auto retval = SamplerRef(new Sampler(graphics, createInfo, true));
  if( retval->mImpl->CreateSampler() )
  {
    return retval;
  }
  return SamplerRef();
}

Sampler::Sampler( Graphics& graphics, vk::SamplerCreateInfo createInfo, bool immutable )
{
  mImpl.reset(new Impl( *this, graphics, createInfo, immutable ));
}

Sampler::~Sampler() = default;

/**
 * Sets minification and magnifcation filters
 * @param minFilter
 * @param magFilter
 */
void Sampler::SetFilter( vk::Filter minFilter, vk::Filter magFilter )
{
  mImpl->mCreateInfo.setMinFilter( minFilter );
  mImpl->mCreateInfo.setMagFilter( magFilter );
  mImpl->mModified = true;
}

/**
 * Sets UVW address mode
 * @param addressModeU
 * @param addressModeV
 * @param addressModeW
 */
void Sampler::SetAddressMode( vk::SamplerAddressMode addressModeU,
                     vk::SamplerAddressMode addressModeV,
                     vk::SamplerAddressMode addressModeW )
{
  mImpl->mCreateInfo.setAddressModeU( addressModeU );
  mImpl->mCreateInfo.setAddressModeV( addressModeV );
  mImpl->mCreateInfo.setAddressModeW( addressModeW );
  mImpl->mModified = true;
}

void Sampler::SetLod( float minLod, float maxLod )
{
  mImpl->mCreateInfo.setMinLod( minLod ).setMaxLod( maxLod );
  mImpl->mModified = true;
}

void Sampler::EnableAnisotropy( bool enabled )
{
  mImpl->mCreateInfo.setAnisotropyEnable( enabled );
  mImpl->mModified = true;
}

void Sampler::SetMipmapMode( vk::SamplerMipmapMode mode )
{
  mImpl->mCreateInfo.setMipmapMode( mode );
  mImpl->mModified = true;
}

void Sampler::SetMaxAnisotropy( float maxAnisotropy )
{
  mImpl->mCreateInfo.setMaxAnisotropy( maxAnisotropy );
  mImpl->mModified = true;
}

void Sampler::EnableCompare( bool enabled )
{
  mImpl->mCreateInfo.setCompareEnable( enabled );
  mImpl->mModified = true;
}

void Sampler::SetBorderColor( vk::BorderColor color )
{
  mImpl->mCreateInfo.setBorderColor( color );
  mImpl->mModified = true;
}

void Sampler::UseUnnormalizedCoordinates( bool enabled )
{
  mImpl->mCreateInfo.setUnnormalizedCoordinates( enabled );
  mImpl->mModified = true;
}

void Sampler::SetCompareOp( vk::CompareOp compareOp )
{
  mImpl->mCreateInfo.setCompareOp( compareOp );
  mImpl->mModified = true;
}

vk::Sampler Sampler::GetVkSampler() const
{
  // if no sampler yet, create it now
  if( mImpl->mModified )
  {
    mImpl->CreateSampler();
  }
  return mImpl->mVkSampler;
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali