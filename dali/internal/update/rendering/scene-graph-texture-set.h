#ifndef DALI_INTERNAL_SCENE_GRAPH_TEXTURE_SET_H
#define DALI_INTERNAL_SCENE_GRAPH_TEXTURE_SET_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/rendering/texture-set.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/scene-graph-connection-change-propagator.h>
#include <dali/internal/update/common/uniform-map.h>
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

class TextureSet : public PropertyOwner, public UniformMap::Observer
{
public:

  /**
   * Construct a new TextureSet.
   */
  static TextureSet* New();

  /**
   * Destructor
   */
  virtual ~TextureSet();

  /**
   * Overriden delete operator
   * Deletes the texture set from its global memory pool
   */
  void operator delete( void* ptr );

  /**
   * Prepare the texture set, check texture loading status, opacity etc
   * @param resourceManager for checking texture details and loading status
   */
  void Prepare( const ResourceManager& resourceManager );

  /**
   * Set the resource id for the texture at position "index"
   * @param[in] index The index of the texture
   * @param[in] imageId the resource id of the imag
   */
  void SetImage( size_t index,  ResourceId imageId );

  /**
   * Set the sampler to be used by the texture at position "index"
   * @param[in] index The index of the texture
   * @param[in] sampler The sampler to be used by the texture
   */
  void SetSampler( size_t index, Render::Sampler* sampler );

  /**
   * Return whether any texture in the texture set has an alpha channel
   * @return true if at least one texture in the texture set has an alpha channel, false otherwise
   */
  bool HasAlpha() const;

  /**
   * Get the resource status
   * Note, we need two values as it's possible that some resource failed to load
   * in which case resourcesReady is false (the texture set is not good to be used for rendering)
   * but finishedResourceAcquisition if true as there is no more loading going on
   * @param[out] resourcesReady if the texture set is ready to be used for rendering
   * @param[out] finishedResourceAcquisition if
   */
  void GetResourcesStatus( bool& resourcesReady, bool& finishedResourceAcquisition );


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
   * Get the ResourceId of a texture in the TextureSet
   * @param[in] index The index of the texture in the textures array
   * @return the ResourceId
   */
  ResourceId GetTextureId( size_t index )
  {
    return mTextureId[index];
  }

  /**
   * Get the sampler of a texture in the TextureSet
   * @param[in] index The index of the texture in the textures array
   * @return the sampler used by the texture
   */
  Render::Sampler* GetTextureSampler( size_t index )
  {
    return mSamplers[index];
  }

  /**
   * Get the number of textures in the texture set
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

private:

  /**
   * Protected constructor; See also TextureSet::New()
   */
  TextureSet();

private: // Data

  Vector< Render::Sampler* >      mSamplers; // Not owned
  Vector< ResourceId >            mTextureId;
  ConnectionChangePropagator      mConnectionObservers;
  bool                            mResourcesReady; ///< if the textures are ready to be used for rendering
  bool                            mFinishedResourceAcquisition; ///< if resource loading is completed
  bool                            mChanged; ///< if the texture set has changed since the last frame
  bool                            mHasAlpha; ///< if any of the textures has an alpha channel
};

inline void SetImageMessage( EventThreadServices& eventThreadServices, const TextureSet& textureSet, size_t index, ResourceId resourceId )
{
  typedef MessageValue2< TextureSet, size_t, ResourceId > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &textureSet, &TextureSet::SetImage, index, resourceId );
}

inline void SetSamplerMessage( EventThreadServices& eventThreadServices, const TextureSet& textureSet, size_t index, Render::Sampler* sampler )
{
  typedef MessageValue2< TextureSet, size_t, Render::Sampler* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &textureSet, &TextureSet::SetSampler, index, sampler );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif //  DALI_INTERNAL_SCENE_GRAPH_TEXTURE_SET_H
