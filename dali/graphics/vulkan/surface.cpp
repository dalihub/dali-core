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

#include <dali/graphics/vulkan/surface.h>
#include <dali/graphics/vulkan/surface/vulkan-surface-base.h>
namespace Dali
{
namespace Graphics
{
namespace Vulkan
{


uint32_t Surface::GetWidth() const
{
  return GetObject()->GetWidth();
}

uint32_t Surface::GetHeight() const
{
  return GetObject()->GetHeight();
}

const vk::SurfaceKHR Surface::GetSurface() const
{
  return static_cast<VulkanSurfaceBase*>(GetObject())->GetVkSurface();
}

const vk::SurfaceFormatKHR& Surface::GetFormat() const
{
  return static_cast<VulkanSurfaceBase*>(GetObject())->GetVkSurfaceFormat();
}

const vk::SurfaceCapabilitiesKHR& Surface::GetCapabilities() const
{
  return static_cast<VulkanSurfaceBase*>(GetObject())->GetCapabilities();
}

const std::vector< vk::SurfaceFormatKHR >& Surface::GetAllFormats() const
{
  return static_cast<VulkanSurfaceBase*>(GetObject())->GetAllFormats();
}

} // Vulkan
} // Graphics
} // Dali
