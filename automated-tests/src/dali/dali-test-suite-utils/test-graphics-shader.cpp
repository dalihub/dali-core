/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include "test-graphics-shader.h"

namespace Test
{

GraphicsShader::GraphicsShader( GraphicsController& controller, const GraphicsShaderCreateInfo& createInfo )
: mController(controller),
  mCreateInfo(createInfo)
{
}

GraphicsShader::~GraphicsShader() = default;

bool GraphicsShader::IsReflectionSupported() const
{
  return false;
}

uint32_t GraphicsShader::GetVertexAttributeLocation(const std::string &name) const
{
  return 0;
}

Dali::Graphics::ShaderDetails::VertexInputAttributeFormat GraphicsShader::GetVertexAttributeFormat( uint32_t location ) const
{
  return Dali::Graphics::ShaderDetails::VertexInputAttributeFormat{};
};

std::string GraphicsShader::GetVertexAttributeName(uint32_t location) const
{
  return "";
}

std::vector<uint32_t> GraphicsShader::GetVertexAttributeLocations() const
{
  std::vector<uint32_t> locs = {};
  return locs;
}

std::vector<Dali::Graphics::ShaderDetails::UniformInfo> GraphicsShader::GetSamplers() const
{
  std::vector<Dali::Graphics::ShaderDetails::UniformInfo> ui;
  return ui;
}

bool GraphicsShader::GetNamedUniform( const std::string& name, Dali::Graphics::ShaderDetails::UniformInfo& out ) const
{
  return false;
}

uint32_t GraphicsShader::GetUniformBlockCount() const
{
  return 0;
}

uint32_t GraphicsShader::GetUniformBlockBinding( uint32_t index ) const
{
  return 0;
}

uint32_t GraphicsShader::GetUniformBlockSize( uint32_t index ) const
{
  return 0;
}

bool GraphicsShader::GetUniformBlock( uint32_t index, Dali::Graphics::ShaderDetails::UniformBlockInfo& out ) const
{
  return false;
}


} // namespace Test
