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

#include <stdlib.h>
#include <string>
#include <cstdio>

#include <execinfo.h>
#include <cxxabi.h>

#include <cstring>

// CLASS HEADER
#ifdef NATIVE_IMAGE_SUPPORT
#include <dlfcn.h>
#include <tbm_surface.h>
#include <vulkan/vulkan.h>
#include <vulkan/vk_tizen.h>

#ifdef EXPORT_API
#undef EXPORT_API
#endif

#ifndef DRM_FORMAT_MOD_LINEAR
#define DRM_FORMAT_MOD_LINEAR 0
#endif


#define USING_CSTYLE 1
#endif
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

#include <dali/integration-api/debug.h>

#include <algorithm>

namespace Dali
{
namespace Graphics
{
using Vulkan::VkAssert;
namespace VulkanAPI
{
using namespace Dali::Graphics::Vulkan;

namespace
{
// @todo Move to a derived class as a member variable?
#ifdef NATIVE_IMAGE_SUPPORT
PFN_vkCreateSamplerYcbcrConversionKHR        gCreateSamplerYcbcrConversionKHR = 0;       // device
PFN_vkGetPhysicalDeviceFormatProperties2KHR  gGetPhysicalDeviceFormatProperties2KHR = 0; // instance
PFN_vkDestroySamplerYcbcrConversionKHR       gDestroySamplerYcbcrConversionKHR = 0;      // device
#endif

} // anonymous namespace


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
        vk::ComponentSwizzle::eG,
        vk::ComponentSwizzle::eB,
        vk::ComponentSwizzle::eA
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

Texture::Texture( Dali::Graphics::API::TextureFactory& factory )
  : mTextureFactory( dynamic_cast<VulkanAPI::TextureFactory&>( factory ) ),
    mController( mTextureFactory.GetController() ),
    mGraphics( mTextureFactory.GetGraphics() ),
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
}

Texture::~Texture()
{
  if ( mYcbcrConv )
  {
    gDestroySamplerYcbcrConversionKHR( static_cast<VkDevice>(mGraphics.GetDevice()), mYcbcrConv, nullptr);
  }
}

bool Texture::Initialise()
{
  auto size = mTextureFactory.GetSize();
  mWidth = uint32_t( size.width );
  mHeight = uint32_t( size.height );
  auto sizeInBytes = mTextureFactory.GetDataSize();
  auto data = mTextureFactory.GetData();
  mLayout = vk::ImageLayout::eUndefined;
  NativeImageInterfacePtr nativeImage = mTextureFactory.GetNativeImage();
  fprintf(stderr,"%s: Texture::Initialise() GetNativeImage()############################################\n",__FUNCTION__);

  if ( !gCreateSamplerYcbcrConversionKHR && !gGetPhysicalDeviceFormatProperties2KHR )
  {
    gCreateSamplerYcbcrConversionKHR = reinterpret_cast<PFN_vkCreateSamplerYcbcrConversionKHR>(
                mGraphics.GetDeviceProcedureAddress( "vkCreateSamplerYcbcrConversionKHR" ) );

    gGetPhysicalDeviceFormatProperties2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties2KHR>(
                mGraphics.GetInstanceProcedureAddress( "vkGetPhysicalDeviceFormatProperties2KHR" ) );

    gDestroySamplerYcbcrConversionKHR = reinterpret_cast<PFN_vkDestroySamplerYcbcrConversionKHR>(
                mGraphics.GetDeviceProcedureAddress( "vkDestroySamplerYcbcrConversionKHR" ) );
  }

  if ( gCreateSamplerYcbcrConversionKHR
       && gGetPhysicalDeviceFormatProperties2KHR
       && nativeImage )
  {
      mIsSupportNativeImage = true;
      //fprintf(stderr, "Support Native Image now\n");
  }
  else
  {
      //fprintf(stderr, "Not support Native Image\n");
  }

  switch( mTextureFactory.GetUsage())
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

  mFormat = ConvertApiToVk( mTextureFactory.GetFormat() );
  mComponentMapping = GetVkComponentMapping( mTextureFactory.GetFormat() );

  /**
   * Lots of Vulkan drivers DO NOT SUPPORT R8G8B8 format therefore we are forced
   * to implement a fallback as lots of textures comes as RGB ( ie. jpg ). In case
   * the valid format is supported we're going to use it as it is.
   */
  std::vector<uint8_t> rgbaBuffer{};
  if(mTextureFactory.GetFormat() == API::Format::R8G8B8_UNORM )
  {
    auto formatProperties = mGraphics.GetPhysicalDevice().getFormatProperties( mFormat );
    if( !formatProperties.optimalTilingFeatures )
    {
      if( data && sizeInBytes > 0 )
      {
        assert( (sizeInBytes == mWidth*mHeight*3) && "Corrupted RGB image data!" );

        auto inData = reinterpret_cast<const uint8_t*>(data);

        rgbaBuffer.reserve( mWidth * mHeight * 4 );
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

        data = outData;
        sizeInBytes = mWidth * mHeight * 4;
        mFormat = vk::Format::eR8G8B8A8Unorm;
      }
    }
  }

  bool result = false;
  if (mIsSupportNativeImage)
  {
    mUsage = vk::ImageUsageFlagBits::eSampled;
    mLayout = vk::ImageLayout::eUndefined;

    mComponentMapping.r = vk::ComponentSwizzle::eIdentity;
    mComponentMapping.g = vk::ComponentSwizzle::eIdentity;
    mComponentMapping.b = vk::ComponentSwizzle::eIdentity;
    mComponentMapping.a = vk::ComponentSwizzle::eIdentity;

    mNativeImage = nativeImage->GetNativeImageHandle();
    //fprintf(stderr,"%s:Texture::Initialise() GetNativeImageHandle()\n",__FUNCTION__);
    if (InitialiseNativeImage())
    {
      CopyNativeImage( API::TextureDetails::UpdateMode::UNDEFINED );
    }
    result = true;
  }
  else
  {
    if( InitialiseTexture() )
    {
        // copy data to the image
      if( data )
      {
        CopyMemory(data, sizeInBytes, {mWidth, mHeight}, {0, 0}, 0, 0, API::TextureDetails::UpdateMode::UNDEFINED );
      }
      result = true;
    }
  }

  return result;
}

void Texture::CopyMemory(const void *srcMemory, uint32_t srcMemorySize, API::Extent2D srcExtent, API::Offset2D dstOffset, uint32_t layer, uint32_t level, API::TextureDetails::UpdateMode updateMode )
{
  // @todo transient buffer memory could be persistently mapped and aliased ( work like a per-frame stack )
  uint32_t allocationSize = 0u;

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

void Texture::CopyTexture(const API::Texture &srcTexture, API::Rect2D srcRegion, API::Offset2D dstOffset, uint32_t layer, uint32_t level, API::TextureDetails::UpdateMode updateMode )
{
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

void Texture::CopyBuffer(const API::Buffer &srcBuffer, API::Extent2D srcExtent, API::Offset2D dstOffset, uint32_t layer, uint32_t level, API::TextureDetails::UpdateMode updateMode )
{
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
                 .setBufferImageHeight({ srcExtent.height });

  transferRequest.bufferToImageInfo.dstImage = mImage;
  transferRequest.bufferToImageInfo.srcBuffer = static_cast<const VulkanAPI::Buffer&>(srcBuffer).GetBufferRef();
  transferRequest.deferredTransferMode = !( updateMode == API::TextureDetails::UpdateMode::IMMEDIATE );

  // schedule transfer
  mController.ScheduleResourceTransfer( std::move(transferRequest) );
}

void Texture::CopyNativeImage( API::TextureDetails::UpdateMode updateMode )
{
  ResourceTransferRequest transferRequest( TransferRequestType::USE_TBM_SURFACE );

  transferRequest.useTBMInfo.srcImage = mImage;
  transferRequest.deferredTransferMode = !( updateMode == API::TextureDetails::UpdateMode::IMMEDIATE );

  // schedule transfer
  mController.ScheduleResourceTransfer( std::move(transferRequest) );
}

// creates image with pre-allocated memory and default sampler, no data
// uploaded at this point
bool Texture::InitialiseTexture()
{
  // Check whether format is supported  by the platform
  auto properties = mGraphics.GetPhysicalDevice().getFormatProperties( mFormat );

  if( !properties.optimalTilingFeatures )
  {
    // terminate if not supported
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
    .setTiling( vk::ImageTiling::eOptimal )
    .setMipLevels( 1 );

  // Create the image handle
  mImage = mGraphics.CreateImage( imageCreateInfo );

  // allocate memory for the image
  auto memory = mGraphics.AllocateMemory( mImage, vk::MemoryPropertyFlagBits::eDeviceLocal );

  // bind the allocated memory to the image
  mGraphics.BindImageMemory( mImage, std::move(memory), 0 );

  // create default image view
  CreateImageView();

  // create basic sampler
  CreateSampler();

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
      .setLayerCount( mImage->GetLayerCount() ),
      nullptr
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

  if (mIsSupportNativeImage)
  {
    samplerCreateInfo.setBorderColor( vk::BorderColor::eFloatTransparentBlack );
    samplerCreateInfo.setMipmapMode( vk::SamplerMipmapMode::eNearest );
  }

  //auto vkSamplerCreateInfo = reinterpret_cast<const VkSamplerCreateInfo*>( &samplerCreateInfo );

  //fprintf(stderr,"%s:<<---------VkSamplerCreateInfo Information--------->>\n",__FUNCTION__);
  //fprintf(stderr,"%s:sType %d, pNext %p, flag %d\n",__FUNCTION__, vkSamplerCreateInfo->sType, vkSamplerCreateInfo->pNext, vkSamplerCreateInfo->flags);
  //fprintf(stderr,"%s:magFilter %d, minFilter %d, mipmapMode %d\n",__FUNCTION__, vkSamplerCreateInfo->magFilter, vkSamplerCreateInfo->minFilter, vkSamplerCreateInfo->mipmapMode);
  //fprintf(stderr,"%s:addressModeU %d, addressModeV %d, addressModeW %d\n",__FUNCTION__, vkSamplerCreateInfo->addressModeU, vkSamplerCreateInfo->addressModeV, vkSamplerCreateInfo->addressModeW);

  //fprintf(stderr,"%s:mipLodBias %lf\n",__FUNCTION__, static_cast<double>(vkSamplerCreateInfo->mipLodBias));
  //fprintf(stderr,"%s:anisotropyEnable %d\n",__FUNCTION__, vkSamplerCreateInfo->anisotropyEnable);
  //fprintf(stderr,"%s:maxAnisotropy %lf\n",__FUNCTION__, static_cast<double>(vkSamplerCreateInfo->maxAnisotropy));


  //fprintf(stderr,"%s:addressModeU %d, addressModeV %d, addressModeW %d\n",__FUNCTION__, vkSamplerCreateInfo->addressModeU, vkSamplerCreateInfo->addressModeV, vkSamplerCreateInfo->addressModeW);
  //fprintf(stderr,"%s:compareEnable %d, compareOp %d\n",__FUNCTION__, vkSamplerCreateInfo->compareEnable, vkSamplerCreateInfo->compareOp);
  //fprintf(stderr,"%s:minLod %lf, maxLod %lf\n",__FUNCTION__, static_cast<double>(vkSamplerCreateInfo->minLod), static_cast<double>(vkSamplerCreateInfo->maxLod));

  //fprintf(stderr,"%s:borderColor %d, unnormalizedCoordinates %d\n" ,__FUNCTION__, vkSamplerCreateInfo->borderColor, vkSamplerCreateInfo->unnormalizedCoordinates);
  //fprintf(stderr,"%s:<<------------------------------------------>>\n",__FUNCTION__);

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

bool Texture::InitialiseNativeImage()
{
  //fprintf(stderr,"%s:<<<<<================== InitialiseNativeImage ==================>>>>>\n",__FUNCTION__);
#ifdef NATIVE_IMAGE_SUPPORT
  tbm_surface_h tbmSurface = 0;
  tbm_surface_info_s tbmSurface_info;
  //fprintf(stderr,"%s:Create VkImage===========================================================\n",__FUNCTION__);
  VkImage image = 0;

  std::vector <VkSubresourceLayout> plane_layout;
  VkDrmFormatModifierPropertiesEXT  drm_fmt_modifier;

  if ( mNativeImage.GetType() == typeid( tbm_surface_h ) )
  {
    tbmSurface =  AnyCast< tbm_surface_h >( mNativeImage );
  }

  if (!tbmSurface)
  {
    //fprintf(stderr,"%s:tbmSurface is nullptr \n",__FUNCTION__);
    return false;
  }

  tbm_surface_get_info(tbmSurface, &tbmSurface_info);

  //fprintf(stderr,"%s:tbmSurface is %p\n",__FUNCTION__, static_cast<void*>(tbmSurface));
  //fprintf(stderr,"%s:<<---------tbmSurface's information--------->>\n",__FUNCTION__);;

  //fprintf(stderr,"%s:width %d, height %d, size %d\n",__FUNCTION__,tbmSurface_info.width, tbmSurface_info.height, tbmSurface_info.size);
  //fprintf(stderr,"%s:format %d, bpp %d, num_planes %d\n",__FUNCTION__,tbmSurface_info.format, tbmSurface_info.bpp, tbmSurface_info.num_planes);
  for (uint32_t i = 0; i < tbmSurface_info.num_planes; i++)
  {
      //fprintf(stderr,"%s:index %d\n",__FUNCTION__,i);
      //fprintf(stderr,"%s:offset %d, size %d\n",__FUNCTION__, tbmSurface_info.planes[i].offset, tbmSurface_info.planes[i].size);
      //fprintf(stderr,"%s:stride %d, pointer %p\n",__FUNCTION__, tbmSurface_info.planes[i].stride, tbmSurface_info.planes[i].ptr);
  }
  //fprintf(stderr,"%s:<<------------------------------------------>>\n",__FUNCTION__);;

  // set format
  if ( tbmSurface_info.format == TBM_FORMAT_NV21
     || tbmSurface_info.format == TBM_FORMAT_NV12 )
  {
    //fprintf(stderr,"%s:tbmSurface's format is TBM_FORMAT_NV21\n",__FUNCTION__);;
    // VK_FORMAT_G8_B8R8_2PLANE_420_UNORM_KHR
    mFormat = vk::Format::eG8B8R82Plane420UnormKHR;
    //fprintf(stderr,"%s:update mFormat with vk::Format::eG8B8R82Plane420UnormKHR\n",__FUNCTION__);;
  }
  else if ( tbmSurface_info.format == TBM_FORMAT_RGB888
      || tbmSurface_info.format == TBM_FORMAT_XRGB8888
      || tbmSurface_info.format == TBM_FORMAT_RGBX8888
      || tbmSurface_info.format == TBM_FORMAT_ARGB8888
      || tbmSurface_info.format == TBM_FORMAT_RGBA8888)
  {
    //fprintf(stderr,"%s:tbmSurface's format is the other\n",__FUNCTION__);;
    // VK_FORMAT_R8G8B8A8_UNORM
    mFormat = vk::Format::eR8G8B8A8Unorm;
  }
  else if ( tbmSurface_info.format == TBM_FORMAT_BGR888
      || tbmSurface_info.format == TBM_FORMAT_XBGR8888
      || tbmSurface_info.format == TBM_FORMAT_BGRX8888
      || tbmSurface_info.format == TBM_FORMAT_ABGR8888
      || tbmSurface_info.format == TBM_FORMAT_BGRA8888)
  {
    // VK_FORMAT_B8G8R8A8_UNORM
    mFormat = vk::Format::eB8G8R8A8Unorm;
  }

  mLayout = vk::ImageLayout::eUndefined;

  GetFormatLinearDrmModifierNativeImage( static_cast<VkFormat>(mFormat), drm_fmt_modifier );

  plane_layout.resize (drm_fmt_modifier.drmFormatModifierPlaneCount);

  //fprintf(stderr,"%s:resize to plane_layout with %d\n",__FUNCTION__, drm_fmt_modifier.drmFormatModifierPlaneCount);

  //fprintf(stderr,"%s:---> update plane_layout's data\n",__FUNCTION__);
  for (uint32_t i = 0; i < tbmSurface_info.num_planes; i++)
  {
    //fprintf(stderr,"%s:index %d\n",__FUNCTION__,i);
    plane_layout[i].offset = tbmSurface_info.planes[i].offset;
    plane_layout[i].size = tbmSurface_info.planes[i].size;
    plane_layout[i].rowPitch = tbmSurface_info.planes[i].stride;
    plane_layout[i].arrayPitch = 0;
    plane_layout[i].depthPitch = 0;
    //fprintf(stderr,"%s:offset %lld, size %lld\n",__FUNCTION__,plane_layout[i].offset, plane_layout[i].size);
    //fprintf(stderr,"%s:rowPitch %lld, arrayPitch %lld, depthPitch %lld\n",__FUNCTION__,plane_layout[i].rowPitch, plane_layout[i].arrayPitch, plane_layout[i].depthPitch);
  }
  //fprintf(stderr,"%s:<---\n",__FUNCTION__);

  VkImageDrmFormatModifierExplicitCreateInfoEXT mod_create_info =
  {
    static_cast< VkStructureType >(VK_STRUCTURE_TYPE_IMAGE_EXCPLICIT_DRM_FORMAT_MODIFIER_CREATE_INFO_EXT), //VkStructureType sType;
    nullptr,                                                           //const void*                   pNext;
    DRM_FORMAT_MOD_LINEAR,                                             //uint64_t                      drmFormatModifier;
    drm_fmt_modifier.drmFormatModifierPlaneCount,                      //uint32_t                      drmFormatModifierPlaneCount;
    &plane_layout[0]                                                   //const VkSubresourceLayout*    pPlaneLayouts;
  };

  VkExternalMemoryImageCreateInfoKHR ext_mem_create_info =
  {
    static_cast< VkStructureType >(VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO_KHR), //VkStructureType  sType;
    &mod_create_info,                                                //const void*      pNext;
    VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT                   //VkExternalMemoryHandleTypeFlags    handleTypes;
  };

  auto imageCreateInfo = vk::ImageCreateInfo{}
    .setPNext( static_cast< void * >(&ext_mem_create_info) )
    .setImageType( vk::ImageType::e2D )
    .setFormat( mFormat )
    .setExtent( { mWidth, mHeight, 1 } )
    .setMipLevels( 1 )
    .setArrayLayers( 1 )
    .setSamples( vk::SampleCountFlagBits::e1 )
    .setTiling( static_cast< vk::ImageTiling >(VK_IMAGE_TILING_DRM_FORMAT_MODIFIER_EXT) )
    .setUsage( mUsage )
    .setSharingMode( vk::SharingMode::eExclusive )
    .setQueueFamilyIndexCount( 0 )
    .setPQueueFamilyIndices( nullptr )
    .setInitialLayout( mLayout );

  //auto vkImageCreateInfo = reinterpret_cast<const VkImageCreateInfo*>( &imageCreateInfo );

  //fprintf(stderr,"%s:<<---------VKImageCreate Information--------->>\n",__FUNCTION__);
  //fprintf(stderr,"%s:sType %d, pNext %p, format %d\n",__FUNCTION__, vkImageCreateInfo->sType, vkImageCreateInfo->pNext, vkImageCreateInfo->format);
  //fprintf(stderr,"%s:flags %d, mipLevels %d, arrayLayers %d\n",__FUNCTION__, vkImageCreateInfo->flags, vkImageCreateInfo->mipLevels, vkImageCreateInfo->arrayLayers);
  //fprintf(stderr,"%s:samples %d, tiling %d, usage %d\n",__FUNCTION__, vkImageCreateInfo->samples, vkImageCreateInfo->tiling, vkImageCreateInfo->usage);
  //fprintf(stderr,"%s:sharingMode %d, queueFamilyIndexCount %d\n",__FUNCTION__, vkImageCreateInfo->sharingMode, vkImageCreateInfo->queueFamilyIndexCount);
  //if (vkImageCreateInfo->pQueueFamilyIndices)
    //fprintf(stderr,"%s:pQueueFamilyIndices %d, initialLayout %d\n",__FUNCTION__, *(vkImageCreateInfo->pQueueFamilyIndices), vkImageCreateInfo->initialLayout);
  //else
    //fprintf(stderr,"%s:pQueueFamilyIndices NULL, initialLayout %d\n",__FUNCTION__, vkImageCreateInfo->initialLayout);
  //fprintf(stderr,"%s:width %d, height %d, depht %d\n",__FUNCTION__, vkImageCreateInfo->extent.width, vkImageCreateInfo->extent.height, vkImageCreateInfo->extent.depth);
  //fprintf(stderr,"%s:<<------------------------------------------>>\n",__FUNCTION__);

  //fprintf(stderr,"%s:create extend with w %d, h %d\n",__FUNCTION__, mWidth, mHeight);

  mImage = mGraphics.CreateImage( imageCreateInfo );

  //fprintf(stderr,"%s:AllocateMemory===========================================================\n",__FUNCTION__);
  // allocate memory for the image
  auto memory = mGraphics.AllocateMemory( mImage, vk::MemoryPropertyFlagBits::eHostVisible, mNativeImage );
  if (memory)
  {
    fprintf(stderr,"%s:success to AllocateMemory!!\n",__FUNCTION__);
  }
  else
  {
    fprintf(stderr,"%s:fail to AllocateMemory!!\n",__FUNCTION__);
    return false;
  }

  // bind the allocated memory to the image
  //fprintf(stderr,"%s:BindImageMemory===========================================================\n",__FUNCTION__);
  mGraphics.BindImageMemory( mImage, std::move(memory), 0 );

  if ( mFormat == vk::Format::eG8B8R82Plane420UnormKHR )
  {
    //fprintf(stderr,"%s:CreateSamplerYUVNativeImage===========================================================\n",__FUNCTION__);
    CreateSamplerYUVNativeImage();
    //fprintf(stderr,"%s:CreateImageViewYUVNativeImage===========================================================\n",__FUNCTION__);
    CreateImageViewYUVNativeImage();
  }
  else
  {
    //fprintf(stderr,"%s:CreateSampler===========================================================\n",__FUNCTION__);
    CreateSampler();
    //fprintf(stderr,"%s:CreateImageView===========================================================\n",__FUNCTION__);
    CreateImageView();
  }
#endif

  return true;
}

bool Texture::GetFormatLinearDrmModifierNativeImage( VkFormat format, VkDrmFormatModifierPropertiesEXT &outMode )
{
#ifdef NATIVE_IMAGE_SUPPORT
  //fprintf(stderr,"%s:VkFormat' %d\n",__FUNCTION__, format);

  std::vector<VkDrmFormatModifierPropertiesEXT> drm_format_modifiers;

  VkDrmFormatModifierPropertiesListEXT mod_props =
  {
    static_cast< VkStructureType >(VK_STRUCTURE_TYPE_DRM_FORMAT_MODIFIER_PROPERTIES_LIST_EXT),// VkStructureType sType;
    nullptr,        //  void*                              pNext;
    0,              //  uint32_t                           drmFormatModifierCount;
    nullptr         //  VkDrmFormatModifierPropertiesEXT*  pDrmFormatModifierProperties;
  };

  VkFormatProperties    formatProperties = {};
  VkFormatProperties2KHR format_props =
  {
    VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2_KHR, // VkStructureType       sType;
    &mod_props,                                // void*                 pNext;
    formatProperties                           // VkFormatProperties    formatProperties;
  };

  VkPhysicalDevice vkPhysicalDevice = static_cast<VkPhysicalDevice>(mGraphics.GetPhysicalDevice());
  //fprintf(stderr,"%s:call VkGetPhysicalDeviceFormatProperties2KHR with VkPhysicalDevice %p\n",__FUNCTION__, static_cast<void*>(vkPhysicalDevice));
  gGetPhysicalDeviceFormatProperties2KHR( vkPhysicalDevice, format, &format_props);
  //mGraphics.GetPhysicalDevice().getFormatProperties2KHR( mFormat, reinterpret_cast<vk::FormatProperties2KHR*>( &format_props ));

  //fprintf(stderr,"%s:[-----VkFormatProperties2KHR's information\n",__FUNCTION__);
  //fprintf(stderr,"%s:sType %d\n",__FUNCTION__, format_props.sType);
  //fprintf(stderr,"%s:formatProperties' linearTilingFeatures %d\n",__FUNCTION__, formatProperties.linearTilingFeatures);
  //fprintf(stderr,"%s:formatProperties' optimalTilingFeatures %d\n",__FUNCTION__, formatProperties.optimalTilingFeatures);
  //fprintf(stderr,"%s:formatProperties' bufferFeatures %d\n",__FUNCTION__, formatProperties.bufferFeatures);

  //fprintf(stderr,"%s:mod_props' sType %d\n",__FUNCTION__, mod_props.sType);
  //fprintf(stderr,"%s:mod_props' drmFormatModifierCount %d\n",__FUNCTION__, mod_props.drmFormatModifierCount);

  if (mod_props.drmFormatModifierCount <= 0)
  {
    std::cout << "Could get drmFormatModifierCount " << std::endl;
    return false;
  }

  drm_format_modifiers.resize( mod_props.drmFormatModifierCount);
  mod_props.pDrmFormatModifierProperties = &drm_format_modifiers[0];

  //fprintf(stderr,"%s:mod_props' pDrmFormatModifierProperties %p\n",__FUNCTION__, static_cast<void*>(mod_props.pDrmFormatModifierProperties));

  //fprintf(stderr,"%s:call VkGetPhysicalDeviceFormatProperties2KHR with VkPhysicalDevice %p\n",__FUNCTION__, static_cast<void*>(vkPhysicalDevice));
  gGetPhysicalDeviceFormatProperties2KHR( vkPhysicalDevice, format, &format_props);
  //mGraphics.GetPhysicalDevice().getFormatProperties2KHR( mFormat, reinterpret_cast<vk::FormatProperties2KHR*>( &format_props ));

  for( VkDrmFormatModifierPropertiesEXT &mode : drm_format_modifiers )
  {
    //fprintf(stderr,"%s:[-----VkDrmFormatModifierPropertiesEXT's information\n",__FUNCTION__);
    //fprintf(stderr,"%s:drmFormatModifier %lld\n",__FUNCTION__, mode.drmFormatModifier);
    //fprintf(stderr,"%s:drmFormatModifierPlaneCount %d\n",__FUNCTION__, mode.drmFormatModifierPlaneCount);
    //fprintf(stderr,"%s:drmFormatModifierTilingFeatures %d\n",__FUNCTION__, mode.drmFormatModifierTilingFeatures);


    if( mode.drmFormatModifier == DRM_FORMAT_MOD_LINEAR )
    {
      outMode = mode;
      //fprintf(stderr,"%s:find VkDrmFormatModifierPropertiesEXT -----]\n",__FUNCTION__);
      //fprintf(stderr,"%s:return TRUE-----]\n",__FUNCTION__);
      return true;
    }

    //fprintf(stderr,"%s:not find VkDrmFormatModifierPropertiesEXT -----]\n",__FUNCTION__);
  }

  //fprintf(stderr,"%s:return FALSE-----]\n",__FUNCTION__);
#endif
  return false;
}

void Texture::CreateImageViewYUVNativeImage()
{
  mImageView = mGraphics.CreateImageView(
    {}, mImage, vk::ImageViewType::e2D, mImage->GetFormat(), mComponentMapping,
    vk::ImageSubresourceRange{}
      .setAspectMask( mImage->GetAspectFlags() )
      .setBaseArrayLayer( 0 )
      .setBaseMipLevel( 0 )
      .setLevelCount( mImage->GetMipLevelCount() )
      .setLayerCount( mImage->GetLayerCount() ),
      static_cast<void*>(&mYcbcrConvInfo)
  );
  //fprintf(stderr,"%s:Create VkImageView %llud\n",__FUNCTION__, static_cast<VkImageView>(mImageView->GetVkHandle()));
}

bool Texture::CreateSamplerYUVNativeImage()
{
#ifdef NATIVE_IMAGE_SUPPORT
    VkSamplerYcbcrConversionCreateInfoKHR conv_create_info =
    {
      VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_CREATE_INFO_KHR, // VkStructureType                  sType;
      nullptr,                                                    //const void*                      pNext;
      static_cast<VkFormat>(mFormat),                             //VkFormat                         format;
      VK_SAMPLER_YCBCR_MODEL_CONVERSION_YCBCR_709_KHR,            //VkSamplerYcbcrModelConversion    ycbcrModel;
      VK_SAMPLER_YCBCR_RANGE_ITU_FULL_KHR,                        // VkSamplerYcbcrRange              ycbcrRange;
      {

          VK_COMPONENT_SWIZZLE_IDENTITY,                          //VkComponentSwizzle    r;
          VK_COMPONENT_SWIZZLE_IDENTITY,                          //VkComponentSwizzle    g;
          VK_COMPONENT_SWIZZLE_IDENTITY,                          //VkComponentSwizzle    b;
          VK_COMPONENT_SWIZZLE_IDENTITY,                          //VkComponentSwizzle    a;
      },                                                          // VkComponentMapping   components;
      VK_CHROMA_LOCATION_MIDPOINT_KHR,                            //VkChromaLocation      xChromaOffset;
      VK_CHROMA_LOCATION_MIDPOINT_KHR,                            //VkChromaLocation      yChromaOffset;
      VK_FILTER_NEAREST,                                          //VkFilter              chromaFilter;
      VK_FALSE                                                    //VkBool32              forceExplicitReconstruction;
    };

    //fprintf(stderr,"%s:<<---------VkSamplerYcbcrConversionCreateInfoKHR Information--------->>\n",__FUNCTION__);
    //fprintf(stderr,"%s:sType %d, pNext %p, format %d\n",__FUNCTION__, conv_create_info.sType, conv_create_info.pNext, conv_create_info.format);
    //fprintf(stderr,"%s:ycbcrModel %d, ycbcrRange %d\n",__FUNCTION__, conv_create_info.ycbcrModel, conv_create_info.ycbcrRange);
    //fprintf(stderr,"%s:components.r %d, g %d\n",__FUNCTION__, conv_create_info.components.r, conv_create_info.components.g);
    //fprintf(stderr,"%s:components.b %d, a %d\n",__FUNCTION__, conv_create_info.components.b, conv_create_info.components.a);
    //fprintf(stderr,"%s:xChromaOffset %d, yChromaOffset %d\n",__FUNCTION__, conv_create_info.xChromaOffset, conv_create_info.yChromaOffset);
    //fprintf(stderr,"%s:chromaFilter %d, forceExplicitReconstruction %d\n",__FUNCTION__, conv_create_info.chromaFilter, conv_create_info.forceExplicitReconstruction);
    //fprintf(stderr,"%s:<<------------------------------------------>>\n",__FUNCTION__);

   if (VK_SUCCESS != gCreateSamplerYcbcrConversionKHR ( static_cast<VkDevice>(mGraphics.GetDevice()), &conv_create_info, nullptr, &mYcbcrConv))
   {
     std::cout << "Could not create vkCreateSamplerYcbcrConversionKHR !" << std::endl;
     //fprintf(stderr,"%s:Could not create vkCreateSamplerYcbcrConversionKHR !\n",__FUNCTION__);
     return false;
   }

   mYcbcrConvInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_YCBCR_CONVERSION_INFO_KHR;
   mYcbcrConvInfo.pNext = nullptr;
   mYcbcrConvInfo.conversion = mYcbcrConv;

  auto samplerCreateInfo = vk::SamplerCreateInfo{}
    .setPNext(( static_cast< void * >(&mYcbcrConvInfo) ))
    .setMagFilter( vk::Filter::eNearest )
    .setMinFilter( vk::Filter::eNearest )
    .setMipmapMode( vk::SamplerMipmapMode::eNearest )
    .setAddressModeU( vk::SamplerAddressMode::eClampToEdge )
    .setAddressModeV( vk::SamplerAddressMode::eClampToEdge )
    .setAddressModeW( vk::SamplerAddressMode::eClampToEdge )
    .setMipLodBias( 0.0f )
    .setAnisotropyEnable( false )
    .setMaxAnisotropy( 1.0f )
    .setCompareEnable( false )
    .setCompareOp( vk::CompareOp::eLessOrEqual )
    .setMinLod( -1000.0f )
    .setMaxLod( 1000.0f )
    .setBorderColor( vk::BorderColor::eFloatTransparentBlack )
    .setUnnormalizedCoordinates( false);

  //auto sampler_create_info = reinterpret_cast<const VkSamplerCreateInfo*>( &samplerCreateInfo );

  //fprintf(stderr,"%s:<<---------VkSamplerCreateInfo Information--------->>\n",__FUNCTION__);
  //fprintf(stderr,"%s:sType %d, pNext %p, flag %d\n",__FUNCTION__, sampler_create_info->sType, sampler_create_info->pNext, sampler_create_info->flags);
  //fprintf(stderr,"%s:magFilter %d, minFilter %d, mipmapMode %d\n",__FUNCTION__, sampler_create_info->magFilter, sampler_create_info->minFilter, sampler_create_info->mipmapMode);
  //fprintf(stderr,"%s:addressModeU %d, addressModeV %d, addressModeW %d\n",__FUNCTION__, sampler_create_info->addressModeU, sampler_create_info->addressModeV, sampler_create_info->addressModeW);

  //fprintf(stderr,"%s:mipLodBias %lf\n",__FUNCTION__, static_cast<double>(sampler_create_info->mipLodBias));
  //fprintf(stderr,"%s:anisotropyEnable %d\n",__FUNCTION__, sampler_create_info->anisotropyEnable);
  //fprintf(stderr,"%s:maxAnisotropy %lf\n",__FUNCTION__, static_cast<double>(sampler_create_info->maxAnisotropy));


  //fprintf(stderr,"%s:addressModeU %d, addressModeV %d, addressModeW %d\n",__FUNCTION__, sampler_create_info->addressModeU, sampler_create_info->addressModeV, sampler_create_info->addressModeW);
  //fprintf(stderr,"%s:compareEnable %d, compareOp %d\n",__FUNCTION__, sampler_create_info->compareEnable, sampler_create_info->compareOp);
  //fprintf(stderr,"%s:minLod %lf, maxLod %lf\n",__FUNCTION__, static_cast<double>(sampler_create_info->minLod), static_cast<double>(sampler_create_info->maxLod));

  //fprintf(stderr,"%s:borderColor %d, unnormalizedCoordinates %d\n",__FUNCTION__, sampler_create_info->borderColor, sampler_create_info->unnormalizedCoordinates);
  //fprintf(stderr,"%s:<<------------------------------------------>>\n",__FUNCTION__);

  mSampler = mGraphics.CreateSampler( samplerCreateInfo );
  //fprintf(stderr,"%s:Create VkSampler %llud\n",__FUNCTION__, static_cast<VkSampler>(mSampler->GetVkHandle()));
#endif
  return true;
}



} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali
