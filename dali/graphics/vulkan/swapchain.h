#ifndef DALI_CORE_GRAPHICS_VULKAN_SWAPCHAIN_H
#define DALI_CORE_GRAPHICS_VULKAN_SWAPCHAIN_H

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

#include <dali/graphics/graphics-swapchain.h>
#include <dali/graphics/vulkan/common.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

class Surface;
class LogicalDevice;
class CommandBuffer;

/**
 * In computer graphics swap chain represents a series of images which are sort of
 * virtual framebuffers.
 * Swapchain class manages mentioned images ( swaps them when needed for example on vsync )
 * and delivers an image to the graphics API as a render target bound to
 * the surface ( simplifying, delivers main framebuffer to render to ).
 */
class Swapchain : public GraphicsSwapchain
{
public:
  Swapchain(Integration::GraphicsSwapchainBase* impl = nullptr) : GraphicsSwapchain{impl}
  {
  }

  using GraphicsSwapchain::operator=;

  static Swapchain New(const LogicalDevice& device, const Surface& surface, uint32_t bufferCount,
                       DepthStencil depthStencil);

  const CommandBuffer& GetCommandBuffer(uint32_t index) const;

  const CommandBuffer& GetCurrentCommandBuffer() const;

  const vk::Image& GetCurrentImage() const;
};

} // Vulkan
} // Graphics
} // Dali

#endif //DALI_CORE_GRAPHICS_VULKAN_SWAPCHAIN_H
