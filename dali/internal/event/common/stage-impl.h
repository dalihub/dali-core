#ifndef DALI_INTERNAL_STAGE_H
#define DALI_INTERNAL_STAGE_H

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

// INTERNAL INCLUDES
#include <dali/public-api/common/stage.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/object/base-object.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/devel-api/common/stage-devel.h>
#include <dali/integration-api/context-notifier.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/common/object-registry-impl.h>
#include <dali/internal/event/common/stage-def.h>
#include <dali/internal/event/render-tasks/render-task-defaults.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/event/render-tasks/render-task-impl.h>

namespace Dali
{

struct Vector2;

namespace Integration
{
class RenderController;
}

namespace Internal
{

namespace SceneGraph
{
class UpdateManager;
}

class AnimationPlaylist;
class PropertyNotificationManager;
class Layer;
class LayerList;
class CameraActor;
class RenderTaskList;
class Scene;

/**
 * Implementation of Stage
 */
class Stage : public BaseObject, public RenderTaskDefaults, public Integration::ContextNotifierInterface, public ConnectionTracker
{
public:

  /**
   * Create the stage
   * @param[in] updateManager
   */
  static StagePtr New( SceneGraph::UpdateManager& updateManager );

  /**
   * Initialize the stage.
   * @param[in] scene The default scene (for main window).
   */
  void Initialize( Scene& scene );

  /**
   * @copydoc Dali::Stage::GetCurrent()
   * @note this version is for internal usage so it does not assert
   */
  static StagePtr GetCurrent();

  /**
   * @copydoc Dali::Stage::IsInstalled().
   */
  static bool IsInstalled();

  /**
   * @copydoc Dali::Stage::GetObjectRegistry()
   */
  ObjectRegistry& GetObjectRegistry();

  /**
   * Retrieve the root actor (not publically accessible).
   * @return The root actor.
   */
  Layer& GetRootActor();

  // Root actor accessors

  /**
   * @copydoc Dali::Stage::Add()
   */
  void Add( Actor& actor );

  /**
   * @copydoc Dali::Stage::Remove()
   */
  void Remove( Actor& actor );

  /**
   * Returns the size of the Stage in pixels as a Vector.
   * The x component will be the width of the Stage in pixels
   * The y component will be the height of the Stage in pixels
   * @return The size of the Stage as a Vector.
   */
  Vector2 GetSize() const;

  /**
   * @copydoc Dali::Stage::GetRenderTaskList()
   */
  RenderTaskList& GetRenderTaskList() const;

  /**
   * From RenderTaskDefaults; retrieve the default root actor.
   * @return The default root actor.
   */
  virtual Actor& GetDefaultRootActor();

  /**
   * From RenderTaskDefaults; retrieve the default camera actor.
   * @return The default camera actor.
   */
  virtual CameraActor& GetDefaultCameraActor();

  // Layers

  /**
   * @copydoc Dali::Stage::GetLayerCount()
   */
  uint32_t GetLayerCount() const;

  /**
   * @copydoc Dali::Stage::GetLayer()
   */
  Dali::Layer GetLayer( uint32_t depth ) const;

  /**
   * @copydoc Dali::Stage::GetRootLayer()
   */
  Dali::Layer GetRootLayer() const;

  /**
   * Retrieve the ordered list of on-stage layers.
   * @return The layer-list.
   */
  LayerList& GetLayerList();

  // Misc

  /**
   * @copydoc Dali::Stage::SetBackgroundColor
   */
  void SetBackgroundColor(Vector4 color);

  /**
   * @copydoc Dali::Stage::GetBackgroundColor
   */
  Vector4 GetBackgroundColor() const;

  /**
   * @copydoc Dali::Stage::GetDpi
   */
  Vector2 GetDpi() const;

  /**
   * @copydoc Dali::Stage::KeepRendering()
   */
  void KeepRendering( float durationSeconds );

  /**
   * @copydoc Dali::DevelStage::SetRenderingBehavior()
   */
  void SetRenderingBehavior( DevelStage::Rendering renderingBehavior );

  /**
   * @copydoc Dali::DevelStage::GetRenderingBehavior()
   */
  DevelStage::Rendering GetRenderingBehavior() const;

  /**
   * Callback for Internal::Scene EventProcessingFinished signal
   */
  void OnEventProcessingFinished();

  /**
   * Callback for Internal::Scene KeyEventSignal signal
   */
  void OnKeyEvent( const Dali::KeyEvent& event );

  /**
   * Callback for Internal::Scene TouchedEventSignal signal
   */
  void OnTouchedEvent( const Dali::TouchEvent& touch );

  /**
   * Callback for Internal::Scene TouchSignal signal
   */
  void OnTouchEvent( const Dali::TouchData& touch );

  /**
   * Callback for Internal::Scene WheelEventSignal signal
   */
  void OnWheelEvent( const Dali::WheelEvent& event );

