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
 *
 */

// CLASS HEADER
#include <dali/internal/update/rendering/scene-graph-shader.h>

// INTERNAL INCLUDES
#define DEBUG_OVERRIDE_VULKAN_SHADER
#ifdef DEBUG_OVERRIDE_VULKAN_SHADER
#include <dali/graphics/vulkan/generated/spv-shaders-gen.h>
#endif

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

Shader::Shader( Dali::Shader::Hint::Value& hints )
: mGraphics( nullptr ),
  mGraphicsShader( nullptr ),
  mShaderCache( nullptr ),
  mHints( hints ),
  mConnectionObservers()
{
  AddUniformMapObserver( *this );
}

Shader::~Shader()
{
  mConnectionObservers.Destroy( *this );
}

void Shader::Initialize( Integration::Graphics::Graphics& graphics, ShaderCache& shaderCache )
{
  mGraphics = &graphics;
  mShaderCache = &shaderCache;
}

Graphics::API::Accessor<Graphics::API::Shader>& Shader::GetGfxObject()
{
  return mGraphicsShader;
}

void Shader::AddConnectionObserver( ConnectionChangePropagator::Observer& observer )
{
  mConnectionObservers.Add(observer);
}

void Shader::RemoveConnectionObserver( ConnectionChangePropagator::Observer& observer )
{
  mConnectionObservers.Remove(observer);
}

void Shader::UniformMappingsChanged( const UniformMap& mappings )
{
  // Our uniform map, or that of one of the watched children has changed.
  // Inform connected observers.
  mConnectionObservers.ConnectedUniformMapChanged();
}

void Shader::SetShaderProgram( Internal::ShaderDataPtr shaderData, bool modifiesGeometry )
{
  // TODO: for now we will use hardcoded binary SPIRV shaders which will replace anything
  // that is passed by the caller
#ifdef DEBUG_OVERRIDE_VULKAN_SHADER

  mGraphicsShader = mShaderCache->GetShader( Graphics::API::ShaderDetails::ShaderSource( VSH_IMAGE_VISUAL_CODE ),
                                             Graphics::API::ShaderDetails::ShaderSource( FSH_IMAGE_VISUAL_CODE ));

#else
  auto& controller = mGraphics->GetController();

  mGraphicsShader = controller.CreateShader(
    controller.GetShaderFactory()
    .SetShaderModule( Graphics::API::ShaderDetails::PipelineStage::VERTEX,
                      Graphics::API::ShaderDetails::Language::SPIRV_1_0,
                      Graphics::API::ShaderDetails::ShaderSource( shaderData->GetVertexShader() ))
    .SetShaderModule( Graphics::API::ShaderDetails::PipelineStage::FRAGMENT,
                      Graphics::API::ShaderDetails::Language::SPIRV_1_0,
                      Graphics::API::ShaderDetails::ShaderSource( shaderData->GetFragmentShader() )) );
#endif
}


} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
