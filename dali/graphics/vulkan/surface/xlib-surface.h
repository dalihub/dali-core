#ifndef DALI_CORE_GRAPHICS_VULKAN_XLIB_SURFACE_H
#define DALI_CORE_GRAPHICS_VULKAN_XLIB_SURFACE_H

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

#include <X11/Xlib.h>
#include <dali/graphics/vulkan/surface/vulkan-surface-base.h>
#include <cinttypes>

namespace Dali
{
namespace Graphics
{
class GraphicsPhysicalDevice;

struct XlibSurfaceCreateInfo : NativeSurfaceCreateInfo
{
  XlibSurfaceCreateInfo() : NativeSurfaceCreateInfo(NativeSurfaceType::X11)
  {
  }
  Display* display;
  Window   window;
};

namespace Vulkan
{

/**
 * Implementation of Xlib surface
 */
class XlibSurface : public VulkanSurfaceBase
{
public:
  XlibSurface(const GraphicsPhysicalDevice& device, Display* display, Window window);

  virtual ~XlibSurface() = default;

  // GraphicsSurfaceBase

  virtual bool Initialise();

  virtual bool Replace();

  virtual bool Destroy();

  virtual uint32_t GetWidth();

  virtual uint32_t GetHeight();

  // VulkanSurfaceBase

  virtual const vk::SurfaceKHR GetVkSurface() const;

  virtual const vk::SurfaceFormatKHR& GetVkSurfaceFormat() const;

  virtual const vk::SurfaceCapabilitiesKHR& GetCapabilities() const;

  virtual const std::vector< vk::SurfaceFormatKHR >& GetAllFormats() const;

private:
  GraphicsPhysicalDevice mPhysicalDevice{nullptr};

  vk::SurfaceKHR                      mSurface{nullptr};
  vk::SurfaceCapabilitiesKHR          mCapabilities{};
  vk::SurfaceFormatKHR                mDefaultFormat{};
  std::vector< vk::SurfaceFormatKHR > mFormats{};

  uint32_t mSurfaceWidth{0u};
  uint32_t mSurfaceHeight{0u};

  Display* mDisplay{nullptr};
  Window   mWindow{};
};
}
}
}

#endif //DALI_CORE_GRAPHICS_VULKAN_XLIB_SURFACE_H
