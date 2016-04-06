#ifndef __DALI_INTERNAL_SCENE_GRAPH_SHADER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_SHADER_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/shader-effects/shader-effect.h>

#include <dali/internal/common/shader-data.h>

#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/type-abstraction-enums.h>

#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/effects/shader-declarations.h>

#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/scene-graph-connection-change-propagator.h>

#include <dali/internal/render/gl-resources/gl-resource-owner.h>
#include <dali/internal/render/gl-resources/texture-declarations.h>

#include <dali/internal/render/common/render-manager.h>


namespace Dali
{

namespace Internal
{

class Program;

namespace SceneGraph
{

class RenderQueue;
class UniformMeta;
class TextureCache;
class ConnectionObserver;
class SceneController;

/**
 * A base class for a collection of shader programs, to apply an effect to different geometry types.
 * This class is also the default shader so its easier to override default behaviour
 */
class Shader : public PropertyOwner, public UniformMap::Observer
{
public:

  /**
   * Constructor
   * @param hints Geometry hints
   */
  Shader( Dali::ShaderEffect::GeometryHints& hints );

  /**
   * Virtual destructor
   */
  virtual ~Shader();

  /**
   * Second stage initialization, called when added to the UpdateManager
   * @param renderQueue Used to queue messages from update to render thread.
   * @param textureCache Used to retrieve effect textures when rendering.
   */
  void Initialize( RenderQueue& renderQueue, TextureCache& textureCache );

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // The following methods are called during UpdateManager::Update()
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  /**
   * Query whether a shader geometry hint is set.
   * @pre The shader has been initialized.
   * @param[in] hint The geometry hint to check.
   * @return True if the given geometry hint is set.
   */
  bool GeometryHintEnabled( Dali::ShaderEffect::GeometryHints hint ) const
  {
    return mGeometryHints & hint;
  }

  /**
   * Retrieve the set of geometry hints.
   * @return The hints.
   */
  Dali::ShaderEffect::GeometryHints GetGeometryHints() const
  {
    return mGeometryHints;
  }

  /**
   * Set the geometry hints.
   * @param[in] hints The hints.
   */
  void SetGeometryHints( Dali::ShaderEffect::GeometryHints hints )
  {
    mGeometryHints = hints;
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
  // The following methods are called in Render thread
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

  /**
   * Get the program built for this shader
   * @return The program built from the shader sources.
   */
  Program* GetProgram();

public: // Implementation of ObjectOwnerContainer template methods

  /**
   * Connect the object to the scene graph
   *
   * @param[in] sceneController The scene controller - used for sending messages to render thread
   * @param[in] bufferIndex The current buffer index - used for sending messages to render thread
   */
  void ConnectToSceneGraph( SceneController& sceneController, BufferIndex bufferIndex );

  /**
   * Disconnect the object from the scene graph
   * @param[in] sceneController The scene controller - used for sending messages to render thread
   * @param[in] bufferIndex The current buffer index - used for sending messages to render thread
   */
  void DisconnectFromSceneGraph( SceneController& sceneController, BufferIndex bufferIndex );

public: // Implementation of ConnectionChangePropagator

  /**
   * @copydoc ConnectionChangePropagator::AddObserver
   */
  void AddConnectionObserver(ConnectionChangePropagator::Observer& observer);

  /**
   * @copydoc ConnectionChangePropagator::RemoveObserver
   */
  void RemoveConnectionObserver(ConnectionChangePropagator::Observer& observer);

public:

public: // UniformMap::Observer
  /**
   * @copydoc UniformMap::Observer::UniformMappingsChanged
   */
  virtual void UniformMappingsChanged( const UniformMap& mappings );

private: // Data

  Dali::ShaderEffect::GeometryHints mGeometryHints;    ///< shader geometry hints for building the geometry
  float                          mGridDensity;      ///< grid density

  Texture*                       mTexture;          ///< Raw Pointer to Texture
  Integration::ResourceId        mRenderTextureId;  ///< Copy of the texture ID for the render thread
  Integration::ResourceId        mUpdateTextureId;  ///< Copy of the texture ID for update thread

  Program*                       mProgram;

  ConnectionChangePropagator     mConnectionObservers;

  // These members are only safe to access during UpdateManager::Update()
  RenderQueue*                   mRenderQueue;                   ///< Used for queuing a message for the next Render

  // These members are only safe to access in render thread
  TextureCache*                  mTextureCache; // Used for retrieving textures in the render thread
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_SHADER_H__
