//
// Created by adam.b on 23/10/18.
//

#ifndef DALI_VULKAN_161117_VULKAN_API_DESCRIPTOR_SET_REQUIREMENTS_H
#define DALI_VULKAN_161117_VULKAN_API_DESCRIPTOR_SET_REQUIREMENTS_H

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

  operator const vk::DescriptorSet*() const;
  operator const std::vector<vk::DescriptorSet>&() const;

  std::vector<vk::DescriptorSet> descriptorSets;

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

  Reserved reserved;
};

}
}
}


#endif //DALI_VULKAN_161117_VULKAN_API_DESCRIPTOR_SET_REQUIREMENTS_H
