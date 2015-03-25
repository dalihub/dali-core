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

// CLASS HEADER
#include <dali/internal/common/core-impl.h>

// INTERNAL INCLUDES
#include <dali/integration-api/system-overlay.h>
#include <dali/integration-api/core.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/integration-api/gl-sync-abstraction.h>
#include <dali/integration-api/render-controller.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/animation/animation-playlist.h>
#include <dali/internal/event/common/property-notification-manager.h>
#include <dali/internal/event/common/notification-manager.h>
#include <dali/integration-api/events/event.h>
#include <dali/internal/event/events/event-processor.h>
#include <dali/internal/event/events/gesture-event-processor.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/render/common/performance-monitor.h>
#include <dali/internal/render/common/render-manager.h>
#include <dali/internal/update/common/discard-queue.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/internal/event/text/font-factory.h>
#include <dali/internal/event/images/image-factory.h>
#include <dali/internal/event/images/emoji-factory.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/effects/shader-factory.h>
#include <dali/internal/update/touch/touch-resampler.h>
#include <dali/internal/event/common/type-registry-impl.h>
#include <dali/internal/event/render-tasks/render-task-list-impl.h>

#include <dali/internal/render/gl-resources/texture-cache.h>
#include <dali/internal/render/gl-resources/context.h>

using Dali::Internal::SceneGraph::UpdateManager;
using Dali::Internal::SceneGraph::RenderManager;
using Dali::Internal::SceneGraph::DiscardQueue;
using Dali::Internal::SceneGraph::RenderQueue;
using Dali::Internal::SceneGraph::TextureCache;

namespace
{
// The Update for frame N+1 may be processed whilst frame N is being rendered.
const unsigned int MAXIMUM_UPDATE_COUNT = 2u;

#if defined(DEBUG_ENABLED)
Debug::Filter* gCoreFilter = Debug::Filter::New(Debug::Concise, false, "LOG_CORE");
#endif
}

