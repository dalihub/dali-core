#ifndef DALI_GRAPHICS_RENDERPASS_CREATE_INFO_H
#define DALI_GRAPHICS_RENDERPASS_CREATE_INFO_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <memory>

// INTERNAL INCLUDES
#include "graphics-render-pass.h"
#include "graphics-types.h"

namespace Dali
{
namespace Graphics
{
class RenderTarget;
/**
 * @brief Interface class for RenderPassCreateInfo types in the graphics API.
 *
 * This class describes RenderPass properties (TBD)
 */
struct RenderPassCreateInfo
{
  /**
   * @brief Sets pointer to the extension
   *
   * The pointer to the extension must be set either to nullptr
   * or to the valid structure. The structures may create
   * a chain. The last structure in a chain must point at
   * nullptr.
   *
   * @param[in] value pointer to the valid extension structure
   * @return reference to this structure
   */
  auto& SetNextExtension(ExtensionCreateInfo* value)
  {
    nextExtension = value;
    return *this;
  }

  /**
   * @brief Binds render target to render pass
   *
   * @param[in] value valid RenderTarget object
   * @return reference to this structure
   */
  auto& SetRenderTarget(RenderTarget* value)
  {
    renderTarget = value;
    return *this;
  }

  /**
   * @brief Sets allocation callbacks which will be used when object is created
   * and destroyed.
   *
   * @param[in] value Valid reference to AllocationCallbacksStructure
   * @return reference to this structure
   */
  auto& SetAllocationCallbacks(const AllocationCallbacks& value)
  {
    allocationCallbacks = &value;
    return *this;
  }

  GraphicsStructureType type{GraphicsStructureType::RENDERPASS_CREATE_INFO_STRUCT};
  ExtensionCreateInfo*  nextExtension{nullptr};
  RenderTarget*         renderTarget{nullptr};

  const AllocationCallbacks* allocationCallbacks{nullptr};
};

} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_FRAMEBUFFER_FACTORY_H