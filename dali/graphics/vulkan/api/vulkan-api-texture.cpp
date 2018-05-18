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

#include <dali/graphics/vulkan/api/vulkan-api-texture.h>

#include <dali/graphics/vulkan/api/vulkan-api-texture-factory.h>
#include <dali/graphics-api/graphics-api-texture-details.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-allocator.h>
#include <dali/graphics/vulkan/gpu-memory/vulkan-gpu-memory-manager.h>


namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{
using namespace Dali::Graphics::Vulkan;

struct Texture::Impl
{
  Impl( Texture& api, Dali::Graphics::API::TextureFactory& factory )
  : mTextureFactory( dynamic_cast<VulkanAPI::TextureFactory&>( factory ) ),
    mGraphics( mTextureFactory.GetGraphics() )
  {
  }

  ~Impl() = default;

  bool Initialise()
  {
    //return mImpl->CreateTexture( data, sizeInBytes, width, height );
    // AB: yup, yet another hack
    auto size = mTextureFactory.GetSize();
    auto width = size.width;
    auto height = size.height;
    auto sizeInBytes = mTextureFactory.GetDataSize();
    auto data = mTextureFactory.GetData();

    auto textureRef = Dali::Graphics::Vulkan::Texture::New( mGraphics, U32(size.width), U32(size.height), vk::Format::eR8G8B8A8Unorm );

    // check bpp, if 24bpp convert
    if( sizeInBytes == width*height*3 )
    {

      auto inData = reinterpret_cast<const uint8_t*>(data);
      auto outData = new uint8_t[width*height*4];

      auto outIdx = 0u;
      for( auto i = 0u; i < sizeInBytes; i+= 3 )
      {
        outData[outIdx] = inData[i];
        outData[outIdx+1] = inData[i+1];
        outData[outIdx+2] = inData[i+2];
        outData[outIdx+3] = 0xff;
        outIdx += 4;
      }

      data = outData;
      sizeInBytes = U32(width*height*4);
    }

    // Upload data immediately. Will stall the queue :(
    textureRef->UploadData( data, sizeInBytes, TextureUploadMode::eImmediate );

    mTexture = textureRef;
    return true;
  }

  Vulkan::RefCountedTexture mTexture;
  VulkanAPI::TextureFactory& mTextureFactory;
  Vulkan::Graphics& mGraphics;
};

Texture::Texture( Dali::Graphics::API::TextureFactory& factory )
{
  mImpl = std::make_unique<Impl>( *this, factory );
}

Texture::~Texture() = default;

Vulkan::RefCountedTexture Texture::GetTextureRef() const
{
  return mImpl->mTexture;
}

bool Texture::Initialise()
{
  return mImpl->Initialise();
}

} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali