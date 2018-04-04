#ifndef __DALI_INTERNAL_SCENE_GRAPH_SHADER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_SHADER_H__

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

// INTERNAL INCLUDES
#include <dali/internal/common/shader-data.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/scene-graph-connection-change-propagator.h>
#include <dali/graphics-api/graphics-api-accessor.h>
#include <dali/graphics-api/graphics-api-shader.h>

namespace Dali
{
namespace Internal
{

class Program;
class ProgramCache;

namespace SceneGraph
{

class ConnectionObserver;
class SceneController;
/**
 * A holder class for Program; also enables sharing of uniform properties
 */
class Shader : public PropertyOwner, public UniformMap::Observer
{
public:

  /**
   * Constructor
   * @param hints Shader hints
   */
  Shader( Dali::Shader::Hint::Value& hints );

  /**
   * Virtual destructor
   */
  virtual ~Shader();

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // The following methods are called during Update
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * Query whether a shader hint is set.
   * @pre The shader has been initialized.
   * @param[in] hint The hint to check.
   * @return True if the given hint is set.
   */
  bool HintEnabled( Dali::Shader::Hint::Value hint ) const
  {
    return mHints & hint;
  }

  /**
   * @return True if the fragment shader outputs only 1.0 on the alpha channel
   *
   * @note Shaders that can output any value on the alpha channel
   * including 1.0 should return false for this.
   */
  bool IsOutputOpaque();

  /**
   * @return True if the fragment shader can output any value but 1.0 on the alpha channel
   *
   * @note Shaders that can output any value on the alpha channel
   * including 1.0 should return false for this
   */
  bool IsOutputTransparent();

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyOwner::ResetDefaultProperties
   */
  virtual void ResetDefaultProperties( BufferIndex updateBufferIndex )
  {
    // no default properties
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // The following methods are called during Render
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * @brief Set the program for this shader.
   * @param[in] shaderData        The program's vertex/fragment source and optionally precompiled shader binary.
   * @param[in] programCache      Owner of the Programs.
   * @param[in] modifiesGeometry  True if the vertex shader changes the positions of vertexes such that
   * they might exceed the bounding box of vertexes passing through the default transformation.
   */
  void SetProgram( Internal::ShaderDataPtr shaderData,
                   ProgramCache* programCache,
                   bool modifiesGeometry );

  void SetGfxObject( const Graphics::API::Accessor<Graphics::API::Shader>& shader );

  const Graphics::API::Accessor<Graphics::API::Shader>& GetGfxObject() const;

  /**
   * Get the program built for this shader
   * @return The program built from the shader sources.
   */
  Program* GetProgram();

public: // Implementation of ConnectionChangePropagator

  /**
   * @copydoc ConnectionChangePropagator::AddObserver
   */
  void AddConnectionObserver(ConnectionChangePropagator::Observer& observer);

  /**
   * @copydoc ConnectionChangePropagator::RemoveObserver
   */
  void RemoveConnectionObserver(ConnectionChangePropagator::Observer& observer);

public: // UniformMap::Observer
  /**
   * @copydoc UniformMap::Observer::UniformMappingsChanged
   */
  virtual void UniformMappingsChanged( const UniformMap& mappings );

private: // Data

  Dali::Shader::Hint::Value     mHints;

  Program*                       mProgram;

  ConnectionChangePropagator     mConnectionObservers;

  Graphics::API::Accessor<Graphics::API::Shader> mGfxShader;
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_SHADER_H__
