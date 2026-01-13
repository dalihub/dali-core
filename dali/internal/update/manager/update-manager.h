#ifndef DALI_INTERNAL_SCENE_GRAPH_UPDATE_MANAGER_H
#define DALI_INTERNAL_SCENE_GRAPH_UPDATE_MANAGER_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/vector-wrapper.h>

#include <dali/devel-api/common/stage-devel.h>

#include <dali/internal/common/message.h>
#include <dali/internal/common/owner-key-type.h>
#include <dali/internal/common/shader-saver.h>
#include <dali/internal/common/type-abstraction-enums.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/rendering/texture-impl.h>
#include <dali/internal/render/renderers/render-texture-key.h>   // For RenderTextureKey
#include <dali/internal/render/renderers/render-texture.h>       // For OwnerPointer<Render::Texture>
#include <dali/internal/render/renderers/render-uniform-block.h> // for OwnerPointer<Render::UniformBlock>
#include <dali/internal/render/renderers/render-vertex-buffer.h>
#include <dali/internal/render/shaders/render-shader.h> // for OwnerPointer< Shader >
#include <dali/internal/update/animation/scene-graph-animation.h>
#include <dali/internal/update/common/node-resetter.h>
#include <dali/internal/update/common/property-owner-flag-manager.h>
#include <dali/internal/update/common/property-resetter.h>
#include <dali/internal/update/common/resetter-manager.h>
#include <dali/internal/update/common/scene-graph-buffers.h>
#include <dali/internal/update/common/scene-graph-property-notification.h>
#include <dali/internal/update/common/scene-graph-scene.h>
#include <dali/internal/update/gestures/scene-graph-pan-gesture.h>
#include <dali/internal/update/manager/scene-graph-frame-callback.h> // for OwnerPointer< FrameCallback >
#include <dali/internal/update/nodes/node.h>
#include <dali/internal/update/nodes/scene-graph-layer.h>
#include <dali/internal/update/render-tasks/scene-graph-camera.h>
#include <dali/internal/update/render-tasks/scene-graph-render-task-list.h>
#include <dali/internal/update/rendering/scene-graph-renderer.h>    // for OwnerPointer< Renderer >
#include <dali/internal/update/rendering/scene-graph-texture-set.h> // for OwnerPointer< TextureSet >

// EXTERNAL INCLUDES
#include <cstddef>

namespace Dali
{
class FrameCallbackInterface;

namespace Integration
{
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
} // namespace Render
// value types used by messages
template<>
struct ParameterType<PropertyNotification::NotifyMode>
: public BasicType<PropertyNotification::NotifyMode>
{
};

namespace SceneGraph
{
class Animation;
class MemoryPoolCollection;
class RenderManager;
class RenderTaskList;
class RenderTaskProcessor;
class VertexBuffer;

/**
 * UpdateManager maintains a scene graph i.e. a tree of nodes as well as
 * other scene graph property owning objects.
 * It controls the Update traversal, in which nodes are repositioned/animated,
 * and organizes the the culling and rendering of the scene.
 * It also maintains the lifecycle of nodes and other property owners that are
 * disconnected from the scene graph.
 */
class UpdateManager : public ShaderSaver, public ResetterManager, public PropertyOwnerFlagManager
{
public:
  /**
   * Construct a new UpdateManager.
   * @param[in] notificationManager This should be notified when animations have finished.
   * @param[in] animationPlaylist The CompleteNotificationInterface that handles the animations
   * @param[in] propertyNotifier The PropertyNotifier
   * @param[in] renderController After messages are flushed, we request a render from the RenderController.
   * @param[in] renderManager This is responsible for rendering the results of each "update".
   * @param[in] renderTaskProcessor Handles RenderTasks and RenderInstrucitons.
   * @param[in] memoryPoolCollection Memory pool collection.
   */
  UpdateManager(NotificationManager&           notificationManager,
                CompleteNotificationInterface& animationPlaylist,
                PropertyNotifier&              propertyNotifier,
                Integration::RenderController& renderController,
                RenderManager&                 renderManager,
                RenderTaskProcessor&           renderTaskProcessor,
                MemoryPoolCollection&          memoryPoolCollection);

  /**
   * Destructor.
   */
  ~UpdateManager() override;

  /**
   * @brief The graphics context is being shutdown. Clean down any outstanding graphics resources.
   */
  void ContextDestroyed();

  // Node connection methods

  /**
   * Installs a new layer as the root node.
   * @pre The layer is of derived Node type Layer.
   * @pre The layer does not have a parent.
   * @param[in] layer The new root node.
   * @post The node is owned by UpdateManager.
   */
  void InstallRoot(OwnerPointer<Layer>& layer);

