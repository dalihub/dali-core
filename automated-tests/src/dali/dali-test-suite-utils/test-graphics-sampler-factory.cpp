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

#include "test-graphics-sampler-factory.h"

namespace Test
{


GraphicsSamplerFactory::GraphicsSamplerFactory()
{
}

GraphicsSamplerFactory::~GraphicsSamplerFactory() = default;

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetAddressModeU( Dali::Graphics::SamplerAddressMode mode )
{
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetAddressModeV( Dali::Graphics::SamplerAddressMode mode )
{
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetAddressModeW( Dali::Graphics::SamplerAddressMode mode )
{
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetMinFilter( Dali::Graphics::SamplerFilter filter )
{
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetMagFilter( Dali::Graphics::SamplerFilter filter )
{
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetMipmapMode( Dali::Graphics::SamplerMipmapMode mipmapMode )
{
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetAnisotropyEnable( bool anisotropyEnable )
{
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetMaxAnisotropy( float maxAnisotropy )
{
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetMinLod( float minLod )
{
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetMaxLod( float maxLod )
{
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetUnnormalizeCoordinates( bool unnormalizedCoordinates )
{
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetCompareEnable( bool compareEnable )
{
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetCompareOp( Dali::Graphics::CompareOp compareOp )
{
  return *this;
}

Dali::Graphics::SamplerFactory::PointerType GraphicsSamplerFactory::Create() const
{
  return nullptr;
};


} // Test
