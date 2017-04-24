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

#include <dali/graphics/vulkan/command-pool.h>
#include <dali/graphics/vulkan/physical-device.h>
#include <dali/graphics/vulkan/logical-device.h>
#include <dali/graphics/vulkan/common.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{


class CommandPoolImpl : public VkObject
{
public:
  CommandPoolImpl(const LogicalDevice& device, QueueType type, bool isExclusive, bool createTransient,
  bool createResetCommandBuffer)
  : VkObject(),
    mDevice( device ), mQueueType( type ), mExclusive( isExclusive ),
    mResetFlag( createResetCommandBuffer ), mTransientFlag( createTransient )
  {
  }

  virtual ~CommandPoolImpl() {};

  bool Initialise();

  const LogicalDevice& GetLogicalDevice() const
  {
    return mDevice;
  }

  // vk objects getters
public:

  const vk::CommandPool GetCommandPool() const
  {
    return mCommandPool;
  }

  const vk::CommandPoolCreateInfo& GetCommandPoolCreateInfo() const
  {
    return mCreateInfo;
  }

  bool IsExclusive() const
  {
    return mExclusive;
  }

  bool IsTransient() const
  {
    return mTransientFlag;
  }

  bool IsResetFlag() const
  {
    return mResetFlag;
  }

  QueueType GetQueueType() const
  {
    return mQueueType;
  }

  std::vector< CommandBuffer > AllocateCommandBuffers(int count, bool primary);

private:

  LogicalDevice mDevice { nullptr };

  // vulkan specific
  vk::CommandPool           mCommandPool{nullptr};
  vk::CommandPoolCreateInfo mCreateInfo{};


  std::thread::id           mThreadId; // used if exclusive

  // queue properties
  bool mExclusive{false};
  bool mResetFlag{false};
  bool mTransientFlag{false};

  QueueType mQueueType;
};

bool CommandPoolImpl::Initialise()
{
  auto vkDevice = mDevice.GetVkDevice();
  auto phDevice = mDevice.GetPhysicalDevice();

  vk::CommandPoolCreateFlags flags{};
  if( mResetFlag )
    flags |= vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
  if( mTransientFlag )
    flags |= vk::CommandPoolCreateFlagBits::eTransient;

  vk::CommandPoolCreateInfo info;
  info.setQueueFamilyIndex( phDevice.GetQueueFamilyIndex( mQueueType) );
  info.flags = flags;
  mCreateInfo = info;

  VkAssertCall( vkDevice.createCommandPool( &info, mDevice.GetVkAllocator(), &mCommandPool ) );

  VkLog("[VULKAN] CommandPoolImpl::Initialise() created!");

  return true;
}

std::vector< CommandBuffer > CommandPoolImpl::AllocateCommandBuffers(int count, bool primary)
{
  return std::move(CommandBuffer::New( CommandPool(this), primary, count ));
}

CommandPool CommandPool::New(const LogicalDevice& device, QueueType type, bool isExclusive,
                                         bool createTransient, bool createResetCommandBuffer)
{
  auto impl = new CommandPoolImpl(device, type, isExclusive, createTransient, createResetCommandBuffer);
  CommandPool retval( impl );
  if(!impl->Initialise())
    retval = nullptr;
  return retval;
}


// Implementation getter
namespace
{
CommandPoolImpl* GetImpl( CommandPool* pool )
{
  return static_cast<CommandPoolImpl*>(pool->GetObject());
}
CommandPoolImpl* GetImpl( const CommandPool* pool )
{
  return static_cast<CommandPoolImpl*>(pool->GetObject());
}
}
bool CommandPool::Initialise()
{
  return GetImpl(this)->Initialise();
}

const vk::CommandPool CommandPool::GetCommandPool() const
{
  return GetImpl(this)->GetCommandPool();
}

const vk::CommandPoolCreateInfo& CommandPool::GetVkCommandPoolCreateInfo() const
{
  return GetImpl(this)->GetCommandPoolCreateInfo();
}

const LogicalDevice& CommandPool::GetLogicalDevice() const
{
  return GetImpl(this)->GetLogicalDevice();
}

std::thread::id CommandPool::GetThreadId() const
{
  //return GetImplementation<CommandPoolImpl>(this)->mThreadId;
}

void CommandPool::ThreadAttach()
{

}

void CommandPool::ThreadDetach()
{

}


std::vector< CommandBuffer > CommandPool::AllocateCommandBuffers(int count, bool primary)
{
  return std::move(GetImpl(this)->AllocateCommandBuffers( count, primary ));
}

CommandBuffer CommandPool::AllocateCommandBuffer(bool primary)
{
  return GetImpl(this)->AllocateCommandBuffers( 1, primary )[0];
}


}
}
}
