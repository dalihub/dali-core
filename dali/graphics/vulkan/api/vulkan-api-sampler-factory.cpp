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

#include <dali/graphics/vulkan/api/vulkan-api-sampler-factory.h>
#include <dali/graphics/vulkan/api/vulkan-api-sampler.h>

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{

API::SamplerFactory& SamplerFactory::SetAddressModeU( API::SamplerAddressMode mode )
{
  mAddressModeU = mode;
  return *this;
}

API::SamplerFactory& SamplerFactory::SetAddressModeV( API::SamplerAddressMode mode )
{
  mAddressModeV = mode;
  return *this;
}

API::SamplerFactory& SamplerFactory::SetAddressModeW( API::SamplerAddressMode mode )
{
  mAddressModeW = mode;
  return *this;
}

API::SamplerFactory& SamplerFactory::SetMinFilter( API::SamplerFilter filter )
{
  mMinFilter = filter;
  return *this;
}

API::SamplerFactory& SamplerFactory::SetMagFilter( API::SamplerFilter filter )
{
  mMagFilter = filter;
  return *this;
}

API::SamplerFactory& SamplerFactory::SetMipmapMode( API::SamplerMipmapMode mipmapMode )
{
  mMipmapMode = mipmapMode;
  return *this;
}

API::SamplerFactory& SamplerFactory::SetAnisotropyEnable( bool anisotropyEnable )
{
  mAnisotropyEnable = anisotropyEnable;
  return *this;
}

API::SamplerFactory& SamplerFactory::SetMaxAnisotropy( float maxAnisotropy )
{
  mMaxAnisotropy = maxAnisotropy;
  return *this;
}

API::SamplerFactory& SamplerFactory::SetMinLod( float minLod )
{
  mMinLod = minLod;
  return *this;
}

API::SamplerFactory& SamplerFactory::SetMaxLod( float maxLod )
{
  mMaxLod = maxLod;
  return *this;
}

API::SamplerFactory& SamplerFactory::SetUnnormalizeCoordinates( bool unnormalizedCoordinates )
{
  mUnnormalizedCoordinates = unnormalizedCoordinates;
  return *this;
}

API::SamplerFactory& SamplerFactory::SetCompareEnable( bool compareEnable )
{
  mCompareEnable = compareEnable;
  return *this;
}

API::SamplerFactory& SamplerFactory::SetCompareOp( API::CompareOp compareOp )
{
  mCompareOp = compareOp;
  return *this;
}

SamplerFactory& SamplerFactory::Reset()
{
  mAddressModeU = API::SamplerAddressMode{};
  mAddressModeV = API::SamplerAddressMode{};
  mAddressModeW = API::SamplerAddressMode{};
  mMinFilter = API::SamplerFilter{};
  mMagFilter = API::SamplerFilter{};
  mMipmapMode = API::SamplerMipmapMode{};
  mCompareOp = API::CompareOp{};
  mMaxAnisotropy = 0.0f;
  mMinLod = 0.0f;
  mMaxLod = 0.0f;
  mCompareEnable = false;
  mAnisotropyEnable = false;
  mUnnormalizedCoordinates = false;
  return *this;
}

std::unique_ptr< Graphics::API::Sampler > SamplerFactory::Create() const
{
  return std::unique_ptr<VulkanAPI::Sampler>( new VulkanAPI::Sampler( mController, *this ) );
}

} // namespace API
} // namespace Graphics
} // namespace Dali
