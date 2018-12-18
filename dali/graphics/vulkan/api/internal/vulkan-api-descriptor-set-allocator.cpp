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
static const uint32_t DESCRIPTOR_SET_MAX_MULTIPLIER ( 3u );

DescriptorSetAllocator::DescriptorSetAllocator( VulkanAPI::Controller& controller )
: mController( controller )
{
}

DescriptorSetAllocator::~DescriptorSetAllocator()
{
  // destroy all pools
  std::for_each( mPoolSet.begin(), mPoolSet.end(),[&]( auto& poolset )
  {
    std::for_each( poolset.begin(), poolset.end(), [&]( Pool& pool )
    {
      if( pool.vkPool )
      {
        auto& graphics = mController.GetGraphics();

        graphics.GetDevice().freeDescriptorSets( pool.vkPool, pool.vkDescriptorSets );

        // destroy each pool
        graphics.GetDevice().destroyDescriptorPool( pool.vkPool, &graphics.GetAllocator() );
      }
    });
  });
}

void DescriptorSetAllocator::ResolveFreeDescriptorSets()
{
  auto& graphics = mController.GetGraphics();

  const auto bufferIndex = mController.GetCurrentBufferIndex();
  for( auto& pool : mPoolSet[bufferIndex] )
  {
    if( !pool.vkDescriptorSetsToBeFreed.empty() )
    {
      // free unused descriptor sets
      std::vector<vk::DescriptorSet> existingDSToFree;
      auto freeIt = pool.vkDescriptorSetsToBeFreed.begin();
      std::vector<vk::DescriptorSet> newList{};
      std::for_each( pool.vkDescriptorSets.begin(), pool.vkDescriptorSets.end(), [&]( auto& item )
      {
        if( static_cast<VkDescriptorSet>(item) <= static_cast<VkDescriptorSet>(*freeIt) )
        {
          if( item != *freeIt )
          {
            newList.emplace_back( item );
          }
          else
          {
            existingDSToFree.emplace_back( *freeIt );
            ++freeIt;
          }
        }
        else if( freeIt != pool.vkDescriptorSetsToBeFreed.end() )
        {
          ++freeIt;
        }
      });

      if( !existingDSToFree.empty() )
      {
        graphics.GetDevice().freeDescriptorSets( pool.vkPool, existingDSToFree );
      }

      pool.vkDescriptorSets = std::move( newList );

      // update available number of descriptor
      pool.available += uint32_t( existingDSToFree.size() );

      // clear the free list
      pool.vkDescriptorSetsToBeFreed.clear();
    }
  }
}

bool DescriptorSetAllocator::ValidateDescriptorSetList( const DescriptorSetList& list )
{
  for( auto& pool : mPoolSet[list.reserved->bufferIndex] )
  {
    if( list.reserved->poolUID == pool.uid )
    {
      return true;
    }
  }
  return false;
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

  const auto bufferIndex = mController.GetCurrentBufferIndex();

  // clean pools dirty flags
  for( auto& pool : mPoolSet[mController.GetCurrentBufferIndex()] )
  {
    pool.dirty = false;
  }

  auto& poolset = mPoolSet[bufferIndex];

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
      (*it).shouldReallocate = true;
      (*it).available = 0;
      (*it).uid = ++mPoolUID;
      (*it).maxSets = 0;
      requirement.resultDirty = true;
    }
    else
    {
      (*it).shouldReallocate = false;
      requirement.resultDirty = false;
    }

    auto& pool = *it;

    pool.requiredDescriptorCount = requirement;
    pool.requiredSets = requirement.maxSets;

    if( pool.available < pool.requiredSets )//requirement.notAllocatedSets && requirement.notAllocatedSets > pool.available )
    {
      pool.shouldReallocate = true;
    }
    else
    {
      pool.shouldReallocate = false;
    }

    if( !requirement.resultDirty && !pool.shouldReallocate )
    {
      continue;
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
          sizes.emplace_back( VK_DESCRIPTOR_TYPE[i], pool.requiredDescriptorCount.requirements[i] * DESCRIPTOR_SET_MAX_MULTIPLIER );
        }
      }

      vk::DescriptorPoolCreateInfo createInfo;
      createInfo.setMaxSets( pool.requiredSets * DESCRIPTOR_SET_MAX_MULTIPLIER );
      createInfo.setPPoolSizes( sizes.data() );
      createInfo.setPoolSizeCount( uint32_t(sizes.size()) );
      createInfo.setFlags( vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet );

      auto& graphics = mController.GetGraphics();

      auto newPool = graphics.GetDevice().createDescriptorPool( createInfo, graphics.GetAllocator( "DescriptorPool" ) ).value;

      // Update current pool
      pool.maxDescriptorCount = std::move( pool.requiredDescriptorCount );
      pool.maxSets = pool.requiredSets * DESCRIPTOR_SET_MAX_MULTIPLIER;

      auto device = graphics.GetDevice();
      auto pGraphics = &graphics;
      auto oldPool = pool.vkPool;

      if( oldPool )
      {
        if( !pool.vkDescriptorSets.empty() )
        {
          graphics.GetDevice().freeDescriptorSets( pool.vkPool, pool.vkDescriptorSets );
        }

        // clear the free list
        pool.vkDescriptorSetsToBeFreed.clear();

        device.destroyDescriptorPool( oldPool, pGraphics->GetAllocator( "DESCRIPTORPOOL" ) );
      }

      ++mPoolUID;

      // replace pool
      pool.vkPool = newPool;
      pool.dirty = true;
      pool.vkDescriptorSets = {};
      pool.available = pool.maxSets;

      pool.uid = mPoolUID;
    }
  }
}


