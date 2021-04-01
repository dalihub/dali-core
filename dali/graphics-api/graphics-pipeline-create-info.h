#ifndef DALI_GRAPHICS_PIPELINE_CREATE_INFO_H
#define DALI_GRAPHICS_PIPELINE_CREATE_INFO_H

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
#include "graphics-pipeline.h"
#include "graphics-types.h"

namespace Dali
{
namespace Graphics
{
/**
 * @brief Interface class for generating Pipeline types in the graphics API.
 *
 * Pipeline after creation stays immutable unless the dynamic states flag is set.
 * Dynamic states are set as a bitmask indicating which pipeline states will
 * be changed dynamically by issuing a command in from the command buffer.
 *
 * The dynamic states are listed in Dali::Graphics::PipelineDynamicStateBits.
 */
struct PipelineCreateInfo
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
   * @brief Set the program
   *
   * @param[in] value
   * @return a reference to this structure
   */
  auto& SetProgramState(ProgramState* value)
  {
    programState = value;
    return *this;
  }

  /**
   * @brief Sets the color blend state
   * @param[in] value pointer to valid color blend state structure
   * @return reference to this structure
   */
  auto& SetColorBlendState(ColorBlendState* value)
  {
    colorBlendState = value;
    return *this;
  }

  /**
   * @brief Sets the viewport state.
   *
   * @param[in] value pointer to valid viewport state structure
   * @return reference to this structure
   */
  auto& SetViewportState(ViewportState* value)
  {
    viewportState = value;
    return *this;
  }

  /**
   * @brief Sets the base pipeline.
   *
   * Setting base pipeline allows inheriting that pipeline state
   * and build the new pipeline from it. The base pipeline
   * must stay valid until derived pipeline needs it.
   *
   * @param[in] value pointer to valid pipeline object
   * @return reference to this structure
   */
  auto& SetBasePipeline(Pipeline* value)
  {
    basePipeline = value;
    return *this;
  }

  /**
   * @brief Sets the depth/stencil state.
   *
   * @param[in] pointer to valid depth/stencil state structure
   * @return reference to this structure
   */
  auto& SetDepthStencilState(DepthStencilState* value)
  {
    depthStencilState = value;
    return *this;
  }

  /**
   * @brief Sets the rasterization state.
   *
   * @param[in] pointer to valid rasterization state structure
   * @return reference to this structure
   */
  auto& SetRasterizationState(RasterizationState* value)
  {
    rasterizationState = value;
    return *this;
  }

  /**
   * @brief Sets the vertex input state.
   *
   * Vertex input state describes format of vertices and must
   * be compatible with attached shaders.
   *
   * @param[in] pointer to vertex input state structure
   * @return reference to this structure
   */
  auto& SetVertexInputState(VertexInputState* value)
  {
    vertexInputState = value;
    return *this;
  }

  /**
   * @brief Sets the input assembly state.
   *
   * This state describes the topology of the pipeline.
   *
   * @param[in] pointer to valid input assembly state structure
   * @return reference to this structure
   */
  auto& SetInputAssemblyState(InputAssemblyState* value)
  {
    inputAssemblyState = value;
    return *this;
  }

  /**
   * @brief Sets the dynamic state mask.
   *
   * Certain states can be modified on fly without a need of
   * creating new pipeline. The commands which modify particular
   * states may be issued later by executing command buffers.
   *
   * @param[in] pointer to valid color blend state structure
   * @return reference to this structure
   */
  auto& SetDynamicStateMask(PipelineDynamicStateMask value)
  {
    dynamicStateMask = value;
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

  GraphicsStructureType type{GraphicsStructureType::PIPELINE_CREATE_INFO_STRUCT};
  ExtensionCreateInfo*  nextExtension{nullptr};

  ProgramState*            programState{nullptr};
  ColorBlendState*         colorBlendState{nullptr};
  ViewportState*           viewportState{nullptr};
  Pipeline*                basePipeline{nullptr};
  DepthStencilState*       depthStencilState{nullptr};
  RasterizationState*      rasterizationState{nullptr};
  VertexInputState*        vertexInputState{nullptr};
  InputAssemblyState*      inputAssemblyState{nullptr};
  PipelineDynamicStateMask dynamicStateMask{0u};

  const AllocationCallbacks* allocationCallbacks{nullptr};
};

} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_PIPELINE_CREATE_INFO_H
