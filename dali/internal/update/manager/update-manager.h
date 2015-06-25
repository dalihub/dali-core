#ifndef __DALI_INTERNAL_SCENE_GRAPH_UPDATE_MANAGER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_UPDATE_MANAGER_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/common/dali-common.h>

#include <dali/integration-api/resource-declarations.h>

#include <dali/internal/common/message.h>
#include <dali/internal/common/type-abstraction-enums.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/animation/scene-graph-animation.h>
#include <dali/internal/update/common/scene-graph-buffers.h>
#include <dali/internal/update/common/scene-graph-property-notification.h>
#include <dali/internal/update/manager/object-owner-container.h>
#include <dali/internal/update/node-attachments/node-attachment.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>

#include <dali/internal/render/shaders/scene-graph-shader.h>

namespace Dali
{

namespace Integration
{
class GlSyncAbstraction;
class RenderController;
struct DynamicsWorldSettings;

} // namespace Integration

namespace Internal
{

class PropertyNotifier;
struct DynamicsWorldSettings;
class NotificationManager;
class CompleteNotificationInterface;
class ResourceManager;
class TouchResampler;

// value types used by messages
template <> struct ParameterType< PropertyNotification::NotifyMode >
: public BasicType< PropertyNotification::NotifyMode > {};

namespace SceneGraph
{

class Animation;
class DiscardQueue;
class PanGesture;
class RenderManager;
class RenderTaskList;
class RenderQueue;
class DynamicsWorld;
class TextureCache;
class Geometry;
class PropertyBuffer;
class Material;
class Sampler;
class RendererAttachment;

/**
 * UpdateManager maintains a scene graph i.e. a tree of nodes and attachments and
 * other property owner objects.
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
   * @param[in] glSyncAbstraction Used to determine when framebuffers are ready
   * @param[in] animationFinishedNotifier The CompleteNotificationInterface that handles animation completions
   * @param[in] propertyNotifier The PropertyNotifier
   * @param[in] resourceManager The resource manager used to load textures etc.
   * @param[in] discardQueue Nodes are added here when disconnected from the scene-graph.
   * @param[in] controller After messages are flushed, we request a render from the RenderController.
   * @param[in] renderManager This is responsible for rendering the results of each "update".
   * @param[in] renderQueue Used to queue messages for the next render.
   * @param[in] textureCache Used for caching textures.
   * @param[in] touchResampler Used for re-sampling touch events.
   */
  UpdateManager( NotificationManager& notificationManager,
                 Integration::GlSyncAbstraction& glSyncAbstraction,
                 CompleteNotificationInterface& animationFinishedNotifier,
                 PropertyNotifier& propertyNotifier,
                 ResourceManager& resourceManager,
                 DiscardQueue& discardQueue,
                 Integration::RenderController& controller,
                 RenderManager& renderManager,
                 RenderQueue& renderQueue,
                 TextureCache& textureCache,
                 TouchResampler& touchResampler );

  /**
   * Destructor. Not virtual as this is not a base class
   */
  ~UpdateManager();

  // Node connection methods

  /**
   * Get the scene graph side list of RenderTasks.
   * @param[in] systemLevel True if using the system-level overlay.
   * @return The list of render tasks
   */
  RenderTaskList* GetRenderTaskList( bool systemLevel );

  /**
   * Installs a new layer as the root node.
   * @pre The UpdateManager does not already have an installed root node.
   * @pre The layer is of derived Node type Layer.
   * @pre The layer does not have a parent.
   * @param[in] layer The new root node.
   * @param[in] systemLevel True if using the system-level overlay.
   * @post The node is owned by UpdateManager.
   */
  void InstallRoot( Layer* layer, bool systemLevel );

  /**
   * Add a Node; UpdateManager takes ownership.
   * @pre The node does not have a parent.
   * @param[in] node The node to add.
   */
  void AddNode( Node* node );

  /**
   * Connect a Node to the scene-graph.
   * A disconnected Node has has no parent or children, and its properties cannot be animated/constrained.
   * @pre The node does not already have a parent.
   * @param[in] node The new parent node.
   * @param[in] node The node to connect.
   */
  void ConnectNode( Node* parent, Node* node, int index );

  /**
   * Disconnect a Node from the scene-graph.
   * A disconnected Node has has no parent or children, and its properties cannot be animated/constrained.
   * @pre The node has a parent.
   * @param[in] node The node to disconnect.
   */
  void DisconnectNode( Node* node );

  /**
   * Called when a property is set on a disconnected Node (via public API)
   * A disconnected Node has has no parent or children, and its properties cannot be animated/constrained.
   * @pre The node does not have a parent.
   * @param[in] node The node to set as "active".
   */
  void SetNodeActive( Node* node );

  /**
   * Destroy a Node owned by UpdateManager.
   * This is not immediate; Nodes are passed to the RenderQueue to allow GL resources to be deleted.
   * In the following update, the previously queued Nodes may be deleted.
   * @pre The node has been disconnected from the scene-graph i.e. has no parent or children.
   * @param[in] node The node to destroy.
   */
  void DestroyNode( Node* node );

  /**
   * Attach an object to a Node.
   * The UpdateManager is responsible for calling NodeAttachment::Initialize().
   * @param[in] node The node which will own the attachment.
   * @param[in] attachment The object to attach.
   */
  void AttachToNode( Node* node, NodeAttachment* attachment );

  /**
   * Attach a renderer to the scene graph
   */
  void AttachToSceneGraph( RendererAttachment* renderer );


  /**
   * Add a newly created object.
   * @param[in] object The object to add.
   * @post The object is owned by UpdateManager.
   */
  void AddObject( PropertyOwner* object );

  /**
   * Remove an object.
   * @param[in] object The object to remove.
   */
  void RemoveObject( PropertyOwner* object );

  // Animations

  /**
   * Add a newly created animation.
   * @param[in] animation The animation to add.
   * @post The animation is owned by UpdateManager.
   */
  void AddAnimation( Animation* animation );

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

  // Property Notification

  /**
   * Add a newly created property notification
   * @param[in] propertyNotification The notification to add
   * @post The propertyNotification is owned by UpdateManager.
   */
  void AddPropertyNotification( PropertyNotification* propertyNotification );

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

  /**
   * @brief Get the geometry owner
   *
   * @return The geometry owner
   */
  ObjectOwnerContainer< Geometry >& GetGeometryOwner();

  /**
   * @brief Get the material owner
   *
   * @return The material owner
   */
  ObjectOwnerContainer< Material >& GetMaterialOwner();

  /**
   * @brief Get the sampler owner
   *
   * @return The sampler owner
   */
  ObjectOwnerContainer< Sampler >& GetSamplerOwner();

  /**
   * @brief Get the property buffer owner
   *
   * @return The property buffer owner
   */
  ObjectOwnerContainer< PropertyBuffer >& GetPropertyBufferOwner();


  // Shaders

  /**
   * Add a newly created shader.
   * @param[in] shader The shader to add.
   * @post The shader is owned by the UpdateManager.
   */
  void AddShader(Shader* shader);

  /**
   * Remove a shader.
   * @pre The shader has been added to the UpdateManager.
   * @param[in] shader The shader to remove.
   * @post The shader is destroyed.
   */
  void RemoveShader(Shader* shader);

  /**
   * Set the shader program for a Shader object
   * @param[in] shader        The shader to modify
   * @param[in] resourceId    A ResourceManager ticket ID for the program data (source and compiled binary)
   * @param[in] shaderHash    hash key created with vertex and fragment shader code
   * @param[in] modifiesGeometry True if the vertex shader modifies geometry
   */
  void SetShaderProgram( Shader* shader, Integration::ResourceId resourceId, size_t shaderHash, bool modifiesGeometry );

