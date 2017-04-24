#ifndef DALI_CORE_GRAPHICS_VULKAN_COMMAND_QUEUE_H
#define DALI_CORE_GRAPHICS_VULKAN_COMMAND_QUEUE_H

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

#include <dali/graphics/vulkan/common.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class CommandBuffer;
class LogicalDevice;

class CommandQueue : public VkHandle
{
public:
  CommandQueue(VkObject* o = nullptr) : VkHandle(o){};
  using VkHandle::operator=;

  static CommandQueue Get(const LogicalDevice& device, QueueType type, int queueIndex);

  vk::Queue GetVkQueue() const;
  int       GetIndex() const;
  QueueType GetType() const;

  bool Submit(const CommandBuffer* buffers, uint32_t count, vk::Fence fence);
  bool Submit(const std::vector< CommandBuffer >& buffers, vk::Fence fence);
  bool Submit(const CommandBuffer* pBuffers, const uint32_t bufferCount,
              const vk::Semaphore* pWaitSemaphores, const uint32_t waitSemaphoreCount,
              const vk::Semaphore* pSignalSemaphores, const uint32_t signalSemaphoreCount,
              const vk::PipelineStageFlags* pWaitDstStageFlags, vk::Fence fence) const;

  void WaitIdle() const;
};
}
}
}

#endif // DALI_CORE_GRAPHICS_VULKAN_COMMAND_QUEUE_H
