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

#include <dali/graphics/vulkan/vulkan-descriptor-set.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-buffer.h>
#include <dali/graphics/vulkan/vulkan-image.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
struct DescriptorPool::Impl
{
  Impl( DescriptorPool& owner, Graphics& graphics, vk::DescriptorPoolCreateInfo createInfo )
  : mGraphics( graphics ),
    mOwner( owner ),
    mCreateInfo( createInfo )
  {

  }

  ~Impl()
  {

  }

  /**
   * Allocates an array of descriptor sets
   * @param allocateInfo
   * @return
   */
  std::vector<Handle<DescriptorSet>> AllocateDescriptorSets( vk::DescriptorSetAllocateInfo allocateInfo )
  {
    // all other fields must be set correct
    allocateInfo.setDescriptorPool( mVkDescriptorPool );
    auto result = VkAssert( mGraphics.GetDevice().allocateDescriptorSets( allocateInfo ) );

    std::vector<Handle<DescriptorSet>> retval;
    retval.reserve( result.size() );
    for( auto&& item : result )
    {
      Handle<DescriptorSet> handle( new DescriptorSet(mGraphics, mOwner, item, allocateInfo) );
      retval.emplace_back( handle );
      mDescriptorSetCache.emplace_back( handle );
    }

    return retval;
  }

  bool Initialise()
  {
    mVkDescriptorPool = VkAssert( mGraphics.GetDevice().createDescriptorPool( mCreateInfo, mGraphics.GetAllocator() ) );
    return true;
  }

  Graphics& mGraphics;
  DescriptorPool& mOwner;
  vk::DescriptorPoolCreateInfo mCreateInfo;

  vk::DescriptorPool mVkDescriptorPool;

  // cache
  std::vector<Handle<DescriptorSet>> mDescriptorSetCache;
};

Handle<DescriptorPool> DescriptorPool::New( Graphics& graphics, const vk::DescriptorPoolCreateInfo& createInfo )
{
  std::unique_ptr<DescriptorPool> pool{ new DescriptorPool( graphics, createInfo ) };
  if(pool->mImpl->Initialise())
  {
    auto handle = Handle<DescriptorPool>(pool.get());
    graphics.AddDescriptorPool(std::move(pool));
    return handle;
  }
  return Handle<DescriptorPool>();
}

DescriptorPool::DescriptorPool( Graphics& graphics, const vk::DescriptorPoolCreateInfo& createInfo )
{
  mImpl = MakeUnique<Impl>( *this, graphics, createInfo );
}

DescriptorPool::~DescriptorPool() = default;

vk::DescriptorPool DescriptorPool::GetVkDescriptorPool() const
{
  return mImpl->mVkDescriptorPool;
}

std::vector<DescriptorSetHandle> DescriptorPool::AllocateDescriptorSets( vk::DescriptorSetAllocateInfo allocateInfo )
{
  return mImpl->AllocateDescriptorSets( allocateInfo );
}

/****************************************************************************************
 * Class DescriptorSet::Impl
 */

struct DescriptorSet::Impl
{
  Impl( Graphics& graphics, DescriptorPool& pool, vk::DescriptorSet set, vk::DescriptorSetAllocateInfo allocateInfo )
  : mGraphics( graphics ),
    mPool( pool ),
    mAllocateInfo( allocateInfo ),
    mVkDescriptorSet( set )
  {

  }

  ~Impl()
  {
    if(mVkDescriptorSet)
    {
      mGraphics.GetDevice().freeDescriptorSets( mPool.GetVkDescriptorPool(), 1, &mVkDescriptorSet );
    }
  }

  void WriteUniformBuffer( uint32_t binding, Handle<Buffer> buffer, uint32_t offset, uint32_t size )
  {
    // add resource to the list
    mResources.emplace_back( buffer.StaticCast<VkManaged>() );

    auto bufferInfo = vk::DescriptorBufferInfo{}
         .setOffset( U32(offset) )
         .setRange( U32(size) )
         .setBuffer( buffer->GetVkBuffer() );

    auto write = vk::WriteDescriptorSet{}.setPBufferInfo( &bufferInfo )
         .setDescriptorType( vk::DescriptorType::eUniformBuffer )
         .setDescriptorCount( 1 )
         .setDstSet( mVkDescriptorSet )
         .setDstBinding( binding )
         .setDstArrayElement( 0 );

    // write descriptor set
    mGraphics.GetDevice().updateDescriptorSets( 1, &write, 0, nullptr  );
  }

  void WriteStorageBuffer( Handle<Buffer> buffer, uint32_t offset, uint32_t size )
  {
    mResources.emplace_back( buffer.StaticCast<VkManaged>() );
  }

  Graphics& mGraphics;
  DescriptorPool& mPool;
  vk::DescriptorSetAllocateInfo mAllocateInfo;
  vk::DescriptorSet             mVkDescriptorSet;

  // attached resources
  std::vector<Handle<VkManaged>> mResources;
};

/**
 * Called by DescriptorPool only!
 */
DescriptorSet::DescriptorSet( Graphics& graphics, DescriptorPool& pool, vk::DescriptorSet descriptorSet, vk::DescriptorSetAllocateInfo allocateInfo )
{
  mImpl = MakeUnique<Impl>( graphics, pool, descriptorSet, allocateInfo );
}

DescriptorSet::~DescriptorSet() = default;

void DescriptorSet::WriteUniformBuffer( uint32_t binding, Handle<Buffer> buffer, uint32_t offset, uint32_t size )
{
  mImpl->WriteUniformBuffer( binding, buffer, offset, size );
}





















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

} // Namespace Vulkan

} // Namespace Graphics

} // Namespace Dali
