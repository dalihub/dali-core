#ifndef __DALI_INTERNAL_SCENE_GRAPH_RENDER_TASK_DEBUG_H__
#define __DALI_INTERNAL_SCENE_GRAPH_RENDER_TASK_DEBUG_H__

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

#if defined(DEBUG_ENABLED)
Debug::Filter* gRenderTaskLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_SCENE_GRAPH_RENDER_TASK");

#define STATE_STRING(state)                                     \
  (state==RENDER_CONTINUOUSLY?"RenderContinuously":              \
    (state==RENDER_ONCE_WAITING_FOR_RESOURCES?"RenderOnceWaiting":  \
    (state==RENDERED_ONCE?"RenderedOnce":                        \
    (state==RENDERED_ONCE_AND_NOTIFIED?"RenderedOnceAndNotified":  \
     "Unknown"))))

#define TASK_LOG(level)                                              \
  DALI_LOG_INFO(gRenderTaskLogFilter, level, "SceneGraph::RenderTask::%s: this:%p\n", __FUNCTION__, this)
#define TASK_LOG_FMT(level, format, ...) \
  DALI_LOG_INFO(gRenderTaskLogFilter, level, "SceneGraph::RenderTask::%s: this:%p " format, __FUNCTION__, this, ## __VA_ARGS__)
#else

#define TASK_LOG(level)
#define TASK_LOG_FMT(level, format, ...)
#endif

#endif // __DALI_INTERNAL_SCENE_GRAPH_RENDER_TASK_DEBUG_H__
