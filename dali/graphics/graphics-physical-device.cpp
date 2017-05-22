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

#include <dali/graphics/graphics-physical-device.h>

namespace Dali
{
namespace Graphics
{

GraphicsPhysicalDevice GraphicsPhysicalDevice::New()
{
  return GraphicsPhysicalDevice(nullptr);
}

bool GraphicsPhysicalDevice::Initialise(const ExtensionNameList& extensions, const ValidationLayerFlags2& layers)
{
  return GetObject()->Initialise(extensions, layers);
}

bool GraphicsPhysicalDevice::IsExtensionAvailable(const std::string& instanceExtensionName)
{
  return GetObject()->IsExtensionAvailable(instanceExtensionName);
}

bool GraphicsPhysicalDevice::IsLayerAvailable(const std::string& instanceLayerName)
{
  return GetObject()->IsLayerAvailable(instanceLayerName);
}

bool GraphicsPhysicalDevice::ChoosePhysicalDevice(const PhysicalDeviceFlags& flags)
{
  return GetObject()->ChoosePhysicalDevice(flags);
}

GraphicsLogicalDevice GraphicsPhysicalDevice::CreateLogicalDevice( const ExtensionNameList& enabledExtensions )
{
  return GetObject()->CreateLogicalDevice( enabledExtensions );
}

GraphicsLogicalDevice GraphicsPhysicalDevice::CreateLogicalDevice()
{
  return CreateLogicalDevice( ExtensionNameList() );
}

GraphicsSurface GraphicsPhysicalDevice::CreateSurface( const NativeSurfaceCreateInfo& info  )
{
  return GetObject()->CreateSurface(info);
}

} // Graphics
} // Dali