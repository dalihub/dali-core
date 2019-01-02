#ifndef TEST_GRAPHICS_SAMPLER_FACTORY_H
#define TEST_GRAPHICS_SAMPLER_FACTORY_H

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
 */

#include <dali/graphics-api/graphics-api-sampler-factory.h>

namespace Test
{


class GraphicsSamplerFactory : public Dali::Graphics::API::SamplerFactory
{
public:
  GraphicsSamplerFactory();
  virtual ~GraphicsSamplerFactory();

  Dali::Graphics::API::SamplerFactory& SetAddressModeU( Dali::Graphics::API::SamplerAddressMode mode ) override;

  Dali::Graphics::API::SamplerFactory& SetAddressModeV( Dali::Graphics::API::SamplerAddressMode mode ) override;

  Dali::Graphics::API::SamplerFactory& SetAddressModeW( Dali::Graphics::API::SamplerAddressMode mode ) override;

  Dali::Graphics::API::SamplerFactory& SetMinFilter( Dali::Graphics::API::SamplerFilter filter ) override;

  Dali::Graphics::API::SamplerFactory& SetMagFilter( Dali::Graphics::API::SamplerFilter filter ) override;

  Dali::Graphics::API::SamplerFactory& SetMipmapMode( Dali::Graphics::API::SamplerMipmapMode mipmapMode ) override;

  Dali::Graphics::API::SamplerFactory& SetAnisotropyEnable( bool anisotropyEnable ) override;

  Dali::Graphics::API::SamplerFactory& SetMaxAnisotropy( float maxAnisotropy ) override;

  Dali::Graphics::API::SamplerFactory& SetMinLod( float minLod ) override;

  Dali::Graphics::API::SamplerFactory& SetMaxLod( float maxLod ) override;

  Dali::Graphics::API::SamplerFactory& SetUnnormalizeCoordinates( bool unnormalizedCoordinates ) override;

  Dali::Graphics::API::SamplerFactory& SetCompareEnable( bool compareEnable ) override;

  Dali::Graphics::API::SamplerFactory& SetCompareOp( Dali::Graphics::API::CompareOp compareOp ) override;

  Dali::Graphics::API::SamplerFactory::PointerType Create() const override;
};

} // Test

#endif //TEST_GRAPHICS_SAMPLER_FACTORY_H
