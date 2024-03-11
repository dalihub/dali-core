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

#include <dali/internal/update/rendering/shader-cache.h>
#include <dali/graphics-api/graphics-controller.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

ShaderCache::ShaderCache( Dali::Graphics::Controller& controller )
: mController( controller )
{
}

Dali::Graphics::Program& ShaderCache::GetShader(
  const std::vector<char>& vertexShaderSource,
  const std::vector<char>& fragmentShaderSource)
{
  for( auto&& item : mItems )
  {
    if( item.vertexSource == vertexShaderSource && item.fragmentSource == fragmentShaderSource )
    {
      return *item.program.get();
    }
  }

  Graphics::ShaderCreateInfo vertexShaderCreateInfo;
  vertexShaderCreateInfo.SetPipelineStage(Graphics::PipelineStage::VERTEX_SHADER);
  vertexShaderCreateInfo.SetSourceMode(Graphics::ShaderSourceMode::BINARY);
  vertexShaderCreateInfo.SetSourceSize(vertexShaderSource.size());
  vertexShaderCreateInfo.SetSourceData(static_cast<const void*>(vertexShaderSource.data()));
  auto vertexShader = mController.CreateShader(vertexShaderCreateInfo, nullptr);

  Graphics::ShaderCreateInfo fragmentShaderCreateInfo;
  fragmentShaderCreateInfo.SetPipelineStage(Graphics::PipelineStage::FRAGMENT_SHADER);
  fragmentShaderCreateInfo.SetSourceMode(Graphics::ShaderSourceMode::BINARY);
  fragmentShaderCreateInfo.SetSourceSize(fragmentShaderSource.size());
  fragmentShaderCreateInfo.SetSourceData(static_cast<const void*>(fragmentShaderSource.data()));
  auto fragmentShader = mController.CreateShader(fragmentShaderCreateInfo, nullptr);

  std::vector<Graphics::ShaderState> shaderStates{
    Graphics::ShaderState()
    .SetShader(*vertexShader.get())
    .SetPipelineStage(Graphics::PipelineStage::VERTEX_SHADER),
    Graphics::ShaderState()
    .SetShader(*fragmentShader.get())
    .SetPipelineStage(Graphics::PipelineStage::FRAGMENT_SHADER)};

  auto createInfo = Graphics::ProgramCreateInfo();
  createInfo.SetShaderState(shaderStates);

  auto graphicsProgram = mController.CreateProgram(createInfo, nullptr);
  mItems.emplace_back(std::move(graphicsProgram), vertexShaderSource, fragmentShaderSource);
  return *mItems.back().program.get();
}

void ShaderCache::DestroyGraphicsObjects()
{
  mItems.clear();
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
