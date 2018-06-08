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

// INTERNAL INCLUDES
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-command-pool.h>
#include <dali/graphics/vulkan/vulkan-command-buffer.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

/**
 *
 * Struct: InternalPool
 */
CommandPool::InternalPool::Node::Node( uint32_t _nextFreeIndex, CommandBuffer* _commandBuffer )
  : nextFreeIndex( _nextFreeIndex ),
    commandBuffer( _commandBuffer )
{
}

CommandPool::InternalPool::InternalPool( CommandPool& owner, Dali::Graphics::Vulkan::Graphics* graphics,
                                         uint32_t initialCapacity, bool isPrimary )
        : mOwner( owner ),
          mGraphics( graphics ),
          mPoolData{},
          mFirstFree(INVALID_NODE_INDEX),
          mCapacity( initialCapacity ),
          mAllocationCount( 0u ),
          mIsPrimary( isPrimary )
{
  // don't allocate anything if initial capacity is 0
  if(initialCapacity)
  {
    Resize(initialCapacity);
  }
}

CommandPool::InternalPool::~InternalPool()
{
  // free all buffers here
  for( auto&& cmd : mPoolData )
  {
    delete cmd.commandBuffer;
  }
}

std::vector< vk::CommandBuffer > CommandPool::InternalPool::AllocateVkCommandBuffers( vk::CommandBufferAllocateInfo allocateInfo )
{
  return VkAssert( mGraphics->GetDevice().allocateCommandBuffers( allocateInfo ) );
}

void CommandPool::InternalPool::Resize( uint32_t newCapacity )
{
  if( newCapacity <= mPoolData.size() )
  {
    return;
  }

  auto diff = newCapacity - mPoolData.size();

  auto allocateInfo = vk::CommandBufferAllocateInfo{}
          .setCommandBufferCount( U32(diff) )
          .setCommandPool(mOwner.GetVkHandle() )
          .setLevel( mIsPrimary ? vk::CommandBufferLevel::ePrimary : vk::CommandBufferLevel::eSecondary );
  auto newBuffers = AllocateVkCommandBuffers( allocateInfo );

  uint32_t i = U32(mPoolData.size() + 1);

  mFirstFree = U32(mPoolData.size());
  if(!mPoolData.empty())
  {
    mPoolData.back()
             .nextFreeIndex = U32(mPoolData.size());
  }
  for( auto&& cmdbuf : newBuffers )
  {
    auto commandBuffer = new CommandBuffer( mOwner, i-1, allocateInfo, cmdbuf);
    mPoolData.emplace_back( i, commandBuffer );
    ++i;
  }
  mPoolData.back().nextFreeIndex = INVALID_NODE_INDEX;
  mCapacity = U32(mPoolData.size());
}

RefCountedCommandBuffer CommandPool::InternalPool::AllocateCommandBuffer( bool reset )
{
  // resize if no more nodes
  if( mFirstFree == INVALID_NODE_INDEX )
  {
    auto newSize = static_cast<uint32_t>(mPoolData.empty() ? 1 : 2 * mPoolData.size());
    Resize( U32( newSize ) );
  }

  auto& node = mPoolData[mFirstFree];
  mFirstFree = node.nextFreeIndex;

  if( reset )
  {
    node.commandBuffer->Reset();
  }

  ++mAllocationCount;
  return RefCountedCommandBuffer( node.commandBuffer );
}

void CommandPool::InternalPool::ReleaseCommandBuffer( CommandBuffer& buffer, bool reset )
{
  auto indexInPool = buffer.GetPoolAllocationIndex();
  mPoolData[indexInPool].nextFreeIndex = mFirstFree;
  mFirstFree = indexInPool;

  if( reset )
  {
    buffer.Reset();
  }
  --mAllocationCount;
}

uint32_t CommandPool::InternalPool::GetCapacity() const
{
  return mCapacity;
}

