#ifndef DALI_VULKAN_API_SAMPLER_FACTORY_H
#define DALI_VULKAN_API_SAMPLER_FACTORY_H

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

#include <dali/graphics-api/graphics-api-sampler-factory.h>
#include "vulkan-api-controller.h"

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{

/**
 * @brief Interface class for ShaderFactory types in the graphics API.
 */
class SamplerFactory : public API::SamplerFactory
{
public:

  SamplerFactory( Controller& controller )
  : mController( controller )
  {

  }

  API::SamplerFactory& SetAddressModeU( API::SamplerAddressMode mode ) override;

  API::SamplerFactory& SetAddressModeV( API::SamplerAddressMode mode ) override;

  API::SamplerFactory& SetAddressModeW( API::SamplerAddressMode mode ) override;

  API::SamplerFactory& SetMinFilter( API::SamplerFilter filter ) override;

  API::SamplerFactory& SetMagFilter( API::SamplerFilter filter ) override;

  API::SamplerFactory& SetMipmapMode( API::SamplerMipmapMode mipmapMode ) override;

  API::SamplerFactory& SetAnisotropyEnable( bool anisotropyEnable ) override;

  API::SamplerFactory& SetMaxAnisotropy( float maxAnisotropy ) override;

  API::SamplerFactory& SetMinLod( float minLod ) override;

  API::SamplerFactory& SetMaxLod( float maxLod ) override;

  API::SamplerFactory& SetUnnormalizeCoordinates( bool unnormalizedCoordinates ) override;

  API::SamplerFactory& SetCompareEnable( bool compareEnable ) override;

  API::SamplerFactory& SetCompareOp( API::CompareOp compareOp ) override;

  // not copyable
  SamplerFactory(const API::SamplerFactory&) = delete;
  SamplerFactory& operator=(const SamplerFactory&) = delete;

  ~SamplerFactory() = default;

  PointerType Create() const override;

protected:

  /// @brief default constructor
  SamplerFactory() = default;

  // derived types should not be moved directly to prevent slicing
  SamplerFactory(SamplerFactory&&) = default;
  SamplerFactory& operator=(SamplerFactory&&) = default;

public:

  SamplerFactory& Reset();

  VulkanAPI::Controller& mController;

  API::SamplerAddressMode mAddressModeU {};
  API::SamplerAddressMode mAddressModeW {};
  API::SamplerAddressMode mAddressModeV {};
  API::SamplerFilter      mMinFilter {};
  API::SamplerFilter      mMagFilter {};
  API::SamplerMipmapMode  mMipmapMode {};
  API::CompareOp          mCompareOp {};
  float                   mMaxAnisotropy { 0.0f };
  float                   mMinLod { 0.0f };
  float                   mMaxLod { 0.0f };
  bool                    mCompareEnable {};
  bool                    mAnisotropyEnable { false };
  bool                    mUnnormalizedCoordinates { false };


};

} // namespace API
} // namespace Graphics
} // namespace Dali

#endif // DALI_VULKAN_API_SAMPLER_FACTORY_H
