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

#include <dali/graphics/vulkan/common.h>
#include <dali/graphics/graphics-swapchain.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

class Surface;
class LogicalDevice;

class Swapchain : public GraphicsSwapchain
{
public:
  Swapchain(Integration::GraphicsSwapchainBase* o = nullptr) : GraphicsSwapchain(o){};
  using GraphicsSwapchain::operator=;

  static Swapchain New(const LogicalDevice& device, const Surface& surface, int bufferCount, DepthStencil depthStencil);
};

} // Vulkan
} // Graphics
} // Dali

#endif //DALI_CORE_GRAPHICS_VULKAN_SWAPCHAIN_H
