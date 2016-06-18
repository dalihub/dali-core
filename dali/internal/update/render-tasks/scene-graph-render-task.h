#ifndef __DALI_INTERNAL_SCENE_GRAPH_RENDER_TASK_H__
#define __DALI_INTERNAL_SCENE_GRAPH_RENDER_TASK_H__

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
#include <dali/public-api/math/viewport.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/render/renderers/render-frame-buffer.h>

namespace Dali
{

namespace Internal
{
class FrameBufferTexture;
class ResourceManager;

namespace Render
{
class RenderTracker;
}

namespace SceneGraph
{
class Node;
class Camera;
class RenderInstruction;
class RenderMessageDispatcher;

/**
 * RenderTasks describe how the Dali scene should be rendered.
 */
class RenderTask : public PropertyOwner
{
public:

  enum State
  {
    RENDER_CONTINUOUSLY,               ///< mRefreshRate > 0
    RENDER_ONCE_WAITING_FOR_RESOURCES, ///< mRefreshRate = REFRESH_ONCE
    RENDERED_ONCE,                     ///< mRefreshRate = REFRESH_ONCE & rendered
    RENDERED_ONCE_AND_NOTIFIED         ///< mRefreshRate = REFRESH_ONCE & rendered & notified
  };

  /**
   * Create a new RenderTask
   */
  static RenderTask* New();

  /**
   * Virtual destructor
   */
  virtual ~RenderTask();

  /**
   * Initialize the render task. Called in update thread
   * @param[in] renderMessageDispatcher to send messages to render thread
   * @param[in] resourceManager to check and update status of FBOs
   */
  void Initialize( RenderMessageDispatcher& renderMessageDispatcher, ResourceManager& resourceManager );

  /**
   * Set the nodes to be rendered.
   * @param[in] node This node and its children will be rendered.
   */
  void SetSourceNode( Node* node );

  /**
   * Retrieve the source node.
   * @return This node and its children will be rendered.
   */
  Node* GetSourceNode() const;

  /**
   * Set whether the RenderTask has exclusive access to the source nodes.
   * @param[in] exclusive True if the source nodes will only be rendered by this render-task.
   */
  void SetExclusive( bool exclusive );

  /**
   * Query whether the RenderTask has exclusive access to the source actors.
   * @return True if the source actors will only be rendered by this render-task.
   */
  bool IsExclusive() const;

  /**
   * Set the camera from which the scene is viewed.
   * @param[in] cameraNode that camera is connected with
   * @param[in] camera to use.
   */
  void SetCamera( Node* cameraNode, Camera* camera );

  /**
   * Set the frame-buffer used as a render target.
   * @param[in] resourceId The resource ID of the frame-buffer, or zero if not rendering off-screen.
   * @param[in] isNativeFBO if this render task is targeting a native FBO
   */
  void SetFrameBufferId( unsigned int resourceId, bool isNativeFBO );

  /**
   * Retrieve the resource ID of the frame-buffer.
   * @return The resource ID, or zero if not rendering off-screen.
   */
  unsigned int GetFrameBufferId() const;

  /**
   * Set the frame-buffer used as a render target.
   * @param[in] frameBuffer The framebuffer
   */
  void SetFrameBuffer( Render::FrameBuffer* frameBuffer );

  /**
   * Retrieve the resource ID of the frame-buffer.
   * @return The resource ID, or zero if not rendering off-screen.
   */
  Render::FrameBuffer* GetFrameBuffer();

  /**
   * Set the value of property viewportPosition
   * This value will persist only for the current frame.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] value The value of the property
   */
  void SetViewportPosition( BufferIndex updateBufferIndex, const Vector2& value );

  /**
   * Get the value of property viewportPosition
   * @warning Should only be called from the Update thread
   * @param[in] bufferIndex The buffer to read from.
   * @return the value of the property.
   */
  const Vector2& GetViewportPosition( BufferIndex bufferIndex ) const;

  /**
   * Bake the value of the property viewportPosition
   * This will also set the base value
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] value The new value for property.
   */
  void BakeViewportPosition( BufferIndex updateBufferIndex, const Vector2& value );

  /**
   * Set the value of property viewportSize
   * This value will persist only for the current frame.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] value The value of the property
   */
  void SetViewportSize( BufferIndex updateBufferIndex, const Vector2& value );

  /**
   * Get the value of property viewportSize
   * @warning Should only be called from the Update thread
   * @param[in] bufferIndex The buffer to read from.
   * @return the value of the property.
   */
  const Vector2& GetViewportSize( BufferIndex bufferIndex ) const;

  /**
   * Bake the value of the property viewportSize
   * This will also set the base value
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] value The new value for property.
   */
  void BakeViewportSize( BufferIndex updateBufferIndex, const Vector2& value );

  /**
   * Get the value of property viewportEnabled
   * @warning Should only be called from the Update thread
   * @param[in] bufferIndex The buffer to read from.
   * @return the value of the property.
   */
  bool GetViewportEnabled( BufferIndex bufferIndex ) const;