  /**
   * Add a newly created gesture.
   * @param[in] gesture The gesture to add.
   * @post The gesture is owned by the UpdateManager.
   */
  void AddGesture( PanGesture* gesture );

  /**
   * Remove a gesture.
   * @pre The gesture has been added to the UpdateManager.
   * @param[in] gesture The gesture to remove.
   * @post The gesture is destroyed.
   */
  void RemoveGesture( PanGesture* gesture );

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
  unsigned int* ReserveMessageSlot( std::size_t size, bool updateScene = true );

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

public:

  /**
   * Performs an Update traversal on the scene-graph.
   * @param[in] elapsedSeconds The elapsed time that should be applied to animations.
   * @param[in] lastVSyncTimeMilliseconds The last time, in milliseconds, that we had a VSync.
   * @param[in] nextVSyncTimeMilliseconds The estimated time, in milliseconds, of the next VSync.
   * @return True if further updates are required e.g. during animations.
   */
  unsigned int Update( float elapsedSeconds, unsigned int lastVSyncTimeMilliseconds, unsigned int nextVSyncTimeMilliseconds );

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

  /**
   * @copydoc Dali::Stage::KeepRendering()
   */
  void KeepRendering( float durationSeconds );

  /**
   * Sets the depths of all layers.
   * @param layers The layers in depth order.
   * @param[in] systemLevel True if using the system-level overlay.
   */
  void SetLayerDepths( const std::vector< Layer* >& layers, bool systemLevel );

#ifdef DALI_DYNAMICS_SUPPORT

  /**
   * Initialize the dynamics world
   * @param[in] world The dynamics world
   * @param[in] worldSettings The dynamics world settings
   * @param[in] debugShader The shader used for rendering dynamics debug information
   */
  void InitializeDynamicsWorld( DynamicsWorld* world, Integration::DynamicsWorldSettings* worldSettings );

  /**
   * Terminate the dynamics world
   */
  void TerminateDynamicsWorld();

#endif // DALI_DYNAMICS_SUPPORT

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
  unsigned int KeepUpdatingCheck( float elapsedSeconds ) const;

  /**
   * Helper to calculate new camera setup when root node resizes.
   * @param[in] updateBuffer The buffer to read the root node size from.
   */
  void UpdateProjectionAndViewMatrices(int updateBuffer);

  /**
   * Post process resources that have been updated by renderer
   */
  void PostProcessResources();

  /**
   * Helper to reset a Node properties.
   * @param[in] node The node.
   */
  void ResetNodeProperty( Node& node );

  /**
   * Helper to reset all Node properties
   */
  void ResetProperties();

  /**
   * Perform gesture updates.
   * @param[in]  lastVSyncTime  The last VSync time in milliseconds.
   * @param[in]  nextVSyncTime  The estimated time of the next VSync in milliseconds.
   * @return true, if any properties were updated.
   */
  bool ProcessGestures( unsigned int lastVSyncTimeMilliseconds, unsigned int nextVSyncTimeMilliseconds );

  /**
   * Perform animation updates
   * @param[in] elapsedSeconds time since last frame
   */
  void Animate( float elapsedSeconds );

  /**
   * Perform constraint updates.
   * @note Applies constraints to nodes first (depth first search order).
   * Then shader constraints second (construction order)
   */
  void ApplyConstraints();

  /**
   * Perform property notification updates
   */
  void ProcessPropertyNotifications();

  /**
   * Update the default camera.
   * This must be altered to match the root Node for 2D layouting.
   * @param[in] updateBuffer The buffer to read the root node size from.
   */
  void UpdateDefaultCamera( int updateBuffer );

  /**
   * Update node shaders, opacity, geometry etc.
   */
  void UpdateNodes();

private:

  // needs to be direct member so that getter for event buffer can be inlined
  SceneGraphBuffers mSceneGraphBuffers;

