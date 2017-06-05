#ifndef DALI_GRAPHICS_VULKAN_COMMANDPOOL_H
#define DALI_GRAPHICS_VULKAN_COMMANDPOOL_H

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

class Graphics;
class CommandBuffer;
class CommandPool
{
public:

  CommandPool( Graphics& graphics, const vk::CommandPoolCreateInfo& createInfo );
  ~CommandPool();

  std::unique_ptr<CommandBuffer> AllocateCommandBuffer( const vk::CommandBufferAllocateInfo& info );

  std::unique_ptr<CommandBuffer> AllocateCommandBuffer( vk::CommandBufferLevel level );

  vk::CommandPool GetPool() const;

private:
  Graphics&       mGraphics;
  vk::CommandPool mPool;
};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_COMMANDPOOL_H
