/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

namespace
{
// These match the GL specification
const GLint GL_MINIFY_DEFAULT  = GL_NEAREST_MIPMAP_LINEAR;
const GLint GL_MAGNIFY_DEFAULT = GL_LINEAR;
const GLint GL_WRAP_DEFAULT    = GL_CLAMP_TO_EDGE;

// These are the Dali defaults
const GLint DALI_MINIFY_DEFAULT  = GL_LINEAR;
const GLint DALI_MAGNIFY_DEFAULT = GL_LINEAR;

GLuint GetTextureTarget(Graphics::TextureType type)
{
  GLuint target{GL_TEXTURE_2D};

  switch(type)
  {
    case Graphics::TextureType::TEXTURE_2D:
      target = GL_TEXTURE_2D; // Native texture may override this with GL_TEXTURE_EXTERNAL_OES
      break;
    case Graphics::TextureType::TEXTURE_3D:
      target = GL_TEXTURE_3D;
      break;
    case Graphics::TextureType::TEXTURE_CUBEMAP:
      target = GL_TEXTURE_CUBE_MAP;
      break;
  }
  return target;
}

/**
 * @brief Whether specified pixel format is compressed.
 *
 * @param [in] pixelformat Pixel format
 * @return true if format is compressed, false otherwise
 */
bool IsCompressedFormat(Graphics::Format pixelFormat)
{
  switch(pixelFormat)
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
    case Graphics::Format::BC7_SRGB_BLOCK:
    {
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
    case Graphics::Format::PVRTC2_4BPP_SRGB_BLOCK_IMG:
    {
      return true;
    }
  }

  return false;
}

/**
 * @brief Retrives the GL format, GL internal format and pixel data type from a Graphics::Format
 * @param[in] pixelFormat The pixel format.
 * @param[out] glFormat The gl format.
 * @param[out] glInternalFormat The gl internal format.
 * @param[out] pixelDataType The data type of the pixel data.
 */
void PixelFormatToGl(Graphics::Format pixelFormat, GLenum& glFormat, GLint& glInternalFormat, GLenum& pixelDataType)
{
  // Compressed textures have no pixelDataType, so init to an invalid value:
  pixelDataType = -1;

  switch(pixelFormat)
  {
    case Graphics::Format::R8_UNORM:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
      glFormat      = GL_ALPHA;
      break;
    }

    case Graphics::Format::L8:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
      glFormat      = GL_LUMINANCE;
      break;
    }