  struct Impl;
  Impl* mImpl;

};

// Messages for UpdateManager

inline void InstallRootMessage( UpdateManager& manager, Layer& root, bool systemLevel )
{
  typedef MessageValue2< UpdateManager, Layer*, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::InstallRoot, &root, systemLevel );
}

inline void AddNodeMessage( UpdateManager& manager, Node& node )
{
  typedef MessageValue1< UpdateManager, OwnerPointer<Node> > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddNode, &node );
}

inline void ConnectNodeMessage( UpdateManager& manager, const Node& constParent, const Node& constChild, int index )
{
  // Update thread can edit the object
  Node& parent = const_cast< Node& >( constParent );
  Node& child = const_cast< Node& >( constChild );

  typedef MessageValue3< UpdateManager, Node*, Node*, int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::ConnectNode, &parent, &child, index );
}

inline void DisconnectNodeMessage( UpdateManager& manager, const Node& constNode )
{
  // Scene graph thread can modify this object.
  Node& node = const_cast< Node& >( constNode );

  typedef MessageValue1< UpdateManager, Node* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::DisconnectNode, &node );
}

inline void DestroyNodeMessage( UpdateManager& manager, const Node& constNode )
{
  // Scene graph thread can destroy this object.
  Node& node = const_cast< Node& >( constNode );

  typedef MessageValue1< UpdateManager, Node* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::DestroyNode, &node );
}

inline void AttachToNodeMessage( UpdateManager& manager, const Node& constParent, NodeAttachment* attachment )
{
  // Scene graph thread can modify this object.
  Node& parent = const_cast< Node& >( constParent );

  // @todo MESH_REWORK Don't pass by owner pointer after merge with SceneGraph::RenderableAttachment ? (not needed if we split RendererAttachment to 2 objects)
  typedef MessageValue2< UpdateManager, Node*, NodeAttachmentOwner > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AttachToNode, &parent, attachment );
}

inline void AttachToSceneGraphMessage( UpdateManager& manager, RendererAttachment* renderer )
{
  // @todo MESH_REWORK Pass by owner pointer after merge with SceneGraph::RenderableAttachment
  typedef MessageValue1< UpdateManager, RendererAttachment* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AttachToSceneGraph, renderer );
}

inline void AddObjectMessage( UpdateManager& manager, PropertyOwner* object )
{
  typedef MessageValue1< UpdateManager, OwnerPointer<PropertyOwner> > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddObject, object );
}

inline void RemoveObjectMessage( UpdateManager& manager, PropertyOwner* object )
{
  typedef MessageValue1< UpdateManager, PropertyOwner* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveObject, object );
}

inline void AddAnimationMessage( UpdateManager& manager, Animation* animation )
{
  typedef MessageValue1< UpdateManager, Animation* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddAnimation, animation );
}

inline void StopAnimationMessage( UpdateManager& manager, const Animation& constAnimation )
{
  // The scene-graph thread owns this object so it can safely edit it.
  Animation& animation = const_cast< Animation& >( constAnimation );

  typedef MessageValue1< UpdateManager, Animation* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::StopAnimation, &animation );
}

inline void RemoveAnimationMessage( UpdateManager& manager, const Animation& constAnimation )
{
  // The scene-graph thread owns this object so it can safely edit it.
  Animation& animation = const_cast< Animation& >( constAnimation );

  typedef MessageValue1< UpdateManager, Animation* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveAnimation, &animation );
}

inline void AddPropertyNotificationMessage( UpdateManager& manager, PropertyNotification* propertyNotification )
{
  typedef MessageValue1< UpdateManager, PropertyNotification* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddPropertyNotification, propertyNotification );
}