  /**
   * Query whether the optional viewport is set.
   * @param[in] bufferIndex The buffer to read from.
   * @param[out] viewport The viewport position and size is populated.
   * @return true if the viewport has been set
   */
  bool QueryViewport( BufferIndex bufferIndex, Viewport& viewport ) const;

  /**
   * Set the value of property clearColor
   * This value will persist only for the current frame.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] value The value of the property
   */
  void SetClearColor( BufferIndex updateBufferIndex, const Vector4& value );

  /**
   * Get the value of property clearColor
   * @warning Should only be called from the Update thread
   * @param[in] bufferIndex The buffer to read from.
   * @return the value of the property.
   */
  const Vector4& GetClearColor( BufferIndex bufferIndex ) const;

  /**
   * Bake the value of the property clearColor
   * This will also set the base value
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] value The new value for property.
   */
  void BakeClearColor( BufferIndex updateBufferIndex, const Vector4& value );

  /**
   * @copydoc Dali::RenderTask::SetClearEnabled()
   */
  void SetClearEnabled( bool enabled );

  /**
   * @copydoc Dali::RenderTask::GetClearEnabled()
   */
  bool GetClearEnabled() const;

  /**
   * @copydoc Dali::RenderTask::SetCullMode()
   */
  void SetCullMode( bool mode );

  /**
   * @copydoc Dali::RenderTask::GetCullMode()
   */
  bool GetCullMode() const;

  /**
   * Set the refresh-rate of the RenderTask.
   * @param[in] refreshRate The new refresh rate.
   */
  void SetRefreshRate( unsigned int refreshRate );

  /**
   * Retrieve the refresh-rate of the RenderTask.
   * @return The refresh rate.
   */
  unsigned int GetRefreshRate() const;

  /**
   * Check if the render task is ready for rendering.
   * @param[in] updateBufferIndex The current update buffer index.
   * @return True if the render-task is ready for rendering.
   */
  bool ReadyToRender( BufferIndex updateBufferIndex );

  /**
   * True if a render is required. If the current state is RENDER_CONTINUOUSLY, then
   * this returns true if the frame count is zero. If the current state is RENDER_ONCE_WAITING_FOR_RESOURCES, then it always returns true. In all other states, it returns false.
   * @return true if a render is required
   */
  bool IsRenderRequired();

  /**
   * Set whether all resources were available when the render-task was processed
   * @param[in] resourcesComplete True if the resources of the source tree are completely loaded.
   */
  void SetResourcesFinished( bool resourcesFinished );

  /**
   * Process a frame. This method is called each frame for every ready render task, regardless
   * of whether it needs to render (so that the frame counter can be updated).
   */
  void UpdateState();

  /**
   * Return true only if currently waiting for the render task to
   * finish rendering and the update thread should be kept alive.
   * @return true if waiting to be rendered
   */
  bool IsWaitingToRender();

  /**
   * Return true when the render task has finished rendering and a notification
   * needs sending. (Only one notification is sent per render once request)
   * @return true if notification is required.
   */
  bool HasRendered();

  /**
   * @return The number of times we have transited from RENDERED_ONCE to RENDERED_ONCE_AND_NOTIFIED state.
   */
  unsigned int GetRenderedOnceCounter() const;

  /**
   * Retrieve the view-matrix; this is double buffered for input handling.
   * @pre GetCameraNode() returns a node with valid Camera.
   * @param[in] bufferIndex The buffer to read from.
   * @return The view-matrix.
   */
  const Matrix& GetViewMatrix( BufferIndex bufferIndex ) const;

  /**
   * @brief Retrieve the camera.
   * @pre GetCameraNode() returns a node with valid Camera.
   *
   * @return The camera.
   */
  SceneGraph::Camera& GetCamera() const;

  /**
   * Retrieve the projection-matrix; this is double buffered for input handling.
   * @pre GetCameraNode() returns a node with valid Camera.
   * @param[in] bufferIndex The buffer to read from.
   * @return The projection-matrix.
   */
  const Matrix& GetProjectionMatrix( BufferIndex bufferIndex ) const;

  /**
   * Prepares the render-instruction buffer to be populated with instructions.
   *
   * If the render task is a render-once framebuffer backed by a native image,
   * then this method will ensure that a GL sync object is created to track
   * when the rendering has finished.
   *
   * @param[out] instruction to prepare
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void PrepareRenderInstruction( RenderInstruction& instruction, BufferIndex updateBufferIndex );

  /**
   * @return true if the view matrix has been updated during this or last frame
   */
  bool ViewMatrixUpdated();

  /**
   * Indicate whether GL sync is required for native render target.
   * @param[in] requiresSync whether GL sync is required for native render target
   */
  void SetSyncRequired( bool requiresSync );

private:

  /**
   * Protected constructor.
   */
  RenderTask();

  // Undefined
  RenderTask(const RenderTask&);

  // Undefined
  RenderTask& operator=(const RenderTask&);

private: // PropertyOwner

