#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDERER_ATTACHMENT_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDERER_ATTACHMENT_H

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

#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/double-buffered.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/scene-graph-connection-change-propagator.h>
#include <dali/internal/update/controllers/render-message-dispatcher.h>
#include <dali/internal/update/controllers/scene-controller.h>
#include <dali/internal/update/node-attachments/scene-graph-renderable-attachment.h>
#include <dali/internal/render/data-providers/render-data-provider.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
class Material;
class Geometry;
class NewRenderer;
struct RendererWithSortAttributes;

/**
 * The renderer attachment is the SceneGraph equivalent of Dali::Renderer. It is used to create an instance of a geometry and material for rendering, and is attached to an actor.
 *
 * It observes it's children (Material and Geometry) for connection change and for uniform map change, and observer's it's actor parent for uniform map change - this allows it to re-generate the uniform maps used by its RenderThread equivalent class.
 *
 * Lifetime and ownership
 * It is created when a Dali::Renderer is created, and sent to UpdateManager. At this point
 * Initialize is called on the object, but ownership is NOT taken by UpdateManager.

 * When a Dali::Renderer is added to an actor, then this object becomes the node attachment
 * for that actor, and the node takes ownership. It will create the Render::NewRenderer object
 * in the Update thread on reciept of the connection message.

 * When it's not attached to an actor, it is still possible to send messages to this
 * object, to, e.g., set the material, or a property.
 *
 * @todo MESH_REWORK On merge with RenderableAttachment, change owner of all attachments to UpdateManager.
 */
