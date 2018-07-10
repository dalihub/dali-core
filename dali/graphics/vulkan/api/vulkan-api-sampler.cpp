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

#include <dali/graphics/vulkan/api/vulkan-api-sampler.h>
#include <dali/graphics/vulkan/api/vulkan-api-sampler-factory.h>
#include <dali/graphics/vulkan/api/vulkan-api-controller.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{
namespace
{
constexpr vk::Filter ConvertFilter( API::SamplerFilter filter )
{
  switch(filter)
  {
    case API::SamplerFilter::LINEAR: return vk::Filter::eLinear;
    case API::SamplerFilter::NEAREST: return vk::Filter::eNearest;
  }
  return vk::Filter{};
}

constexpr vk::SamplerAddressMode ConvertAddressMode( API::SamplerAddressMode mode )
{
  switch(mode)
  {
    case API::SamplerAddressMode::CLAMP_TO_EDGE: return vk::SamplerAddressMode::eClampToEdge;
    case API::SamplerAddressMode::CLAMP_TO_BORDER: return vk::SamplerAddressMode::eClampToBorder;
    case API::SamplerAddressMode::MIRROR_CLAMP_TO_EDGE: return vk::SamplerAddressMode::eMirrorClampToEdge;
    case API::SamplerAddressMode::MIRRORED_REPEAT: return vk::SamplerAddressMode::eMirroredRepeat;
    case API::SamplerAddressMode::REPEAT: return vk::SamplerAddressMode::eRepeat;
  }
  return vk::SamplerAddressMode{};
}

constexpr vk::SamplerMipmapMode ConvertMipmapMode( API::SamplerMipmapMode mode )
{
  switch(mode)
  {
    case API::SamplerMipmapMode::LINEAR: return vk::SamplerMipmapMode::eLinear;
    case API::SamplerMipmapMode::NEAREST: return vk::SamplerMipmapMode::eNearest;
  }
  return vk::SamplerMipmapMode{};
}

}

Sampler::Sampler( Controller& controller, const SamplerFactory& factory )
: mController( controller )
{
  mSampler = controller.GetGraphics().CreateSampler(
                         vk::SamplerCreateInfo()
                           .setMinFilter( ConvertFilter( factory.mMinFilter ) )
                           .setMagFilter( ConvertFilter( factory.mMagFilter ) )
                           .setAddressModeU( ConvertAddressMode( factory.mAddressModeU ) )
                           .setAddressModeV( ConvertAddressMode( factory.mAddressModeV ) )
                           .setAddressModeW( ConvertAddressMode( factory.mAddressModeW ) )
                           .setMipmapMode( ConvertMipmapMode( factory.mMipmapMode ) )
                           .setCompareEnable( vk::Bool32(factory.mCompareEnable) )
                           .setUnnormalizedCoordinates( vk::Bool32(factory.mUnnormalizedCoordinates) )
                           .setBorderColor( vk::BorderColor::eFloatOpaqueBlack )
                           .setAnisotropyEnable( vk::Bool32(factory.mAnisotropyEnable) )
                           .setMaxAnisotropy( factory.mMaxAnisotropy )
                           .setMinLod( factory.mMinLod )
                           .setMaxLod( factory.mMaxLod )
                       );
}

} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali
