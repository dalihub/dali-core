#ifndef DALI_INTERNAL_SCENE_GRAPH_UPDATE_MANAGER_H
#define DALI_INTERNAL_SCENE_GRAPH_UPDATE_MANAGER_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

//EXTERNAL INCLUDES
#include <iostream>
#include <memory>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/vector-wrapper.h>

#include <dali/devel-api/common/stage-devel.h>
#include <dali/internal/common/message.h>
#include <dali/internal/common/type-abstraction-enums.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/rendering/texture-impl.h>
#include <dali/internal/update/animation/scene-graph-animation.h>
#include <dali/internal/update/common/property-resetter.h>
#include <dali/internal/update/common/scene-graph-buffers.h>
#include <dali/internal/update/common/scene-graph-property-notification.h>
#include <dali/internal/update/gestures/scene-graph-pan-gesture.h>
#include <dali/internal/update/manager/scene-graph-frame-callback.h> // for OwnerPointer< FrameCallback >
#include <dali/internal/update/manager/node-depths.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/update/rendering/scene-graph-geometry.h>
#include <dali/internal/update/rendering/scene-graph-property-buffer.h>
#include <dali/internal/update/rendering/scene-graph-frame-buffer.h>
#include <dali/internal/update/rendering/scene-graph-texture.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>    // for OwnerPointer< Renderer >
#include <dali/internal/update/rendering/scene-graph-shader.h> // for OwnerPointer< Shader >
#include <dali/internal/update/rendering/scene-graph-texture-set.h> // for OwnerPointer< TextureSet >
#include <dali/internal/update/render-tasks/scene-graph-render-task-list.h>

#include <dali/graphics-api/graphics-controller.h>

namespace Dali
{

class FrameCallbackInterface;

namespace Integration
{
class RenderController;
} // namespace Integration

namespace Internal
{
namespace SceneGraph
{
class FrameBuffer;
}

class PropertyNotifier;
class NotificationManager;
class CompleteNotificationInterface;
class TouchResampler;

// value types used by messages
template <> struct ParameterType< PropertyNotification::NotifyMode >
: public BasicType< PropertyNotification::NotifyMode > {};

namespace SceneGraph
{

class Animation;
class DiscardQueue;
class RenderTaskList;
class RenderTaskProcessor;


/**
 * UpdateManager maintains a scene graph i.e. a tree of nodes as well as
 * other scene graph property owning objects.
 * It controls the Update traversal, in which nodes are repositioned/animated,
 * and organizes the the culling and rendering of the scene.
 * It also maintains the lifecycle of nodes and other property owners that are
 * disconnected from the scene graph.
 */
class UpdateManager
{
public:

  /**
   * Construct a new UpdateManager.
   * @param[in] notificationManager This should be notified when animations have finished.
   * @param[in] animationPlaylist The CompleteNotificationInterface that handles the animations
   * @param[in] propertyNotifier The PropertyNotifier
   * @param[in] discardQueue Nodes are added here when disconnected from the scene-graph.
   * @param[in] controller After messages are flushed, we request a render from the RenderController.
   * @param[in] renderTaskProcessor Handles RenderTasks and RenderInstrucitons.
   * @param[in] graphicsController The graphics backend
   */
  UpdateManager( NotificationManager& notificationManager,
                 CompleteNotificationInterface& animationPlaylist,
                 PropertyNotifier& propertyNotifier,
                 DiscardQueue& discardQueue,
                 Integration::RenderController& controller,
                 RenderTaskProcessor& renderTaskProcessor,
                 Graphics::Controller& graphicsController );

  /**
   * Destructor.
   */
  virtual ~UpdateManager();

  // Node connection methods

  /**
   * Installs a new layer as the root node.
   * @pre The UpdateManager does not already have an installed root node.
   * @pre The layer is of derived Node type Layer.
   * @pre The layer does not have a parent.
   * @param[in] layer The new root node.
   * @post The node is owned by UpdateManager.
   */
  void InstallRoot( OwnerPointer<Layer>& layer );

  /**
   * Add a Node; UpdateManager takes ownership.
   * @pre The node does not have a parent.
   * @note even though nodes are pool allocated, they also contain other heap allocated data, thus using OwnerPointer when transferring the data
   * @param[in] node The node to add.
   */
  void AddNode( OwnerPointer<Node>& node );

  /**
   * Connect a Node to the scene-graph.
   * A disconnected Node has has no parent or children, and its properties cannot be animated/constrained.
   * @pre The node does not already have a parent.
   * @param[in] parent The new parent node.
   */
  void ConnectNode( Node* parent, Node* node );

  /**
   * Disconnect a Node from the scene-graph.
   * A disconnected Node has has no parent or children, and its properties cannot be animated/constrained.
   * @pre The node has a parent.
   * @param[in] node The node to disconnect.
   */
  void DisconnectNode( Node* node );

  /**
   * Destroy a Node owned by UpdateManager.
   * This is not immediate; Nodes are passed to the RenderQueue to allow GL resources to be deleted.
   * In the following update, the previously queued Nodes may be deleted.
   * @pre The node has been disconnected from the scene-graph i.e. has no parent or children.
   * @param[in] node The node to destroy.
   */
  void DestroyNode( Node* node );

  /**
   * Add a camera on scene
   * @param[in] camera The camera to add
   */
  void AddCamera( OwnerPointer< Camera >& camera );

  /**
   * Remove a camera from scene
   * @param[in] camera to remove
   */
  void RemoveCamera( Camera* camera );

  /**
   * Add a newly created object.
   * @param[in] object The object to add.
   * @post The object is owned by UpdateManager.
   */
  void AddObject( OwnerPointer<PropertyOwner>& object );

  /**
   * Remove an object.
   * @param[in] object The object to remove.
   */
  void RemoveObject( PropertyOwner* object );

  /**
   * Add a newly created render task list.
   * @param[in] taskList The render task list to add.
   * @post The render task list is owned by UpdateManager.
   */
  void AddRenderTaskList( OwnerPointer<RenderTaskList>& taskList );

  /**
   * Remove a render task list.
   * @param[in] taskList The render task list to remove.
   */
  void RemoveRenderTaskList( RenderTaskList* taskList );

  // Animations

  /**
   * Add a newly created animation.
   * @param[in] animation The animation to add.
   * @post The animation is owned by UpdateManager.
   */
  void AddAnimation( OwnerPointer< SceneGraph::Animation >& animation );

  /**
   * Stop an animation.
   * @param[in] animation The animation to stop.
   */
  void StopAnimation( Animation* animation );

  /**
   * Remove an animation.
   * @param[in] animation The animation to remove.
   */
  void RemoveAnimation( Animation* animation );

  /**
   * Query whether any animations are currently running.
   * @return True if any animations are running.
   */
  bool IsAnimationRunning() const;

  /**
   * Add a property resetter. UpdateManager takes ownership of the object.
   * It will be killed by UpdateManager when the associated animator or
   * constraint has finished; or the property owner of the property is destroyed.
   */
  void AddPropertyResetter( OwnerPointer<PropertyResetterBase>& propertyResetter );

  // Property Notification

  /**
   * Add a newly created property notification
   * @param[in] propertyNotification The notification to add
   * @post The propertyNotification is owned by UpdateManager.
   */
  void AddPropertyNotification( OwnerPointer< PropertyNotification >& propertyNotification );

  /**
   * Remove a property notification
   * @param[in] propertyNotification The notification to remove
   */
  void RemovePropertyNotification( PropertyNotification* propertyNotification );

  /**
   * Set Notify state for PropertyNotification
   * @param[in] propertyNotification The notification to remove
   * @param[in] notifyMode The notification mode.
   */
  void PropertyNotificationSetNotify( PropertyNotification* propertyNotification, PropertyNotification::NotifyMode notifyMode );

  // Shaders

  /**
   * Add a newly created shader.
   * @param[in] shader The shader to add.
   * @post The shader is owned by the UpdateManager.
   */
  void AddShader( OwnerPointer< Shader >& shader );

