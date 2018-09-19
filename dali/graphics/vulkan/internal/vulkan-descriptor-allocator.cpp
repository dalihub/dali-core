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
 */

#include <dali/graphics/vulkan/internal/vulkan-descriptor-allocator.h>
#include <cmath>
#include <dali/graphics/vulkan/internal/vulkan-debug.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

DescriptorSetAllocator::DescriptorSetAllocator( Graphics& graphics, uint32_t maxPoolSize )
: mGraphics( &graphics ),
  mMaxPoolSize( maxPoolSize )
{
}

std::vector< RefCountedDescriptorSet >
DescriptorSetAllocator::AllocateDescriptorSets( const std::vector< DescriptorSetLayoutSignature >& signatures,
                                                const std::vector< vk::DescriptorSetLayout >& descriptorSetLayouts )
{
  //region Check if Pool data exist for this thread
  assert( !signatures.empty() && "Descriptor layout signatures vector should not be empty." );
  assert( !descriptorSetLayouts.empty() && "Descriptor set layout vector should not be empty." );

  auto totalSetsToAllocate = I32( descriptorSetLayouts.size() );

  auto refCountedDescriptorSets = std::vector< RefCountedDescriptorSet >();
  refCountedDescriptorSets.reserve( descriptorSetLayouts.size() );

  mMutex.lock(); //Ensure that we do not reade while another thread writes.
  auto iterator = std::find_if( mStorage.begin(),
                                mStorage.end(),
                                [ & ]( const SignatureAndPoolsPerThread& entry ) {
                                  return entry.threadId == std::this_thread::get_id();
                                } );
  //endregion

  //region If the Pool data do not exist create a new entry.
  if( iterator == mStorage.end() ) //no data exists for this thread. Create an entry
  {
    auto entry = std::vector< SignatureAndPools >{};
    entry.reserve( signatures.size() ); // 1 instance of SignatureAndPools per signature

    for( auto& signature : signatures ) // Create 1 tuple per signature
    {
      auto poolVector = std::vector< RefCountedDescriptorPool >{};
      poolVector.reserve( 1 );

      ExtendPoolVectorForSignature( signature, poolVector, 1 );

      entry.emplace_back( signature, poolVector  );
    }

    mStorage.emplace_back( std::this_thread::get_id(), entry );

    iterator = mStorage.end() - 1;
  }
  //endregion

  //region Pool data found or created. Allocate descriptor sets for each requested signature.
  // Vector of pools was found or created so fetch it
  auto& signatureAndPoolsVector = ( *iterator ).signatureAndPools;
  mMutex.unlock();

  for( auto& signature : signatures ) // For each one of the signatures...
  {
    //region Try to locate it inside the data structure
    auto foundIterator = std::find_if( signatureAndPoolsVector.begin(),
                                       signatureAndPoolsVector.end(),
                                       [ & ]( const SignatureAndPools& signatureAndPools )
                                       {
                                         bool found = false;
                                         for( auto& entry : signatures )
                                         {
                                           if( entry == signatureAndPools.signature )
                                           {
                                             found = true;
                                           }
                                         }

                                         return found;
                                       } );

    // If an entry with the signature is not found create a new entry
    if( foundIterator == signatureAndPoolsVector.end() )
    {
      auto descriptorPoolSizes = GetPoolSizesFromDescriptorSetLayoutSignature( signature );

      auto poolCreateInfo = vk::DescriptorPoolCreateInfo{}
      .setMaxSets( mMaxPoolSize )
      .setFlags( vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet )
      .setPoolSizeCount( static_cast< uint32_t >( descriptorPoolSizes.size() ) )
      .setPPoolSizes( descriptorPoolSizes.data() );

      auto pool = DescriptorPool::New( *mGraphics, poolCreateInfo );
      signatureAndPoolsVector.emplace_back( signature, std::vector< RefCountedDescriptorPool >{ pool } );

      foundIterator = signatureAndPoolsVector.end() - 1;
    }
    //endregion

    //region Signature match! Start the allocation process...
    // Get the vector of pools
    auto& pools = ( *foundIterator ).pools;

    // Iterator that determines from which point in the vector
    // the descriptor set layouts should be read for the allocation.
    auto allocationIterator = descriptorSetLayouts.begin();

    // While there are still allocations left to be done...
    while( totalSetsToAllocate > 0 )
    {
      // Check all the available pools
      for( auto& pool : pools )
      {
        int allocationCount = 0;

        // If the pool has capacity to allocate
        if( pool->GetAvailableAllocations() > 0 )
        {
          // Check how many of the total amount of sets can be allocated from this pool...
          // A result > 0 means that the whole allocation fits and the pool has sets to spare
          // A result = 0 means that the requested allocation consumes the remaining capacity of this pool exactly.
          // A result < 0 means that the requested allocation cannot be done in it's entirety by this pool.
          // In the third case allocate all we can from this pool.
          // The rest of the allocation will happen using another.
          auto poolCapacity = I32( pool->GetAvailableAllocations() ) - totalSetsToAllocate;

          allocationCount = poolCapacity >= 0 ? totalSetsToAllocate : totalSetsToAllocate - poolCapacity;

          auto descriptorSetAllocateInfo =
          vk::DescriptorSetAllocateInfo{}
          .setDescriptorSetCount( static_cast< uint32_t>( allocationCount ) )
          .setPSetLayouts( &( *allocationIterator ) );

          auto result = pool->AllocateDescriptorSets( descriptorSetAllocateInfo );
          refCountedDescriptorSets.insert( refCountedDescriptorSets.end(),
                                           result.begin(),
                                           result.end() );

          if( !refCountedDescriptorSets.empty() ) //Successful allocation
          {
            totalSetsToAllocate -= allocationCount;

            // Advance allocation the iterator
            std::advance( allocationIterator, allocationCount );

            // No more sets left to allocate
            if( !totalSetsToAllocate )
            {
              break;
            }
          }
        }
      }

      // There are still sets left to allocate but all the pools are full. Have to create new ones.
      if( totalSetsToAllocate > 0 )
      {
        // Determine the amount of pools needed to complete all the allocations left
        auto division = F32( totalSetsToAllocate ) / F32( mMaxPoolSize );
        auto poolsToAllocate = U32( std::ceil( division ) );

        ExtendPoolVectorForSignature( signature, pools, poolsToAllocate );
      }
    }
    //endregion
  }
  //endregion

  PrintAllocationReport( *this );

  return refCountedDescriptorSets;
}

