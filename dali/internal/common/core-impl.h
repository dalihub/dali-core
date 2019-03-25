#ifndef DALI_INTERNAL_CORE_H
#define DALI_INTERNAL_CORE_H

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
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/integration-api/context-notifier.h>
#include <dali/integration-api/core-enumerations.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/devel-api/common/owner-container.h>
#include <dali/internal/event/animation/animation-playlist-declarations.h>
#include <dali/internal/event/common/stage-def.h>
#include <dali/integration-api/resource-policies.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/common/object-registry-impl.h>

namespace Dali
{

namespace Integration
{
class Processor;
class RenderController;
class PlatformAbstraction;
class GestureManager;
class GlAbstraction;
class GlSyncAbstraction;
class UpdateStatus;
class RenderStatus;
class RenderSurface;
struct Event;
struct TouchData;
}

namespace Internal
{

class NotificationManager;
class AnimationPlaylist;
class PropertyNotificationManager;
class EventProcessor;
class GestureEventProcessor;
class ShaderFactory;
class TouchResampler;
class RelayoutController;
class EventThreadServices;

namespace SceneGraph
{
class UpdateManager;
class RenderManager;
class DiscardQueue;
class RenderTaskProcessor;
}

/**
 * Internal class for Dali::Integration::Core
 */
class Core : public EventThreadServices
{
public:

  /**
   * Create and initialise a new Core instance
   */
  Core( Integration::RenderController& renderController,
        Integration::PlatformAbstraction& platform,
        Integration::GlAbstraction& glAbstraction,
        Integration::GlSyncAbstraction& glSyncAbstraction,
        Integration::GestureManager& gestureManager,
        ResourcePolicy::DataRetention dataRetentionPolicy,
        Integration::RenderToFrameBuffer renderToFboEnabled,
        Integration::DepthBufferAvailable depthBufferAvailable,
        Integration::StencilBufferAvailable stencilBufferAvailable );

  /**
   * Destructor
   */
  ~Core();

  /**
   * @copydoc Dali::Integration::Core::Initialize()
   */
  void Initialize();

  /**
   * @copydoc Dali::Integration::Core::GetContextNotifier()
   */
  Integration::ContextNotifierInterface* GetContextNotifier();

  /**
   * @copydoc Dali::Integration::Core::ContextCreated()
   */
  void ContextCreated();

  /**
   * @copydoc Dali::Integration::Core::ContextDestroyed()
   */
  void ContextDestroyed();

  /**
   * @copydoc Dali::Integration::Core::RecoverFromContextLoss()
   */
  void RecoverFromContextLoss();

  /**
   * @copydoc Dali::Integration::Core::SurfaceResized(Integration::RenderSurface*)
   */
  void SurfaceResized( Integration::RenderSurface* surface );

  /**
   * @copydoc Dali::Integration::Core::SetMinimumFrameTimeInterval(uint32_t)
   */
  void SetMinimumFrameTimeInterval(uint32_t interval);

  /**
   * @copydoc Dali::Integration::Core::Update()
   */
  void Update( float elapsedSeconds, uint32_t lastVSyncTimeMilliseconds, uint32_t nextVSyncTimeMilliseconds, Integration::UpdateStatus& status, bool renderToFboEnabled, bool isRenderingToFbo );

  /**
   * @copydoc Dali::Integration::Core::Render()
   */
  void Render( Integration::RenderStatus& status, bool forceClear );

  /**
   * @copydoc Dali::Integration::Core::SceneCreated()
   */
  void SceneCreated();

  /**
   * @copydoc Dali::Integration::Core::QueueEvent(const Integration::Event&)
   */
  void QueueEvent( const Integration::Event& event );

  /**
   * @copydoc Dali::Integration::Core::ProcessEvents()
   */
  void ProcessEvents();

  /**
   * @copydoc Dali::Integration::Core::GetMaximumUpdateCount()
   */
  uint32_t GetMaximumUpdateCount() const;

  /**
   * @copydoc Dali::Integration::Core::RegisterProcessor
   */
  void RegisterProcessor( Dali::Integration::Processor& processor );

  /**
   * @copydoc Dali::Integration::Core::UnregisterProcessor
   */
  void UnregisterProcessor( Dali::Integration::Processor& processor );

  /**
   * @copydoc Dali::Internal::ThreadLocalStorage::AddScene()
   */
  void AddScene( Scene* scene );

  /**
   * @copydoc Dali::Internal::ThreadLocalStorage::RemoveScene()
   */
  void RemoveScene( Scene* scene );

public: // Implementation of EventThreadServices

  /**
   * @copydoc EventThreadServices::RegisterObject
   */
  void RegisterObject( BaseObject* object) override;

  /**
   * @copydoc EventThreadServices::UnregisterObject
   */
  void UnregisterObject( BaseObject* object) override;

