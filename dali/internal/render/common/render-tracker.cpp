/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/internal/render/common/render-tracker.h>

// INTERNAL INCLUDES
#include <dali/integration-api/gl-sync-abstraction.h>
#include <dali/internal/render/common/render-tracker-debug.h>

// EXTERNAL INCLUDES

namespace Dali
{
namespace Internal
{
namespace Render
{
RenderTracker::RenderTracker()
: mGlSyncAbstraction(nullptr),
  mSyncObject(nullptr),
  mSyncTrigger(0)
{
  TRACKER_LOG(Debug::Verbose);
}

RenderTracker::~RenderTracker()
{
  TRACKER_LOG(Debug::Verbose);
  if(mSyncObject)
  {
    mGlSyncAbstraction->DestroySyncObject(mSyncObject);
    mSyncObject = nullptr;
  }
}

void RenderTracker::CreateSyncObject(Integration::GlSyncAbstraction& glSyncAbstraction)
{
  mGlSyncAbstraction = &glSyncAbstraction;
  TRACKER_LOG(Debug::General);

  // Destroy any previous sync object
  if(mSyncObject)
  {
    mGlSyncAbstraction->DestroySyncObject(mSyncObject);
    mSyncObject = nullptr;
  }
  ResetSyncFlag();
  mSyncObject = mGlSyncAbstraction->CreateSyncObject();
}

void RenderTracker::PollSyncObject()
{
  if(mSyncObject && mSyncObject->IsSynced())
  {
    SetSyncFlag();
    mGlSyncAbstraction->DestroySyncObject(mSyncObject);
    mSyncObject = nullptr;

    TRACKER_LOG_FMT(Debug::General, " Synced\n");
    return;
  }
  TRACKER_LOG_FMT(Debug::General, " Not Synced\n");
}

bool RenderTracker::IsSynced()
{
  int x = __sync_val_compare_and_swap(&mSyncTrigger, 0xFF, 0x0);

  TRACKER_LOG_FMT(Debug::General, " = %s\n", x != 0 ? "T" : "F");
  return x != 0;
}

void RenderTracker::ResetSyncFlag()
{
  TRACKER_LOG(Debug::General);
  (void)__sync_lock_test_and_set(&mSyncTrigger, 0x0);
}

void RenderTracker::SetSyncFlag()
{
  (void)__sync_lock_test_and_set(&mSyncTrigger, 0xFF);
}

} // namespace Render

} // namespace Internal

} // namespace Dali
