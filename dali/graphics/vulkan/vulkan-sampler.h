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
#include <dali/graphics/vulkan/vulkan-types.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class Graphics;
class Sampler : public VkManaged
{
public:

  /**
   * Creates new mutable sampler
   * @param graphics
   * @return
   */
  static SamplerRef New( Graphics& graphics );

  /**
   * Creates new immutable sampler
   * @param graphics
   * @param createInfo
   * @return
   */
  static SamplerRef NewImmutable( Graphics& graphics, vk::SamplerCreateInfo createInfo );

  /**
   * Sets minification and magnifcation filters
   * @param minFilter
   * @param magFilter
   */
  void SetFilter( vk::Filter minFilter, vk::Filter magFilter );

  /**
   * Sets UVW address mode
   * @param addressModeU
   * @param addressModeV
   * @param addressModeW
   */
  void SetAddressMode( vk::SamplerAddressMode addressModeU,
                       vk::SamplerAddressMode addressModeV,
                       vk::SamplerAddressMode addressModeW );

  /**
   * Sets minimum and maximum LOD
   * @param minLod
   * @param maxLod
   */
  void SetLod( float minLod, float maxLod );

  /**
   * Enables anisotropy
   * @param enabled
   */
  void EnableAnisotropy( bool enabled );

  /**
   * Sets mipmap mode
   * @param mode
   */
  void SetMipmapMode( vk::SamplerMipmapMode mode );

  /**
   * Sets maximum anisotropy
   * @param maxAnisotropy
   */
  void SetMaxAnisotropy( float maxAnisotropy );

  /**
   * Turns on/off comparison with reference value when sampling
   * @param enabled
   */
  void EnableCompare( bool enabled );

  /**
   * Specifies predefined border color to use
   * @param color
   */
  void SetBorderColor( vk::BorderColor color );

  /**
   * Enables/disables use of unnormalized texture coordinates
   * @param enabled
   */
  void UseUnnormalizedCoordinates( bool enabled );

  /**
   * Specifies comparison function used when comparing with reference value
   * @param compareOp
   */
  void SetCompareOp( vk::CompareOp compareOp );

  /**
   * Returns VkSampler object
   * @return
   */
  vk::Sampler GetVkHandle() const;

private:

  explicit Sampler( Graphics& graphics, vk::SamplerCreateInfo createInfo, bool immutable );

  ~Sampler() override;

  struct Impl;
  std::unique_ptr<Impl> mImpl;

};
} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
#endif //DALI_GRAPHICS_VULKAN_SAMPLER_H
