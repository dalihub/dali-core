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

GraphicsPipelineFactory::GraphicsPipelineFactory(GraphicsController* controller)
: mController(*controller)
{
}
GraphicsPipelineFactory::~GraphicsPipelineFactory() = default;

Dali::Graphics::PipelineFactory& GraphicsPipelineFactory::SetColorBlendState( const Dali::Graphics::ColorBlendState& state )
{
  mCreateInfo.mColorBlendState = state;
  return *this;
}

Dali::Graphics::PipelineFactory& GraphicsPipelineFactory::SetShaderState( const Dali::Graphics::ShaderState& state )
{
  mCreateInfo.mShaderState = state;
  return *this;
}

Dali::Graphics::PipelineFactory& GraphicsPipelineFactory::SetViewportState( const Dali::Graphics::ViewportState& state )
{
  mCreateInfo.mViewportState = state;
  return *this;
}


Dali::Graphics::PipelineFactory& GraphicsPipelineFactory::SetFramebufferState( const Dali::Graphics::FramebufferState& state )
{
  mCreateInfo.mFramebufferState = state;
  return *this;
}

Dali::Graphics::PipelineFactory& GraphicsPipelineFactory::SetBasePipeline( Dali::Graphics::Pipeline& pipeline )
{
  mBasePipeline = dynamic_cast<Test::GraphicsPipeline*>(&pipeline);
  return *this;
}

Dali::Graphics::PipelineFactory& GraphicsPipelineFactory::SetDepthStencilState( Dali::Graphics::DepthStencilState state )
{
  mCreateInfo.mDepthStencilState = state;
  return *this;
}

Dali::Graphics::PipelineFactory& GraphicsPipelineFactory::SetRasterizationState( const Dali::Graphics::RasterizationState& state )
{
  mCreateInfo.mRasterizationState = state;
  return *this;
}


Dali::Graphics::PipelineFactory& GraphicsPipelineFactory::SetVertexInputState( const Dali::Graphics::VertexInputState& state )
{
  mCreateInfo.mVertexInputState = state;
  return *this;
}


Dali::Graphics::PipelineFactory& GraphicsPipelineFactory::SetInputAssemblyState( const Dali::Graphics::InputAssemblyState& state )
{
  mCreateInfo.mInputAssemblyState = state;
  return *this;
}


Dali::Graphics::PipelineFactory& GraphicsPipelineFactory::SetDynamicStateMask( const Dali::Graphics::PipelineDynamicStateMask mask )
{
  mCreateInfo.mDynamicStateMask = mask;
  return *this;
}


Dali::Graphics::PipelineFactory& GraphicsPipelineFactory::SetOldPipeline( std::unique_ptr<Dali::Graphics::Pipeline> oldPipeline )
{
  mOldPipeline = std::move(oldPipeline);
  return *this;
}


std::unique_ptr<Dali::Graphics::Pipeline> GraphicsPipelineFactory::Create()
{

  if( mUseOldPipeline )
  {
    return std::move( mOldPipeline );
  }
  else
  {
    return std::unique_ptr< Dali::Graphics::Pipeline>( new GraphicsPipeline( mController, mCreateInfo ) );
  }

  return nullptr;
}

void GraphicsPipelineFactory::TestReset()
{
  mCreateInfo.mDepthStencilState = {};
  mCreateInfo.mColorBlendState = {};
  mCreateInfo.mShaderState = {};
  mCreateInfo.mViewportState = {};
  mCreateInfo.mFramebufferState = {};
  mCreateInfo.mRasterizationState = {};
  mCreateInfo.mVertexInputState = {};
  mCreateInfo.mInputAssemblyState = {};
  mCreateInfo.mDynamicStateMask = 0u;
  mBasePipeline = nullptr;

  if( mOldPipeline )
  {
    mOldPipeline.reset();
  }
}


} // Test