class RendererAttachment : public RenderableAttachment,
                           public PropertyOwner,
                           public UniformMapDataProvider,
                           public UniformMap::Observer,
                           public ConnectionChangePropagator::Observer
{
public:
  /**
   * Create a new renderer attachment.
   * @return The newly allocated RendererAttachment
   */
  static RendererAttachment* New();

  /**
   * Constructor
   */
  RendererAttachment();

  /**
   * Destructor
   */
  virtual ~RendererAttachment();

  /**
   * @copydoc RenderableAttachment::Initialize2().
   */
  virtual void Initialize2( BufferIndex updateBufferIndex );

  /**
   * @copydoc RenderableAttachment::OnDestroy2().
   */
  virtual void OnDestroy2();

  /**
   * @copydoc NodeAttachment::ConnectedToSceneGraph()
   */
  virtual void ConnectedToSceneGraph();

  /**
   * @copydoc NodeAttachment::DisconnectedFromSceneGraph()
   */
  virtual void DisconnectedFromSceneGraph();

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
  Material& GetMaterial();

  /**
   * Set the geometry for the renderer
   * @param[in] bufferIndex The current frame's buffer index
   * @param[in] geometry The geometry this renderer will use
   */
  void SetGeometry( BufferIndex bufferIndex, Geometry* geometry);

  /**
   * Get the geometry of this renderer
   * @return the geometry this renderer uses
   */
  Geometry& GetGeometry();

  /**
   * Set the depth index
   * @param[in] bufferIndex The buffer index
   * @param[in] depthIndex the new depth index to use
   */
  void SetDepthIndex( BufferIndex bufferIndex, int depthIndex );

protected: // From NodeAttachment
  /**
   * @copydoc NodeAttachment::ResetToBaseValues
   */
  virtual void ResetToBaseValues( BufferIndex updateBufferIndex );

  /**
   * @param[in] bufferIndex The buffer index
   */
  virtual int GetDepthIndex( BufferIndex bufferIndex )
  {
    return mDepthIndex[bufferIndex];
  }

  /**
   * Write the attachment's sort attributes to the passed in reference.
   * @param[in] bufferIndex The buffer index
   * @param[out] sortAttributes
   */
  virtual void SetSortAttributes( BufferIndex bufferIndex, RendererWithSortAttributes& sortAttributes );

protected: // From RenderableAttachment
  /**
   * @copydoc RenderableAttachment::GetRenderer().
   */
  virtual Renderer& GetRenderer();

  /**
   * @copydoc RenderableAttachment::GetRenderer().
   */
  virtual const Renderer& GetRenderer() const;

  /**
   * @copydoc RenderableAttachment::DoPrepareResources()
   */
  virtual bool DoPrepareResources( BufferIndex updateBufferIndex,
                                   ResourceManager& resourceManager );

  /**
   * @copydoc RenderableAttachment::DoPrepareRender()
   */
  virtual void DoPrepareRender( BufferIndex updateBufferIndex );

  /**
   * @copydoc RenderableAttachment::IsFullyOpaque()
   */
  virtual bool IsFullyOpaque( BufferIndex updateBufferIndex );

  /**
   * @copydoc RenderableAttachment::SizeChanged()
   */
  virtual void SizeChanged( BufferIndex updateBufferIndex );

protected: // From ConnectionObserver
  /**
   * @copydoc ConnectionObservers::Observer::ConnectionsChanged
   */
  virtual void ConnectionsChanged(PropertyOwner& object);

  /**
   * @copydoc ConnectionObservers::Observer::ConnectedUniformMapChanged
   */
  virtual void ConnectedUniformMapChanged();

protected: // From UniformMap::Observer
  /**
   * @copydoc UniformMap::Observer::UniformMappingsChanged
   */
  virtual void UniformMappingsChanged( const UniformMap& mappings );

protected: // From UniformMapDataProvider
  /**
   * @copydoc UniformMapDataProvider::GetUniformMapChanged
   */
  virtual bool GetUniformMapChanged( BufferIndex bufferIndex ) const;

  /**
   * @copydoc UniformMapDataProvider::GetUniformMap
   */
  virtual const CollectedUniformMap& GetUniformMap( BufferIndex bufferIndex ) const;

private:
  /**
   * Add any new mappings from map into the current map.
   * This doesn't override any existing mappings.
   * @param[in] localMap The map of collected uniforms.
   * @param[in] map The map to import into the local map
   */
  void AddMappings( CollectedUniformMap& localMap, const UniformMap& map );

  /**
   * Create a new render data provider
   * @return the new (initialized) data provider
   */
  RenderDataProvider* NewRenderDataProvider();

private:
  NewRenderer* mRenderer; ///< Raw pointer to the new renderer (that's owned by RenderManager)

  Material* mMaterial; ///< The material this renderer uses. (Not owned)
  Geometry* mGeometry; ///< The geometry this renderer uses. (Not owned)

  CollectedUniformMap mCollectedUniformMap[2];
  int mRegenerateUniformMap;  ///< 2 if the map should be regenerated, 1 if it should be copied.
  bool mUniformMapChanged[2]; ///< Records if the uniform map has been altered this frame
  bool mResendDataProviders;  ///< True if the data providers should be resent to the renderer

public: // Properties

  AnimatableProperty<int> mDepthIndex; ///< Used only in PrepareRenderInstructions
};

// Messages for RendererAttachment

inline void SetMaterialMessage( EventThreadServices& eventThreadServices, const RendererAttachment& attachment, const Material& material )
{
  typedef MessageDoubleBuffered1< RendererAttachment, Material* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &RendererAttachment::SetMaterial, const_cast<Material*>(&material) );
}

inline void SetGeometryMessage( EventThreadServices& eventThreadServices, const RendererAttachment& attachment, const Geometry& geometry )
{
  typedef MessageDoubleBuffered1< RendererAttachment, Geometry* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &RendererAttachment::SetGeometry, const_cast<Geometry*>(&geometry) );
}

inline void SetDepthIndexMessage( EventThreadServices& eventThreadServices, const RendererAttachment& attachment, int depthIndex )
{
  typedef MessageDoubleBuffered1< RendererAttachment, int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &attachment, &RendererAttachment::SetDepthIndex, depthIndex );
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali


#endif //  DALI_INTERNAL_SCENE_GRAPH_RENDERER_ATTACHMENT_H
