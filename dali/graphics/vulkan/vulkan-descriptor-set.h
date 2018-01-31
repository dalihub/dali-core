#ifndef DALI_GRAPHICS_VULKAN_DESCRIPTOR_SET_H
#define DALI_GRAPHICS_VULKAN_DESCRIPTOR_SET_H

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

#include <dali/graphics/vulkan/vulkan-types.h>

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
   *
   * @param buffer
   * @param offset
   * @param size
   */
  void WriteUniformBuffer( uint32_t binding, Handle<Buffer> buffer, uint32_t offset, uint32_t size );

  /**
   *
   * @param buffer
   * @param offset
   * @param size
   */
  void WriteStorageBuffer( Handle<Buffer> buffer, uint32_t offset, uint32_t size );

  /**
   *
   */
  void WriteImage( Handle<Image> );

  /**
   *
   * @param writeDescriptorSet
   */
  void Write( vk::WriteDescriptorSet writeDescriptorSet );

  /**
   * Returns VkDescriptorSet associated with this object
   * @return Descriptor set
   */
  vk::DescriptorSet GetVkDescriptorSet() const;

private:

  DescriptorSet( Graphics& graphics, DescriptorPool& pool, vk::DescriptorSet descriptorSet, vk::DescriptorSetAllocateInfo allocateInfo );

  class Impl;
  std::unique_ptr<Impl> mImpl;
};

using DescriptorSetHandle = Handle<DescriptorSet>;

class DescriptorPool : public VkManaged
{
public:

  static Handle<DescriptorPool> New( Graphics& graphics, const vk::DescriptorPoolCreateInfo& createInfo );

  ~DescriptorPool() override;

  vk::DescriptorPool GetVkDescriptorPool() const;

  std::vector<DescriptorSetHandle> AllocateDescriptorSets( vk::DescriptorSetAllocateInfo allocateInfo );

private:

  DescriptorPool( Graphics& graphics, const vk::DescriptorPoolCreateInfo& createInfo );

  class Impl;
  std::unique_ptr<Impl> mImpl;
};






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

} // Namespace Vulkan

} // Namespace Graphics

} // Namespace Dali

#endif //DALI_GRAPHICS_VULKAN_DESCRIPTOR_SET_H
