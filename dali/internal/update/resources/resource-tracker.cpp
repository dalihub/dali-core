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
#include <dali/internal/update/resources/resource-tracker.h>

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

#if defined(DEBUG_ENABLED)
Debug::Filter* gResourceTrackerLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_RESOURCE_TRACKER");
#endif

ResourceTracker::ResourceTracker()
: mComplete(false)
{
}

ResourceTracker::~ResourceTracker()
{
}

void ResourceTracker::Initialize()
{
}

void ResourceTracker::OnDestroy()
{
}

void ResourceTracker::Reset()
{
  TRACKER_LOG(Debug::General);
  mComplete = false;
  DoReset();
}

void ResourceTracker::SetComplete()
{
  TRACKER_LOG(Debug::General);
  DoSetComplete();
}

bool ResourceTracker::IsComplete()
{
  return DoIsComplete();
}

void ResourceTracker::DoReset()
{
}

void ResourceTracker::DoSetComplete( )
{
  // Allow derived types to override complete behaviour - so do here
  // rather than in SetComplete
  mComplete = true;
}

bool ResourceTracker::DoIsComplete()
{
  return mComplete;
}


} // Internal
} // Dali
