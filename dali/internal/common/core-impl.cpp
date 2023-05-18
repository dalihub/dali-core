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

// CLASS HEADER
#include <dali/internal/common/core-impl.h>

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-controller.h>
#include <dali/integration-api/core.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/events/event.h>
#include <dali/integration-api/gl-context-helper-abstraction.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/integration-api/processor-interface.h>
#include <dali/integration-api/render-controller.h>
#include <dali/integration-api/trace.h>

#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/animation/animation-playlist.h>
#include <dali/internal/event/common/event-thread-services.h>
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
#include <dali/internal/update/manager/render-task-processor.h>
#include <dali/internal/update/manager/update-manager.h>

#include <dali/internal/render/common/performance-monitor.h>
#include <dali/internal/render/common/render-manager.h>

using Dali::Internal::SceneGraph::DiscardQueue;
using Dali::Internal::SceneGraph::RenderManager;
using Dali::Internal::SceneGraph::RenderQueue;
using Dali::Internal::SceneGraph::UpdateManager;

namespace
{
// The Update for frame N+1 may be processed whilst frame N is being rendered.
const uint32_t MAXIMUM_UPDATE_COUNT = 2u;

DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_PERFORMANCE_MARKER, false);

#if defined(DEBUG_ENABLED)
Debug::Filter* gCoreFilter = Debug::Filter::New(Debug::Concise, false, "LOG_CORE");
#endif
} // namespace

