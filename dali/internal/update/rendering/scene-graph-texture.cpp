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
 */

// CLASS HEADER
#include <dali/internal/update/rendering/scene-graph-texture.h>

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/images/image-operations.h> // Dali::ImageDimensions
#include <dali/public-api/rendering/sampler.h>
#include <dali/public-api/rendering/texture.h>
#include <dali/devel-api/images/native-image-interface-extension.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/rendering/texture-impl.h>
#include <dali/internal/update/rendering/scene-graph-sampler.h>
#include <dali/graphics-api/graphics-controller.h>
#include <dali/graphics-api/graphics-texture.h>
#include <dali/graphics-api/graphics-texture-upload-helper.h>
#include <dali/graphics-api/graphics-types.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

namespace
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gTextureFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_TEXTURE");
#endif
} //Unnamed namespace


Texture::Texture( Type type, Pixel::Format format, ImageDimensions size )
: mGraphicsController( nullptr ),
  mGraphicsTexture( nullptr ),
  mNativeImage(),
  mSampler(),
  mFormat( format ),
  mWidth( size.GetWidth() ),
  mHeight( size.GetHeight() ),
  mMaxMipMapLevel( 0 ),
  mType( type ),
  mHasAlpha( HasAlpha( format ) ),
  mIsCompressed( Pixel::IsCompressed( format ) )
{
}

Texture::Texture( NativeImageInterfacePtr nativeImageInterface )
: mGraphicsController( nullptr ),
  mGraphicsTexture( nullptr ),
  mNativeImage( nativeImageInterface ),
  mSampler(),
  mFormat( Pixel::RGBA8888 ),
  mWidth( uint16_t(nativeImageInterface->GetWidth()) ),
  mHeight( uint16_t(nativeImageInterface->GetHeight()) ),
  mMaxMipMapLevel( 0 ),
  mType( TextureType::TEXTURE_2D ),
  mHasAlpha( nativeImageInterface->RequiresBlending() ),
  mIsCompressed( false )
{
}

Texture::~Texture() = default;

void Texture::Initialize( Graphics::Controller& graphicsController )
{
  mGraphicsController = &graphicsController;
  if (mNativeImage)
  {
    CreateTexture( 0|Graphics::TextureUsageFlagBits::SAMPLE );
  }
}

Graphics::Texture* Texture::GetGraphicsObject() const
{
  DALI_LOG_INFO( gTextureFilter, Debug::General, "SC::Texture(%p)::GetGraphicsObject() = %p\n", this, mGraphicsTexture.get() );

  return mGraphicsTexture.get();
}

void Texture::UploadTexture( PixelDataPtr pixelData, const Internal::Texture::UploadParams& params )
{
  if( !mGraphicsTexture )
  {
    CreateTextureInternal( 0|Graphics::TextureUsageFlagBits::SAMPLE, nullptr, 0u );
  }

  Graphics::TextureUpdateInfo info{};

  const uint32_t srcStride = pixelData->GetWidth();
  uint32_t       srcOffset = 0u;
  uint32_t       srcSize   = pixelData->GetBufferSize();

  const bool requiredSubPixelData = (!Pixel::IsCompressed(pixelData->GetPixelFormat())) &&
                                    ((params.xOffset != 0) ||
                                     (params.yOffset != 0) ||
                                     (params.width != pixelData->GetWidth()) ||
                                     (params.height != pixelData->GetHeight()));
  if(requiredSubPixelData)
  {
    const uint32_t bytePerPixel    = Pixel::GetBytesPerPixel(pixelData->GetPixelFormat());
    const uint32_t dataStrideByte  = (srcStride ? srcStride : static_cast<uint32_t>(params.width)) * bytePerPixel;
    const uint32_t dataXOffsetByte = params.xOffset * bytePerPixel;
    const uint32_t dataWidthByte   = static_cast<uint32_t>(params.width) * bytePerPixel;
    srcOffset = params.yOffset * dataStrideByte + dataXOffsetByte;
    srcSize   = static_cast<uint32_t>(params.height) * dataStrideByte - (dataStrideByte - dataWidthByte);
  }

  info.dstTexture   = mGraphicsTexture.get();
  info.dstOffset2D  = {params.xOffset, params.yOffset};
  info.layer        = params.layer;
  info.level        = params.mipmap;
  info.srcReference = 0;
  info.srcExtent2D  = {params.width, params.height};
  info.srcOffset    = srcOffset;
  info.srcSize      = srcSize;
  info.srcStride    = srcStride;
  info.srcFormat    = Dali::Graphics::ConvertPixelFormat(pixelData->GetPixelFormat());

  Graphics::TextureUpdateSourceInfo updateSourceInfo{};
  updateSourceInfo.sourceType                = Graphics::TextureUpdateSourceInfo::Type::MEMORY;
  updateSourceInfo.memorySource.memory = pixelData->GetBuffer();

  mGraphicsController->UpdateTextures({info}, {updateSourceInfo});


  DALI_LOG_INFO( gTextureFilter, Debug::General, "SC::Texture(%p)::UploadTexture() GfxTexture: %p\n", this, mGraphicsTexture.get() );
}

