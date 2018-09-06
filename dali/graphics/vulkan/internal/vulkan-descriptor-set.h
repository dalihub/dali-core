#ifndef DALI_GRAPHICS_VULKAN_DESCRIPTOR_SET
#define DALI_GRAPHICS_VULKAN_DESCRIPTOR_SET

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
class Graphics;

class Buffer;

class Image;

class DescriptorPool;

class DescriptorSet : public VkManaged
{
  friend class DescriptorPool;

public:

  ~DescriptorSet() override;

  /**
   * Returns VkDescriptorSet associated with this object
   * @return Descriptor set
   */
  vk::DescriptorSet GetVkDescriptorSet() const;

  void Flush();

  bool OnDestroy() override;

private:

  DescriptorSet( Graphics& graphics, DescriptorPool& pool, vk::DescriptorSet descriptorSet,
                 vk::DescriptorSetAllocateInfo allocateInfo );

  Graphics* mGraphics;
  DescriptorPool* mPool;
  vk::DescriptorSetAllocateInfo mAllocateInfo;
  vk::DescriptorSet mDescriptorSet;
  std::vector< vk::WriteDescriptorSet > mDescriptorWrites;
};

class DescriptorPool : public VkManaged
{
public:

  static RefCountedDescriptorPool New( Graphics& graphics, const vk::DescriptorPoolCreateInfo& createInfo );

  ~DescriptorPool() override;

  bool Initialise();

  vk::DescriptorPool GetVkHandle() const;

  std::vector< RefCountedDescriptorSet > AllocateDescriptorSets( vk::DescriptorSetAllocateInfo allocateInfo );

  void FreeDescriptorSets( const std::vector< vk::DescriptorSet >& descriptorSets );

  uint32_t GetAvailableAllocations() const;



  /**
   * Resets descriptor pool
   */
  void Reset();

  bool OnDestroy() override;

private:

  DescriptorPool( Graphics& graphics, const vk::DescriptorPoolCreateInfo& createInfo );

  Graphics* mGraphics;
  vk::DescriptorPoolCreateInfo mCreateInfo;

  vk::DescriptorPool mDescriptorPool;

  uint32_t mAvailableAllocations{ 0u };

  // cache
  std::vector< RefCountedDescriptorSet > mDescriptorSetCache;
};

#if 0
class DescriptorSetLayout
{
public:

  static std::unique_ptr<DescriptorSetLayout> New( Graphics& graphics, const vk::DescriptorSetLayoutCreateInfo& createInfo );

private:

  DescriptorSetLayout( Graphics& graphics, const vk::DescriptorSetLayoutCreateInfo& createInfo );

private:

  class Impl;
  std::unique_ptr<Impl> mImpl;
};
#endif
} // Namespace Vulkan

} // Namespace Graphics

} // Namespace Dali

#endif // DALI_GRAPHICS_VULKAN_DESCRIPTOR_SET
