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

Dali::Graphics::Shader& ShaderCache::GetShader(
  const Dali::Graphics::ShaderDetails::ShaderSource& vsh,
  const Dali::Graphics::ShaderDetails::ShaderSource& fsh )
{
  for( auto&& item : mItems )
  {
    if( item.vertexSource == vsh && item.fragmentSource == fsh )
    {
      return *item.shader.get();
    }
  }
  auto shader =
    mController.CreateShader( mController.GetShaderFactory()
                              .SetShaderModule( Graphics::ShaderDetails::PipelineStage::VERTEX,
                                                Graphics::ShaderDetails::Language::SPIRV_1_0,
                                                vsh )
                              .SetShaderModule( Graphics::ShaderDetails::PipelineStage::FRAGMENT,
                                                Graphics::ShaderDetails::Language::SPIRV_1_0,
                                                fsh ) );

  auto retval = shader.get();
  mItems.emplace_back( std::move(shader), vsh, fsh );
  return *retval;
}

void ShaderCache::DestroyGraphicsObjects()
{
  mItems.clear();
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
