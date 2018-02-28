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

// CLASS HEADER
#include <dali/internal/common/core-impl.h>
#include <dali/integration-api/graphics/graphics.h>

// INTERNAL INCLUDES
#include <dali/integration-api/system-overlay.h>
#include <dali/integration-api/core.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/events/event.h>
#include <dali/integration-api/gl-sync-abstraction.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/integration-api/render-controller.h>
#include <dali/integration-api/graphics/graphics.h>

#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/animation/animation-playlist.h>
#include <dali/internal/event/common/notification-manager.h>
#include <dali/internal/event/common/property-notification-manager.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/common/type-registry-impl.h>
#include <dali/internal/event/effects/shader-factory.h>
#include <dali/internal/event/events/event-processor.h>
#include <dali/internal/event/events/gesture-event-processor.h>
#include <dali/internal/event/render-tasks/render-task-list-impl.h>
#include <dali/internal/event/size-negotiation/relayout-controller-impl.h>

#include <dali/internal/update/common/discard-queue.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/manager/render-task-processor.h>

#include <dali/internal/render/common/performance-monitor.h>
#include <dali/internal/render/common/render-manager.h>
#include <dali/internal/render/gl-resources/context.h>

using Dali::Internal::SceneGraph::UpdateManager;
using Dali::Internal::SceneGraph::RenderManager;
using Dali::Internal::SceneGraph::DiscardQueue;
using Dali::Internal::SceneGraph::RenderQueue;

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
using Integration::Graphics::Graphics;


Core::Core( RenderController& renderController,
            PlatformAbstraction& platform,
            Graphics& graphics,
            GlAbstraction& glAbstraction,
            GlSyncAbstraction& glSyncAbstraction,
            GestureManager& gestureManager,
            ResourcePolicy::DataRetention dataRetentionPolicy,
            Integration::RenderToFrameBuffer renderToFboEnabled,
            Integration::DepthBufferAvailable depthBufferAvailable,
            Integration::StencilBufferAvailable stencilBufferAvailable )
: mRenderController( renderController ),
  mPlatform(platform),
  mProcessingEvent(false),
  mGraphics(graphics)
{
  // fixme: for now to ensure libgraphics.a won't be removed during linking due to being
  Integration::Graphics::IncludeThisLibrary();

  // Create the thread local storage
  CreateThreadLocalStorage();

  // This does nothing until Core is built with --enable-performance-monitor
  PERFORMANCE_MONITOR_INIT( platform );

  mNotificationManager = new NotificationManager();

  mAnimationPlaylist = AnimationPlaylist::New();

  mPropertyNotificationManager = PropertyNotificationManager::New();

  mRenderTaskProcessor = new SceneGraph::RenderTaskProcessor();

  mRenderManager = RenderManager::New( glAbstraction, glSyncAbstraction, depthBufferAvailable, stencilBufferAvailable );

  RenderQueue& renderQueue = mRenderManager->GetRenderQueue();

  mDiscardQueue = new DiscardQueue( renderQueue );

  mUpdateManager = new UpdateManager( *mNotificationManager,
                                      *mAnimationPlaylist,
                                      *mPropertyNotificationManager,
                                      *mDiscardQueue,
                                       renderController,
                                      *mRenderManager,
                                       renderQueue,
                                      *mRenderTaskProcessor,
                                       graphics );

  mRenderManager->SetShaderSaver( *mUpdateManager );

  mStage = IntrusivePtr<Stage>( Stage::New( *mAnimationPlaylist, *mPropertyNotificationManager, *mUpdateManager, *mNotificationManager, mRenderController ) );

  // This must be called after stage is created but before stage initialization
  mRelayoutController = IntrusivePtr< RelayoutController >( new RelayoutController( mRenderController ) );

  mStage->Initialize( renderToFboEnabled == Integration::RenderToFrameBuffer::TRUE );

  mGestureEventProcessor = new GestureEventProcessor( *mStage, *mUpdateManager, gestureManager, mRenderController );
  mEventProcessor = new EventProcessor( *mStage, *mNotificationManager, *mGestureEventProcessor );

  mShaderFactory = new ShaderFactory();
  mUpdateManager->SetShaderSaver( *mShaderFactory );

  GetImplementation(Dali::TypeRegistry::Get()).CallInitFunctions();
}

Core::~Core()
{
  /*
   * The order of destructing these singletons is important!!!
   */

  // clear the thread local storage first
  // allows core to be created / deleted many times in the same thread (how TET cases work).
  // Do this before mStage.Reset() so Stage::IsInstalled() returns false
  ThreadLocalStorage* tls = ThreadLocalStorage::GetInternal();
  if( tls )
  {
    tls->Remove();
    delete tls;
  }

  // Stop relayout requests being raised on stage destruction
  mRelayoutController.Reset();

  // Clean-up stage - remove default camera and root layer
  mStage->Uninitialize();

  // remove (last?) reference to stage
  mStage.Reset();

}

