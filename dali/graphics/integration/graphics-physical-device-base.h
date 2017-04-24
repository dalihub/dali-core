#ifndef DALI_CORE_GRAPHICS_PHYSCIAL_DEVICE_BASE_H
#define DALI_CORE_GRAPHICS_PHYSCIAL_DEVICE_BASE_H

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
#include <dali/graphics/vulkan/common.h>
namespace Dali
{
namespace Graphics
{
namespace Integration
{

// abstract class for building the graphics adaptor
class GraphicsPhysicalDeviceBase : public VkObject
{
public:
  /**
   *
   * @param extensions
   * @param layers
   * @return
   */
  virtual bool Initialise(const ExtensionNameList& extensions, const ValidationLayerFlags2& layers) = 0;

  /**
   *
   * @param instanceExtensionName
   * @return
   */
  virtual bool IsExtensionAvailable(const std::string& instanceExtensionName) = 0;

  /**
   *
   * @param instanceExtensionName
   * @return
   */
  virtual bool IsLayerAvailable(const std::string& instanceExtensionName) = 0;

  /**
   *
   * @param surface
   * @return
   */
  virtual GraphicsLogicalDevice CreateLogicalDevice(const ExtensionNameList& enabledExtensions){};

  /**
   *
   * @param flags
   * @return
   */
  virtual bool ChoosePhysicalDevice(const PhysicalDeviceFlags& flags) = 0;

  /**
   *
   * @param flags
   */
  virtual void SetValidationDebugChannels(const ValidationChannelFlags& flags) = 0;

  /**
   *
   * @param info
   * @return
   */
  virtual GraphicsSurface CreateSurface(const NativeSurfaceCreateInfo& info) = 0;
};
} // Integration
} // Graphics
} // Dali

#endif //DALI_CORE_GRAPHICS_PHYSCIAL_DEVICE_BASE_H
