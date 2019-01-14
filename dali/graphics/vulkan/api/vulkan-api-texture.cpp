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

// CLASS HEADER
#include <dali/graphics/vulkan/api/vulkan-api-texture.h>

// INTERNAL INCLUDES
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/internal/vulkan-buffer.h>
#include <dali/graphics/vulkan/internal/vulkan-command-buffer.h>
#include <dali/graphics/vulkan/internal/vulkan-command-pool.h>
#include <dali/graphics/vulkan/internal/vulkan-image.h>
#include <dali/graphics/vulkan/internal/vulkan-image-view.h>
#include <dali/graphics/vulkan/internal/vulkan-fence.h>
#include <dali/graphics/vulkan/internal/vulkan-sampler.h>
#include <dali/graphics/vulkan/internal/vulkan-utils.h>
#include <dali/graphics/vulkan/api/vulkan-api-controller.h>
#include <dali/graphics/vulkan/api/vulkan-api-buffer.h>
#include <dali/graphics/vulkan/api/vulkan-api-texture-factory.h>
#include <dali/graphics/vulkan/api/vulkan-api-controller.h>

#include <algorithm>


namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{
using namespace Dali::Graphics::Vulkan;


/**
 * Remaps components
 */
inline vk::ComponentMapping GetVkComponentMapping( API::Format format )
{
  switch( format )
  {
    case API::Format::L8:
    {
      return vk::ComponentMapping{
        vk::ComponentSwizzle::eR,
        vk::ComponentSwizzle::eR,
        vk::ComponentSwizzle::eR,
        vk::ComponentSwizzle::eOne
      };
    }
    case API::Format::L8A8:
    {
      return vk::ComponentMapping{
        vk::ComponentSwizzle::eR,
        vk::ComponentSwizzle::eR,
        vk::ComponentSwizzle::eR,
        vk::ComponentSwizzle::eG,
      };
    }
    default:
    {
      return vk::ComponentMapping{
        vk::ComponentSwizzle::eR,
        vk::ComponentSwizzle::eG,
        vk::ComponentSwizzle::eB,
        vk::ComponentSwizzle::eA
      };
    }
  }
}

/**
 * Converts API pixel format to Vulkan
 */
constexpr vk::Format ConvertApiToVk( API::Format format )
{
  switch( format )
  {
    case API::Format::L8:
    {
      return vk::Format::eR8Unorm;
    }
    case API::Format::L8A8:
    {
      return vk::Format::eR8G8Unorm;
    }
    case API::Format::UNDEFINED:
    {
      return vk::Format::eUndefined;
    }
    case API::Format::R4G4_UNORM_PACK8:
    {
      return vk::Format::eR4G4UnormPack8;
    }
    case API::Format::R4G4B4A4_UNORM_PACK16:
    {
      return vk::Format::eR4G4B4A4UnormPack16;
    }
    case API::Format::B4G4R4A4_UNORM_PACK16:
    {
      return vk::Format::eB4G4R4A4UnormPack16;
    }
    case API::Format::R5G6B5_UNORM_PACK16:
    {
      return vk::Format::eR5G6B5UnormPack16;
    }
    case API::Format::B5G6R5_UNORM_PACK16:
    {
      return vk::Format::eB5G6R5UnormPack16;
    }
    case API::Format::R5G5B5A1_UNORM_PACK16:
    {
      return vk::Format::eR5G5B5A1UnormPack16;
    }
    case API::Format::B5G5R5A1_UNORM_PACK16:
    {
      return vk::Format::eB5G5R5A1UnormPack16;
    }
    case API::Format::A1R5G5B5_UNORM_PACK16:
    {
      return vk::Format::eA1R5G5B5UnormPack16;
    }
    case API::Format::R8_UNORM:
    {
      return vk::Format::eR8Unorm;
    }
    case API::Format::R8_SNORM:
    {
      return vk::Format::eR8Snorm;
    }
    case API::Format::R8_USCALED:
    {
      return vk::Format::eR8Uscaled;
    }
    case API::Format::R8_SSCALED:
    {
      return vk::Format::eR8Sscaled;
    }
    case API::Format::R8_UINT:
    {
      return vk::Format::eR8Uint;
    }
    case API::Format::R8_SINT:
    {
      return vk::Format::eR8Sint;
    }
    case API::Format::R8_SRGB:
    {
      return vk::Format::eR8Srgb;
    }
    case API::Format::R8G8_UNORM:
    {
      return vk::Format::eR8G8Unorm;
    }
    case API::Format::R8G8_SNORM:
    {
      return vk::Format::eR8G8Snorm;
    }
    case API::Format::R8G8_USCALED:
    {
      return vk::Format::eR8G8Uscaled;
    }
    case API::Format::R8G8_SSCALED:
    {
      return vk::Format::eR8G8Sscaled;
    }
    case API::Format::R8G8_UINT:
    {
      return vk::Format::eR8G8Uint;
    }
    case API::Format::R8G8_SINT:
    {
      return vk::Format::eR8G8Sint;
    }
    case API::Format::R8G8_SRGB:
    {
      return vk::Format::eR8G8Srgb;
    }
    case API::Format::R8G8B8_UNORM:
    {
      return vk::Format::eR8G8B8Unorm;
    }
    case API::Format::R8G8B8_SNORM:
    {
      return vk::Format::eR8G8B8Snorm;
    }
    case API::Format::R8G8B8_USCALED:
    {
      return vk::Format::eR8G8B8Uscaled;
    }
    case API::Format::R8G8B8_SSCALED:
    {
      return vk::Format::eR8G8B8Sscaled;
    }
    case API::Format::R8G8B8_UINT:
    {
      return vk::Format::eR8G8B8Uint;
    }
    case API::Format::R8G8B8_SINT:
    {
      return vk::Format::eR8G8B8Sint;
    }
    case API::Format::R8G8B8_SRGB:
    {
      return vk::Format::eR8G8B8Srgb;
    }
    case API::Format::B8G8R8_UNORM:
    {
      return vk::Format::eB8G8R8Unorm;
    }
    case API::Format::B8G8R8_SNORM:
    {
      return vk::Format::eB8G8R8Snorm;
    }
    case API::Format::B8G8R8_USCALED:
    {
      return vk::Format::eB8G8R8Uscaled;
    }
    case API::Format::B8G8R8_SSCALED:
    {
      return vk::Format::eB8G8R8Sscaled;
    }
    case API::Format::B8G8R8_UINT:
    {
      return vk::Format::eB8G8R8Uint;
    }
    case API::Format::B8G8R8_SINT:
    {
      return vk::Format::eB8G8R8Sint;
    }
    case API::Format::B8G8R8_SRGB:
    {
      return vk::Format::eB8G8R8Srgb;
    }
    case API::Format::R8G8B8A8_UNORM:
    {
      return vk::Format::eR8G8B8A8Unorm;
    }
    case API::Format::R8G8B8A8_SNORM:
    {
      return vk::Format::eR8G8B8A8Snorm;
    }
    case API::Format::R8G8B8A8_USCALED:
    {
      return vk::Format::eR8G8B8A8Uscaled;
    }
    case API::Format::R8G8B8A8_SSCALED:
    {
      return vk::Format::eR8G8B8A8Sscaled;
    }
    case API::Format::R8G8B8A8_UINT:
    {
      return vk::Format::eR8G8B8A8Uint;
    }
    case API::Format::R8G8B8A8_SINT:
    {
      return vk::Format::eR8G8B8A8Sint;
    }
    case API::Format::R8G8B8A8_SRGB:
    {
      return vk::Format::eR8G8B8A8Srgb;
    }
    case API::Format::B8G8R8A8_UNORM:
    {
      return vk::Format::eB8G8R8A8Unorm;
    }
    case API::Format::B8G8R8A8_SNORM:
    {
      return vk::Format::eB8G8R8A8Snorm;
    }
    case API::Format::B8G8R8A8_USCALED:
    {
      return vk::Format::eB8G8R8A8Uscaled;
    }
    case API::Format::B8G8R8A8_SSCALED:
    {
      return vk::Format::eB8G8R8A8Sscaled;
    }
    case API::Format::B8G8R8A8_UINT:
    {
      return vk::Format::eB8G8R8A8Uint;
    }
    case API::Format::B8G8R8A8_SINT:
    {
      return vk::Format::eB8G8R8A8Sint;
    }
    case API::Format::B8G8R8A8_SRGB:
    {
      return vk::Format::eB8G8R8A8Srgb;
    }
    case API::Format::A8B8G8R8_UNORM_PACK32:
    {
      return vk::Format::eA8B8G8R8UnormPack32;
    }
    case API::Format::A8B8G8R8_SNORM_PACK32:
    {
      return vk::Format::eA8B8G8R8SnormPack32;
    }
    case API::Format::A8B8G8R8_USCALED_PACK32:
    {
      return vk::Format::eA8B8G8R8UscaledPack32;
    }
    case API::Format::A8B8G8R8_SSCALED_PACK32:
    {
      return vk::Format::eA8B8G8R8SscaledPack32;
    }
    case API::Format::A8B8G8R8_UINT_PACK32:
    {
      return vk::Format::eA8B8G8R8UintPack32;
    }
    case API::Format::A8B8G8R8_SINT_PACK32:
    {
      return vk::Format::eA8B8G8R8SintPack32;
    }
    case API::Format::A8B8G8R8_SRGB_PACK32:
    {
      return vk::Format::eA8B8G8R8SrgbPack32;
    }
    case API::Format::A2R10G10B10_UNORM_PACK32:
    {
      return vk::Format::eA2R10G10B10UnormPack32;
    }
    case API::Format::A2R10G10B10_SNORM_PACK32:
    {
      return vk::Format::eA2R10G10B10SnormPack32;
    }
    case API::Format::A2R10G10B10_USCALED_PACK32:
    {
      return vk::Format::eA2R10G10B10UscaledPack32;
    }
    case API::Format::A2R10G10B10_SSCALED_PACK32:
    {
      return vk::Format::eA2R10G10B10SscaledPack32;
    }
    case API::Format::A2R10G10B10_UINT_PACK32:
    {
      return vk::Format::eA2R10G10B10UintPack32;
    }
    case API::Format::A2R10G10B10_SINT_PACK32:
    {
      return vk::Format::eA2R10G10B10SintPack32;
    }
    case API::Format::A2B10G10R10_UNORM_PACK32:
    {
      return vk::Format::eA2B10G10R10UnormPack32;
    }
    case API::Format::A2B10G10R10_SNORM_PACK32:
    {
      return vk::Format::eA2B10G10R10SnormPack32;
    }
    case API::Format::A2B10G10R10_USCALED_PACK32:
    {
      return vk::Format::eA2B10G10R10UscaledPack32;
    }
    case API::Format::A2B10G10R10_SSCALED_PACK32:
    {
      return vk::Format::eA2B10G10R10SscaledPack32;
    }
    case API::Format::A2B10G10R10_UINT_PACK32:
    {
      return vk::Format::eA2B10G10R10UintPack32;
    }
    case API::Format::A2B10G10R10_SINT_PACK32:
    {
      return vk::Format::eA2B10G10R10SintPack32;
    }
    case API::Format::R16_UNORM:
    {
      return vk::Format::eR16Unorm;
    }
    case API::Format::R16_SNORM:
    {
      return vk::Format::eR16Snorm;
    }
    case API::Format::R16_USCALED:
    {
      return vk::Format::eR16Uscaled;
    }
    case API::Format::R16_SSCALED:
    {
      return vk::Format::eR16Sscaled;
    }
    case API::Format::R16_UINT:
    {
      return vk::Format::eR16Uint;
    }
    case API::Format::R16_SINT:
    {
      return vk::Format::eR16Sint;
    }
    case API::Format::R16_SFLOAT:
    {
      return vk::Format::eR16Sfloat;
    }
    case API::Format::R16G16_UNORM:
    {
      return vk::Format::eR16G16Unorm;
    }
    case API::Format::R16G16_SNORM:
    {
      return vk::Format::eR16G16Snorm;
    }
    case API::Format::R16G16_USCALED:
    {
      return vk::Format::eR16G16Uscaled;
    }
    case API::Format::R16G16_SSCALED:
    {
      return vk::Format::eR16G16Sscaled;
    }
    case API::Format::R16G16_UINT:
    {
      return vk::Format::eR16G16Uint;
    }
    case API::Format::R16G16_SINT:
    {
      return vk::Format::eR16G16Sint;
    }
    case API::Format::R16G16_SFLOAT:
    {
      return vk::Format::eR16G16Sfloat;
    }
    case API::Format::R16G16B16_UNORM:
    {
      return vk::Format::eR16G16B16Unorm;
    }
    case API::Format::R16G16B16_SNORM:
    {
      return vk::Format::eR16G16B16Snorm;
    }
    case API::Format::R16G16B16_USCALED:
    {
      return vk::Format::eR16G16B16Uscaled;
    }
    case API::Format::R16G16B16_SSCALED:
    {
      return vk::Format::eR16G16B16Sscaled;
    }
    case API::Format::R16G16B16_UINT:
    {
      return vk::Format::eR16G16B16Uint;
    }
    case API::Format::R16G16B16_SINT:
    {
      return vk::Format::eR16G16B16Sint;
    }
    case API::Format::R16G16B16_SFLOAT:
    {
      return vk::Format::eR16G16B16Sfloat;
    }
    case API::Format::R16G16B16A16_UNORM:
    {
      return vk::Format::eR16G16B16A16Unorm;
    }
    case API::Format::R16G16B16A16_SNORM:
    {
      return vk::Format::eR16G16B16A16Snorm;
    }
    case API::Format::R16G16B16A16_USCALED:
    {
      return vk::Format::eR16G16B16A16Uscaled;
    }
    case API::Format::R16G16B16A16_SSCALED:
    {
      return vk::Format::eR16G16B16A16Sscaled;
    }
    case API::Format::R16G16B16A16_UINT:
    {
      return vk::Format::eR16G16B16A16Uint;
    }
    case API::Format::R16G16B16A16_SINT:
    {
      return vk::Format::eR16G16B16A16Sint;
    }
    case API::Format::R16G16B16A16_SFLOAT:
    {
      return vk::Format::eR16G16B16A16Sfloat;
    }
    case API::Format::R32_UINT:
    {
      return vk::Format::eR32Uint;
    }
    case API::Format::R32_SINT:
    {
      return vk::Format::eR32Sint;
    }
    case API::Format::R32_SFLOAT:
    {
      return vk::Format::eR32Sfloat;
    }
    case API::Format::R32G32_UINT:
    {
      return vk::Format::eR32G32Uint;
    }
    case API::Format::R32G32_SINT:
    {
      return vk::Format::eR32G32Sint;
    }
    case API::Format::R32G32_SFLOAT:
    {
      return vk::Format::eR32G32Sfloat;
    }
    case API::Format::R32G32B32_UINT:
    {
      return vk::Format::eR32G32B32Uint;
    }
    case API::Format::R32G32B32_SINT:
    {
      return vk::Format::eR32G32B32Sint;
    }
    case API::Format::R32G32B32_SFLOAT:
    {
      return vk::Format::eR32G32B32Sfloat;
    }
    case API::Format::R32G32B32A32_UINT:
    {
      return vk::Format::eR32G32B32A32Uint;
    }
    case API::Format::R32G32B32A32_SINT:
    {
      return vk::Format::eR32G32B32A32Sint;
    }
    case API::Format::R32G32B32A32_SFLOAT:
    {
      return vk::Format::eR32G32B32A32Sfloat;
    }
    case API::Format::R64_UINT:
    {
      return vk::Format::eR64Uint;
    }
    case API::Format::R64_SINT:
    {
      return vk::Format::eR64Sint;
    }
    case API::Format::R64_SFLOAT:
    {
      return vk::Format::eR64Sfloat;
    }
    case API::Format::R64G64_UINT:
    {
      return vk::Format::eR64G64Uint;
    }
    case API::Format::R64G64_SINT:
    {
      return vk::Format::eR64G64Sint;
    }
    case API::Format::R64G64_SFLOAT:
    {
      return vk::Format::eR64G64Sfloat;
    }
    case API::Format::R64G64B64_UINT:
    {
      return vk::Format::eR64G64B64Uint;
    }
    case API::Format::R64G64B64_SINT:
    {
      return vk::Format::eR64G64B64Sint;
    }
    case API::Format::R64G64B64_SFLOAT:
    {
      return vk::Format::eR64G64B64Sfloat;
    }
    case API::Format::R64G64B64A64_UINT:
    {
      return vk::Format::eR64G64B64A64Uint;
    }
    case API::Format::R64G64B64A64_SINT:
    {
      return vk::Format::eR64G64B64A64Sint;
    }
    case API::Format::R64G64B64A64_SFLOAT:
    {
      return vk::Format::eR64G64B64A64Sfloat;
    }
    case API::Format::B10G11R11_UFLOAT_PACK32:
    {
      return vk::Format::eB10G11R11UfloatPack32;
    }
    case API::Format::E5B9G9R9_UFLOAT_PACK32:
    {
      return vk::Format::eE5B9G9R9UfloatPack32;
    }
    case API::Format::D16_UNORM:
    {
      return vk::Format::eD16Unorm;
    }
    case API::Format::X8_D24_UNORM_PACK32:
    {
      return vk::Format::eX8D24UnormPack32;
    }
    case API::Format::D32_SFLOAT:
    {
      return vk::Format::eD32Sfloat;
    }
    case API::Format::S8_UINT:
    {
      return vk::Format::eS8Uint;
    }
    case API::Format::D16_UNORM_S8_UINT:
    {
      return vk::Format::eD16UnormS8Uint;
    }
    case API::Format::D24_UNORM_S8_UINT:
    {
      return vk::Format::eD24UnormS8Uint;
    }
    case API::Format::D32_SFLOAT_S8_UINT:
    {
      return vk::Format::eD32SfloatS8Uint;
    }
    case API::Format::BC1_RGB_UNORM_BLOCK:
    {
      return vk::Format::eBc1RgbUnormBlock;
    }
    case API::Format::BC1_RGB_SRGB_BLOCK:
    {
      return vk::Format::eBc1RgbSrgbBlock;
    }
    case API::Format::BC1_RGBA_UNORM_BLOCK:
    {
      return vk::Format::eBc1RgbaUnormBlock;
    }
    case API::Format::BC1_RGBA_SRGB_BLOCK:
    {
      return vk::Format::eBc1RgbaSrgbBlock;
    }
    case API::Format::BC2_UNORM_BLOCK:
    {
      return vk::Format::eBc2UnormBlock;
    }
    case API::Format::BC2_SRGB_BLOCK:
    {
      return vk::Format::eBc2SrgbBlock;
    }
    case API::Format::BC3_UNORM_BLOCK:
    {
      return vk::Format::eBc3UnormBlock;
    }
    case API::Format::BC3_SRGB_BLOCK:
    {
      return vk::Format::eBc3SrgbBlock;
    }
    case API::Format::BC4_UNORM_BLOCK:
    {
      return vk::Format::eBc4UnormBlock;
    }
    case API::Format::BC4_SNORM_BLOCK:
    {
      return vk::Format::eBc4SnormBlock;
    }
    case API::Format::BC5_UNORM_BLOCK:
    {
      return vk::Format::eBc5UnormBlock;
    }
    case API::Format::BC5_SNORM_BLOCK:
    {
      return vk::Format::eBc5SnormBlock;
    }
    case API::Format::BC6H_UFLOAT_BLOCK:
    {
      return vk::Format::eBc6HUfloatBlock;
    }
    case API::Format::BC6H_SFLOAT_BLOCK:
    {
      return vk::Format::eBc6HSfloatBlock;
    }
    case API::Format::BC7_UNORM_BLOCK:
    {
      return vk::Format::eBc7UnormBlock;
    }
    case API::Format::BC7_SRGB_BLOCK:
    {
      return vk::Format::eBc7SrgbBlock;
    }
    case API::Format::ETC2_R8G8B8_UNORM_BLOCK:
    {
      return vk::Format::eEtc2R8G8B8UnormBlock;
    }
    case API::Format::ETC2_R8G8B8_SRGB_BLOCK:
    {
      return vk::Format::eEtc2R8G8B8SrgbBlock;
    }
    case API::Format::ETC2_R8G8B8A1_UNORM_BLOCK:
    {
      return vk::Format::eEtc2R8G8B8A1UnormBlock;
    }
    case API::Format::ETC2_R8G8B8A1_SRGB_BLOCK:
    {
      return vk::Format::eEtc2R8G8B8A1SrgbBlock;
    }
    case API::Format::ETC2_R8G8B8A8_UNORM_BLOCK:
    {
      return vk::Format::eEtc2R8G8B8A8UnormBlock;
    }
    case API::Format::ETC2_R8G8B8A8_SRGB_BLOCK:
    {
      return vk::Format::eEtc2R8G8B8A8SrgbBlock;
    }
    case API::Format::EAC_R11_UNORM_BLOCK:
    {
      return vk::Format::eEacR11UnormBlock;
    }
    case API::Format::EAC_R11_SNORM_BLOCK:
    {
      return vk::Format::eEacR11SnormBlock;
    }
    case API::Format::EAC_R11G11_UNORM_BLOCK:
    {
      return vk::Format::eEacR11G11UnormBlock;
    }
    case API::Format::EAC_R11G11_SNORM_BLOCK:
    {
      return vk::Format::eEacR11G11SnormBlock;
    }
    case API::Format::ASTC_4x4_UNORM_BLOCK:
    {
      return vk::Format::eAstc4x4UnormBlock;
    }
    case API::Format::ASTC_4x4_SRGB_BLOCK:
    {
      return vk::Format::eAstc4x4SrgbBlock;
    }
    case API::Format::ASTC_5x4_UNORM_BLOCK:
    {
      return vk::Format::eAstc5x4UnormBlock;
    }
    case API::Format::ASTC_5x4_SRGB_BLOCK:
    {
      return vk::Format::eAstc5x4SrgbBlock;
    }
    case API::Format::ASTC_5x5_UNORM_BLOCK:
    {
      return vk::Format::eAstc5x5UnormBlock;
    }
    case API::Format::ASTC_5x5_SRGB_BLOCK:
    {
      return vk::Format::eAstc5x5SrgbBlock;
    }
    case API::Format::ASTC_6x5_UNORM_BLOCK:
    {
      return vk::Format::eAstc6x5UnormBlock;
    }
    case API::Format::ASTC_6x5_SRGB_BLOCK:
    {
      return vk::Format::eAstc6x5SrgbBlock;
    }
    case API::Format::ASTC_6x6_UNORM_BLOCK:
    {
      return vk::Format::eAstc6x6UnormBlock;
    }
    case API::Format::ASTC_6x6_SRGB_BLOCK:
    {
      return vk::Format::eAstc6x6SrgbBlock;
    }
    case API::Format::ASTC_8x5_UNORM_BLOCK:
    {
      return vk::Format::eAstc8x5UnormBlock;
    }
    case API::Format::ASTC_8x5_SRGB_BLOCK:
    {
      return vk::Format::eAstc8x5SrgbBlock;
    }
    case API::Format::ASTC_8x6_UNORM_BLOCK:
    {
      return vk::Format::eAstc8x6UnormBlock;
    }
    case API::Format::ASTC_8x6_SRGB_BLOCK:
    {
      return vk::Format::eAstc8x6SrgbBlock;
    }
    case API::Format::ASTC_8x8_UNORM_BLOCK:
    {
      return vk::Format::eAstc8x8UnormBlock;
    }
    case API::Format::ASTC_8x8_SRGB_BLOCK:
    {
      return vk::Format::eAstc8x8SrgbBlock;
    }
    case API::Format::ASTC_10x5_UNORM_BLOCK:
    {
      return vk::Format::eAstc10x5UnormBlock;
    }
    case API::Format::ASTC_10x5_SRGB_BLOCK:
    {
      return vk::Format::eAstc10x5SrgbBlock;
    }
    case API::Format::ASTC_10x6_UNORM_BLOCK:
    {
      return vk::Format::eAstc10x6UnormBlock;
    }
    case API::Format::ASTC_10x6_SRGB_BLOCK:
    {
      return vk::Format::eAstc10x6SrgbBlock;
    }
    case API::Format::ASTC_10x8_UNORM_BLOCK:
    {
      return vk::Format::eAstc10x8UnormBlock;
    }
    case API::Format::ASTC_10x8_SRGB_BLOCK:
    {
      return vk::Format::eAstc10x8SrgbBlock;
    }
    case API::Format::ASTC_10x10_UNORM_BLOCK:
    {
      return vk::Format::eAstc10x10UnormBlock;
    }
    case API::Format::ASTC_10x10_SRGB_BLOCK:
    {
      return vk::Format::eAstc10x10SrgbBlock;
    }
    case API::Format::ASTC_12x10_UNORM_BLOCK:
    {
      return vk::Format::eAstc12x10UnormBlock;
    }
    case API::Format::ASTC_12x10_SRGB_BLOCK:
    {
      return vk::Format::eAstc12x10SrgbBlock;
    }
    case API::Format::ASTC_12x12_UNORM_BLOCK:
    {
      return vk::Format::eAstc12x12UnormBlock;
    }
    case API::Format::ASTC_12x12_SRGB_BLOCK:
    {
      return vk::Format::eAstc12x12SrgbBlock;
    }
    case API::Format::PVRTC1_2BPP_UNORM_BLOCK_IMG:
    {
      return vk::Format::ePvrtc12BppUnormBlockIMG;
    }
    case API::Format::PVRTC1_4BPP_UNORM_BLOCK_IMG:
    {
      return vk::Format::ePvrtc14BppUnormBlockIMG;
    }
    case API::Format::PVRTC2_2BPP_UNORM_BLOCK_IMG:
    {
      return vk::Format::ePvrtc22BppUnormBlockIMG;
    }
    case API::Format::PVRTC2_4BPP_UNORM_BLOCK_IMG:
    {
      return vk::Format::ePvrtc24BppUnormBlockIMG;
    }
    case API::Format::PVRTC1_2BPP_SRGB_BLOCK_IMG:
    {
      return vk::Format::ePvrtc12BppSrgbBlockIMG;
    }
    case API::Format::PVRTC1_4BPP_SRGB_BLOCK_IMG:
    {
      return vk::Format::ePvrtc14BppSrgbBlockIMG;
    }
    case API::Format::PVRTC2_2BPP_SRGB_BLOCK_IMG:
    {
      return vk::Format::ePvrtc22BppSrgbBlockIMG;
    }
    case API::Format::PVRTC2_4BPP_SRGB_BLOCK_IMG:
    {
      return vk::Format::ePvrtc24BppSrgbBlockIMG;
    }
  }
  return {};
}

struct ColorConversion
{
  vk::Format oldFormat;
  vk::Format newFormat;
  std::vector<uint8_t>(*pConversionFunc)(const void*, uint32_t, uint32_t, uint32_t, uint32_t);
  void(*pConversionWriteFunc)(const void*, uint32_t, uint32_t, uint32_t, uint32_t, void*);
};

/**
 * Converts RGB to RGBA
 */
inline std::vector<uint8_t> ConvertRGB32ToRGBA32( const void* pData, uint32_t sizeInBytes, uint32_t width, uint32_t height, uint32_t rowStride )
{
  //@todo: use stride if non-zero
  std::vector<uint8_t> rgbaBuffer{};

  auto inData = reinterpret_cast<const uint8_t*>(pData);

  rgbaBuffer.resize( width * height * 4 );
  auto outData = rgbaBuffer.data();
  auto outIdx = 0u;
  for( auto i = 0u; i < sizeInBytes; i += 3 )
  {
    outData[outIdx] = inData[i];
    outData[outIdx + 1] = inData[i + 1];
    outData[outIdx + 2] = inData[i + 2];
    outData[outIdx + 3] = 0xff;
    outIdx += 4;
  }
  return rgbaBuffer;
}

inline void WriteRGB32ToRGBA32( const void* pData, uint32_t sizeInBytes, uint32_t width, uint32_t height, uint32_t rowStride, void* pOutput )
{
  auto inData = reinterpret_cast<const uint8_t*>(pData);
  auto outData = reinterpret_cast<uint8_t*>(pOutput);
  auto outIdx = 0u;
  for( auto i = 0u; i < sizeInBytes; i += 3 )
  {
    outData[outIdx] = inData[i];
    outData[outIdx + 1] = inData[i + 1];
    outData[outIdx + 2] = inData[i + 2];
    outData[outIdx + 3] = 0xff;
    outIdx += 4;
  }
}

/**
 * Format conversion table
 */
static const std::vector<ColorConversion> COLOR_CONVERSION_TABLE =
                                      {
                                        { vk::Format::eR8G8B8Unorm, vk::Format::eR8G8B8A8Unorm, ConvertRGB32ToRGBA32, WriteRGB32ToRGBA32 }
                                      };

/**
 * This function tests whether format is supported by the driver. If possible it applies
 * format conversion to suitable supported pixel format.
 */
bool Texture::TryConvertPixelData( const void* pData, uint32_t sizeInBytes, uint32_t width, uint32_t height, std::vector<uint8_t>& outputBuffer )
{
  // No need to convert
  if( mConvertFromFormat == vk::Format::eUndefined )
  {
    return false;
  }

  auto it = std::find_if( COLOR_CONVERSION_TABLE.begin(), COLOR_CONVERSION_TABLE.end(), [&]( auto& item )
  {
    return item.oldFormat == mConvertFromFormat;
  });

  // No suitable format, return empty array
  if( it == COLOR_CONVERSION_TABLE.end() )
  {
    return false;
  }

  auto begin = reinterpret_cast<const uint8_t*>( pData );

  outputBuffer = std::move( it->pConversionFunc( begin, sizeInBytes, width, height, 0u ) );
  return !outputBuffer.empty();
}

bool Texture::TryConvertPixelData( const void* pData, uint32_t sizeInBytes, uint32_t width, uint32_t height, void* pOutputBuffer )
{
  // No need to convert
  if( mConvertFromFormat == vk::Format::eUndefined )
  {
    return false;
  }

  auto it = std::find_if( COLOR_CONVERSION_TABLE.begin(), COLOR_CONVERSION_TABLE.end(), [&]( auto& item )
  {
    return item.oldFormat == mConvertFromFormat;
  });

  // No suitable format, return empty array
  if( it == COLOR_CONVERSION_TABLE.end() )
  {
    return false;
  }

  auto begin = reinterpret_cast<const uint8_t*>( pData );

  it->pConversionWriteFunc( begin, sizeInBytes, width, height, 0u, pOutputBuffer );

  return true;
}

vk::Format Texture::ValidateFormat( vk::Format sourceFormat )
{
  auto formatProperties = mGraphics.GetPhysicalDevice().getFormatProperties( sourceFormat );
  vk::FormatFeatureFlags formatFlags = ( mDisableStagingBuffer ? formatProperties.linearTilingFeatures : formatProperties.optimalTilingFeatures );

  auto retval = vk::Format::eUndefined;

  // if format isn't supported, see whether suitable conversion is implemented
  if( !formatFlags )
  {
    auto it = std::find_if( COLOR_CONVERSION_TABLE.begin(), COLOR_CONVERSION_TABLE.end(), [&]( auto& item )
    {
      return item.oldFormat == sourceFormat;
    });

    // No suitable format, return empty array
    if( it != COLOR_CONVERSION_TABLE.end() )
    {
      retval = it->newFormat;
    }
  }
  else
  {
    retval = sourceFormat;
  }

  return retval;
}


Texture::Texture( Dali::Graphics::API::TextureFactory& factory )
  : mTextureFactory( static_cast<VulkanAPI::TextureFactory&>(factory).Clone() ),
    mController( mTextureFactory->GetController() ),
    mGraphics( mTextureFactory->GetGraphics() ),
    mImage(),
    mImageView(),
    mSampler(),
    mWidth( 0u ),
    mHeight( 0u ),
    mFormat( vk::Format::eUndefined ),
    mUsage( vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst ),
    mLayout( vk::ImageLayout::eUndefined ),
    mComponentMapping()
{
  // Check env variable in order to disable staging buffers
  auto var = getenv( "DALI_DISABLE_TEXTURE_STAGING_BUFFERS" );
  if( var && var[0] != '0')
  {
    mDisableStagingBuffer = true;
  }
}

Texture::~Texture() = default;

bool Texture::Initialise()
{
  auto size = mTextureFactory->GetSize();
  mWidth = uint32_t( size.width );
  mHeight = uint32_t( size.height );
  mLayout = vk::ImageLayout::eUndefined;
  switch( mTextureFactory->GetUsage())
  {
    case API::TextureDetails::Usage::COLOR_ATTACHMENT:
    {
      mUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled;
      break;
    }
    case API::TextureDetails::Usage::DEPTH_ATTACHMENT:
    {
      mUsage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled;
      break;
    }
    case API::TextureDetails::Usage::SAMPLE:
    {
      mUsage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
      break;
    }
  }

  auto format = ConvertApiToVk( mTextureFactory->GetFormat() );

  mFormat = ValidateFormat( format );
  mConvertFromFormat = vk::Format::eUndefined;
  if( mFormat == vk::Format::eUndefined )
  {
    // not supported!
    return false;
  }

  if( format != mFormat )
  {
    mConvertFromFormat = format;
  }

  mComponentMapping = GetVkComponentMapping( mTextureFactory->GetFormat() );

  auto retval = InitialiseTexture();

  // force generating properties
  GetProperties();

  return retval;
}

void Texture::CopyMemory(const void *srcMemory, uint32_t srcMemorySize, API::Extent2D srcExtent, API::Offset2D dstOffset, uint32_t layer, uint32_t level, API::TextureDetails::UpdateMode updateMode )
{
  if(!mImageView)
  {
    InitialiseMemory();
  }

  // Validate source
  std::vector<uint8_t> convertedData{};

  if( mConvertFromFormat != vk::Format::eUndefined )
  {
    if( TryConvertPixelData( srcMemory, srcMemorySize, srcExtent.width, srcExtent.height, convertedData ) )
    {
      srcMemorySize = uint32_t( convertedData.size() );
      srcMemory = convertedData.data();
    }
    else
    {
      // format unsupported, early exit
      return;
    }
  }

  if( !mDisableStagingBuffer )
  {
    // @todo transient buffer memory could be persistently mapped and aliased ( work like a per-frame stack )
    uint32_t allocationSize{};

    auto requirements = mGraphics.GetDevice().getImageMemoryRequirements( mImage->GetVkHandle() );
    allocationSize = uint32_t( requirements.size );

    // allocate transient buffer
    auto buffer = mGraphics.CreateBuffer( vk::BufferCreateInfo{}
                              .setSize( allocationSize )
                              .setSharingMode( vk::SharingMode::eExclusive )
                              .setUsage( vk::BufferUsageFlagBits::eTransferSrc));

    // bind memory
    mGraphics.BindBufferMemory( buffer,
                                mGraphics.AllocateMemory( buffer, vk::MemoryPropertyFlagBits::eHostVisible|vk::MemoryPropertyFlagBits::eHostCoherent ),
                                0u );

    // write into the buffer
    auto ptr = buffer->GetMemory()->MapTyped<char>();
    std::copy( reinterpret_cast<const char*>(srcMemory), reinterpret_cast<const char*>(srcMemory)+srcMemorySize, ptr );
    buffer->GetMemory()->Unmap();

    ResourceTransferRequest transferRequest( TransferRequestType::BUFFER_TO_IMAGE );

    transferRequest.bufferToImageInfo.copyInfo
            .setImageSubresource( vk::ImageSubresourceLayers{}
                                      .setBaseArrayLayer( layer )
                                      .setLayerCount( 1 )
                                      .setAspectMask( vk::ImageAspectFlagBits::eColor )
                                      .setMipLevel( level ) )
            .setImageOffset({ dstOffset.x, dstOffset.y, 0 } )
            .setImageExtent({ srcExtent.width, srcExtent.height, 1 } )
            .setBufferRowLength({ 0u })
            .setBufferOffset({ 0u })
            .setBufferImageHeight( 0u );

    transferRequest.bufferToImageInfo.dstImage = mImage;
    transferRequest.bufferToImageInfo.srcBuffer = std::move(buffer);
    transferRequest.deferredTransferMode = !( updateMode == API::TextureDetails::UpdateMode::IMMEDIATE );

    assert( transferRequest.bufferToImageInfo.srcBuffer.GetRefCount() == 1 && "Too many transient buffer owners, buffer will be released automatically!" );

    // schedule transfer
    mController.ScheduleResourceTransfer( std::move(transferRequest) );
  }
  else
  {
    auto memory = mImage->GetMemory();
    if( !memory )
    {
      return;
    }
    auto ptr = mImage->GetMemory()->MapTyped<char>();
    auto subresourceLayout = mGraphics.GetDevice().
      getImageSubresourceLayout( mImage->GetVkHandle(),
                                 vk::ImageSubresource{}
                                   .setAspectMask( vk::ImageAspectFlagBits::eColor )
                                   .setMipLevel( 0 )
                                   .setArrayLayer( 0 ));

    auto format = mImage->GetFormat();
    auto formatInfo = Vulkan::GetFormatInfo( format );
    int  sizeInBytes = int(formatInfo.blockSizeInBits / 8);
    auto dstRowLength = subresourceLayout.rowPitch;
    auto dstPtr = ptr + int(dstRowLength)*dstOffset.y + sizeInBytes*dstOffset.x;
    auto srcPtr = reinterpret_cast<const char*>( srcMemory );
    auto srcRowLength = int(srcExtent.width)*sizeInBytes;

    for( auto i = 0u; i < srcExtent.height; ++i )
    {
      std::copy( srcPtr, srcPtr + int(srcExtent.width)*sizeInBytes, dstPtr );

      dstPtr += dstRowLength;
      srcPtr += srcRowLength;
    }

    mImage->GetMemory()->Unmap();

    ResourceTransferRequest transferRequest( TransferRequestType::LAYOUT_TRANSITION_ONLY );

    transferRequest.imageLayoutTransitionInfo.image = mImage;
    transferRequest.imageLayoutTransitionInfo.srcLayout = mImage->GetImageLayout();
    transferRequest.imageLayoutTransitionInfo.dstLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    transferRequest.deferredTransferMode = !( updateMode == API::TextureDetails::UpdateMode::IMMEDIATE );

    // schedule transfer
    mController.ScheduleResourceTransfer( std::move(transferRequest) );
  }
}

void Texture::CopyTexture(const API::Texture &srcTexture, API::Rect2D srcRegion, API::Offset2D dstOffset, uint32_t layer, uint32_t level, API::TextureDetails::UpdateMode updateMode )
{
  if(!mImageView)
  {
    InitialiseMemory();
  }

  ResourceTransferRequest transferRequest( TransferRequestType::IMAGE_TO_IMAGE );

  auto imageSubresourceLayers = vk::ImageSubresourceLayers{}
       .setAspectMask( vk::ImageAspectFlagBits::eColor )
       .setBaseArrayLayer( layer )
       .setLayerCount( 1 )
       .setMipLevel( level );

  transferRequest.imageToImageInfo.srcImage = static_cast<const VulkanAPI::Texture&>( srcTexture ).GetImageRef();
  transferRequest.imageToImageInfo.dstImage = mImage;
  transferRequest.imageToImageInfo.copyInfo
                 .setSrcOffset( { srcRegion.x, srcRegion.y, 0 } )
                 .setDstOffset( { dstOffset.x, dstOffset.y, 0 } )
                 .setExtent( { srcRegion.width, srcRegion.height } )
                 .setSrcSubresource( imageSubresourceLayers  )
                 .setDstSubresource( imageSubresourceLayers );

  transferRequest.deferredTransferMode = !( updateMode == API::TextureDetails::UpdateMode::IMMEDIATE );

  // schedule transfer
  mController.ScheduleResourceTransfer( std::move(transferRequest) );
}

void Texture::CopyBuffer(const API::Buffer& buffer,
                uint32_t bufferOffset,
                API::Extent2D extent2D,
                API::Offset2D textureOffset2D,
                uint32_t layer,
                uint32_t level,
                API::TextureUpdateFlags flags )
{
  if(!mImageView)
  {
    InitialiseMemory();
  }

  ResourceTransferRequest transferRequest( TransferRequestType::BUFFER_TO_IMAGE );

  transferRequest.bufferToImageInfo.copyInfo
                 .setImageSubresource( vk::ImageSubresourceLayers{}
                                         .setBaseArrayLayer( layer )
                                         .setLayerCount( 1 )
                                         .setAspectMask( vk::ImageAspectFlagBits::eColor )
                                         .setMipLevel( level ) )
                 .setImageOffset({ textureOffset2D.x, textureOffset2D.y, 0 } )
                 .setImageExtent({ extent2D.width, extent2D.height, 1 } )
                 .setBufferRowLength({ 0u })
                 .setBufferOffset({ bufferOffset })
                 .setBufferImageHeight({ extent2D.height });

  transferRequest.bufferToImageInfo.dstImage = mImage;
  transferRequest.bufferToImageInfo.srcBuffer = static_cast<const VulkanAPI::Buffer&>(buffer).GetBufferRef();
  transferRequest.deferredTransferMode = true;

  // schedule transfer
  mController.ScheduleResourceTransfer( std::move(transferRequest) );
}

// creates image with pre-allocated memory and default sampler, no data
// uploaded at this point
bool Texture::InitialiseTexture()
{
  if( mImage )
  {
    return false;
  }

  // create image
  auto imageCreateInfo = vk::ImageCreateInfo{}
    .setFormat( mFormat )
    .setInitialLayout( mLayout )
    .setSamples( vk::SampleCountFlagBits::e1 )
    .setSharingMode( vk::SharingMode::eExclusive )
    .setUsage( mUsage )
    .setExtent( { mWidth, mHeight, 1 } )
    .setArrayLayers( 1 )
    .setImageType( vk::ImageType::e2D )
    .setTiling( mDisableStagingBuffer ? vk::ImageTiling::eLinear : vk::ImageTiling::eOptimal )
    .setMipLevels( 1 );

  // Create the image handle
  mImage = mGraphics.CreateImage( imageCreateInfo );

  if( !mImage )
  {
    return false;
  }

  //InitialiseMemory();

  return true;
}

bool Texture::InitialiseMemory()
{
  if( !mImageView )
  {
    // allocate memory for the image
    auto memory = mGraphics.AllocateMemory( mImage,
                                            mDisableStagingBuffer ?
                                            vk::MemoryPropertyFlagBits::eHostVisible|vk::MemoryPropertyFlagBits::eHostCoherent :
                                            vk::MemoryPropertyFlagBits::eDeviceLocal );

    // bind the allocated memory to the image
    mGraphics.BindImageMemory( mImage, std::move(memory), 0 );

    // create default image view
    CreateImageView();

    // create basic sampler
    CreateSampler();
  }
  return true;
}

void Texture::CreateImageView()
{
  mImageView = mGraphics.CreateImageView(
    {}, mImage, vk::ImageViewType::e2D, mImage->GetFormat(), mComponentMapping,
    vk::ImageSubresourceRange{}
      .setAspectMask( mImage->GetAspectFlags() )
      .setBaseArrayLayer( 0 )
      .setBaseMipLevel( 0 )
      .setLevelCount( mImage->GetMipLevelCount() )
      .setLayerCount( mImage->GetLayerCount() )
  );
}

void Texture::CreateSampler()
{
  auto samplerCreateInfo = vk::SamplerCreateInfo()
    .setAddressModeU( vk::SamplerAddressMode::eClampToEdge )
    .setAddressModeV( vk::SamplerAddressMode::eClampToEdge )
    .setAddressModeW( vk::SamplerAddressMode::eClampToEdge )
    .setBorderColor( vk::BorderColor::eFloatOpaqueBlack )
    .setCompareOp( vk::CompareOp::eNever )
    .setMinFilter( vk::Filter::eLinear )
    .setMagFilter( vk::Filter::eLinear )
    .setMipmapMode( vk::SamplerMipmapMode::eLinear )
    .setMaxAnisotropy( 1.0f ); // must be 1.0f when anisotropy feature isn't enabled

  mSampler = mGraphics.CreateSampler( samplerCreateInfo );
}

Vulkan::RefCountedImage Texture::GetImageRef() const
{
  return mImage;
}

Vulkan::RefCountedImageView Texture::GetImageViewRef() const
{
  return mImageView;
}

Vulkan::RefCountedSampler Texture::GetSamplerRef() const
{
  return mSampler;
}

API::MemoryRequirements Texture::GetMemoryRequirements() const
{
  auto requirements = mGraphics.GetDevice().getImageMemoryRequirements( mImage->GetVkHandle() );
  API::MemoryRequirements retval{};
  retval.alignment = size_t(requirements.alignment);
  retval.size = size_t(requirements.size);
  return retval;
}

const API::TextureProperties& Texture::GetProperties()
{
  if( !mProperties )
  {
    mProperties = std::move(std::make_unique<API::TextureProperties>());

    auto formatInfo = GetFormatInfo( mFormat );
    mProperties->compressed = formatInfo.compressed;
    mProperties->packed = formatInfo.packed;
    mProperties->emulated = mConvertFromFormat != vk::Format::eUndefined;
    mProperties->format = mTextureFactory->GetFormat();
    mProperties->format1 = mTextureFactory->GetFormat();
    mProperties->extent2D = { mWidth, mHeight };
  }
  return *mProperties;
}

} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali

