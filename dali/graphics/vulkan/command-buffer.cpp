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

#include <dali/graphics/vulkan/command-buffer.h>
#include <dali/graphics/vulkan/command-pool.h>
#include <dali/graphics/vulkan/logical-device.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
using State = CommandBufferState;
class CommandBufferImpl : public VkObject
{
public:
  CommandBufferImpl(const LogicalDevice& context, const CommandPool& pool, vk::CommandBufferLevel level, vk::CommandBuffer buffer)
  : VkObject(), mPool(pool), mDevice(context), mBuffer(buffer), mLevel(level)
  {
  }

  bool OnSafeDelete() override
  {
    // mind that the resources may not be released if the pool doesn't allow
    // freeing a single command buffer! Synchronization is explicit
    if(mPool)
    {
      mDevice.GetVkDevice().freeCommandBuffers(mPool.GetCommandPool(), 1, &mBuffer);
      return true;
    }
    else
    {
      return false;
    }
  }

  const vk::CommandBuffer& GetVkCommandBuffer() const
  {
    return mBuffer;
  }

  const CommandPool& GetCommandPool() const
  {
    return mPool;
  }

  State GetState() const
  {
    return mState;
  }

  bool Begin( bool oneTimeSubmit, bool renderPassContinue, bool simultaneousUse );
  bool End();
  bool Free();
  bool Reset();

private:
  CommandPool            mPool;
  LogicalDevice          mDevice;
  vk::CommandBuffer      mBuffer;
  vk::CommandBufferLevel mLevel;

  // state of command buffer
  State mState{State::UNDEFINED};
};

bool CommandBufferImpl::Begin( bool oneTimeSubmit, bool renderPassContinue, bool simultaneousUse )
{
  // check state
  if(mState != State::CREATED && mState != State::UNDEFINED && mState != State::RESET)
  {
    VkLog("[VKCMDBUF] Invalid buffer state: %d", static_cast< int >(mState));
    return false;
  }

  vk::CommandBufferUsageFlags flags{};
  if(oneTimeSubmit)
    flags |= vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
  if(renderPassContinue)
    flags |= vk::CommandBufferUsageFlagBits::eRenderPassContinue;
  if(simultaneousUse)
    flags |= vk::CommandBufferUsageFlagBits::eSimultaneousUse;

  // todo implement inheritance
  vk::CommandBufferBeginInfo info;
  info.setFlags(flags).setPInheritanceInfo(nullptr);

  if(VkTestCall(mBuffer.begin(&info)) == vk::Result::eSuccess)
  {
    mState = State::RECORDING;
    return true;
  }

  return false;
}

bool CommandBufferImpl::End()
{
  // check state
  if(mState != State::RECORDING)
  {
    VkLog("[VKCMDBUF] Invalid buffer state: %d, it must be RECORDING", static_cast< int >(mState));
    return false;
  }

  if(VkTestCall(mBuffer.end()) == vk::Result::eSuccess)
  {
    mState = State::RECORDED;
    return true;
  }
  return false;
}

bool CommandBufferImpl::Free()
{
  // todo
  assert( false && "CommandBufferImpl::Free() unimplemented!");
}

bool CommandBufferImpl::Reset()
{
  if(vk::Result::eSuccess == VkTestCall(mBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources)))
  {
    mState = State::RESET;
  }
  return true;
}

// Implementation getter
namespace
{
CommandBufferImpl* GetImpl( CommandBuffer* handle )
{
  return static_cast<CommandBufferImpl*>(handle->GetObject());
}
CommandBufferImpl* GetImpl( const CommandBuffer* handle )
{
  return static_cast<CommandBufferImpl*>(handle->GetObject());
}
}

const vk::CommandBuffer* CommandBuffer::operator->() const
{
  auto& vkcmdbuf = GetImpl(this)->GetVkCommandBuffer();
  return &vkcmdbuf;
}

std::vector<CommandBuffer> CommandBuffer::New(const CommandPool& pool, bool isPrimary, int count)
{
  auto vkDevice = pool.GetLogicalDevice().GetVkDevice();

  std::vector< vk::CommandBuffer > buffers;
  buffers.resize(count);

  auto bufferLevel = isPrimary ? vk::CommandBufferLevel::ePrimary : vk::CommandBufferLevel::eSecondary;

  vk::CommandBufferAllocateInfo info;
  info.setCommandBufferCount(count)
      .setCommandPool(pool.GetCommandPool())
      .setLevel(bufferLevel);

  VkAssertCall(vkDevice.allocateCommandBuffers(&info, buffers.data()));

  // move buffers data
  std::vector<CommandBuffer> output;

  for(auto& cmdbuf : buffers)
  {
    CommandBuffer tmp( new CommandBufferImpl( pool.GetLogicalDevice(), pool, bufferLevel, cmdbuf ) );
    output.emplace_back(tmp);
  }
  return std::move(output);
}

CommandBuffer CommandBuffer::New(const CommandPool& pool, bool isPrimary)
{
  // todo make more efficient implementation
  return CommandBuffer::New(pool, isPrimary, 1)[0];
}

bool CommandBuffer::Begin(bool oneTimeSubmit, bool renderPassContinue, bool simultaneousUse)
{
  return GetImpl(this)->Begin( oneTimeSubmit, renderPassContinue, simultaneousUse );
}

bool CommandBuffer::End()
{
  return GetImpl(this)->End();
}

bool CommandBuffer::Free()
{
  return GetImpl(this)->Free();
}

bool CommandBuffer::Reset()
{
  return GetImpl(this)->Reset();
}

CommandBufferState CommandBuffer::GetState() const
{
  return GetImpl(this)->GetState();
}

vk::CommandBuffer CommandBuffer::GetVkBuffer() const
{
  return GetImpl(this)->GetVkCommandBuffer();
}

const CommandPool& CommandBuffer::GetCommandPool() const
{
  return GetImpl(this)->GetCommandPool();
}

std::thread::id CommandBuffer::GetThreadId() const
{
  return std::thread::id{};
}
}
}
}
