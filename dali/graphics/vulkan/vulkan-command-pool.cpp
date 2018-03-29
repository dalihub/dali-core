/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
 * CommandBufferPool contains preallocated command buffers that are
 * reusable.
 */
struct CommandBufferPool
{
  static constexpr uint32_t INVALID_NODE_INDEX{ 0xffffffffu };
  struct Node
  {
    Node( uint32_t _nextFreeIndex, CommandBuffer* _commandBuffer ) :
      nextFreeIndex( _nextFreeIndex ), commandBuffer( _commandBuffer )
    {

    }
    uint32_t          nextFreeIndex;
    CommandBuffer*    commandBuffer;
  };

  CommandBufferPool( CommandPool& owner, Graphics& graphics, uint32_t initialCapacity, uint32_t defaultIncrease, bool isPrimary )
  : mOwner( owner ),
    mGraphics( graphics ),
    mPoolData{},
    mFirstFree(INVALID_NODE_INDEX),
    mCapacity( initialCapacity ),
    mAllocationCount( 0u ),
    mDefaultIncrease( defaultIncrease ),
    mIsPrimary( isPrimary )
  {
    // don't allocate anything if initial capacity is 0
    if(initialCapacity)
    {
      Resize(initialCapacity);
    }
  }

  ~CommandBufferPool()
  {
    // free all buffers here
    for( auto&& cmd : mPoolData )
    {
      delete cmd.commandBuffer;
    }
  }

  /**
   * Creates new batch of command buffers
   * @param allocateInfo
   * @return
   */
  std::vector<vk::CommandBuffer> AllocateVkCommandBuffers( vk::CommandBufferAllocateInfo allocateInfo )
  {
    return VkAssert( mGraphics.GetDevice().allocateCommandBuffers( allocateInfo ) );
  }

  /**
   * Resizes command pool to the new capacity. Pool may only grow
   * @param newCapacity
   */
  void Resize( uint32_t newCapacity )
  {
    if( newCapacity <= mPoolData.size() )
    {
      return;
    }

    auto diff = newCapacity - mPoolData.size();

    auto allocateInfo = vk::CommandBufferAllocateInfo{}
      .setCommandBufferCount( U32(diff) )
      .setCommandPool( mOwner.GetPool() )
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

  /**
   * Allocates new command buffer
   * @return
   */
  CommandBufferRef AllocateCommandBuffer( bool reset )
  {
    // resize if no more nodes
    if( mFirstFree == INVALID_NODE_INDEX )
    {
      Resize( U32(mPoolData.size() + mDefaultIncrease) );
    }

    auto& node = mPoolData[mFirstFree];
    mFirstFree = node.nextFreeIndex;

    if( reset )
    {
      node.commandBuffer->Reset();
    }

    ++mAllocationCount;
    return CommandBufferRef(node.commandBuffer);
  }

  /**
   * Releases command buffer back to the pool
   * @param reset if true, Resets command buffer
   * @param ref
   */
  void ReleaseCommandBuffer( CommandBuffer& buffer, bool reset = false )
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

  uint32_t GetCapacity() const
  {
    return mCapacity;
  }

  uint32_t GetAllocationCount() const
  {
    return mAllocationCount;
  }

  CommandPool&                  mOwner;
  Graphics&                     mGraphics;
  std::vector<Node>             mPoolData;
  uint32_t                      mFirstFree;
  uint32_t                      mCapacity;
  uint32_t                      mAllocationCount;
  uint32_t                      mDefaultIncrease;
  bool                          mIsPrimary;
};

/**
 *
 * Class: CommandPool::Impl
 */
struct CommandPool::Impl
{
  Impl( Graphics& graphics, CommandPool& interface, const vk::CommandPoolCreateInfo& createInfo )
  : mGraphics( graphics ),
    mInterface( interface ),
    mCreateInfo( createInfo ),
    mCommandPool( nullptr )
  {
  }

  ~Impl()
  {
    if(mCommandPool)
    {
      mGraphics.GetDevice().destroyCommandPool( mCommandPool, mGraphics.GetAllocator());
    }
  }

