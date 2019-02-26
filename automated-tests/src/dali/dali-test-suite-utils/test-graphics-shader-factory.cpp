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

#include "test-graphics-shader-factory.h"

namespace Test
{
GraphicsShaderFactory::GraphicsShaderFactory(GraphicsController* controller)
: mController(*controller)
{
}
GraphicsShaderFactory::~GraphicsShaderFactory() = default;

Dali::Graphics::ShaderFactory& GraphicsShaderFactory::SetShaderModule(
  Dali::Graphics::ShaderDetails::PipelineStage pipelineStage,
  Dali::Graphics::ShaderDetails::Language language,
  const Dali::Graphics::ShaderDetails::ShaderSource& source )
{
  mCreateInfo.mPipelineStage = pipelineStage;
  mCreateInfo.mLanguage = language;
  mCreateInfo.mSource = source;
  return *this;
}

Dali::Graphics::ShaderFactory::PointerType GraphicsShaderFactory::Create() const
{
  return std::unique_ptr<GraphicsShader>(new GraphicsShader( mController, mCreateInfo));
}

void GraphicsShaderFactory::TestReset()
{
  mCreateInfo = GraphicsShaderCreateInfo { };
}

} // Test
