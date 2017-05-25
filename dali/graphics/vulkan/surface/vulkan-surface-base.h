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

#ifndef DALI_CORE_VULKAN_SURFACE_BASE_H
#define DALI_CORE_VULKAN_SURFACE_BASE_H

#include <dali/graphics/integration/graphics-surface-base.h>
#include <dali/graphics/vulkan/common.h>

namespace Dali
{
namespace Graphics
{
class GraphicsPhysicalDevice;

namespace Vulkan
{

/**
 * Internal interface inherited by the Surface implementation
 */
class VulkanSurfaceBase : public Integration::GraphicsSurfaceBase
{
public:
  VulkanSurfaceBase(const GraphicsPhysicalDevice& physicalDevice)
  : Integration::GraphicsSurfaceBase(physicalDevice){};
  virtual ~VulkanSurfaceBase() = default;

  virtual const vk::SurfaceKHR                       GetVkSurface() const       = 0;
  virtual const vk::SurfaceFormatKHR&                GetVkSurfaceFormat() const = 0;
  virtual const vk::SurfaceCapabilitiesKHR&          GetCapabilities() const    = 0;
  virtual const std::vector< vk::SurfaceFormatKHR >& GetAllFormats() const      = 0;
};
}
}
}

#endif //DALI_CORE_VULKAN_SURFACE_BASE_H
