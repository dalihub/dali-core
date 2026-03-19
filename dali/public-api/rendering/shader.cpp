/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/public-api/rendering/shader.h> // Dali::Shader

// INTERNAL INCLUDES
#include <dali/integration-api/string-utils.h>
#include <dali/internal/event/rendering/shader-impl.h> // Dali::Internal::Shader

using Dali::Integration::ToDaliString;

namespace Dali
{
Shader Shader::New(Dali::StringView vertexShader,
                   Dali::StringView fragmentShader,
                   Hint::Value      hints,
                   Dali::StringView shaderName)
{
  Internal::ShaderPtr shader = Internal::Shader::New(vertexShader, fragmentShader, hints, shaderName, {}, false);
  return Shader(shader.Get());
}

Shader Shader::New(Dali::Property::Value shaderMap)
{
  Internal::ShaderPtr shader = Internal::Shader::New(std::move(shaderMap), {}, false);
  return Shader(shader.Get());
}

Shader::Shader() = default;

Shader::~Shader() = default;

Shader::Shader(const Shader& handle) = default;

Shader Shader::DownCast(BaseHandle handle)
{
  return Shader(dynamic_cast<Dali::Internal::Shader*>(handle.GetObjectPtr()));
}

Shader& Shader::operator=(const Shader& handle) = default;

Shader::Shader(Shader&& rhs) noexcept = default;

Shader& Shader::operator=(Shader&& rhs) noexcept = default;

Shader::Shader(Internal::Shader* pointer)
: Handle(pointer)
{
}

uint32_t Shader::GetShaderLanguageVersion()
{
  return Dali::Internal::Shader::GetShaderLanguageVersion();
}

Dali::String Shader::GetShaderVersionPrefix()
{
  return ToDaliString(Dali::Internal::Shader::GetShaderVersionPrefix());
}

Dali::String Shader::GetVertexShaderPrefix()
{
  return ToDaliString(Dali::Internal::Shader::GetVertexShaderPrefix());
}

Dali::String Shader::GetFragmentShaderPrefix()
{
  return ToDaliString(Dali::Internal::Shader::GetFragmentShaderPrefix());
}

} // namespace Dali