    case Graphics::Format::L8A8:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
      glFormat      = GL_LUMINANCE_ALPHA;
      break;
    }

    case Graphics::Format::R5G6B5_UNORM_PACK16:
    {
      pixelDataType = GL_UNSIGNED_SHORT_5_6_5;
      glFormat      = GL_RGB;
      break;
    }

    case Graphics::Format::B5G6R5_UNORM_PACK16:
    {
      pixelDataType = GL_UNSIGNED_SHORT_5_6_5;
#ifdef _ARCH_ARM_
      glFormat = GL_BGRA_EXT; // alpha is reserved but not used
#else
      glFormat = GL_RGBA; // alpha is reserved but not used
#endif
      break;
    }

    case Graphics::Format::R4G4B4A4_UNORM_PACK16:
    {
      pixelDataType = GL_UNSIGNED_SHORT_4_4_4_4;
      glFormat      = GL_RGBA;
      break;
    }

    case Graphics::Format::B4G4R4A4_UNORM_PACK16:
    {
      pixelDataType = GL_UNSIGNED_SHORT_4_4_4_4;
#ifdef _ARCH_ARM_
      glFormat = GL_BGRA_EXT; // alpha is reserved but not used
#else
      glFormat = GL_RGBA; // alpha is reserved but not used
#endif
      break;
    }

    case Graphics::Format::R5G5B5A1_UNORM_PACK16:
    {
      pixelDataType = GL_UNSIGNED_SHORT_5_5_5_1;
      glFormat      = GL_RGBA;
      break;
    }

    case Graphics::Format::B5G5R5A1_UNORM_PACK16:
    {
      pixelDataType = GL_UNSIGNED_SHORT_5_5_5_1;
#ifdef _ARCH_ARM_
      glFormat = GL_BGRA_EXT; // alpha is reserved but not used
#else
      glFormat = GL_RGBA; // alpha is reserved but not used
#endif
      break;
    }

    case Graphics::Format::R8G8B8_UNORM:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
      glFormat      = GL_RGB;
      break;
    }

    case Graphics::Format::R8G8B8A8_UNORM:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
      glFormat      = GL_RGBA; // alpha is reserved but not used
      break;
    }

    case Graphics::Format::B8G8R8A8_UNORM:
    {
      pixelDataType = GL_UNSIGNED_BYTE;
#ifdef GL_BGRA_EXT
      glFormat = GL_BGRA_EXT; // alpha is reserved but not used
#else
      glFormat = GL_RGBA; // alpha is reserved but not used
#endif
      break;
    }

    case Graphics::Format::ETC2_R8G8B8_UNORM_BLOCK:
    {
      glFormat = GL_COMPRESSED_RGB8_ETC2;
      break;
    }

    case Graphics::Format::PVRTC1_4BPP_UNORM_BLOCK_IMG:
    {
      glFormat = 0x8C00; ///! < Hardcoded so we can test before we move to GLES 3.0 or greater.
      break;
    }

    // GLES 3.0 standard compressed formats:
    case Graphics::Format::EAC_R11_UNORM_BLOCK:
    {
      glFormat = GL_COMPRESSED_R11_EAC;
      break;
    }
    case Graphics::Format::EAC_R11_SNORM_BLOCK:
    {
      glFormat = GL_COMPRESSED_SIGNED_R11_EAC;
      break;
    }
    case Graphics::Format::EAC_R11G11_UNORM_BLOCK:
    {
      glFormat = GL_COMPRESSED_RG11_EAC;
      break;
    }
    case Graphics::Format::EAC_R11G11_SNORM_BLOCK:
    {
      glFormat = GL_COMPRESSED_SIGNED_RG11_EAC;
      break;
    }
    case Graphics::Format::ETC2_R8G8B8_SRGB_BLOCK:
    {
      glFormat = GL_COMPRESSED_SRGB8_ETC2;
      break;
    }
    case Graphics::Format::ETC2_R8G8B8A1_UNORM_BLOCK:
    {
      glFormat = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
      break;
    }
    case Graphics::Format::ETC2_R8G8B8A1_SRGB_BLOCK:
    {
      glFormat = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;
      break;
    }
    case Graphics::Format::ETC2_R8G8B8A8_SRGB_BLOCK:
    {
      glFormat = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
      break;
    }

    // GLES 3.1 extension compressed formats:
    case Graphics::Format::ASTC_4x4_UNORM_BLOCK:
    {
      glFormat = GL_COMPRESSED_RGBA_ASTC_4x4_KHR;
      break;
    }
    case Graphics::Format::ASTC_5x4_UNORM_BLOCK:
    {
      glFormat = GL_COMPRESSED_RGBA_ASTC_5x4_KHR;
      break;
    }
    case Graphics::Format::ASTC_5x5_UNORM_BLOCK:
    {
      glFormat = GL_COMPRESSED_RGBA_ASTC_5x5_KHR;
      break;
    }
    case Graphics::Format::ASTC_6x5_UNORM_BLOCK:
    {
      glFormat = GL_COMPRESSED_RGBA_ASTC_6x5_KHR;
      break;
    }
    case Graphics::Format::ASTC_6x6_UNORM_BLOCK:
    {
      glFormat = GL_COMPRESSED_RGBA_ASTC_6x6_KHR;
      break;
    }
    case Graphics::Format::ASTC_8x5_UNORM_BLOCK:
    {
      glFormat = GL_COMPRESSED_RGBA_ASTC_8x5_KHR;
      break;
    }
    case Graphics::Format::ASTC_8x6_UNORM_BLOCK:
    {
      glFormat = GL_COMPRESSED_RGBA_ASTC_8x6_KHR;
      break;
    }
    case Graphics::Format::ASTC_8x8_UNORM_BLOCK:
    {
      glFormat = GL_COMPRESSED_RGBA_ASTC_8x8_KHR;
      break;
    }
    case Graphics::Format::ASTC_10x5_UNORM_BLOCK:
    {
      glFormat = GL_COMPRESSED_RGBA_ASTC_10x5_KHR;
      break;
    }
    case Graphics::Format::ASTC_10x6_UNORM_BLOCK:
    {
      glFormat = GL_COMPRESSED_RGBA_ASTC_10x6_KHR;
      break;
    }
    case Graphics::Format::ASTC_10x8_UNORM_BLOCK:
    {
      glFormat = GL_COMPRESSED_RGBA_ASTC_10x8_KHR;
      break;
    }
    case Graphics::Format::ASTC_10x10_UNORM_BLOCK:
    {
      glFormat = GL_COMPRESSED_RGBA_ASTC_10x10_KHR;
      break;
    }
    case Graphics::Format::ASTC_12x10_UNORM_BLOCK:
    {
      glFormat = GL_COMPRESSED_RGBA_ASTC_12x10_KHR;
      break;
    }
    case Graphics::Format::ASTC_12x12_UNORM_BLOCK:
    {
      glFormat = GL_COMPRESSED_RGBA_ASTC_12x12_KHR;
      break;
    }

    case Graphics::Format::ASTC_4x4_SRGB_BLOCK:
    {
      glFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR;
      break;
    }
    case Graphics::Format::ASTC_5x4_SRGB_BLOCK:
    {
      glFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR;
      break;
    }
    case Graphics::Format::ASTC_5x5_SRGB_BLOCK:
    {
      glFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR;
      break;
    }
    case Graphics::Format::ASTC_6x5_SRGB_BLOCK:
    {
      glFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR;
      break;
    }
    case Graphics::Format::ASTC_6x6_SRGB_BLOCK:
    {
      glFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR;
      break;
    }
    case Graphics::Format::ASTC_8x5_SRGB_BLOCK:
    {
      glFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR;
      break;
    }
    case Graphics::Format::ASTC_8x6_SRGB_BLOCK:
    {
      glFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR;
      break;
    }
    case Graphics::Format::ASTC_8x8_SRGB_BLOCK:
    {
      glFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR;
      break;
    }
    case Graphics::Format::ASTC_10x5_SRGB_BLOCK:
    {
      glFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR;
      break;
    }
    case Graphics::Format::ASTC_10x6_SRGB_BLOCK:
    {
      glFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR;
      break;
    }
    case Graphics::Format::ASTC_10x8_SRGB_BLOCK:
    {
      glFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR;
      break;
    }
    case Graphics::Format::ASTC_10x10_SRGB_BLOCK:
    {
      glFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR;
      break;
    }
    case Graphics::Format::ASTC_12x10_SRGB_BLOCK:
    {
      glFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR;
      break;
    }
    case Graphics::Format::ASTC_12x12_SRGB_BLOCK:
    {
      glFormat = GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR;
      break;
    }

    // GLES 3.0 floating point formats.
    case Graphics::Format::R16G16B16_SFLOAT:
    {
      glFormat      = GL_RGB;
      pixelDataType = GL_HALF_FLOAT;
      break;
    }
    case Graphics::Format::R32G32B32_SFLOAT:
    {
      glFormat      = GL_RGB;
      pixelDataType = GL_FLOAT;
      break;
    }

    // GLES 3.0 depth and stencil formats
    case Graphics::Format::D16_UNORM:
    {
      glFormat      = GL_DEPTH_COMPONENT;
      pixelDataType = GL_UNSIGNED_INT;
      break;
    }

    case Graphics::Format::D32_SFLOAT:
    {
      glFormat      = GL_DEPTH_COMPONENT;
      pixelDataType = GL_FLOAT;
      break;
    }

    case Graphics::Format::D24_UNORM_S8_UINT:
    {
      glFormat      = GL_DEPTH_STENCIL;
      pixelDataType = GL_UNSIGNED_INT_24_8;
      break;
    }

    case Graphics::Format::UNDEFINED:
    {
      //DALI_LOG_ERROR( "Invalid pixel format for bitmap\n" );
      glFormat = 0;
      break;
    }
    case Graphics::Format::R11G11B10_UFLOAT_PACK32:
    {
      glFormat      = GL_RGB;
      pixelDataType = GL_FLOAT;
      break;
    }

    case Graphics::Format::R4G4_UNORM_PACK8:
    case Graphics::Format::A1R5G5B5_UNORM_PACK16:
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
    case Graphics::Format::R8G8B8A8_SNORM:
    case Graphics::Format::R8G8B8A8_USCALED:
    case Graphics::Format::R8G8B8A8_SSCALED:
    case Graphics::Format::R8G8B8A8_UINT:
    case Graphics::Format::R8G8B8A8_SINT:
    case Graphics::Format::R8G8B8A8_SRGB:
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
    case Graphics::Format::E5B9G9R9_UFLOAT_PACK32:
    case Graphics::Format::X8_D24_UNORM_PACK32:
    case Graphics::Format::S8_UINT:
    case Graphics::Format::D16_UNORM_S8_UINT:
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
    case Graphics::Format::BC7_SRGB_BLOCK:
    case Graphics::Format::ETC2_R8G8B8A8_UNORM_BLOCK:
    case Graphics::Format::PVRTC1_2BPP_UNORM_BLOCK_IMG:
    case Graphics::Format::PVRTC2_2BPP_UNORM_BLOCK_IMG:
    case Graphics::Format::PVRTC2_4BPP_UNORM_BLOCK_IMG:
    case Graphics::Format::PVRTC1_2BPP_SRGB_BLOCK_IMG:
    case Graphics::Format::PVRTC1_4BPP_SRGB_BLOCK_IMG:
    case Graphics::Format::PVRTC2_2BPP_SRGB_BLOCK_IMG:
    case Graphics::Format::PVRTC2_4BPP_SRGB_BLOCK_IMG:
    {
      glFormat = 0;
      break;
    }
  }

  switch(pixelFormat)
  {
    case Graphics::Format::R16G16B16A16_SFLOAT:
    case Graphics::Format::R32G32B32A32_SFLOAT:
    case Graphics::Format::R11G11B10_UFLOAT_PACK32:
    {
      glInternalFormat = GL_R11F_G11F_B10F;
      break;
    }
    case Graphics::Format::D32_SFLOAT:
    {
      glInternalFormat = GL_DEPTH_COMPONENT32F;
      break;
    }
    case Graphics::Format::D24_UNORM_S8_UINT:
    {
      glInternalFormat = GL_DEPTH24_STENCIL8;
      break;
    }
    default:
    {
      glInternalFormat = glFormat;
    }
  }
}

} // namespace

