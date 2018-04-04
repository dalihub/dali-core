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
#include <dali/graphics-api/graphics-api-shader-details.h>
#include <dali/graphics/vulkan/vulkan-shader.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{
using namespace Dali::Graphics::API;
using namespace Dali::Graphics::Vulkan;

using ShaderLanguage = Dali::Graphics::API::ShaderDetails::Language;
using ShaderPipelineStage = Dali::Graphics::API::ShaderDetails::PipelineStage;
using ShaderSource = Dali::Graphics::API::ShaderDetails::ShaderSource;

Shader::Shader( Vulkan::Graphics& graphics ) :
mGraphics( graphics )
{

}

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
    return Vulkan::ShaderRef( );
  }
}

bool Shader::AddShaderModule( Dali::Graphics::API::ShaderDetails::PipelineStage pipelineStage,
                              Dali::Graphics::API::ShaderDetails::Language language,
                              Dali::Graphics::API::ShaderDetails::ShaderSource shaderSource )

{
  // TODO: AB: only supported language is SPIRV for now
  if( language != ShaderLanguage::SPIRV_1_0 && language != ShaderLanguage::SPIRV_1_1 )
  {
    // unsupported language
    return false;
  }

  // TODO: AB: only binary shader supported for now
  if( shaderSource.type != ShaderDetails::ShaderSourceType::BINARY )
  {
    return false;
  }

  auto shaderRef = Vulkan::Shader::New( mGraphics, &shaderSource.code[0], shaderSource.code.size() );

  if(!shaderRef)
  {
    return false;
  }

  if(pipelineStage == ShaderPipelineStage::VERTEX )
  {
    mVertexShader = shaderRef;
    return true;
  }
  else if(pipelineStage == ShaderPipelineStage::FRAGMENT )
  {
    mFragmentShader = shaderRef;
    return true;
  }

  // possibly unsupported pipeline stage
  return false;
}



} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali