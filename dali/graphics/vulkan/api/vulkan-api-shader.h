#ifndef DALI_VULKAN_161117_VULKAN_API_SHADER_H
#define DALI_VULKAN_161117_VULKAN_API_SHADER_H

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
class VulkanShader : public Dali::Graphics::API::Shader
{
public:
  // not copyable
  VulkanShader( const VulkanShader& ) = delete;
  VulkanShader& operator=( const VulkanShader& ) = delete;

  ~VulkanShader() override = default;

  Vulkan::ShaderRef GetShader( vk::ShaderStageFlagBits shaderStage ) const ;

  static VulkanShader& DownCast( Dali::Graphics::API::Shader& shader );

protected:
  // derived types should not be moved direcly to prevent slicing
  VulkanShader( VulkanShader&& ) = default;
  VulkanShader& operator=( VulkanShader&& ) = default;

  VulkanShader() = default;

private:

  Vulkan::ShaderRef mVertexShader;
  Vulkan::ShaderRef mFragmentShader;
};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
#endif //DALI_VULKAN_161117_VULKAN_API_SHADER_H
