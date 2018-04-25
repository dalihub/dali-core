#ifndef DALI_GRAPHICS_VULKAN_API_SHADER_H
#define DALI_GRAPHICS_VULKAN_API_SHADER_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/graphics-api/graphics-api-shader-details.h>
#include <dali/graphics/vulkan/vulkan-types.h>
#include <dali/graphics/vulkan/spirv/vulkan-spirv.h>

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{
/**
 * @brief API::Shader represents program conceptually
 */
class Shader : public Dali::Graphics::API::Shader
{
public:
  // not copyable
  Shader( const Shader& ) = delete;
  Shader& operator=( const Shader& ) = delete;

  ~Shader() override = default;

  explicit Shader( Vulkan::Graphics& graphics );

  bool AddShaderModule( Graphics::API::ShaderDetails::PipelineStage pipelineStage,
          Graphics::API::ShaderDetails::Language language,
          Graphics::API::ShaderDetails::ShaderSource shaderSource );

  Vulkan::ShaderRef GetShader( vk::ShaderStageFlagBits shaderStage ) const ;

  static Shader& DownCast( Dali::Graphics::API::Shader& shader );

  Vulkan::ShaderRef GetShaderRef( vk::ShaderStageFlagBits shaderStage ) const;


  // REFLECTION

  // Vertex attributes
  bool IsReflectionSupported() const override;

  uint32_t GetVertexAttributeLocation(const std::string &name) const override;

  API::ShaderDetails::VertexInputAttributeFormat GetVertexAttributeFormat( uint32_t location ) const override;

  std::string GetVertexAttributeName(uint32_t location) const override;

  std::vector<uint32_t> GetVertexAttributeLocations() const override;

  /**
   * Uniforms
   */

  std::vector<uint32_t> GetUniformBlockLocations() const;

  std::string GetUniformBlockName( uint32_t blockIndex) const;

  uint32_t GetUniformBlockMemberCount( uint32_t blockIndex ) const;

  std::string GetUniformBlockMemberName( uint32_t blockIndex, uint32_t memberLocation ) const;

  uint32_t GetUniformBlockMemberOffset( uint32_t blockIndex, uint32_t memberLocation ) const;


  bool GetNamedUniform( const std::string& name, API::ShaderDetails::UniformInfo& out ) const override;

  std::vector<API::ShaderDetails::UniformInfo> GetSamplers() const override;

  uint32_t GetUniformBlockCount() const override;

  bool GetUniformBlock( uint32_t index, API::ShaderDetails::UniformBlockInfo& out ) const override;

protected:
  // derived types should not be moved direcly to prevent slicing
  Shader( Shader&& ) = default;
  Shader& operator=( Shader&& ) = default;

  Shader() = default;

private:

  Vulkan::Graphics& mGraphics;
  Vulkan::ShaderRef mVertexShader;
  Vulkan::ShaderRef mFragmentShader;

  std::vector<Vulkan::SpirV::SPIRVVertexInputAttribute> mVertexInputAttributes;
  std::vector<Vulkan::SpirV::SPIRVUniformBlock> mUniformBlocks;
  std::vector<Vulkan::SpirV::SPIRVUniformOpaque> mUniformOpaques;
};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
#endif //DALI_GRAPHICS_VULKAN_API_SHADER_H