  /**
   * Remove a shader.
   * @pre The shader has been added to the UpdateManager.
   * @param[in] shader The shader to remove.
   * @post The shader is destroyed.
   */
  void RemoveShader( Shader* shader );

  // Renderers

  /**
   * Add a new renderer to scene
   * @param renderer to add
   */
  void AddRenderer( OwnerPointer< Renderer >& renderer );

  /**
   * Add a renderer from scene
   * @param renderer to remove
   */
  void RemoveRenderer( Renderer* renderer );

  // Gestures

  /**
   * Set the pan gesture processor.
   * Pan Gesture processor lives for the lifetime of UpdateManager
   * @param[in] gesture The gesture processor.
   * @post The gestureProcessor is owned by the UpdateManager.
   */
  void SetPanGestureProcessor( PanGesture* gestureProcessor );

  // Textures

  /**
   * Add a newly created TextureSet.
   * @param[in] textureSet The texture set to add.
   * @post The TextureSet is owned by the UpdateManager.
   */
  void AddTextureSet( OwnerPointer< TextureSet >& textureSet );

  /**
   * Remove a TextureSet.
   * @pre The TextureSet has been added to the UpdateManager.
   * @param[in] textureSet The TextureSet to remove.
   * @post The TextureSet is destroyed.
   */
  void RemoveTextureSet( TextureSet* textureSet );

  // Render tasks

  /**
   * Get the scene graph side list of RenderTasks.
   * @param[in] systemLevel True if using the system-level overlay.
   * @return The list of render tasks
   */
  RenderTaskList* GetRenderTaskList( bool systemLevel );

// Message queue handling

  /**
   * Reserve space for another message in the queue; this must then be initialized by the caller.
   * The message will be read from the update-thread after the next FlushMessages is called.
   * @post Calling this method may invalidate any previously returned slots.
   * @param[in] size The message size with respect to the size of type "char".
   * @param[in] updateScene A flag, when true denotes that the message will cause the scene-graph node tree to require an update.
   * @note the default value of updateScene should match that in EventThreadServices::ReserveMessageSlot.
   * @return A pointer to the first char allocated for the message.
   */
  uint32_t* ReserveMessageSlot( uint32_t size, bool updateScene = true );

  /**
   * @return the current event-buffer index.
   */
  BufferIndex GetEventBufferIndex() const
  {
    // inlined as its called often from event thread
    return mSceneGraphBuffers.GetEventBufferIndex();
  }

  /**
   * Called by the event-thread to signal that FlushQueue will be called
   * e.g. when it has finished event processing.
   */
  void EventProcessingStarted();

  /**
   * Flush the set of messages, which were previously stored with QueueMessage().
   * Calls to this thread-safe method should be minimized, to avoid thread blocking.
   *
   * @return True if there are messages to process.
   */
  bool FlushQueue();

  /**
   * Add a new sampler to Update Manager
   * @param[in] sampler The sampler to add
   * The sampler will be owned by UpdateManager
   */
  void AddSampler( OwnerPointer< SceneGraph::Sampler >& sampler );

  /**
   * Removes an existing sampler from UpdateManager
   * @param[in] sampler The sampler to remove
   */
  void RemoveSampler( SceneGraph::Sampler* sampler );

  /**
   * Add a new property buffer to UpdateManager
   * @param[in] propertryBuffer The property buffer to add
   * The property buffer will be owned by UpdateManager
   */
  void AddPropertyBuffer( OwnerPointer< SceneGraph::PropertyBuffer >& propertyBuffer );

  /**
   * Removes an existing PropertyBuffer from UpdateManager
   * @param[in] propertryBuffer The property buffer to remove
   */
  void RemovePropertyBuffer( SceneGraph::PropertyBuffer* propertryBuffer );

  /**
   * Adds a geometry to the UpdateManager
   * @param[in] geometry The geometry to add
   * The geometry will be owned by UpdateManager
   */
  void AddGeometry( OwnerPointer< SceneGraph::Geometry >& geometry );

