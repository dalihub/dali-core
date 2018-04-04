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

#include <dali/graphics/vulkan/api/vulkan-api-shader-factory.h>
#include <dali/graphics/vulkan/api/vulkan-api-shader.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-shader.h>

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{
ShaderFactory::ShaderFactory( Vulkan::Graphics& graphics ) :
mGraphics( graphics )
{
}

ShaderFactory::~ShaderFactory() = default;

ShaderFactory& ShaderFactory::SetShaderModule( Graphics::API::ShaderDetails::PipelineStage       pipelineStage,
                                         Graphics::API::ShaderDetails::Language            language,
                                         const Graphics::API::ShaderDetails::ShaderSource& source )
{
  using PipelineStage = Graphics::API::ShaderDetails::PipelineStage;
  if( pipelineStage == PipelineStage::VERTEX )
  {
    mVertexShader = ShaderModuleInfo( pipelineStage, language, source );
  }
  else if( pipelineStage == PipelineStage::FRAGMENT )
  {
    mFragmentShader = ShaderModuleInfo( pipelineStage, language, source );
  }
  return *this;
}

std::unique_ptr<Graphics::API::Shader> ShaderFactory::Create() const
{
  if( !(mVertexShader.source.IsSet() && mFragmentShader.source.IsSet()) )
  {
    return nullptr;
  }

  auto retval = std::make_unique<VulkanAPI::Shader>( mGraphics );

  // add vertex shader
  retval->AddShaderModule( mVertexShader.pipelineStage, mVertexShader.language, mVertexShader.source );

  // add fragment shader
  retval->AddShaderModule( mFragmentShader.pipelineStage, mFragmentShader.language, mFragmentShader.source );

  return std::move(retval);
}

} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali
