#ifndef DALI_CORE_GRAPHICS_SURFACE_BASE_H
#define DALI_CORE_GRAPHICS_SURFACE_BASE_H

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

namespace Dali
{
namespace Graphics
{
class GraphicsPhysicalDevice;

namespace Vulkan
{

namespace Internal
{
class GraphicsSurfaceBase : public VkObject
{
private:
  GraphicsSurfaceBase() = default;

public:
  //GraphicsSurfaceBase(const GraphicsPhysicalDevice &adaptor)
  //{};
  GraphicsSurfaceBase(const GraphicsPhysicalDevice &physicalDevice){};
  virtual ~GraphicsSurfaceBase() = default;

  virtual bool Initialise() = 0;

  virtual bool Replace() = 0;

  virtual bool Destroy() = 0;

  virtual uint32_t GetWidth() = 0;

  virtual uint32_t GetHeight() = 0;
};
}

} // Vulkan

// todo: remove 'internal' namespace
namespace Integration
{
using GraphicsSurfaceBase = Vulkan::Internal::GraphicsSurfaceBase;
} // Integration
} // Graphics
} // Dali

#endif // DALI_CORE_GRAPHICS_SURFACE_BASE_H
