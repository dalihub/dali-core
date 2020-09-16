#ifndef DALI_INTERNAL_SCENE_H
#define DALI_INTERNAL_SCENE_H

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
#include <dali/integration-api/scene.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/actors/layer.h>
#include <dali/public-api/render-tasks/render-task-list.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/events/event-processor.h>
#include <dali/internal/event/render-tasks/render-task-defaults.h>

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
}

class EventProcessor;
class Layer;
class LayerList;
class CameraActor;
class RenderTaskList;
class FrameBuffer;

using FrameBufferPtr = IntrusivePtr<FrameBuffer>;
using ScenePtr = IntrusivePtr<Scene>;

/**
 * @brief Scene creates a "world" that can be bound to a surface for rendering.
 */
class Scene : public BaseObject, public RenderTaskDefaults
{

public:
  /**
   * @copydoc Dali::Integration::Scene::New
   */
  static ScenePtr New( Size size );

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
   * @copydoc Dali::Integration::Scene::GetSize
   */
  Size GetSize() const;

  /**
   * @copydoc Dali::Integration::Scene::SetDpi
   */
  void SetDpi( Vector2 dpi );

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
  void SurfaceResized( float width, float height );

  /**
   * @copydoc Dali::Integration::Scene::SurfaceReplaced
   */
  void SurfaceReplaced();

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
  void QueueEvent( const Integration::Event& event );

  /**
   * This function is called by Core when events are processed.
   */
  void ProcessEvents();

  /**
   * Rebuilds the depth tree at the end of the event frame if
   * it was requested this frame.
   */
  void RebuildDepthTree();

  /**
   * @brief Sets the background color of the render surface.
   * @param[in] color The new background color
   */
  void SetBackgroundColor( const Vector4& color );

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
   * Emits the event processing finished signal.
   *
   * @see Dali::Scene::SignalEventProcessingFinished()
   */
  void EmitEventProcessingFinishedSignal();

  /**
   * Emits the touched signal.
   * @param[in] touch The touch event details.
   */
  void EmitTouchedSignal( const Dali::TouchEvent& touch );

  /**
   * Used by the EventProcessor to emit wheel event signals.
   * @param[in] event The wheel event.
   */
  void EmitWheelEventSignal( const Dali::WheelEvent& event );

  /**
   * @copydoc Dali::Integration::Scene::AddFrameRenderedCallback
   */
  void AddFrameRenderedCallback( std::unique_ptr< CallbackBase > callback, int32_t frameId );

  /**
   * @copydoc Dali::Integration::Scene::AddFramePresentedCallback
   */
  void AddFramePresentedCallback( std::unique_ptr< CallbackBase > callback, int32_t frameId );

  /**
   * @copydoc Dali::Integration::Scene::GetFrameRenderedCallback
   */
  void GetFrameRenderedCallback( Dali::Integration::Scene::FrameCallbackContainer& callbacks );

  /**
   * @copydoc Dali::Integration::Scene::GetFramePresentedCallback
   */
  void GetFramePresentedCallback( Dali::Integration::Scene::FrameCallbackContainer& callbacks );

  /**
   * @copydoc Integration::Scene::KeyEventSignal()
   */
  Integration::Scene::KeyEventSignalType& KeyEventSignal();

    /**
   * @copydoc Integration::Scene::KeyEventGeneratedSignal()
   */
  Integration::Scene::KeyEventGeneratedSignalType& KeyEventGeneratedSignal();

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
  CameraActor& GetDefaultCameraActor() override;

private:

  // Constructor
  Scene();

  /**
   * Second-phase constructor.
   *
   * @param[in] size The size of the set surface
   */
  void Initialize( Size size );

  // Undefined
  Scene(const Scene&) = delete;

  // Undefined
  Scene& operator=(const Scene& rhs) = delete;

private:
  Internal::SceneGraph::Scene* mSceneObject;

  Size mSize;

  Vector2 mDpi;

  Vector4 mBackgroundColor;

  LayerPtr mRootLayer;

  // Ordered list of currently on-stage layers
  OwnerPointer<LayerList> mLayerList;

  IntrusivePtr<CameraActor> mDefaultCamera;

  // The list of render-tasks
  IntrusivePtr<RenderTaskList> mRenderTaskList;

  bool mDepthTreeDirty:1;  ///< True if the depth tree needs recalculating

  EventProcessor mEventProcessor;

  // The key event signal
  Integration::Scene::KeyEventSignalType mKeyEventSignal;
  Integration::Scene::KeyEventGeneratedSignalType   mKeyEventGeneratedSignal;

  // The event processing finished signal
  Integration::Scene::EventProcessingFinishedSignalType mEventProcessingFinishedSignal;

  // The touch signal
  Integration::Scene::TouchEventSignalType mTouchedSignal;

  // The wheel event signal
  Integration::Scene::WheelEventSignalType mWheelEventSignal;
};

} // Internal

// Get impl of handle
inline Internal::Scene& GetImplementation(Dali::Integration::Scene& scene)
{
  DALI_ASSERT_ALWAYS( scene && "Scene handle is empty" );
  Dali::RefObject& object = scene.GetBaseObject();
  return static_cast<Internal::Scene&>(object);
}

inline const Internal::Scene& GetImplementation(const Dali::Integration::Scene& scene)
{
  DALI_ASSERT_ALWAYS( scene && "Scene handle is empty" );
  const Dali::RefObject& object = scene.GetBaseObject();
  return static_cast<const Internal::Scene&>(object);
}

} // Dali

#endif // DALI_INTERNAL_SCENE_H
