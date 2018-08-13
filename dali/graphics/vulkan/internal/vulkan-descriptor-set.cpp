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

#include <dali/graphics/vulkan/internal/vulkan-descriptor-set.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/internal/vulkan-buffer.h>
#include <dali/graphics/vulkan/internal/vulkan-image.h>
#include <dali/graphics/vulkan/internal/vulkan-image-view.h>
#include <dali/graphics/vulkan/internal/vulkan-sampler.h>
#include <dali/graphics/vulkan/internal/vulkan-debug.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

/**
 * Class DescriptorPool
 */

Handle< DescriptorPool > DescriptorPool::New( Graphics& graphics, const vk::DescriptorPoolCreateInfo& createInfo )
{
  auto pool = Handle< DescriptorPool >( new DescriptorPool( graphics, createInfo ) );
  if( pool->Initialise() )
  {
    graphics.AddDescriptorPool( *pool );
  }
  return pool;
}

DescriptorPool::~DescriptorPool() = default;

bool DescriptorPool::Initialise()
{
  mDescriptorPool = VkAssert( mGraphics->GetDevice().createDescriptorPool( mCreateInfo, mGraphics->GetAllocator() ) );
  return true;
}

DescriptorPool::DescriptorPool( Graphics& graphics, const vk::DescriptorPoolCreateInfo& createInfo )
        : mGraphics( &graphics ),
          mCreateInfo( createInfo ),
          mAvailableAllocations( createInfo.maxSets )
{
}

vk::DescriptorPool DescriptorPool::GetVkHandle() const
{
  return mDescriptorPool;
}

std::vector< RefCountedDescriptorSet >
DescriptorPool::AllocateDescriptorSets( vk::DescriptorSetAllocateInfo allocateInfo )
{
  assert( allocateInfo.descriptorSetCount <= mAvailableAllocations );

  // all other fields must be set correct
  allocateInfo.setDescriptorPool( mDescriptorPool );

  auto descriptorSetHandles = std::vector< vk::DescriptorSet >{};
  descriptorSetHandles.resize( allocateInfo.descriptorSetCount );
  auto result = mGraphics->GetDevice().allocateDescriptorSets( &allocateInfo, descriptorSetHandles.data() );

  if( result != vk::Result::eSuccess )
  {
    return std::move( std::vector< RefCountedDescriptorSet >{} );
  }

  auto retval = std::vector< RefCountedDescriptorSet >{};
  retval.reserve( descriptorSetHandles.size() );
  for( auto&& item : descriptorSetHandles )
  {
    RefCountedDescriptorSet handle( new DescriptorSet( *mGraphics, *this, item, allocateInfo ) );
    retval.emplace_back( handle );
    mDescriptorSetCache.emplace_back( handle );
  }

  if( !retval.empty() )
  {
    mAvailableAllocations -= allocateInfo.descriptorSetCount;
  }

  return retval;
}

void DescriptorPool::FreeDescriptorSets( const std::vector< vk::DescriptorSet >& descriptorSets )
{
  assert( mCreateInfo.flags & vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet
          && "Cannot call free descriptor sets. Pool has not been created with: eFreeDescriptorSet." );
  if( !descriptorSets.empty() )
  {
    if( mGraphics->GetDevice().freeDescriptorSets( mDescriptorPool, descriptorSets ) == vk::Result::eSuccess )
    {
      mAvailableAllocations += U32( descriptorSets.size() );
    }
  }
}

uint32_t DescriptorPool::GetAvailableAllocations() const
{
  return mAvailableAllocations;
}

void DescriptorPool::Reset()
{
  mGraphics->GetDevice().resetDescriptorPool( mDescriptorPool );
  mDescriptorSetCache.clear();
}

bool DescriptorPool::OnDestroy()
{
  mGraphics->RemoveDescriptorPool( *this );

  auto device = mGraphics->GetDevice();
  auto descriptorPool = mDescriptorPool;
  auto allocator = &mGraphics->GetAllocator();

  mGraphics->DiscardResource( [ device, descriptorPool, allocator ]() {
    DALI_LOG_INFO( gVulkanFilter, Debug::General, "Invoking deleter function: descriptor pool->%p\n",
                   static_cast< VkDescriptorPool >(descriptorPool) )
    device.destroyDescriptorPool( descriptorPool, allocator );
  } );

  return false;
}

/**
 * Class DescriptorSet
 */

