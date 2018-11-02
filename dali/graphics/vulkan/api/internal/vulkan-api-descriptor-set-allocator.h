#ifndef DALI_GRAPHICS_VULKAN_API_DESCRIPTORSETALLOCATOR
#define DALI_GRAPHICS_VULKAN_API_DESCRIPTORSETALLOCATOR

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
  /**
   * Class constructor
   *
   * @param[in] controller reference to the Controller object
   */
  DescriptorSetAllocator( VulkanAPI::Controller& controller );

  /**
   * Class destructor
   */
  ~DescriptorSetAllocator();

public:

  /**
   * Updates allocator with given allocation requirements. This function should run
   * once per frame.

   * @param[in,out] Requirements List of requirements
   * @param[in] flags Allocation flags, currently not used
   */
  void UpdateWithRequirements( std::vector<DescriptorSetRequirements>& requirements, DescriptorSetAllocationFlags flags );

  /**
   * Allocates descriptor sets for specified layouts and signatures
   *
   * @param[in] signatures List of signatures
   * @param[in] layouts List of layouts
   * @param[out] descriptorSets Returned descriptor sets
   * @return true on success
   */
  bool AllocateDescriptorSets( const std::vector<Dali::Graphics::Vulkan::DescriptorSetLayoutSignature>& signatures,
                               const std::vector<vk::DescriptorSetLayout>& layouts,
                               DescriptorSetList& descriptorSets );

  /**
   * This function takes r-value only as parameter invalidating input
   * @param descriptorSets
   */
  void FreeDescriptorSets( std::vector<DescriptorSetList>&& descriptorSets );

  /**
   * Swaps internal buffers
   */
  void SwapBuffers();

  /**
   * Tests whether DescriptorSetList object is still valid ( for example, the pool
   * it was allocated from still exists ).
   *
   * @param[in] list List containing descriptor sets
   * @param[out] results Output results ( one per each descriptor set )
   * @return True if all valid, false otherwise
   */
  bool TestIfValid( const DescriptorSetList& list, std::vector<bool>& results ) const;

private:

  /**
   * Resolves which descriptor sets should be freed
   */
  void ResolveFreeDescriptorSets();

  /**
   * Helper function returning index of Pool within Poolset for given signature and buffer index
   * @param[in] signature Layout signature
   * @param[in] bufferIndex update buffer index
   * @return Valid index or -1 if failed
   */
  int32_t GetPoolIndexBySignature( Vulkan::DescriptorSetLayoutSignature signature, uint32_t bufferIndex ) const;

  /**
   * Structure wrapping single descriptor pool
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

    std::vector<vk::DescriptorSet>                       vkDescriptorSetsToBeFreed {};

    bool                                                 dirty { false };
  };

  VulkanAPI::Controller& mController;

  using PoolSet = std::vector<Pool>;
  std::array<PoolSet, 2u> mPoolSet;

  uint32_t mBufferIndex { 0u };
};

} // Namespace Internal

} // Namespace VulkanAPI
} // Namespace Graphics
} // Namespace Dali

#endif // DALI_GRAPHICS_VULKAN_API_DESCRIPTORSETALLOCATOR
