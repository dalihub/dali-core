#ifndef DALI_INTERNAL_SCENE_GRAPH_UPDATE_MANAGER_H
#define DALI_INTERNAL_SCENE_GRAPH_UPDATE_MANAGER_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

#include <dali/devel-api/common/stage-devel.h>

#include <dali/internal/common/message.h>
#include <dali/internal/common/type-abstraction-enums.h>
#include <dali/internal/common/shader-saver.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/animation/scene-graph-animation.h>
#include <dali/internal/update/common/property-resetter.h>
#include <dali/internal/update/common/scene-graph-buffers.h>
#include <dali/internal/update/common/scene-graph-property-notification.h>
#include <dali/internal/update/common/scene-graph-scene.h>
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/update/manager/scene-graph-frame-callback.h> // for OwnerPointer< FrameCallback >
#include <dali/internal/update/rendering/scene-graph-renderer.h>  // for OwnerPointer< Renderer >
#include <dali/internal/update/rendering/scene-graph-texture-set.h> // for OwnerPointer< TextureSet >
#include <dali/internal/update/gestures/scene-graph-pan-gesture.h>
#include <dali/internal/update/render-tasks/scene-graph-camera.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task-list.h>
#include <dali/internal/render/shaders/scene-graph-shader.h>   // for OwnerPointer< Shader >
#include <dali/internal/render/renderers/render-vertex-buffer.h>
#include <dali/internal/event/rendering/texture-impl.h>

namespace Dali
{

class FrameCallbackInterface;

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
class RenderManager;
class RenderTaskList;
class RenderTaskProcessor;
class RenderQueue;
class VertexBuffer;

struct NodeDepthPair
{
  SceneGraph::Node* node;
  uint32_t sortedDepth;
  NodeDepthPair( SceneGraph::Node* node, uint32_t sortedDepth )
  : node(node),
    sortedDepth(sortedDepth)
  {
  }
};

struct NodeDepths
{
  NodeDepths() = default;

  void Add( SceneGraph::Node* node, uint32_t sortedDepth )
  {
    nodeDepths.push_back( NodeDepthPair( node, sortedDepth ) );
  }

  std::vector<NodeDepthPair> nodeDepths;
};


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
   * @param[in] animationPlaylist The CompleteNotificationInterface that handles the animations
   * @param[in] propertyNotifier The PropertyNotifier
   * @param[in] discardQueue Nodes are added here when disconnected from the scene-graph.
   * @param[in] controller After messages are flushed, we request a render from the RenderController.
   * @param[in] renderManager This is responsible for rendering the results of each "update".
   * @param[in] renderQueue Used to queue messages for the next render.
   * @param[in] renderTaskProcessor Handles RenderTasks and RenderInstrucitons.
   */
  UpdateManager( NotificationManager& notificationManager,
                 CompleteNotificationInterface& animationPlaylist,
                 PropertyNotifier& propertyNotifier,
                 DiscardQueue& discardQueue,
                 Integration::RenderController& controller,
                 RenderManager& renderManager,
                 RenderQueue& renderQueue,
                 RenderTaskProcessor& renderTaskProcessor );

  /**
   * Destructor.
   */
  ~UpdateManager() override;

  // Node connection methods

  /**
   * Installs a new layer as the root node.
   * @pre The layer is of derived Node type Layer.
   * @pre The layer does not have a parent.
   * @param[in] layer The new root node.
   * @post The node is owned by UpdateManager.
   */
  void InstallRoot( OwnerPointer<Layer>& layer );

  /**
   * Uninstalls the root node.
   * @pre The layer is of derived Node type Layer.
   * @pre The layer does not have a parent.
   * @param[in] layer The root node.
   * @post The node is owned by UpdateManager.
   */
  void UninstallRoot( Layer* layer );

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

  /**
   * Add a newly created scene.
   * @param[in] scene The scene to add.
   * @post The scene is owned by UpdateManager.
   */
  void AddScene( OwnerPointer<Scene>& scene );

  /**
   * Remove a scene.
   * @param[in] scene The scene to remove.
   */
  void RemoveScene( Scene* scene );

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
  void SaveBinary( Internal::ShaderDataPtr shaderData ) override;

  /**
   * @brief Set the destination for compiled shader binaries to be passed on to.
   * The dispatcher passed in will be called from the update thread.
   * @param[in] upstream A sink for ShaderDatas to be passed into.
   */
  void SetShaderSaver( ShaderSaver& upstream );

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
   * Add a new sampler to RenderManager
   * @param[in] sampler The sampler to add
   * @post Sends a message to RenderManager to add the sampler.
   * The sampler will be owned by RenderManager
   */
  void AddSampler( OwnerPointer< Render::Sampler >& sampler );

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
  void SetFilterMode( Render::Sampler* sampler, uint32_t minFilterMode, uint32_t magFilterMode );

  /**
   * Sets the wrap mode for an existing sampler
   * @param[in] sampler The sampler
   * @param[in] rWrapMode Wrapping mode in z direction
   * @param[in] sWrapMode Wrapping mode in x direction
   * @param[in] tWrapMode Wrapping mode in y direction
   */
  void SetWrapMode( Render::Sampler* sampler, uint32_t rWrapMode, uint32_t sWrapMode, uint32_t tWrapMode );

  /**
   * Add a new property buffer to RenderManager
   * @param[in] propertryBuffer The property buffer to add
   * @post Sends a message to RenderManager to add the property buffer.
   * The property buffer will be owned by RenderManager
   */
  void AddVertexBuffer( OwnerPointer< Render::VertexBuffer >& propertryBuffer );

  /**
   * Removes an existing VertexBuffer from RenderManager
   * @param[in] propertryBuffer The property buffer to remove
   * @post The property buffer will be destroyed in the render thread
   */
  void RemoveVertexBuffer( Render::VertexBuffer* propertryBuffer );

  /**
   * Sets the format of an existing property buffer
   * @param[in] vertexBuffer The property buffer.
   * @param[in] format The new format of the buffer
   * @post Sends a message to RenderManager to set the new format to the property buffer.
   */
  void SetVertexBufferFormat( Render::VertexBuffer* vertexBuffer, OwnerPointer< Render::VertexBuffer::Format>& format );

  /**
   * Sets the data of an existing property buffer
   * @param[in] vertexBuffer The property buffer.
   * @param[in] data The new data of the buffer
   * @param[in] size The new size of the buffer
   * @post Sends a message to RenderManager to set the new data to the property buffer.
   */
  void SetVertexBufferData( Render::VertexBuffer* vertexBuffer, OwnerPointer< Vector<uint8_t> >& data, uint32_t size );

  /**
   * Adds a geometry to the RenderManager
   * @param[in] geometry The geometry to add
   * @post Sends a message to RenderManager to add the Geometry
   * The geometry will be owned by RenderManager
   */
  void AddGeometry( OwnerPointer< Render::Geometry >& geometry );

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
  void SetGeometryType( Render::Geometry* geometry, uint32_t geometryType );

  /**
   * Sets the index buffer to be used by a geometry
   * @param[in] geometry The geometry
   * @param[in] indices A vector containing the indices for the geometry
   */
  void SetIndexBuffer( Render::Geometry* geometry, Dali::Vector<uint16_t>& indices );

  /**
   * Adds a vertex buffer to a geometry
   * @param[in] geometry The geometry
   * @param[in] vertexBuffer The property buffer
   */
  void AttachVertexBuffer( Render::Geometry* geometry, Render::VertexBuffer* vertexBuffer );

  /**
   * Removes a vertex buffer from a geometry
   * @param[in] geometry The geometry
   * @param[in] vertexBuffer The property buffer
   */
  void RemoveVertexBuffer( Render::Geometry* geometry, Render::VertexBuffer* vertexBuffer );

  /**
   * Adds a texture to the render manager
   * @param[in] texture The texture to add
   * The texture will be owned by RenderManager
   */
  void AddTexture( OwnerPointer< Render::Texture >& texture );

  /**
   * Removes a texture from the render manager
   * @param[in] texture The texture to remove
   * @post The texture will be destroyed in the render thread
   */
  void RemoveTexture( Render::Texture* texture );

  /**
   * Uploads data to a texture owned by the RenderManager
   * @param[in] texture The texture
   * @param[in] pixelData The pixel data object
   * @param[in] params The parameters for the upload
   */
  void UploadTexture( Render::Texture* texture, PixelDataPtr pixelData, const Texture::UploadParams& params );

  /**
   * Generates mipmaps for a texture owned by the RenderManager
   * @param[in] texture The texture
   */
  void GenerateMipmaps( Render::Texture* texture );

  /**
   * Adds a framebuffer to the render manager
   * @param[in] frameBuffer The framebuffer to add
   * The framebuffer will be owned by RenderManager
   */
  void AddFrameBuffer( OwnerPointer< Render::FrameBuffer >& frameBuffer );

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
  void AttachColorTextureToFrameBuffer( Render::FrameBuffer* frameBuffer, Render::Texture* texture, uint32_t mipmapLevel, uint32_t face );

  /**
   * Attach a texture as depth output to an existing FrameBuffer
   * @param[in] frameBuffer The FrameBuffer
   * @param[in] texture The texture that will be used as output when rendering
   * @param[in] mipmapLevel The mipmap of the texture to be attached
   */
  void AttachDepthTextureToFrameBuffer( Render::FrameBuffer* frameBuffer, Render::Texture* texture, uint32_t mipmapLevel );

  /**
   * Attach a texture as depth/stencil output to an existing FrameBuffer
   * @param[in] frameBuffer The FrameBuffer
   * @param[in] texture The texture that will be used as output when rendering
   * @param[in] mipmapLevel The mipmap of the texture to be attached
   */
  void AttachDepthStencilTextureToFrameBuffer( Render::FrameBuffer* frameBuffer, Render::Texture* texture, uint32_t mipmapLevel );

  /**
   * This is called when the surface of the scene has been replaced.
   * @param[in] scene The scene.
   */
  void SurfaceReplaced( Scene* scene );

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
   * Set the default surface rect.
   * @param[in] rect The rect value representing the surface.
   */
  void SetDefaultSurfaceRect( const Rect<int>& rect );

  /**
   * @copydoc Dali::Stage::KeepRendering()
   */
  void KeepRendering( float durationSeconds );

  /**
   * @copydoc Dali::DevelStage::SetRenderingBehavior()
   */
  void SetRenderingBehavior( DevelStage::Rendering renderingBehavior );

  /**
   * Request to render the current frame
   * @note This is a temporary workaround (to be removed in the future) to request the rendering of
   *       the current frame if the color or visibility of any actor is updated. It MUST NOT be used
   *       for any other purposes.
   */
  void RequestRendering();

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
   * @return true if at least one animations is currently active or false otherwise
   */
  bool Animate( BufferIndex bufferIndex, float elapsedSeconds );

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

private:

  // needs to be direct member so that getter for event buffer can be inlined
  SceneGraphBuffers mSceneGraphBuffers;

  struct Impl;
  Impl* mImpl;

};

// Messages for UpdateManager

inline void InstallRootMessage( UpdateManager& manager, OwnerPointer<Layer>& root )
{
  // Message has ownership of Layer while in transit from event -> update
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<Layer> >;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::InstallRoot, root );
}

inline void UninstallRootMessage( UpdateManager& manager, const Layer* constRoot )
{
  // Scene graph thread can destroy this object.
  Layer* root = const_cast< Layer* >( constRoot );

  using LocalType = MessageValue1<UpdateManager, Layer*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::UninstallRoot, root );
}

inline void AddNodeMessage( UpdateManager& manager, OwnerPointer<Node>& node )
{
  // Message has ownership of Node while in transit from event -> update
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<Node> >;

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

  using LocalType = MessageValue2<UpdateManager, Node*, Node*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::ConnectNode, &parent, &child );
}

inline void DisconnectNodeMessage( UpdateManager& manager, const Node& constNode )
{
  // Scene graph thread can modify this object.
  Node& node = const_cast< Node& >( constNode );

  using LocalType = MessageValue1<UpdateManager, Node*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::DisconnectNode, &node );
}