TestGraphicsTexture::TestGraphicsTexture(TestGlAbstraction& glAbstraction, const Graphics::TextureCreateInfo& createInfo)
: mGlAbstraction(glAbstraction),
  mCreateInfo(createInfo),
  mIsCompressed(IsCompressedFormat(createInfo.format))
{
  GLuint target = GetTarget();
  if(mCreateInfo.nativeImagePtr)
  {
    InitializeNativeImage(target);
  }
  else
  {
    Initialize(target);

    if(mCreateInfo.textureType == Graphics::TextureType::TEXTURE_2D)
    {
      if(!mIsCompressed)
      {
        glAbstraction.TexImage2D(target, 0, mGlInternalFormat, createInfo.size.width, createInfo.size.height, 0, mGlFormat, mPixelDataType, nullptr);
      }
      else
      {
        glAbstraction.CompressedTexImage2D(target, 0, mGlInternalFormat, createInfo.size.width, createInfo.size.height, 0, 0, nullptr);
      }
    }
    else if(mCreateInfo.textureType == Graphics::TextureType::TEXTURE_CUBEMAP)
    {
      if(!mIsCompressed)
      {
        for(uint32_t i(0); i < 6; ++i)
        {
          glAbstraction.TexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mGlInternalFormat, createInfo.size.width, createInfo.size.height, 0, mGlFormat, mPixelDataType, nullptr);
        }
      }
      else
      {
        for(uint32_t i(0); i < 6; ++i)
        {
          glAbstraction.CompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mGlInternalFormat, createInfo.size.width, createInfo.size.height, 0, 0, nullptr);
        }
      }
      TestGraphicsSampler::SetTexParameter(glAbstraction, target, GL_TEXTURE_WRAP_R, GL_WRAP_DEFAULT);
    }
  }
}

