#ifndef DALI_GRAPHICS_VULKAN_API_PIPELINE_FACTORY_H
#define DALI_GRAPHICS_VULKAN_API_PIPELINE_FACTORY_H

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

#include <dali/graphics-api/graphics-api-pipeline-factory.h>

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
class Controller;

class Pipeline;

class PipelineCache;

/**
 * @brief Interface class for ShaderFactory types in the graphics API.
 */
class PipelineFactory : public API::PipelineFactory
{
public:

  PipelineFactory( Controller& controller );

  API::PipelineFactory& SetPipelineCache( VulkanAPI::PipelineCache& pipelineCache );

  API::PipelineFactory& SetColorBlendState( const API::ColorBlendState& state ) override;

  API::PipelineFactory& SetShaderState( const API::ShaderState& state ) override;

  API::PipelineFactory& SetViewportState( const API::ViewportState& state ) override;

  API::PipelineFactory& SetBasePipeline( API::Pipeline& pipeline ) override;

  API::PipelineFactory& SetDepthStencilState( API::DepthStencilState state ) override;

  API::PipelineFactory& SetRasterizationState( const API::RasterizationState& state ) override;

  API::PipelineFactory& SetVertexInputState( const API::VertexInputState& state ) override;

  API::PipelineFactory& SetInputAssemblyState( const API::InputAssemblyState& state ) override;

  API::PipelineFactory& SetDynamicStateMask( const API::PipelineDynamicStateMask mask );

  // To be called when getting new factory
  void Reset();

  std::unique_ptr< API::Pipeline > Create() const override;

  uint32_t GetHashCode() const;

  struct Info
  {
    API::DepthStencilState depthStencilState;
    API::ColorBlendState colorBlendState;
    API::ShaderState shaderState;
    API::ViewportState viewportState;
    API::RasterizationState rasterizationState;
    API::VertexInputState vertexInputState;
    API::InputAssemblyState inputAssemblyState;
    API::PipelineDynamicStateMask dynamicStateMask;
  };

  const Info& GetCreateInfo() const
  {
    return mInfo;
  }

public:

  Info mInfo;
  Controller& mController;
  Vulkan::Graphics& mGraphics;
  VulkanAPI::PipelineCache* mPipelineCache;
  VulkanAPI::Pipeline* mBasePipeline;
  mutable uint32_t mHashCode;
};

} // namespace API
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_API_PIPELINE_FACTORY_H