inline void DestroyNodeMessage( UpdateManager& manager, const Node& constNode )
{
  // Scene graph thread can destroy this object.
  Node& node = const_cast< Node& >( constNode );

  using LocalType = MessageValue1<UpdateManager, Node*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::DestroyNode, &node );
}

inline void AddCameraMessage( UpdateManager& manager, OwnerPointer< Camera >& camera )
{
  // Message has ownership of Camera while in transit from event -> update
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<Camera> >;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddCamera, camera );
}

inline void RemoveCameraMessage( UpdateManager& manager, const Camera* camera )
{
  using LocalType = MessageValue1<UpdateManager, Camera*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveCamera, const_cast<Camera*>( camera ) );
}

inline void AddObjectMessage( UpdateManager& manager, OwnerPointer<PropertyOwner>& object )
{
  // Message has ownership of object while in transit from event -> update
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<PropertyOwner> >;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddObject, object );
}

inline void RemoveObjectMessage( UpdateManager& manager, const PropertyOwner* object )
{
  using LocalType = MessageValue1<UpdateManager, PropertyOwner*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveObject, const_cast<PropertyOwner*>( object ) );
}

inline void AddAnimationMessage( UpdateManager& manager, OwnerPointer< SceneGraph::Animation >& animation )
{
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<SceneGraph::Animation> >;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddAnimation, animation );
}

inline void StopAnimationMessage( UpdateManager& manager, const Animation& constAnimation )
{
  // The scene-graph thread owns this object so it can safely edit it.
  Animation& animation = const_cast< Animation& >( constAnimation );

  using LocalType = MessageValue1<UpdateManager, Animation*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::StopAnimation, &animation );
}

inline void RemoveAnimationMessage( UpdateManager& manager, const Animation& constAnimation )
{
  // The scene-graph thread owns this object so it can safely edit it.
  Animation& animation = const_cast< Animation& >( constAnimation );

  using LocalType = MessageValue1<UpdateManager, Animation*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveAnimation, &animation );
}

inline void AddRenderTaskListMessage( UpdateManager& manager, OwnerPointer< SceneGraph::RenderTaskList >& taskList )
{
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<SceneGraph::RenderTaskList> >;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddRenderTaskList, taskList );
}

inline void RemoveRenderTaskListMessage( UpdateManager& manager, const RenderTaskList& constTaskList )
{
  // The scene-graph thread owns this object so it can safely edit it.
  RenderTaskList& taskList = const_cast< RenderTaskList& >( constTaskList );

  using LocalType = MessageValue1<UpdateManager, RenderTaskList*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveRenderTaskList, &taskList );
}

inline void AddSceneMessage( UpdateManager& manager, OwnerPointer< SceneGraph::Scene >& scene )
{
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<SceneGraph::Scene> >;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddScene, scene );
}

inline void RemoveSceneMessage( UpdateManager& manager, const SceneGraph::Scene& constScene )
{
  // The scene-graph thread owns this object so it can safely edit it.
  Scene& scene = const_cast< Scene& >( constScene );

  using LocalType = MessageValue1<UpdateManager, Scene*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveScene, &scene );
}

inline void AddPropertyNotificationMessage( UpdateManager& manager, OwnerPointer< PropertyNotification >& propertyNotification )
{
  // Message has ownership of PropertyNotification while in transit from event -> update
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<PropertyNotification> >;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddPropertyNotification, propertyNotification );
}

inline void RemovePropertyNotificationMessage( UpdateManager& manager, const PropertyNotification& constPropertyNotification )
{
  // The scene-graph thread owns this object so it can safely edit it.
  PropertyNotification& propertyNotification = const_cast< PropertyNotification& >( constPropertyNotification );

  using LocalType = MessageValue1<UpdateManager, PropertyNotification*>;

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

  using LocalType = MessageValue2<UpdateManager, PropertyNotification*, PropertyNotification::NotifyMode>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::PropertyNotificationSetNotify, propertyNotification, notifyMode );
}

// The render thread can safely change the Shader
inline void AddShaderMessage( UpdateManager& manager, OwnerPointer< Shader >& shader )
{
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<Shader> >;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddShader, shader );
}

// The render thread can safely change the Shader
inline void RemoveShaderMessage( UpdateManager& manager, const Shader* shader )
{
  using LocalType = MessageValue1<UpdateManager, Shader*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveShader, const_cast<Shader*>( shader ) );
}

inline void SetShaderProgramMessage( UpdateManager& manager,
                                     const Shader& shader,
                                     Internal::ShaderDataPtr shaderData,
                                     bool modifiesGeometry )
{
  using LocalType = MessageValue3<UpdateManager, Shader*, Internal::ShaderDataPtr, bool>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetShaderProgram, const_cast<Shader*>( &shader ), shaderData, modifiesGeometry );
}

inline void SetDefaultSurfaceRectMessage( UpdateManager& manager, const Rect<int32_t>& rect  )
{
  using LocalType = MessageValue1<UpdateManager, Rect<int32_t> >;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetDefaultSurfaceRect, rect );
}

inline void SurfaceReplacedMessage( UpdateManager& manager, const SceneGraph::Scene& constScene )
{
  // The scene-graph thread owns this object so it can safely edit it.
  Scene& scene = const_cast< Scene& >( constScene );

  using LocalType = MessageValue1<UpdateManager, Scene*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SurfaceReplaced, &scene );
}

inline void KeepRenderingMessage( UpdateManager& manager, float durationSeconds )
{
  using LocalType = MessageValue1<UpdateManager, float>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::KeepRendering, durationSeconds );
}

inline void SetRenderingBehaviorMessage( UpdateManager& manager, DevelStage::Rendering renderingBehavior )
{
  using LocalType = MessageValue1<UpdateManager, DevelStage::Rendering>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetRenderingBehavior, renderingBehavior );
}

inline void RequestRenderingMessage( UpdateManager& manager )
{
  using LocalType = Message<UpdateManager>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RequestRendering );
}

/**
 * Create a message for setting the depth of a layer
 * @param[in] manager The update manager
 * @param[in] layers list of layers
 * @param[in] rootLayer The rool layer
 */
inline void SetLayerDepthsMessage( UpdateManager& manager, const std::vector< Layer* >& layers, const Layer* rootLayer )
{
  using LocalType = MessageValue2<UpdateManager, std::vector<Layer*>, const Layer*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetLayerDepths, layers, rootLayer );
}

inline void AddRendererMessage( UpdateManager& manager, OwnerPointer< Renderer >& object )
{
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<Renderer> >;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );
  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddRenderer, object );
}

inline void RemoveRendererMessage( UpdateManager& manager, const Renderer& object )
{
  using LocalType = MessageValue1<UpdateManager, Renderer*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );
  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveRenderer, const_cast<Renderer*>( &object ) );
}

// The render thread can safely change the Shader
inline void AddTextureSetMessage( UpdateManager& manager, OwnerPointer< TextureSet >& textureSet )
{
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<TextureSet> >;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddTextureSet, textureSet );
}

// The render thread can safely change the Shader
inline void RemoveTextureSetMessage( UpdateManager& manager, TextureSet& textureSet )
{
  using LocalType = MessageValue1<UpdateManager, TextureSet*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveTextureSet, &textureSet );
}

inline void AddSamplerMessage( UpdateManager& manager, OwnerPointer< Render::Sampler >& sampler )
{
  // Message has ownership of Sampler while in transit from event -> update
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<Render::Sampler> >;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddSampler, sampler );
}

inline void RemoveSamplerMessage( UpdateManager& manager, Render::Sampler& sampler )
{
  using LocalType = MessageValue1<UpdateManager, Render::Sampler*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveSampler, &sampler );
}

inline void SetFilterModeMessage( UpdateManager& manager, Render::Sampler& sampler, uint32_t minFilterMode, uint32_t magFilterMode )
{
  using LocalType = MessageValue3<UpdateManager, Render::Sampler*, uint32_t, uint32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetFilterMode, &sampler, minFilterMode, magFilterMode );
}

inline void SetWrapModeMessage( UpdateManager& manager, Render::Sampler& sampler, uint32_t rWrapMode, uint32_t sWrapMode, uint32_t tWrapMode )
{
  using LocalType = MessageValue4<UpdateManager, Render::Sampler*, uint32_t, uint32_t, uint32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetWrapMode, &sampler, rWrapMode, sWrapMode, tWrapMode );
}

inline void AddVertexBuffer( UpdateManager& manager, OwnerPointer< Render::VertexBuffer >& vertexBuffer )
{
  // Message has ownership of vertexBuffer while in transit from event -> update
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<Render::VertexBuffer> >;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddVertexBuffer, vertexBuffer );
}

inline void RemoveVertexBuffer( UpdateManager& manager, Render::VertexBuffer& vertexBuffer )
{
  using LocalType = MessageValue1<UpdateManager, Render::VertexBuffer*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveVertexBuffer, &vertexBuffer );
}

inline void SetVertexBufferFormat( UpdateManager& manager, Render::VertexBuffer& vertexBuffer, OwnerPointer< Render::VertexBuffer::Format>& format )
{
  // Message has ownership of VertexBuffer::Format while in transit from event -> update
  using LocalType = MessageValue2<UpdateManager, Render::VertexBuffer*, OwnerPointer<Render::VertexBuffer::Format> >;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetVertexBufferFormat, &vertexBuffer, format );
}

inline void SetVertexBufferData( UpdateManager& manager, Render::VertexBuffer& vertexBuffer, OwnerPointer< Vector<uint8_t> >& data, uint32_t size )
{
  // Message has ownership of VertexBuffer data while in transit from event -> update
  using LocalType = MessageValue3<UpdateManager, Render::VertexBuffer*, OwnerPointer<Vector<uint8_t> >, uint32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetVertexBufferData, &vertexBuffer, data, size );
}

inline void AddGeometry( UpdateManager& manager, OwnerPointer< Render::Geometry >& geometry )
{
  // Message has ownership of Geometry while in transit from event -> update
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<Render::Geometry> >;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddGeometry, geometry );
}

inline void RemoveGeometry( UpdateManager& manager, Render::Geometry& geometry )
{
  using LocalType = MessageValue1<UpdateManager, Render::Geometry*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveGeometry, &geometry );
}

inline void AttachVertexBufferMessage( UpdateManager& manager, Render::Geometry& geometry, const Render::VertexBuffer& vertexBuffer )
{
  using LocalType = MessageValue2<UpdateManager, Render::Geometry*, Render::VertexBuffer*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AttachVertexBuffer, &geometry, const_cast<Render::VertexBuffer*>(&vertexBuffer) );
}

inline void RemoveVertexBufferMessage( UpdateManager& manager, Render::Geometry& geometry, const Render::VertexBuffer& vertexBuffer )
{
  using LocalType = MessageValue2<UpdateManager, Render::Geometry*, Render::VertexBuffer*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveVertexBuffer, &geometry, const_cast<Render::VertexBuffer*>(&vertexBuffer) );
}

// Custom message type for SetIndexBuffer() used to move data with Vector::Swap()
template< typename T >
class IndexBufferMessage : public MessageBase
{
public:

  /**
   * Constructor which does a Vector::Swap()
   */
  IndexBufferMessage( T* manager, Render::Geometry* geometry, Dali::Vector<uint16_t>& indices )
  : MessageBase(),
    mManager( manager ),
    mRenderGeometry( geometry )
  {
    mIndices.Swap( indices );
  }

  /**
   * Virtual destructor
   */
  ~IndexBufferMessage() override = default;

  /**
   * @copydoc MessageBase::Process
   */
  void Process( BufferIndex /*bufferIndex*/ ) override
  {
    DALI_ASSERT_DEBUG( mManager && "Message does not have an object" );
    mManager->SetIndexBuffer( mRenderGeometry, mIndices );
  }

private:

  T* mManager;
  Render::Geometry* mRenderGeometry;
  Dali::Vector<uint16_t> mIndices;
};

inline void SetIndexBufferMessage( UpdateManager& manager, Render::Geometry& geometry, Dali::Vector<uint16_t>& indices )
{
  using LocalType = IndexBufferMessage<UpdateManager>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &geometry, indices );
}

