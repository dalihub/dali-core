#ifndef DALI_INTERNAL_SCENE_H
#define DALI_INTERNAL_SCENE_H

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
#include <dali/integration-api/scene.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/events/event-processor.h>
#include <dali/internal/event/render-tasks/render-task-defaults.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/render-tasks/render-task-list.h>

namespace Dali
{
namespace Integration
{
struct Event;

}

namespace Internal
{
namespace SceneGraph
{
class Scene;
} // namespace SceneGraph

class EventProcessor;
class Layer;
class LayerList;
class CameraActor;
class RenderTaskList;
class FrameBuffer;

using FrameBufferPtr = IntrusivePtr<FrameBuffer>;
using ScenePtr       = IntrusivePtr<Scene>;

/**
 * @brief Scene creates a "world" that can be bound to a surface for rendering.
 */
class Scene : public BaseObject, public RenderTaskDefaults
{
public:
  /**
   * @copydoc Dali::Integration::Scene::New
   */
  static ScenePtr New(Size size, int32_t windowOrientation = 0, int32_t screenOrientation = 0);

  /**
   * virtual destructor
   */
  ~Scene() override;

  /**
   * @copydoc Dali::Integration::Scene::Add
   */
  void Add(Actor& actor);

  /**
   * @copydoc Dali::Integration::Scene::Remove
   */
  void Remove(Actor& actor);

  /**
   * @copydoc Dali::Integration::Scene::Show
   */
  void Show();

  /**
   * @copydoc Dali::Integration::Scene::Hide
   */
  void Hide();

  /**
   * @copydoc Dali::Integration::Scene::IsVisible
   */
  bool IsVisible() const;

  /**
   * @copydoc Dali::Integration::Scene::GetSize
   */
  Size GetSize() const;

  /**
   * @copydoc Dali::Integration::Scene::SetDpi
   */
  void SetDpi(Vector2 dpi);

  /**
   * @copydoc Dali::Integration::Scene::GetDpi
   */
  Vector2 GetDpi() const;

  /**
   * @copydoc Dali::Integration::Scene::GetRenderTaskList
   */
  RenderTaskList& GetRenderTaskList() const;

  /**
   * @copydoc Dali::Integration::Scene::GetRootLayer
   */
  Dali::Layer GetRootLayer() const;

  /**
   * @copydoc Dali::Integration::Scene::GetOverlayLayer
   */
  Dali::Layer GetOverlayLayer();

  /**
   * @copydoc Dali::Integration::Scene::GetLayerCount
   */
  uint32_t GetLayerCount() const;

  /**
   * @copydoc Dali::Integration::Scene::GetLayer
   */
  Dali::Layer GetLayer(uint32_t depth) const;

  /**
   * Notify the surface has been resized.
   *
   * @param[in] width The new width of the set surface
   * @param[in] height The new height of the set surface
   */
  void SurfaceResized(float width, float height);

  /**
   * @copydoc Dali::Integration::Scene::SurfaceReplaced
   */
  void SurfaceReplaced();

  /**
   * @copydoc Dali::Integration::Scene::RemoveSceneObject
   */
  void RemoveSceneObject();

  /**
   * @copydoc Dali::Integration::Scene::Discard
   */
  void Discard();

  /**
   * Retrieve the ordered list of on-scene layers.
   * @return The layer-list.
   */
  LayerList& GetLayerList() const;

  /**
   * Request that the depth tree is rebuilt
   */
  void RequestRebuildDepthTree();

  /**
   * This function is called when an event is queued.
   * @param[in] event A event to queue.
   */
  void QueueEvent(const Integration::Event& event);

  /**
   * This function is called by Core when events are processed.
   */
  void ProcessEvents();

  /**
   * This function is called by Core when sending a interrupted event to a specific actor.
   * @param[in] actor The actor on which the event should occur.
   */
  void SendInterruptedEvents(Dali::Internal::Actor* actor);

  /**
   * Rebuilds the depth tree at the end of the event frame if
   * it was requested this frame.
   */
  void RebuildDepthTree();

  /**
   * @brief Sets the background color of the render surface.
   * @param[in] color The new background color
   */
  void SetBackgroundColor(const Vector4& color);

  /**
   * @brief Gets the background color of the render　surface.
   * @return The background color
   */
  Vector4 GetBackgroundColor() const;

  /**
   * @brief Get the Scene scene graph object
   *
   * @return the Scene scene graph object
   */
  SceneGraph::Scene* GetSceneObject() const;

  /**
   * Notify the surface has been rotated.
   * When the device is rotated or the rotation event is received by display manager,
   * this function will be called by window implementation.
   *
   * @param[in] width The width of rotated surface
   * @param[in] height The height of rotated surface
   * @param[in] windowOrientation the current window orientation
   * @param[in] screenOrientation the current screen orientation
   */
  void SurfaceRotated(float width, float height, int32_t windowOrientation, int32_t screenOrientation);

