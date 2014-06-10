#ifndef __DALI_INTERNAL_RESOURCE_TRACKER_DEBUG_H__
#define __DALI_INTERNAL_RESOURCE_TRACKER_DEBUG_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Internal
{

#if defined(DEBUG_ENABLED)
extern Debug::Filter* gResourceTrackerLogFilter;

#define TRACKER_LOG(level)                                              \
  DALI_LOG_INFO(gResourceTrackerLogFilter, level, "ResourceTracker::%s: this:%p\n", __FUNCTION__, this)
#define TRACKER_LOG_FMT(level, format, args...) \
  DALI_LOG_INFO(gResourceTrackerLogFilter, level, "ResourceTracker::%s: this:%p " format, __FUNCTION__, this, ## args)

#else // DEBUG_ENABLED

#define TRACKER_LOG(level)
#define TRACKER_LOG_FMT(level, format, args...)

#endif

} // Internal
} // Dali


#endif // __DALI_INTERNAL_RESOURCE_TRACKER_H__