uint32_t CommandPool::InternalPool::GetAllocationCount() const
{
  return mAllocationCount;
}

/**
 *
 * Class: CommandPool
 */
RefCountedCommandPool CommandPool::New( Graphics& graphics, const vk::CommandPoolCreateInfo& createInfo )
{
  auto retval = Handle<CommandPool>( new CommandPool(graphics, createInfo) );

  if(retval && retval->Initialize())
  {
    graphics.AddCommandPool( retval );
  }

  return retval;
}

RefCountedCommandPool CommandPool::New( Graphics& graphics )
{
  return New( graphics, vk::CommandPoolCreateInfo{});
}

bool CommandPool::Initialize()
{
  mCreateInfo.setFlags( vk::CommandPoolCreateFlagBits::eResetCommandBuffer );
  mCommandPool = VkAssert(mGraphics->GetDevice().createCommandPool(mCreateInfo, mGraphics->GetAllocator()));
  mInternalPoolPrimary = std::make_unique<InternalPool>( *this, mGraphics, 0, true );
  mInternalPoolSecondary = std::make_unique<InternalPool>( *this, mGraphics, 0, false );
  return true;
}

CommandPool::CommandPool() = default;

CommandPool::CommandPool(Graphics& graphics, const vk::CommandPoolCreateInfo& createInfo)
        : mGraphics(&graphics),
          mCreateInfo(createInfo)
{
}

CommandPool::~CommandPool()
{
  if(mCommandPool)
  {
    mGraphics->GetDevice().destroyCommandPool( mCommandPool, mGraphics->GetAllocator());
  }
}

vk::CommandPool CommandPool::GetVkHandle() const
{
  return mCommandPool;
}

Graphics& CommandPool::GetGraphics() const
{
  return *mGraphics;
}

RefCountedCommandBuffer CommandPool::NewCommandBuffer( const vk::CommandBufferAllocateInfo& allocateInfo )
{
  return NewCommandBuffer( allocateInfo.level == vk::CommandBufferLevel::ePrimary );
}

RefCountedCommandBuffer CommandPool::NewCommandBuffer( bool isPrimary )
{
  auto& usedPool = isPrimary ? *mInternalPoolPrimary : *mInternalPoolSecondary;
  return usedPool.AllocateCommandBuffer( false );
}

void CommandPool::Reset( bool releaseResources )
{
  mGraphics->GetDevice()
           .resetCommandPool( mCommandPool,
                              releaseResources ? vk::CommandPoolResetFlagBits::eReleaseResources
                                               : vk::CommandPoolResetFlags{} );
}

bool CommandPool::ReleaseCommandBuffer( CommandBuffer& buffer )
{
  if(buffer.IsPrimary())
  {
    mInternalPoolPrimary->ReleaseCommandBuffer( buffer );
  }
  else
  {
    mInternalPoolSecondary->ReleaseCommandBuffer( buffer );
  }
  return false;
}

uint32_t CommandPool::GetCapacity() const
{
  return mInternalPoolPrimary->GetCapacity()+
         mInternalPoolSecondary->GetCapacity();
}

uint32_t CommandPool::GetAllocationCount() const
{
  return mInternalPoolPrimary->GetAllocationCount()+
         mInternalPoolSecondary->GetAllocationCount();
}

uint32_t CommandPool::GetAllocationCount( vk::CommandBufferLevel level ) const
{
  return level == vk::CommandBufferLevel::ePrimary ?
         mInternalPoolPrimary->GetAllocationCount() :
         mInternalPoolSecondary->GetAllocationCount();
}

bool CommandPool::OnDestroy()
{
  mGraphics->RemoveCommandPool( *this );

  auto device = mGraphics->GetDevice();
  auto commandPool = mCommandPool;
  auto allocator = &mGraphics->GetAllocator();

  mGraphics->DiscardResource( [device, commandPool, allocator]() {
    device.destroyCommandPool( commandPool, allocator );
  } );

  return false;
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

