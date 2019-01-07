#ifndef DALI_GRAPHICS_API_SAMPLER_FACTORY_H
#define DALI_GRAPHICS_API_SAMPLER_FACTORY_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include <dali/graphics-api/graphics-api-base-factory.h>
#include <dali/graphics-api/graphics-api-sampler.h>
#include <dali/graphics-api/graphics-api-types.h>

namespace Dali
{
namespace Graphics
{

/**
 * @brief Interface class for ShaderFactory types in the graphics API.
 */
class SamplerFactory : public BaseFactory<Sampler>
{
public:

  virtual SamplerFactory& SetAddressModeU( SamplerAddressMode mode ) = 0;

  virtual SamplerFactory& SetAddressModeV( SamplerAddressMode mode ) = 0;

  virtual SamplerFactory& SetAddressModeW( SamplerAddressMode mode ) = 0;

  virtual SamplerFactory& SetMinFilter( SamplerFilter filter ) = 0;

  virtual SamplerFactory& SetMagFilter( SamplerFilter filter ) = 0;

  virtual SamplerFactory& SetMipmapMode( SamplerMipmapMode mipmapMode ) = 0;

  virtual SamplerFactory& SetAnisotropyEnable( bool anisotropyEnable ) = 0;

  virtual SamplerFactory& SetMaxAnisotropy( float maxAnisotropy ) = 0;

  virtual SamplerFactory& SetMinLod( float minLod ) = 0;

  virtual SamplerFactory& SetMaxLod( float maxLod ) = 0;

  virtual SamplerFactory& SetUnnormalizeCoordinates( bool unnormalizedCoordinates ) = 0;

  virtual SamplerFactory& SetCompareEnable( bool compareEnable ) = 0;

  virtual SamplerFactory& SetCompareOp( CompareOp compareOp ) = 0;

  // not copyable
  SamplerFactory(const SamplerFactory&) = delete;
  SamplerFactory& operator=(const SamplerFactory&) = delete;

  virtual ~SamplerFactory() = default;

protected:

  /// @brief default constructor
  SamplerFactory() = default;

  // derived types should not be moved directly to prevent slicing
  SamplerFactory(SamplerFactory&&) = default;
  SamplerFactory& operator=(SamplerFactory&&) = default;
};

} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_SHADER_FACTORY_H
