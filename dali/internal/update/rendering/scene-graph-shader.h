#ifndef __DALI_INTERNAL_SCENE_GRAPH_SHADER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_SHADER_H__

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

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-api-controller.h>
#include <dali/graphics-api/graphics-api-accessor.h>
#include <dali/graphics-api/graphics-api-shader.h>

#include <dali/integration-api/graphics/graphics.h>

#include <dali/internal/common/message.h>
#include <dali/internal/common/shader-data.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/scene-graph-connection-change-propagator.h>
#include <dali/internal/update/rendering/shader-cache.h>

namespace Dali
{
namespace Internal
{

namespace SceneGraph
{

class ConnectionObserver;
class SceneController;

/**
 * Owner of Graphics Shader; also enables sharing of uniform properties.
 * Owned by UpdateManager.
 */
class Shader : public PropertyOwner, public UniformMap::Observer
{
public:

  /**
   * Devel constructor
   * @param hints
   */
  Shader( Dali::Shader::Hint::Value& hints );

  /**
   * Virtual destructor
   */
  virtual ~Shader();

  /**
   * Initialize the shader object with the Graphics API when added to UpdateManager
   *
   * @param[in] graphics The Graphics API
   * @param[in] shaderCache A cache
   */
  void Initialize( Integration::Graphics::Graphics& graphics, ShaderCache& shaderCache );

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
   * @copydoc Dali::Internal::SceneGraph::PropertyOwner::ResetDefaultProperties
   */
  virtual void ResetDefaultProperties( BufferIndex updateBufferIndex )
  {
    // no default properties
  }

  /**
   * Get the graphics shader object
   * @return the graphics shader object
   */
  Graphics::API::Accessor<Graphics::API::Shader>& GetGfxObject();

public: // Messages
  /**
   * Set the shader data into the graphics API.
   *
   * @param[in] shaderData The shader source or binary.
   * @param[in] modifiesGeometry Hint to say if the shader modifies geometry. (useful for culling)
   */
  void SetShaderProgram( Internal::ShaderDataPtr shaderData, bool modifiesGeometry );

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
  Integration::Graphics::Graphics*               mGraphics; ///< Graphics interface object
  Graphics::API::Accessor<Graphics::API::Shader> mGraphicsShader; ///< The graphics object
  ShaderCache*                                   mShaderCache;
  Dali::Shader::Hint::Value                      mHints; ///< Hints for the shader
  ConnectionChangePropagator                     mConnectionObservers; ///< Watch for connection changes
};


inline void SetShaderProgramMessage( EventThreadServices& eventThreadServices,
                                     Shader& shader,
                                     Internal::ShaderDataPtr shaderData,
                                     bool modifiesGeometry )
{
  typedef MessageValue2< Shader, Internal::ShaderDataPtr, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &shader, &Shader::SetShaderProgram, shaderData, modifiesGeometry );
}



} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_SHADER_H__
