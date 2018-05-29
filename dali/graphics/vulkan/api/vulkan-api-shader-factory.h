#ifndef DALI_GRAPHICS_VULKAN_API_SHADER_FACTORY_H
#define DALI_GRAPHICS_VULKAN_API_SHADER_FACTORY_H

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
#include <dali/graphics-api/graphics-api-shader-factory.h>
#include <dali/graphics-api/graphics-api-shader-details.h>
#include <dali/graphics-api/graphics-api-shader.h>

#include <vector>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class Graphics;
}
namespace VulkanAPI
{

class ShaderFactory : public Dali::Graphics::API::ShaderFactory
{
public:
  explicit ShaderFactory( Vulkan::Graphics& graphics );

  ShaderFactory& SetShaderModule(Graphics::API::ShaderDetails::PipelineStage       pipelineStage,
                                 Graphics::API::ShaderDetails::Language            language,
                                 const Graphics::API::ShaderDetails::ShaderSource& source) override;

  PointerType Create() const override;

  ~ShaderFactory() override;

private:

  struct ShaderModuleInfo
  {
    using PipelineStage = Graphics::API::ShaderDetails::PipelineStage;
    using Language = Graphics::API::ShaderDetails::Language;
    using ShaderSource = Graphics::API::ShaderDetails::ShaderSource;

    PipelineStage pipelineStage;
    Language language;
    ShaderSource source;

    ShaderModuleInfo( PipelineStage stage, Language shaderLanguage, const ShaderSource& shaderSource )
    : pipelineStage( stage ),
      language( shaderLanguage ),
      source( shaderSource )
    {
    }

    ShaderModuleInfo()
      : pipelineStage( PipelineStage::VERTEX ),
        language( Language::SPIRV_1_0 ),
        source( ShaderSource(std::vector<char>()) )
    {
    }

    ShaderModuleInfo( const ShaderModuleInfo& info ) = default;

    ~ShaderModuleInfo() = default;
  };

  Vulkan::Graphics& mGraphics;
  ShaderModuleInfo mVertexShader;
  ShaderModuleInfo mFragmentShader;
};


}
}
}

#endif // DALI_GRAPHICS_VULKAN_API_SHADER_FACTORY_H