//Called by DescriptorPool only!
DescriptorSet::DescriptorSet( Graphics& graphics,
                              DescriptorPool& pool,
                              vk::DescriptorSet descriptorSet,
                              vk::DescriptorSetAllocateInfo allocateInfo )
        : mGraphics( &graphics ),
          mPool( &pool ),
          mAllocateInfo( allocateInfo ),
          mDescriptorSet( descriptorSet )
{
}

DescriptorSet::~DescriptorSet() = default;

void DescriptorSet::WriteUniformBuffer( uint32_t binding, Handle< Buffer > buffer, uint32_t offset, uint32_t size )
{
  auto bufferInfo = vk::DescriptorBufferInfo{}
          .setOffset( U32( offset ) )
          .setRange( U32( size ) )
          .setBuffer( buffer->GetVkHandle() );

  auto write = vk::WriteDescriptorSet{}.setPBufferInfo( &bufferInfo )
                                       .setDescriptorType( vk::DescriptorType::eUniformBuffer )
                                       .setDescriptorCount( 1 )
                                       .setDstSet( mDescriptorSet )
                                       .setDstBinding( binding )
                                       .setDstArrayElement( 0 );

  // write descriptor set
  mGraphics->GetDevice().updateDescriptorSets( 1, &write, 0, nullptr );
}

vk::DescriptorSet DescriptorSet::GetVkDescriptorSet() const
{
  return mDescriptorSet;
}

void DescriptorSet::WriteCombinedImageSampler( uint32_t binding,
                                               RefCountedSampler sampler,
                                               RefCountedImageView imageView )
{
  auto imageViewInfo = vk::DescriptorImageInfo{}
          .setImageLayout( vk::ImageLayout::eShaderReadOnlyOptimal )
          .setImageView( imageView->GetVkHandle() )
          .setSampler( sampler->GetVkHandle() );

  auto write = vk::WriteDescriptorSet{}.setPImageInfo( &imageViewInfo )
                                       .setDescriptorType( vk::DescriptorType::eCombinedImageSampler )
                                       .setDescriptorCount( 1 )
                                       .setDstSet( mDescriptorSet )
                                       .setDstBinding( binding )
                                       .setDstArrayElement( 0 );

  // write descriptor set
  mGraphics->GetDevice().updateDescriptorSets( 1, &write, 0, nullptr );
}

bool DescriptorSet::OnDestroy()
{
  auto descriptorPool = mPool;
  auto descriptorSet = mDescriptorSet;

  mGraphics->EnqueueAction( [ descriptorPool, descriptorSet ]()
                              {
                                DALI_LOG_INFO( gVulkanFilter,
                                               Debug::General,
                                               "Freeing descriptor set: %p -> Returning set to pool: %p\n",
                                               static_cast< void* >(descriptorSet),
                                               static_cast< void* >(descriptorPool))
                                descriptorPool->FreeDescriptorSets( { descriptorSet } );
                              } );

  return false;
}


#if 0
struct DescriptorSetLayout::Impl
{
  Impl( Graphics& graphics, const vk::DescriptorSetLayoutCreateInfo& createInfo ) :
    mGraphics( graphics ),
    mCreateInfo( createInfo )
  {
  }

  ~Impl()
  {
    if(mLayout)
    {
      mGraphics.GetDevice().destroyDescriptorSetLayout( mLayout, mGraphics.GetAllocator() );
    }
  }

  bool Initialise()
  {
    mLayout = VkAssert( mGraphics.GetDevice().createDescriptorSetLayout( mCreateInfo, mGraphics.GetAllocator() ));
    if(mLayout)
    {
      return true;
    }
    return false;
  }

  Graphics&                         mGraphics;
  vk::DescriptorSetLayout           mLayout;
  vk::DescriptorSetLayoutCreateInfo mCreateInfo;
};

/**
 * Class: DescriptorSetLayout
 */

std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::New( Graphics& graphics, const vk::DescriptorSetLayoutCreateInfo& createInfo )
{
  auto retval = std::unique_ptr<DescriptorSetLayout>( new DescriptorSetLayout(graphics, createInfo ) );
  if( retval->mImpl->Initialise() )
  {
    return retval;
  }
  return nullptr;
}

DescriptorSetLayout::DescriptorSetLayout( Graphics& graphics, const vk::DescriptorSetLayoutCreateInfo& createInfo )
{
  mImpl = MakeUnique<DescriptorSetLayout::Impl>( graphics, createInfo );
}
#endif
} // Namespace Vulkan

} // Namespace Graphics

} // Namespace Dali
