#ifndef DALI_INTERNAL_RENDER_TASK_H
#define DALI_INTERNAL_RENDER_TASK_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/object/weak-handle.h>
#include <dali/public-api/render-tasks/render-task.h>
#include <dali/public-api/images/pixel-data.h>

#include <dali/internal/event/common/object-impl.h>
#include <dali/internal/event/events/actor-observer.h>
#include <dali/internal/event/rendering/frame-buffer-impl.h>

namespace Dali
{
namespace Internal
{
class Actor;
class CameraActor;
class EventThreadServices;
class RenderTaskList;

namespace SceneGraph
{
class RenderTaskList;
class RenderTask;
class Camera;
} // namespace SceneGraph

using RenderTaskPtr = IntrusivePtr<RenderTask>;

class RenderTask : public Object
{
public:
  using ScreenToFrameBufferFunction = Dali::RenderTask::ScreenToFrameBufferFunction;

  /**
   * Creates a new RenderTask.
   *
   * @param[in] sourceActor The source actor.
   * @param[in] cameraActor The camera actor.
   * @param[in] renderTaskList The render task list.
   * @param[in] isOverlayTask True if this render task is overlay task. default is false.
   * @return The created render task
   */
  static RenderTaskPtr New(Actor* sourceActor, CameraActor* cameraActor, RenderTaskList& renderTaskList, bool isOverlayTask = false);

  /**
   * @copydoc Dali::RenderTask::SetSourceActor()
   */
  void SetSourceActor(Actor* actor);

  /**
   * @copydoc Dali::RenderTask::GetSourceActor()
   */
  Actor* GetSourceActor() const;

  /**
   * @copydoc Dali::RenderTask::GetStopperActor()
   */
  Actor* GetStopperActor() const;

  /**
   * @copydoc Dali::RenderTask::SetExclusive()
   */
  void SetExclusive(bool exclusive);

  /**
   * @copydoc Dali::RenderTask::IsExclusive()
   */
  bool IsExclusive() const;

  /**
   * @copydoc Dali::RenderTask::SetInputEnabled()
   */
  void SetInputEnabled(bool enabled);

  /**
   * @copydoc Dali::RenderTask::GetInputEnabled()
   */
  bool GetInputEnabled() const;

  /**
   * @copydoc Dali::RenderTask::SetCameraActor()
   */
  void SetCameraActor(CameraActor* cameraActor);

  /**
   * @copydoc Dali::RenderTask::GetCameraActor()
   */
  CameraActor* GetCameraActor() const;

  /**
   * @copydoc Dali::RenderTask::SetFrameBuffer()
   */
  void SetFrameBuffer(FrameBufferPtr frameBuffer);

  /**
   * @copydoc Dali::RenderTask::GetFrameBuffer
   */
  FrameBuffer* GetFrameBuffer() const;

  /**
   * @copydoc Dali::RenderTask::SetScreenToFrameBufferFunction
   */
  void SetScreenToFrameBufferFunction(ScreenToFrameBufferFunction conversionFunction);

  /**
   * @copydoc Dali::RenderTask::GetScreenToFrameBufferFunction
   */
  ScreenToFrameBufferFunction GetScreenToFrameBufferFunction() const;

  /**
   * @copydoc Dali::RenderTask::SetScreenToFrameBufferMappingActor
   */
  void SetScreenToFrameBufferMappingActor(Dali::Actor& mappingActor);

  /**
   * @copydoc Dali::RenderTask::GetScreenToFrameBufferMappingActor
   */
  Dali::Actor GetScreenToFrameBufferMappingActor() const;

  /**
   * @copydoc Dali::RenderTask::SetViewportGuideActor
   */
  void SetViewportGuideActor(Actor* actor);

  /**
   * @copydoc Dali::RenderTask::GetViewportGuideActor
   */
  Actor* GetViewportGuideActor() const;

  /**
   * @copydoc Dali::RenderTask::ResetViewportGuideActor
   */
  void ResetViewportGuideActor();

  /**
   * @copydoc Dali::RenderTask::SetViewportPosition
   */
  void SetViewportPosition(const Vector2& value);

  /**
   * @copydoc Dali::RenderTask::GetCurrentViewportPosition
   */
  Vector2 GetCurrentViewportPosition() const;

  /**
   * @copydoc Dali::RenderTask::SetViewportSize
   */
  void SetViewportSize(const Vector2& value);

  /**
   * @copydoc Dali::RenderTask::GetCurrentViewportSize
   */
  Vector2 GetCurrentViewportSize() const;

  /**
   * @copydoc Dali::RenderTask::SetViewport()
   */
  void SetViewport(const Viewport& viewport);