namespace Dali
{

namespace Internal
{

using Integration::RenderController;
using Integration::PlatformAbstraction;
using Integration::GlSyncAbstraction;
using Integration::GestureManager;
using Integration::GlAbstraction;
using Integration::Event;
using Integration::UpdateStatus;
using Integration::RenderStatus;

Core::Core( RenderController& renderController, PlatformAbstraction& platform,
            GlAbstraction& glAbstraction, GlSyncAbstraction& glSyncAbstraction,
            GestureManager& gestureManager, ResourcePolicy::DataRetention dataRetentionPolicy)
: mRenderController( renderController ),
  mPlatform(platform),
  mGestureEventProcessor(NULL),
  mEventProcessor(NULL),
  mUpdateManager(NULL),
  mRenderManager(NULL),
  mDiscardQueue(NULL),
  mResourcePostProcessQueue(),
  mNotificationManager(NULL),
  mFontFactory(NULL),
  mImageFactory(NULL),
  mShaderFactory(NULL),
  mEmojiFactory(NULL),
  mIsActive(true),
  mProcessingEvent(false)
{
  // Create the thread local storage
  CreateThreadLocalStorage();

  // This does nothing until Core is built with --enable-performance-monitor
  PERFORMANCE_MONITOR_INIT( platform );

  mNotificationManager = new NotificationManager();

  mAnimationPlaylist = AnimationPlaylist::New();

  mPropertyNotificationManager = PropertyNotificationManager::New();

  std::vector< ResourcePostProcessRequest> init;
  mResourcePostProcessQueue = new ResourcePostProcessList(init);

  mRenderManager = RenderManager::New( glAbstraction, *mResourcePostProcessQueue );

  RenderQueue& renderQueue = mRenderManager->GetRenderQueue();
  TextureCache& textureCache = mRenderManager->GetTextureCache();

  ResourcePolicy::Discardable discardPolicy = ResourcePolicy::DISCARD;
  if( dataRetentionPolicy == ResourcePolicy::DALI_RETAINS_ALL_DATA )
  {
    discardPolicy = ResourcePolicy::RETAIN;
  }
  textureCache.SetDiscardBitmapsPolicy(discardPolicy);

  mDiscardQueue = new DiscardQueue( renderQueue );

  mResourceManager = new ResourceManager(  mPlatform,
                                          *mNotificationManager,
                                           textureCache,
                                          *mResourcePostProcessQueue,
                                          *mRenderManager,
                                          *mDiscardQueue,
                                           renderQueue );

  mTouchResampler = TouchResampler::New();

  mUpdateManager = new UpdateManager( *mNotificationManager,
                                       glSyncAbstraction,
                                      *mAnimationPlaylist,
                                      *mPropertyNotificationManager,
                                      *mResourceManager,
                                      *mDiscardQueue,
                                       renderController,
                                      *mRenderManager,
                                       renderQueue,
                                       textureCache,
                                      *mTouchResampler );

  mStage = IntrusivePtr<Stage>( Stage::New( *mAnimationPlaylist, *mPropertyNotificationManager, *mUpdateManager, *mNotificationManager ) );
  mStage->Initialize();

  mResourceClient = new ResourceClient( *mResourceManager, *mStage, dataRetentionPolicy );

  mGestureEventProcessor = new GestureEventProcessor(*mStage, gestureManager, mRenderController);
  mEventProcessor = new EventProcessor(*mStage, *mNotificationManager, *mGestureEventProcessor);

  mFontFactory = new FontFactory(*mResourceClient);
  mImageFactory = new ImageFactory( *mResourceClient );
  mShaderFactory = new ShaderFactory(*mResourceClient);
  mShaderFactory->LoadDefaultShaders();
  mEmojiFactory = new EmojiFactory();

  GetImplementation(Dali::TypeRegistry::Get()).CallInitFunctions();
}

Core::~Core()
{
  /**
   * TODO this should be done by Adaptor, Core does not know about threading
   * First stop the resource loading thread(s)
   */
  mPlatform.JoinLoaderThreads();

  /*
   * The order of destructing these singletons is important!!!
   */

  // clear the thread local storage first
  // allows core to be created / deleted many times in the same thread (how TET cases work).
  // Do this before mStage.Reset() so Stage::IsInstalled() returns false
  ThreadLocalStorage::Get().Remove();

  // Clean-up stage - remove default camera and root layer
  mStage->Uninitialize();

  // remove (last?) reference to stage
  mStage.Reset();

  delete mEventProcessor;
  delete mGestureEventProcessor;
  delete mNotificationManager;
  delete mFontFactory;
  delete mImageFactory;
  delete mShaderFactory;
  delete mResourceClient;
  delete mResourceManager;
  delete mUpdateManager;
  delete mTouchResampler;
  delete mEmojiFactory;
  delete mRenderManager;
  delete mDiscardQueue;
  delete mResourcePostProcessQueue;
}

Integration::ContextNotifierInterface* Core::GetContextNotifier()
{
  return mStage.Get();
}

void Core::RecoverFromContextLoss()
{
  DALI_LOG_INFO(gCoreFilter, Debug::Verbose, "Core::RecoverFromContextLoss()\n");

  mImageFactory->RecoverFromContextLoss(); // Reload images from files
  mFontFactory->RecoverFromContextLoss();  // Reload glyphs from cache into new atlas
  mStage->GetRenderTaskList().RecoverFromContextLoss(); // Re-trigger render-tasks
}

void Core::ContextCreated()
{
  mRenderManager->ContextCreated();
}

void Core::ContextDestroyed()
{
  mRenderManager->ContextDestroyed();
}

void Core::SurfaceResized(unsigned int width, unsigned int height)
{
  mStage->SetSize(width, height);
}

void Core::SetDpi(unsigned int dpiHorizontal, unsigned int dpiVertical)
{
  mPlatform.SetDpi( dpiHorizontal, dpiVertical );
  mFontFactory->SetDpi( dpiHorizontal, dpiVertical);
  mStage->SetDpi( Vector2( dpiHorizontal , dpiVertical) );
}

void Core::Update( float elapsedSeconds, unsigned int lastVSyncTimeMilliseconds, unsigned int nextVSyncTimeMilliseconds, Integration::UpdateStatus& status )
{
  // set the time delta so adaptor can easily print FPS with a release build with 0 as
  // it is cached by frametime
  status.secondsFromLastFrame = elapsedSeconds;

  // Render returns true when there are updates on the stage or one or more animations are completed.
  // Use the estimated time diff till we render as the elapsed time.
  status.keepUpdating = mUpdateManager->Update( elapsedSeconds,
                                                lastVSyncTimeMilliseconds,
                                                nextVSyncTimeMilliseconds );

  // Check the Notification Manager message queue to set needsNotification
  status.needsNotification = mNotificationManager->MessagesToProcess();

  // No need to keep update running if there are notifications to process.
  // Any message to update will wake it up anyways

  if ( mResourceManager->ResourcesToProcess() )
  {
    // If we are still processing resources, then we have to continue the update
    status.keepUpdating |= Integration::KeepUpdating::LOADING_RESOURCES;
  }
}

void Core::Render( RenderStatus& status )
{
  bool updateRequired = mRenderManager->Render( status );

  status.SetNeedsUpdate( updateRequired );
}

void Core::Suspend()
{
  mPlatform.Suspend();

  mIsActive = false;
}

void Core::Resume()
{
  mPlatform.Resume();

  mIsActive = true;

  // trigger processing of events queued up while paused
  ProcessEvents();
}

void Core::SceneCreated()
{
  mStage->EmitSceneCreatedSignal();
}

void Core::QueueEvent( const Integration::Event& event )
{
  mEventProcessor->QueueEvent( event );
}

void Core::ProcessEvents()
{
  // Guard against calls to ProcessEvents() during ProcessEvents()
  if( mProcessingEvent )
  {
    DALI_LOG_ERROR( "ProcessEvents should not be called from within ProcessEvents!" );
    mRenderController.RequestProcessEventsOnIdle();
    return;
  }

  mProcessingEvent = true;

  // Signal that any messages received will be flushed soon
  mUpdateManager->EventProcessingStarted();

  mEventProcessor->ProcessEvents();

  mNotificationManager->ProcessMessages();

  // Avoid allocating MessageBuffers, triggering size-negotiation or sending any other spam whilst paused
  if( mIsActive )
  {
    // Emit signal here to start size negotiation and control relayout.
    mStage->EmitEventProcessingFinishedSignal();

    // Flush discard queue for image factory
    mImageFactory->FlushReleaseQueue();

    // send text requests if required
    mFontFactory->SendTextRequests();

    // Flush any queued messages for the update-thread
    const bool messagesToProcess = mUpdateManager->FlushQueue();

    // Check if the touch or gestures require updates.
    const bool touchNeedsUpdate = mTouchResampler->NeedsUpdate();
    const bool gestureNeedsUpdate = mGestureEventProcessor->NeedsUpdate();

    if( messagesToProcess || touchNeedsUpdate || gestureNeedsUpdate )
    {
      // tell the render controller to keep update thread running
      mRenderController.RequestUpdate();
    }
  }

  // ProcessEvents() may now be called again
  mProcessingEvent = false;
}

void Core::UpdateTouchData(const Integration::TouchData& touch)
{
  mTouchResampler->SendTouchData( touch );
}

unsigned int Core::GetMaximumUpdateCount() const
{
  return MAXIMUM_UPDATE_COUNT;
}

Integration::SystemOverlay& Core::GetSystemOverlay()
{
  return mStage->GetSystemOverlay();
}

void Core::SetViewMode( ViewMode viewMode )
{
  mStage->SetViewMode( viewMode );
}

ViewMode Core::GetViewMode() const
{
  return mStage->GetViewMode();
}

void Core::SetStereoBase( float stereoBase )
{
  mStage->SetStereoBase( stereoBase );
}

float Core::GetStereoBase() const
{
  return mStage->GetStereoBase();
}

StagePtr Core::GetCurrentStage()
{
  return mStage.Get();
}

PlatformAbstraction& Core::GetPlatform()
{
  return mPlatform;
}

UpdateManager& Core::GetUpdateManager()
{
  return *(mUpdateManager);
}

RenderManager& Core::GetRenderManager()
{
  return *(mRenderManager);
}

NotificationManager& Core::GetNotificationManager()
{
  return *(mNotificationManager);
}

ResourceManager& Core::GetResourceManager()
{
  return *(mResourceManager);
}

ResourceClient& Core::GetResourceClient()
{
  return *(mResourceClient);
}

FontFactory& Core::GetFontFactory()
{
  return *(mFontFactory);
}

ImageFactory& Core::GetImageFactory()
{
  return *(mImageFactory);
}

ShaderFactory& Core::GetShaderFactory()
{
  return *(mShaderFactory);
}

GestureEventProcessor& Core::GetGestureEventProcessor()
{
  return *(mGestureEventProcessor);
}

EmojiFactory& Core::GetEmojiFactory()
{
  return *mEmojiFactory;
}

void Core::CreateThreadLocalStorage()
{
  // a pointer to the ThreadLocalStorage object will be stored in TLS
  // and automatically deleted when the thread is killed
  new ThreadLocalStorage(this);
}

} // namespace Internal

} // namespace Dali