void Texture::CreateTexture( Graphics::TextureUsageFlags usage )
{
  if( ! mGraphicsTexture )
  {
    CreateTextureInternal( usage, nullptr, 0 );
  }
  DALI_LOG_INFO( gTextureFilter, Debug::General, "SC::Texture(%p)::CreateTexture(Usage:%02x) GfxTexture: %p\n",
                 this, usage);
}

void Texture::CreateTextureInternal( Graphics::TextureUsageFlags usage, unsigned char* buffer, unsigned int bufferSize )
{
  if( mGraphicsController )
  {
    // Convert DALi format to Graphics API format
    auto createInfo = Graphics::TextureCreateInfo();
    createInfo
      .SetTextureType(Dali::Graphics::ConvertTextureType(mType))
      .SetUsageFlags(usage)
      .SetFormat(Dali::Graphics::ConvertPixelFormat(mFormat))
      .SetSize({mWidth, mHeight})
      .SetLayout(Graphics::TextureLayout::LINEAR)
      .SetAllocationPolicy(Graphics::TextureAllocationPolicy::CREATION)
      .SetData(buffer)
      .SetDataSize(bufferSize)
      .SetNativeImage(mNativeImage)
      .SetMipMapFlag(Graphics::TextureMipMapFlag::DISABLED);

    mGraphicsTexture = mGraphicsController->CreateTexture( createInfo, std::move(mGraphicsTexture));
  }
}

void Texture::PrepareTexture()
{
  if( mNativeImage )
  {
    //TODO : use NativeImageInterface::Extension::IsSetSource for Native Surface
    NativeImageInterface::Extension* extension = mNativeImage->GetExtension();
    if ( extension != nullptr )
    {
      if ( extension->IsSetSource() )
      {
        // When native image is changed, are the width and height changed?
        mGraphicsTexture.reset();
        mWidth = uint16_t(mNativeImage->GetWidth());
        mHeight = uint16_t(mNativeImage->GetHeight());
        mHasAlpha= mNativeImage->RequiresBlending();

        auto createInfo = Graphics::TextureCreateInfo();
        createInfo
          .SetTextureType(Dali::Graphics::ConvertTextureType(mType))
          .SetUsageFlags(0|Graphics::TextureUsageFlagBits::SAMPLE)
          .SetFormat(Dali::Graphics::ConvertPixelFormat(mFormat))
          .SetSize({mWidth, mHeight})
          .SetLayout(Graphics::TextureLayout::LINEAR)
          .SetAllocationPolicy(Graphics::TextureAllocationPolicy::CREATION)
          .SetData(nullptr)
          .SetDataSize(0)
          .SetNativeImage(mNativeImage)
          .SetMipMapFlag(Graphics::TextureMipMapFlag::DISABLED);

        mGraphicsTexture = mGraphicsController->CreateTexture( createInfo, std::move(mGraphicsTexture));
      }
    }
    mNativeImage->PrepareTexture();
  }
}

bool Texture::HasAlphaChannel()
{
  return mHasAlpha;
}

void Texture::GenerateMipmaps()
{
  mMaxMipMapLevel = 0;
  DALI_LOG_ERROR( "FIXME: GRAPHICS\n");
  //@todo Implement with GraphicsAPI
}

void Texture::DestroyGraphicsObjects()
{
  mGraphicsTexture.reset();
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
