#ifndef DALI_GRAPHICS_VULKAN_VKSURFACEFACTORY_H
#define DALI_GRAPHICS_VULKAN_VKSURFACEFACTORY_H

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

// INTERNAL INCLUDES

#include <dali/integration-api/graphics/surface-factory.h>

// EXTERNAL INCLUDES
// Vulkan
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#include <vulkan/vulkan.hpp>
#pragma GCC diagnostic pop

namespace Dali
{
namespace Integration
{
namespace Graphics
{
namespace Vulkan
{

class VkSurfaceFactory : public SurfaceFactory
{
public:

  VkSurfaceFactory() = default;

  virtual vk::SurfaceKHR Create(vk::Instance instance, vk::AllocationCallbacks *allocCallbacks,
                                vk::PhysicalDevice physicalDevice) const = 0;
};

} // namespace Vulkan
} // namespace Graphics
} // namespace Integration
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_VKSURFACEFACTORY_H
