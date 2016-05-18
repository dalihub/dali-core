#ifndef __DALI_INTERNAL_CORE_H__
#define __DALI_INTERNAL_CORE_H__

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
#include <dali/public-api/object/ref-object.h>
#include <dali/integration-api/context-notifier.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/event/animation/animation-playlist-declarations.h>
#include <dali/internal/event/common/stage-def.h>
#include <dali/internal/update/resources/resource-manager-declarations.h>
#include <dali/public-api/common/view-mode.h>
#include <dali/integration-api/resource-policies.h>

namespace Dali
{

namespace Integration
{
class RenderController;
class PlatformAbstraction;
class GestureManager;
class GlAbstraction;
class GlSyncAbstraction;
class SystemOverlay;
class UpdateStatus;
class RenderStatus;
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
class ResourceClient;
class ResourceManager;
class ImageFactory;
class ShaderFactory;
class TouchResampler;
class RelayoutController;

namespace SceneGraph
{
class UpdateManager;
class RenderManager;
class DiscardQueue;
class TextureCacheDispatcher;
}

/**
 * Internal class for Dali::Integration::Core
 */
class Core
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
        ResourcePolicy::DataRetention dataRetentionPolicy );

  /**
   * Destructor
   */
  ~Core();

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
   * @copydoc Dali::Integration::Core::SurfaceResized(unsigned int, unsigned int)
   */
  void SurfaceResized(unsigned int width, unsigned int height);

  /**
   * @copydoc Dali::Integration::Core::SetDpi(unsigned int, unsigned int)
   */
  void SetDpi(unsigned int dpiHorizontal, unsigned int dpiVertical);

  /**
   * @copydoc Dali::Integration::Core::SetMinimumFrameTimeInterval(unsigned int)
   */
  void SetMinimumFrameTimeInterval(unsigned int interval);

  /**
   * @copydoc Dali::Integration::Core::Update()
   */
  void Update( float elapsedSeconds, unsigned int lastVSyncTimeMilliseconds, unsigned int nextVSyncTimeMilliseconds, Integration::UpdateStatus& status );

  /**
   * @copydoc Dali::Integration::Core::Render()
   */
  void Render( Integration::RenderStatus& status );

  /**
   * @copydoc Dali::Integration::Core::Suspend()
   */
  void Suspend();

  /**
   * @copydoc Dali::Integration::Core::Resume()
   */
  void Resume();

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
   * @copydoc Dali::Integration::Core::UpdateTouchData(const Integration::TouchData&)
   */
  void UpdateTouchData(const Integration::TouchData& touch);

  /**
   * @copydoc Dali::Integration::Core::GetMaximumUpdateCount()
   */
  unsigned int GetMaximumUpdateCount() const;

  /**
   * @copydoc Dali::Integration::Core::GetSystemOverlay()
   */
  Integration::SystemOverlay& GetSystemOverlay();

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

private:  // for use by ThreadLocalStorage

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
   * Returns the update manager.
   * @return A reference to the update manager.
   */
  SceneGraph::UpdateManager& GetUpdateManager();

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
   * Returns the Resource Manager.
   * @return A reference to the Resource Manager.
   */
  ResourceManager& GetResourceManager();

  /**
   * Returns the Resource client.
   * @return A reference to the Resource Client.
   */
  ResourceClient& GetResourceClient();

  /**
   * Returns the Image factory
   * @return A reference to the Image factory.
   */
  ImageFactory& GetImageFactory();

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

private:

  /**
   * Undefined copy and assignment operators
   */
  Core(const Core& core);  // No definition
  Core& operator=(const Core& core);  // No definition

  /**
   * Create Thread local storage
   */
  void CreateThreadLocalStorage();

private:

  Integration::RenderController&            mRenderController;            ///< Reference to Render controller to tell it to keep rendering
  Integration::PlatformAbstraction&         mPlatform;                    ///< The interface providing platform specific services.

  IntrusivePtr<Stage>                       mStage;                       ///< The current stage
  GestureEventProcessor*                    mGestureEventProcessor;       ///< The gesture event processor
  EventProcessor*                           mEventProcessor;              ///< The event processor
  SceneGraph::UpdateManager*                mUpdateManager;               ///< Update manager
  SceneGraph::RenderManager*                mRenderManager;               ///< Render manager
  SceneGraph::DiscardQueue*                 mDiscardQueue;                ///< Used to cleanup nodes & resources when no longer in use.
  SceneGraph::TextureCacheDispatcher*       mTextureCacheDispatcher;      ///< Used to send messages to TextureCache
  LockedResourceQueue*                      mTextureUploadedQueue;        ///< Stores resource ids which require post processing after render
  NotificationManager*                      mNotificationManager;         ///< Notification manager
  AnimationPlaylistOwner                    mAnimationPlaylist;           ///< For 'Fire and forget' animation support
  OwnerPointer<PropertyNotificationManager> mPropertyNotificationManager; ///< For safe signal emmision of property changed notifications
  ImageFactory*                             mImageFactory;                ///< Image resource factory
  ShaderFactory*                            mShaderFactory;               ///< Shader resource factory
  ResourceClient*                           mResourceClient;              ///< Asynchronous Resource Loading
  ResourceManager*                          mResourceManager;             ///< Asynchronous Resource Loading
  TouchResampler*                           mTouchResampler;              ///< Resamples touches to correct frame rate.
  IntrusivePtr< RelayoutController >        mRelayoutController;          ///< Size negotiation relayout controller

  bool                                      mIsActive         : 1;        ///< Whether Core is active or suspended
  bool                                      mProcessingEvent  : 1;        ///< True during ProcessEvents()

  friend class ThreadLocalStorage;

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_CORE_H__
