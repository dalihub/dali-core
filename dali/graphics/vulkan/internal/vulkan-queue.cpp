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
#include <dali/graphics/vulkan/internal/vulkan-command-buffer.h>
#include <dali/graphics/vulkan/internal/vulkan-fence.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/internal/vulkan-queue.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

// submission
SubmissionData::SubmissionData( const std::vector< vk::Semaphore >& waitSemaphores_,
                                vk::PipelineStageFlags waitDestinationStageMask_,
                                const std::vector< RefCountedCommandBuffer >& commandBuffers_,
                                const std::vector< vk::Semaphore >& signalSemaphores_ )
        : waitSemaphores( waitSemaphores_ ),
          waitDestinationStageMask( waitDestinationStageMask_ ),
          commandBuffers( commandBuffers_ ),
          signalSemaphores( signalSemaphores_ )
{
}

SubmissionData& SubmissionData::SetWaitSemaphores( const std::vector< vk::Semaphore >& semaphores )
{
  waitSemaphores = semaphores;
  return *this;
}

SubmissionData& SubmissionData::SetWaitDestinationStageMask( vk::PipelineStageFlags dstStageMask )
{
  waitDestinationStageMask = dstStageMask;
  return *this;
}

SubmissionData& SubmissionData::SetCommandBuffers( const std::vector< RefCountedCommandBuffer >& cmdBuffers )
{
  commandBuffers = cmdBuffers;
  return *this;
}

SubmissionData& SubmissionData::SetSignalSemaphores( const std::vector< vk::Semaphore >& semaphores )
{
  signalSemaphores = semaphores;
  return *this;
}

// queue
Queue::Queue( Graphics& graphics,
              vk::Queue queue,
              uint32_t queueFamilyIndex,
              uint32_t queueIndex,
              vk::QueueFlags queueFlags )
        : mGraphics( graphics ),
          mQueue( queue ),
          mFlags( queueFlags ),
          mQueueFamilyIndex( queueFamilyIndex ),
          mQueueIndex( queueIndex )
{
}

Queue::~Queue() = default; // queues are non-destructible

vk::Queue Queue::GetVkHandle()
{
  return mQueue;
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
