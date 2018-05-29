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

#include <dali/graphics/vulkan/api/vulkan-api-texture-factory.h>
#include <dali/graphics/vulkan/api/vulkan-api-texture.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{

struct TextureFactory::Impl
{
  Impl( TextureFactory& api, Vulkan::Graphics& graphics )
  : mApi( api ), mGraphics( graphics )
  {

  }

  ~Impl() = default;

  std::unique_ptr<Graphics::API::Texture> Create()
  {
    auto retval = std::make_unique<VulkanAPI::Texture>( static_cast<Graphics::API::TextureFactory&>(mApi) );

    if(retval->Initialise())
    {
      return std::move(retval);
    }

    return nullptr;
  }

  TextureFactory& mApi;
  Vulkan::Graphics& mGraphics;

  API::TextureDetails::Type       mType;
  API::RectSize                   mSize;
  API::TextureDetails::Format     mFormat;
  API::TextureDetails::MipMapFlag mMipmapFlags;
  void*                           mData;
  uint32_t                        mDataSizeInBytes;

};

TextureFactory::TextureFactory( Vulkan::Graphics& graphics )
{
  mImpl = std::make_unique<Impl>( *this, graphics );
}

TextureFactory::~TextureFactory() = default;

Graphics::API::TextureFactory& TextureFactory::SetType(API::TextureDetails::Type type)
{
  mImpl->mType = type;
  return *this;
}

Graphics::API::TextureFactory& TextureFactory::SetSize(const API::RectSize& size)
{
  mImpl->mSize = size;
  return *this;
}

Graphics::API::TextureFactory& TextureFactory::SetFormat(API::TextureDetails::Format format)
{
  mImpl->mFormat = format;
  return *this;
}

Graphics::API::TextureFactory& TextureFactory::SetMipMapFlag(API::TextureDetails::MipMapFlag mipMSapFlag)
{
  mImpl->mMipmapFlags = mipMSapFlag;
  return *this;
}

Graphics::API::TextureFactory& TextureFactory::SetData( void* pData )
{
  mImpl->mData = pData;
  return *this;
}

Graphics::API::TextureFactory& TextureFactory::SetDataSize( uint32_t dataSizeInBytes )
{
  mImpl->mDataSizeInBytes = dataSizeInBytes;
  return *this;
}

std::unique_ptr<Graphics::API::Texture> TextureFactory::Create() const
{
  return mImpl->Create();
}

const API::TextureDetails::Type& TextureFactory::GetType() const
{
  return mImpl->mType;
}

const API::RectSize& TextureFactory::GetSize() const
{
  return mImpl->mSize;
}

const API::TextureDetails::Format& TextureFactory::GetFormat() const
{
  return mImpl->mFormat;
}

const API::TextureDetails::MipMapFlag& TextureFactory::GetMipMapFlag() const
{
  return mImpl->mMipmapFlags;
}

const void* TextureFactory::GetData() const
{
  return mImpl->mData;
}

uint32_t TextureFactory::GetDataSize() const
{
  return mImpl->mDataSizeInBytes;
}

Vulkan::Graphics& TextureFactory::GetGraphics() const
{
  return mImpl->mGraphics;
}


}
}
}

