#ifndef DALI_GRAPHICS_VULKAN_API_DESCRIPTORSETREQUIREMENTS
#define DALI_GRAPHICS_VULKAN_API_DESCRIPTORSETREQUIREMENTS

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

#include <dali/graphics/vulkan/vulkan-graphics.h>

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{

/**
 * Describes requirements of descriptor set allocation
 */
using DescriptorTypeRequirements = std::array<uint32_t, static_cast<uint32_t>(Vulkan::DescriptorType::DESCRIPTOR_TYPE_COUNT)>;
struct DescriptorSetRequirements
{
  Vulkan::DescriptorSetLayoutSignature  layoutSignature {};
  DescriptorTypeRequirements            requirements {};
  std::vector<vk::DescriptorSet>        freeDescriptorSets{}; // list of descriptorsets to free
  uint32_t                              maxSets { 0u };

  uint32_t                              unallocatedSets { 0u }; // number of sets to be allocated

  bool                                  resultDirty { false }; // output flag, set when descriptorsets must be reallocated
};

namespace Internal
{
class DescriptorSetAllocator;
}

struct DescriptorSetList
{
public:

  friend class Internal::DescriptorSetAllocator;

  DescriptorSetList() = default;
  ~DescriptorSetList() = default;
  operator const vk::DescriptorSet*() const;
  operator const std::vector<vk::DescriptorSet>&() const;

  std::vector<vk::DescriptorSet> descriptorSets;

  // only moveable
  DescriptorSetList( const DescriptorSetList& ) = delete;
  DescriptorSetList& operator=( const DescriptorSetList& ) = delete;
  DescriptorSetList( DescriptorSetList&& ) = default;
  DescriptorSetList& operator=( DescriptorSetList&& ) = default;

private:

  // opaque book-keeping data used by the allocator only
  struct Reserved
  {
    Reserved() = default;
    ~Reserved() = default;
    vk::DescriptorPool                   pool {};
    uint32_t                             bufferIndex {};
    Vulkan::DescriptorSetLayoutSignature signature {};
  };

  std::unique_ptr<Reserved> reserved;
};

} // Namespace VulkanAPI
} // Namespace Graphics
} // Namespace Dali

#endif // DALI_GRAPHICS_VULKAN_API_DESCRIPTORSETREQUIREMENTS
