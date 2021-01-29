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

#include "test-graphics-pipeline.h"

namespace Dali
{
TestGraphicsPipeline::TestGraphicsPipeline(TestGlAbstraction& gl, const Graphics::PipelineCreateInfo& createInfo)
: mGl(gl)
{
  // Need to deep copy, otherwise pointed at memory will go out of scope. Probably should do something about this.

  if(createInfo.colorBlendState)
    colorBlendState = *createInfo.colorBlendState;

  if(createInfo.shaderState)
    shaderState = *createInfo.shaderState;

  if(createInfo.viewportState)
    viewportState = *createInfo.viewportState;

  if(createInfo.framebufferState)
    framebufferState = *createInfo.framebufferState;

  if(createInfo.depthStencilState)
    depthStencilState = *createInfo.depthStencilState;

  if(createInfo.rasterizationState)
    rasterizationState = *createInfo.rasterizationState;

  if(createInfo.vertexInputState)
    vertexInputState = *createInfo.vertexInputState;

  if(createInfo.inputAssemblyState)
    inputAssemblyState = *createInfo.inputAssemblyState;

  if(createInfo.dynamicStateMask)
    dynamicStateMask = createInfo.dynamicStateMask;
}

} // namespace Dali