  /**
   * Uninstalls the root node.
   * @pre The layer is of derived Node type Layer.
   * @pre The layer does not have a parent.
   * @param[in] layer The root node.
   * @post The node is owned by UpdateManager.
   */
  void UninstallRoot(Layer* layer);

  /**
   * Add a Node; UpdateManager takes ownership.
   * @pre The node does not have a parent.
   * @note even though nodes are pool allocated, they also contain other heap allocated data, thus using OwnerPointer when transferring the data
   * @param[in] node The node to add.
   */
  void AddNode(OwnerPointer<Node>& node);

  /**
   * Connect a Node to the scene-graph.
   * A disconnected Node has has no parent or children, and its properties cannot be animated/constrained.
   * @pre The node does not already have a parent.
   * @param[in] parent The new parent node.
   */
  void ConnectNode(Node* parent, Node* node);

  /**
   * Disconnect a Node from the scene-graph.
   * A disconnected Node has has no parent or children, and its properties cannot be animated/constrained.
   * @pre The node has a parent.
   * @param[in] node The node to disconnect.
   */
  void DisconnectNode(Node* node);

  /**
   * Destroy a Node owned by UpdateManager.
   * This is not immediate; Nodes are passed to the RenderManager to allow GL resources to be deleted.
   * In the following update, the previously queued Nodes may be deleted.
   * @pre The node has been disconnected from the scene-graph i.e. has no parent or children.
   * @param[in] node The node to destroy.
   */
  void DestroyNode(Node* node);

  /**
   * Add a newly created object.
   * @param[in] object The object to add.
   * @post The object is owned by UpdateManager.
   */
  void AddObject(OwnerPointer<PropertyOwner>& object);

  /**
   * Remove an object.
   * @param[in] object The object to remove.
   */
  void RemoveObject(PropertyOwner* object);

  /**
   * Add a newly created render task list.
   * @param[in] taskList The render task list to add.
   * @post The render task list is owned by UpdateManager.
   */
  void AddRenderTaskList(OwnerPointer<RenderTaskList>& taskList);

  /**
   * Remove a render task list.
   * @param[in] taskList The render task list to remove.
   */
  void RemoveRenderTaskList(RenderTaskList* taskList);

  /**
   * Add a newly created scene.
   * @param[in] scene The scene to add.
   * @post The scene is owned by UpdateManager.
   */
  void AddScene(OwnerPointer<Scene>& scene);

  /**
   * Remove a scene.
   * @param[in] scene The scene to remove.
   */
  void RemoveScene(Scene* scene);

  // Animations

  /**
   * Add a newly created animation.
   * @param[in] animation The animation to add.
   * @post The animation is owned by UpdateManager.
   */
  void AddAnimation(OwnerPointer<SceneGraph::Animation>& animation);

  /**
   * Stop an animation.
   * @param[in] animation The animation to stop.
   */
  void StopAnimation(Animation* animation);

  /**
   * Clear an animation.
   * @param[in] animation The animation to clear.
   */
  void ClearAnimation(Animation* animation);

  /**
   * Remove an animation.
   * @param[in] animation The animation to remove.
   */
  void RemoveAnimation(Animation* animation);

  /**
   * Query whether any animations are currently running.
   * @return True if any animations are running.
   */
  bool IsAnimationRunning() const;

  // PropertyOwnerFlagManager

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyOwnerFlagManager::RequestResetUpdated()
   */
  void RequestResetUpdated(const PropertyOwner& owner) override;

  /**
   * @copydoc Dali::Internal::SceneGraph::PropertyOwnerFlagManager::DiscardPropertyOwner()
   */
  void DiscardPropertyOwner(PropertyOwner* discardedOwner) override;

  // ResetterManager

  /**
   * @copydoc Dali::Internal::SceneGraph::ResetterManager::AddPropertyResetter()
   */
  void AddPropertyResetter(OwnerPointer<PropertyResetterBase>& propertyResetter) override;

  /**
   * @copydoc Dali::Internal::SceneGraph::ResetterManager::AddNodeResetter()
   */
  void AddNodeResetter(const Node& node) override;

  /**
   * @copydoc Dali::Internal::SceneGraph::ResetterManager::RequestPropertyBaseResetToBaseValue()
   */
  void RequestPropertyBaseResetToBaseValue(PropertyBase* propertyBase) override;

  // Property Notification

  /**
   * Add a newly created property notification
   * @param[in] propertyNotification The notification to add
   * @post The propertyNotification is owned by UpdateManager.
   */
  void AddPropertyNotification(OwnerPointer<PropertyNotification>& propertyNotification);

  /**
   * Remove a property notification
   * @param[in] propertyNotification The notification to remove
   */
  void RemovePropertyNotification(PropertyNotification* propertyNotification);

  /**
   * Set Notify state for PropertyNotification
   * @param[in] propertyNotification The notification to remove
   * @param[in] notifyMode The notification mode.
   */
  void PropertyNotificationSetNotify(PropertyNotification* propertyNotification, PropertyNotification::NotifyMode notifyMode);

