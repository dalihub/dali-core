#ifndef DALI_INTERNAL_SCENE_GRAPH_RENDER_TRACKER_DEBUG_H
#define DALI_INTERNAL_SCENE_GRAPH_RENDER_TRACKER_DEBUG_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Internal
{
namespace Render
{
#if defined(DEBUG_ENABLED)

Debug::Filter* gRenderTrackerLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_RENDER_TRACKER");

#define TRACKER_LOG(level) \
  DALI_LOG_INFO(gRenderTrackerLogFilter, level, "RenderTracker::%s: this:%p\n", __FUNCTION__, this)

#define TRACKER_LOG_FMT(level, format, ...) \
  DALI_LOG_INFO(gRenderTrackerLogFilter, level, "RenderTracker::%s: this:%p " format, __FUNCTION__, this, ##__VA_ARGS__)
#else
#define TRACKER_LOG(level)
#define TRACKER_LOG_FMT(level, format, ...)
#endif

} // namespace Render

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_RENDER_TRACKER_DEBUG_H