  /**
   * Used by the EventProcessor to emit key event signals.
   * @param[in] event The key event.
   */
  void EmitKeyEventSignal(const KeyEvent& event);

  /**
   * Used by the KeyEventProcessor to emit KeyEventGenerated signals.
   * @param[in] event The key event.
   * @return The return is true if KeyEvent is consumed, otherwise false.
   */
  bool EmitKeyEventGeneratedSignal(const KeyEvent& event);

  /**
   * Emits the event processing finished signal.
   *
   * @see Dali::Stage::SignalEventProcessingFinished()
   */
  void EmitEventProcessingFinishedSignal();

  /**
   * Emits the touched signal.
   * @param[in] touchEvent The touch event details (Old API).
   * @param[in] touch The touch event details.
   */
  void EmitTouchedSignal( const TouchEvent& touchEvent, const Dali::TouchData& touch );

  /**
   * Used by the EventProcessor to emit wheel event signals.
   * @param[in] event The wheel event.
   */
  void EmitWheelEventSignal( const WheelEvent& event );

  /**
   * Emits the scene created.
   */
  void EmitSceneCreatedSignal();

  /**
   * @copydoc Dali::Stage::KeyEventSignal()
   */
  Dali::Stage::KeyEventSignalType& KeyEventSignal();

  /**
   * @copydoc Dali::Stage::SignalEventProcessingFinished()
   */
  Dali::Stage::EventProcessingFinishedSignalType& EventProcessingFinishedSignal();

  /**
    * @copydoc Dali::Stage::TouchedSignal()
    */
  Dali::Stage::TouchedSignalType& TouchedSignal();

  /**
    * @copydoc Dali::Stage::TouchSignal()
    */
  Dali::Stage::TouchSignalType& TouchSignal();

  /**
   * @copydoc Dali::Stage::WheelEventSignal()
   */
  Dali::Stage::WheelEventSignalType& WheelEventSignal();

  /**
   * @copydoc Dali::Stage::ContextLostSignal()
   */
  Dali::Stage::ContextStatusSignal& ContextLostSignal();

  /**
   * @copydoc Dali::Stage::ContextRegainedSignal()
   */
  Dali::Stage::ContextStatusSignal& ContextRegainedSignal();

  /**
   * @copydoc Dali::Stage::SceneCreatedSignal()
   */
  Dali::Stage::SceneCreatedSignalType& SceneCreatedSignal();

  /**
   * @copydoc Dali::DevelStage::KeyEventGeneratedSignal()
   */
  Dali::DevelStage::KeyEventGeneratedSignalType& KeyEventGeneratedSignal();

  /**
   * @copydoc Dali::DevelStage::AddFrameCallback()
   */
  void AddFrameCallback( FrameCallbackInterface& frameCallback, Actor& rootActor );

  /**
   * @copydoc Dali::DevelStage::RemoveFrameCallback()
   */
  void RemoveFrameCallback( FrameCallbackInterface& frameCallback );

  /**
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

private: // Implementation of ContextNotificationInterface:

  /**
   * @copydoc Dali::Integration::NotifyContextLost();
   */
  virtual void NotifyContextLost();

  /**
   * @copydoc Dali::Integration::NotifyContextRegained();
   */
  virtual void NotifyContextRegained();

private:

  /**
   * Protected constructor; see also Stage::New()
   */
  Stage( SceneGraph::UpdateManager& updateManager );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Stage();

private:

  SceneGraph::UpdateManager& mUpdateManager;

  IntrusivePtr<Scene> mScene;

  // The key event signal
  Dali::Stage::KeyEventSignalType                 mKeyEventSignal;
  Dali::DevelStage::KeyEventGeneratedSignalType   mKeyEventGeneratedSignal;

  // The event processing finished signal
  Dali::Stage::EventProcessingFinishedSignalType  mEventProcessingFinishedSignal;

  // The touched signals
  Dali::Stage::TouchedSignalType                  mTouchedSignal;
  Dali::Stage::TouchSignalType                    mTouchSignal;

  // The wheel event signal
  Dali::Stage::WheelEventSignalType               mWheelEventSignal;

  Dali::Stage::ContextStatusSignal mContextLostSignal;
  Dali::Stage::ContextStatusSignal mContextRegainedSignal;

  Dali::Stage::SceneCreatedSignalType mSceneCreatedSignal;

  DevelStage::Rendering mRenderingBehavior; ///< The rendering behavior
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::Stage& GetImplementation(Dali::Stage& stage)
{
  DALI_ASSERT_ALWAYS( stage && "Stage handle is empty" );

  BaseObject& handle = stage.GetBaseObject();

  return static_cast<Internal::Stage&>(handle);
}

inline const Internal::Stage& GetImplementation(const Dali::Stage& stage)
{
  DALI_ASSERT_ALWAYS( stage && "Stage handle is empty" );

  const BaseObject& handle = stage.GetBaseObject();

  return static_cast<const Internal::Stage&>(handle);
}

} // namespace Dali

#endif // DALI_INTERNAL_STAGE_H