inline void RemovePropertyNotificationMessage( UpdateManager& manager, const PropertyNotification& constPropertyNotification )
{
  // The scene-graph thread owns this object so it can safely edit it.
  PropertyNotification& propertyNotification = const_cast< PropertyNotification& >( constPropertyNotification );

  typedef MessageValue1< UpdateManager, PropertyNotification* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

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
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::PropertyNotificationSetNotify, propertyNotification, notifyMode );
}



// The render thread can safely change the Shader
inline void AddShaderMessage( UpdateManager& manager, Shader& shader )
{
  typedef MessageValue1< UpdateManager, OwnerPointer< Shader > > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddShader, &shader );
}

// The render thread can safely change the Shader
inline void RemoveShaderMessage( UpdateManager& manager, Shader& shader )
{
  typedef MessageValue1< UpdateManager, Shader* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveShader, &shader );
}

inline void SetShaderProgramMessage( UpdateManager& manager,
                                     Shader& shader,
                                     Integration::ResourceId resourceId,
                                     size_t shaderHash,
                                     bool modifiesGeometry )
{
  typedef MessageValue4< UpdateManager, Shader*, Integration::ResourceId, size_t, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetShaderProgram, &shader, resourceId, shaderHash, modifiesGeometry );
}

inline void SetBackgroundColorMessage( UpdateManager& manager, const Vector4& color )
{
  typedef MessageValue1< UpdateManager, Vector4 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetBackgroundColor, color );
}

inline void SetDefaultSurfaceRectMessage( UpdateManager& manager, const Rect<int>& rect  )
{
  typedef MessageValue1< UpdateManager, Rect<int> > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetDefaultSurfaceRect, rect );
}

inline void KeepRenderingMessage( UpdateManager& manager, float durationSeconds )
{
  typedef MessageValue1< UpdateManager, float > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::KeepRendering, durationSeconds );
}

/**
 * Create a message for setting the depth of a layer
 * @param[in] manager The update manager
 * @param[in] layers list of layers
 * @param[in] systemLevel True if the layers are added via the SystemOverlay API
 */
inline void SetLayerDepthsMessage( UpdateManager& manager, const std::vector< Layer* >& layers, bool systemLevel )
{
  typedef MessageValue2< UpdateManager, std::vector< Layer* >, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetLayerDepths, layers, systemLevel );
}

inline void AddGestureMessage( UpdateManager& manager, PanGesture* gesture )
{
  typedef MessageValue1< UpdateManager, PanGesture* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddGesture, gesture );
}

inline void RemoveGestureMessage( UpdateManager& manager, PanGesture* gesture )
{
  typedef MessageValue1< UpdateManager, PanGesture* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveGesture, gesture );
}

#ifdef DALI_DYNAMICS_SUPPORT

// Dynamics messages
inline void InitializeDynamicsWorldMessage( UpdateManager& manager, DynamicsWorld* dynamicsworld, Integration::DynamicsWorldSettings* worldSettings )
{
  typedef MessageValue2< UpdateManager, DynamicsWorld*, Integration::DynamicsWorldSettings* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::InitializeDynamicsWorld, dynamicsworld, worldSettings );
}

inline void TerminateDynamicsWorldMessage(UpdateManager& manager)
{
  typedef Message< UpdateManager > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::TerminateDynamicsWorld );
}

#endif // DALI_DYNAMICS_SUPPORT


template< typename T >
inline void AddMessage( UpdateManager& manager, ObjectOwnerContainer<T>& owner, T& object )
{
  typedef MessageValue1< ObjectOwnerContainer<T>, OwnerPointer< T > > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );
  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &owner, &ObjectOwnerContainer<T>::Add, &object );
}

template< typename T >
inline void RemoveMessage( UpdateManager& manager, ObjectOwnerContainer<T>& owner, T& object )
{
  typedef MessageValue1< ObjectOwnerContainer<T>, OwnerPointer< T > > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );
  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &owner, &ObjectOwnerContainer<T>::Remove, &object );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_UPDATE_MANAGER_H__
