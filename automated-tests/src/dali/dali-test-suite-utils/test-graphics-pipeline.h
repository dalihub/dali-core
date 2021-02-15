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
#include "test-graphics-program.h"
#include "test-graphics-reflection.h"

namespace Dali
{
class TestGraphicsReflection;

template<typename T>
T* Uncast(const Graphics::Program* object)
{
  return const_cast<T*>(static_cast<const T*>(object));
}

class TestGraphicsPipeline : public Graphics::Pipeline
{
public:
  TestGraphicsPipeline(TestGlAbstraction& gl, const Graphics::PipelineCreateInfo& createInfo);

  const TestGraphicsReflection& GetReflection() const
  {
    return Uncast<TestGraphicsProgram>(programState.program)->GetReflection();
  }

public:
  TestGlAbstraction& mGl;

  Graphics::ColorBlendState          colorBlendState;
  Graphics::ProgramState             programState;
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