  // Shaders

  /**
   * Add a newly created shader.
   * @param[in] shader The shader to add.
   * @post The shader is owned by the UpdateManager.
   */
  void AddShader(OwnerPointer<Shader>& shader);

  /**
   * Remove a shader.
   * @pre The shader has been added to the UpdateManager.
   * @param[in] shader The shader to remove.
   * @post The shader is destroyed.
   */
  void RemoveShader(Shader* shader);

  /**
   * Set the shader data for a Shader object
   * @param[in] shader        The shader to modify
   * @param[in] shaderData    Source code, hash over source, and optional compiled binary for the shader program
   */
  void SetShaderData(Shader* shader, Internal::ShaderDataPtr shaderData);

  /**
   * @brief Accept compiled shaders passed back on render thread for saving.
   * @param[in] shaderData Source code, hash over source, and corresponding compiled binary to be saved.
   */
  void SaveBinary(Internal::ShaderDataPtr shaderData) override;

  /**
   * @brief Set the destination for compiled shader binaries to be passed on to.
   * The dispatcher passed in will be called from the update thread.
   * @param[in] upstream A sink for ShaderDatas to be passed into.
   */
  void SetShaderSaver(ShaderSaver& upstream);

  // Renderers

  /**
   * Add a new renderer to scene
   * @param rendererKeyPointer to add
   */
  void AddRenderer(OwnerKeyType<Renderer>& rendererKeyPointer);

  /**
   * Remove a renderer from scene
   * @param renderer to remove
   */
  void RemoveRenderer(const RendererKey& renderer);

  /**
   * Attach a renderer to node
   * @param renderer to attach
   */
  void AttachRenderer(Node* node, Renderer* renderer);

  /**
   * Attach cache renderer to node
   * @param[in] node target
   * @param[in] renderer to attach
   */
  void AttachCacheRenderer(Node* node, Renderer* renderer);

  // Gestures

  /**
   * Set the pan gesture processor.
   * Pan Gesture processor lives for the lifetime of UpdateManager
   * @param[in] gesture The gesture processor.
   * @post The gestureProcessor is owned by the UpdateManager.
   */
  void SetPanGestureProcessor(PanGesture* gestureProcessor);

  // Textures

  /**
   * Add a newly created TextureSet.
   * @param[in] textureSet The texture set to add.
   * @post The TextureSet is owned by the UpdateManager.
   */
  void AddTextureSet(OwnerPointer<TextureSet>& textureSet);

  /**
   * Remove a TextureSet.
   * @pre The TextureSet has been added to the UpdateManager.
   * @param[in] textureSet The TextureSet to remove.
   * @post The TextureSet is destroyed.
   */
  void RemoveTextureSet(TextureSet* textureSet);

  // UniformBlock

  /**
   * Add uniform block.
   * @param[in] uniformBlock The uniform block.
   */
  void AddUniformBlock(OwnerPointer<Render::UniformBlock>& uniformBlock);

  /**
   * Remove uniform block.
   * @param[in] uniformBlock The uniform block.
   */
  void RemoveUniformBlock(Render::UniformBlock* uniformBlock);

  // Render tasks

  /**
   * Get the scene graph side list of RenderTasks.
   * @param[in] systemLevel True if using the system-level overlay.
   * @return The list of render tasks
   */
  RenderTaskList* GetRenderTaskList(bool systemLevel);

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
  uint32_t* ReserveMessageSlot(uint32_t size, bool updateScene = true);

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
   * Called by the event-thread to signal that we need to queue messages.
   */
  void EventProcessingFinished();

  /**
   * Flush the set of messages, which were previously stored with QueueMessage().
   * Calls to this thread-safe method should be minimized, to avoid thread blocking.
   *
   * @return True if there are messages to process.
   */
  bool FlushQueue();

  // API list to call RenderManager directly.

  /**
   * This is called when the surface of the scene has been replaced.
   * @param[in] scene The scene.
   */
  void SurfaceReplaced(Scene* scene);

  /**
   * Request to clear the program cache at RenderManager.
   */
  void RequestClearProgramCache();

  // Sampler

  /**
   * Add a new sampler to RenderManager
   * @param[in] sampler The sampler to add
   * @post Sends a message to RenderManager to add the sampler.
   * The sampler will be owned by RenderManager
   */
  void AddSampler(OwnerPointer<Render::Sampler>& sampler);

  /**
   * Removes an existing sampler from RenderManager
   * @param[in] sampler The sampler to remove
   * @post The sampler will be destroyed in the render thread
   */
  void RemoveSampler(Render::Sampler* sampler);

  // Vertex Buffer

