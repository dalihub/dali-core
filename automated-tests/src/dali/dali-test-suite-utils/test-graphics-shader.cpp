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
#include "test-graphics-controller.h"

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
  mController.mShaderTrace.PushCall("IsReflectionSupported");
  return false;
}

uint32_t GraphicsShader::GetVertexAttributeLocation(const std::string &name) const
{
  Test::TraceCallStack::NamedParams namedParams;
  namedParams["name"] = Dali::Any( name );
  mController.mShaderTrace.PushCall("GetVertexAttributeLocation", namedParams);
  return 0;
}

Dali::Graphics::ShaderDetails::VertexInputAttributeFormat GraphicsShader::GetVertexAttributeFormat( uint32_t location ) const
{
  Test::TraceCallStack::NamedParams namedParams;
  namedParams["name"] = Dali::Any( location );
  mController.mShaderTrace.PushCall("GetVertexAttributeLocation", namedParams);
  return Dali::Graphics::ShaderDetails::VertexInputAttributeFormat{};
};

std::string GraphicsShader::GetVertexAttributeName(uint32_t location) const
{
  Test::TraceCallStack::NamedParams namedParams;
  namedParams["location"] = Dali::Any( location );
  mController.mShaderTrace.PushCall("GetVertexAttributeName", namedParams);

  return "";
}

std::vector<uint32_t> GraphicsShader::GetVertexAttributeLocations() const
{
  mController.mShaderTrace.PushCall("GetVertexAttributeLocations");
  std::vector<uint32_t> locs = {};
  return locs;
}

std::vector<Dali::Graphics::ShaderDetails::UniformInfo> GraphicsShader::GetSamplers() const
{
  mController.mShaderTrace.PushCall("GetSamplers");
  // Matches basic-shader.frag from test-actor-utils.cpp
  std::vector<Dali::Graphics::ShaderDetails::UniformInfo> ui = {
    {"sTexture", Dali::Graphics::ShaderDetails::UniformClass::SAMPLER, 4u, 0u, 0u, 0u},
    {"sTexture2", Dali::Graphics::ShaderDetails::UniformClass::SAMPLER, 2u, 0u, 0u, 0u}};

  return ui;
}

bool GraphicsShader::GetNamedUniform( const std::string& name, Dali::Graphics::ShaderDetails::UniformInfo& out ) const
{
  Test::TraceCallStack::NamedParams namedParams;
  namedParams["name"] = Dali::Any( name );
  mController.mShaderTrace.PushCall("GetNamedUniform", namedParams);
  return false;
}

uint32_t GraphicsShader::GetUniformBlockCount() const
{
  mController.mShaderTrace.PushCall("GetUniformBlockCount");
  return 0;
}

uint32_t GraphicsShader::GetUniformBlockBinding( uint32_t index ) const
{
  Test::TraceCallStack::NamedParams namedParams;
  namedParams["index"] = Dali::Any( index );
  mController.mShaderTrace.PushCall("GetUniformBlockBinding", namedParams);
  return 0;
}

uint32_t GraphicsShader::GetUniformBlockSize( uint32_t index ) const
{
  Test::TraceCallStack::NamedParams namedParams;
  namedParams["index"] = Dali::Any( index );
  mController.mShaderTrace.PushCall("GetUniformBlockSize", namedParams);
  return 0;
}

bool GraphicsShader::GetUniformBlock( uint32_t index, Dali::Graphics::ShaderDetails::UniformBlockInfo& out ) const
{
  Test::TraceCallStack::NamedParams namedParams;
  namedParams["index"] = Dali::Any( index );
  mController.mShaderTrace.PushCall("GetUniformBlock", namedParams);
  return false;
}


} // namespace Test
