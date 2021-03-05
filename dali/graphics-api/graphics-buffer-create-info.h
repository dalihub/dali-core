#ifndef DALI_GRAPHICS_API_BUFFER_CREATE_INFO
#define DALI_GRAPHICS_API_BUFFER_CREATE_INFO

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
#include "graphics-buffer.h"
#include "graphics-types.h"

namespace Dali
{
namespace Graphics
{
/**
 * BufferCreateInfo describes new buffer
 */
struct BufferCreateInfo
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
   * @brief Sets the expected buffer usage
   *
   * The buffer usage should be explicitly set. It cannot be modified
   * later so in case of changing the usage, the new buffer
   * should be created. Based on Usage the implementation may
   * execute certain optimizations.
   *
   * @param[in] value Usage flags
   * @return reference to this structure
   */
  auto& SetUsage(BufferUsageFlags value)
  {
    usage = value;
    return *this;
  }

  /**
   * @brief Sets size of buffer in bytes
   *
   * @param[in] value Size of the buffer in bytes
   * @return reference to this structure
   */
  auto& SetSize(uint32_t value)
  {
    size = value;
    return *this;
  }

  /**
   * @brief Sets properties flags
   *
   * Properties flag bits can alter behaviour of the implementation
   *
   * @param[in] value Flags
   * @return reference to this structure
   */
  auto& SetBufferPropertiesFlags( BufferPropertiesFlags value )
  {
    propertiesFlags = value;
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

  GraphicsStructureType type{GraphicsStructureType::BUFFER_CREATE_INFO_STRUCT};
  ExtensionCreateInfo*  nextExtension{nullptr};
  BufferUsageFlags      usage{};
  uint32_t              size{0u};
  BufferPropertiesFlags propertiesFlags{};
  const AllocationCallbacks* allocationCallbacks{nullptr};
};

} // namespace Graphics
} // namespace Dali

#endif