  /**
   * Removes an existing Geometry from UpdateManager
   * @param[in] geometry The geometry to remove
   */
  void RemoveGeometry( SceneGraph::Geometry* geometry );

  /**
   * Adds a texture to the update manager
   * @param[in] texture The texture to add
   * The texture will be owned by UpdateManager
   */
  void AddTexture( OwnerPointer< SceneGraph::Texture >& texture );

  /**
   * Removes a texture from the update manager
   * @param[in] texture The texture to remove
   */
  void RemoveTexture( SceneGraph::Texture* texture );

  /**
   * Adds a framebuffer to the update manager
   * @param[in] frameBuffer The framebuffer to add
   * The framebuffer will be owned by UpdateManager
   */
  void AddFrameBuffer( OwnerPointer<SceneGraph::FrameBuffer>& frameBuffer );

  /**
   * Removes a FrameBuffer from the update manager
   * @param[in] frameBuffer The FrameBuffer to remove
   */
  void RemoveFrameBuffer( SceneGraph::FrameBuffer* frameBuffer );


public:

  /**
   * Performs an Update traversal on the scene-graph.
   * @param[in] elapsedSeconds The elapsed time that should be applied to animations.
   * @param[in] lastVSyncTimeMilliseconds The last time, in milliseconds, that we had a VSync.
   * @param[in] nextVSyncTimeMilliseconds The estimated time, in milliseconds, of the next VSync.
   * @param[in] renderToFboEnabled Whether rendering into the Frame Buffer Object is enabled.
   * @param[in] isRenderingToFbo   Whether this frame is being rendered into the Frame Buffer Object.
   * @return True if further updates are required e.g. during animations.
   */
  uint32_t Update( float elapsedSeconds,
                   uint32_t lastVSyncTimeMilliseconds,
                   uint32_t nextVSyncTimeMilliseconds,
                   bool renderToFboEnabled,
                   bool isRenderingToFbo );

  /**
   * Set the background color i.e. the glClear color used at the beginning of each frame.
   * @param[in] color The new background color.
   */
  void SetBackgroundColor(const Vector4& color);

  /**
   * Set the default surface rect.
   * @param[in] rect The rect value representing the surface.
   */
  void SetDefaultSurfaceRect( const Rect<int>& rect );

  void SetDefaultSurfaceRenderTarget(const Graphics::RenderTargetCreateInfo& rtInfo);

  /**
   * @copydoc Dali::Stage::KeepRendering()
   */
  void KeepRendering( float durationSeconds );

  /**
   * @copydoc Dali::DevelStage::SetRenderingBehavior()
   */
  void SetRenderingBehavior( DevelStage::Rendering renderingBehavior );

  /**
   * Sets the depths of all layers.
   * @param layers The layers in depth order.
   * @param[in] rootLayer The root layer of the sorted layers.
   */
  void SetLayerDepths( const std::vector< Layer* >& layers, const Layer* rootLayer );

  /**
   * Set the depth indices of all nodes (in LayerUI's)
   * @param[in] nodeDepths A vector of nodes and associated depth indices
   */
  void SetDepthIndices( OwnerPointer< NodeDepths >& nodeDepths );

  /**
   * Query wheter the default surface rect is changed or not.
   * @return true if the default surface rect is changed.
   */
  bool IsDefaultSurfaceRectChanged();

  /**
   * Adds an implementation of the FrameCallbackInterface.
   * @param[in] frameCallback An OwnerPointer to the SceneGraph FrameCallback object
   * @param[in] rootNode A pointer to the root node to apply the FrameCallback to
   */
  void AddFrameCallback( OwnerPointer< FrameCallback >& frameCallback, const Node* rootNode );

  /**
   * Removes the specified implementation of FrameCallbackInterface.
   * @param[in] frameCallback A pointer to the implementation of the FrameCallbackInterface to remove.
   */
  void RemoveFrameCallback( FrameCallbackInterface* frameCallback );

  /**
   * The graphics implementation is about to die. Before we cleanup
   * core, destroy all graphics objects whilst we still have a chance.
   */
  void DestroyGraphicsObjects();

private:

  // Undefined
  UpdateManager(const UpdateManager&);

  // Undefined
  UpdateManager& operator=(const UpdateManager& rhs);

  /**
   * Helper to check whether the update-thread should keep going.
   * @param[in] elapsedSeconds The time in seconds since the previous update.
   * @return True if the update-thread should keep going.
   */
  uint32_t KeepUpdatingCheck( float elapsedSeconds ) const;

  /**
   * Helper to reset all Node properties
   * @param[in] bufferIndex to use
   */
  void ResetProperties( BufferIndex bufferIndex );

  /**
   * Perform gesture updates.
   * @param[in] bufferIndex to use
   * @param[in] lastVSyncTime  The last VSync time in milliseconds.
   * @param[in] nextVSyncTime  The estimated time of the next VSync in milliseconds.
   * @return true, if any properties were updated.
   */
  bool ProcessGestures( BufferIndex bufferIndex, uint32_t lastVSyncTimeMilliseconds, uint32_t nextVSyncTimeMilliseconds );

  /**
   * Perform animation updates
   * @param[in] bufferIndex to use
   * @param[in] elapsedSeconds time since last frame
   */
  void Animate( BufferIndex bufferIndex, float elapsedSeconds );

  /**
   * Applies constraints to CustomObjects
   * @param[in] bufferIndex to use
   */
  void ConstrainCustomObjects( BufferIndex bufferIndex );

  /**
   * Applies constraints to RenderTasks
   * @param[in] bufferIndex to use
   */
  void ConstrainRenderTasks( BufferIndex bufferIndex );

  /**
   * Applies constraints to Shaders
   * @param[in] bufferIndex to use
   */
  void ConstrainShaders( BufferIndex bufferIndex );

  /**
   * Perform property notification updates
   * @param[in] bufferIndex to use
   */
  void ProcessPropertyNotifications( BufferIndex bufferIndex );

  /**
   * Pass shader binaries queued here on to event thread.
   */
  void ForwardCompiledShadersToEventThread();

  /**
   * Update node shaders, opacity, geometry etc.
   * @param[in] bufferIndex to use
   */
  void UpdateNodes( BufferIndex bufferIndex );

  /**
   * Update Renderers
   * @param[in] bufferIndex to use
   */
  void UpdateRenderers( BufferIndex bufferIndex );

  /**
   * Ensure all nodes inform renderers of any changes to uniform maps
   * @param[in] bufferIndex to use
   */
  void PrepareNodes( BufferIndex bufferIndex );

  /**
   * Prepare renderer graphics
   * @param[in] bufferIndex to use
   */
  void PrepareRenderers( BufferIndex bufferIndex );

public:

  /**
   * Schedules texture upload for batch upload
   * @param[in] texture valid pointer to the texture
   * @param[in] pixelData pixel data
   * @param[in] params upload parameters
   */
  void UploadTexture( Texture* texture,
                      PixelDataPtr pixelData,
                      const Internal::Texture::UploadParams& params );
private:

  // needs to be direct member so that getter for event buffer can be inlined
  SceneGraphBuffers mSceneGraphBuffers;

  struct Impl;
  std::unique_ptr<Impl> mImpl;

};

// Messages for UpdateManager

inline void InstallRootMessage( UpdateManager& manager, OwnerPointer<Layer>& root )
{
  // Message has ownership of Layer while in transit from event -> update
  typedef MessageValue1< UpdateManager, OwnerPointer<Layer> > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::InstallRoot, root );
}

inline void AddNodeMessage( UpdateManager& manager, OwnerPointer<Node>& node )
{
  // Message has ownership of Node while in transit from event -> update
  typedef MessageValue1< UpdateManager, OwnerPointer<Node> > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddNode, node );
}

inline void ConnectNodeMessage( UpdateManager& manager, const Node& constParent, const Node& constChild )
{
  // Update thread can edit the object
  Node& parent = const_cast< Node& >( constParent );
  Node& child = const_cast< Node& >( constChild );

  typedef MessageValue2< UpdateManager, Node*, Node* > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::ConnectNode, &parent, &child );
}

