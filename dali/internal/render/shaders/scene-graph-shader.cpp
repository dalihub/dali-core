/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/internal/render/shaders/scene-graph-shader.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/common/render-debug.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/common/image-sampler.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

Shader::Shader( Dali::Shader::Hint::Value& hints )
: mHints( hints ),
  mProgram( NULL ),
  mConnectionObservers()
{
  AddUniformMapObserver( *this );
}

Shader::~Shader()
{
  mConnectionObservers.Destroy( *this );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The following methods are called during RenderManager::Render()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Shader::SetProgram( Internal::ShaderDataPtr shaderData,
                         ProgramCache* programCache,
                         bool modifiesGeometry )
{
  DALI_LOG_TRACE_METHOD_FMT( Debug::Filter::gShader, "%d\n", shaderData->GetHashValue() );

  mProgram = Program::New( *programCache, shaderData, modifiesGeometry );
  // The program cache owns the Program object so we don't need to worry about this raw allocation here.

  mConnectionObservers.ConnectionsChanged(*this);
}

Program* Shader::GetProgram()
{
  return mProgram;
}

void Shader::AddConnectionObserver( ConnectionChangePropagator::Observer& observer )
{
  mConnectionObservers.Add(observer);
}

void Shader::RemoveConnectionObserver( ConnectionChangePropagator::Observer& observer )
{
  DALI_LOG_ERROR("+++ Scene Graph Shader::RemoveConnectionObserver\n");

  mConnectionObservers.Remove(observer);

  DALI_LOG_ERROR("--- Scene Graph Shader::RemoveConnectionObserver\n");
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