Integration::ContextNotifierInterface* Core::GetContextNotifier()
{
  return mStage.Get();
}

void Core::RecoverFromContextLoss()
{
  DALI_LOG_INFO(gCoreFilter, Debug::Verbose, "Core::RecoverFromContextLoss()\n");

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

void Core::SurfaceResized( unsigned int width, unsigned int height )
{
  mStage->SurfaceResized( width, height );

  // The stage-size may be less than surface-size (reduced by top-margin)
  Vector2 size = mStage->GetSize();
  mRelayoutController->SetStageSize( size.width, size.height );
}

void Core::SetTopMargin( unsigned int margin )
{
  mStage->SetTopMargin( margin );

  // The stage-size may be less than surface-size (reduced by top-margin)
  Vector2 size = mStage->GetSize();
  mRelayoutController->SetStageSize( size.width, size.height );
}

void Core::SetDpi( unsigned int dpiHorizontal, unsigned int dpiVertical )
{
  mStage->SetDpi( Vector2( dpiHorizontal , dpiVertical) );
}

void Core::Update( float elapsedSeconds, unsigned int lastVSyncTimeMilliseconds, unsigned int nextVSyncTimeMilliseconds, Integration::UpdateStatus& status, bool renderToFboEnabled, bool isRenderingToFbo )
{
  // set the time delta so adaptor can easily print FPS with a release build with 0 as
  // it is cached by frametime
  status.secondsFromLastFrame = elapsedSeconds;

  // Render returns true when there are updates on the stage or one or more animations are completed.
  // Use the estimated time diff till we render as the elapsed time.
  status.keepUpdating = mUpdateManager->Update( elapsedSeconds,
                                                lastVSyncTimeMilliseconds,
                                                nextVSyncTimeMilliseconds,
                                                renderToFboEnabled,
                                                isRenderingToFbo );

  // Check the Notification Manager message queue to set needsNotification
  status.needsNotification = mNotificationManager->MessagesToProcess();

  // No need to keep update running if there are notifications to process.
  // Any message to update will wake it up anyways
}

void Core::Render( RenderStatus& status, bool forceClear )
{
  mRenderManager->Render( status, forceClear );
}

void Core::SceneCreated()
{
  mStage->EmitSceneCreatedSignal();

  mRelayoutController->OnApplicationSceneCreated();
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
    DALI_LOG_ERROR( "ProcessEvents should not be called from within ProcessEvents!\n" );
    mRenderController.RequestProcessEventsOnIdle( false );
    return;
  }

  mProcessingEvent = true;
  mRelayoutController->SetProcessingCoreEvents( true );

  // Signal that any messages received will be flushed soon
  mUpdateManager->EventProcessingStarted();

  mEventProcessor->ProcessEvents();

  mNotificationManager->ProcessMessages();

  // Emit signal here to inform listeners that event processing has finished.
  mStage->EmitEventProcessingFinishedSignal();

  // Run the size negotiation after event processing finished signal
  mRelayoutController->Relayout();

  // Rebuild depth tree after event processing has finished
  mStage->RebuildDepthTree();

  // Flush any queued messages for the update-thread
  const bool messagesToProcess = mUpdateManager->FlushQueue();

  // Check if the touch or gestures require updates.
  const bool gestureNeedsUpdate = mGestureEventProcessor->NeedsUpdate();
  // Check if the next update is forced.
  const bool forceUpdate = mStage->IsNextUpdateForced();

  if( messagesToProcess || gestureNeedsUpdate || forceUpdate )
  {
    // tell the render controller to keep update thread running
    mRenderController.RequestUpdate( forceUpdate );
  }

  mRelayoutController->SetProcessingCoreEvents( false );

  // ProcessEvents() may now be called again
  mProcessingEvent = false;
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

ShaderFactory& Core::GetShaderFactory()
{
  return *(mShaderFactory);
}

GestureEventProcessor& Core::GetGestureEventProcessor()
{
  return *(mGestureEventProcessor);
}

RelayoutController& Core::GetRelayoutController()
{
  return *(mRelayoutController.Get());
}

void Core::CreateThreadLocalStorage()
{
  // a pointer to the ThreadLocalStorage object will be stored in TLS
  // The ThreadLocalStorage object should be deleted by the Core destructor
  new ThreadLocalStorage(this);
}

} // namespace Internal

} // namespace Dali
