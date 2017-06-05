#ifndef DALI_GRAPHICS_VULKAN_QUEUE_H
#define DALI_GRAPHICS_VULKAN_QUEUE_H

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
#include <dali/graphics/vulkan/types.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

class Fence;
class Submission
{
public:
  Submission(Fence& fence);

  bool WaitForFence(uint32_t timeout = 0u);

private:
  // todo: possibly resources locks stored here?
  FenceRef mFences;
};

class Queue
{
public:
  Queue() = delete;
  Queue(Graphics& graphics, vk::Queue queue, uint32_t queueFamilyIndex, uint32_t queueIndex,
        vk::QueueFlags queueFlags);
  ~Queue(); // queues are non-destructible

  /** Submits command buffers */
  std::unique_ptr< Submission > Submit(const std::vector< CommandBufferRef >& commandBuffers,
                                       Fence&                                 fence);

  /** Helper function to submit single command buffer */
  std::unique_ptr< Submission > Submit(CommandBuffer& commandBuffer, Fence& fence);

  void WaitIdle() const;

  /**
   *
   * @param presentInfo
   * @return
   */
  vk::Result Present( const vk::PresentInfoKHR& presentInfo );

  /**
   *
   * @param swapchain
   * @return
   */
  vk::Result Present( vk::SwapchainKHR swapchain, uint32_t imageIndex );

private:
  /** Prepares command buffers for submission */
  std::vector< vk::CommandBuffer > PrepareBuffers(
      const std::vector< CommandBufferRef >& commandBuffers) const;



private:
  Graphics&                mGraphics;
  vk::Queue                mQueue;
  vk::QueueFlags           mFlags;
  std::vector< vk::Fence > mFences;
  uint32_t                 mQueueFamilyIndex;
  uint32_t                 mQueueIndex;
};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_QUEUE_H