namespace Dali
{
namespace Internal
{
using Integration::Event;
using Integration::GlAbstraction;
using Integration::GlContextHelperAbstraction;
using Integration::PlatformAbstraction;
using Integration::RenderController;
using Integration::RenderStatus;
using Integration::UpdateStatus;

Core::Core(RenderController&                   renderController,
           PlatformAbstraction&                platform,
           Graphics::Controller&               graphicsController,
           Integration::RenderToFrameBuffer    renderToFboEnabled,
           Integration::DepthBufferAvailable   depthBufferAvailable,
           Integration::StencilBufferAvailable stencilBufferAvailable,
           Integration::PartialUpdateAvailable partialUpdateAvailable)
: mRenderController(renderController),
  mPlatform(platform),
  mGraphicsController(graphicsController),
  mProcessingEvent(false),
  mProcessorUnregistered(false),
  mPostProcessorUnregistered(false)
{
  // Create the thread local storage
  CreateThreadLocalStorage();

  // This does nothing until Core is built with --enable-performance-monitor
  PERFORMANCE_MONITOR_INIT(platform);

  mNotificationManager = new NotificationManager();

  mAnimationPlaylist = AnimationPlaylist::New();

  mPropertyNotificationManager = PropertyNotificationManager::New();

  mRenderTaskProcessor = new SceneGraph::RenderTaskProcessor();

  mRenderManager = RenderManager::New(graphicsController, depthBufferAvailable, stencilBufferAvailable, partialUpdateAvailable);

  RenderQueue& renderQueue = mRenderManager->GetRenderQueue();

  mDiscardQueue = new DiscardQueue(renderQueue);

  mUpdateManager = new UpdateManager(*mNotificationManager,
                                     *mAnimationPlaylist,
                                     *mPropertyNotificationManager,
                                     *mDiscardQueue,
                                     renderController,
                                     *mRenderManager,
                                     renderQueue,
                                     *mRenderTaskProcessor);

  mRenderManager->SetShaderSaver(*mUpdateManager);

  mObjectRegistry = ObjectRegistry::New();

  mStage = IntrusivePtr<Stage>(Stage::New(*mUpdateManager));

  // This must be called after stage is created but before stage initialization
  mRelayoutController = IntrusivePtr<RelayoutController>(new RelayoutController(mRenderController));

  mGestureEventProcessor = new GestureEventProcessor(*mUpdateManager, mRenderController);

  mShaderFactory = new ShaderFactory();
  mUpdateManager->SetShaderSaver(*mShaderFactory);

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
  if(tls)
  {
    tls->Remove();
    tls->Unreference();
  }

  mObjectRegistry.Reset();

  // Stop relayout requests being raised on stage destruction
  mRelayoutController.Reset();

  // remove (last?) reference to stage
  mStage.Reset();
}

void Core::Initialize()
{
  mStage->Initialize(*mScenes[0]);
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
}

void Core::ContextDestroyed()
{
}

void Core::Update(float elapsedSeconds, uint32_t lastVSyncTimeMilliseconds, uint32_t nextVSyncTimeMilliseconds, Integration::UpdateStatus& status, bool renderToFboEnabled, bool isRenderingToFbo, bool uploadOnly)
{
  // set the time delta so adaptor can easily print FPS with a release build with 0 as
  // it is cached by frametime
  status.secondsFromLastFrame = elapsedSeconds;

  // Render returns true when there are updates on the stage or one or more animations are completed.
  // Use the estimated time diff till we render as the elapsed time.
  status.keepUpdating = mUpdateManager->Update(elapsedSeconds,
                                               lastVSyncTimeMilliseconds,
                                               nextVSyncTimeMilliseconds,
                                               renderToFboEnabled,
                                               isRenderingToFbo,
                                               uploadOnly);

  // Check the Notification Manager message queue to set needsNotification
  status.needsNotification = mNotificationManager->MessagesToProcess();

  // No need to keep update running if there are notifications to process.
  // Any message to update will wake it up anyways
}

void Core::PreRender(RenderStatus& status, bool forceClear)
{
  mRenderManager->PreRender(status, forceClear);
}

void Core::PreRender(Integration::Scene& scene, std::vector<Rect<int>>& damagedRects)
{
  mRenderManager->PreRender(scene, damagedRects);
}

void Core::RenderScene(RenderStatus& status, Integration::Scene& scene, bool renderToFbo)
{
  mRenderManager->RenderScene(status, scene, renderToFbo);
}

void Core::RenderScene(RenderStatus& status, Integration::Scene& scene, bool renderToFbo, Rect<int>& clippingRect)
{
  mRenderManager->RenderScene(status, scene, renderToFbo, clippingRect);
}

void Core::PostRender()
{
  mRenderManager->PostRender();
}

void Core::SceneCreated()
{
  mStage->EmitSceneCreatedSignal();

  mRelayoutController->OnApplicationSceneCreated();

  for(const auto& scene : mScenes)
  {
    Dali::Actor sceneRootLayer = scene->GetRootLayer();
    mRelayoutController->RequestRelayoutTree(sceneRootLayer);
  }
}

void Core::QueueEvent(const Integration::Event& event)
{
  if(mScenes.size() != 0)
  {
    mScenes.front()->QueueEvent(event);
  }
}

void Core::ProcessEvents()
{
  // Guard against calls to ProcessEvents() during ProcessEvents()
  if(mProcessingEvent)
  {
    DALI_LOG_ERROR("ProcessEvents should not be called from within ProcessEvents!\n");
    mRenderController.RequestProcessEventsOnIdle();
    return;
  }

  mProcessingEvent = true;
  mRelayoutController->SetProcessingCoreEvents(true);

  // Signal that any messages received will be flushed soon
  mUpdateManager->EventProcessingStarted();

  // Scene could be added or removed while processing the events
  // Copy the Scene container locally to avoid possibly invalid iterator
  SceneContainer scenes = mScenes;

  // process events in all scenes
  for(auto scene : scenes)
  {
    scene->ProcessEvents();
  }

  mNotificationManager->ProcessMessages();

  // Emit signal here to inform listeners that event processing has finished.
  for(auto scene : scenes)
  {
    scene->EmitEventProcessingFinishedSignal();
  }

  // Run any registered processors
  RunProcessors();

  // Run the size negotiation after event processing finished signal
  mRelayoutController->Relayout();

  // Run any registered post processors
  RunPostProcessors();

  // Rebuild depth tree after event processing has finished
  for(auto scene : scenes)
  {
    scene->RebuildDepthTree();
  }

  // Flush any queued messages for the update-thread
  const bool messagesToProcess = mUpdateManager->FlushQueue();

  // Check if the touch or gestures require updates.
  const bool gestureNeedsUpdate = mGestureEventProcessor->NeedsUpdate();

  if(messagesToProcess || gestureNeedsUpdate)
  {
    // tell the render controller to keep update thread running
    mRenderController.RequestUpdate();
  }

  mRelayoutController->SetProcessingCoreEvents(false);

  // ProcessEvents() may now be called again
  mProcessingEvent = false;
}

uint32_t Core::GetMaximumUpdateCount() const
{
  return MAXIMUM_UPDATE_COUNT;
}

void Core::RegisterProcessor(Integration::Processor& processor, bool postProcessor)
{
  if(postProcessor)
  {
    mPostProcessors.PushBack(&processor);
  }
  else
  {
    mProcessors.PushBack(&processor);
  }
}

void Core::UnregisterProcessor(Integration::Processor& processor, bool postProcessor)
{
  if(postProcessor)
  {
    auto iter = std::find(mPostProcessors.Begin(), mPostProcessors.End(), &processor);
    if(iter != mPostProcessors.End())
    {
      mPostProcessors.Erase(iter);
      mPostProcessorUnregistered = true;
    }
  }
  else
  {
    auto iter = std::find(mProcessors.Begin(), mProcessors.End(), &processor);
    if(iter != mProcessors.End())
    {
      mProcessors.Erase(iter);
      mProcessorUnregistered = true;
    }
  }
}

void Core::RunProcessors()
{
  if(mProcessors.Count() != 0)
  {
    DALI_TRACE_BEGIN(gTraceFilter, "DALI_CORE_RUN_PROCESSORS");

    // Copy processor pointers to prevent changes to vector affecting loop iterator.
    Dali::Vector<Integration::Processor*> processors(mProcessors);

    // To prevent accessing processor unregistered during the loop
    mProcessorUnregistered = false;

    for(auto processor : processors)
    {
      if(processor)
      {
        if(!mProcessorUnregistered)
        {
          processor->Process(false);
        }
        else
        {
          // Run processor if the processor is still in the list.
          // It may be removed during the loop.
          auto iter = std::find(mProcessors.Begin(), mProcessors.End(), processor);
          if(iter != mProcessors.End())
          {
            processor->Process(false);
          }
        }
      }
    }
    DALI_TRACE_END(gTraceFilter, "DALI_CORE_RUN_PROCESSORS");
  }
}

void Core::RunPostProcessors()
{
  if(mPostProcessors.Count() != 0)
  {
    DALI_TRACE_BEGIN(gTraceFilter, "DALI_CORE_RUN_POST_PROCESSORS");

    // Copy processor pointers to prevent changes to vector affecting loop iterator.
    Dali::Vector<Integration::Processor*> processors(mPostProcessors);

    // To prevent accessing processor unregistered during the loop
    mPostProcessorUnregistered = false;

    for(auto processor : processors)
    {
      if(processor)
      {
        if(!mPostProcessorUnregistered)
        {
          processor->Process(true);
        }
        else
        {
          // Run processor if the processor is still in the list.
          // It may be removed during the loop.
          auto iter = std::find(mPostProcessors.Begin(), mPostProcessors.End(), processor);
          if(iter != mPostProcessors.End())
          {
            processor->Process(true);
          }
        }
      }
    }
    DALI_TRACE_END(gTraceFilter, "DALI_CORE_RUN_POST_PROCESSORS");
  }
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

ObjectRegistry& Core::GetObjectRegistry() const
{
  return *(mObjectRegistry.Get());
}

EventThreadServices& Core::GetEventThreadServices()
{
  return *this;
}

PropertyNotificationManager& Core::GetPropertyNotificationManager() const
{
  return *(mPropertyNotificationManager);
}

AnimationPlaylist& Core::GetAnimationPlaylist() const
{
  return *(mAnimationPlaylist);
}

Integration::GlAbstraction& Core::GetGlAbstraction() const
{
  return mGraphicsController.GetGlAbstraction();
}

void Core::AddScene(Scene* scene)
{
  mScenes.push_back(scene);
}

void Core::RemoveScene(Scene* scene)
{
  auto iter = std::find(mScenes.begin(), mScenes.end(), scene);
  if(iter != mScenes.end())
  {
    mScenes.erase(iter);
  }
}

void Core::CreateThreadLocalStorage()
{
  // a pointer to the ThreadLocalStorage object will be stored in TLS
  // The ThreadLocalStorage object should be deleted by the Core destructor
  ThreadLocalStorage* tls = new ThreadLocalStorage(this);
  tls->Reference();
}

void Core::RegisterObject(Dali::BaseObject* object)
{
  mObjectRegistry = &ThreadLocalStorage::Get().GetObjectRegistry();
  mObjectRegistry->RegisterObject(object);
}

void Core::UnregisterObject(Dali::BaseObject* object)
{
  mObjectRegistry = &ThreadLocalStorage::Get().GetObjectRegistry();
  mObjectRegistry->UnregisterObject(object);
}

Integration::RenderController& Core::GetRenderController()
{
  return mRenderController;
}

uint32_t* Core::ReserveMessageSlot(uint32_t size, bool updateScene)
{
  return mUpdateManager->ReserveMessageSlot(size, updateScene);
}

BufferIndex Core::GetEventBufferIndex() const
{
  return mUpdateManager->GetEventBufferIndex();
}

} // namespace Internal

} // namespace Dali
