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
#include <dali/internal/update/resources/sync-resource-tracker.h>

// INTERNAL INCLUDES
#include <dali/internal/update/controllers/render-message-dispatcher.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/common/render-tracker.h>
#include <dali/internal/render/common/render-manager.h>
#include <dali/internal/update/resources/resource-tracker-debug.h>

// EXTERNAL INCLUDES

namespace Dali
{
namespace Internal
{

SyncResourceTracker::SyncResourceTracker( Integration::GlSyncAbstraction& glSyncAbstraction,
                                      SceneGraph::RenderMessageDispatcher& renderMessageDispatcher)
: ResourceTracker(),
  mGlSyncAbstraction(glSyncAbstraction),
  mRenderMessageDispatcher(renderMessageDispatcher),
  mRenderTracker(NULL)
{
}

SyncResourceTracker::~SyncResourceTracker()
{
}

void SyncResourceTracker::Initialize()
{
  mRenderTracker = new SceneGraph::RenderTracker(mGlSyncAbstraction);

  // Pass ownership of render tracker to render manager, keep pointer for access
  mRenderMessageDispatcher.AddRenderTracker( *mRenderTracker );
}

void SyncResourceTracker::OnDestroy()
{
  if( mRenderTracker != NULL )
  {
    mRenderMessageDispatcher.RemoveRenderTracker( *mRenderTracker );
    mRenderTracker = NULL;
  }
}

SceneGraph::RenderTracker* SyncResourceTracker::GetRenderTracker()
{
  return mRenderTracker;
}

void SyncResourceTracker::DoReset()
{
  if( mRenderTracker != NULL )
  {
    mRenderTracker->ResetSyncFlag();
  }
}

void SyncResourceTracker::DoSetComplete()
{
  // Do nothing with this status, we are using only the RenderTracker to manage completeness
}

bool SyncResourceTracker::DoIsComplete()
{
  if( ! mComplete && mRenderTracker != NULL )
  {
    if( mRenderTracker->IsSynced() )
    {
      TRACKER_LOG_FMT(Debug::General, " Synced");
      mComplete = true;
    }
  }
  return mComplete;
}

} // Internal
} // Dali
