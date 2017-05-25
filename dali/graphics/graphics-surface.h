#ifndef DALI_CORE_GRAPHICS_SURFACE_H
#define DALI_CORE_GRAPHICS_SURFACE_H

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

#include <dali/graphics/integration/graphics-surface-base.h>
#include "dali/graphics/vulkan/common.h"

namespace Dali
{
namespace Graphics
{
class GraphicsPhysicalDevice;

enum class NativeSurfaceType
{
  UNDEFINED,
  X11,
  XCB,
  WAYLAND,
  ANDROID_NATIVE_WINDOW,
  WIN32
};

struct NativeSurfaceCreateInfo
{
  NativeSurfaceCreateInfo(NativeSurfaceType type) : surfaceType(type)
  {
  }
  NativeSurfaceType surfaceType;
};

using SurfaceHandle = VkHandleBase< Integration::GraphicsSurfaceBase >;

class GraphicsSurface : public SurfaceHandle
{
public:
  GraphicsSurface(Integration::GraphicsSurfaceBase* impl = nullptr) : SurfaceHandle{impl}
  {
  }
  using SurfaceHandle::operator=;

  template< typename T, typename... ARGS >
  static GraphicsSurface New(const GraphicsPhysicalDevice& physicalDevice, ARGS&&... args)
  {
    GraphicsSurface retval = (new T(physicalDevice, args...));
    if(!retval.Initialise())
      retval = nullptr;
    return retval;
  }

  bool     Initialise();
  bool     Replace();
  bool     Destroy();
  uint32_t GetWidth();
  uint32_t GetHeight();

private:
};
}
}

#endif //DALI_CORE_GRRAPHICSSURFACE_H
