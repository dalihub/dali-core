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
#ifdef NATIVE_IMAGE_SUPPORT
#include <dlfcn.h>
#include <tbm_surface.h>
#include <vulkan/vulkan.h>
#include <vulkan/vk_tizen.h>
#endif
#include <dali/graphics/vulkan/api/vulkan-api-texture.h>

// INTERNAL INCLUDES
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/internal/vulkan-gpu-memory-allocator.h>
#include <dali/graphics/vulkan/internal/vulkan-gpu-memory-manager.h>
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
#if 0
typedef VkResult (VKAPI_PTR *PFN_vkCreateImageFromNativeBufferTIZEN)(
  VkDevice                     device,
  tbm_surface_h                surface,
  const VkImageCreateInfo     *pCreateInfo,
  const VkAllocationCallbacks *pAllocator,
  VkImage                     *pImage );
#endif
PFN_vkCreateImageFromNativeBufferTIZEN  gCreatePresentableImageProcedure = 0;
//PFN_vkGetInstanceProcAddr               getProcAddr = 0;
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

Texture::~Texture() = default;

bool Texture::Initialise()
{
  auto size = mTextureFactory.GetSize();
  mWidth = uint32_t( size.width );
  mHeight = uint32_t( size.height );
  auto sizeInBytes = mTextureFactory.GetDataSize();
  auto data = mTextureFactory.GetData();

  NativeImageInterfacePtr nativeImage = mTextureFactory.GetNativeImage();

  switch( mTextureFactory.GetUsage())
  {
    case API::TextureDetails::Usage::COLOR_ATTACHMENT:
    {
      mUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled;
      mLayout = vk::ImageLayout::eUndefined;
      break;
    }
    case API::TextureDetails::Usage::DEPTH_ATTACHMENT:
    {
      mUsage = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled;
      mLayout = vk::ImageLayout::eUndefined;
      break;
    }
    case API::TextureDetails::Usage::SAMPLE:
    {
      mUsage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst;
      mLayout = vk::ImageLayout::ePreinitialized;
      break;
    }
  }

  mFormat = ConvertApiToVk( mTextureFactory.GetFormat() );
  mComponentMapping = GetVkComponentMapping( mTextureFactory.GetFormat() );

  if(mTextureFactory.GetFormat() == API::Format::R8G8B8_UNORM )
  {
    if( data && sizeInBytes > 0 )
    {
      assert( (sizeInBytes == mWidth*mHeight*3) && "Corrupted RGB image data!" );

      auto inData = reinterpret_cast<const uint8_t*>(data);
      auto outData = new uint8_t[mWidth * mHeight * 4];

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
      mFormat = vk::Format::eR8G8B8A8Unorm;
    }
  }


  if (nativeImage)
  {
    mTbmSurface = nativeImage->GetNativeImageSource();
    InitialiseNativeTexture();
  }
  else
  {
    InitialiseTexture();
     if( data )
     {
        CopyMemory(data, {mWidth, mHeight}, {0, 0}, 0, 0, API::TextureDetails::UpdateMode::UNDEFINED );
     }
  }

  return true;
}


void Texture::CopyMemory(
  const void                      *srcMemory,
  API::Extent2D                    srcExtent,
  API::Offset2D                    dstOffset,
  uint32_t                         layer,
  uint32_t                         level,
  API::TextureDetails::UpdateMode  updateMode )
{
  // @todo transient buffer memory could be persistently mapped and aliased ( work like a per-frame stack )
  uint32_t allocationSize = srcExtent.width * srcExtent.height * (Vulkan::GetFormatInfo(mFormat).blockSizeInBits / 8 );

  // allocate transient buffer
  auto buffer = mGraphics.CreateBuffer( vk::BufferCreateInfo{}
                            .setSize( allocationSize )
                            .setSharingMode( vk::SharingMode::eExclusive )
                            .setUsage( vk::BufferUsageFlagBits::eTransferSrc));

  // bind memory
  mGraphics.BindBufferMemory( buffer,
                              mGraphics.AllocateMemory( buffer,
                                                        ( vk::MemoryPropertyFlagBits::eHostVisible |
                                                          vk::MemoryPropertyFlagBits::eHostCoherent ) ),
                              0u );

  // write into the buffer
  auto ptr = buffer->GetMemoryHandle()->MapTyped<char>();
  std::copy( reinterpret_cast<const char*>(srcMemory), reinterpret_cast<const char*>(srcMemory)+allocationSize, ptr );
  buffer->GetMemoryHandle()->Unmap();

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

// creates image with pre-allocated memory and default sampler, no data
// uploaded at this point
void Texture::InitialiseTexture()
{
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
  mGraphics.BindImageMemory( mImage, memory, 0 );

  // create default image view
  CreateImageView();

  // create basic sampler
  CreateSampler();
}

void Texture::InitialiseNativeTexture()
{
#ifdef NATIVE_IMAGE_SUPPORT
  tbm_surface_h tbmSurface = 0;
  VkImage image = 0;

  char *icdname = NULL;
  void *lib = NULL;
#if 0
  if( ! gCreatePresentableImageProcedure )
  {
    gCreatePresentableImageProcedure = reinterpret_cast<PFN_vkCreateImageFromNativeBufferTIZEN>(
      mGraphics.GetProcedureAddress( "vkCreateImageFromNativeBufferTIZEN" ) );
  }
#endif
  if( ! gCreatePresentableImageProcedure )
  {
      icdname = getenv("VK_TIZEN_ICD");
      lib = dlopen(icdname, RTLD_LAZY | RTLD_LOCAL);

      if (lib)
      {
#if 0
          getProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(lib,"vk_icdGetInstanceProcAddr"));
          if (getProcAddr)
          {
             gCreatePresentableImageProcedure = reinterpret_cast<PFN_vkCreateImageFromNativeBufferTIZEN>(
                getProcAddr(NULL, "vkCreateImageFromNativeBufferTIZEN"));
          }
#endif
          if ( ! gCreatePresentableImageProcedure )
          {
             gCreatePresentableImageProcedure = reinterpret_cast<PFN_vkCreateImageFromNativeBufferTIZEN>(
                dlsym(lib,"vkCreateImageFromNativeBufferTIZEN"));
          }
      }
  }

  if( gCreatePresentableImageProcedure )
  {
    // Enforce format:
    mFormat = vk::Format::eR8G8B8A8Unorm;  // VK_FORMAT_R8G8B8A8_UNORM;

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

    if( mTbmSurface.GetType() == typeid( tbm_surface_h ) )
    {
      tbmSurface =  AnyCast< tbm_surface_h >( mTbmSurface );
    }
    if (tbmSurface)
    {
      // @todo consider adding new CreateTexture() API to NativeImage (like GlExtensionCreate(), but generic)
      // to perform this from within native image implementation.
        VkImageCreateInfo imageInfo {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        imageInfo.extent.width = mWidth;
        imageInfo.extent.height = mHeight;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

       gCreatePresentableImageProcedure( static_cast<VkDevice>(mGraphics.GetDevice()),
                                                  tbmSurface,
                                                  &imageInfo,
                                                  0,
                                                  &image );
    }

    vk::Extent2D extent(mWidth, mHeight);

    mImage = mGraphics.CreateImageFromExternal( static_cast<vk::Image>(image), imageCreateInfo, mFormat, extent );

    CreateImageView();
    CreateSampler();

    if (lib)
        dlclose(lib);
  }
#endif
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
    .setLayerCount( mImage->GetLayerCount() ) );
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
    .setMipmapMode( vk::SamplerMipmapMode::eLinear );

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

} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali
