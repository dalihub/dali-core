#ifndef DALI_INTERNAL_STAGE_H
#define DALI_INTERNAL_STAGE_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/common/stage.h>
#include <dali/devel-api/common/stage-devel.h>
#include <dali/public-api/object/base-object.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/event/actors/layer-impl.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/common/object-registry-impl.h>
#include <dali/internal/event/common/stage-def.h>
#include <dali/internal/event/render-tasks/render-task-defaults.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/public-api/common/view-mode.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/render-tasks/render-task.h>

namespace Dali
{

struct Vector2;

namespace Integration
{
class SystemOverlay;
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
class SystemOverlay;
class CameraActor;
class RenderTaskList;

/**
 * Implementation of Stage
 */
class Stage : public BaseObject, public RenderTaskDefaults, public EventThreadServices
{
public:

  /**
   * Create the stage
   * @param[in] playlist for animations
   * @param[in] propertyNotificationManager
   * @param[in] updateManager
   * @param[in] notificationManager
   * @param[in] renderController
   */
  static StagePtr New( AnimationPlaylist& playlist,
                       PropertyNotificationManager& propertyNotificationManager,
                       SceneGraph::UpdateManager& updateManager,
                       NotificationManager& notificationManager,
                       Integration::RenderController& renderController );

  /**
   * Initialize the stage.
   * @param[in] renderToFbo Whether to render into a Frame Buffer Object.
   */
  void Initialize( bool renderToFbo );

  /**
   * Uninitialize the stage.
   */
  void Uninitialize();

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

  /**
   * Returns the animation playlist.
   * @return reference to the animation playlist.
   */
  AnimationPlaylist& GetAnimationPlaylist();

  /**
   * Returns the property notification manager.
   * @return reference to the property notification manager.
   */
  PropertyNotificationManager& GetPropertyNotificationManager();

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
   * Used to calculate the size of the stage and indirectly, the root actor.
   * @param [in] width  The new surface width.
   * @param [in] height The new surface height.
   */
  void SurfaceResized( float width, float height );

  /**
   * Sets the top margin size.
   * Available stage size is reduced by this size.
   * The stage is located below the size at the top of the display
   * initial size is zero before it is assigned
   * @param[in] margin margin size
   */
  void SetTopMargin( unsigned int margin );

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
   * Create a default camera actor
   */
  void CreateDefaultCameraActor();

  /**
   * Set position of default camera for current stage size
   */
  void SetDefaultCameraPosition();

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
  unsigned int GetLayerCount() const;

  /**
   * @copydoc Dali::Stage::GetLayer()
   */
  Dali::Layer GetLayer( unsigned int depth ) const;

  /**
   * @copydoc Dali::Stage::GetRootLayer()
   */
  Dali::Layer GetRootLayer() const;

  /**
   * Retrieve the ordered list of on-stage layers.
   * @return The layer-list.
   */
  LayerList& GetLayerList();

  // System-level overlay actors

  /**
   * @copydoc Dali::Integration::Core::GetSystemOverlay()
   */
  Integration::SystemOverlay& GetSystemOverlay();

  /**
   * Retrieve the internal implementation of the SystemOverlay.
   * @return The implementation, or NULL if this has never been requested from Integration API.
   */
  SystemOverlay* GetSystemOverlayInternal();

  // Stereoscopy

  /**
   * @copydoc Dali::Integration::Core::SetViewMode()
   */
  void SetViewMode( ViewMode viewMode );

  /**
   * @copydoc Dali::Integration::Core::GetViewMode()
   */
  ViewMode GetViewMode() const;

  /**
   * @copydoc Dali::Integration::Core::SetStereoBase()
   */
  void SetStereoBase( float stereoBase );

  /**
   * @copydoc Dali::Integration::Core::GetStereoBase()
   */
  float GetStereoBase() const;

  // Keyboard stuff

  /**
   * As one virtual keyboard per stage, the stage will hold a pointer to the Actor currently
   * set to receive keyboard input.
   * @param[in] actor to receive keyboard input
   */
  void SetKeyboardFocusActor( Actor* actor );

  /**
   * Get the actor that is currently set to receive keyboard inputs
   * @return Pointer to the actor set to receive keyboard inputs.
   */
  Actor* GetKeyboardFocusActor() const;

  /**
   * Removes the given actor from keyboard focus so it will no longer receive key events from keyboard.
   * @param [in] actor which should be removed from focus.
   */
  void RemoveActorFromKeyFocus( Actor* actor );

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
   * Sets horizontal and vertical pixels per inch value that is used by the display
   * @param[in] dpi Horizontal and vertical dpi value
   */
  void SetDpi( Vector2 dpi );

  NotificationManager& GetNotificationManager()
  {
    return mNotificationManager;
  }

  /**
   * @copydoc Dali::Stage::KeepRendering()
   */
  void KeepRendering( float durationSeconds );

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
   * Connects a callback function with the object's signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

public: // Implementation of EventThreadServices

  /**
   * @copydoc EventThreadServices::RegisterObject
   */
  virtual void RegisterObject( BaseObject* object);

  /**
   * @copydoc EventThreadServices::UnregisterObject
   */
  virtual void UnregisterObject( BaseObject* object);

  /**
   * @copydoc EventThreadServices::GetUpdateManager
   */
  virtual SceneGraph::UpdateManager& GetUpdateManager();

  /**
   * @copydoc EventThreadServices::GetRenderController
   */
  virtual Integration::RenderController& GetRenderController();

  /**
   * @copydoc EventThreadServices::ReserveMessageSlot
   */
  virtual unsigned int* ReserveMessageSlot( std::size_t size, bool updateScene );

  /**
   * @copydoc EventThreadServices::GetEventBufferIndex
   */
  virtual BufferIndex GetEventBufferIndex() const;

  /**
   * @copydoc EventThreadServices::ForceNextUpdate
   */
  virtual void ForceNextUpdate();

  /**
   * @copydoc EventThreadServices::IsNextUpdateForced
   */
  virtual bool IsNextUpdateForced();

  /**
   * Request that the depth tree is rebuilt
   */
  void RequestRebuildDepthTree();

  /**
   * Rebuilds the depth tree at the end of the event frame if
   * it was requested this frame.
   */
  void RebuildDepthTree();

private:

  /**
   * Protected constructor; see also Stage::New()
   */
  Stage( AnimationPlaylist& playlist,
         PropertyNotificationManager& propertyNotificationManager,
         SceneGraph::UpdateManager& updateManager,
         NotificationManager& notificationManager,
         Integration::RenderController& renderController );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Stage();

private:

  // For 'Fire and forget' animation support
  AnimationPlaylist& mAnimationPlaylist;

  PropertyNotificationManager& mPropertyNotificationManager;

  SceneGraph::UpdateManager& mUpdateManager;

  NotificationManager& mNotificationManager;

  Integration::RenderController& mRenderController;

  // The stage-size may be less than surface-size (reduced by top-margin)
  Vector2 mSize;
  Vector2 mSurfaceSize;

  // Cached for public GetBackgroundColor()
  Vector4 mBackgroundColor;

  LayerPtr mRootLayer;

  // Ordered list of currently on-stage layers
  OwnerPointer<LayerList> mLayerList;

  IntrusivePtr<CameraActor> mDefaultCamera;

  ViewMode mViewMode;
  float mStereoBase;

  unsigned int mTopMargin;
  Vector2 mDpi;

  // The object registry
  ObjectRegistryPtr mObjectRegistry;

  // The list of render-tasks
  IntrusivePtr<RenderTaskList> mRenderTaskList;

  Dali::RenderTask mRightRenderTask;
  IntrusivePtr<CameraActor> mRightCamera;
  Dali::RenderTask mLeftRenderTask;
  IntrusivePtr<CameraActor> mLeftCamera;

  Integration::SystemOverlay* mSystemOverlay; ///< SystemOverlay stage access

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

  bool mDepthTreeDirty:1;  ///< True if the depth tree needs recalculating
  bool mForceNextUpdate:1; ///< True if the next rendering is really required.
  bool mRenderToFbo:1;     ///< Whether to render to a Frame Buffer Object.
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
