#ifndef DALI_CORE_GRAPHICS_VULKAN_COMMAND_BUFFER_H
#define DALI_CORE_GRAPHICS_VULKAN_COMMAND_BUFFER_H

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
#include <thread>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

enum class CommandBufferState
{
  UNDEFINED,
  CREATED,
  RESET,
  RECORDING,
  RECORDED,
  SUBMITTED
};

class CommandPool;
class CommandBuffer : public VkHandle
{
public:
  CommandBuffer(VkObject* object = nullptr) : VkHandle(object)
  {
  }

  using VkHandle::operator=;

  static std::vector< CommandBuffer > New(const CommandPool& pool, bool isPrimary, int count);
  static CommandBuffer New(const CommandPool& pool, bool isPrimary);

  vk::CommandBuffer  GetVkBuffer() const;
  const CommandPool& GetCommandPool() const;
  std::thread::id    GetThreadId() const;

  CommandBufferState GetCommandBufferState() const;

  // operator -> allows to record commands directly into Vulkan command buffer
  const vk::CommandBuffer* operator->() const;

  bool Begin(bool oneTimeSubmit, bool renderPassContinue = false, bool simultaneousUse = false);
  bool End();
  bool Free();
  bool Reset();

  CommandBufferState GetState() const;
};
}
}
}

#endif // DALI_CORE_GRAPHICS_VULKAN_COMMAND_BUFFER_H
