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
#include <dali/graphics/vulkan/command-buffer.h>
#include <dali/graphics/vulkan/command-pool.h>
#include <dali/graphics/vulkan/graphics.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

CommandBuffer::CommandBuffer(Graphics& graphics, CommandPool& ownerPool)
: CommandBuffer(graphics, ownerPool,
                vk::CommandBufferAllocateInfo().setCommandBufferCount(1).setLevel(
                    vk::CommandBufferLevel::ePrimary))
{
}

CommandBuffer::CommandBuffer(Graphics& graphics, CommandPool& ownerPool,
                             const vk::CommandBufferAllocateInfo& allocateInfo)
: mGraphics(graphics), mCommandPool(ownerPool), mRecording(false)
{
  assert(allocateInfo.commandBufferCount == 1 && "Number of buffers to allocate must be equal 1!");
  mCommandBuffer = VkAssert(mGraphics.GetDevice().allocateCommandBuffers(allocateInfo))[0];
}

CommandBuffer::~CommandBuffer()
{
  if(mCommandBuffer)
  {
    mGraphics.GetDevice().freeCommandBuffers(mCommandPool.GetPool(), mCommandBuffer);
  }
}

/** Begin recording */
void CommandBuffer::Begin(vk::CommandBufferUsageFlags       usageFlags,
                          vk::CommandBufferInheritanceInfo* inheritanceInfo)
{
  assert(!mRecording && "CommandBuffer already is in the recording state");
  auto info = vk::CommandBufferBeginInfo{};
  info.setPInheritanceInfo(inheritanceInfo);
  info.setFlags(usageFlags);
  VkAssert(mCommandBuffer.begin(info));
  mRecording = true;
}

/** Finish recording */
void CommandBuffer::End()
{
  assert(mRecording && "CommandBuffer is not in the recording state!");
  VkAssert(mCommandBuffer.end());
  mRecording = false;
}

/** Reset command buffer */
void CommandBuffer::Reset()
{
  assert(!mRecording && "Can't reset command buffer during recording!");
  assert(mCommandBuffer && "Invalid command buffer!");
  mCommandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
}

/** Free command buffer */
void CommandBuffer::Free()
{
  assert(mCommandBuffer && "Invalid command buffer!");
  mGraphics.GetDevice().freeCommandBuffers(mCommandPool.GetPool(), mCommandBuffer);
}

/** Records image layout transition barrier for one image */
void CommandBuffer::ImageLayoutTransition(vk::Image            image,
                                          vk::ImageLayout      oldLayout,
                                          vk::ImageLayout      newLayout,
                                          vk::ImageAspectFlags aspectMask)
{
  // must be in recording state

  vk::ImageSubresourceRange subres;
  subres.setLayerCount(1).setBaseMipLevel(0).setBaseArrayLayer(0).setLevelCount(1).setAspectMask(
      aspectMask);

  // just push new image barrier until any command is being called or buffer recording ends.
  // it will make sure we batch barriers together rather than calling cmdPipelineBarrier
  // for each separately
  vk::AccessFlags        srcAccessMask, dstAccessMask;
  vk::PipelineStageFlags srcStageMask, dstStageMask;

  // TODO: add other transitions
  switch(oldLayout)
  {
  case vk::ImageLayout::eUndefined:
  {
    srcStageMask = vk::PipelineStageFlagBits::eTopOfPipe;
  }
  break;
  case vk::ImageLayout::ePresentSrcKHR:
  {
    srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
    srcAccessMask =
        vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eColorAttachmentRead;
  }
  case vk::ImageLayout::eColorAttachmentOptimal:
  {
    srcStageMask = vk::PipelineStageFlagBits::eFragmentShader |
                   vk::PipelineStageFlagBits::eColorAttachmentOutput;
    srcAccessMask =
        vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eColorAttachmentRead;
  }
  break;
  default:
  {
  }
  }

  switch(newLayout)
  {
  case vk::ImageLayout::eColorAttachmentOptimal:
  {
    dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput |
                   vk::PipelineStageFlagBits::eFragmentShader;
    dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eHostWrite;
    break;
  }
  case vk::ImageLayout::eDepthStencilAttachmentOptimal:
  {
    dstStageMask =
        vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eEarlyFragmentTests;
    dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead |
                    vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    break;
  }
  case vk::ImageLayout::ePresentSrcKHR:
  {
    dstStageMask  = vk::PipelineStageFlagBits::eBottomOfPipe;
    dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eMemoryRead;
  }
  default:
  {
  }
  }

  vk::ImageMemoryBarrier barrier;
  barrier.setImage(image)
      .setSubresourceRange(subres)
      .setSrcAccessMask(srcAccessMask)
      .setDstAccessMask(dstAccessMask)
      .setOldLayout(oldLayout)
      .setNewLayout(newLayout);

  // todo: implement barriers batching
  mCommandBuffer.pipelineBarrier(srcStageMask, dstStageMask, vk::DependencyFlags{}, nullptr,
                                 nullptr, barrier);
}

/** Push wait semaphores */
void CommandBuffer::PushWaitSemaphores(const std::vector< vk::Semaphore >&          semaphores,
                                       const std::vector< vk::PipelineStageFlags >& stages)
{
  mWaitSemaphores = semaphores;
  mWaitStages     = stages;
}

/** Push signal semaphores */
void CommandBuffer::PushSignalSemaphores(const std::vector< vk::Semaphore >& semaphores)
{
  mSignalSemaphores = semaphores;
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
