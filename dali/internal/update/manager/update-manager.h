#ifndef __DALI_INTERNAL_SCENE_GRAPH_UPDATE_MANAGER_H__
#define __DALI_INTERNAL_SCENE_GRAPH_UPDATE_MANAGER_H__

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
 *
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/common/dali-common.h>

#include <dali/integration-api/resource-declarations.h>

#include <dali/internal/common/message.h>
#include <dali/internal/common/type-abstraction-enums.h>
#include <dali/internal/common/shader-saver.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/animation/scene-graph-animation.h>
#include <dali/internal/update/common/scene-graph-buffers.h>
#include <dali/internal/update/common/scene-graph-property-notification.h>
#include <dali/internal/update/manager/object-owner-container.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>
#include <dali/internal/render/renderers/render-property-buffer.h>
#include <dali/internal/event/rendering/texture-impl.h>

namespace Dali
{

namespace Integration
{
class GlSyncAbstraction;
class RenderController;

} // namespace Integration

namespace Internal
{

class PropertyNotifier;
class NotificationManager;
class CompleteNotificationInterface;
class ResourceManager;
class TouchResampler;

namespace Render
{
struct Sampler;
class FrameBuffer;
}
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
class TextureCacheDispatcher;
class PropertyBuffer;
class TextureSet;
class Camera;

/**
 * UpdateManager maintains a scene graph i.e. a tree of nodes as well as
 * other scene graph property owning objects.
 * It controls the Update traversal, in which nodes are repositioned/animated,
 * and organizes the the culling and rendering of the scene.
 * It also maintains the lifecycle of nodes and other property owners that are
 * disconnected from the scene graph.
 */
class UpdateManager : public ShaderSaver
{
public:

  /**
   * Construct a new UpdateManager.
   * @param[in] notificationManager This should be notified when animations have finished.
   * @param[in] animationFinishedNotifier The CompleteNotificationInterface that handles animation completions
   * @param[in] propertyNotifier The PropertyNotifier
   * @param[in] resourceManager The resource manager used to load textures etc.
   * @param[in] discardQueue Nodes are added here when disconnected from the scene-graph.
   * @param[in] controller After messages are flushed, we request a render from the RenderController.
   * @param[in] renderManager This is responsible for rendering the results of each "update".
   * @param[in] renderQueue Used to queue messages for the next render.
   * @param[in] textureCacheDispatcher Used for sending messages to texture cache.
   * @param[in] geometryBatcher Used when geometry batching is enabled.
   */
  UpdateManager( NotificationManager& notificationManager,
                 CompleteNotificationInterface& animationFinishedNotifier,
                 PropertyNotifier& propertyNotifier,
                 ResourceManager& resourceManager,
                 DiscardQueue& discardQueue,
                 Integration::RenderController& controller,
                 RenderManager& renderManager,
                 RenderQueue& renderQueue,
                 TextureCacheDispatcher& textureCacheDispatcher,
                 GeometryBatcher& geometryBatcher );

  /**
   * Destructor.
   */
  virtual ~UpdateManager();

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
   * @param[in] camera to add
   */
  void AddCamera( Camera* camera );

  /**
   * Remove a camera from scene
   * @param[in] camera to remove
   */
  void RemoveCamera( const Camera* camera );

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
   * @brief Get the renderer owner
   *
   * @return The renderer owner
   */
  ObjectOwnerContainer< Renderer >& GetRendererOwner();

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
   * Add a newly created TextureSet.
   * @param[in] textureSet The texture set to add.
   * @post The TextureSet is owned by the UpdateManager.
   */
  void AddTextureSet(TextureSet* textureSet);

  /**
   * Remove a TextureSet.
   * @pre The TextureSet has been added to the UpdateManager.
   * @param[in] textureSet The TextureSet to remove.
   * @post The TextureSet is destroyed.
   */
  void RemoveTextureSet(TextureSet* textureSet);

