#ifndef DALI_GRAPHICS_RENDER_TARGET_CREATE_INFO
#define DALI_GRAPHICS_RENDER_TARGET_CREATE_INFO

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
#include "graphics-render-target.h"
#include "graphics-types.h"

namespace Dali
{
namespace Integration
{
struct RenderSurface;
}

namespace Graphics
{
/**
 * @brief Describes specification of new render target.
 *
 * RenderTarget represents any presentable graphics output.
 */
struct RenderTargetCreateInfo
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
   * @brief Sets surface associated with the RenderTarget
   *
   * The surface is a pointer to any native surface object.
   *
   * @param[in] value Pointer to the native surface
   * @return reference to this structure
   */
  auto& SetSurface( Integration::RenderSurface* value )
  {
    surface = value;
    return *this;
  }

  /**
   * @brief Sets framebuffer associated with the RenderTarget
   *
   * The framebuffer and surface must not be set together.
   *
   * @param[in] value Pointer to the Framebuffer object
   * @return reference to this structure
   */
  auto& SetFramebuffer( Framebuffer* value )
  {
    framebuffer = value;
    return *this;
  }

  /**
   * @brief Sets render target size
   *
   * @param[in] value Size of render target
   * @return reference to this structure
   */
  auto& SetExtent( Extent2D value )
  {
    extent = value;
    return *this;
  }

  /**
   * @brief Sets pre-transform of the render target
   *
   * @param[in] value transform flags
   * @return reference to this structure
   */
  auto& SetPreTransform( RenderTargetTransformFlags value )
  {
    preTransform = value;
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

  GraphicsStructureType         type{GraphicsStructureType::RENDER_TARGET_CREATE_INFO_STRUCT};
  Integration::RenderSurface*   surface{nullptr};
  Framebuffer*                  framebuffer{nullptr};
  Extent2D                      extent{};
  RenderTargetTransformFlags    preTransform{0u};
  ExtensionCreateInfo*          nextExtension{nullptr};

  const AllocationCallbacks* allocationCallbacks{nullptr};
};

} // namespace Graphics
} // namespace Dali
#endif //DALI_GRAPHICS_RENDER_TARGET_CREATE_INFO
