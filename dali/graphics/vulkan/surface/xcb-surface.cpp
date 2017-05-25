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
#include <dali/graphics/vulkan/surface/xcb-surface.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

XcbSurface::XcbSurface(const GraphicsPhysicalDevice &device, xcb_connection_t *connection, xcb_window_t window)
: VulkanSurfaceBase(device), mPhysicalDevice(device), mConnection(connection), mWindow(window)
{
}

// GraphicsSurfaceBase

bool XcbSurface::Initialise()
{
  xcb_generic_error_t *err;

  PhysicalDevice device(mPhysicalDevice.GetObject());

  auto cookie = xcb_get_geometry(mConnection, mWindow);
  auto reply  = xcb_get_geometry_reply(mConnection, cookie, &err);

  mSurfaceWidth  = reply->width;
  mSurfaceHeight = reply->height;

  auto vkInstance  = device.GetInstance();
  auto vkAllocator = device.GetAllocator();
  vk::XcbSurfaceCreateInfoKHR info;
  info.setConnection(mConnection).setWindow(mWindow);

  VkAssertCall(vkInstance.createXcbSurfaceKHR(&info, vkAllocator, &mSurface));

  mFormats       = device.GetPhysicalDevice().getSurfaceFormatsKHR(mSurface).value;
  mCapabilities  = device.GetPhysicalDevice().getSurfaceCapabilitiesKHR(mSurface).value;
  mDefaultFormat = mFormats[0];
  for(auto&& format : mFormats)
  {
    if(format.format == vk::Format::eR8G8B8A8Unorm)
    {
      mDefaultFormat = format;
    }
  }

  return true;
}

bool XcbSurface::Replace()
{
  assert( true && "XcbSurface::Replace() not implemented!");
  return false;
}

bool XcbSurface::Destroy()
{
  assert( true && "XcbSurface::Destroy() not implemented!");
  return false;
}

uint32_t XcbSurface::GetWidth()
{
  return mSurfaceWidth;
}

uint32_t XcbSurface::GetHeight()
{
  return mSurfaceHeight;
}

// VulkanSurfaceBase

const vk::SurfaceKHR XcbSurface::GetVkSurface() const
{
  return mSurface;
}

const vk::SurfaceFormatKHR& XcbSurface::GetVkSurfaceFormat() const
{
  return mDefaultFormat;
}

const vk::SurfaceCapabilitiesKHR& XcbSurface::GetCapabilities() const
{
  return mCapabilities;
}

const std::vector< vk::SurfaceFormatKHR >& XcbSurface::GetAllFormats() const
{
  return mFormats;
}


}
}
}