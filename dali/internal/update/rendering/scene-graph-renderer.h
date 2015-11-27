#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDERER2_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDERER2_H

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


#include <dali/devel-api/rendering/geometry.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/double-buffered.h>
#include <dali/internal/update/common/double-buffered-property.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/property-boolean.h>
#include <dali/internal/update/common/uniform-map.h>
#include <dali/internal/update/common/scene-graph-connection-change-propagator.h>
#include <dali/internal/render/data-providers/render-data-provider.h>
#include <dali/internal/update/resources/resource-manager.h>

namespace Dali
{
namespace Internal
{

namespace Render
{
class Renderer;
}

namespace SceneGraph
{
class SceneController;

class Renderer;
typedef Dali::Vector< Renderer* > RendererContainer;
typedef RendererContainer::Iterator RendererIter;
typedef RendererContainer::ConstIterator RendererConstIter;

class Material;
class Geometry;

class Renderer :  public PropertyOwner,
                  public UniformMapDataProvider,
                  public UniformMap::Observer,
                  public ConnectionChangePropagator::Observer
{
public:

  /**
   * Default constructor
   */
  Renderer();

  /**
   * Destructor
   */
  virtual ~Renderer();

  /**
   * Set the material for the renderer
   * @param[in] bufferIndex The current frame's buffer index
   * @param[in] material The material this renderer will use
   */
  void SetMaterial( BufferIndex bufferIndex, Material* material);

  /**
   * Get the material of this renderer
   * @return the material this renderer uses
   */
  Material& GetMaterial()
  {
    return *mMaterial;
  }

  /**
   * Set the geometry for the renderer
   * @param[in] bufferIndex The current frame's buffer index
   * @param[in] geometry The geometry this renderer will use
   */
  void SetGeometry( BufferIndex bufferIndex, Geometry* material);

  /**
   * Get the geometry of this renderer
   * @return the geometry this renderer uses
   */
  Geometry& GetGeometry()
  {
    return *mGeometry;
  }

  /**
   * Set the depth index
   * @param[in] depthIndex the new depth index to use
   */
  void SetDepthIndex( int depthIndex );

  /**
   * @brief Get the depth index
   * @return The depth index
   */
  int GetDepthIndex() const
  {
    return mDepthIndex;
  }

  /**
   * Called when an actor with this renderer is added to the stage
   */
  void OnStageConnect();

  /*
   * Called when an actor with this renderer is removed from the stage
   */
  void OnStageDisconnect();

  /**
   * Prepare the object for rendering.
   * This is called by the UpdateManager when an object is due to be rendered in the current frame.
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void PrepareRender( BufferIndex updateBufferIndex );

  /*
   * Retrieve the Render thread renderer
   * @return The associated render thread renderer
   */
  Render::Renderer& GetRenderer();

  /**
     * Prepare the object resources.
     * This must be called by the UpdateManager before calling PrepareRender, for each frame.
     * @param[in] updateBufferIndex The current update buffer index.
     * @param[in] resourceManager The resource manager.
     */
  void PrepareResources( BufferIndex updateBufferIndex, ResourceManager& resourceManager );

  /**
   * Check whether the renderer has been marked as ready to render
   * @param[out] ready TRUE if the renderer has resources to render
   * @param[out] complete TRUE if the renderer resources are complete
   * (e.g. image has finished loading, framebuffer is ready to render, native image
   * framebuffer has been rendered)
   */
  void GetReadyAndComplete(bool& ready, bool& complete) const;

  /**
   * Query whether the renderer is fully opaque.
   * @param[in] updateBufferIndex The current update buffer index.
   * @return True if fully opaque.
   */
  bool IsFullyOpaque( BufferIndex updateBufferIndex, const Node& node ) const;

  /**
   * Query whether the renderer is currently in use by an actor on the stage
   */
  bool IsReferenced() const
  {
    return mReferenceCount > 0;
  }


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
  void AddConnectionObserver(ConnectionChangePropagator::Observer& observer){};

  /**
   * @copydoc ConnectionChangePropagator::RemoveObserver
   */
  void RemoveConnectionObserver(ConnectionChangePropagator::Observer& observer){};

public:


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
  virtual void ConnectedUniformMapChanged( );

  /**
   * @copydoc ConnectionChangePropagator::ConnectedUniformMapChanged
   */
  virtual void ObservedObjectDestroyed(PropertyOwner& owner);

public: // PropertyOwner implementation
  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyOwner::ResetDefaultProperties()
   */
  virtual void ResetDefaultProperties( BufferIndex updateBufferIndex ){};

public: // From UniformMapDataProvider

  /**
   * @copydoc UniformMapDataProvider::GetUniformMapChanged
   */
  virtual bool GetUniformMapChanged( BufferIndex bufferIndex ) const{ return mUniformMapChanged[bufferIndex];}

  /**
   * @copydoc UniformMapDataProvider::GetUniformMap
   */
  virtual const CollectedUniformMap& GetUniformMap( BufferIndex bufferIndex ) const;

private:

  /**
   * Helper function to create a new render data provider
   * @return the new (initialized) data provider
   */
  RenderDataProvider* NewRenderDataProvider();

  SceneController* mSceneController;  ///< Used for initializing renderers whilst attached
  Render::Renderer*  mRenderer;    ///< Raw pointer to the new renderer (that's owned by RenderManager)
  Material*             mMaterial;    ///< The material this renderer uses. (Not owned)
  Geometry*             mGeometry;    ///< The geometry this renderer uses. (Not owned)

  Dali::Vector< Integration::ResourceId > mTrackedResources; ///< Filled during PrepareResources if there are uncomplete, tracked resources.

  CollectedUniformMap mCollectedUniformMap[2];    ///< Uniform maps collected by the renderer
  unsigned int mReferenceCount;                   ///< Number of nodes currently using this renderer
  unsigned int mRegenerateUniformMap;             ///< 2 if the map should be regenerated, 1 if it should be copied.
  bool         mUniformMapChanged[2];             ///< Records if the uniform map has been altered this frame
  bool         mResendDataProviders         : 1;  ///< True if the data providers should be resent to the renderer
  bool         mResendGeometry              : 1;  ///< True if geometry should be resent to the renderer
  bool         mHasUntrackedResources       : 1;  ///< Set during PrepareResources, true if have tried to follow untracked resources
  bool         mFinishedResourceAcquisition : 1;  ///< Set during DoPrepareResources; true if ready & all resource acquisition has finished (successfully or otherwise)
  bool         mResourcesReady              : 1;  ///< Set during the Update algorithm; true if the attachment has resources ready for the current frame.

public:
  int mDepthIndex; ///< Used only in PrepareRenderInstructions
};


/// Messages
inline void SetMaterialMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, const Material& material )
{
  typedef MessageDoubleBuffered1< Renderer, Material* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &renderer, &Renderer::SetMaterial, const_cast<Material*>(&material) );
}

inline void SetGeometryMessage( EventThreadServices& eventThreadServices, const Renderer& renderer, const Geometry& geometry )
{
  typedef MessageDoubleBuffered1< Renderer, Geometry* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &renderer, &Renderer::SetGeometry, const_cast<Geometry*>(&geometry) );
}

inline void SetDepthIndexMessage( EventThreadServices& eventThreadServices, const Renderer& attachment, int depthIndex )
{
  typedef MessageValue1< Renderer, int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &Renderer::SetDepthIndex, depthIndex );
}

inline void OnStageConnectMessage( EventThreadServices& eventThreadServices, const Renderer& renderer )
{
  typedef Message< Renderer > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &renderer, &Renderer::OnStageConnect );
}

inline void OnStageDisconnectMessage( EventThreadServices& eventThreadServices, const Renderer& renderer )
{
  typedef Message< Renderer > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &renderer, &Renderer::OnStageDisconnect );
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali

#endif //  DALI_INTERNAL_SCENE_GRAPH_RENDERER_H
