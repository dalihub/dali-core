#ifndef TEST_GRAPHICS_SHADER_H
#define TEST_GRAPHICS_SHADER_H

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

#include <dali/graphics-api/graphics-api-shader.h>
#include <dali/graphics-api/graphics-api-types.h>

namespace Test
{
class GraphicsController;

struct GraphicsShaderCreateInfo
{
  Dali::Graphics::ShaderDetails::PipelineStage mPipelineStage;
  Dali::Graphics::ShaderDetails::Language      mLanguage;
  Dali::Graphics::ShaderDetails::ShaderSource  mSource;

  GraphicsShaderCreateInfo()
  : mPipelineStage{},
    mLanguage{},
    mSource("")
  {
  }
};

class GraphicsShader : public Dali::Graphics::Shader
{
public:
  explicit GraphicsShader( GraphicsController& controller, const GraphicsShaderCreateInfo& createInfo );

  ~GraphicsShader() override;

  bool IsReflectionSupported() const override;

  uint32_t GetVertexAttributeLocation(const std::string &name) const override;

  Dali::Graphics::ShaderDetails::VertexInputAttributeFormat GetVertexAttributeFormat( uint32_t location ) const override;

  std::string GetVertexAttributeName(uint32_t location) const override;

  std::vector<uint32_t> GetVertexAttributeLocations() const override;

  std::vector<Dali::Graphics::ShaderDetails::UniformInfo> GetSamplers() const override;

  bool GetNamedUniform( const std::string& name, Dali::Graphics::ShaderDetails::UniformInfo& out ) const override;

  uint32_t GetUniformBlockCount() const override;

  uint32_t GetUniformBlockBinding( uint32_t index ) const override;

  uint32_t GetUniformBlockSize( uint32_t index ) const override;

  bool GetUniformBlock( uint32_t index, Dali::Graphics::ShaderDetails::UniformBlockInfo& out ) const override;

public:
  GraphicsController& mController;
  GraphicsShaderCreateInfo mCreateInfo;
};

} // namespace Test

#endif // TEST_GRAPHICS_SHADER_H