inline void DisconnectNodeMessage( UpdateManager& manager, const Node& constNode )
{
  // Scene graph thread can modify this object.
  Node& node = const_cast< Node& >( constNode );

  typedef MessageValue1< UpdateManager, Node* > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::DisconnectNode, &node );
}

inline void DestroyNodeMessage( UpdateManager& manager, const Node& constNode )
{
  // Scene graph thread can destroy this object.
  Node& node = const_cast< Node& >( constNode );

  typedef MessageValue1< UpdateManager, Node* > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::DestroyNode, &node );
}

inline void AddCameraMessage( UpdateManager& manager, OwnerPointer< Camera >& camera )
{
  // Message has ownership of Camera while in transit from event -> update
  typedef MessageValue1< UpdateManager, OwnerPointer< Camera > > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddCamera, camera );
}

inline void RemoveCameraMessage( UpdateManager& manager, const Camera* camera )
{
  typedef MessageValue1< UpdateManager, Camera* > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveCamera, const_cast<Camera*>( camera ) );
}

inline void AddObjectMessage( UpdateManager& manager, OwnerPointer<PropertyOwner>& object )
{
  // Message has ownership of object while in transit from event -> update
  typedef MessageValue1< UpdateManager, OwnerPointer<PropertyOwner> > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddObject, object );
}

inline void RemoveObjectMessage( UpdateManager& manager, const PropertyOwner* object )
{
  typedef MessageValue1< UpdateManager, PropertyOwner* > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveObject, const_cast<PropertyOwner*>( object ) );
}

inline void AddAnimationMessage( UpdateManager& manager, OwnerPointer< SceneGraph::Animation >& animation )
{
  typedef MessageValue1< UpdateManager, OwnerPointer< SceneGraph::Animation > > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddAnimation, animation );
}

inline void StopAnimationMessage( UpdateManager& manager, const Animation& constAnimation )
{
  // The scene-graph thread owns this object so it can safely edit it.
  Animation& animation = const_cast< Animation& >( constAnimation );

  typedef MessageValue1< UpdateManager, Animation* > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::StopAnimation, &animation );
}

inline void RemoveAnimationMessage( UpdateManager& manager, const Animation& constAnimation )
{
  // The scene-graph thread owns this object so it can safely edit it.
  Animation& animation = const_cast< Animation& >( constAnimation );

  typedef MessageValue1< UpdateManager, Animation* > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveAnimation, &animation );
}

inline void AddRenderTaskListMessage( UpdateManager& manager, OwnerPointer< SceneGraph::RenderTaskList >& taskList )
{
  typedef MessageValue1< UpdateManager, OwnerPointer< SceneGraph::RenderTaskList > > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddRenderTaskList, taskList );
}

inline void RemoveRenderTaskListMessage( UpdateManager& manager, const RenderTaskList& constTaskList )
{
  // The scene-graph thread owns this object so it can safely edit it.
  RenderTaskList& taskList = const_cast< RenderTaskList& >( constTaskList );

  typedef MessageValue1< UpdateManager, RenderTaskList* > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveRenderTaskList, &taskList );
}

inline void AddPropertyNotificationMessage( UpdateManager& manager, OwnerPointer< PropertyNotification >& propertyNotification )
{
  // Message has ownership of PropertyNotification while in transit from event -> update
  typedef MessageValue1< UpdateManager, OwnerPointer< PropertyNotification > > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddPropertyNotification, propertyNotification );
}

inline void RemovePropertyNotificationMessage( UpdateManager& manager, const PropertyNotification& constPropertyNotification )
{
  // The scene-graph thread owns this object so it can safely edit it.
  PropertyNotification& propertyNotification = const_cast< PropertyNotification& >( constPropertyNotification );

  typedef MessageValue1< UpdateManager, PropertyNotification* > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemovePropertyNotification, &propertyNotification );
}

