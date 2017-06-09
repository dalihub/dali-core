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
#include <dali/graphics/vulkan/vulkan-command-pool.h>
#include <dali/graphics/vulkan/vulkan-command-buffer.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

CommandPool::CommandPool(Graphics& graphics, const vk::CommandPoolCreateInfo& createInfo)
: mGraphics(graphics)
{
  mPool = VkAssert(graphics.GetDevice().createCommandPool(createInfo, graphics.GetAllocator()));
}

CommandPool::~CommandPool()
{
  if(mPool)
  {
    mGraphics.GetDevice().destroyCommandPool(mPool, mGraphics.GetAllocator());
  }
}

std::unique_ptr< CommandBuffer > CommandPool::AllocateCommandBuffer(
    const vk::CommandBufferAllocateInfo& info)
{
  auto copyInfo = info;
  copyInfo.setCommandPool(mPool);
  return MakeUnique<CommandBuffer>(mGraphics, *this, copyInfo);
}

std::unique_ptr< CommandBuffer > CommandPool::AllocateCommandBuffer(vk::CommandBufferLevel level)
{
  auto info =
      vk::CommandBufferAllocateInfo{}.setCommandBufferCount(1).setLevel(level).setCommandPool(
          mPool);
  return MakeUnique<CommandBuffer>(mGraphics, *this, info);
}

vk::CommandPool CommandPool::GetPool() const
{
  return mPool;
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

