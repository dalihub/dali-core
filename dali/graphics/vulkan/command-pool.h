#ifndef DALI_CORE_VULKAN_COMMAND_POOL_H
#define DALI_CORE_VULKAN_COMMAND_POOL_H

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
#include <dali/graphics/vulkan/command-buffer.h>
#include <thread>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class LogicalDevice;

//using VulkanCommandBufferArray = std::vector< VulkanCommandBuffer >;

class CommandPool : public VkHandle
{
public:
  CommandPool(VkObject* o = nullptr) : VkHandle(o){};
  using VkHandle::operator=;

  static CommandPool New(const LogicalDevice& context, QueueType type, bool isExclusive,
                         bool createTransient, bool createResetCommandBuffer);

  bool Initialise();

  // attaching to thread will make sure that any attempt of using pool on the
  // wrong thread will assert
  void ThreadAttach();
  void ThreadDetach();

  const vk::CommandPool            GetCommandPool() const;
  const LogicalDevice&             GetLogicalDevice() const;
  const vk::CommandPoolCreateInfo& GetVkCommandPoolCreateInfo() const;
  std::thread::id                  GetThreadId() const;

  std::vector< CommandBuffer > AllocateCommandBuffers(uint32_t count, bool primary);
  CommandBuffer AllocateCommandBuffer(bool primary);
};
}
}
}

#endif //DALI_CORE_VULKANCOMMANDPOOL_H