  /**
   * @copydoc Dali::Integration::Scene::SetRotationCompletedAcknowledgement
   */
  void SetRotationCompletedAcknowledgement();

  /**
   * @copydoc Dali::Integration::Scene::IsRotationCompletedAcknowledgementSet
   */
  bool IsRotationCompletedAcknowledgementSet() const;

  /**
   * @copydoc Dali::Integration::Scene::GetCurrentSurfaceOrientation
   */
  int32_t GetCurrentSurfaceOrientation() const;

  /**
   * @copydoc Dali::Integration::Scene::GetCurrentScreenOrientation
   */
  int32_t GetCurrentScreenOrientation() const;

  /**
   * @copydoc Dali::Integration::Scene::GetCurrentSurfaceRect
   */
  const Rect<int32_t>& GetCurrentSurfaceRect() const;

  /**
   * @copydoc Dali::Integration::Scene::GetSurfaceRectChangedCount
   */
  uint32_t GetSurfaceRectChangedCount() const;

  /**
   * @copydoc Dali::Integration::Scene::SetSurfaceRenderTarget
   */
  void SetSurfaceRenderTarget(const Graphics::RenderTargetCreateInfo& renderTargetCreateInfo);

  /**
   * @copydoc Dali::Integration::Scene::SetGeometryHittestEnabled
   */
  void SetGeometryHittestEnabled(bool enabled);

  /**
   * @copydoc Dali::Integration::Scene::IsGeometryHittestEnabled
   */
  bool IsGeometryHittestEnabled() const;

  /**
   * @copydoc Dali::Integration::Scene::GetTouchPropagationType
   */
  Dali::Integration::Scene::TouchPropagationType GetTouchPropagationType() const;

  /**
   * @copydoc Dali::Integration::Scene::SetNativeId
   */
  void SetNativeId(int32_t nativeId);

  /**
   * @copydoc Dali::Integration::Scene::GetNativeId
   */
  int32_t GetNativeId() const;

  /**
   * Used by the EventProcessor to emit key event signals.
   * @param[in] event The key event.
   */
  void EmitKeyEventSignal(const Dali::KeyEvent& event);

  /**
   * Used by the KeyEventProcessor to emit KeyEventGenerated signals.
   * @param[in] event The key event.
   * @return The return is true if KeyEvent is consumed, otherwise false.
   */
  bool EmitKeyEventGeneratedSignal(const Dali::KeyEvent& event);

  /**
   * Used by the KeyEventProcessor to emit InterceptKeyEventSignal signals.
   * @param[in] event The key event.
   * @return The return is true if KeyEvent is consumed, otherwise false.
   */
  bool EmitInterceptKeyEventSignal(const Dali::KeyEvent& event);

  /**
   * Emits the event processing finished signal.
   *
   * @see Dali::Scene::SignalEventProcessingFinished()
   */
  void EmitEventProcessingFinishedSignal();

  /**
   * Emits the touched signal.
   * @param[in] touch The touch event details.
   */
  void EmitTouchedSignal(const Dali::TouchEvent& touch);

  /**
   * Used by the EventProcessor to emit wheel event signals.
   * @param[in] event The wheel event.
   */
  void EmitWheelEventSignal(const Dali::WheelEvent& event);

  /**
   * Used by the WheelEventProcessor to emit WheelEventGenerated signals.
   * @param[in] event The wheel event.
   * @return The return is true if WheelEvent is consumed, otherwise false.
   */
  bool EmitWheelEventGeneratedSignal(const Dali::WheelEvent& event);

  /**
   * @copydoc Dali::Integration::Scene::AddFrameRenderedCallback
   */
  void AddFrameRenderedCallback(std::unique_ptr<CallbackBase> callback, int32_t frameId);

  /**
   * @copydoc Dali::Integration::Scene::AddFramePresentedCallback
   */
  void AddFramePresentedCallback(std::unique_ptr<CallbackBase> callback, int32_t frameId);

  /**
   * @copydoc Dali::Integration::Scene::GetFrameRenderedCallback
   */
  void GetFrameRenderedCallback(Dali::Integration::Scene::FrameCallbackContainer& callbacks);

  /**
   * @copydoc Dali::Integration::Scene::GetFramePresentedCallback
   */
  void GetFramePresentedCallback(Dali::Integration::Scene::FrameCallbackContainer& callbacks);

  /**
   * @copydoc Dali::Integration::Scene::KeepRendering
   */
  void KeepRendering(float durationSeconds);

  /**
   * @copydoc Dali::Integration::Scene::SetPartialUpdateEnabled()
   */
  void SetPartialUpdateEnabled(bool enabled);

  /**
   * @copydoc Dali::Integration::Scene::IsPartialUpdateEnabled()
   */
  bool IsPartialUpdateEnabled() const;