  /**
   * Set the shader program for a Shader object
   * @param[in] shader        The shader to modify
   * @param[in] shaderData    Source code, hash over source, and optional compiled binary for the shader program
   * @param[in] modifiesGeometry True if the vertex shader modifies geometry
   */
  void SetShaderProgram( Shader* shader, Internal::ShaderDataPtr shaderData, bool modifiesGeometry );

  /**
   * @brief Accept compiled shaders passed back on render thread for saving.
   * @param[in] shaderData Source code, hash over source, and corresponding compiled binary to be saved.
   */
  virtual void SaveBinary( Internal::ShaderDataPtr shaderData );

  /**
   * @brief Set the destination for compiled shader binaries to be passed on to.
   * The dispatcher passed in will be called from the update thread.
   * @param[in] upstream A sink for ShaderDatas to be passed into.
   */
  void SetShaderSaver( ShaderSaver& upstream );

  // Gestures

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

  /**
   * Add a new sampler to RenderManager
   * @param[in] sampler The sampler to add
   * @post Sends a message to RenderManager to add the sampler.
   * The sampler will be owned by RenderManager
   */
  void AddSampler( Render::Sampler* sampler );

  /**
   * Removes an existing sampler from RenderManager
   * @param[in] sampler The sampler to remove
   * @post The sampler will be destroyed in the render thread
   */
  void RemoveSampler( Render::Sampler* sampler );

  /**
   * Sets the filter modes for an existing sampler
   * @param[in] sampler The sampler
   * @param[in] minFilterMode The filter to use under minification
   * @param[in] magFilterMode The filter to use under magnification
   */
  void SetFilterMode( Render::Sampler* sampler, unsigned int minFilterMode, unsigned int magFilterMode );

  /**
   * Sets the wrap mode for an existing sampler
   * @param[in] sampler The sampler
   * @param[in] rWrapMode Wrapping mode in z direction
   * @param[in] sWrapMode Wrapping mode in x direction
   * @param[in] tWrapMode Wrapping mode in y direction
   */
  void SetWrapMode( Render::Sampler* sampler, unsigned int rWrapMode, unsigned int sWrapMode, unsigned int tWrapMode );

  /**
   * Add a new property buffer to RenderManager
   * @param[in] propertryBuffer The property buffer to add
   * @post Sends a message to RenderManager to add the property buffer.
   * The property buffer will be owned by RenderManager
   */
  void AddPropertyBuffer( Render::PropertyBuffer* propertryBuffer );

  /**
   * Removes an existing PropertyBuffer from RenderManager
   * @param[in] propertryBuffer The property buffer to remove
   * @post The property buffer will be destroyed in the render thread
   */
  void RemovePropertyBuffer( Render::PropertyBuffer* propertryBuffer );

  /**
   * Sets the format of an existing property buffer
   * @param[in] propertyBuffer The property buffer.
   * @param[in] format The new format of the buffer
   * @post Sends a message to RenderManager to set the new format to the property buffer.
   */
  void SetPropertyBufferFormat(Render::PropertyBuffer* propertyBuffer, Render::PropertyBuffer::Format* format );

  /**
   * Sets the data of an existing property buffer
   * @param[in] propertyBuffer The property buffer.
   * @param[in] data The new data of the buffer
   * @param[in] size The new size of the buffer
   * @post Sends a message to RenderManager to set the new data to the property buffer.
   */
  void SetPropertyBufferData(Render::PropertyBuffer* propertyBuffer, Dali::Vector<char>* data, size_t size);

  /**
   * Adds a geometry to the RenderManager
   * @param[in] geometry The geometry to add
   * @post Sends a message to RenderManager to add the Geometry
   * The geometry will be owned by RenderManager
   */
  void AddGeometry( Render::Geometry* geometry );

  /**
   * Removes an existing Geometry from RenderManager
   * @param[in] geometry The geometry to remove
   * @post The geometry will be destroyed in the render thread
   */
  void RemoveGeometry( Render::Geometry* geometry );

