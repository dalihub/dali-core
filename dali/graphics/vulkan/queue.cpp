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
#include <dali/graphics/vulkan/fence.h>
#include <dali/graphics/vulkan/graphics.h>
#include <dali/graphics/vulkan/queue.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

namespace
{
// this class is non-copyable, just movable
struct PrepareSemaphoresData
{
  PrepareSemaphoresData()                             = default;
  PrepareSemaphoresData(const PrepareSemaphoresData&) = delete;
  PrepareSemaphoresData& operator=(const PrepareSemaphoresData&) = delete;
  PrepareSemaphoresData(PrepareSemaphoresData&&)                 = default;
  PrepareSemaphoresData& operator=(PrepareSemaphoresData&&) = default;

  std::vector< vk::Semaphore >          signalSemaphores;
  std::vector< vk::Semaphore >          waitSemaphores;
  std::vector< vk::PipelineStageFlags > waitDstStageMasks;
};

// helper function converting size_t to required uint32_t
template< typename T >
inline uint32_t u32(T val)
{
  return static_cast< uint32_t >(val);
}

inline PrepareSemaphoresData PrepareSemaphores(const std::vector< CommandBufferRef >& commandBuffers)
{
  PrepareSemaphoresData retval{};
  for(auto& cmdbufref : commandBuffers)
  {
    auto& cmdbuf = cmdbufref.get();
    if(!retval.signalSemaphores.empty())
    {
      retval.signalSemaphores.insert(retval.signalSemaphores.end(),
                                     cmdbuf.GetSignalSemaphores().begin(),
                                     cmdbuf.GetSignalSemaphores().end());
    }
    if(!retval.waitSemaphores.empty())
    {
      retval.waitSemaphores.insert(retval.waitSemaphores.end(), cmdbuf.GetSWaitSemaphores().begin(),
                                   cmdbuf.GetSWaitSemaphores().end());
      retval.waitDstStageMasks.insert(retval.waitDstStageMasks.end(),
                                      cmdbuf.GetWaitSemaphoreStages().begin(),
                                      cmdbuf.GetWaitSemaphoreStages().end());
    }
  }
  return std::move(retval);
}
}

// submission
Submission::Submission(Fence& fence) : mFences(fence)
{
}

bool Submission::WaitForFence(uint32_t timeout)
{
  return mFences.get().Wait(timeout);
}

// queue
Queue::Queue(Graphics& graphics, vk::Queue queue, uint32_t queueFamilyIndex, uint32_t queueIndex,
             vk::QueueFlags queueFlags)
: mGraphics(graphics), mQueue(queue), mFlags(queueFlags), mQueueFamilyIndex(queueFamilyIndex), mQueueIndex(queueIndex)
{
}

Queue::~Queue() // queues are non-destructible
{
}

std::unique_ptr< Submission > Queue::Submit(CommandBuffer& commandBuffer, Fence& fence)
{
  auto buffers = std::vector< CommandBufferRef >({commandBuffer});
  return Submit(buffers, fence);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wframe-larger-than="
std::unique_ptr< Submission > Queue::Submit(const std::vector< CommandBufferRef >& commandBuffers, Fence& fence)
{
  // Prepare command buffers for submission
  auto buffers = PrepareBuffers(commandBuffers);

  auto semaphores = PrepareSemaphores(commandBuffers);

  auto info = vk::SubmitInfo{};

  /* semaphores per command buffer */
  info.setCommandBufferCount(u32(commandBuffers.size()));
  info.setPCommandBuffers(buffers.data());
  info.setSignalSemaphoreCount(u32(semaphores.signalSemaphores.size()));
  info.setPSignalSemaphores(semaphores.signalSemaphores.data());
  info.setWaitSemaphoreCount(u32(semaphores.waitSemaphores.size()));
  info.setPWaitSemaphores(semaphores.waitSemaphores.data());
  info.setPWaitDstStageMask(semaphores.waitDstStageMasks.data());

  VkAssert(mQueue.submit(1, &info, fence.GetFence()));

  return MakeUnique< Submission >(fence);
}
#pragma GCC diagnostic pop

std::vector< vk::CommandBuffer > Queue::PrepareBuffers(const std::vector< CommandBufferRef >& commandBuffers) const
{
  std::vector< vk::CommandBuffer > retval(commandBuffers.size());
  for(uint32_t i = 0; i < commandBuffers.size(); ++i)
  {
    retval[i] = commandBuffers[i].get().Get();
  }
  return retval;
}

void Queue::WaitIdle() const
{
  assert(mQueue && "Queue isn't initialised!");
  mQueue.waitIdle();
}

vk::Result Queue::Present(const vk::PresentInfoKHR& presentInfo)
{
  return VkTest(mQueue.presentKHR(presentInfo));
}

vk::Result Queue::Present(vk::SwapchainKHR swapchain, uint32_t imageIndex)
{
  auto info = vk::PresentInfoKHR{}
                  .setWaitSemaphoreCount(0)
                  .setPWaitSemaphores(nullptr)
                  .setPResults(nullptr)
                  .setPImageIndices(&imageIndex)
                  .setPSwapchains(&swapchain)
                  .setSwapchainCount(1);

  return Present(info);
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