  /**
   * Add a new property buffer to RenderManager
   * @param[in] propertryBuffer The property buffer to add
   * @post Sends a message to RenderManager to add the property buffer.
   * The property buffer will be owned by RenderManager
   */
  void AddVertexBuffer(OwnerPointer<Render::VertexBuffer>& propertryBuffer);

  /**
   * Removes an existing VertexBuffer from RenderManager
   * @param[in] propertryBuffer The property buffer to remove
   * @post The property buffer will be destroyed in the render thread
   */
  void RemoveVertexBuffer(Render::VertexBuffer* propertryBuffer);

  // Geometry

  /**
   * Adds a geometry to the RenderManager
   * @param[in] geometry The geometry to add
   * @post Sends a message to RenderManager to add the Geometry
   * The geometry will be owned by RenderManager
   */
  void AddGeometry(OwnerPointer<Render::Geometry>& geometry);

  /**
   * Removes an existing Geometry from RenderManager
   * @param[in] geometry The geometry to remove
   * @post The geometry will be destroyed in the render thread
   */
  void RemoveGeometry(Render::Geometry* geometry);

  // Texture

  /**
   * Adds a texture to the render manager
   * @param[in] textureKeyPointer The texture to add
   * The texture will be owned by RenderManager
   */
  void AddTexture(OwnerKeyType<Render::Texture>& textureKeyPointer);

  /**
   * Removes a texture from the render manager
   * @param[in] texture The texture to remove
   * @post The texture will be destroyed in the render thread
   */
  void RemoveTexture(const Render::TextureKey& texture);

  // FrameBuffer

  /**
   * Adds a framebuffer to the render manager
   * @param[in] frameBuffer The framebuffer to add
   * The framebuffer will be owned by RenderManager
   */
  void AddFrameBuffer(OwnerPointer<Render::FrameBuffer>& frameBuffer);

  /**
   * Removes a FrameBuffer from the render manager
   * @param[in] frameBuffer The FrameBuffer to remove
   * @post The FrameBuffer will be destroyed in the render thread
   */
  void RemoveFrameBuffer(Render::FrameBuffer* frameBuffer);

public:
  /**
   * Performs an Update traversal on the scene-graph.
   * @param[in] elapsedSeconds The elapsed time that should be applied to animations.
   * @param[in] lastVSyncTimeMilliseconds The last time, in milliseconds, that we had a VSync.
   * @param[in] nextVSyncTimeMilliseconds The estimated time, in milliseconds, of the next VSync.
   * @param[in] renderToFboEnabled Whether rendering into the Frame Buffer Object is enabled.
   * @param[in] isRenderingToFbo   Whether this frame is being rendered into the Frame Buffer Object.
   * @param[in] uploadOnly uploadOnly Upload the resource only without rendering.
   * @param[out] rendererAdded Whether at least one of renderer added to update manager, or not.
   * @return True if further updates are required e.g. during animations.
   */
  uint32_t Update(float    elapsedSeconds,
                  uint32_t lastVSyncTimeMilliseconds,
                  uint32_t nextVSyncTimeMilliseconds,
                  bool     renderToFboEnabled,
                  bool     isRenderingToFbo,
                  bool     uploadOnly,
                  bool&    rendererAdded);

  /**
   * This is called after rendering all the scenes in the next frame.
   */
  void PostRender();

  /**
   * @copydoc Dali::Stage::KeepRendering()
   */
  void KeepRendering(float durationSeconds);

  /**
   * @copydoc Dali::DevelStage::SetRenderingBehavior()
   */
  void SetRenderingBehavior(DevelStage::Rendering renderingBehavior);

  /**
   * Request to render the current frame
   * @note This is a temporary workaround (to be removed in the future) to request the rendering of
   *       the current frame if the color or visibility of any actor is updated. It MUST NOT be used
   *       for any other purposes.
   */
  void RequestRendering();

  /**
   * Resets the renderer added flag.
   */
  void ResetRendererAddedFlag();

  /**
   * @brief Get the active Node pointer by node id.
   *
   * @param[in] nodeId The id of node what we want to get.
   * @return The pointer of node, or nullptr if given node id is not exist, or discarded.
   */
  Node* GetNodePointerById(uint32_t nodeId) const;

  /**
   * Sets the depths of all layers.
   * @param layers The layers in depth order.
   * @param[in] rootLayer The root layer of the sorted layers.
   */
  void SetLayerDepths(const std::vector<Layer*>& layers, const Layer* rootLayer);

  /**
   * Set the depth indices of all nodes (in LayerUI's)
   * @param[in] nodeDepths A vector of nodes and associated depth indices
   */
  void SetDepthIndices(OwnerPointer<NodeDepths>& nodeDepths);

