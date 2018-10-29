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
   * Swaps internal buffers
   */
  void SwapBuffers();

  /**
   *
   * @param list
   */
  bool TestIfValid( const DescriptorSetList& list, std::vector<bool>& outValue ) const;

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

    std::vector<vk::DescriptorSet>                       vkDescriptorSetsToBeFreed {};

    bool                                                 dirty { false };
  };

  using PoolSet = std::vector<Pool>;

  int32_t GetPoolIndexBySignature( Vulkan::DescriptorSetLayoutSignature signature, uint32_t bufferIndex ) const;

  std::array<PoolSet, 2u> mPoolSet;
  uint32_t mBufferIndex { 0u };
};

} // Namespace Internal

} // Namespace VulkanAPI
} // Namespace Graphics
} // Namespace Dali

#endif // DALI_GRAPHICS_VULKAN_API_DESCRIPTORSETALLOCATOR