TestGraphicsTexture::~TestGraphicsTexture()
{
  mGlAbstraction.DeleteTextures(1, &mId);
  if(mCreateInfo.nativeImagePtr)
  {
    mCreateInfo.nativeImagePtr->DestroyResource();
  }
}

void TestGraphicsTexture::Initialize(GLuint target)
{
  PixelFormatToGl(mCreateInfo.format,
                  mGlFormat,
                  mGlInternalFormat,
                  mPixelDataType);

  mGlAbstraction.GenTextures(1, &mId);
  mGlAbstraction.BindTexture(target, mId);
  mGlAbstraction.PixelStorei(GL_UNPACK_ALIGNMENT, 1); // We always use tightly packed data

  //Apply default sampling parameters
  TestGraphicsSampler::SetTexParameter(mGlAbstraction, target, GL_TEXTURE_MIN_FILTER, DALI_MINIFY_DEFAULT);
  TestGraphicsSampler::SetTexParameter(mGlAbstraction, target, GL_TEXTURE_MAG_FILTER, DALI_MAGNIFY_DEFAULT);
  TestGraphicsSampler::SetTexParameter(mGlAbstraction, target, GL_TEXTURE_WRAP_S, GL_WRAP_DEFAULT);
  TestGraphicsSampler::SetTexParameter(mGlAbstraction, target, GL_TEXTURE_WRAP_T, GL_WRAP_DEFAULT);
}

