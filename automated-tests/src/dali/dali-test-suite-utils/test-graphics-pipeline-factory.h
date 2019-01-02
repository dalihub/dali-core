#ifndef TEST_GRAPHICS_PIPELINE_FACTORY_H
#define TEST_GRAPHICS_PIPELINE_FACTORY_H

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
 */

#include <dali/graphics-api/graphics-api-pipeline-factory.h>

namespace Test
{

class GraphicsPipelineFactory : public Dali::Graphics::API::PipelineFactory
{
public:
  GraphicsPipelineFactory();
  virtual ~GraphicsPipelineFactory();

  Dali::Graphics::API::PipelineFactory& SetColorBlendState( const Dali::Graphics::API::ColorBlendState& state ) override;

  Dali::Graphics::API::PipelineFactory& SetShaderState( const Dali::Graphics::API::ShaderState& state ) override;

  Dali::Graphics::API::PipelineFactory& SetViewportState( const Dali::Graphics::API::ViewportState& state ) override;

  Dali::Graphics::API::PipelineFactory& SetFramebufferState( const Dali::Graphics::API::FramebufferState& state ) override;

  Dali::Graphics::API::PipelineFactory& SetBasePipeline( Dali::Graphics::API::Pipeline& pipeline ) override;

  Dali::Graphics::API::PipelineFactory& SetDepthStencilState( Dali::Graphics::API::DepthStencilState state ) override;

  Dali::Graphics::API::PipelineFactory& SetRasterizationState( const Dali::Graphics::API::RasterizationState& state ) override;

  Dali::Graphics::API::PipelineFactory& SetVertexInputState( const Dali::Graphics::API::VertexInputState& state ) override;

  Dali::Graphics::API::PipelineFactory& SetInputAssemblyState( const Dali::Graphics::API::InputAssemblyState& state ) override;

  Dali::Graphics::API::PipelineFactory& SetDynamicStateMask( const Dali::Graphics::API::PipelineDynamicStateMask mask ) override;

  Dali::Graphics::API::PipelineFactory& SetOldPipeline( std::unique_ptr<Dali::Graphics::API::Pipeline> oldPipeline ) override;

  std::unique_ptr<Dali::Graphics::API::Pipeline> Create() override;
};

} // Test

#endif //TEST_GRAPHICS_PIPELINE_FACTORY_H