  /**
   * @param[out] viewPort instance to copy the values into
   */
  void GetViewport(Viewport& viewPort) const;

  /**
   * @copydoc Dali::RenderTask::SetClearColor()
   */
  void SetClearColor(const Vector4& color);

  /**
   * @copydoc Dali::RenderTask::GetClearColor()
   */
  const Vector4& GetClearColor() const;

  /**
   * Indicate whether GL sync is required for native render target.
   * @param[in] requiresSync whether GL sync is required.
   */
  void SetSyncRequired(bool requiresSync);

  /**
   * Query whether the sync object is required for native render target.
   * @return True if the sync object is required, false otherwise.
   */
  bool IsSyncRequired() const;

  /**
   * @copydoc Dali::RenderTask::SetClearEnabled()
   */
  void SetClearEnabled(bool enabled);

  /**
   * @copydoc Dali::RenderTask::GetClearEnabled()
   */
  bool GetClearEnabled() const;

  /**
   * @copydoc Dali::RenderTask::SetCullMode()
   */
  void SetCullMode(bool mode);

  /**
   * @copydoc Dali::RenderTask::GetCullMode()
   */
  bool GetCullMode() const;

  /**
   * @copydoc Dali::RenderTask::SetRefreshRate()
   */
  void SetRefreshRate(uint32_t refreshRate);

  /**
   * @copydoc Dali::RenderTask::GetRefreshRate()
   */
  uint32_t GetRefreshRate() const;

  /**
   * Retrieves whether the RenderTask is input available or not
   * @return True if the RenderTask is input available.
   */
  bool IsInputAvailable() const;

  /**
   * Check if the render-task is hittable. If render task is offscreen, the screen coordinates may be translated.
   * @param[in,out] screenCoords The screen coordinate, which may be converted (for hit-testing actors which are rendered off-screen).
   * @return True the render-task can be used for input-handling; otherwise the output parameters are not valid.
   */
  bool IsHittable(Vector2& screenCoords) const;

  /**
   * Translates screen coordinates to render task coordinates for offscreen render tasks
   * @param[in,out] screenCoords The screen coordinates, which may be converted (for off-screen).
   * @return false if the conversion function decides the coordinates are not inside. returns true if there is no conversion function
   */
  bool TranslateCoordinates(Vector2& screenCoords) const;

  /**
   * @brief Get Viewport when we need to be used with translated screen coordinated when render task is offscreen.
   * It will be used when we hit-test.
   * @param[out] viewPort instance to copy the values into
   */
  void GetHittableViewport(Viewport& viewPort) const;

  /**
   * @copydoc Dali::RenderTask::WorldToViewport()
   */
  bool WorldToViewport(const Vector3& position, float& viewportX, float& viewportY) const;

  /**
   * @copydoc Dali::RenderTask::ViewportToLocal()
   */
  bool ViewportToLocal(Actor* actor, float viewportX, float viewportY, float& localX, float& localY) const;

  /**
   * @copydoc Dali::RenderTask::SetRenderPassTag()
   */
  void SetRenderPassTag(uint32_t renderPassTag);

  /**
   * @copydoc Dali::RenderTask::GetRenderPassTag()
   */
  uint32_t GetRenderPassTag() const;

  /**
   * @copydoc Dali::RenderTask::SetOrderIndex()
   */
  void SetOrderIndex(int32_t orderIndex);

  /**
   * @copydoc Dali::RenderTask::GetOrderIndex()
   */
  int32_t GetOrderIndex() const;

  /**
   * @copydoc Dali::RenderTask::GetRenderTaskId()
   */
  uint32_t GetRenderTaskId() const;

  /**
   * @copydoc Dali::RenderTask::RenderUntil
   */
  void RenderUntil(Actor* stopperActor);

  /**
   * @copydoc Dali::RenderTask::KeepRenderResult
   */
  void KeepRenderResult();

  /**
   * @copydoc Dali::RenderTask::ClearRenderResult
   */
  void ClearRenderResult();

  /**
   * @copydoc Dali::RenderTask::GetRenderResult
   */
  Dali::PixelData GetRenderResult();

public: // Used by RenderTaskList, which owns the SceneGraph::RenderTasks
  /**
   * Retrieve the scene-graph RenderTask object.
   * @return The scene-graph object
   */
  const SceneGraph::RenderTask* GetRenderTaskSceneObject() const;

  /**
   * Remove the scene-graph RenderTask object.
   * @param[in] renderTaskList The render task list.
   */
  void RemoveRenderTaskSceneObject(RenderTaskList& renderTaskList);

