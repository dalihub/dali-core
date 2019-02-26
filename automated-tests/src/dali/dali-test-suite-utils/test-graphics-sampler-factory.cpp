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
class GraphicsSampler;

GraphicsSamplerFactory::GraphicsSamplerFactory(GraphicsController* controller)
: mController(*controller)
{
}
GraphicsSamplerFactory::~GraphicsSamplerFactory() = default;

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetAddressModeU( Dali::Graphics::SamplerAddressMode mode )
{
  mCreateInfo.mAddressModeU = mode;
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetAddressModeV( Dali::Graphics::SamplerAddressMode mode )
{
  mCreateInfo.mAddressModeV = mode;
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetAddressModeW( Dali::Graphics::SamplerAddressMode mode )
{
  mCreateInfo.mAddressModeW = mode;
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetMinFilter( Dali::Graphics::SamplerFilter filter )
{
  mCreateInfo.mMinFilter = filter;
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetMagFilter( Dali::Graphics::SamplerFilter filter )
{
  mCreateInfo.mMagFilter = filter;
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetMipmapMode( Dali::Graphics::SamplerMipmapMode mipmapMode )
{
  mCreateInfo.mMipmapMode = mipmapMode;
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetAnisotropyEnable( bool anisotropyEnable )
{
  mCreateInfo.mAnisotropyEnable = anisotropyEnable;
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetMaxAnisotropy( float maxAnisotropy )
{
  mCreateInfo.mMaxAnisotropy = maxAnisotropy;
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetMinLod( float minLod )
{
  mCreateInfo.mMinLod = minLod;
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetMaxLod( float maxLod )
{
  mCreateInfo.mMaxLod = maxLod;
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetUnnormalizeCoordinates( bool unnormalizedCoordinates )
{
  mCreateInfo.mUnnormalizedCoordinates = unnormalizedCoordinates;
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetCompareEnable( bool compareEnable )
{
  mCreateInfo.mCompareEnable = compareEnable;
  return *this;
}

Dali::Graphics::SamplerFactory& GraphicsSamplerFactory::SetCompareOp( Dali::Graphics::CompareOp compareOp )
{
  mCreateInfo.mCompareOp = compareOp;
  return *this;
}

Dali::Graphics::SamplerFactory::PointerType GraphicsSamplerFactory::Create() const
{
  return std::unique_ptr<GraphicsSampler>( new GraphicsSampler( mController, mCreateInfo ) );
};


void GraphicsSamplerFactory::TestReset()
{
  mCreateInfo.mAddressModeU = Dali::Graphics::SamplerAddressMode{};
  mCreateInfo.mAddressModeV = Dali::Graphics::SamplerAddressMode{};
  mCreateInfo.mAddressModeW = Dali::Graphics::SamplerAddressMode{};
  mCreateInfo.mMinFilter = Dali::Graphics::SamplerFilter{};
  mCreateInfo.mMagFilter = Dali::Graphics::SamplerFilter{};
  mCreateInfo.mMipmapMode = Dali::Graphics::SamplerMipmapMode{};
  mCreateInfo.mCompareOp = Dali::Graphics::CompareOp{};
  mCreateInfo.mMaxAnisotropy = 1.0f;
  mCreateInfo.mMinLod = 0.0f;
  mCreateInfo.mMaxLod = 0.0f;
  mCreateInfo.mCompareEnable = false;
  mCreateInfo.mAnisotropyEnable = false;
  mCreateInfo.mUnnormalizedCoordinates = false;
}


} // Test
