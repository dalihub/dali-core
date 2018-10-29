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

#include <dali/graphics/vulkan/api/internal/vulkan-api-descriptor-set-allocator.h>
#include <dali/graphics/vulkan/api/vulkan-api-controller.h>
#include <dali/graphics/vulkan/internal/vulkan-debug-allocator.h>
#include <dali/graphics/vulkan/internal/vulkan-debug.h>

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{
namespace Internal
{

DescriptorSetAllocator::DescriptorSetAllocator( VulkanAPI::Controller& controller )
: mController( controller )
{
}

DescriptorSetAllocator::~DescriptorSetAllocator()
{
  // destroy all pools
  std::for_each( mPoolSet.begin(), mPoolSet.end(),[&]( auto& poolset )
  {
    std::for_each( poolset.begin(), poolset.end(), [&]( auto& pool )
    {
      if( pool.vkPool )
      {
        auto& graphics = mController.GetGraphics();

        // destroy each pool
        graphics.GetDevice().destroyDescriptorPool( pool.vkPool, &graphics.GetAllocator() );
      }
    });
  });
}

void DescriptorSetAllocator::ResolveFreeDescriptorSets()
{
  auto& graphics = mController.GetGraphics();

  for( auto& pool : mPoolSet[mBufferIndex] )
  {
    if( !pool.vkDescriptorSetsToBeFreed.empty() )
    {
      // free unused descriptor sets
      graphics.GetDevice().freeDescriptorSets( pool.vkPool, pool.vkDescriptorSetsToBeFreed );

      // Update tracked descriptor sets
      std::sort( pool.vkDescriptorSetsToBeFreed.begin(), pool.vkDescriptorSetsToBeFreed.end() );
      // track allocated descriptor sets ( should be used only for debugging )
      auto freeIt = pool.vkDescriptorSetsToBeFreed.begin();
      std::vector<vk::DescriptorSet> newList{};
      std::for_each( pool.vkDescriptorSets.begin(), pool.vkDescriptorSets.end(), [&]( auto& item )
      {
        if( item != *freeIt )
        {
          newList.emplace_back( item );
          ++freeIt;
        }
      });
      pool.vkDescriptorSets = std::move( newList );

      // update available number of descriptor
      pool.available += uint32_t( pool.vkDescriptorSetsToBeFreed.size() );

      // clear the free list
      pool.vkDescriptorSetsToBeFreed.clear();
    }
  }
}

bool DescriptorSetAllocator::TestIfValid( const DescriptorSetList& list, std::vector<bool>& results ) const
{
  auto retval = true;
  std::for_each( list.descriptorSets.begin(), list.descriptorSets.end(), [&]( auto& testDs )
  {
    int poolIndex = -1;
    if( (poolIndex = this->GetPoolIndexBySignature( list.reserved->signature, list.reserved->bufferIndex ) ) > -1 )
    {
      auto& pool = mPoolSet[list.reserved->bufferIndex][uint32_t(poolIndex)];
      auto it = std::find_if( pool.vkDescriptorSets.begin(), pool.vkDescriptorSets.end(), [&]( auto& ds )
      {
        return ds == testDs;
      });

      auto invalid = ( it == pool.vkDescriptorSets.end() );
      if( invalid )
      {
        retval = false;
      }
      results.emplace_back( !invalid );
    }
  });
  return retval;
}

void DescriptorSetAllocator::UpdateWithRequirements(
  std::vector<DescriptorSetRequirements>& requirements,
  DescriptorSetAllocationFlags flags )
{
  ResolveFreeDescriptorSets();

  auto& poolset = mPoolSet[mBufferIndex];

  // For each signature decide whether we should reallocate pool or not.
  // Newly created pool always reallocates
  // Allocation strategy assumes overallocation using threshold
  for( auto& requirement : requirements )
  {
    auto it = std::find_if( poolset.begin(), poolset.end(), [&]( auto& item )
    {
      return item.signature == requirement.layoutSignature;
    });

    // Create new pool with signature
    if( it == poolset.end() )
    {
      poolset.emplace_back( requirement.layoutSignature );
      it = poolset.end()-1;
      (*it).signature = requirement.layoutSignature;
    }

    auto& pool = *it;
    if( !requirement.unallocatedSets )
    {
      pool.shouldReallocate = false;
      requirement.resultDirty = false;
      continue;
    }

    pool.requiredDescriptorCount = requirement;
    pool.requiredSets = requirement.maxSets;

    if( requirement.unallocatedSets >= pool.available )
    {
      pool.requiredSets *= 1; // double
      pool.shouldReallocate = true;
    }
    else
    {
      pool.shouldReallocate = false;
    }

    requirement.resultDirty = pool.shouldReallocate;
  }

  for( auto& pool : poolset )
  {
    if( pool.shouldReallocate )
    {
      const std::array<vk::DescriptorType, uint32_t(Vulkan::DescriptorType::DESCRIPTOR_TYPE_COUNT)> VK_DESCRIPTOR_TYPE = {
        vk::DescriptorType::eStorageImage,
        vk::DescriptorType::eSampler,
        vk::DescriptorType::eSampledImage,
        vk::DescriptorType::eCombinedImageSampler,
        vk::DescriptorType::eUniformTexelBuffer,
        vk::DescriptorType::eStorageTexelBuffer,
        vk::DescriptorType::eUniformBuffer,
        vk::DescriptorType::eStorageBuffer,
        vk::DescriptorType::eUniformBufferDynamic,
        vk::DescriptorType::eStorageBufferDynamic,
        vk::DescriptorType::eInputAttachment
      };

      // prepare pool sizes for the new pool before replacing old pool
      std::vector<vk::DescriptorPoolSize> sizes;

      for( auto i = 0u; i < uint32_t( Vulkan::DescriptorType::DESCRIPTOR_TYPE_COUNT ); ++i )
      {
        if( pool.requiredDescriptorCount.requirements[i] )
        {
          sizes.emplace_back( VK_DESCRIPTOR_TYPE[i], pool.requiredDescriptorCount.requirements[i]);
        }
      }

      vk::DescriptorPoolCreateInfo createInfo;
      createInfo.setMaxSets( pool.requiredSets );
      createInfo.setPPoolSizes( sizes.data() );
      createInfo.setPoolSizeCount( uint32_t(sizes.size()) );
      createInfo.setFlags( vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet );

      auto& graphics = mController.GetGraphics();

      auto newPool = graphics.GetDevice().createDescriptorPool( createInfo, graphics.GetAllocator( "DescriptorPool" ) ).value;

      // Update current pool
      pool.maxDescriptorCount = std::move( pool.requiredDescriptorCount );
      pool.maxSets = pool.requiredSets;

      auto device = graphics.GetDevice();
      auto pGraphics = &graphics;
      auto oldPool = pool.vkPool;

      if( oldPool )
      {
        device.destroyDescriptorPool( oldPool, pGraphics->GetAllocator( "DESCRIPTORPOOL" ) );
      }

      // replace pool
      pool.vkPool = newPool;
      pool.dirty = true;
      pool.vkDescriptorSets = {};
      pool.available = pool.maxSets;
    }
  }

  // For all the signatures that "disappeared" during update clean up
  // and delete those pools
  // ...
}

void DescriptorSetAllocator::Reset()
{

}

bool DescriptorSetAllocator::AllocateDescriptorSets(
  const std::vector<Dali::Graphics::Vulkan::DescriptorSetLayoutSignature>& signatures,
  const std::vector<vk::DescriptorSetLayout>& layouts,
  DescriptorSetList& descriptorSets )
{
  auto& poolset = mPoolSet[mBufferIndex];

  auto& retval = descriptorSets.descriptorSets;
  for( auto& signature : signatures )
  {
    auto it = std::find_if( poolset.begin(), poolset.end(), [&]( auto& item ){
      return item.signature == signature;
    });

    // something went vewy wong
    if( it == poolset.end() )
    {
      continue;
    }

    if( !(*it).dirty && !descriptorSets.descriptorSets.empty() )
    {
      continue;
    }

    vk::DescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.setDescriptorSetCount( uint32_t( signatures.size() ) );
    allocateInfo.setDescriptorPool( (*it).vkPool );
    allocateInfo.setPSetLayouts( layouts.data() );

    auto& graphics = mController.GetGraphics();
    auto result = graphics.GetDevice().allocateDescriptorSets( allocateInfo );
    if( result.result != vk::Result::eSuccess )
    {
      // dammit! shouldn't happen :(
      return { false };
    }

    // reset book-keeping data
    (*it).available -= uint32_t( result.value.size() );
    descriptorSets.reserved.reset( new DescriptorSetList::Reserved() );
    descriptorSets.reserved->pool = (*it).vkPool;
    descriptorSets.reserved->bufferIndex = mBufferIndex;
    descriptorSets.reserved->signature = (*it).signature;

    // track descriptor sets
    if( result.value.size() > 1 )
    {
      (*it).vkDescriptorSets.insert( (*it).vkDescriptorSets.end(), result.value.begin(), result.value.end() );
    }
    else
    {
      (*it).vkDescriptorSets.emplace_back( result.value[0] );
    }
    retval = std::move( result.value );

    // DEBUG: for faster lookup
    std::sort( (*it).vkDescriptorSets.begin(), (*it).vkDescriptorSets.end() );

    // return descriptor sets
    return true;
  }

  return false;
}

int32_t DescriptorSetAllocator::GetPoolIndexBySignature( Vulkan::DescriptorSetLayoutSignature signature, uint32_t bufferIndex ) const
{
  int32_t retval = {0u};
  for( auto& item : mPoolSet[bufferIndex] )
  {
    if( item.signature == signature )
    {
      return retval;
    }
    ++retval;
  }
  return -1;
}


void DescriptorSetAllocator::FreeDescriptorSets( std::vector<DescriptorSetList>&& descriptorSets )
{
  // build free lists
  for( auto& list : descriptorSets )
  {
    int32_t poolIndex = -1;
    if( (poolIndex = GetPoolIndexBySignature( list.reserved->signature, list.reserved->bufferIndex )) >= 0 )
    {
      auto& pool = mPoolSet[list.reserved->bufferIndex][uint32_t( poolIndex )];

      // if pool is gone, all descriptors are already invalid, no need to free them
      if( pool.vkPool == list.reserved->pool )
      {
        // collect descriptor sets to free
        pool.vkDescriptorSetsToBeFreed.insert( pool.vkDescriptorSetsToBeFreed.end(), list.descriptorSets.begin(), list.descriptorSets.end() );
      }
    }
  }
}

void DescriptorSetAllocator::SwapBuffers()
{
  // clean pools dirty flags
  for( auto& pools : mPoolSet )
  {
    for( auto& pool : pools )
    {
      pool.dirty = false;
    }
  }
  mBufferIndex = (mBufferIndex+1)%2;
}





} // Namespace Internal

} // Namespace VulkanAPI
} // Namespace Graphics
} // Namespace Vulkan