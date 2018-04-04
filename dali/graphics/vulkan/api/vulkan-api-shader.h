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

protected:
  // derived types should not be moved direcly to prevent slicing
  Shader( Shader&& ) = default;
  Shader& operator=( Shader&& ) = default;

  Shader() = default;

private:

  Vulkan::Graphics& mGraphics;
  Vulkan::ShaderRef mVertexShader;
  Vulkan::ShaderRef mFragmentShader;


};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
#endif //DALI_GRAPHICS_VULKAN_API_SHADER_H
