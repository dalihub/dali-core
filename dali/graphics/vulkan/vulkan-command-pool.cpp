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
    return true;
  }


  void Reset( bool releaseResources )
  {
    mGraphics.GetDevice().resetCommandPool( mCommandPool, releaseResources ? vk::CommandPoolResetFlagBits::eReleaseResources : vk::CommandPoolResetFlags{} );
    mAllocatedCommandBuffers.clear();
  }

  CommandBufferRef NewCommandBuffer( const vk::CommandBufferAllocateInfo& allocateInfo )
  {
    vk::CommandBufferAllocateInfo info( allocateInfo );
    info.setCommandPool( mCommandPool );
    info.setCommandBufferCount(1);
    auto retval = VkAssert( mGraphics.GetDevice().allocateCommandBuffers( info ) );
    mAllocatedCommandBuffers.emplace_back( new CommandBuffer( mInterface, info, retval[0]) );
    return mAllocatedCommandBuffers.back();
  }

  bool ReleaseCommandBuffer( const CommandBufferRef& buffer, bool forceRelease )
  {
    if( buffer.GetRefCount() == 2 )
    {
      for(auto&& cmdBuf : mAllocatedCommandBuffers )
      {
        if(cmdBuf == buffer )
        {
          // fixme: should remove from list but in future the cache of command buffer will work
          // different
          cmdBuf.Reset();
          return true;
        }
      }
    }
    return false;
  }

  Graphics& mGraphics;
  CommandPool& mInterface;
  vk::CommandPoolCreateInfo mCreateInfo;
  vk::CommandPool mCommandPool;

  std::vector<CommandBufferRef> mAllocatedCommandBuffers;
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

bool CommandPool::ReleaseCommandBuffer( CommandBufferRef buffer, bool forceRelease )
{
  return mImpl->ReleaseCommandBuffer( buffer, forceRelease );
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

