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
  mGraphicsShader = mShaderCache->GetShader( Graphics::API::ShaderDetails::ShaderSource( shaderData->GetShaderForStage( ShaderData::ShaderStage::VERTEX ) ),
                                             Graphics::API::ShaderDetails::ShaderSource( shaderData->GetShaderForStage( ShaderData::ShaderStage::FRAGMENT) ) );

}


} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
