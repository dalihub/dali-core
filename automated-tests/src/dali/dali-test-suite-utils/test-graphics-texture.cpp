/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

#include "test-graphics-texture.h"

#include <iostream>
#include <sstream>

namespace Dali
{

/**
 * @brief Whether specified pixel format is compressed.
 *
 * @param [in] pixelformat Pixel format
 * @return true if format is compressed, false otherwise
 */
bool IsCompressedFormat( Graphics::Format pixelFormat )
{
  switch( pixelFormat )
  {
    case Graphics::Format::UNDEFINED:
    case Graphics::Format::L8:
    case Graphics::Format::L8A8:
    case Graphics::Format::R4G4_UNORM_PACK8:
    case Graphics::Format::R4G4B4A4_UNORM_PACK16:
    case Graphics::Format::B4G4R4A4_UNORM_PACK16:
    case Graphics::Format::R5G6B5_UNORM_PACK16:
    case Graphics::Format::B5G6R5_UNORM_PACK16:
    case Graphics::Format::R5G5B5A1_UNORM_PACK16:
    case Graphics::Format::B5G5R5A1_UNORM_PACK16:
    case Graphics::Format::A1R5G5B5_UNORM_PACK16:
    case Graphics::Format::R8_UNORM:
    case Graphics::Format::R8_SNORM:
    case Graphics::Format::R8_USCALED:
    case Graphics::Format::R8_SSCALED:
    case Graphics::Format::R8_UINT:
    case Graphics::Format::R8_SINT:
    case Graphics::Format::R8_SRGB:
    case Graphics::Format::R8G8_UNORM:
    case Graphics::Format::R8G8_SNORM:
    case Graphics::Format::R8G8_USCALED:
    case Graphics::Format::R8G8_SSCALED:
    case Graphics::Format::R8G8_UINT:
    case Graphics::Format::R8G8_SINT:
    case Graphics::Format::R8G8_SRGB:
    case Graphics::Format::R8G8B8_UNORM:
    case Graphics::Format::R8G8B8_SNORM:
    case Graphics::Format::R8G8B8_USCALED:
    case Graphics::Format::R8G8B8_SSCALED:
    case Graphics::Format::R8G8B8_UINT:
    case Graphics::Format::R8G8B8_SINT:
    case Graphics::Format::R8G8B8_SRGB:
    case Graphics::Format::B8G8R8_UNORM:
    case Graphics::Format::B8G8R8_SNORM:
    case Graphics::Format::B8G8R8_USCALED:
    case Graphics::Format::B8G8R8_SSCALED:
    case Graphics::Format::B8G8R8_UINT:
    case Graphics::Format::B8G8R8_SINT:
    case Graphics::Format::B8G8R8_SRGB:
    case Graphics::Format::R8G8B8A8_UNORM:
    case Graphics::Format::R8G8B8A8_SNORM:
    case Graphics::Format::R8G8B8A8_USCALED:
    case Graphics::Format::R8G8B8A8_SSCALED:
    case Graphics::Format::R8G8B8A8_UINT:
    case Graphics::Format::R8G8B8A8_SINT:
    case Graphics::Format::R8G8B8A8_SRGB:
    case Graphics::Format::B8G8R8A8_UNORM:
    case Graphics::Format::B8G8R8A8_SNORM:
    case Graphics::Format::B8G8R8A8_USCALED:
    case Graphics::Format::B8G8R8A8_SSCALED:
    case Graphics::Format::B8G8R8A8_UINT:
    case Graphics::Format::B8G8R8A8_SINT:
    case Graphics::Format::B8G8R8A8_SRGB:
    case Graphics::Format::A8B8G8R8_UNORM_PACK32:
    case Graphics::Format::A8B8G8R8_SNORM_PACK32:
    case Graphics::Format::A8B8G8R8_USCALED_PACK32:
    case Graphics::Format::A8B8G8R8_SSCALED_PACK32:
    case Graphics::Format::A8B8G8R8_UINT_PACK32:
    case Graphics::Format::A8B8G8R8_SINT_PACK32:
    case Graphics::Format::A8B8G8R8_SRGB_PACK32:
    case Graphics::Format::A2R10G10B10_UNORM_PACK32:
    case Graphics::Format::A2R10G10B10_SNORM_PACK32:
    case Graphics::Format::A2R10G10B10_USCALED_PACK32:
    case Graphics::Format::A2R10G10B10_SSCALED_PACK32:
    case Graphics::Format::A2R10G10B10_UINT_PACK32:
    case Graphics::Format::A2R10G10B10_SINT_PACK32:
    case Graphics::Format::A2B10G10R10_UNORM_PACK32:
    case Graphics::Format::A2B10G10R10_SNORM_PACK32:
    case Graphics::Format::A2B10G10R10_USCALED_PACK32:
    case Graphics::Format::A2B10G10R10_SSCALED_PACK32:
    case Graphics::Format::A2B10G10R10_UINT_PACK32:
    case Graphics::Format::A2B10G10R10_SINT_PACK32:
    case Graphics::Format::R16_UNORM:
    case Graphics::Format::R16_SNORM:
    case Graphics::Format::R16_USCALED:
    case Graphics::Format::R16_SSCALED:
    case Graphics::Format::R16_UINT:
    case Graphics::Format::R16_SINT:
    case Graphics::Format::R16_SFLOAT:
    case Graphics::Format::R16G16_UNORM:
    case Graphics::Format::R16G16_SNORM:
    case Graphics::Format::R16G16_USCALED:
    case Graphics::Format::R16G16_SSCALED:
    case Graphics::Format::R16G16_UINT:
    case Graphics::Format::R16G16_SINT:
    case Graphics::Format::R16G16_SFLOAT:
    case Graphics::Format::R16G16B16_UNORM:
    case Graphics::Format::R16G16B16_SNORM:
    case Graphics::Format::R16G16B16_USCALED:
    case Graphics::Format::R16G16B16_SSCALED:
    case Graphics::Format::R16G16B16_UINT:
    case Graphics::Format::R16G16B16_SINT:
    case Graphics::Format::R16G16B16_SFLOAT:
    case Graphics::Format::R16G16B16A16_UNORM:
    case Graphics::Format::R16G16B16A16_SNORM:
    case Graphics::Format::R16G16B16A16_USCALED:
    case Graphics::Format::R16G16B16A16_SSCALED:
    case Graphics::Format::R16G16B16A16_UINT:
    case Graphics::Format::R16G16B16A16_SINT:
    case Graphics::Format::R16G16B16A16_SFLOAT:
    case Graphics::Format::R32_UINT:
    case Graphics::Format::R32_SINT:
    case Graphics::Format::R32_SFLOAT:
    case Graphics::Format::R32G32_UINT:
    case Graphics::Format::R32G32_SINT:
    case Graphics::Format::R32G32_SFLOAT:
    case Graphics::Format::R32G32B32_UINT:
    case Graphics::Format::R32G32B32_SINT:
    case Graphics::Format::R32G32B32_SFLOAT:
    case Graphics::Format::R32G32B32A32_UINT:
    case Graphics::Format::R32G32B32A32_SINT:
    case Graphics::Format::R32G32B32A32_SFLOAT:
    case Graphics::Format::R64_UINT:
    case Graphics::Format::R64_SINT:
    case Graphics::Format::R64_SFLOAT:
    case Graphics::Format::R64G64_UINT:
    case Graphics::Format::R64G64_SINT:
    case Graphics::Format::R64G64_SFLOAT:
    case Graphics::Format::R64G64B64_UINT:
    case Graphics::Format::R64G64B64_SINT:
    case Graphics::Format::R64G64B64_SFLOAT:
    case Graphics::Format::R64G64B64A64_UINT:
    case Graphics::Format::R64G64B64A64_SINT:
    case Graphics::Format::R64G64B64A64_SFLOAT:
    case Graphics::Format::B10G11R11_UFLOAT_PACK32:
    case Graphics::Format::R11G11B10_UFLOAT_PACK32:
    case Graphics::Format::E5B9G9R9_UFLOAT_PACK32:
    case Graphics::Format::D16_UNORM:
    case Graphics::Format::X8_D24_UNORM_PACK32:
    case Graphics::Format::D32_SFLOAT:
    case Graphics::Format::S8_UINT:
    case Graphics::Format::D16_UNORM_S8_UINT:
    case Graphics::Format::D24_UNORM_S8_UINT:
    case Graphics::Format::D32_SFLOAT_S8_UINT:
    case Graphics::Format::BC1_RGB_UNORM_BLOCK:
    case Graphics::Format::BC1_RGB_SRGB_BLOCK:
    case Graphics::Format::BC1_RGBA_UNORM_BLOCK:
    case Graphics::Format::BC1_RGBA_SRGB_BLOCK:
    case Graphics::Format::BC2_UNORM_BLOCK:
    case Graphics::Format::BC2_SRGB_BLOCK:
    case Graphics::Format::BC3_UNORM_BLOCK:
    case Graphics::Format::BC3_SRGB_BLOCK:
    case Graphics::Format::BC4_UNORM_BLOCK:
    case Graphics::Format::BC4_SNORM_BLOCK:
    case Graphics::Format::BC5_UNORM_BLOCK:
    case Graphics::Format::BC5_SNORM_BLOCK:
    case Graphics::Format::BC6H_UFLOAT_BLOCK:
    case Graphics::Format::BC6H_SFLOAT_BLOCK:
    case Graphics::Format::BC7_UNORM_BLOCK:
    case Graphics::Format::BC7_SRGB_BLOCK: {
      return false;
    }

    case Graphics::Format::ETC2_R8G8B8_UNORM_BLOCK:
    case Graphics::Format::ETC2_R8G8B8_SRGB_BLOCK:
    case Graphics::Format::ETC2_R8G8B8A1_UNORM_BLOCK:
    case Graphics::Format::ETC2_R8G8B8A1_SRGB_BLOCK:
    case Graphics::Format::ETC2_R8G8B8A8_UNORM_BLOCK:
    case Graphics::Format::ETC2_R8G8B8A8_SRGB_BLOCK:
    case Graphics::Format::EAC_R11_UNORM_BLOCK:
    case Graphics::Format::EAC_R11_SNORM_BLOCK:
    case Graphics::Format::EAC_R11G11_UNORM_BLOCK:
    case Graphics::Format::EAC_R11G11_SNORM_BLOCK:
    case Graphics::Format::ASTC_4x4_UNORM_BLOCK:
    case Graphics::Format::ASTC_4x4_SRGB_BLOCK:
    case Graphics::Format::ASTC_5x4_UNORM_BLOCK:
    case Graphics::Format::ASTC_5x4_SRGB_BLOCK:
    case Graphics::Format::ASTC_5x5_UNORM_BLOCK:
    case Graphics::Format::ASTC_5x5_SRGB_BLOCK:
    case Graphics::Format::ASTC_6x5_UNORM_BLOCK:
    case Graphics::Format::ASTC_6x5_SRGB_BLOCK:
    case Graphics::Format::ASTC_6x6_UNORM_BLOCK:
    case Graphics::Format::ASTC_6x6_SRGB_BLOCK:
    case Graphics::Format::ASTC_8x5_UNORM_BLOCK:
    case Graphics::Format::ASTC_8x5_SRGB_BLOCK:
    case Graphics::Format::ASTC_8x6_UNORM_BLOCK:
    case Graphics::Format::ASTC_8x6_SRGB_BLOCK:
    case Graphics::Format::ASTC_8x8_UNORM_BLOCK:
    case Graphics::Format::ASTC_8x8_SRGB_BLOCK:
    case Graphics::Format::ASTC_10x5_UNORM_BLOCK:
    case Graphics::Format::ASTC_10x5_SRGB_BLOCK:
    case Graphics::Format::ASTC_10x6_UNORM_BLOCK:
    case Graphics::Format::ASTC_10x6_SRGB_BLOCK:
    case Graphics::Format::ASTC_10x8_UNORM_BLOCK:
    case Graphics::Format::ASTC_10x8_SRGB_BLOCK:
    case Graphics::Format::ASTC_10x10_UNORM_BLOCK:
    case Graphics::Format::ASTC_10x10_SRGB_BLOCK:
    case Graphics::Format::ASTC_12x10_UNORM_BLOCK:
    case Graphics::Format::ASTC_12x10_SRGB_BLOCK:
    case Graphics::Format::ASTC_12x12_UNORM_BLOCK:
    case Graphics::Format::ASTC_12x12_SRGB_BLOCK:
    case Graphics::Format::PVRTC1_2BPP_UNORM_BLOCK_IMG:
    case Graphics::Format::PVRTC1_4BPP_UNORM_BLOCK_IMG:
    case Graphics::Format::PVRTC2_2BPP_UNORM_BLOCK_IMG:
    case Graphics::Format::PVRTC2_4BPP_UNORM_BLOCK_IMG:
    case Graphics::Format::PVRTC1_2BPP_SRGB_BLOCK_IMG:
    case Graphics::Format::PVRTC1_4BPP_SRGB_BLOCK_IMG:
    case Graphics::Format::PVRTC2_2BPP_SRGB_BLOCK_IMG:
    case Graphics::Format::PVRTC2_4BPP_SRGB_BLOCK_IMG: {
      return true;
    }
  }

  return false;
}

TestGraphicsTexture::TestGraphicsTexture( const Graphics::TextureCreateInfo& createInfo )
: mCreateInfo( createInfo ), mIsCompressed( IsCompressedFormat( createInfo.format ) )
{
  uint32_t target = GetTarget();
  if( mCreateInfo.nativeImagePtr )
  {
    InitializeNativeImage( target );
  }
  else
  {
    Initialize( target );

    if( mCreateInfo.textureType == Graphics::TextureType::TEXTURE_2D )
    {
      if( !mIsCompressed )
      {
        //glAbstraction.TexImage2D(target, 0, mGlInternalFormat, createInfo.size.width, createInfo.size.height, 0, mGlFormat, mPixelDataType, nullptr);
      }
      else
      {
        //glAbstraction.CompressedTexImage2D(target, 0, mGlInternalFormat, createInfo.size.width, createInfo.size.height, 0, 0, nullptr);
      }
    }
    else if( mCreateInfo.textureType == Graphics::TextureType::TEXTURE_CUBEMAP )
    {
      if( !mIsCompressed )
      {
        for( uint32_t i( 0 ); i < 6; ++i )
        {
          //glAbstraction.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mGlInternalFormat, createInfo.size.width, createInfo.size.height, 0, mGlFormat, mPixelDataType, nullptr);
        }
      }
      else
      {
        for( uint32_t i( 0 ); i < 6; ++i )
        {
          //glAbstraction.CompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mGlInternalFormat, createInfo.size.width, createInfo.size.height, 0, 0, nullptr);
        }
      }
      //TestGraphicsSampler::SetTexParameter(glAbstraction, target, GL_TEXTURE_WRAP_R, GL_WRAP_DEFAULT);
    }
  }
}

TestGraphicsTexture::~TestGraphicsTexture()
{
}

void TestGraphicsTexture::Initialize( uint32_t target )
{
}

void TestGraphicsTexture::InitializeNativeImage( uint32_t target )
{
  Initialize( target );

  if( mCreateInfo.nativeImagePtr->TargetTexture() != 0u ) // This can definitely fail
  {
    mId = 0u;
  }
}

uint32_t TestGraphicsTexture::GetTarget()
{
  uint32_t target = 0;
  return target;
}

void TestGraphicsTexture::Bind( uint32_t textureUnit )
{
}

void TestGraphicsTexture::Prepare()
{
}

void TestGraphicsTexture::Update( Graphics::TextureUpdateInfo updateInfo, Graphics::TextureUpdateSourceInfo source )
{
}

}