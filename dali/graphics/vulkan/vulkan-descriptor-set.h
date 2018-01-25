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

class DescriptorPool
{
public:

  static std::unique_ptr<DescriptorPool> New( Graphics& graphics, const vk::DescriptorPoolCreateInfo& createInfo );

};

class DescriptorSet
{

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