void TestGraphicsTexture::InitializeNativeImage(GLuint target)
{
  mCreateInfo.nativeImagePtr->CreateResource();
  Initialize(target);

  if(mCreateInfo.nativeImagePtr->TargetTexture() != 0u) // This can definitely fail
  {
    mGlAbstraction.DeleteTextures(1, &mId);
    mCreateInfo.nativeImagePtr->DestroyResource();
    mId = 0u;
  }
}

GLuint TestGraphicsTexture::GetTarget()
{
  GLuint target;
  if(mCreateInfo.nativeImagePtr)
  {
    target = mCreateInfo.nativeImagePtr->GetTextureTarget(); // Could be GL_TEXTURE_2D or GL_TEXTURE_EXTERNAL_OES
  }
  else
  {
    target = GetTextureTarget(mCreateInfo.textureType);
  }
  return target;
}

void TestGraphicsTexture::Bind(uint32_t textureUnit)
{
  if(mCreateInfo.nativeImagePtr)
  {
    if(mId == 0)
    {
      InitializeNativeImage(GetTarget());
    }
  }
  mGlAbstraction.ActiveTexture(textureUnit + GL_TEXTURE0);
  mGlAbstraction.BindTexture(GetTarget(), mId);
}

void TestGraphicsTexture::Prepare()
{
  if(mCreateInfo.nativeImagePtr)
  {
    /***********************************************************************************
     * If the native image source changes, we need to re-create the texture.           *
     * In EGL, this is done in native image implementation in PrepareTexture below.    *
     *                                                                                 *
     * In Vulkan impl, this was done in dali-core side. I think we should make this    *
     * work in the graphics implementation instead.                                    *
     ***********************************************************************************/
    if(mCreateInfo.nativeImagePtr->SourceChanged())
    {
      uint32_t width  = mCreateInfo.nativeImagePtr->GetWidth();
      uint32_t height = mCreateInfo.nativeImagePtr->GetHeight();
      mCreateInfo.SetSize({width, height}); // Size may change

      // @todo Re-initialize this texture from the new create info.
    }

    // Ensure the native image is up-to-date
    mCreateInfo.nativeImagePtr->PrepareTexture();
  }
}

void TestGraphicsTexture::Update(Graphics::TextureUpdateInfo updateInfo, Graphics::TextureUpdateSourceInfo source)
{
  GLenum target{GetTarget()};
  if(mCreateInfo.textureType == Graphics::TextureType::TEXTURE_CUBEMAP)
  {
    target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + updateInfo.layer;
  }

  mGlAbstraction.PixelStorei(GL_UNPACK_ALIGNMENT, 1); // We always use tightly packed data

  const bool isSubImage(updateInfo.dstOffset2D.x != 0 || updateInfo.dstOffset2D.y != 0 ||
                        updateInfo.srcExtent2D.width != (mCreateInfo.size.width / (1 << updateInfo.level)) ||
                        updateInfo.srcExtent2D.height != (mCreateInfo.size.height / (1 << updateInfo.level)));

  if(!isSubImage)
  {
    if(!mIsCompressed)
    {
      mGlAbstraction.TexImage2D(target, updateInfo.level, mGlInternalFormat, updateInfo.srcExtent2D.width, updateInfo.srcExtent2D.height, 0, mGlFormat, mPixelDataType, source.memorySource.memory);
    }
    else
    {
      mGlAbstraction.CompressedTexImage2D(target, updateInfo.level, mGlInternalFormat, updateInfo.srcExtent2D.width, updateInfo.srcExtent2D.height, 0, updateInfo.srcSize, source.memorySource.memory);
    }
  }
  else
  {
    if(!mIsCompressed)
    {
      mGlAbstraction.TexSubImage2D(target, updateInfo.level, updateInfo.dstOffset2D.x, updateInfo.dstOffset2D.y, updateInfo.srcExtent2D.width, updateInfo.srcExtent2D.height, mGlFormat, mPixelDataType, source.memorySource.memory);
    }
    else
    {
      mGlAbstraction.CompressedTexSubImage2D(target, updateInfo.level, updateInfo.dstOffset2D.x, updateInfo.dstOffset2D.y, updateInfo.srcExtent2D.width, updateInfo.srcExtent2D.height, mGlFormat, updateInfo.srcSize, source.memorySource.memory);
    }
  }
}