  /**
   * Adds an implementation of the FrameCallbackInterface.
   * @param[in] frameCallback An OwnerPointer to the SceneGraph FrameCallback object
   * @param[in] rootNode A pointer to the root node to apply the FrameCallback to
   */
  void AddFrameCallback(OwnerPointer<FrameCallback>& frameCallback, const Node* rootNode);

  /**
   * Removes the specified implementation of FrameCallbackInterface.
   * @param[in] frameCallback A pointer to the implementation of the FrameCallbackInterface to remove.
   */
  void RemoveFrameCallback(FrameCallbackInterface* frameCallback);

  /**
   * Notify the specified implementation of FrameCallbackInterface with the given sync point.
   * @param[in] frameCallback A pointer to the implementation of the FrameCallbackInterface to notify.
   * @param[in] syncPoint The unique sync point to notify with.
   */
  void NotifyFrameCallback(FrameCallbackInterface* frameCallback, Dali::UpdateProxy::NotifySyncPoint syncPoint);

  /**
   * Get the update message queue capacity (mutex locked)
   */
  std::size_t GetUpdateMessageQueueCapacity() const;

  /**
   * Get the render instruction capacity
   */
  std::size_t GetRenderInstructionCapacity() const;

private:
  // Undefined
  UpdateManager(const UpdateManager&);

  // Undefined
  UpdateManager& operator=(const UpdateManager& rhs);

  /**
   * Add a camera on scene
   * @param[in] camera The camera to add
   */
  void AddCamera(Camera* camera);

  /**
   * Remove a camera from scene
   * @param[in] camera to remove
   */
  void RemoveCamera(Camera* camera);

  /**
   * Helper to check whether the update-thread should keep going.
   * @param[in] elapsedSeconds The time in seconds since the previous update.
   * @return True if the update-thread should keep going.
   */
  uint32_t KeepUpdatingCheck(float elapsedSeconds) const;

  /**
   * Helper to reset all Node properties
   * @param[in] bufferIndex to use
   */
  void ResetProperties(BufferIndex bufferIndex);

  /**
   * Perform gesture updates.
   * @param[in] bufferIndex to use
   * @param[in] lastVSyncTime  The last VSync time in milliseconds.
   * @param[in] nextVSyncTime  The estimated time of the next VSync in milliseconds.
   * @return true, if any properties were updated.
   */
  bool ProcessGestures(BufferIndex bufferIndex, uint32_t lastVSyncTimeMilliseconds, uint32_t nextVSyncTimeMilliseconds);

  /**
   * Perform animation updates
   * @param[in] bufferIndex to use
   * @param[in] elapsedSeconds time since last frame
   * @return true if at least one animations is currently active or false otherwise
   */
  bool Animate(BufferIndex bufferIndex, float elapsedSeconds);

  /**
   * Applies constraints to CustomObjects
   * @param[out] postPropertyOwner property owners those have post constraint.
   * @param[in] bufferIndex to use
   */
  void ConstrainCustomObjects(PropertyOwnerContainer& postPropertyOwners, BufferIndex bufferIndex);

  /**
   * Applies constraints to RenderTasks
   * @param[out] postPropertyOwner property owners those have post constraint.
   * @param[in] bufferIndex to use
   */
  void ConstrainRenderTasks(PropertyOwnerContainer& postPropertyOwners, BufferIndex bufferIndex);

  /**
   * Applies constraints to Shaders
   * @param[out] postPropertyOwner property owners those have post constraint.
   * @param[in] bufferIndex to use
   */
  void ConstrainShaders(PropertyOwnerContainer& postPropertyOwners, BufferIndex bufferIndex);

  /**
   * Perform property notification updates
   * @param[in] bufferIndex to use
   */
  void ProcessPropertyNotifications(BufferIndex bufferIndex);

  /**
   * Pass shader binaries queued here on to event thread.
   */
  void ForwardCompiledShadersToEventThread();

  /**
   * Update node shaders, opacity, geometry etc.
   * @param[out] postPropertyOwner property owners those have post constraint.
   * @param[in] bufferIndex to use
   */
  void UpdateNodes(PropertyOwnerContainer& postPropertyOwners, BufferIndex bufferIndex);

  /**
   * initialize layer renderables
   * @param[in] bufferIndex
   */
  void UpdateLayers(BufferIndex bufferIndex);

  /**
   * Update Renderers
   * @param[out] postPropertyOwner property owners those have post constraint.
   * @param[in] bufferIndex to use
   */
  void UpdateRenderers(PropertyOwnerContainer& postPropertyOwners, BufferIndex bufferIndex);

private:
  // needs to be direct member so that getter for event buffer can be inlined
  SceneGraphBuffers mSceneGraphBuffers;

