#ifndef DALI_GRAPHICS_SHADER_CREATE_INFO_H
#define DALI_GRAPHICS_SHADER_CREATE_INFO_H

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
#include <cstring>
#include <memory>

// INTERNAL INCLUDES
#include "graphics-shader.h"
#include "graphics-types.h"

namespace Dali
{
namespace Graphics
{
/**
 * @brief ShaderCreateInfo contains details of a single shader (not a GL program!)
 * attached to a specified pipeline stage (ie. vertex shader, fragment shader etc.)
 */
struct ShaderCreateInfo
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
   * @brief Sets pipeline stage the shader will be executed in
   *
   * @param[in] value valid pipeline stage
   * @return reference to this structure
   */
  auto& SetPipelineStage(PipelineStage value)
  {
    pipelineStage = value;
    return *this;
  }

  /**
   * @brief Sets shader source language
   *
   * @param[in] value valid source language
   * @return reference to this structure
   */
  auto& SetShaderlanguage(ShaderLanguage value)
  {
    shaderlanguage = value;
    return *this;
  }

  /**
   * @brief Sets pointer to the source data
   *
   * @param[in] value pointer to the source data
   * @return reference to this structure
   */
  auto& SetSourceData(const void* value)
  {
    sourceData = value;
    return *this;
  }

  /**
   * @brief Sets size of the source data (in bytes)
   *
   * If the shader mode is TEXT, the size must include
   * null-terminator.
   *
   * @param[in] value size in bytes
   * @return reference to this structure
   */
  auto& SetSourceSize(uint32_t value)
  {
    sourceSize = value;
    return *this;
  }

  /**
   * @brief Sets shader source mode
   *
   * @param[in] value shader mode
   * @return reference to this structure
   */
  auto& SetSourceMode(ShaderSourceMode value)
  {
    sourceMode = value;
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

  /**
   * @brief Equality operator.
   *
   * @param[in] rhs The ShaderCreateInfo to test against
   * @return True if the ShaderCreateInfo are equal
   */
  bool operator==(const ShaderCreateInfo& rhs) const
  {
    return (sourceSize == rhs.sourceSize && 0 == memcmp(sourceData, rhs.sourceData, sourceSize) && pipelineStage == rhs.pipelineStage && allocationCallbacks == rhs.allocationCallbacks);
  }

  GraphicsStructureType type{GraphicsStructureType::SHADER_CREATE_INFO_STRUCT};
  ExtensionCreateInfo*  nextExtension{nullptr};

  PipelineStage    pipelineStage{};
  ShaderLanguage   shaderlanguage{};
  const void*      sourceData{nullptr};
  uint32_t         sourceSize{0u};
  ShaderSourceMode sourceMode{};

  const AllocationCallbacks* allocationCallbacks{nullptr};
};

} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_SHADER_CREATE_INFO_H