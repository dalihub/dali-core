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

class GraphicsPipelineFactory : public Dali::Graphics::PipelineFactory
{
public:
  GraphicsPipelineFactory();
  virtual ~GraphicsPipelineFactory();

  Dali::Graphics::PipelineFactory& SetColorBlendState( const Dali::Graphics::ColorBlendState& state ) override;

  Dali::Graphics::PipelineFactory& SetShaderState( const Dali::Graphics::ShaderState& state ) override;

  Dali::Graphics::PipelineFactory& SetViewportState( const Dali::Graphics::ViewportState& state ) override;

  Dali::Graphics::PipelineFactory& SetFramebufferState( const Dali::Graphics::FramebufferState& state ) override;

  Dali::Graphics::PipelineFactory& SetBasePipeline( Dali::Graphics::Pipeline& pipeline ) override;

  Dali::Graphics::PipelineFactory& SetDepthStencilState( Dali::Graphics::DepthStencilState state ) override;

  Dali::Graphics::PipelineFactory& SetRasterizationState( const Dali::Graphics::RasterizationState& state ) override;

  Dali::Graphics::PipelineFactory& SetVertexInputState( const Dali::Graphics::VertexInputState& state ) override;

  Dali::Graphics::PipelineFactory& SetInputAssemblyState( const Dali::Graphics::InputAssemblyState& state ) override;

  Dali::Graphics::PipelineFactory& SetDynamicStateMask( const Dali::Graphics::PipelineDynamicStateMask mask ) override;

  Dali::Graphics::PipelineFactory& SetOldPipeline( std::unique_ptr<Dali::Graphics::Pipeline> oldPipeline ) override;

  std::unique_ptr<Dali::Graphics::Pipeline> Create() override;
};

} // Test

#endif //TEST_GRAPHICS_PIPELINE_FACTORY_H
