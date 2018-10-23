//
// Created by adam.b on 19/10/18.
//

#ifndef DALI_GRAPHICS_VULKAN_API_DESCRIPTORSETALLOCATOR
#define DALI_GRAPHICS_VULKAN_API_DESCRIPTORSETALLOCATOR

#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/api/internal/vulkan-api-descriptor-set-requirements.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class Graphics;
}

namespace VulkanAPI
{
class Controller;

namespace Internal
{
using DescriptorSetAllocationFlags = uint32_t;

class DescriptorSetAllocator
{
public:
  DescriptorSetAllocator( VulkanAPI::Controller& controller );
  ~DescriptorSetAllocator();

public:

  void UpdateWithRequirements( std::vector<DescriptorSetRequirements>& requirements, DescriptorSetAllocationFlags flags );

  /**
   * Reset all pool indices
   */
  void Reset();

  bool AllocateDescriptorSets( const std::vector<Dali::Graphics::Vulkan::DescriptorSetLayoutSignature>& signatures,
                               const std::vector<vk::DescriptorSetLayout>& layouts,
                               DescriptorSetList& descriptorSets );

  /**
   * This function takes r-value only as parameter invalidating input
   * @param descriptorSets
   */
  void FreeDescriptorSets( std::vector<DescriptorSetList>&& descriptorSets );

  /**
   * Tests whether descriptorset list is still valid
   * @param descriptorSetList
   * @return
   */
  bool IsDescriptorSetListValid( const DescriptorSetList& descriptorSetList ) const;

  void SwapBuffers();

private:

  void ResolveFreeDescriptorSets();

  VulkanAPI::Controller& mController;

  /**
   *
   */
  struct Pool
  {
    Pool( Vulkan::DescriptorSetLayoutSignature newSignature )
      : signature( newSignature ){}
    Vulkan::DescriptorSetLayoutSignature                 signature {};
    vk::DescriptorPool                                   vkPool { nullptr};

    // Current capacity of the pool
    uint32_t                                             maxSets { 0 };
    DescriptorSetRequirements                            maxDescriptorCount {};

    // Requested capacity of pool, used when 'shouldReallocate' is True
    uint32_t                                             requiredSets { 0 };
    DescriptorSetRequirements                            requiredDescriptorCount {};

    // Preallocated descriptor sets
    std::vector<vk::DescriptorSet>                       vkDescriptorSets{ };

    uint32_t                                             available { 0u };
    bool                                                 shouldReallocate { true };
    bool                                                 shouldDiscard { false };

    // the layout used in the allocation
    vk::DescriptorSetLayout                              vkLayout;

    std::vector<vk::DescriptorSet>                       vkDescriptorSetsToBeFreed {};

    bool                                                 dirty { false };
  };

  using PoolSet = std::vector<Pool>;

  int32_t GetPoolIndexBySignature( Vulkan::DescriptorSetLayoutSignature signature, uint32_t bufferIndex );

  PoolSet mPoolSet[2u];
  uint32_t mBufferIndex { 0u };
};

} // Namespace Internal

} // Namespace VulkanAPI
} // Namespace Graphics
} // Namespace Dali

#endif //DALI_VULKAN_161117_VULKAN_API_DESCRIPTOR_SET_ALLOCATOR_H
