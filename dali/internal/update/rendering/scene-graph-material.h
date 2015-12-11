#ifndef DALI_INTERNAL_SCENE_GRAPH_MATERIAL_H
#define DALI_INTERNAL_SCENE_GRAPH_MATERIAL_H

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
 */

// INTERNAL INCLUDES
#include <dali/devel-api/rendering/material.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/blending-options.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/scene-graph-connection-change-propagator.h>
#include <dali/internal/update/common/uniform-map.h>
#include <dali/internal/render/data-providers/material-data-provider.h>
#include <dali/internal/update/resources/resource-manager-declarations.h>

namespace Dali
{
namespace Internal
{
class ResourceManager;

namespace Render
{
class Sampler;
}
namespace SceneGraph
{
class Sampler;
class Shader;
class ConnectionObserver;
class SceneController;

class Material : public PropertyOwner, public MaterialDataProvider, public UniformMap::Observer, public ConnectionChangePropagator::Observer
{
public:
  /**
   * This enum defines the outputs of the PrepareRender step, and is used
   * by the Renderer to determine final opacity.
   */
  enum BlendPolicy
  {
    OPAQUE,          ///< If the renderer should never use blending
    TRANSLUCENT,     ///< If the renderer should always be use blending
    USE_ACTOR_COLOR  ///< If the renderer should determine opacity using the actor color
  };

  /**
   * Constructor
   */
  Material();

  /**
   * Destructor
   */
  virtual ~Material();

  /**
   * Prepare material, check texture loading status, opacity etc
   * @param resourceManager for checking texture details and loading status
   */
  void Prepare( const ResourceManager& resourceManager );

  /**
   * Set the shader effect for this material
   * @param[in] shader The shader effect to use
   */
  void SetShader( Shader* shader );

  /**
   * Get the shader effect of this material
   * @return the shader effect;
   */
  Shader* GetShader() const;

  /**
   * Set the face culling mode
   * @param[in] faceCullingMode to use
   */
  void SetFaceCullingMode( unsigned int faceCullingMode );

  /**
   * Set the blending mode
   * @param[in] blendingMode to use
   */
  void SetBlendingMode( unsigned int blendingMode );

  /**
   * Return the blend policy ( a combination of all the different shader hints, color, samper and image properties ).
   * This should only be called from the update thread
   * @return The material's blend policy
   */
  BlendPolicy GetBlendPolicy() const;

  /**
   * Set the blending options. This should only be called from the update thread.
   * @param[in] options A bitmask of blending options.
   */
  void SetBlendingOptions( unsigned int options );

  /**
   * Set the blend color for blending operation
   * @param blendColor to pass to GL
   */
  void SetBlendColor( const Vector4& blendColor );

  /**
   * Adds a new texture to be used by the material
   * @param[in] image The image used by the texture
   * @param[in] uniformName The uniform name of the texture
   * @param[in] sampler Sampling parameters. If not provided the default sampling parameters will be used
   */
  void AddTexture( const std::string& name, ResourceId id, Render::Sampler* sampler );

  /**
   * Removes a texture from the material
   * @param[in] index The index of the texture in the array of textures
   */
  void RemoveTexture( size_t index );

  /**
   * Sets the image to be used by a given texture
   * @param[in] index The index of the texture in the array of textures
   * @param[in] image The new image
   */
  void SetTextureImage( size_t index, ResourceId id );

  /**
   * Set the sampler used by a given texture
   * @param[in] index The index of the texture in the array of textures
   * @param[in] sampler The new sampler
   */
  void SetTextureSampler( size_t index, Render::Sampler* sampler);

  /**
   * Set the uniform name of a given texture
   * @param[in] index The index of the texture in the array of textures
   * @param[in] uniformName The new uniform name
   */
  void SetTextureUniformName( size_t index, const std::string& uniformName );

  /**
   * Get the material resource status
   * Note, we need two values as it's possible that some resource failed to load
   * in which case resourcesReady is false (the material is not good to be rendered)
   * but finishedResourceAcquisition if true as there is no more loading going on
   * @param[out] resourcesReady if the material is ready to be rendered
   * @param[out] finishedResourceAcquisition if
   */
  void GetResourcesStatus( bool& resourcesReady, bool& finishedResourceAcquisition );

public: // Implementation of MaterialDataProvider

  /**
   * @copydoc MaterialDataProvider::GetBlendColor
   */
  virtual Vector4* GetBlendColor() const;

  /**
   * @copydoc MaterialDataProvider::GetBlendingOptions
   */
  virtual const BlendingOptions& GetBlendingOptions() const;

  /**
   * @copydoc MaterialDataProvider::GetFaceCullingMode
   */
  virtual Dali::Material::FaceCullingMode GetFaceCullingMode() const;


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

  /**
   * Get the ResourceId of a texture used by the material
   * @param[in] index The index of the texture in the textures array
   * @return the ResourceId
   */
  ResourceId GetTextureId( size_t index )
  {
    return mTextureId[index];
  }