  struct Impl;
  Impl* mImpl;
};

// Messages for UpdateManager

inline void InstallRootMessage(UpdateManager& manager, OwnerPointer<Layer>& root)
{
  // Message has ownership of Layer while in transit from event -> update
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<Layer>>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::InstallRoot, root);
}

inline void UninstallRootMessage(UpdateManager& manager, const Layer* constRoot)
{
  // Scene graph thread can destroy this object.
  Layer* root = const_cast<Layer*>(constRoot);

  using LocalType = MessageValue1<UpdateManager, Layer*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::UninstallRoot, root);
}

inline void AddNodeMessage(UpdateManager& manager, OwnerPointer<Node>& node)
{
  // Message has ownership of Node while in transit from event -> update
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<Node>>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::AddNode, node);
}

inline void ConnectNodeMessage(UpdateManager& manager, const Node& constParent, const Node& constChild)
{
  // Update thread can edit the object
  Node& parent = const_cast<Node&>(constParent);
  Node& child  = const_cast<Node&>(constChild);

  using LocalType = MessageValue2<UpdateManager, Node*, Node*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::ConnectNode, &parent, &child);
}

inline void DisconnectNodeMessage(UpdateManager& manager, const Node& constNode)
{
  // Scene graph thread can modify this object.
  Node& node = const_cast<Node&>(constNode);

  using LocalType = MessageValue1<UpdateManager, Node*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::DisconnectNode, &node);
}

inline void DestroyNodeMessage(UpdateManager& manager, const Node& constNode)
{
  // Scene graph thread can destroy this object.
  Node& node = const_cast<Node&>(constNode);

  using LocalType = MessageValue1<UpdateManager, Node*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::DestroyNode, &node);
}

inline void AddObjectMessage(UpdateManager& manager, OwnerPointer<PropertyOwner>& object)
{
  // Message has ownership of object while in transit from event -> update
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<PropertyOwner>>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::AddObject, object);
}

inline void RemoveObjectMessage(UpdateManager& manager, const PropertyOwner* object)
{
  using LocalType = MessageValue1<UpdateManager, PropertyOwner*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::RemoveObject, const_cast<PropertyOwner*>(object));
}

inline void AddAnimationMessage(UpdateManager& manager, OwnerPointer<SceneGraph::Animation>& animation)
{
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<SceneGraph::Animation>>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::AddAnimation, animation);
}

inline void StopAnimationMessage(UpdateManager& manager, const Animation& constAnimation)
{
  // The scene-graph thread owns this object so it can safely edit it.
  Animation& animation = const_cast<Animation&>(constAnimation);

  using LocalType = MessageValue1<UpdateManager, Animation*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::StopAnimation, &animation);
}

inline void ClearAnimationMessage(UpdateManager& manager, const Animation& constAnimation)
{
  // The scene-graph thread owns this object so it can safely edit it.
  Animation& animation = const_cast<Animation&>(constAnimation);

  using LocalType = MessageValue1<UpdateManager, Animation*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::ClearAnimation, &animation);
}

inline void RemoveAnimationMessage(UpdateManager& manager, const Animation& constAnimation)
{
  // The scene-graph thread owns this object so it can safely edit it.
  Animation& animation = const_cast<Animation&>(constAnimation);

  using LocalType = MessageValue1<UpdateManager, Animation*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::RemoveAnimation, &animation);
}

inline void AddRenderTaskListMessage(UpdateManager& manager, OwnerPointer<SceneGraph::RenderTaskList>& taskList)
{
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<SceneGraph::RenderTaskList>>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::AddRenderTaskList, taskList);
}

inline void RemoveRenderTaskListMessage(UpdateManager& manager, const RenderTaskList& constTaskList)
{
  // The scene-graph thread owns this object so it can safely edit it.
  RenderTaskList& taskList = const_cast<RenderTaskList&>(constTaskList);

  using LocalType = MessageValue1<UpdateManager, RenderTaskList*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::RemoveRenderTaskList, &taskList);
}

inline void AddSceneMessage(UpdateManager& manager, OwnerPointer<SceneGraph::Scene>& scene)
{
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<SceneGraph::Scene>>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::AddScene, scene);
}

inline void RemoveSceneMessage(UpdateManager& manager, const SceneGraph::Scene& constScene)
{
  // The scene-graph thread owns this object so it can safely edit it.
  Scene& scene = const_cast<Scene&>(constScene);

  using LocalType = MessageValue1<UpdateManager, Scene*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::RemoveScene, &scene);
}

inline void AddPropertyNotificationMessage(UpdateManager& manager, OwnerPointer<PropertyNotification>& propertyNotification)
{
  // Message has ownership of PropertyNotification while in transit from event -> update
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<PropertyNotification>>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::AddPropertyNotification, propertyNotification);
}

inline void RemovePropertyNotificationMessage(UpdateManager& manager, const PropertyNotification& constPropertyNotification)
{
  // The scene-graph thread owns this object so it can safely edit it.
  PropertyNotification& propertyNotification = const_cast<PropertyNotification&>(constPropertyNotification);

  using LocalType = MessageValue1<UpdateManager, PropertyNotification*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::RemovePropertyNotification, &propertyNotification);
}

