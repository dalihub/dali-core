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

/**
 * @brief Whether specified pixel format is compressed.
 *
 * @param [in] pixelformat Pixel format
 * @return true if format is compressed, false otherwise
 */
bool IsCompressedFormat(Pixel::Format pixelFormat)
{
  switch (pixelFormat)
  {
    case Pixel::L8:
    case Pixel::A8:
    case Pixel::LA88:
    case Pixel::RGB565:
    case Pixel::RGBA4444:
    case Pixel::RGBA5551:
    case Pixel::BGR565:
    case Pixel::BGRA4444:
    case Pixel::BGRA5551:
    case Pixel::RGB888:
    case Pixel::RGB8888:
    case Pixel::BGR8888:
    case Pixel::RGBA8888:
    case Pixel::BGRA8888:
    case Pixel::RGB16F:
    case Pixel::RGB32F:
    case Pixel::INVALID:
    {
      return false;
    }

    case Pixel::COMPRESSED_R11_EAC:
    case Pixel::COMPRESSED_SIGNED_R11_EAC:
    case Pixel::COMPRESSED_RG11_EAC:
    case Pixel::COMPRESSED_SIGNED_RG11_EAC:
    case Pixel::COMPRESSED_RGB8_ETC2:
    case Pixel::COMPRESSED_SRGB8_ETC2:
    case Pixel::COMPRESSED_RGB8_ETC1:
    case Pixel::COMPRESSED_RGB_PVRTC_4BPPV1:
    case Pixel::COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case Pixel::COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case Pixel::COMPRESSED_RGBA8_ETC2_EAC:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
    case Pixel::COMPRESSED_RGBA_ASTC_4x4_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_5x4_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_5x5_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_6x5_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_6x6_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_8x5_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_8x6_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_8x8_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_10x5_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_10x6_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_10x8_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_10x10_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_12x10_KHR:
    case Pixel::COMPRESSED_RGBA_ASTC_12x12_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
    {
      return true;
    }
  }

  return false;
}

/**
 * Converts DALi pixel format to Graphics::Format
 * @param format
 * @return
 */