  /**
   * @copydoc EventThreadServices::GetUpdateManager
   */
  SceneGraph::UpdateManager& GetUpdateManager() override;

  /**
   * @copydoc EventThreadServices::GetRenderController
   */
  Integration::RenderController& GetRenderController() override;

  /**
   * @copydoc EventThreadServices::ReserveMessageSlot
   */
  uint32_t* ReserveMessageSlot( uint32_t size, bool updateScene ) override;

  /**
   * @copydoc EventThreadServices::GetEventBufferIndex
   */
  BufferIndex GetEventBufferIndex() const override;

  /**
   * @copydoc EventThreadServices::ForceNextUpdate
   */
  void ForceNextUpdate() override;

  /**
   * @copydoc EventThreadServices::IsNextUpdateForced
   */
  bool IsNextUpdateForced() override;

private:
  /**
   * Run each registered processor
   */
  void RunProcessors();

  // for use by ThreadLocalStorage

  /**
   * Returns the current stage.
   * @return A smart-pointer to the current stage.
   */
  StagePtr GetCurrentStage();

  /**
   * Returns the platform abstraction.
   * @return A reference to the platform abstraction.
   */
  Integration::PlatformAbstraction& GetPlatform();

  /**
   * Returns the render manager.
   * @return A reference to the render manager.
   */
  SceneGraph::RenderManager& GetRenderManager();

  /**
   * Returns the notification manager.
   * @return A reference to the Notification Manager.
   */
  NotificationManager& GetNotificationManager();

  /**
   * Returns the Shader factory
   * @return A reference to the Shader binary factory.
   */
  ShaderFactory& GetShaderFactory();

  /**
   * Returns the gesture event processor.
   * @return A reference to the gesture event processor.
   */
  GestureEventProcessor& GetGestureEventProcessor();

  /**
   * Return the relayout controller
   * @Return Return a reference to the relayout controller
   */
  RelayoutController& GetRelayoutController();

  /**
   * @brief Gets the Object registry.
   * @return A reference to the object registry
   */
  ObjectRegistry& GetObjectRegistry() const;

  /**
   * @brief Gets the event thread services.
   * @return A reference to the event thread services
   */
  EventThreadServices& GetEventThreadServices();

  /**
   * @brief Gets the property notification manager.
   * @return A reference to the property notification manager
   */
  PropertyNotificationManager& GetPropertyNotificationManager() const;

  /**
   * @brief Gets the animation play list.
   * @return A reference to the animation play list
   */
  AnimationPlaylist& GetAnimationPlaylist() const;

private:

  /**
   * Undefined copy and assignment operators
   */
  Core(const Core& core) = delete;  // No definition
  Core& operator=(const Core& core) = delete;  // No definition

  /**
   * Create Thread local storage
   */
  void CreateThreadLocalStorage();

private:

  Integration::RenderController&            mRenderController;            ///< Reference to Render controller to tell it to keep rendering
  Integration::PlatformAbstraction&         mPlatform;                    ///< The interface providing platform specific services.

  IntrusivePtr<Stage>                       mStage;                       ///< The current stage
  AnimationPlaylistOwner                    mAnimationPlaylist;           ///< For 'Fire and forget' animation support
  OwnerPointer<PropertyNotificationManager> mPropertyNotificationManager; ///< For safe signal emmision of property changed notifications
  IntrusivePtr< RelayoutController >        mRelayoutController;          ///< Size negotiation relayout controller

  OwnerPointer<SceneGraph::RenderTaskProcessor> mRenderTaskProcessor;         ///< Handles the processing of render tasks
  OwnerPointer<SceneGraph::RenderManager>       mRenderManager;               ///< Render manager
  OwnerPointer<SceneGraph::UpdateManager>       mUpdateManager;               ///< Update manager
  OwnerPointer<SceneGraph::DiscardQueue>        mDiscardQueue;                ///< Used to cleanup nodes & resources when no longer in use.
  OwnerPointer<ShaderFactory>                   mShaderFactory;               ///< Shader resource factory
  OwnerPointer<NotificationManager>             mNotificationManager;         ///< Notification manager
  OwnerPointer<GestureEventProcessor>           mGestureEventProcessor;       ///< The gesture event processor
  Dali::Vector<Integration::Processor*>         mProcessors;                  ///< Registered processors (not owned)

  std::vector<ScenePtr>                         mScenes;                      ///< A container of scenes that bound to a surface for rendering, owned by Core

  // The object registry
  ObjectRegistryPtr                             mObjectRegistry;

  bool                                      mProcessingEvent  : 1;        ///< True during ProcessEvents()
  bool                                      mForceNextUpdate:1;           ///< True if the next rendering is really required.

  friend class ThreadLocalStorage;

};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_CORE_H
