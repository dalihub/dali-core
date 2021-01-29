#ifndef DALI_TEST_GRAPHICS_PIPELINE_H
#define DALI_TEST_GRAPHICS_PIPELINE_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

#include <dali/graphics-api/graphics-pipeline-create-info.h>
#include <dali/graphics-api/graphics-pipeline.h>
#include "test-gl-abstraction.h"

namespace Dali
{
class TestGraphicsPipeline : public Graphics::Pipeline
{
public:
  TestGraphicsPipeline(TestGlAbstraction& gl, const Graphics::PipelineCreateInfo& createInfo);

  TestGlAbstraction& mGl;

  Graphics::ColorBlendState          colorBlendState;
  std::vector<Graphics::ShaderState> shaderState;
  Graphics::ViewportState            viewportState;
  Graphics::FramebufferState         framebufferState;
  Graphics::Pipeline                 basePipeline;
  Graphics::DepthStencilState        depthStencilState;
  Graphics::RasterizationState       rasterizationState;
  Graphics::VertexInputState         vertexInputState;
  Graphics::InputAssemblyState       inputAssemblyState;
  Graphics::PipelineDynamicStateMask dynamicStateMask{0u};
};

} // namespace Dali

#endif //DALI_TEST_GRAPHICS_PIPELINE_H