  /**
   * Retrieve the render task list RenderTask object belongs to.
   * @return The render task list
   */
  RenderTaskList& GetRenderTaskList() const;

public: // Implementation of Object
  /**
   * @copydoc Dali::Internal::Object::SetDefaultProperty()
   */
  void SetDefaultProperty(Property::Index index, const Property::Value& propertyValue) override;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultProperty()
   */
  Property::Value GetDefaultProperty(Property::Index index) const override;

  /**
   * @copydoc Dali::Internal::Object::GetDefaultPropertyCurrentValue()
   */
  Property::Value GetDefaultPropertyCurrentValue(Property::Index index) const override;

  /**
   * @copydoc Dali::Internal::Object::OnNotifyDefaultPropertyAnimation()
   */
  void OnNotifyDefaultPropertyAnimation(Animation& animation, Property::Index index, const Property::Value& value, Animation::Type animationType) override;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectAnimatableProperty()
   */
  const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty(Property::Index index) const override;

  /**
   * @copydoc Dali::Internal::Object::GetSceneObjectInputProperty()
   */
  const PropertyInputImpl* GetSceneObjectInputProperty(Property::Index index) const override;

public: // signals
  /**
   * Query whether a Finished signal should be emitted for this render-task.
   * This should only be called by NotificationManager, before signals are emitted.
   * @pre The refresh rate must be REFRESH_ONCE.
   * @post HasFinished() will return false on subsequent calls, until the render-task is processed again.
   * @return \e true if the refresh once render task has finished. Otherwise it returns \e false.
   */
  bool HasFinished();

  /**
   * Emit the Finished signal
   */
  void EmitSignalFinish();

  /**
   * @copydoc Dali::RenderTask::FinishedSignal()
   */
  Dali::RenderTask::RenderTaskSignalType& FinishedSignal();

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor);

protected:
  /**
   * Constructor.
   *
   * @param[in] sceneObject The scene graph object
   * @param[in] renderTaskList The render task list
   */
  RenderTask(const SceneGraph::RenderTask* sceneObject, RenderTaskList& renderTaskList);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~RenderTask() override;

private: // not copyable
  RenderTask()                  = delete;
  RenderTask(const RenderTask&) = delete;
  RenderTask& operator=(const RenderTask&) = delete;

private:
  ActorObserver           mSourceActor;        ///< Source actor
  ActorObserver           mCameraActor;        ///< Camera actor
  ActorObserver           mViewportGuideActor; ///< Actor to matching viewport of this render task to this Actor.
  ActorObserver           mStopperActor;       ///< A child of mSourceActor. Actor to stop rendering.
  WeakHandle<Dali::Actor> mInputMappingActor;  /// used to mapping screen to frame buffer coordinate, not kept alive by rendertask
  RenderTaskList&         mRenderTaskList;     ///< The render task list

  Vector4 mClearColor; ///< Optional clear color

  Vector2 mViewportPosition; ///< The cached viewport position
  Vector2 mViewportSize;     ///< The cached viewport size

  uint32_t mRefreshRate; ///< Determines how often the task is processed.

  uint32_t mRefreshOnceCounter;

  FrameBufferPtr mFrameBuffer;

  Dali::RenderTask::ScreenToFrameBufferFunction mScreenToFrameBufferFunction; ///< Used to convert screen to frame-buffer coordinates

  uint32_t mRenderPassTag{0u};
  int32_t mOrderIndex{0u};

  uint32_t mRenderTaskId{0u};

  bool mIsRequestedToKeepRenderResult : 1;
  bool mExclusive : 1;    ///< True if the render-task has exclusive access to the source Nodes.
  bool mInputEnabled : 1; ///< True if the render-task should be considered for input handling.
  bool mClearEnabled : 1; ///< True if the render-task should be clear the color buffer.
  bool mCullMode : 1;     ///< True if the render-task's actors should be culled
  bool mRequiresSync : 1; ///< True if the GL sync is required to track the render of.

  // Signals
  Dali::RenderTask::RenderTaskSignalType mSignalFinished; ///< Signal emmited when the render task has been processed.
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::RenderTask& GetImplementation(Dali::RenderTask& task)
{
  DALI_ASSERT_ALWAYS(task && "RenderTask handle is empty");

  BaseObject& handle = task.GetBaseObject();

  return static_cast<Internal::RenderTask&>(handle);
}

inline const Internal::RenderTask& GetImplementation(const Dali::RenderTask& task)
{
  DALI_ASSERT_ALWAYS(task && "RenderTask handle is empty");

  const BaseObject& handle = task.GetBaseObject();

  return static_cast<const Internal::RenderTask&>(handle);
}

} // namespace Dali

#endif // DALI_INTERNAL_RENDER_TASK_H