  /**
   * @copydoc Integration::Scene::KeyEventSignal()
   */
  Integration::Scene::KeyEventSignalType& KeyEventSignal();

  /**
   * @copydoc Integration::Scene::KeyEventGeneratedSignal()
   */
  Integration::Scene::KeyEventGeneratedSignalType& KeyEventGeneratedSignal();

  /**
   * @copydoc Integration::Scene::InterceptKeyEventSignal()
   */
  Integration::Scene::KeyEventGeneratedSignalType& InterceptKeyEventSignal();

  /**
   * @copydoc Integration::Scene::SignalEventProcessingFinished()
   */
  Integration::Scene::EventProcessingFinishedSignalType& EventProcessingFinishedSignal();

  /**
    * @copydoc Integration::Scene::TouchedSignal()
    */
  Integration::Scene::TouchEventSignalType& TouchedSignal();

  /**
   * @copydoc Integration::Scene::sWheelEventSignal()
   */
  Integration::Scene::WheelEventSignalType& WheelEventSignal();

  /**
   * @copydoc Integration::Scene::WheelEventGeneratedSignal()
   */
  Integration::Scene::WheelEventGeneratedSignalType& WheelEventGeneratedSignal();

public:
  /**
   * From RenderTaskDefaults; retrieve the default root actor.
   * @return The default root actor.
   */
  Actor& GetDefaultRootActor() override;

  /**
   * From RenderTaskDefaults; retrieve the default camera actor.
   * @return The default camera actor.
   */
  CameraActor& GetDefaultCameraActor() const override;

private:
  // Constructor
  Scene();

  /**
   * Second-phase constructor.
   *
   * @param[in] size The size of the set surface
   * @param[in] windowOrientation The rotated angle of the set surface for this scene
   * @param[in] screenOrientation The rotated angle of the screen
   */
  void Initialize(Size size, int32_t windowOrientation, int32_t screenOrientation);

  // Undefined
  Scene(const Scene&) = delete;

  // Undefined
  Scene& operator=(const Scene& rhs) = delete;

  /**
   * Informs the scene that the set surface has been resized or rotated.
   *
   * @param[in] width The width of rotated surface
   * @param[in] height The height of rotated surface
   * @param[in] windowOrientation the current window orientation
   * @param[in] screenOrientation the current screen orientation
   */
  void ChangedSurface(float width, float height, int32_t windowOrientation, int32_t screenOrientation);

private:
  Internal::SceneGraph::Scene* mSceneObject;

  Size mSize;

  Vector2 mDpi;

  Vector4 mBackgroundColor;

  LayerPtr mRootLayer;

  LayerPtr mOverlayLayer;

  // Ordered list of currently on-stage layers
  OwnerPointer<LayerList> mLayerList;

  IntrusivePtr<CameraActor> mDefaultCamera;

  // The list of render-tasks
  IntrusivePtr<RenderTaskList> mRenderTaskList;

  bool mDepthTreeDirty : 1;       ///< True if the depth tree needs recalculating
  bool mPartialUpdateEnabled : 1; ///< True if the partial update is enabled
  bool mGeometryHittest : 1;      ///< True if the geometry hittest is enabled
  bool mIsVisible : 1;            ///< True if this Scene is visible

  EventProcessor mEventProcessor;

  // The Surface's orientation
  int32_t mSurfaceOrientation;

  // The Screen's orientation
  int32_t mScreenOrientation;

  // The native window id
  int32_t mNativeId;

  // The key event signal
  Integration::Scene::KeyEventSignalType          mKeyEventSignal;
  Integration::Scene::KeyEventGeneratedSignalType mKeyEventGeneratedSignal;
  Integration::Scene::KeyEventGeneratedSignalType mInterceptKeyEventSignal;

  // The event processing finished signal
  Integration::Scene::EventProcessingFinishedSignalType mEventProcessingFinishedSignal;

  // The touch signal
  Integration::Scene::TouchEventSignalType mTouchedSignal;

  // The wheel event signal
  Integration::Scene::WheelEventSignalType          mWheelEventSignal;
  Integration::Scene::WheelEventGeneratedSignalType mWheelEventGeneratedSignal;
};

} // namespace Internal

// Get impl of handle
inline Internal::Scene& GetImplementation(Dali::Integration::Scene& scene)
{
  DALI_ASSERT_ALWAYS(scene && "Scene handle is empty");
  Dali::RefObject& object = scene.GetBaseObject();
  return static_cast<Internal::Scene&>(object);
}

inline const Internal::Scene& GetImplementation(const Dali::Integration::Scene& scene)
{
  DALI_ASSERT_ALWAYS(scene && "Scene handle is empty");
  const Dali::RefObject& object = scene.GetBaseObject();
  return static_cast<const Internal::Scene&>(object);
}

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_H
