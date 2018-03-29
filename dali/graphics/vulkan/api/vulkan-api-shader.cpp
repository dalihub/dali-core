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

#include <dali/graphics/vulkan/api/vulkan-api-shader.h>
#include <dali/graphics/vulkan/vulkan-shader.h>
namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{
using namespace Dali::Graphics::API;
using namespace Dali::Graphics::Vulkan;

ShaderRef Shader::GetShader( vk::ShaderStageFlagBits shaderStage ) const
{
  switch( shaderStage )
  {
    case vk::ShaderStageFlagBits::eVertex:
    {
      return mVertexShader;
    }
    case vk::ShaderStageFlagBits::eFragment:
    {
      return mFragmentShader;
    }
    case vk::ShaderStageFlagBits::eTessellationControl:
    case vk::ShaderStageFlagBits::eTessellationEvaluation:
    case vk::ShaderStageFlagBits::eGeometry:
    case vk::ShaderStageFlagBits::eCompute:
    case vk::ShaderStageFlagBits::eAllGraphics:
    case vk::ShaderStageFlagBits::eAll:
    {
    }
  }
  return ShaderRef();
}

Shader& Shader::DownCast( Dali::Graphics::API::Shader& shader )
{
  return *( dynamic_cast<Shader*>( &shader ) );
}

Vulkan::ShaderRef Shader::GetShaderRef( vk::ShaderStageFlagBits shaderStage ) const
{
  if(shaderStage == vk::ShaderStageFlagBits::eVertex)
  {
    return mVertexShader;
  }
  else if(shaderStage == vk::ShaderStageFlagBits::eFragment)
  {
    return mFragmentShader;
  }
  else
  {
    return Vulkan::ShaderRef();
  }
}

} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali