#ifndef DALI_GRAPHICS_VULKAN_SAMPLER_H
#define DALI_GRAPHICS_VULKAN_SAMPLER_H

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

// INTERNAL INCLUDES
#include <dali/graphics/vulkan/internal/vulkan-types.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

class Graphics;

class Sampler : public VkManaged
{
  friend class Graphics;

public:

  vk::SamplerCreateFlags GetCreateFlags() const;

  vk::Filter GetMinFilter() const;

  vk::Filter GetMagFilter() const;

  vk::SamplerMipmapMode GetMipMapMode() const;

  vk::SamplerAddressMode GetAddressModeU() const;

  vk::SamplerAddressMode GetAddressModeV() const;

  vk::SamplerAddressMode GetAddressModeW() const;

  float GetMipLodBias() const;

  vk::Bool32 AnisotropyEnabled() const;

  float GetMaxAnisotropy() const;

  vk::Bool32 CompareEnabled() const;

  vk::CompareOp GetCompareOperation() const;

  float GetMinLod() const;

  float GetMaxLod() const;

  vk::BorderColor GetBorderColor() const;

  vk::Bool32 UsesUnnormalizedCoordinates() const;

  /**
   * Returns VkSampler object
   * @return
   */
  vk::Sampler GetVkHandle() const;

  const Sampler& ConstRef();

  Sampler& Ref();

  bool OnDestroy() override;

private:

  explicit Sampler( Graphics& graphics, const vk::SamplerCreateInfo& createInfo );

  ~Sampler() override;

private:
  Graphics* mGraphics;
  vk::SamplerCreateInfo mCreateInfo;
  vk::Sampler mSampler;

};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
#endif //DALI_GRAPHICS_VULKAN_SAMPLER_H
