#ifndef DALI_GRAPHICS_FRAMEBUFFER_CREATE_INFO_H
#define DALI_GRAPHICS_FRAMEBUFFER_CREATE_INFO_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include "graphics-framebuffer.h"
#include "graphics-types.h"

namespace Dali
{
namespace Graphics
{
/**
 * @brief Interface class for FramebufferCreateInfo types in the graphics API.
 */
struct FramebufferCreateInfo
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
   * @brief Adds color attachments to the framebuffer
   *
   * The color attachments are provided as a list. The number
   * of color attachments depends on the driver capability. It may be
   * a single attachment allowed for old hardware and multiple attachments
   * for the modern one (MRT).
   *
   * @param[in] value List of attachment descriptors
   * @return reference to this structure
   */
  auto& SetColorAttachments(std::vector<ColorAttachment> value)
  {
    colorAttachments = value;
    return *this;
  }

  /**
   * @brief Adds depth/stencil attachment to the framebuffer
   *
   * The depth/stencil attachment may be supported only by certain (modern)
   * hardware. The hardware capabilities should be checked before using
   * the depth/stencil attachment.
   *
   * @param[in] value Depth/stencil attachment descriptor
   * @return reference to this structure
   */
  auto& SetDepthStencilAttachment(DepthStencilAttachment value)
  {
    depthStencilAttachment = value;
    return *this;
  }

  /**
   * @brief Sets size of framebuffer
   *
   * @param[in] value size of framebuffer
   * @return reference to this structure
   */
  auto& SetSize(Extent2D value)
  {
    size = value;
    return *this;
  }

  /**
   * @brief Sets multisampling level of framebuffer
   *
   * @param[in] value multisampling level of framebuffer
   * @return reference to this structure
   */
  auto& SetMultiSamplingLevel(uint8_t value)
  {
    multiSamplingLevel = value;
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

  GraphicsStructureType        type{GraphicsStructureType::FRAMEBUFFER_CREATE_INFO_STRUCT};
  ExtensionCreateInfo*         nextExtension{nullptr};
  std::vector<ColorAttachment> colorAttachments{};
  DepthStencilAttachment       depthStencilAttachment{};
  Extent2D                     size{};
  uint8_t                      multiSamplingLevel{0u};

  const AllocationCallbacks* allocationCallbacks{nullptr};
};

} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_FRAMEBUFFER_CREATE_INFO_H