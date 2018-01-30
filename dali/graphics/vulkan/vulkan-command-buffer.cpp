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
#include <dali/graphics/vulkan/vulkan-command-buffer.h>
#include <dali/graphics/vulkan/vulkan-command-pool.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

struct CommandBuffer::Impl
{
  Impl( CommandPool& commandPool, const vk::CommandBufferAllocateInfo& allocateInfo, vk::CommandBuffer commandBuffer )
  : mGraphics( commandPool.GetGraphics() ),
    mOwnerCommandPool( commandPool ),
    mAllocateInfo( allocateInfo ),
    mCommandBuffer( commandBuffer )
  {

  }

  ~Impl()
  {
  }

  bool Initialise()
  {
    return true;
  }

  /**
   *
   */
  void Begin(vk::CommandBufferUsageFlags usageFlags, vk::CommandBufferInheritanceInfo* inheritanceInfo)
  {
    assert(!mRecording && "CommandBuffer already is in the recording state");
    auto info = vk::CommandBufferBeginInfo{};
    info.setPInheritanceInfo(inheritanceInfo);
    info.setFlags(usageFlags);
    VkAssert(mCommandBuffer.begin(info));
    mRecording = true;
  }

  void End()
  {
    assert(mRecording && "CommandBuffer is not in the recording state!");
    VkAssert(mCommandBuffer.end());
    mRecording = false;
  }

  void Reset()
  {
    assert(!mRecording && "Can't reset command buffer during recording!");
    assert(mCommandBuffer && "Invalid command buffer!");
    mCommandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);
  }

  void Free()
  {
    assert(mCommandBuffer && "Invalid command buffer!");
    mGraphics.GetDevice().freeCommandBuffers(mOwnerCommandPool.GetPool(), mCommandBuffer);
  }

  void ImageLayoutTransition(vk::Image            image,
                             vk::ImageLayout      oldLayout,
                             vk::ImageLayout      newLayout,
                             vk::ImageAspectFlags aspectMask)
  {
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
        srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eColorAttachmentRead;
      }
      break;
      case vk::ImageLayout::eColorAttachmentOptimal:
      {
        srcStageMask = vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eColorAttachmentOutput;
        srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eColorAttachmentRead;
      }
      break;
      case vk::ImageLayout::eGeneral:
      case vk::ImageLayout::eDepthStencilAttachmentOptimal:
      case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
      case vk::ImageLayout::eShaderReadOnlyOptimal:
      case vk::ImageLayout::eTransferSrcOptimal:
      case vk::ImageLayout::eTransferDstOptimal:
      case vk::ImageLayout::ePreinitialized:
      case vk::ImageLayout::eSharedPresentKHR:
      {
      }
      }

      switch(newLayout)
      {
      case vk::ImageLayout::eColorAttachmentOptimal:
      {
        dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eFragmentShader;
        dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eHostWrite;
        break;
      }
      case vk::ImageLayout::eDepthStencilAttachmentOptimal:
      {
        dstStageMask = vk::PipelineStageFlagBits::eFragmentShader | vk::PipelineStageFlagBits::eEarlyFragmentTests;
        dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        break;
      }
      case vk::ImageLayout::ePresentSrcKHR:
      {
        dstStageMask  = vk::PipelineStageFlagBits::eBottomOfPipe;
        dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eMemoryRead;
      }
      case vk::ImageLayout::eGeneral:
      case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
      case vk::ImageLayout::eShaderReadOnlyOptimal:
      case vk::ImageLayout::eTransferSrcOptimal:
      case vk::ImageLayout::eTransferDstOptimal:
      case vk::ImageLayout::ePreinitialized:
      case vk::ImageLayout::eUndefined:
      case vk::ImageLayout::eSharedPresentKHR:
      {
        break;
      }

    }

    RecordImageLayoutTransition(image, srcAccessMask, dstAccessMask, srcStageMask, dstStageMask,
                                oldLayout, newLayout, aspectMask);
  }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wframe-larger-than="
  void RecordImageLayoutTransition(vk::Image image, vk::AccessFlags srcAccessMask,
                                   vk::AccessFlags dstAccessMask, vk::PipelineStageFlags srcStageMask,
                                   vk::PipelineStageFlags dstStageMask, vk::ImageLayout oldLayout,
                                   vk::ImageLayout newLayout, vk::ImageAspectFlags aspectMask)
  {
    vk::ImageSubresourceRange subres;
    subres.setLayerCount(1).setBaseMipLevel(0).setBaseArrayLayer(0).setLevelCount(1).setAspectMask(aspectMask);


    auto barrier = vk::ImageMemoryBarrier{};
    barrier
      .setImage(image)
      .setSubresourceRange(subres)
      .setSrcAccessMask(srcAccessMask)
      .setDstAccessMask(dstAccessMask)
      .setOldLayout(oldLayout)
      .setNewLayout(newLayout);
    ;
    // todo: implement barriers batching
    mCommandBuffer.pipelineBarrier(srcStageMask, dstStageMask, vk::DependencyFlags{}, nullptr, nullptr, barrier);
  }
