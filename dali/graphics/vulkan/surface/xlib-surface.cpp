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

#include <dali/graphics/vulkan/physical-device.h>
#include <dali/graphics/vulkan/surface/xlib-surface.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

XlibSurface::XlibSurface(const GraphicsPhysicalDevice &device, Display* display, Window window)
: VulkanSurfaceBase(device), mPhysicalDevice(device), mDisplay(display), mWindow(window)
{
}

// GraphicsSurfaceBase

bool XlibSurface::Initialise()
{
  PhysicalDevice device(mPhysicalDevice.GetObject());

  Window root;
  int x, y;
  unsigned int width, height, borderWidth, depth;

  XGetGeometry( mDisplay, mWindow, &root, &x, &y, &width, &height, &borderWidth, &depth );

  mSurfaceWidth  = width;
  mSurfaceHeight = height;

  auto vkInstance  = device.GetInstance();
  auto vkAllocator = device.GetAllocator();
  vk::XlibSurfaceCreateInfoKHR info;
  info.setWindow( mWindow );
  info.setDpy( mDisplay );

  VkAssertCall(vkInstance.createXlibSurfaceKHR(&info, vkAllocator, &mSurface));

  mFormats       = device.GetPhysicalDevice().getSurfaceFormatsKHR(mSurface).value;
  mCapabilities  = device.GetPhysicalDevice().getSurfaceCapabilitiesKHR(mSurface).value;
  mDefaultFormat = mFormats[0];
  for(auto& format : mFormats)
  {
    if(format.format == vk::Format::eR8G8B8A8Unorm)
    {
      mDefaultFormat = format;
    }
  }

  return true;
}

bool XlibSurface::Replace()
{
  assert( "XcbSurface::Replace() not implemented!");
  return false;
}

bool XlibSurface::Destroy()
{
  assert( "XcbSurface::Destroy() not implemented!");
  return false;
}

uint32_t XlibSurface::GetWidth()
{
  return mSurfaceWidth;
}

uint32_t XlibSurface::GetHeight()
{
  return mSurfaceHeight;
}

// VulkanSurfaceBase

const vk::SurfaceKHR XlibSurface::GetVkSurface() const
{
  return mSurface;
}

const vk::SurfaceFormatKHR& XlibSurface::GetVkSurfaceFormat() const
{
  return mDefaultFormat;
}

const vk::SurfaceCapabilitiesKHR& XlibSurface::GetCapabilities() const
{
  return mCapabilities;
}

const std::vector< vk::SurfaceFormatKHR >& XlibSurface::GetAllFormats() const
{
  return mFormats;
}


}
}
}