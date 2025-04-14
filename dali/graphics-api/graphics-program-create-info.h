#ifndef DALI_GRAPHICS_PROGRAM_CREATE_INFO_H
#define DALI_GRAPHICS_PROGRAM_CREATE_INFO_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
 */

#include "graphics-program.h"
#include "graphics-types.h"

namespace Dali
{
namespace Graphics
{
/**
 * This structure represents the information needed to generate a program.
 */
struct ProgramCreateInfo
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
   * @brief Set the allocation callbacks.
   *
   * @param[in] value set of allocation callbacks
   * @return reference to this structure.
   */
  auto& SetAllocationCallbacks(const AllocationCallbacks& value)
  {
    allocationCallbacks = &value;
    return *this;
  }

  /**
   * @brief Sets the shader state for the program
   *
   * The function takes an array of shader states in order to compile
   * the program. Each ShaderState structure determines the pipeline stage
   * the shader should be executed on. The Shader object may be already created
   * with a specific stage.
   *
   * Sample:
   * SetShaderState( { ShaderState().SetShader( vertexShader)
   *                                .SetPipelineStage( PipelineStage::VERTEX_SHADER ),
   *                   ShaderState().SetShader( fragmentShader )
   *                                .SetPipelineStage( PipelineStage::FRAGMENT_SHADER )
   *                  } );
   *
   * In modern graphics API it is possible to attach more than one Shader to a single
   * stage. For example, one Shader may be just a library of functions:
   * SetShaderState( { ShaderState().SetShader( vertexShader)
   *                                .SetPipelineStage( PipelineStage::VERTEX_SHADER ),
   *                   ShaderState().SetShader( shaderCommons )
   *                                .SetPipelineStage( PipelineStage::VERTEX_SHADER ),
   *                   ShaderState().SetShader( fragmentShader )
   *                                .SetPipelineStage( PipelineStage::FRAGMENT_SHADER )
   *                  } );
   *
   * The Program will compile and link all given shaders.
   *
   * param[in] value Valid array of shader states
   * @return reference to this structure
   */
  auto& SetShaderState(const std::vector<ShaderState>& value)
  {
    shaderState = &value;
    return *this;
  }

  /**
   * @brief Set the name of program.
   *
   * @param[in] value set of allocation callbacks
   * @return reference to this structure.
   */
  auto& SetName(const std::string& value)
  {
    name = value;
    return *this;
  }

  /**
   * @brief Sets whether the program is used the funcation of file-caching by DALi.
   *
   * @param[in] value true if the program is used internally by DALi. Otherwise false. Default is false.
   * @return reference to this structure.
   */
  auto& SetFileCaching(bool value)
  {
    useFileCache = value;
    return *this;
  }

  GraphicsStructureType type{GraphicsStructureType::PROGRAM_CREATE_INFO_STRUCT};
  ExtensionCreateInfo*  nextExtension{nullptr};

  std::string_view                name{};
  const std::vector<ShaderState>* shaderState{nullptr};
  const AllocationCallbacks*      allocationCallbacks{nullptr};
  bool                            useFileCache{false};
};

} // namespace Graphics

} // namespace Dali

#endif //DALI_GRAPHICS_PROGRAM_CREATE_INFO_H
