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

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

Shader::Shader( Dali::Shader::Hint::Value& hints )
: mHints( hints ),
  mConnectionObservers(),
  mGfxShader( nullptr )
{
  AddUniformMapObserver( *this );
}

Shader::~Shader()
{
  mConnectionObservers.Destroy( *this );
}

void Shader::SetGfxObject( const Graphics::API::Accessor<Graphics::API::Shader>& shader )
{
  mGfxShader = shader;
}

Graphics::API::Accessor<Graphics::API::Shader>& Shader::GetGfxObject()
{
  return mGfxShader;
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

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
