#ifndef DALI_INTERNAL_CORE_H
#define DALI_INTERNAL_CORE_H

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
#include <dali/devel-api/common/owner-container.h>
#include <dali/integration-api/context-notifier.h>
#include <dali/integration-api/core-enumerations.h>
#include <dali/integration-api/resource-policies.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/event/common/object-registry-impl.h>
#include <dali/internal/event/common/scene-impl.h>
#include <dali/internal/event/common/stage-def.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/ref-object.h>

namespace Dali
{
namespace Graphics
{
class Controller;
}

namespace Integration
{
class Processor;
class RenderController;
class GlAbstraction;
class PlatformAbstraction;
class UpdateStatus;
class RenderStatus;
struct Event;
struct TouchEvent;
} // namespace Integration

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
class RenderTaskProcessor;
} // namespace SceneGraph

/**
 * Internal class for Dali::Integration::Core
 */
class Core : public EventThreadServices
{
public:
  /**
   * Create and initialise a new Core instance
   */
  Core(Integration::RenderController&      renderController,
       Integration::PlatformAbstraction&   platform,
       Graphics::Controller&               graphicsController,
       Integration::RenderToFrameBuffer    renderToFboEnabled,
       Integration::DepthBufferAvailable   depthBufferAvailable,
       Integration::StencilBufferAvailable stencilBufferAvailable,
       Integration::PartialUpdateAvailable partialUpdateAvailable);

  /**
   * Destructor
   */
  ~Core() override;

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
   * @copydoc Dali::Integration::Core::SetMinimumFrameTimeInterval(uint32_t)
   */
  void SetMinimumFrameTimeInterval(uint32_t interval);

  /**
   * @copydoc Dali::Integration::Core::Update()
   */
  void Update(float elapsedSeconds, uint32_t lastVSyncTimeMilliseconds, uint32_t nextVSyncTimeMilliseconds, Integration::UpdateStatus& status, bool renderToFboEnabled, bool isRenderingToFbo, bool uploadOnly);

  /**
   * @copydoc Dali::Integration::Core::PreRender()
   */
  void PreRender(Integration::RenderStatus& status, bool forceClear);

  /**
   * @copydoc Dali::Integration::Core::PreRender()
   */
  void PreRender(Integration::Scene& scene, std::vector<Rect<int>>& damagedRects);

  /**
   * @copydoc Dali::Integration::Core::RenderScene()
   */
  void RenderScene(Integration::RenderStatus& status, Integration::Scene& scene, bool renderToFbo);

  /**
   * @copydoc Dali::Integration::Core::RenderScene()
   */
  void RenderScene(Integration::RenderStatus& status, Integration::Scene& scene, bool renderToFbo, Rect<int>& clippingRect);

  /**
   * @copydoc Dali::Integration::Core::Render()
   */
  void PostRender();

  /**
   * @copydoc Dali::Integration::Core::SceneCreated()
   */
  void SceneCreated();

  /**
   * @copydoc Dali::Integration::Core::QueueEvent(const Integration::Event&)
   */
  void QueueEvent(const Integration::Event& event);

  /**
   * @copydoc Dali::Integration::Core::ForceRelayout()
   */
  void ForceRelayout();

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
  void RegisterProcessor(Integration::Processor& processor, bool postProcessor = false);

  /**
   * @copydoc Dali::Integration::Core::UnregisterProcessor
   */
  void UnregisterProcessor(Dali::Integration::Processor& processor, bool postProcessor = false);

  /**
   * @copydoc Dali::Internal::ThreadLocalStorage::AddScene()
   */
  void AddScene(Scene* scene);

  /**
   * @copydoc Dali::Internal::ThreadLocalStorage::RemoveScene()
   */
  void RemoveScene(Scene* scene);

  /**
   * @brief Gets the Object registry.
   * @return A reference to the object registry
   */
  ObjectRegistry& GetObjectRegistry() const;

  /**
   * Log the capacity of DALi memory pools
   */
  void LogMemoryPools() const;

public: // Implementation of EventThreadServices
  /**
   * @copydoc EventThreadServices::RegisterObject
   */
  void RegisterObject(BaseObject* object) override;

  /**
   * @copydoc EventThreadServices::UnregisterObject
   */
  void UnregisterObject(BaseObject* object) override;

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
  uint32_t* ReserveMessageSlot(uint32_t size, bool updateScene) override;

  /**
   * @copydoc EventThreadServices::GetEventBufferIndex
   */
  BufferIndex GetEventBufferIndex() const override;

private:
  using SceneContainer = std::vector<ScenePtr>;

  /**
   * Run each registered processor
   */
  void RunProcessors();

  /**
   * Run each registered postprocessor
   */
  void RunPostProcessors();

  /**
   * Run registered processors, and relayout, and flush messages
   */
  void RelayoutAndFlush(SceneContainer& scenes);

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

  /**
   * @brief Returns GlAbstraction.
   * @note Use only for the capability. Do not use this for bypass context
   * @return GlAbstraction
   */
  Integration::GlAbstraction& GetGlAbstraction() const;

private:
  /**
   * Undefined copy and assignment operators
   */
  Core(const Core& core) = delete;            // No definition
  Core& operator=(const Core& core) = delete; // No definition

  /**
   * Create Thread local storage
   */
  void CreateThreadLocalStorage();

private:
  Integration::RenderController&    mRenderController; ///< Reference to Render controller to tell it to keep rendering
  Integration::PlatformAbstraction& mPlatform;         ///< The interface providing platform specific services.

  IntrusivePtr<Stage>                       mStage;                       ///< The current stage
  OwnerPointer<AnimationPlaylist>           mAnimationPlaylist;           ///< For 'Fire and forget' animation support
  OwnerPointer<PropertyNotificationManager> mPropertyNotificationManager; ///< For safe signal emmision of property changed notifications
  IntrusivePtr<RelayoutController>          mRelayoutController;          ///< Size negotiation relayout controller

  OwnerPointer<SceneGraph::RenderTaskProcessor> mRenderTaskProcessor;   ///< Handles the processing of render tasks
  OwnerPointer<SceneGraph::RenderManager>       mRenderManager;         ///< Render manager
  OwnerPointer<SceneGraph::UpdateManager>       mUpdateManager;         ///< Update manager
  OwnerPointer<ShaderFactory>                   mShaderFactory;         ///< Shader resource factory
  OwnerPointer<NotificationManager>             mNotificationManager;   ///< Notification manager
  OwnerPointer<GestureEventProcessor>           mGestureEventProcessor; ///< The gesture event processor
  Dali::Vector<Integration::Processor*>         mProcessors;            ///< Registered processors (not owned)
  Dali::Vector<Integration::Processor*>         mPostProcessors;        ///< Registered post processors those will called after relayout(not owned)

  SceneContainer mScenes; ///< A container of scenes that bound to a surface for rendering, owned by Core

  // The object registry
  ObjectRegistryPtr mObjectRegistry;

  Graphics::Controller& mGraphicsController;

  bool mProcessingEvent : 1;           ///< True during ProcessEvents()
  bool mProcessorUnregistered : 1;     ///< True if the processor is unregistered during RunProcessors()
  bool mPostProcessorUnregistered : 1; ///< True if the post-processor is unregistered during RunPostProcessors()
  bool mRelayoutFlush : 1;             ///< True during RelayoutAndFlush()

  friend class ThreadLocalStorage;
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_CORE_H