inline void SetGeometryTypeMessage( UpdateManager& manager, Render::Geometry& geometry, uint32_t geometryType )
{
  using LocalType = MessageValue2<UpdateManager, Render::Geometry*, uint32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetGeometryType, &geometry, geometryType );
}

inline void AddTexture( UpdateManager& manager, OwnerPointer< Render::Texture >& texture )
{
  // Message has ownership of Texture while in transit from event -> update
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<Render::Texture> >;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddTexture, texture );
}

inline void RemoveTexture( UpdateManager& manager, Render::Texture& texture )
{
  using LocalType = MessageValue1<UpdateManager, Render::Texture*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveTexture, &texture );
}

inline void UploadTextureMessage( UpdateManager& manager, Render::Texture& texture, PixelDataPtr pixelData, const Texture::UploadParams& params )
{
  using LocalType = MessageValue3<UpdateManager, Render::Texture*, PixelDataPtr, Texture::UploadParams>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::UploadTexture, &texture, pixelData, params );
}

inline void GenerateMipmapsMessage( UpdateManager& manager, Render::Texture& texture )
{
  using LocalType = MessageValue1<UpdateManager, Render::Texture*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::GenerateMipmaps, &texture );
}


inline void AddFrameBuffer( UpdateManager& manager, OwnerPointer< Render::FrameBuffer >& frameBuffer )
{
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<Render::FrameBuffer> >;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddFrameBuffer, frameBuffer );
}

inline void RemoveFrameBuffer( UpdateManager& manager, Render::FrameBuffer& frameBuffer )
{
  using LocalType = MessageValue1<UpdateManager, Render::FrameBuffer*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveFrameBuffer, &frameBuffer );
}

inline void AttachColorTextureToFrameBuffer( UpdateManager& manager, Render::FrameBuffer& frameBuffer, Render::Texture* texture, uint32_t mipmapLevel, uint32_t layer )
{
  using LocalType = MessageValue4<UpdateManager, Render::FrameBuffer*, Render::Texture*, uint32_t, uint32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AttachColorTextureToFrameBuffer, &frameBuffer, texture, mipmapLevel, layer );
}

inline void AttachDepthTextureToFrameBuffer( UpdateManager& manager, Render::FrameBuffer& frameBuffer, Render::Texture* texture, uint32_t mipmapLevel )
{
  using LocalType = MessageValue3<UpdateManager, Render::FrameBuffer*, Render::Texture*, uint32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AttachDepthTextureToFrameBuffer, &frameBuffer, texture, mipmapLevel );
}

inline void AttachDepthStencilTextureToFrameBuffer( UpdateManager& manager, Render::FrameBuffer& frameBuffer, Render::Texture* texture, uint32_t mipmapLevel )
{
  using LocalType = MessageValue3<UpdateManager, Render::FrameBuffer*, Render::Texture*, uint32_t>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AttachDepthStencilTextureToFrameBuffer, &frameBuffer, texture, mipmapLevel );
}

inline void SetDepthIndicesMessage( UpdateManager& manager, OwnerPointer< NodeDepths >& nodeDepths )
{
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<NodeDepths> >;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::SetDepthIndices, nodeDepths );
}

inline void AddResetterMessage( UpdateManager& manager, OwnerPointer<PropertyResetterBase> resetter )
{
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<PropertyResetterBase> >;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddPropertyResetter, resetter );
}

inline void AddFrameCallbackMessage( UpdateManager& manager, OwnerPointer< FrameCallback >& frameCallback, const Node& rootNode )
{
  using LocalType = MessageValue2<UpdateManager, OwnerPointer<FrameCallback>, const Node*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::AddFrameCallback, frameCallback, &rootNode );
}

inline void RemoveFrameCallbackMessage( UpdateManager& manager, FrameCallbackInterface& frameCallback )
{
  using LocalType = MessageValue1<UpdateManager, FrameCallbackInterface*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &manager, &UpdateManager::RemoveFrameCallback, &frameCallback );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_UPDATE_MANAGER_H
