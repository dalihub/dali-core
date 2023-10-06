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

#include <dali/graphics-api/graphics-controller.h>
#include <dali/internal/render/renderers/shader-cache.h>

namespace Dali
{
namespace Internal
{
namespace Render
{
ShaderCache::ShaderCache(Dali::Graphics::Controller& controller)
: mController(controller)
{
}

Dali::Graphics::Shader& ShaderCache::GetShader(const std::vector<char>& shaderCode, Graphics::PipelineStage stage, Graphics::ShaderSourceMode type)
{
  for(auto&& item : mItems)
  {
    if(item.shaderCode == shaderCode && item.stage == stage && item.type == type)
    {
      return *item.shader.get();
    }
  }

  Graphics::ShaderCreateInfo shaderCreateInfo;
  shaderCreateInfo.SetPipelineStage(stage);
  shaderCreateInfo.SetSourceData(static_cast<const void*>(shaderCode.data()));
  shaderCreateInfo.SetSourceSize(shaderCode.size());
  shaderCreateInfo.SetSourceMode(type);

  Graphics::UniquePtr<Graphics::Shader> shader = mController.CreateShader(shaderCreateInfo, nullptr);

  mItems.emplace_back(std::move(shader), shaderCode, stage, type);
  return *mItems.back().shader.get();
}

} // namespace Render
} // namespace Internal
} // namespace Dali
