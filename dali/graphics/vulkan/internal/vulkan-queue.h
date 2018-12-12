#ifndef DALI_GRAPHICS_VULKAN_QUEUE
#define DALI_GRAPHICS_VULKAN_QUEUE

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
#include <dali/graphics/vulkan/internal/vulkan-types.h>

#include <mutex>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

class Fence;

struct SubmissionData
{
  SubmissionData() = default;

  explicit SubmissionData( const std::vector< vk::Semaphore >& waitSemaphores_,
                           vk::PipelineStageFlags waitDestinationStageMask_,
                           const std::vector< RefCountedCommandBuffer >& commandBuffers_,
                           const std::vector< vk::Semaphore >& signalSemaphores_ );

  SubmissionData& SetWaitSemaphores( const std::vector< vk::Semaphore >& semaphores );

  SubmissionData& SetWaitDestinationStageMask( vk::PipelineStageFlags dstStageMask );

  SubmissionData& SetCommandBuffers( const std::vector< RefCountedCommandBuffer >& cmdBuffers );

  SubmissionData& SetSignalSemaphores( const std::vector< vk::Semaphore >& semaphores );

  std::vector< vk::Semaphore > waitSemaphores;
  vk::PipelineStageFlags waitDestinationStageMask;
  std::vector< RefCountedCommandBuffer > commandBuffers;
  std::vector< vk::Semaphore > signalSemaphores;
};

class Graphics;

class Queue
{
  friend class Graphics;

public:

  ~Queue(); // queues are non-destructible

  vk::Queue GetVkHandle();

  std::unique_ptr<std::lock_guard<std::recursive_mutex>> Lock();

private:
  Queue( Graphics& graphics,
         vk::Queue queue,
         uint32_t queueFamilyIndex,
         uint32_t queueIndex,
         vk::QueueFlags queueFlags );

private:
  Graphics& mGraphics;
  vk::Queue mQueue;
  vk::QueueFlags mFlags;
  uint32_t mQueueFamilyIndex;
  uint32_t mQueueIndex;

  std::recursive_mutex mMutex;
};

} // namespace Vulkan

} // namespace Graphics

} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_QUEUE