inline void PropertyNotificationSetNotifyModeMessage(UpdateManager&                   manager,
                                                     const PropertyNotification*      constPropertyNotification,
                                                     PropertyNotification::NotifyMode notifyMode)
{
  // The scene-graph thread owns this object so it can safely edit it.
  PropertyNotification* propertyNotification = const_cast<PropertyNotification*>(constPropertyNotification);

  using LocalType = MessageValue2<UpdateManager, PropertyNotification*, PropertyNotification::NotifyMode>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::PropertyNotificationSetNotify, propertyNotification, notifyMode);
}

// The render thread can safely change the Shader
inline void AddShaderMessage(UpdateManager& manager, OwnerPointer<Shader>& shader)
{
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<Shader>>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::AddShader, shader);
}

// The render thread can safely change the Shader
inline void RemoveShaderMessage(UpdateManager& manager, const Shader* shader)
{
  using LocalType = MessageValue1<UpdateManager, Shader*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::RemoveShader, const_cast<Shader*>(shader));
}

inline void SurfaceReplacedMessage(UpdateManager& manager, const SceneGraph::Scene& constScene)
{
  // The scene-graph thread owns this object so it can safely edit it.
  Scene& scene = const_cast<Scene&>(constScene);

  using LocalType = MessageValue1<UpdateManager, Scene*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::SurfaceReplaced, &scene);
}

inline void KeepRenderingMessage(UpdateManager& manager, float durationSeconds)
{
  using LocalType = MessageValue1<UpdateManager, float>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::KeepRendering, durationSeconds);
}

inline void SetRenderingBehaviorMessage(UpdateManager& manager, DevelStage::Rendering renderingBehavior)
{
  using LocalType = MessageValue1<UpdateManager, DevelStage::Rendering>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::SetRenderingBehavior, renderingBehavior);
}

inline void RequestRenderingMessage(UpdateManager& manager)
{
  using LocalType = Message<UpdateManager>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::RequestRendering);
}

inline void RequestClearProgramCacheMessage(UpdateManager& manager)
{
  using LocalType = Message<UpdateManager>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::RequestClearProgramCache);
}

/**
 * Create a message for setting the depth of a layer
 * @param[in] manager The update manager
 * @param[in] layers list of layers
 * @param[in] rootLayer The rool layer
 */
inline void SetLayerDepthsMessage(UpdateManager& manager, const std::vector<Layer*>& layers, const Layer* rootLayer)
{
  using LocalType = MessageValue2<UpdateManager, std::vector<Layer*>, const Layer*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::SetLayerDepths, layers, rootLayer);
}

inline void AddRendererMessage(UpdateManager& manager, OwnerKeyType<Renderer>& rendererKeyPointer)
{
  using LocalType = MessageValue1<UpdateManager, OwnerKeyType<Renderer>>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));
  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::AddRenderer, rendererKeyPointer);
}

inline void RemoveRendererMessage(UpdateManager& manager, const RendererKey& rendererKey)
{
  using LocalType = MessageValue1<UpdateManager, RendererKey>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));
  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::RemoveRenderer, rendererKey);
}

inline void AttachRendererMessage(UpdateManager& manager, const Node& node, const Renderer& renderer)
{
  using LocalType = MessageValue2<UpdateManager, Node*, Renderer*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));
  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::AttachRenderer, const_cast<Node*>(&node), const_cast<Renderer*>(&renderer));
}

inline void AttachCacheRendererMessage(UpdateManager& manager, const Node& node, const Renderer& renderer)
{
  using LocalType = MessageValue2<UpdateManager, Node*, Renderer*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));
  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::AttachCacheRenderer, const_cast<Node*>(&node), const_cast<Renderer*>(&renderer));
}

// The render thread can safely change the Shader
inline void AddTextureSetMessage(UpdateManager& manager, OwnerPointer<TextureSet>& textureSet)
{
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<TextureSet>>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::AddTextureSet, textureSet);
}

// The render thread can safely change the Shader
inline void RemoveTextureSetMessage(UpdateManager& manager, TextureSet& textureSet)
{
  using LocalType = MessageValue1<UpdateManager, TextureSet*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::RemoveTextureSet, &textureSet);
}

inline void AddSamplerMessage(UpdateManager& manager, OwnerPointer<Render::Sampler>& sampler)
{
  // Message has ownership of Sampler while in transit from event -> update
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<Render::Sampler>>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::AddSampler, sampler);
}

inline void RemoveSamplerMessage(UpdateManager& manager, Render::Sampler& sampler)
{
  using LocalType = MessageValue1<UpdateManager, Render::Sampler*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::RemoveSampler, &sampler);
}

