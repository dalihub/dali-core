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

// FILE HEADER
#include <dali/integration-api/shader-integ.h>

// INTERNAL INCLUDES
#include <dali/internal/event/rendering/shader-impl.h>

namespace Dali::Integration
{
Dali::Shader ShaderNewWithUniformBlock(std::string_view vertexShader, std::string_view fragmentShader, Shader::Hint::Value hints, std::string_view shaderName, std::vector<Dali::UniformBlock> uniformBlocks, bool strongConnection)
{
  Internal::ShaderPtr shader = Dali::Internal::Shader::New(vertexShader, fragmentShader, hints, shaderName, std::move(uniformBlocks), strongConnection);
  return Shader(shader.Get());
}

Dali::Shader ShaderNewWithUniformBlock(Dali::Property::Value shaderMap, std::vector<Dali::UniformBlock> uniformBlocks, bool strongConnection)
{
  Internal::ShaderPtr shader = Dali::Internal::Shader::New(std::move(shaderMap), std::move(uniformBlocks), strongConnection);
  return Shader(shader.Get());
}

std::string GenerateTaggedShaderPrefix(const std::string& shaderPrefix)
{
  return Dali::Internal::Shader::GenerateTaggedShaderPrefix(shaderPrefix);
}
} // namespace Dali::Integration