inline void PropertyNotificationSetNotifyModeMessage( UpdateManager& manager,
                                                      const PropertyNotification* constPropertyNotification,
                                                      PropertyNotification::NotifyMode notifyMode )
{
  // The scene-graph thread owns this object so it can safely edit it.
  PropertyNotification* propertyNotification = const_cast< PropertyNotification* >( constPropertyNotification );

  typedef MessageValue2< UpdateManager, PropertyNotification*, PropertyNotification::NotifyMode > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::PropertyNotificationSetNotify, propertyNotification, notifyMode );
}

inline void AddShaderMessage( UpdateManager& manager, OwnerPointer< Shader >& shader )
{
  typedef MessageValue1< UpdateManager, OwnerPointer< Shader > > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddShader, shader );
}

inline void RemoveShaderMessage( UpdateManager& manager, const Shader* shader )
{
  typedef MessageValue1< UpdateManager, Shader* > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveShader, const_cast<Shader*>( shader ) );
}

inline void SetBackgroundColorMessage( UpdateManager& manager, const Vector4& color )
{
  typedef MessageValue1< UpdateManager, Vector4 > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetBackgroundColor, color );
}

inline void SetDefaultSurfaceRectMessage( UpdateManager& manager, const Rect<int32_t>& rect  )
{
  typedef MessageValue1< UpdateManager, Rect<int32_t> > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetDefaultSurfaceRect, rect );
}

inline void SetDefaultSurfaceRenderTargetCreateInfoMessage( UpdateManager& manager, const Graphics::RenderTargetCreateInfo& rtInfo  )
{
  typedef MessageValue1< UpdateManager, Graphics::RenderTargetCreateInfo > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetDefaultSurfaceRenderTarget, rtInfo );
}


inline void KeepRenderingMessage( UpdateManager& manager, float durationSeconds )
{
  typedef MessageValue1< UpdateManager, float > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::KeepRendering, durationSeconds );
}

inline void SetRenderingBehaviorMessage( UpdateManager& manager, DevelStage::Rendering renderingBehavior )
{
  typedef MessageValue1< UpdateManager, DevelStage::Rendering > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetRenderingBehavior, renderingBehavior );
}

/**
 * Create a message for setting the depth of a layer
 * @param[in] manager The update manager
 * @param[in] layers list of layers
 * @param[in] rootLayer The rool layer
 */
inline void SetLayerDepthsMessage( UpdateManager& manager, const std::vector< Layer* >& layers, const Layer* rootLayer )
{
  typedef MessageValue2< UpdateManager, std::vector< Layer* >, const Layer* > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetLayerDepths, layers, rootLayer );
}

inline void AddRendererMessage( UpdateManager& manager, OwnerPointer< Renderer >& object )
{
  typedef MessageValue1< UpdateManager, OwnerPointer< Renderer > > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );
  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddRenderer, object );
}

inline void RemoveRendererMessage( UpdateManager& manager, const Renderer& object )
{
  typedef MessageValue1< UpdateManager, Renderer* > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );
  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveRenderer, const_cast<Renderer*>( &object ) );
}

// The render thread can safely change the Shader
inline void AddTextureSetMessage( UpdateManager& manager, OwnerPointer< TextureSet >& textureSet )
{
  typedef MessageValue1< UpdateManager, OwnerPointer< TextureSet > > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddTextureSet, textureSet );
}

inline void RemoveTextureSetMessage( UpdateManager& manager, TextureSet& textureSet )
{
  typedef MessageValue1< UpdateManager, TextureSet* > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveTextureSet, &textureSet );
}

inline void AddSamplerMessage( UpdateManager& manager, OwnerPointer< SceneGraph::Sampler >& sampler )
{
  // Message has ownership of Sampler while in transit from event -> update
  typedef MessageValue1< UpdateManager, OwnerPointer< SceneGraph::Sampler > > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddSampler, sampler );
}

inline void RemoveSamplerMessage( UpdateManager& manager, SceneGraph::Sampler& sampler )
{
  typedef MessageValue1< UpdateManager, SceneGraph::Sampler* > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveSampler, &sampler );
}

