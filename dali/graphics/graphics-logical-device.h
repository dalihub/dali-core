#ifndef DALI_CORE_GRAPHICS_GRAPHICS_LOGICAL_DEVICE_H
#define DALI_CORE_GRAPHICS_GRAPHICS_LOGICAL_DEVICE_H

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

#include <dali/graphics/integration/graphics-logical-device-base.h>
#include <dali/graphics/graphics-swapchain.h>

namespace Dali
{
namespace Graphics
{

using GraphicsLogicalDeviceHandle = VkHandleBase<GraphicsLogicalDeviceBase>;

class GraphicsLogicalDevice : public GraphicsLogicalDeviceHandle
{
public:

  GraphicsLogicalDevice( GraphicsLogicalDeviceBase* impl = nullptr ) : GraphicsLogicalDeviceHandle(impl)
  {

  }

  using GraphicsLogicalDeviceHandle::operator=;

  GraphicsSwapchain CreateSwapchain(const GraphicsSurface& surface,
                                                               uint32_t               bufferCount,
                                                               DepthStencil           depthStencil,
                                                               bool                   enforceVSync);


};

} // Graphics
} // Dali


#endif //DALI_CORE_GRAPHICS_GRAPHICS_LOGICAL_DEVICE_H
