#ifndef TEST_GRAPHICS_TEXTURE_FACTORY_H
#define TEST_GRAPHICS_TEXTURE_FACTORY_H

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

#include <dali/graphics-api/graphics-api-texture-factory.h>

namespace Test
{

class GraphicsTextureFactory : public Dali::Graphics::API::TextureFactory
{
public:
  GraphicsTextureFactory();
  virtual ~GraphicsTextureFactory();

  Dali::Graphics::API::TextureFactory& SetType(Dali::Graphics::API::TextureDetails::Type type) override;
  Dali::Graphics::API::TextureFactory& SetSize(const Dali::Graphics::API::RectSize& size) override;
  Dali::Graphics::API::TextureFactory& SetFormat(Dali::Graphics::API::TextureDetails::Format format) override;
  Dali::Graphics::API::TextureFactory& SetMipMapFlag(Dali::Graphics::API::TextureDetails::MipMapFlag mipMapFlag) override;
  Dali::Graphics::API::TextureFactory& SetUsage( Dali::Graphics::API::TextureDetails::Usage usage ) override;
  Dali::Graphics::API::TextureFactory& SetData( void* pData ) override;
  Dali::Graphics::API::TextureFactory& SetDataSize( uint32_t dataSizeInBytes ) override;
  Dali::Graphics::API::TextureFactory::PointerType Create() const override;
};

} // Test

#endif //TEST_GRAPHICS_TEXTURE_FACTORY_H
