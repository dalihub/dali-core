/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/common/render-debug.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/gl-resources/texture.h>
#include <dali/internal/render/gl-resources/texture-cache.h>
#include <dali/internal/render/gl-resources/texture-units.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/render/shaders/uniform-meta.h>
#include <dali/internal/common/image-sampler.h>

// See render-debug.h
#ifdef DALI_PRINT_RENDER_INFO

#include <sstream>
#define DALI_DEBUG_OSTREAM(streamName) std::stringstream streamName;

#define DALI_PRINT_UNIFORM(streamName,bufferIndex,name,value) \
  { \
    streamName << " " << name << ": " << value; \
  }

#define DALI_PRINT_CUSTOM_UNIFORM(streamName,bufferIndex,name,property) \
  { \
    streamName << " " << name << ": "; \
    property.DebugPrint( streamName, bufferIndex ); \
  }

#define DALI_PRINT_SHADER_UNIFORMS(streamName) \
  { \
    std::string debugString( streamName.str() ); \
    DALI_LOG_RENDER_INFO( "           %s\n", debugString.c_str() ); \
  }

#else // DALI_PRINT_RENDER_INFO

#define DALI_DEBUG_OSTREAM(streamName)
#define DALI_PRINT_UNIFORM(streamName,bufferIndex,name,value)
#define DALI_PRINT_CUSTOM_UNIFORM(streamName,bufferIndex,name,property)
#define DALI_PRINT_SHADER_UNIFORMS(streamName)

#endif // DALI_PRINT_RENDER_INFO

namespace Dali
{

namespace Internal
{

template <> struct ParameterType< Dali::ShaderEffect::GeometryHints> : public BasicType< Dali::ShaderEffect::GeometryHints > {};
template <> struct ParameterType< Dali::ShaderEffect::UniformCoordinateType > : public BasicType< Dali::ShaderEffect::UniformCoordinateType > {};

namespace SceneGraph
{

Shader::Shader( Dali::ShaderEffect::GeometryHints& hints )
: mGeometryHints( hints ),
  mGridDensity( Dali::ShaderEffect::DEFAULT_GRID_DENSITY ),
  mTexture( NULL ),
  mRenderTextureId( 0 ),
  mUpdateTextureId( 0 ),
  mProgram( NULL ),
  mConnectionObservers(),
  mRenderQueue( NULL ),
  mTextureCache( NULL )
{
  AddUniformMapObserver( *this );
}

Shader::~Shader()
{
  mConnectionObservers.Destroy( *this );
}

void Shader::Initialize( RenderQueue& renderQueue, TextureCache& textureCache )
{
  mRenderQueue = &renderQueue;
  mTextureCache = &textureCache;
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

void Shader::ConnectToSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
}

void Shader::DisconnectFromSceneGraph( SceneController& sceneController, BufferIndex bufferIndex )
{
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