inline void AddPropertyBuffer( UpdateManager& manager, OwnerPointer< SceneGraph::PropertyBuffer >& propertyBuffer )
{
  // Message has ownership of propertyBuffer while in transit from event -> update
  typedef MessageValue1< UpdateManager, OwnerPointer< SceneGraph::PropertyBuffer > > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddPropertyBuffer, propertyBuffer );
}

inline void RemovePropertyBuffer( UpdateManager& manager, SceneGraph::PropertyBuffer& propertyBuffer )
{
  typedef MessageValue1< UpdateManager, SceneGraph::PropertyBuffer*  > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemovePropertyBuffer, &propertyBuffer );
}

inline void AddGeometryMessage( UpdateManager& manager, OwnerPointer< SceneGraph::Geometry >& geometry )
{
  // Message has ownership of Geometry while in transit from event -> update
  typedef MessageValue1< UpdateManager, OwnerPointer< SceneGraph::Geometry > > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddGeometry, geometry );
}

inline void RemoveGeometryMessage( UpdateManager& manager, SceneGraph::Geometry& geometry )
{
  typedef MessageValue1< UpdateManager, SceneGraph::Geometry*  > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveGeometry, &geometry );
}

inline void AddTextureMessage( UpdateManager& manager, OwnerPointer< SceneGraph::Texture >& texture )
{
  // Message has ownership of Texture while in transit from event -> update
  typedef MessageValue1< UpdateManager, OwnerPointer< SceneGraph::Texture > > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddTexture, texture );
}

inline void RemoveTextureMessage( UpdateManager& manager, SceneGraph::Texture& texture )
{
  typedef MessageValue1< UpdateManager, SceneGraph::Texture*  > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveTexture, &texture );
}

inline void AddFrameBufferMessage( UpdateManager& manager, OwnerPointer<SceneGraph::FrameBuffer>& frameBuffer )
{
  typedef MessageValue1< UpdateManager, OwnerPointer<SceneGraph::FrameBuffer> > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddFrameBuffer, frameBuffer );
}

inline void RemoveFrameBufferMessage( UpdateManager& manager, SceneGraph::FrameBuffer& frameBuffer )
{
  typedef MessageValue1< UpdateManager, SceneGraph::FrameBuffer*  > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveFrameBuffer, &frameBuffer );
}

inline void SetDepthIndicesMessage( UpdateManager& manager, OwnerPointer< NodeDepths >& nodeDepths )
{
  typedef MessageValue1< UpdateManager, OwnerPointer< NodeDepths > > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetDepthIndices, nodeDepths );
}

inline void AddResetterMessage( UpdateManager& manager, OwnerPointer<PropertyResetterBase> resetter )
{
  typedef MessageValue1< UpdateManager, OwnerPointer<PropertyResetterBase> > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddPropertyResetter, resetter );
}

inline void AddFrameCallbackMessage( UpdateManager& manager, OwnerPointer< FrameCallback >& frameCallback, const Node& rootNode )
{
  typedef MessageValue2< UpdateManager, OwnerPointer< FrameCallback >, const Node* > LocalType;
  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddFrameCallback, frameCallback, &rootNode );
}

inline void RemoveFrameCallbackMessage( UpdateManager& manager, FrameCallbackInterface& frameCallback )
{
  typedef MessageValue1< UpdateManager, FrameCallbackInterface* > LocalType;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveFrameCallback, &frameCallback );
}

inline void UploadTextureMessage( EventThreadServices&             eventThreadServices,
                                  SceneGraph::Texture&             texture,
                                  PixelDataPtr                     pixelData,
                                  const Internal::Texture::UploadParams&  params )
{
  typedef MessageValue3< UpdateManager, Texture*, PixelDataPtr, Internal::Texture::UploadParams > LocalType;

  auto& manager = eventThreadServices.GetUpdateManager();

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ), false );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &SceneGraph::UpdateManager::UploadTexture, &texture, pixelData, params );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_UPDATE_MANAGER_H
