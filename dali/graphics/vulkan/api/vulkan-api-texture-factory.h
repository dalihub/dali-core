#ifndef DALI_GRAPHICS_VULKAN_API_TEXTURE_FACTORY_H
#define DALI_GRAPHICS_VULKAN_API_TEXTURE_FACTORY_H

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

#include <dali/graphics-api/graphics-api-texture-factory.h>
#include <dali/graphics-api/graphics-api-texture-details.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class Graphics;
}
namespace VulkanAPI
{

class TextureFactory : public Dali::Graphics::API::TextureFactory
{
public:

  explicit TextureFactory( Vulkan::Graphics& graphics );

  Graphics::API::TextureFactory& SetType(API::TextureDetails::Type type) override;
  Graphics::API::TextureFactory& SetSize(const API::RectSize& size) override;
  Graphics::API::TextureFactory& SetFormat(API::TextureDetails::Format format) override;
  Graphics::API::TextureFactory& SetMipMapFlag(API::TextureDetails::MipMapFlag mipMSapFlag) override;
  Graphics::API::TextureFactory& SetData( void* pData ) override;
  Graphics::API::TextureFactory& SetDataSize( uint32_t dataSizeInBytes ) override;

  // not copyable
  TextureFactory(const TextureFactory&) = delete;
  TextureFactory& operator=(const TextureFactory&) = delete;

  ~TextureFactory() override;

  std::unique_ptr<Graphics::API::Texture> Create() const override;

  /** Internal interface */
  const API::TextureDetails::Type& GetType() const;
  const API::RectSize& GetSize() const;
  const API::TextureDetails::Format& GetFormat() const;
  const API::TextureDetails::MipMapFlag& GetMipMapFlag() const;
  const void* GetData() const;
   uint32_t GetDataSize() const;

  Vulkan::Graphics& GetGraphics() const;

protected:
  /// @brief default constructor
  TextureFactory() = default;

  // derived types should not be moved direcly to prevent slicing
  TextureFactory(TextureFactory&&) = default;
  TextureFactory& operator=(TextureFactory&&) = default;

private:

  struct Impl;
  std::unique_ptr<Impl> mImpl;
};

}
}
}

#endif //DALI_GRAPHICS_VULKAN_API_TEXTURE_FACTORY_H