  virtual void ResetDefaultProperties( BufferIndex currentBufferIndex );

public: // Animatable Properties
  AnimatableProperty< Vector2 >   mViewportPosition;    ///< viewportPosition
  AnimatableProperty< Vector2 >   mViewportSize;        ///< viewportSize
  AnimatableProperty< Vector4 >   mClearColor;          ///< clearColor

private:
  RenderMessageDispatcher* mRenderMessageDispatcher;
  ResourceManager* mResourceManager;
  Render::RenderTracker* mRenderSyncTracker;
  Node* mSourceNode;
  Node* mCameraNode;
  SceneGraph::Camera* mCamera;
  unsigned int mFrameBufferResourceId;
  Render::FrameBuffer* mFrameBuffer;

  bool mResourcesFinished:1; ///< True if all resources were available when the render-task was processed
  bool mWaitingToRender:1; ///< True when an render once to FBO is waiting
  bool mNotifyTrigger:1; ///< True if a render once render task has finished renderering
  bool mExclusive: 1; ///< Whether the render task has exclusive access to the source actor (node in the scene graph implementation).
  bool mClearEnabled: 1; ///< Whether previous results are cleared.
  bool mCullMode: 1; ///< Whether renderers should be frustum culled

  FrameBufferTexture* mRenderTarget;
  Viewport mViewport;

  State mState;                     ///< Render state.
  unsigned int mRefreshRate;        ///< REFRESH_ONCE, REFRESH_ALWAYS or render every N frames
  unsigned int mFrameCounter;       ///< counter for rendering every N frames

  unsigned int mRenderedOnceCounter;  ///< Incremented whenever state changes to RENDERED_ONCE_AND_NOTIFIED
  bool mTargetIsNativeFramebuffer; ///< Tells if our target is a native framebuffer
  bool mRequiresSync;              ///< Whether sync is needed to track the render

};

// Messages for RenderTask

inline void SetFrameBufferIdMessage( EventThreadServices& eventThreadServices, RenderTask& task, unsigned int resourceId, bool isNativeFBO )
{
  typedef MessageValue2< RenderTask, unsigned int, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::SetFrameBufferId, resourceId, isNativeFBO );
}

inline void SetFrameBufferMessage( EventThreadServices& eventThreadServices, RenderTask& task, Render::FrameBuffer* frameBuffer )
{
  typedef MessageValue1< RenderTask, Render::FrameBuffer*> LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::SetFrameBuffer, frameBuffer );
}

inline void SetClearColorMessage( EventThreadServices& eventThreadServices, RenderTask& task, const Vector4& value )
{
  typedef MessageDoubleBuffered1< RenderTask, Vector4 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::SetClearColor, value );
}

inline void BakeClearColorMessage( EventThreadServices& eventThreadServices, const RenderTask& task, const Vector4& value )
{
  typedef MessageDoubleBuffered1< RenderTask, Vector4 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::BakeClearColor, value );
}

inline void SetClearEnabledMessage( EventThreadServices& eventThreadServices, RenderTask& task, bool enabled )
{
  typedef MessageValue1< RenderTask, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::SetClearEnabled, enabled );
}

inline void SetCullModeMessage( EventThreadServices& eventThreadServices, RenderTask& task, bool mode )
{
  typedef MessageValue1< RenderTask, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::SetCullMode, mode );
}

inline void SetRefreshRateMessage( EventThreadServices& eventThreadServices, RenderTask& task, unsigned int refreshRate )
{
  typedef MessageValue1< RenderTask, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::SetRefreshRate, refreshRate );
}

inline void SetSourceNodeMessage( EventThreadServices& eventThreadServices, RenderTask& task, const Node* constNode )
{
  // Scene graph thread can destroy this object.
  Node* node = const_cast< Node* >( constNode );

  typedef MessageValue1< RenderTask, Node* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::SetSourceNode, node );
}

inline void SetCameraMessage( EventThreadServices& eventThreadServices, RenderTask& task, const Node* constNode, const Camera* constCamera )
{
  typedef MessageValue2< RenderTask, Node*, Camera* > LocalType;

  Node* node = const_cast< Node* >( constNode );
  Camera* camera = const_cast< Camera* >( constCamera );
  // Reserve memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::SetCamera, node, camera );
}

inline void SetExclusiveMessage( EventThreadServices& eventThreadServices, RenderTask& task, bool exclusive )
{
  typedef MessageValue1< RenderTask, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::SetExclusive, exclusive );
}

inline void SetSyncRequiredMessage(EventThreadServices& eventThreadServices, RenderTask& task, bool requiresSync )
{
  typedef MessageValue1< RenderTask, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::SetSyncRequired, requiresSync );
}

inline void BakeViewportPositionMessage( EventThreadServices& eventThreadServices, const RenderTask& task, const Vector2& value )
{
  typedef MessageDoubleBuffered1< RenderTask, Vector2 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::BakeViewportPosition, value );
}

inline void BakeViewportSizeMessage( EventThreadServices& eventThreadServices, const RenderTask& task, const Vector2& value )
{
  typedef MessageDoubleBuffered1< RenderTask, Vector2 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::BakeViewportSize, value );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_RENDER_TASK_H__
