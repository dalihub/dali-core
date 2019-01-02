
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

#include "test-graphics-texture-factory.h"

namespace Test
{

GraphicsTextureFactory::GraphicsTextureFactory() = default;
GraphicsTextureFactory::~GraphicsTextureFactory() = default;

Dali::Graphics::API::TextureFactory& GraphicsTextureFactory::SetType(Dali::Graphics::API::TextureDetails::Type type)
{
  return *this;
}
Dali::Graphics::API::TextureFactory& GraphicsTextureFactory::SetSize(const Dali::Graphics::API::RectSize& size)
{
  return *this;
}
Dali::Graphics::API::TextureFactory& GraphicsTextureFactory::SetFormat(Dali::Graphics::API::TextureDetails::Format format)
{
  return *this;
}
Dali::Graphics::API::TextureFactory& GraphicsTextureFactory::SetMipMapFlag(Dali::Graphics::API::TextureDetails::MipMapFlag mipMapFlag)
{
  return *this;
}
Dali::Graphics::API::TextureFactory& GraphicsTextureFactory::SetUsage( Dali::Graphics::API::TextureDetails::Usage usage )
{
  return *this;
}
Dali::Graphics::API::TextureFactory& GraphicsTextureFactory::SetData( void* pData )
{
  return *this;
}
Dali::Graphics::API::TextureFactory& GraphicsTextureFactory::SetDataSize( uint32_t dataSizeInBytes )
{
  return *this;
}
Dali::Graphics::API::TextureFactory::PointerType GraphicsTextureFactory::Create() const
{
  return nullptr;
}

} // Test
