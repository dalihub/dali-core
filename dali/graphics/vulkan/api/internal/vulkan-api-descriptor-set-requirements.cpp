#include <dali/graphics/vulkan/api/internal/vulkan-api-descriptor-set-requirements.h>

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{

DescriptorSetList::operator const vk::DescriptorSet*() const
{
  return descriptorSets.data();
}

DescriptorSetList::operator const std::vector<vk::DescriptorSet>&() const
{
  return descriptorSets;
}

}
}
}
