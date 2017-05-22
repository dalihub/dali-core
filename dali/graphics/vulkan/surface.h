#ifndef DALI_CORE_GRAPHICS_VULKAN_SURFACE_H
#define DALI_CORE_GRAPHICS_VULKAN_SURFACE_H

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

#include <dali/graphics/graphics-surface.h>
#include <dali/graphics/vulkan/common.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
/**
 * This is a common handle for Vulkan surface implementation
 */
class Surface : public GraphicsSurface
{
public:
  Surface(Integration::GraphicsSurfaceBase* impl = nullptr) : GraphicsSurface(impl)
  {
  }
  using GraphicsSurface::operator=;

  /**
   * Returns surface width
   * @return
   */
  uint32_t GetWidth() const;

  /**
 * Returns surface height
 * @return
 */
  uint32_t GetHeight() const;

  // Vulkan specific API
public:
  /**
 * Returns associated Vulkan surface object
 * @return
 */
  const vk::SurfaceKHR GetSurface() const;

  /**
   * Returns current surface format
   * @return
   */
  const vk::SurfaceFormatKHR& GetFormat() const;

  /**
   *
   * @return
   */
  const vk::SurfaceCapabilitiesKHR& GetCapabilities() const;

  /**
   * 
   * @return
   */
  const std::vector< vk::SurfaceFormatKHR >& GetAllFormats() const;
};

} // Vulkan
} // Graphics
} // Dali

#endif //DALI_CORE_GRAPHICS_VULKAN_SURFACE_H
