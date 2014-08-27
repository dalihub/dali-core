#ifndef __DALI_INTERNAL_SCENE_GRAPH_RENDER_TASK_H__
#define __DALI_INTERNAL_SCENE_GRAPH_RENDER_TASK_H__

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
#include <dali/public-api/math/viewport.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/event-to-update.h>
#include <dali/internal/common/message.h>
#include <dali/internal/update/common/double-buffered.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/animatable-property.h>

namespace Dali
{

namespace Internal
{
class FrameBufferTexture;
class CompleteStatusManager;

namespace SceneGraph
{
class Node;
class CameraAttachment;
class RenderInstruction;

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
   * Set the node from which the scene is viewed.
   * @param[in] node The scene is viewed from the perspective of this node.
   */
  void SetCameraNode( Node* node );

  /**
   * Set the frame-buffer used as a render target.
   * @param[in] resourceId The resource ID of the frame-buffer, or zero if not rendering off-screen.
   */
  void SetFrameBufferId( unsigned int resourceId );

  /**
   * Retrieve the resource ID of the frame-buffer.
   * @return The resource ID, or zero if not rendering off-screen.
   */
  unsigned int GetFrameBufferId() const;

  /**
   * Set the value of property viewport-position
   * This value will persist only for the current frame.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] value The value of the property
   */
  void SetViewportPosition( BufferIndex updateBufferIndex, const Vector2& value );

  /**
   * Get the value of property viewport-position
   * @warning Should only be called from the Update thread
   * @param[in] bufferIndex The buffer to read from.
   * @return the value of the property.
   */
  const Vector2& GetViewportPosition( BufferIndex bufferIndex ) const;

  /**
   * Bake the value of the property viewport-position
   * This will also set the base value
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] value The new value for property.
   */
  void BakeViewportPosition( BufferIndex updateBufferIndex, const Vector2& value );

  /**
   * Set the value of property viewport-size
   * This value will persist only for the current frame.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] value The value of the property
   */
  void SetViewportSize( BufferIndex updateBufferIndex, const Vector2& value );

  /**
   * Get the value of property viewport-size
   * @warning Should only be called from the Update thread
   * @param[in] bufferIndex The buffer to read from.
   * @return the value of the property.
   */
  const Vector2& GetViewportSize( BufferIndex bufferIndex ) const;

  /**
   * Bake the value of the property viewport-size
   * This will also set the base value
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] value The new value for property.
   */
  void BakeViewportSize( BufferIndex updateBufferIndex, const Vector2& value );

  /**
   * Get the value of property viewport-enabled
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
   * Set the value of property clear-color
   * This value will persist only for the current frame.
   * @param[in] updateBufferIndex The current update buffer index.
   * @param[in] value The value of the property
   */
  void SetClearColor( BufferIndex updateBufferIndex, const Vector4& value );

  /**
   * Get the value of property clear-color
   * @warning Should only be called from the Update thread
   * @param[in] bufferIndex The buffer to read from.
   * @return the value of the property.
   */
  const Vector4& GetClearColor( BufferIndex bufferIndex ) const;

  /**
   * Bake the value of the property clear-color
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
   * @pre GetCameraNode() returns a node with valid CameraAttachment.
   * @param[in] bufferIndex The buffer to read from.
   * @return The view-matrix.
   */
  const Matrix& GetViewMatrix( BufferIndex bufferIndex ) const;

  /**
   * Retrieve the projection-matrix; this is double buffered for input handling.
   * @pre GetCameraNode() returns a node with valid CameraAttachment.
   * @param[in] bufferIndex The buffer to read from.
   * @return The projection-matrix.
   */
  const Matrix& GetProjectionMatrix( BufferIndex bufferIndex ) const;

  /**
   * Prepares the render-instruction buffer to be populated with instructions.
   * @param[out] instruction to prepare
   * @param[in] updateBufferIndex The current update buffer index.
   */
  void PrepareRenderInstruction( RenderInstruction& instruction, BufferIndex updateBufferIndex );