#pragma GCC diagnostic pop

  /** Push wait semaphores */
  void PushWaitSemaphores(const std::vector< vk::Semaphore >&          semaphores,
                          const std::vector< vk::PipelineStageFlags >& stages)
  {
    mWaitSemaphores = semaphores;
    mWaitStages     = stages;
  }

  /** Push signal semaphores */
  void PushSignalSemaphores(const std::vector< vk::Semaphore >& semaphores)
  {
    mSignalSemaphores = semaphores;
  }


  Graphics&                       mGraphics;
  CommandPool&                    mOwnerCommandPool;
  vk::CommandBufferAllocateInfo   mAllocateInfo {};

  vk::CommandBuffer     mCommandBuffer {};

  // semaphores per command buffer
  std::vector< vk::Semaphore >          mSignalSemaphores {};
  std::vector< vk::Semaphore >          mWaitSemaphores {};
  std::vector< vk::PipelineStageFlags > mWaitStages {};

  bool mRecording { false };
};

/**
 *
 * Class: CommandBuffer
 *
 */

CommandBuffer::CommandBuffer( CommandPool& commandPool, const vk::CommandBufferAllocateInfo& allocateInfo, vk::CommandBuffer vkCommandBuffer )
{
  mImpl = MakeUnique<Impl>( commandPool, allocateInfo, vkCommandBuffer );
}

CommandBuffer::~CommandBuffer()
{
}

/** Begin recording */
void CommandBuffer::Begin(vk::CommandBufferUsageFlags usageFlags, vk::CommandBufferInheritanceInfo* inheritanceInfo)
{
  mImpl->Begin( usageFlags, inheritanceInfo );
}

/** Finish recording */
void CommandBuffer::End()
{
  mImpl->End();
}

/** Reset command buffer */
void CommandBuffer::Reset()
{
  mImpl->Reset();
}

/** Free command buffer */
void CommandBuffer::Free()
{
  mImpl->Free();
}

/** Records image layout transition barrier for one image */
void CommandBuffer::ImageLayoutTransition(vk::Image            image,
                                          vk::ImageLayout      oldLayout,
                                          vk::ImageLayout      newLayout,
                                          vk::ImageAspectFlags aspectMask)
{
  mImpl->ImageLayoutTransition( image, oldLayout, newLayout, aspectMask );
}

void CommandBuffer::RecordImageLayoutTransition(vk::Image image, vk::AccessFlags srcAccessMask,
                                                vk::AccessFlags dstAccessMask, vk::PipelineStageFlags srcStageMask,
                                                vk::PipelineStageFlags dstStageMask, vk::ImageLayout oldLayout,
                                                vk::ImageLayout newLayout, vk::ImageAspectFlags aspectMask)
{
  mImpl->RecordImageLayoutTransition( image, srcAccessMask, dstAccessMask,srcStageMask, dstStageMask, oldLayout, newLayout, aspectMask );
}

/** Push wait semaphores */
void CommandBuffer::PushWaitSemaphores(const std::vector< vk::Semaphore >&          semaphores,
                                       const std::vector< vk::PipelineStageFlags >& stages)
{
  mImpl->PushWaitSemaphores( semaphores, stages );
}

/** Push signal semaphores */
void CommandBuffer::PushSignalSemaphores(const std::vector< vk::Semaphore >& semaphores)
{
  mImpl->PushSignalSemaphores( semaphores );
}

const std::vector< vk::Semaphore >& CommandBuffer::GetSignalSemaphores() const
{
  return mImpl->mSignalSemaphores;
}

const std::vector< vk::Semaphore >& CommandBuffer::GetSWaitSemaphores() const
{
  return mImpl->mWaitSemaphores;
}

const std::vector< vk::PipelineStageFlags >& CommandBuffer::GetWaitSemaphoreStages() const
{
  return mImpl->mWaitStages;
}

vk::CommandBuffer CommandBuffer::GetVkCommandBuffer() const
{
  return mImpl->mCommandBuffer;
}


} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