std::vector< vk::DescriptorPoolSize >
DescriptorSetAllocator::GetPoolSizesFromDescriptorSetLayoutSignature(
const DescriptorSetLayoutSignature& signature )
{
  auto decodedResults = signature.Decode();
  auto descriptorPoolSizes = std::vector< vk::DescriptorPoolSize >{};
  descriptorPoolSizes.reserve( decodedResults.size() );

  for( const auto& tuple : decodedResults )
  {
    auto descriptorType = std::get< DescriptorType >( tuple );

    auto value = U32( std::get< uint64_t >( tuple ) );
    auto descriptorPoolSize = vk::DescriptorPoolSize{}.setDescriptorCount( value * mMaxPoolSize );

    switch( descriptorType )
    {
      case DescriptorType::STORAGE_IMAGE:
        descriptorPoolSize.setType( vk::DescriptorType::eStorageImage );
        break;
      case DescriptorType::SAMPLER:
        descriptorPoolSize.setType( vk::DescriptorType::eSampler );
        break;
      case DescriptorType::SAMPLED_IMAGE:
        descriptorPoolSize.setType( vk::DescriptorType::eSampledImage );
        break;
      case DescriptorType::COMBINED_IMAGE_SAMPLER:
        descriptorPoolSize.setType( vk::DescriptorType::eCombinedImageSampler );
        break;
      case DescriptorType::UNIFORM_TEXEL_BUFFER:
        descriptorPoolSize.setType( vk::DescriptorType::eUniformTexelBuffer );
        break;
      case DescriptorType::STORAGE_TEXEL_BUFFER:
        descriptorPoolSize.setType( vk::DescriptorType::eStorageTexelBuffer );
        break;
      case DescriptorType::UNIFORM_BUFFER:
        descriptorPoolSize.setType( vk::DescriptorType::eUniformBuffer );
        break;
      case DescriptorType::STORAGE_BUFFER:
        descriptorPoolSize.setType( vk::DescriptorType::eStorageBuffer );
        break;
      case DescriptorType::DYNAMIC_UNIFORM_BUFFER:
        descriptorPoolSize.setType( vk::DescriptorType::eUniformBufferDynamic );
        break;
      case DescriptorType::DYNAMIC_STORAGE_BUFFER:
        descriptorPoolSize.setType( vk::DescriptorType::eStorageBufferDynamic );
        break;
      case DescriptorType::INPUT_ATTACHMENT:
        descriptorPoolSize.setType( vk::DescriptorType::eInputAttachment );
        break;
    }

    descriptorPoolSizes.push_back( descriptorPoolSize );
  }


  return std::move( descriptorPoolSizes );
}

void DescriptorSetAllocator::ExtendPoolVectorForSignature( const DescriptorSetLayoutSignature& signature,
                                                           std::vector< RefCountedDescriptorPool >& poolVector,
                                                           uint32_t count )
{
  auto descriptorPoolSizes = GetPoolSizesFromDescriptorSetLayoutSignature( signature );

  auto poolCreateInfo = vk::DescriptorPoolCreateInfo{}
  .setMaxSets( mMaxPoolSize )
  .setFlags( vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet )
  .setPoolSizeCount( static_cast< uint32_t >( descriptorPoolSizes.size() ) )
  .setPPoolSizes( descriptorPoolSizes.data() );

  for( auto i = 0u; i < count; ++i )
  {
    poolVector.emplace_back( DescriptorPool::New( *mGraphics, poolCreateInfo ) );
  }
}

} //namespace Vulkan
} //namespace Graphics
} //namespace Dali