constexpr Graphics::Format ConvertPixelFormat( Pixel::Format format )
{
  switch( format )
  {
    case Pixel::INVALID:
      return Graphics::Format::UNDEFINED;
    case Pixel::A8:
      return Graphics::Format::R8_UNORM;
    case Pixel::L8:
      return Graphics::Format::L8;
    case Pixel::LA88:
      return Graphics::Format::L8A8;
    case Pixel::RGB565:
      return Graphics::Format::R5G6B5_UNORM_PACK16;
    case Pixel::BGR565:
      return Graphics::Format::B5G6R5_UNORM_PACK16;
    case Pixel::RGBA4444:
      return Graphics::Format::R4G4B4A4_UNORM_PACK16;
    case Pixel::BGRA4444:
      return Graphics::Format::B4G4R4A4_UNORM_PACK16;
    case Pixel::RGBA5551:
      return Graphics::Format::R5G5B5A1_UNORM_PACK16;
    case Pixel::BGRA5551:
      return Graphics::Format::B5G5R5A1_UNORM_PACK16;
    case Pixel::RGB888:
      return Graphics::Format::R8G8B8_UNORM;
    case Pixel::RGB8888:
      return Graphics::Format::R8G8B8A8_UNORM;
    case Pixel::BGR8888:
      return Graphics::Format::B8G8R8A8_UNORM;
    case Pixel::RGBA8888:
      return Graphics::Format::R8G8B8A8_UNORM;
    case Pixel::BGRA8888:
      return Graphics::Format::B8G8R8A8_UNORM;

    // EAC
    case Pixel::COMPRESSED_R11_EAC:
      return Graphics::Format::EAC_R11_UNORM_BLOCK;
    case Pixel::COMPRESSED_SIGNED_R11_EAC:
      return Graphics::Format::EAC_R11_SNORM_BLOCK;
    case Pixel::COMPRESSED_RG11_EAC:
      return Graphics::Format::EAC_R11G11_UNORM_BLOCK;
    case Pixel::COMPRESSED_SIGNED_RG11_EAC:
      return Graphics::Format::EAC_R11G11_SNORM_BLOCK;

    // ETC
    case Pixel::COMPRESSED_RGB8_ETC2:
      return Graphics::Format::ETC2_R8G8B8_UNORM_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ETC2:
      return Graphics::Format::ETC2_R8G8B8_SRGB_BLOCK;

    case Pixel::COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
      return Graphics::Format::ETC2_R8G8B8A1_UNORM_BLOCK; // no 'punchthrough' format

    case Pixel::COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
      return Graphics::Format::ETC2_R8G8B8A1_SRGB_BLOCK; // no 'punchthrough' format

    case Pixel::COMPRESSED_RGBA8_ETC2_EAC:
      return Graphics::Format::ETC2_R8G8B8_UNORM_BLOCK; // doesn't seem to map onto any format

    case Pixel::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
      return Graphics::Format::ETC2_R8G8B8A8_SRGB_BLOCK; // doesn't seem to map onto any format

    case Pixel::COMPRESSED_RGB8_ETC1:
      return Graphics::Format::ETC2_R8G8B8_UNORM_BLOCK; // doesn't seem to be supported at all

    case Pixel::COMPRESSED_RGB_PVRTC_4BPPV1:
      return Graphics::Format::PVRTC1_4BPP_UNORM_BLOCK_IMG; // or SRGB?

    // ASTC
    case Pixel::COMPRESSED_RGBA_ASTC_4x4_KHR:
      return Graphics::Format::ASTC_4x4_UNORM_BLOCK; // or SRGB?
    case Pixel::COMPRESSED_RGBA_ASTC_5x4_KHR:
      return Graphics::Format::ASTC_5x4_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_5x5_KHR:
      return Graphics::Format::ASTC_5x5_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_6x5_KHR:
      return Graphics::Format::ASTC_6x5_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_6x6_KHR:
      return Graphics::Format::ASTC_6x6_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_8x5_KHR:
      return Graphics::Format::ASTC_8x5_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_8x6_KHR:
      return Graphics::Format::ASTC_8x6_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_8x8_KHR:
      return Graphics::Format::ASTC_8x8_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_10x5_KHR:
      return Graphics::Format::ASTC_10x5_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_10x6_KHR:
      return Graphics::Format::ASTC_10x6_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_10x8_KHR:
      return Graphics::Format::ASTC_10x8_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_10x10_KHR:
      return Graphics::Format::ASTC_10x10_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_12x10_KHR:
      return Graphics::Format::ASTC_12x10_UNORM_BLOCK;
    case Pixel::COMPRESSED_RGBA_ASTC_12x12_KHR:
      return Graphics::Format::ASTC_12x12_UNORM_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:

      return Graphics::Format::ASTC_4x4_SRGB_BLOCK; // not type with alpha, but likely to use SRGB
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
      return Graphics::Format::ASTC_5x4_SRGB_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
      return Graphics::Format::ASTC_5x5_SRGB_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
      return Graphics::Format::ASTC_6x5_SRGB_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
      return Graphics::Format::ASTC_6x6_SRGB_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
      return Graphics::Format::ASTC_8x5_SRGB_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
      return Graphics::Format::ASTC_8x6_UNORM_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
      return Graphics::Format::ASTC_8x8_SRGB_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
      return Graphics::Format::ASTC_10x5_SRGB_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
      return Graphics::Format::ASTC_10x6_SRGB_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
      return Graphics::Format::ASTC_10x8_SRGB_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
      return Graphics::Format::ASTC_10x10_SRGB_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
      return Graphics::Format::ASTC_12x10_SRGB_BLOCK;
    case Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
      return Graphics::Format::ASTC_12x12_SRGB_BLOCK;
    case Pixel::RGB16F:
      return Graphics::Format::R16G16B16_SFLOAT;
    case Pixel::RGB32F:
      return Graphics::Format::R32G32B32_SFLOAT;
  }
  return Graphics::Format::UNDEFINED;
}

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
  mIsCompressed( IsCompressedFormat( format ) )
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
    CreateTexture( Usage::SAMPLE );
  }
}

Graphics::Texture* Texture::GetGfxObject() const
{
  DALI_LOG_INFO( gTextureFilter, Debug::General, "SC::Texture(%p)::GetGfxObject() = %p\n", this, mGraphicsTexture.get() );

  return mGraphicsTexture.get();
}