  /**
   * @return true if the view matrix has been updated during this or last frame
   */
  bool ViewMatrixUpdated();

  /**
   * Set the complete status tracker.
   * @param[in] completeStatusManager The complete status Tracker (not owned)
   */
  void SetCompleteStatusManager( CompleteStatusManager* completeStatusManager );

  /**
   * @return A pointer to the camera used by the RenderTask
   */
  Node* GetCamera() const;

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
  AnimatableProperty< Vector2 >   mViewportPosition;    ///< viewport-position
  AnimatableProperty< Vector2 >   mViewportSize;        ///< viewport-size
  AnimatableProperty< Vector4 >   mClearColor;          ///< clear-color

private:
  CompleteStatusManager* mCompleteStatusManager;
  Node* mSourceNode;
  Node* mCameraNode;
  CameraAttachment* mCameraAttachment;
  unsigned int mFrameBufferResourceId;

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

};

// Messages for RenderTask

inline void SetFrameBufferIdMessage( EventToUpdate& eventToUpdate, RenderTask& task, unsigned int resourceId )
{
  typedef MessageValue1< RenderTask, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::SetFrameBufferId, resourceId );
}

inline void SetClearColorMessage( EventToUpdate& eventToUpdate, RenderTask& task, const Vector4& value )
{
  typedef MessageDoubleBuffered1< RenderTask, Vector4 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::SetClearColor, value );
}

inline void BakeClearColorMessage( EventToUpdate& eventToUpdate, const RenderTask& task, const Vector4& value )
{
  typedef MessageDoubleBuffered1< RenderTask, Vector4 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::BakeClearColor, value );
}

inline void SetClearEnabledMessage( EventToUpdate& eventToUpdate, RenderTask& task, bool enabled )
{
  typedef MessageValue1< RenderTask, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::SetClearEnabled, enabled );
}

inline void SetCullModeMessage( EventToUpdate& eventToUpdate, RenderTask& task, bool mode )
{
  typedef MessageValue1< RenderTask, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::SetCullMode, mode );
}

inline void SetRefreshRateMessage( EventToUpdate& eventToUpdate, RenderTask& task, unsigned int refreshRate )
{
  typedef MessageValue1< RenderTask, unsigned int > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::SetRefreshRate, refreshRate );
}

inline void SetSourceNodeMessage( EventToUpdate& eventToUpdate, RenderTask& task, const Node* constNode )
{
  // Scene graph thread can destroy this object.
  Node* node = const_cast< Node* >( constNode );

  typedef MessageValue1< RenderTask, Node* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::SetSourceNode, node );
}

inline void SetCameraNodeMessage( EventToUpdate& eventToUpdate, RenderTask& task, const Node* constNode )
{
  // Scene graph thread can destroy this object.
  Node* node = const_cast< Node* >( constNode );

  typedef MessageValue1< RenderTask, Node* > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::SetCameraNode, node );
}

inline void SetExclusiveMessage( EventToUpdate& eventToUpdate, RenderTask& task, bool exclusive )
{
  typedef MessageValue1< RenderTask, bool > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::SetExclusive, exclusive );
}

inline void BakeViewportPositionMessage( EventToUpdate& eventToUpdate, const RenderTask& task, const Vector2& value )
{
  typedef MessageDoubleBuffered1< RenderTask, Vector2 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::BakeViewportPosition, value );
}

inline void BakeViewportSizeMessage( EventToUpdate& eventToUpdate, const RenderTask& task, const Vector2& value )
{
  typedef MessageDoubleBuffered1< RenderTask, Vector2 > LocalType;

  // Reserve some memory inside the message queue
  unsigned int* slot = eventToUpdate.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &task, &RenderTask::BakeViewportSize, value );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_RENDER_TASK_H__