inline void AddVertexBufferMessage(UpdateManager& manager, OwnerPointer<Render::VertexBuffer>& vertexBuffer)
{
  // Message has ownership of vertexBuffer while in transit from event -> update
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<Render::VertexBuffer>>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::AddVertexBuffer, vertexBuffer);
}

inline void RemoveVertexBufferMessage(UpdateManager& manager, Render::VertexBuffer& vertexBuffer)
{
  using LocalType = MessageValue1<UpdateManager, Render::VertexBuffer*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::RemoveVertexBuffer, &vertexBuffer);
}

inline void AddGeometryMessage(UpdateManager& manager, OwnerPointer<Render::Geometry>& geometry)
{
  // Message has ownership of Geometry while in transit from event -> update
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<Render::Geometry>>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::AddGeometry, geometry);
}

inline void RemoveGeometryMessage(UpdateManager& manager, Render::Geometry& geometry)
{
  using LocalType = MessageValue1<UpdateManager, Render::Geometry*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::RemoveGeometry, &geometry);
}

inline void AddTextureMessage(UpdateManager& manager, OwnerKeyType<Render::Texture>& texture)
{
  using LocalType = MessageValue1<UpdateManager, OwnerKeyType<Render::Texture>>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::AddTexture, texture);
}

inline void RemoveTextureMessage(UpdateManager& manager, const Render::TextureKey& texture)
{
  using LocalType = MessageValue1<UpdateManager, Render::TextureKey>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::RemoveTexture, texture);
}

inline void AddFrameBufferMessage(UpdateManager& manager, OwnerPointer<Render::FrameBuffer>& frameBuffer)
{
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<Render::FrameBuffer>>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::AddFrameBuffer, frameBuffer);
}

inline void RemoveFrameBufferMessage(UpdateManager& manager, Render::FrameBuffer& frameBuffer)
{
  using LocalType = MessageValue1<UpdateManager, Render::FrameBuffer*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::RemoveFrameBuffer, &frameBuffer);
}

inline void SetDepthIndicesMessage(UpdateManager& manager, OwnerPointer<NodeDepths>& nodeDepths)
{
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<NodeDepths>>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::SetDepthIndices, nodeDepths);
}

inline void AddResetterMessage(UpdateManager& manager, OwnerPointer<PropertyResetterBase> resetter)
{
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<PropertyResetterBase>>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::AddPropertyResetter, resetter);
}

inline void AddFrameCallbackMessage(UpdateManager& manager, OwnerPointer<FrameCallback>& frameCallback, const Node& rootNode)
{
  using LocalType = MessageValue2<UpdateManager, OwnerPointer<FrameCallback>, const Node*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::AddFrameCallback, frameCallback, &rootNode);
}

inline void AddGlobalFrameCallbackMessage(UpdateManager& manager, OwnerPointer<FrameCallback>& frameCallback)
{
  using LocalType = MessageValue2<UpdateManager, OwnerPointer<FrameCallback>, const Node*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::AddFrameCallback, frameCallback, nullptr);
}

inline void RemoveFrameCallbackMessage(UpdateManager& manager, FrameCallbackInterface& frameCallback)
{
  using LocalType = MessageValue1<UpdateManager, FrameCallbackInterface*>;

  // Reserve some memory inside the message queue
  uint32_t* slot = manager.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&manager, &UpdateManager::RemoveFrameCallback, &frameCallback);
}

inline void NotifyFrameCallbackMessage(UpdateManager& manager, FrameCallbackInterface* frameCallback, Dali::UpdateProxy::NotifySyncPoint syncPoint)
{
  using LocalType = MessageValue2<UpdateManager, FrameCallbackInterface*, Dali::UpdateProxy::NotifySyncPoint>;
  uint32_t* slot  = manager.ReserveMessageSlot(sizeof(LocalType));
  new(slot) LocalType(&manager, &UpdateManager::NotifyFrameCallback, frameCallback, syncPoint);
}

inline void AddUniformBlockMessage(UpdateManager& manager, OwnerPointer<Render::UniformBlock>& uniformBlock)
{
  using LocalType = MessageValue1<UpdateManager, OwnerPointer<Render::UniformBlock>>;
  uint32_t* slot  = manager.ReserveMessageSlot(sizeof(LocalType));
  new(slot) LocalType(&manager, &UpdateManager::AddUniformBlock, uniformBlock);
}

inline void RemoveUniformBlockMessage(UpdateManager& manager, Render::UniformBlock& uniformBlock)
{
  using LocalType = MessageValue1<UpdateManager, Render::UniformBlock*>;
  uint32_t* slot  = manager.ReserveMessageSlot(sizeof(LocalType));
  new(slot) LocalType(&manager, &UpdateManager::RemoveUniformBlock, &uniformBlock);
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_UPDATE_MANAGER_H
