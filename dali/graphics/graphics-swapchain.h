#ifndef DALI_CORE_GRAPHICS_SWAPCHAIN_H
#define DALI_CORE_GRAPHICS_SWAPCHAIN_H

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

#include <cinttypes>
#include "dali/graphics/vulkan/common.h"
#include <dali/graphics/integration/graphics-swapchain-base.h>

namespace Dali
{
namespace Graphics
{
class GraphicsPhysicalDevice;

enum class DepthStencil
{
  NONE,
  DEPTH_16,
  DEPTH_24,
  DEPTH_16_STENCIL_8,
  DEPTH_24_STENCIL_8
};

class GraphicsContext;

using SwapchainHandle = VkHandleBase<Integration::GraphicsSwapchainBase>;

class GraphicsSwapchain : public SwapchainHandle
{
public:

  GraphicsSwapchain(Integration::GraphicsSwapchainBase* o = nullptr) : SwapchainHandle(o){};
  using SwapchainHandle::operator=;

  /**
   *
   */
  void Initialise();

  /**
   * Acquires next renderable frame from swapchain
   */
  void AcquireFrame();

  /**
   * Presents frame
   */
  void PresentFrame();

};

}
}

#endif //DALI_CORE_GRAPHICSSWAPCHAIN_H