bool DescriptorSetAllocator::AllocateDescriptorSets(
  const std::vector<Dali::Graphics::Vulkan::DescriptorSetLayoutSignature>& signatures,
  const std::vector<vk::DescriptorSetLayout>& layouts,
  DescriptorSetList& descriptorSets )
{
  // access correct pool
  auto& poolset = mPoolSet[mController.GetCurrentBufferIndex()];

  auto& retval = descriptorSets.descriptorSets;

  // For each signature look for the right pool
  for( auto& signature : signatures )
  {
    auto it = std::find_if( poolset.begin(), poolset.end(), [&]( auto& item ){
      return item.signature == signature;
    });

    // Assert if pool hasn't been found. It should never happen if
    // pools have been updated with correct requirements before allocation requests.
    assert( (it != poolset.end()) && "No pool found for signature. Did UpdateWithRequirements() run with correct requirements?" );

    // Check if descriptor sets are already allocated and if pool is dirty.
    // Continue to the next signature if pool hasn't changed and there are already
    // existing sets allocated for the render command
    if( !(*it).dirty && !descriptorSets.descriptorSets.empty() )
    {
      continue;
    }

    vk::DescriptorSetAllocateInfo allocateInfo{};
    allocateInfo.setDescriptorSetCount( uint32_t( signatures.size() ) );
    allocateInfo.setDescriptorPool( (*it).vkPool );
    allocateInfo.setPSetLayouts( layouts.data() );

    auto& graphics = mController.GetGraphics();
    std::vector<vk::DescriptorSet> result;
    result.resize( allocateInfo.descriptorSetCount );

    auto err = graphics.GetDevice().allocateDescriptorSets( &allocateInfo, result.data() );

    assert( (err == vk::Result::eSuccess) && "Can't allocate descriptor sets!" );

    // reset book-keeping data
    (*it).available -= uint32_t( result.size() );
    descriptorSets.reserved.reset( new DescriptorSetList::Internal() );
    descriptorSets.reserved->pool = (*it).vkPool;
    descriptorSets.reserved->bufferIndex = mController.GetCurrentBufferIndex();
    descriptorSets.reserved->signature = (*it).signature;
    descriptorSets.reserved->poolUID = (*it).uid;

    // track descriptor sets
    if( result.size() > 1 )
    {
      (*it).vkDescriptorSets.insert( (*it).vkDescriptorSets.end(), result.begin(), result.end() );
    }
    else
    {
      (*it).vkDescriptorSets.emplace_back( result[0] );
    }
    retval = std::move( result );

    // sort for faster lookup
    std::sort( (*it).vkDescriptorSets.begin(), (*it).vkDescriptorSets.end() );

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
    auto bufferIndex = list.reserved->bufferIndex;
    std::find_if( mPoolSet[bufferIndex].begin(), mPoolSet[bufferIndex].end(), [&]( Pool& pool )
    {
      if( pool.uid == list.reserved->poolUID && pool.vkPool == list.reserved->pool )
      {
        pool.vkDescriptorSetsToBeFreed.insert( pool.vkDescriptorSetsToBeFreed.end(), list.descriptorSets.begin(), list.descriptorSets.end() );
        return true;
      }
      return false;
    });
  }
}

void DescriptorSetAllocator::InvalidateAllDescriptorSets()
{
  auto& graphics = mController.GetGraphics();
  graphics.DeviceWaitIdle();
  for( auto& pool : mPoolSet[mController.GetCurrentBufferIndex()] )
  {
    graphics.GetDevice().destroyDescriptorPool(
      pool.vkPool,
      &mController.GetGraphics().GetAllocator("DESCRIPTORPOOL") );
  }
  mPoolSet[mController.GetCurrentBufferIndex()].clear();
}

} // Namespace Internal

} // Namespace VulkanAPI

} // Namespace Graphics

} // Namespace Vulkan