void Texture::UploadTexture( PixelDataPtr pixelData, const Internal::Texture::UploadParams& params )
{
  if( !mGraphicsTexture )
  {
    CreateTextureInternal( Usage::SAMPLE, nullptr, 0u );
  }

  // schedule upload
  mGraphicsTexture->CopyMemory( pixelData->GetBuffer(),
                                pixelData->GetBufferSize(),
                                { params.width, params.height },
                                { params.xOffset, params.yOffset },
                                params.layer,
                                params.mipmap,
                                {} );

  DALI_LOG_INFO( gTextureFilter, Debug::General, "SC::Texture(%p)::UploadTexture() GfxTexture: %p\n", this, mGraphicsTexture.get() );
}

void Texture::CreateTexture( Usage usage )
{
  if( ! mGraphicsTexture )
  {
    CreateTextureInternal( usage, nullptr, 0 );
  }
  DALI_LOG_INFO( gTextureFilter, Debug::General, "SC::Texture(%p)::CreateTexture(Usage:%s) GfxTexture: %p\n", this, (usage==Usage::COLOR_ATTACHMENT?"ColorAttachment":(usage==Usage::DEPTH_ATTACHMENT?"DepthAttachment":"SAMPLE")), mGraphicsTexture.get() );
}

void Texture::CreateTextureInternal( Usage usage, unsigned char* buffer, unsigned int bufferSize )
{
  if( mGraphicsController )
  {
    Graphics::TextureDetails::Usage graphicsUsage{};
    switch( usage )
    {
      case Usage::SAMPLE:
      {
        graphicsUsage = Graphics::TextureDetails::Usage::SAMPLE;
        break;
      }
      case Usage::COLOR_ATTACHMENT:
      {
        graphicsUsage = Graphics::TextureDetails::Usage::COLOR_ATTACHMENT;
        break;
      }
      case Usage::DEPTH_ATTACHMENT:
      {
        graphicsUsage = Graphics::TextureDetails::Usage::DEPTH_ATTACHMENT;
        break;
      }
    }

    // Convert DALi format to Graphics API format
    mGraphicsTexture = mGraphicsController->CreateTexture( mGraphicsController->GetTextureFactory()
                                                           .SetFormat( ConvertPixelFormat( mFormat ) )
                                                           .SetUsage( graphicsUsage )
                                                           .SetSize( { mWidth, mHeight } )
                                                           .SetType( Graphics::TextureDetails::Type::TEXTURE_2D )
                                                           .SetMipMapFlag( Graphics::TextureDetails::MipMapFlag::DISABLED )
                                                           .SetData( buffer )
                                                           .SetDataSize( bufferSize )
                                                           .SetNativeImage( mNativeImage )
                                                           .SetTiling( Graphics::TextureTiling::LINEAR ));
  }
}

void Texture::PrepareTexture()
{
  if( mNativeImage )
  {
    //TODO : use NativeImageInterface::Extension::IsSetSource for Native Surface
    NativeImageInterface::Extension* extension = mNativeImage->GetExtension();
    if ( extension != NULL )
    {
      if ( extension->IsSetSource() )
      {
        // When native image is changed, are the width and height changed?
        mGraphicsTexture.reset();
        mWidth = uint16_t(mNativeImage->GetWidth());
        mHeight = uint16_t(mNativeImage->GetHeight());
        mHasAlpha= mNativeImage->RequiresBlending();
        mGraphicsTexture = mGraphicsController->CreateTexture( mGraphicsController->GetTextureFactory()
                                                               .SetFormat( ConvertPixelFormat( mFormat ) )
                                                               .SetUsage( Graphics::TextureDetails::Usage::SAMPLE )
                                                               .SetSize( { mWidth, mHeight } )
                                                               .SetType( Graphics::TextureDetails::Type::TEXTURE_2D )
                                                               .SetMipMapFlag( Graphics::TextureDetails::MipMapFlag::DISABLED )
                                                               .SetData( nullptr )
                                                               .SetDataSize( 0 )
                                                               .SetNativeImage( mNativeImage ));
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
