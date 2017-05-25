#ifndef DALI_CORE_GRAPHICS_GRAPHICS_LOGICAL_DEVICE_BASE_H
#define DALI_CORE_GRAPHICS_GRAPHICS_LOGICAL_DEVICE_BASE_H

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
#include <cinttypes>

namespace Dali
{
namespace Graphics
{
class GraphicsPhysicalDevice;
class GraphicsSurface;

// base class for context implementation
class GraphicsLogicalDeviceBase : public VkObject
{
public:
  virtual bool Initialise() = 0;

  virtual GraphicsSwapchain CreateSwapchain(const GraphicsSurface& surface,
                                            uint32_t               bufferCount,
                                            DepthStencil           depthStencil,
                                            VSyncMode              enforceVSync) = 0;

  virtual GraphicsPhysicalDevice GetGraphicsPhysicalDevice() const = 0;
};

} // Graphics
} // Dali

#endif //DALI_CORE_GRAPHICS_GRAPHICS_LOGICAL_DEVICE_BASE_H
