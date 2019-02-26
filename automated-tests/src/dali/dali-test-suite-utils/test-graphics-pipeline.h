#ifndef TEST_GRAPHICS_PIPELINE_H
#define TEST_GRAPHICS_PIPELINE_H

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
 *
 */

#include <dali/graphics-api/graphics-api-pipeline.h>

namespace Test
{
class GraphicsController;
class GraphicsPipeline;


struct GraphicsPipelineCreateInfo
{
  Dali::Graphics::ColorBlendState mColorBlendState;
  Dali::Graphics::ShaderState mShaderState;
  Dali::Graphics::ViewportState mViewportState;
  Dali::Graphics::FramebufferState mFramebufferState;
  Test::GraphicsPipeline* mBasePipeline;
  Dali::Graphics::DepthStencilState mDepthStencilState;
  Dali::Graphics::RasterizationState mRasterizationState;
  Dali::Graphics::VertexInputState mVertexInputState;
  Dali::Graphics::InputAssemblyState mInputAssemblyState;
  Dali::Graphics::PipelineDynamicStateMask mDynamicStateMask;
};

class GraphicsPipeline : public Dali::Graphics::Pipeline
{
public:
  explicit GraphicsPipeline( GraphicsController& controller, const GraphicsPipelineCreateInfo& createInfo ) ;

  ~GraphicsPipeline() override;

  bool Equals( const Dali::Graphics::Pipeline& ) const;

public:
  GraphicsController& mController;
  GraphicsPipelineCreateInfo mCreateInfo;
};

} // namespace Test

#endif // TEST_GRAPHICS_PIPELINE_H