  /**
   * Sets the geometry type of an existing Geometry
   * @param[in] geometry The geometry
   * @param[in] geometryType The type of the geometry
   */
  void SetGeometryType( Render::Geometry* geometry, unsigned int geometryType );

  /**
   * Sets the index buffer to be used by a geometry
   * @param[in] geometry The geometry
   * @param[in] indices A vector containing the indices for the geometry
   */
  void SetIndexBuffer( Render::Geometry* geometry, Dali::Vector<unsigned short>& indices );

  /**
   * Adds a vertex buffer to a geomtry
   * @param[in] geometry The geometry
   * @param[in] propertyBuffer The property buffer
   */
  void AddVertexBuffer( Render::Geometry* geometry, Render::PropertyBuffer* propertyBuffer );

  /**
   * Removes a vertex buffer from a geometry
   * @param[in] geometry The geometry
   * @param[in] propertyBuffer The property buffer
   */
  void RemoveVertexBuffer( Render::Geometry* geometry, Render::PropertyBuffer* propertyBuffer );

  /**
   * Adds a texture to the render manager
   * @param[in] texture The texture to add
   * The texture will be owned by RenderManager
   */
  void AddTexture( Render::NewTexture* texture );

  /**
   * Removes a texture from the render manager
   * @param[in] texture The texture to remove
   * @post The texture will be destroyed in the render thread
   */
  void RemoveTexture( Render::NewTexture* texture );

  /**
   * Uploads data to a texture owned by the RenderManager
   * @param[in] texture The texture
   * @param[in] pixelData The pixel data object
   * @param[in] params The parameters for the upload
   */
  void UploadTexture( Render::NewTexture* texture, PixelDataPtr pixelData, const NewTexture::UploadParams& params );

  /**
   * Generates mipmaps for a texture owned by the RenderManager
   * @param[in] texture The texture
   */
  void GenerateMipmaps( Render::NewTexture* texture );

  /**
   * Adds a framebuffer to the render manager
   * @param[in] frameBuffer The framebuffer to add
   * The framebuffer will be owned by RenderManager
   */
  void AddFrameBuffer( Render::FrameBuffer* frameBuffer );

  /**
   * Removes a FrameBuffer from the render manager
   * @param[in] frameBuffer The FrameBuffer to remove
   * @post The FrameBuffer will be destroyed in the render thread
   */
  void RemoveFrameBuffer( Render::FrameBuffer* frameBuffer );

  /**
   * Attach a texture as color output to an existing FrameBuffer
   * @param[in] frameBuffer The FrameBuffer
   * @param[in] texture The texture that will be used as output when rendering
   * @param[in] mipmapLevel The mipmap of the texture to be attached
   * @param[in] layer Indicates which layer of a cube map or array texture to attach. Unused for 2D textures
   */
  void AttachColorTextureToFrameBuffer( Render::FrameBuffer* frameBuffer, Render::NewTexture* texture, unsigned int mipmapLevel, unsigned int face );

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
   * Post process resources that have been updated by renderer
   */
  void PostProcessResources();

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
  bool ProcessGestures( BufferIndex bufferIndex, unsigned int lastVSyncTimeMilliseconds, unsigned int nextVSyncTimeMilliseconds );

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
   * Prepare textures for rendering
   */
  void PrepareTextureSets( BufferIndex bufferIndex );

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

inline void ConnectNodeMessage( UpdateManager& manager, const Node& constParent, const Node& constChild )
{
  // Update thread can edit the object
  Node& parent = const_cast< Node& >( constParent );
  Node& child = const_cast< Node& >( constChild );

  typedef MessageValue2< UpdateManager, Node*, Node* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::ConnectNode, &parent, &child );
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

inline void AddCameraMessage( UpdateManager& manager, const Camera* constCamera )
{
  typedef MessageValue1< UpdateManager, Camera* > LocalType;

  Camera* camera = const_cast<Camera*>( constCamera );
  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddCamera, camera );
}

inline void RemoveCameraMessage( UpdateManager& manager, const Camera* camera )
{
  typedef MessageValue1< UpdateManager, const Camera* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveCamera, camera );
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
                                     Internal::ShaderDataPtr shaderData,
                                     bool modifiesGeometry )
{
  typedef MessageValue3< UpdateManager, Shader*, Internal::ShaderDataPtr, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetShaderProgram, &shader, shaderData, modifiesGeometry );
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
  typedef MessageValue1< ObjectOwnerContainer<T>, T* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );
  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &owner, &ObjectOwnerContainer<T>::Remove, &object );
}

