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

#include "test-graphics-pipeline-factory.h"

namespace Test
{

GraphicsPipelineFactory::GraphicsPipelineFactory() = default;
GraphicsPipelineFactory::~GraphicsPipelineFactory() = default;

Dali::Graphics::API::PipelineFactory& GraphicsPipelineFactory::SetColorBlendState( const Dali::Graphics::API::ColorBlendState& state )
{
  return *this;
}

Dali::Graphics::API::PipelineFactory& GraphicsPipelineFactory::SetShaderState( const Dali::Graphics::API::ShaderState& state )
{
  return *this;
}

Dali::Graphics::API::PipelineFactory& GraphicsPipelineFactory::SetViewportState( const Dali::Graphics::API::ViewportState& state )
{
  return *this;
}


Dali::Graphics::API::PipelineFactory& GraphicsPipelineFactory::SetFramebufferState( const Dali::Graphics::API::FramebufferState& state )
{
  return *this;
}

Dali::Graphics::API::PipelineFactory& GraphicsPipelineFactory::SetBasePipeline( Dali::Graphics::API::Pipeline& pipeline )
{
  return *this;
}

Dali::Graphics::API::PipelineFactory& GraphicsPipelineFactory::SetDepthStencilState( Dali::Graphics::API::DepthStencilState state )
{
  return *this;
}

Dali::Graphics::API::PipelineFactory& GraphicsPipelineFactory::SetRasterizationState( const Dali::Graphics::API::RasterizationState& state )
{
  return *this;
}


Dali::Graphics::API::PipelineFactory& GraphicsPipelineFactory::SetVertexInputState( const Dali::Graphics::API::VertexInputState& state )
{
  return *this;
}


Dali::Graphics::API::PipelineFactory& GraphicsPipelineFactory::SetInputAssemblyState( const Dali::Graphics::API::InputAssemblyState& state )
{
  return *this;
}


Dali::Graphics::API::PipelineFactory& GraphicsPipelineFactory::SetDynamicStateMask( const Dali::Graphics::API::PipelineDynamicStateMask mask )
{
  return *this;
}


Dali::Graphics::API::PipelineFactory& GraphicsPipelineFactory::SetOldPipeline( std::unique_ptr<Dali::Graphics::API::Pipeline> oldPipeline )
{
  return *this;
}


std::unique_ptr<Dali::Graphics::API::Pipeline> GraphicsPipelineFactory::Create()
{
  return nullptr;
}

} // Test
