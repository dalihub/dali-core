#ifndef DALI_SHADER_INTEG_H
#define DALI_SHADER_INTEG_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <string>      // std::string
#include <string_view> // std::string_view

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/rendering/shader.h>
#include <dali/public-api/rendering/uniform-block.h>

namespace Dali::Integration
{
/**
 * @brief Creates Shader and connect by given uniform blocks
 *
 * @SINCE_2_4.14
 * @param[in] vertexShader Vertex shader code for the effect.
 * @param[in] fragmentShader Fragment Shader code for the effect.
 * @param[in] hints Hints to define the geometry of the rendered object
 * @param[in] shaderName The name of this shader.
 * @param[in] uniformBlocks Uniform blocks to be connected to shader.
 * @return A handle to a shader effect
 */
DALI_CORE_API Dali::Shader ShaderNewWithUniformBlock(std::string_view vertexShader, std::string_view fragmentShader, Shader::Hint::Value hints, std::string_view shaderName, std::vector<Dali::UniformBlock> uniformBlocks);

/**
 * @brief Generates tag 'legacy-prefix-end' with end position of
 * prefix text to make shader code parsing easier.
 *
 * @SINCE_2_3.43
 * @return Generated string with tag.
 */
DALI_CORE_API std::string GenerateTaggedShaderPrefix(const std::string& shaderPrefix);

} // namespace Dali::Integration

#endif // DALI_SHADER_INTEG_H
