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
#include <dali/internal/update/resources/complete-status-manager.h>

// INTERNAL INCLUDES
#include <dali/integration-api/gl-sync-abstraction.h>
#include <dali/integration-api/resource-declarations.h>
#include <dali/internal/update/resources/resource-manager.h>
#include <dali/internal/update/resources/resource-tracker.h>
#include <dali/internal/update/resources/sync-resource-tracker.h>
#include <dali/internal/update/controllers/render-message-dispatcher.h>
#include <dali/internal/render/common/render-tracker.h>
#include <dali/internal/update/resources/complete-status-manager-debug.h>

namespace Dali
{
namespace Internal
{

CompleteStatusManager::CompleteStatusManager( Integration::GlSyncAbstraction& glSyncAbstraction,
                                              SceneGraph::RenderMessageDispatcher& renderMessageDispatcher,
                                              ResourceManager& resourceManager )
: mGlSyncAbstraction(glSyncAbstraction),
  mRenderMessageDispatcher(renderMessageDispatcher),
  mResourceManager(resourceManager)
{
  TRACKER_LOG(Debug::Verbose);
}

CompleteStatusManager::~CompleteStatusManager()
{
  // Delete all extant resource trackers
  CompleteStatusManager::TrackedResourcesIter iter = mTrackedResources.begin();
  CompleteStatusManager::TrackedResourcesIter end = mTrackedResources.end();
  while( iter != end )
  {
    delete iter->second;
    ++iter;
  }

  TRACKER_LOG(Debug::Verbose);
}

void CompleteStatusManager::TrackResource( Integration::ResourceId id )
{
  TRACKER_LOG_FMT( Debug::General, "id:%d\n", id );

  CompleteStatusManager::TrackedResourcesIter iter = mTrackedResources.find(id);
  if( iter == mTrackedResources.end() )
  {
    // Create new tracker for ID
    ResourceTracker* resourceTracker = CreateResourceTracker( id );
    resourceTracker->Initialize();
    mTrackedResources[id] = resourceTracker;
  }
  else
  {
    // We've found existing tracker. Reset it
    iter->second->Reset();
  }
}

void CompleteStatusManager::StopTrackingResource( Integration::ResourceId id )
{
  TRACKER_LOG_FMT( Debug::General, "id:%d\n", id );

  CompleteStatusManager::TrackedResourcesIter iter = mTrackedResources.find(id);
  if( iter != mTrackedResources.end() )
  {
    iter->second->OnDestroy();
    delete iter->second;
    mTrackedResources.erase(iter);
  }
}

ResourceTracker* CompleteStatusManager::FindResourceTracker( Integration::ResourceId id )
{
  ResourceTracker* tracker = NULL;

  TrackedResourcesIter iter = mTrackedResources.find(id);
  if( iter != mTrackedResources.end() )
  {
    tracker = iter->second;
  }

  TRACKER_LOG_FMT(Debug::General, "id:%d = tracker:%p\n", id, tracker);
  return tracker;
}

CompleteStatusManager::CompleteState CompleteStatusManager::GetStatus( Integration::ResourceId id )
{
  CompleteState readiness = CompleteStatusManager::NOT_READY;

  if( 0 != id )
  {
    TrackedResourcesIter iter = mTrackedResources.find(id);
    if( iter != mTrackedResources.end() )
    {
      if( iter->second->IsComplete() )
      {
        readiness = CompleteStatusManager::COMPLETE;
      }
    }
    else if( mResourceManager.IsResourceLoaded(id) )
    {
      readiness = CompleteStatusManager::COMPLETE;
    }
    else if( mResourceManager.IsResourceLoadFailed(id) )
    {
      readiness = CompleteStatusManager::NEVER;
    }
  }
  else
  {
    // Loading is essentially finished if we don't have a resource ID
    readiness = CompleteStatusManager::COMPLETE;
  }

  TRACKER_LOG_FMT(Debug::General, "id:%d = %s\n", id, (readiness==CompleteStatusManager::COMPLETE)?"COMPLETE":((readiness==CompleteStatusManager::NEVER)?"NEVER":"NOT_READY"));

  return readiness;
}

ResourceTracker* CompleteStatusManager::CreateResourceTracker( Integration::ResourceId id )
{
  ResourceTracker* resourceTracker = NULL;
  BitmapMetadata bitmapMetadata = mResourceManager.GetBitmapMetadata( id );
  if( bitmapMetadata.GetIsNativeImage() && bitmapMetadata.GetIsFramebuffer()  )
  {
    resourceTracker = new SyncResourceTracker( mGlSyncAbstraction, mRenderMessageDispatcher );
  }
  else
  {
    resourceTracker = new ResourceTracker();
  }
  return resourceTracker;
}

} // Internal
} // Dali
