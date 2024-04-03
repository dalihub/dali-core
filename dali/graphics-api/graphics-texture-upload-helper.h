#ifndef DALI_GRAPHICS_TEXTURE_UPLOAD_INTERFACE_H
#define DALI_GRAPHICS_TEXTURE_UPLOAD_INTERFACE_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/rendering/texture.h>

// INTERNAL INCLUDES
#include "graphics-types.h"

namespace Dali
{
namespace Graphics
{
/**
 * @brief Structure used to pass parameters to the Upload method
 */
struct UploadParams
{
  uint32_t dataXOffset; ///< Specifies a pixeldata offset in the x direction within the pixeldata buffer.
  uint32_t dataYOffset; ///< Specifies a pixeldata offset in the y direction within the pixeldata buffer.
  uint16_t dataWidth;   ///< Specifies the width of the pixeldata subimage.
  uint16_t dataHeight;  ///< Specifies the height of the pixeldata subimage.
  uint16_t layer;       ///< Specifies the layer of a cube map or array texture
  uint16_t mipmap;      ///< Specifies the level-of-detail number. Level 0 is the base image level. Level n is the nth mipmap reduction image.
  uint16_t xOffset;     ///< Specifies a texel offset in the x direction within the texture array.
  uint16_t yOffset;     ///< Specifies a texel offset in the y direction within the texture array.
  uint16_t width;       ///< Specifies the width of the texture subimage
  uint16_t height;      ///< Specifies the height of the texture subimage.
};

/**
 * @brief Converts DALi pixel format to Graphics::Format.
 * @param[in] format Dali::Pixel::Format.
 * @return Converted Graphics::Format.
 */
constexpr Graphics::Format ConvertPixelFormat(Dali::Pixel::Format format)
{
  switch(format)
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

    case Pixel::DEPTH_UNSIGNED_INT:
      return Graphics::Format::D16_UNORM;
    case Pixel::DEPTH_FLOAT:
      return Graphics::Format::D32_SFLOAT;
    case Pixel::DEPTH_STENCIL:
      return Graphics::Format::D24_UNORM_S8_UINT;

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
      return Graphics::Format::ETC2_R8G8B8A8_UNORM_BLOCK;

    case Pixel::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
      return Graphics::Format::ETC2_R8G8B8A8_SRGB_BLOCK;

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
    case Pixel::R11G11B10F:
      return Graphics::Format::R11G11B10_UFLOAT_PACK32;

    case Pixel::CHROMINANCE_U:
      return Graphics::Format::L8;
    case Pixel::CHROMINANCE_V:
      return Graphics::Format::L8;
  }
  return Graphics::Format::UNDEFINED;
}

/**
 * @brief Converts DALi texture type to Graphics::TextureType.
 *
 * @param[in] type Dali::Texture::Type.
 * @return converted Graphics::TextureType.
 */
constexpr Graphics::TextureType ConvertTextureType(Dali::TextureType::Type type)
{
  switch(type)
  {
    case Dali::TextureType::TEXTURE_2D:
      return Graphics::TextureType::TEXTURE_2D;
    case Dali::TextureType::TEXTURE_CUBE:
      return Graphics::TextureType::TEXTURE_CUBEMAP;
  }
  return Graphics::TextureType::TEXTURE_2D;
}

} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_TEXTURE_UPLOAD_INTERFACE_H
