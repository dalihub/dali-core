#ifndef DALI_GRAPHICS_BUFFER_CREATE_INFO_H
#define DALI_GRAPHICS_BUFFER_CREATE_INFO_H

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
#include "graphics-command-buffer.h"
#include "graphics-types.h"

namespace Dali
{
namespace Graphics
{
/**
 * @brief CommandBufferCreateInfo describes new command buffer
 */
struct CommandBufferCreateInfo
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
   * @brief Sets command buffer level
   *
   * The command buffer level can be set to primary or secondary. Primary buffers
   * are able to execute content of secondary buffers. This way, certain commands
   * can be reused within multiple primary buffers by executing them. The pipeline
   * state may be inherited from the primary buffer.
   *
   * @param[in] value Level of buffer
   * @return reference to this structure
   */
  auto& SetLevel(CommandBufferLevel value)
  {
    level = value;
    return *this;
  }

  /**
   * @brief Sets fixed capacity of buffer
   *
   * Using fixed capacity buffer the memory for the commands may be pre-allocated.
   * Fixed capacity buffers may use different memory allocation strategy from dynamic
   * buffers (default behaviour). Buffers of known size and often re-recorded should
   * use fixed capacity.
   *
   * @param[in] value number of commands to be allocated
   * @return reference to this structure
   */
  auto& SetFixedCapacity(uint32_t value)
  {
    fixedCapacity = value;
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

  GraphicsStructureType type{GraphicsStructureType::COMMAND_BUFFER_CREATE_INFO_STRUCT};
  ExtensionCreateInfo*  nextExtension{nullptr};
  CommandBufferLevel    level{};
  uint32_t              fixedCapacity{0u};

  const AllocationCallbacks* allocationCallbacks{nullptr};
};

} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_BUFFER_CREATE_INFO_H
