#ifndef DALI_GRAPHICS_VULKAN_UTILS
#define DALI_GRAPHICS_VULKAN_UTILS

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

#include <dali/graphics/vulkan/internal/vulkan-types.h>

namespace Dali
{

namespace Graphics
{

namespace Vulkan
{

// Returns Vulkan vk::Format details
static inline constexpr FormatInfo GetFormatInfo(const vk::Format format)
{
  auto formatSize = FormatInfo{};
  auto vkFormat = static_cast<VkFormat>(format);
  switch ( vkFormat )
  {
    case VK_FORMAT_R4G4_UNORM_PACK8:
      formatSize.packed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 1 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
    case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
    case VK_FORMAT_R5G6B5_UNORM_PACK16:
    case VK_FORMAT_B5G6R5_UNORM_PACK16:
    case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
    case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
    case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
      formatSize.packed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 2 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_R8_UNORM:
    case VK_FORMAT_R8_SNORM:
    case VK_FORMAT_R8_USCALED:
    case VK_FORMAT_R8_SSCALED:
    case VK_FORMAT_R8_UINT:
    case VK_FORMAT_R8_SINT:
    case VK_FORMAT_R8_SRGB:
      formatSize.packed = false;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 1 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_R8G8_UNORM:
    case VK_FORMAT_R8G8_SNORM:
    case VK_FORMAT_R8G8_USCALED:
    case VK_FORMAT_R8G8_SSCALED:
    case VK_FORMAT_R8G8_UINT:
    case VK_FORMAT_R8G8_SINT:
    case VK_FORMAT_R8G8_SRGB:
      formatSize.packed = false;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 2 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_R8G8B8_UNORM:
    case VK_FORMAT_R8G8B8_SNORM:
    case VK_FORMAT_R8G8B8_USCALED:
    case VK_FORMAT_R8G8B8_SSCALED:
    case VK_FORMAT_R8G8B8_UINT:
    case VK_FORMAT_R8G8B8_SINT:
    case VK_FORMAT_R8G8B8_SRGB:
    case VK_FORMAT_B8G8R8_UNORM:
    case VK_FORMAT_B8G8R8_SNORM:
    case VK_FORMAT_B8G8R8_USCALED:
    case VK_FORMAT_B8G8R8_SSCALED:
    case VK_FORMAT_B8G8R8_UINT:
    case VK_FORMAT_B8G8R8_SINT:
    case VK_FORMAT_B8G8R8_SRGB:
      formatSize.packed = false;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 3 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_R8G8B8A8_UNORM:
    case VK_FORMAT_R8G8B8A8_SNORM:
    case VK_FORMAT_R8G8B8A8_USCALED:
    case VK_FORMAT_R8G8B8A8_SSCALED:
    case VK_FORMAT_R8G8B8A8_UINT:
    case VK_FORMAT_R8G8B8A8_SINT:
    case VK_FORMAT_R8G8B8A8_SRGB:
    case VK_FORMAT_B8G8R8A8_UNORM:
    case VK_FORMAT_B8G8R8A8_SNORM:
    case VK_FORMAT_B8G8R8A8_USCALED:
    case VK_FORMAT_B8G8R8A8_SSCALED:
    case VK_FORMAT_B8G8R8A8_UINT:
    case VK_FORMAT_B8G8R8A8_SINT:
    case VK_FORMAT_B8G8R8A8_SRGB:
      formatSize.packed = false;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 4 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
    case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
    case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
    case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
    case VK_FORMAT_A8B8G8R8_UINT_PACK32:
    case VK_FORMAT_A8B8G8R8_SINT_PACK32:
    case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
      formatSize.packed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 4 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
    case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
    case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
    case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
    case VK_FORMAT_A2R10G10B10_UINT_PACK32:
    case VK_FORMAT_A2R10G10B10_SINT_PACK32:
    case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
    case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
    case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
    case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
    case VK_FORMAT_A2B10G10R10_UINT_PACK32:
    case VK_FORMAT_A2B10G10R10_SINT_PACK32:
      formatSize.packed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 4 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_R16_UNORM:
    case VK_FORMAT_R16_SNORM:
    case VK_FORMAT_R16_USCALED:
    case VK_FORMAT_R16_SSCALED:
    case VK_FORMAT_R16_UINT:
    case VK_FORMAT_R16_SINT:
    case VK_FORMAT_R16_SFLOAT:
      formatSize.packed = false;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 2 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_R16G16_UNORM:
    case VK_FORMAT_R16G16_SNORM:
    case VK_FORMAT_R16G16_USCALED:
    case VK_FORMAT_R16G16_SSCALED:
    case VK_FORMAT_R16G16_UINT:
    case VK_FORMAT_R16G16_SINT:
    case VK_FORMAT_R16G16_SFLOAT:
      formatSize.packed = false;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 4 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_R16G16B16_UNORM:
    case VK_FORMAT_R16G16B16_SNORM:
    case VK_FORMAT_R16G16B16_USCALED:
    case VK_FORMAT_R16G16B16_SSCALED:
    case VK_FORMAT_R16G16B16_UINT:
    case VK_FORMAT_R16G16B16_SINT:
    case VK_FORMAT_R16G16B16_SFLOAT:
      formatSize.packed = false;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 6 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_R16G16B16A16_UNORM:
    case VK_FORMAT_R16G16B16A16_SNORM:
    case VK_FORMAT_R16G16B16A16_USCALED:
    case VK_FORMAT_R16G16B16A16_SSCALED:
    case VK_FORMAT_R16G16B16A16_UINT:
    case VK_FORMAT_R16G16B16A16_SINT:
    case VK_FORMAT_R16G16B16A16_SFLOAT:
      formatSize.packed = false;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 8 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_R32_UINT:
    case VK_FORMAT_R32_SINT:
    case VK_FORMAT_R32_SFLOAT:
      formatSize.packed = false;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 4 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_R32G32_UINT:
    case VK_FORMAT_R32G32_SINT:
    case VK_FORMAT_R32G32_SFLOAT:
      formatSize.packed = false;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 8 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_R32G32B32_UINT:
    case VK_FORMAT_R32G32B32_SINT:
    case VK_FORMAT_R32G32B32_SFLOAT:
      formatSize.packed = false;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 12 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_R32G32B32A32_UINT:
    case VK_FORMAT_R32G32B32A32_SINT:
    case VK_FORMAT_R32G32B32A32_SFLOAT:
      formatSize.packed = false;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 16 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_R64_UINT:
    case VK_FORMAT_R64_SINT:
    case VK_FORMAT_R64_SFLOAT:
      formatSize.packed = false;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 8 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_R64G64_UINT:
    case VK_FORMAT_R64G64_SINT:
    case VK_FORMAT_R64G64_SFLOAT:
      formatSize.packed = false;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 16 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_R64G64B64_UINT:
    case VK_FORMAT_R64G64B64_SINT:
    case VK_FORMAT_R64G64B64_SFLOAT:
      formatSize.packed = false;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 24 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_R64G64B64A64_UINT:
    case VK_FORMAT_R64G64B64A64_SINT:
    case VK_FORMAT_R64G64B64A64_SFLOAT:
      formatSize.packed = false;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 32 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
    case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
      formatSize.packed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 4 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_D16_UNORM:
      formatSize.packed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 2 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_X8_D24_UNORM_PACK32:
      formatSize.packed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 4 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_D32_SFLOAT:
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 4 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_S8_UINT:
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 1 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_D16_UNORM_S8_UINT:
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 3 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_D24_UNORM_S8_UINT:
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 4 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 8 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
    case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
    case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
    case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
      formatSize.compressed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 8 * 8;
      formatSize.blockWidth = 4;
      formatSize.blockHeight = 4;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_BC2_UNORM_BLOCK:
    case VK_FORMAT_BC2_SRGB_BLOCK:
    case VK_FORMAT_BC3_UNORM_BLOCK:
    case VK_FORMAT_BC3_SRGB_BLOCK:
    case VK_FORMAT_BC4_UNORM_BLOCK:
    case VK_FORMAT_BC4_SNORM_BLOCK:
    case VK_FORMAT_BC5_UNORM_BLOCK:
    case VK_FORMAT_BC5_SNORM_BLOCK:
    case VK_FORMAT_BC6H_UFLOAT_BLOCK:
    case VK_FORMAT_BC6H_SFLOAT_BLOCK:
    case VK_FORMAT_BC7_UNORM_BLOCK:
    case VK_FORMAT_BC7_SRGB_BLOCK:
      formatSize.compressed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 16 * 8;
      formatSize.blockWidth = 4;
      formatSize.blockHeight = 4;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
    case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
    case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
    case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
      formatSize.compressed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 8 * 8;
      formatSize.blockWidth = 4;
      formatSize.blockHeight = 4;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
    case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
    case VK_FORMAT_EAC_R11_UNORM_BLOCK:
    case VK_FORMAT_EAC_R11_SNORM_BLOCK:
    case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
    case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
      formatSize.compressed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 16 * 8;
      formatSize.blockWidth = 4;
      formatSize.blockHeight = 4;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
    case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
      formatSize.compressed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 16 * 8;
      formatSize.blockWidth = 4;
      formatSize.blockHeight = 4;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
    case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
      formatSize.compressed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 16 * 8;
      formatSize.blockWidth = 5;
      formatSize.blockHeight = 4;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
    case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
      formatSize.compressed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 16 * 8;
      formatSize.blockWidth = 5;
      formatSize.blockHeight = 5;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
    case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
      formatSize.compressed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 16 * 8;
      formatSize.blockWidth = 6;
      formatSize.blockHeight = 5;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
    case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
      formatSize.compressed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 16 * 8;
      formatSize.blockWidth = 6;
      formatSize.blockHeight = 6;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
    case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
      formatSize.compressed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 16 * 8;
      formatSize.blockWidth = 8;
      formatSize.blockHeight = 5;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
    case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
      formatSize.compressed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 16 * 8;
      formatSize.blockWidth = 8;
      formatSize.blockHeight = 6;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
    case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
      formatSize.compressed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 16 * 8;
      formatSize.blockWidth = 8;
      formatSize.blockHeight = 8;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
    case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
      formatSize.compressed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 16 * 8;
      formatSize.blockWidth = 10;
      formatSize.blockHeight = 5;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
    case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
      formatSize.compressed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 16 * 8;
      formatSize.blockWidth = 10;
      formatSize.blockHeight = 6;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
    case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
      formatSize.compressed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 16 * 8;
      formatSize.blockWidth = 10;
      formatSize.blockHeight = 8;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
    case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
      formatSize.compressed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 16 * 8;
      formatSize.blockWidth = 10;
      formatSize.blockHeight = 10;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
    case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
      formatSize.compressed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 16 * 8;
      formatSize.blockWidth = 12;
      formatSize.blockHeight = 10;
      formatSize.blockDepth = 1;
      break;
    case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
    case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
      formatSize.compressed = true;
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 16 * 8;
      formatSize.blockWidth = 12;
      formatSize.blockHeight = 12;
      formatSize.blockDepth = 1;
      break;
    default:
      formatSize.paletteSizeInBits = 0;
      formatSize.blockSizeInBits = 0 * 8;
      formatSize.blockWidth = 1;
      formatSize.blockHeight = 1;
      formatSize.blockDepth = 1;
      break;
  }

  return formatSize;
}

} // Namespace Vulkan

} // Namespace Graphics

} // Napespace Dali

#endif // DALI_GRAPHICS_VULKAN_UTILS