  bool Initialise()
  {
    mCommandPool = VkAssert(mGraphics.GetDevice().createCommandPool(mCreateInfo, mGraphics.GetAllocator()));
    mInternalPoolPrimary = std::make_unique<CommandBufferPool>( mInterface, mGraphics, 0, 32, true );
    mInternalPoolSecondary = std::make_unique<CommandBufferPool>( mInterface, mGraphics, 0, 32, false );
    return true;
  }

  void Reset( bool releaseResources )
  {
    mGraphics.GetDevice().resetCommandPool( mCommandPool, releaseResources ? vk::CommandPoolResetFlagBits::eReleaseResources : vk::CommandPoolResetFlags{} );
  }

  CommandBufferRef NewCommandBuffer( const vk::CommandBufferAllocateInfo& allocateInfo )
  {
    vk::CommandBufferAllocateInfo info( allocateInfo );
    auto& usedPool = allocateInfo.level == vk::CommandBufferLevel::ePrimary ? *mInternalPoolPrimary.get() : *mInternalPoolSecondary.get();
    auto retval = usedPool.AllocateCommandBuffer( false );
    return retval;
  }

  bool ReleaseCommandBuffer( CommandBuffer& buffer, bool forceRelease )
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

  Graphics& mGraphics;
  CommandPool& mInterface;
  vk::CommandPoolCreateInfo mCreateInfo;
  vk::CommandPool mCommandPool;

  // Pools are lazily allocated, depends on the requested command buffers
  std::unique_ptr<CommandBufferPool> mInternalPoolPrimary;
  std::unique_ptr<CommandBufferPool> mInternalPoolSecondary;
};

/**
 *
 * Class: CommandPool
 */
CommandPoolHandle CommandPool::New( Graphics& graphics, const vk::CommandPoolCreateInfo& createInfo )
{
  auto retval = Handle<CommandPool>( new CommandPool(graphics, createInfo) );

  if(retval && retval->mImpl->Initialise())
  {
    graphics.AddCommandPool( retval );
  }

  return retval;
}

CommandPoolHandle CommandPool::New( Graphics& graphics )
{
  return New( graphics, vk::CommandPoolCreateInfo{});
}

CommandPool::CommandPool() = default;

CommandPool::CommandPool(Graphics& graphics, const vk::CommandPoolCreateInfo& createInfo)
{
  mImpl = MakeUnique<Impl>( graphics, *this, createInfo );
}

CommandPool::~CommandPool() = default;

vk::CommandPool CommandPool::GetPool() const
{
  return mImpl->mCommandPool;
}

Graphics& CommandPool::GetGraphics() const
{
  return mImpl->mGraphics;
}

bool CommandPool::OnDestroy()
{
  return false;
}

CommandBufferRef CommandPool::NewCommandBuffer( const vk::CommandBufferAllocateInfo& allocateInfo )
{
  return mImpl->NewCommandBuffer( allocateInfo );
}

CommandBufferRef CommandPool::NewCommandBuffer( bool isPrimary )
{
  return mImpl->NewCommandBuffer( vk::CommandBufferAllocateInfo{}.setLevel(
    isPrimary ? vk::CommandBufferLevel::ePrimary : vk::CommandBufferLevel::eSecondary
  ) );
}

void CommandPool::Reset( bool releaseResources )
{
  mImpl->Reset( releaseResources );
}

bool CommandPool::ReleaseCommandBuffer( CommandBuffer& buffer, bool forceRelease )
{
  return mImpl->ReleaseCommandBuffer( buffer, forceRelease );
}

uint32_t CommandPool::GetCapacity() const
{
  return mImpl->mInternalPoolPrimary->GetCapacity()+
         mImpl->mInternalPoolSecondary->GetCapacity();
}

uint32_t CommandPool::GetAllocationCount() const
{
  return mImpl->mInternalPoolPrimary->GetAllocationCount()+
         mImpl->mInternalPoolSecondary->GetAllocationCount();
}

uint32_t CommandPool::GetAllocationCount( vk::CommandBufferLevel level ) const
{
  return level == vk::CommandBufferLevel::ePrimary ?
         mImpl->mInternalPoolPrimary->GetAllocationCount() :
         mImpl->mInternalPoolSecondary->GetAllocationCount();
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

