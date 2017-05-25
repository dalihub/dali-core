#ifndef DALI_CORE_GRAPHICS_PHYSICAL_DEVICE_H
#define DALI_CORE_GRAPHICS_PHYSICAL_DEVICE_H

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

#include <dali/graphics/graphics-logical-device.h>
#include <dali/graphics/graphics-surface.h>
#include <dali/graphics/integration/graphics-physical-device-base.h>
#include <dali/graphics/vulkan/common.h>

namespace Dali
{
namespace Graphics
{

class GraphicsPhysicalDevice : public VkHandleBase< Integration::GraphicsPhysicalDeviceBase >
{
public:
  GraphicsPhysicalDevice(Integration::GraphicsPhysicalDeviceBase* impl = nullptr)
  : VkHandleBase{impl}
  {
  }

  using VkHandleBase::operator=;

public:
  /**
   * Creates new uninitialised physical device
   * @return
   */
  static GraphicsPhysicalDevice New();

  /**
   *
   * @param extensions
   * @param layers
   * @return
   */
  bool Initialise(const ExtensionNameList& extensions, const ValidationLayerFlags2& layers);

  /**
   *
   * @param instanceExtensionName
   * @return
   */
  bool IsExtensionAvailable(const std::string& instanceExtensionName) const;

  /**
   *
   * @param instanceExtensionName
   * @return
   */
  bool IsLayerAvailable(const std::string& instanceExtensionName) const;

  /**
 *
 * @param flags
 * @return
 */
  bool ChoosePhysicalDevice(const PhysicalDeviceFlags& flags = 0u);

  /**
   * Creates logical device from the physical device
   * @param enabledExtensions
   * @return
   */
  GraphicsLogicalDevice CreateLogicalDevice(const ExtensionNameList& enabledExtensions);

  /**
   *
   * @return
   */
  GraphicsLogicalDevice CreateLogicalDevice();

  // temporary solution to bind physical device with surface
  /**
   *
   * @param info
   * @return
   */
  GraphicsSurface CreateSurface(const NativeSurfaceCreateInfo& info);
};

using PhysicalDeviceHandle = VkHandleBase< Integration::GraphicsPhysicalDeviceBase >;

} // Graphics
} // Dali

#endif //DALI_CORE_GRAPHICSPHYSICALDEVICE_H