// The render thread can safely change the Shader
inline void AddTextureSetMessage( UpdateManager& manager, TextureSet& textureSet )
{
  typedef MessageValue1< UpdateManager, OwnerPointer< TextureSet > > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddTextureSet, &textureSet );
}

// The render thread can safely change the Shader
inline void RemoveTextureSetMessage( UpdateManager& manager, TextureSet& textureSet )
{
  typedef MessageValue1< UpdateManager, TextureSet* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveTextureSet, &textureSet );
}

inline void AddSamplerMessage( UpdateManager& manager, Render::Sampler& sampler )
{
  typedef MessageValue1< UpdateManager, Render::Sampler* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddSampler, &sampler );
}

inline void RemoveSamplerMessage( UpdateManager& manager, Render::Sampler& sampler )
{
  typedef MessageValue1< UpdateManager, Render::Sampler* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveSampler, &sampler );
}

inline void SetFilterModeMessage( UpdateManager& manager, Render::Sampler& sampler, unsigned int minFilterMode, unsigned int magFilterMode )
{
  typedef MessageValue3< UpdateManager, Render::Sampler*, unsigned int, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetFilterMode, &sampler, minFilterMode, magFilterMode );
}

inline void SetWrapModeMessage( UpdateManager& manager, Render::Sampler& sampler, unsigned int rWrapMode, unsigned int sWrapMode, unsigned int tWrapMode )
{
  typedef MessageValue4< UpdateManager, Render::Sampler*, unsigned int, unsigned int, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetWrapMode, &sampler, rWrapMode, sWrapMode, tWrapMode );
}

inline void AddPropertyBuffer( UpdateManager& manager, Render::PropertyBuffer& propertyBuffer )
{
  typedef MessageValue1< UpdateManager, Render::PropertyBuffer*  > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddPropertyBuffer, &propertyBuffer );
}

inline void RemovePropertyBuffer( UpdateManager& manager, Render::PropertyBuffer& propertyBuffer )
{
  typedef MessageValue1< UpdateManager, Render::PropertyBuffer*  > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemovePropertyBuffer, &propertyBuffer );
}

inline void SetPropertyBufferFormat( UpdateManager& manager, Render::PropertyBuffer& propertyBuffer, Render::PropertyBuffer::Format* format )
{
  typedef MessageValue2< UpdateManager, Render::PropertyBuffer*, Render::PropertyBuffer::Format*  > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetPropertyBufferFormat, &propertyBuffer, format );
}

inline void SetPropertyBufferData( UpdateManager& manager, Render::PropertyBuffer& propertyBuffer, Vector<char>* data, size_t size )
{
  typedef MessageValue3< UpdateManager, Render::PropertyBuffer*, Vector<char>*, size_t  > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetPropertyBufferData, &propertyBuffer, data, size );
}

inline void AddGeometry( UpdateManager& manager, Render::Geometry& geometry )
{
  typedef MessageValue1< UpdateManager, Render::Geometry*  > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddGeometry, &geometry );
}

inline void RemoveGeometry( UpdateManager& manager, Render::Geometry& geometry )
{
  typedef MessageValue1< UpdateManager, Render::Geometry*  > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveGeometry, &geometry );
}

inline void AddVertexBufferMessage( UpdateManager& manager, Render::Geometry& geometry, const Render::PropertyBuffer& vertexBuffer )
{
  typedef MessageValue2< UpdateManager, Render::Geometry*, Render::PropertyBuffer* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddVertexBuffer, &geometry, const_cast<Render::PropertyBuffer*>(&vertexBuffer) );
}

