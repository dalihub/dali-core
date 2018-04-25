/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/graphics-api/graphics-api-controller.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

ShaderCache::ShaderCache( Dali::Graphics::API::Controller& controller )
: mController( controller )
{
}

Dali::Graphics::API::Accessor<Dali::Graphics::API::Shader> ShaderCache::GetShader(
  const Dali::Graphics::API::ShaderDetails::ShaderSource& vsh,
  const Dali::Graphics::API::ShaderDetails::ShaderSource& fsh )
{
  for( auto&& item : mItems )
  {
    if( item.vertexSource == vsh && item.fragmentSource == fsh )
    {
      return item.shader;
    }
  }
  auto shaderRef =
    mController.CreateShader( mController.GetShaderFactory()
                              .SetShaderModule( Graphics::API::ShaderDetails::PipelineStage::VERTEX,
                                                Graphics::API::ShaderDetails::Language::SPIRV_1_0,
                                                vsh )
                              .SetShaderModule( Graphics::API::ShaderDetails::PipelineStage::FRAGMENT,
                                                Graphics::API::ShaderDetails::Language::SPIRV_1_0,
                                                fsh ) );
  mItems.emplace_back( Item() = { shaderRef, vsh, fsh } );
  return shaderRef;
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