  /**
   * Get the uniform name of a texture used by the material
   * @param[in] index The index of the texture in the textures array
   * @return the uniform name
   */
  const std::string& GetTextureUniformName( size_t index )
  {
    return mUniformName[index];
  }

  /**
   * Get the sampler of a texture used by the material
   * @param[in] index The index of the texture in the textures array
   * @return the sampler used by the texture
   */
  Render::Sampler* GetTextureSampler( size_t index )
  {
    return mSamplers[index];
  }

  /**
   * Get the number of textures used by the material
   * @return The number of textures
   */
  size_t GetTextureCount()
  {
    return mTextureId.Size();
  }

public: // UniformMap::Observer
  /**
   * @copydoc UniformMap::Observer::UniformMappingsChanged
   */
  virtual void UniformMappingsChanged( const UniformMap& mappings );

public: // ConnectionChangePropagator::Observer

  /**
   * @copydoc ConnectionChangePropagator::ConnectionsChanged
   */
  virtual void ConnectionsChanged( PropertyOwner& owner );

  /**
   * @copydoc ConnectionChangePropagator::ConnectedUniformMapChanged
   */
  virtual void ConnectedUniformMapChanged();

private: // Data

  Shader*                         mShader;
  Vector4*                        mBlendColor; // not double buffered as its not animateable and not frequently changed
  Vector< Render::Sampler* >      mSamplers; // Not owned
  Vector< ResourceId >            mTextureId;
  std::vector< std::string >      mUniformName;
  ConnectionChangePropagator      mConnectionObservers;
  Dali::Material::FaceCullingMode mFaceCullingMode; // not double buffered as its not animateable and not frequently changed
  BlendingMode::Type              mBlendingMode; // not double buffered as its not animateable and not frequently changed
  BlendingOptions                 mBlendingOptions; // not double buffered as its not animateable and not frequently changed
  BlendPolicy                     mBlendPolicy; ///< The blend policy as determined by PrepareRender
  bool                            mResourcesReady; ///< if the material is ready to be rendered
  bool                            mFinishedResourceAcquisition; ///< if resource loading is completed
  bool                            mMaterialChanged; ///< if the material has changed since the last frame
};

inline void SetShaderMessage( EventThreadServices& eventThreadServices, const Material& material, Shader& shader )
{
  typedef MessageValue1< Material, Shader* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &material, &Material::SetShader, &shader );
}

inline void SetFaceCullingModeMessage( EventThreadServices& eventThreadServices, const Material& material, Dali::Material::FaceCullingMode faceCullingMode )
{
  typedef MessageValue1< Material, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &material, &Material::SetFaceCullingMode, faceCullingMode );
}

inline void SetBlendingModeMessage( EventThreadServices& eventThreadServices, const Material& material, BlendingMode::Type blendingMode )
{
  typedef MessageValue1< Material, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &material, &Material::SetBlendingMode, blendingMode );
}

inline void SetBlendingOptionsMessage( EventThreadServices& eventThreadServices, const Material& material, unsigned int options )
{
  typedef MessageValue1< Material, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &material, &Material::SetBlendingOptions, options );
}

inline void SetBlendColorMessage( EventThreadServices& eventThreadServices, const Material& material, const Vector4& blendColor )
{
  typedef MessageValue1< Material, Vector4 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  new (slot) LocalType( &material, &Material::SetBlendColor, blendColor );
}

inline void AddTextureMessage( EventThreadServices& eventThreadServices, const Material& material, const std::string& uniformName, ResourceId id, Render::Sampler* sampler )
{
  typedef MessageValue3< Material, std::string, ResourceId, Render::Sampler* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &material, &Material::AddTexture, uniformName, id, sampler );
}

inline void RemoveTextureMessage( EventThreadServices& eventThreadServices, const Material& material, size_t index )
{
  typedef MessageValue1< Material, size_t > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &material, &Material::RemoveTexture, index );
}

inline void SetTextureImageMessage( EventThreadServices& eventThreadServices, const Material& material, size_t index, ResourceId id )
{
  typedef MessageValue2< Material, size_t, ResourceId > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &material, &Material::SetTextureImage, index, id );
}

inline void SetTextureSamplerMessage( EventThreadServices& eventThreadServices, const Material& material, size_t index, Render::Sampler* sampler )
{
  typedef MessageValue2< Material, size_t, Render::Sampler* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &material, &Material::SetTextureSampler, index, sampler );
}

inline void SetTextureUniformNameMessage( EventThreadServices& eventThreadServices, const Material& material, size_t index, const std::string& uniformName )
{
  typedef MessageValue2< Material, size_t, std::string > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &material, &Material::SetTextureUniformName, index, uniformName );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif //  DALI_INTERNAL_SCENE_GRAPH_MATERIAL_H