inline void RemoveVertexBufferMessage( UpdateManager& manager, Render::Geometry& geometry, const Render::PropertyBuffer& vertexBuffer )
{
  typedef MessageValue2< UpdateManager, Render::Geometry*, Render::PropertyBuffer* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveVertexBuffer, &geometry, const_cast<Render::PropertyBuffer*>(&vertexBuffer) );
}

// Custom message type for SetIndexBuffer() used to move data with Vector::Swap()
template< typename T >
class IndexBufferMessage : public MessageBase
{
public:

  /**
   * Constructor which does a Vector::Swap()
   */
  IndexBufferMessage( T* manager, Render::Geometry* geometry, Dali::Vector<unsigned short>& indices )
  : MessageBase(),
    mManager( manager ),
    mRenderGeometry( geometry )
  {
    mIndices.Swap( indices );
  }

  /**
   * Virtual destructor
   */
  virtual ~IndexBufferMessage()
  {
  }

  /**
   * @copydoc MessageBase::Process
   */
  virtual void Process( BufferIndex /*bufferIndex*/ )
  {
    DALI_ASSERT_DEBUG( mManager && "Message does not have an object" );
    mManager->SetIndexBuffer( mRenderGeometry, mIndices );
  }

private:

  T* mManager;
  Render::Geometry* mRenderGeometry;
  Dali::Vector<unsigned short> mIndices;
};

inline void SetIndexBufferMessage( UpdateManager& manager, Render::Geometry& geometry, Dali::Vector<unsigned short>& indices )
{
  typedef IndexBufferMessage< UpdateManager > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &geometry, indices );
}

inline void SetGeometryTypeMessage( UpdateManager& manager, Render::Geometry& geometry, unsigned int geometryType )
{
  typedef MessageValue2< UpdateManager, Render::Geometry*, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetGeometryType, &geometry, geometryType );
}

inline void AddTexture( UpdateManager& manager, Render::NewTexture& texture )
{
  typedef MessageValue1< UpdateManager, Render::NewTexture*  > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddTexture, &texture );
}

inline void RemoveTexture( UpdateManager& manager, Render::NewTexture& texture )
{
  typedef MessageValue1< UpdateManager, Render::NewTexture*  > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveTexture, &texture );
}

inline void UploadTextureMessage( UpdateManager& manager, Render::NewTexture& texture, PixelDataPtr pixelData, const NewTexture::UploadParams& params )
{
  typedef MessageValue3< UpdateManager, Render::NewTexture*, PixelDataPtr, NewTexture::UploadParams > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::UploadTexture, &texture, pixelData, params );
}

inline void GenerateMipmapsMessage( UpdateManager& manager, Render::NewTexture& texture )
{
  typedef MessageValue1< UpdateManager, Render::NewTexture*  > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::GenerateMipmaps, &texture );
}


inline void AddFrameBuffer( UpdateManager& manager, Render::FrameBuffer& frameBuffer )
{
  typedef MessageValue1< UpdateManager, Render::FrameBuffer*  > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddFrameBuffer, &frameBuffer );
}

inline void RemoveFrameBuffer( UpdateManager& manager, Render::FrameBuffer& frameBuffer )
{
  typedef MessageValue1< UpdateManager, Render::FrameBuffer*  > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveFrameBuffer, &frameBuffer );
}

inline void AttachColorTextureToFrameBuffer( UpdateManager& manager, Render::FrameBuffer& frameBuffer, Render::NewTexture* texture, unsigned int mipmapLevel, unsigned int layer )
{
  typedef MessageValue4< UpdateManager, Render::FrameBuffer*, Render::NewTexture*, unsigned int, unsigned int  > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AttachColorTextureToFrameBuffer, &frameBuffer, texture, mipmapLevel, layer );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_UPDATE_MANAGER_H__
